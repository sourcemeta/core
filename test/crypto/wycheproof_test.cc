#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <functional>  // std::function
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {
auto to_signature_hash_function(const std::string_view name)
    -> std::optional<sourcemeta::core::SignatureHashFunction> {
  if (name == "SHA-256") {
    return sourcemeta::core::SignatureHashFunction::SHA256;
  } else if (name == "SHA-384") {
    return sourcemeta::core::SignatureHashFunction::SHA384;
  } else if (name == "SHA-512") {
    return sourcemeta::core::SignatureHashFunction::SHA512;
  } else {
    return std::nullopt;
  }
}

struct CurveParameters {
  sourcemeta::core::EllipticCurve curve;
  std::size_t coordinate_bytes;
};

auto to_curve(const std::string_view name) -> std::optional<CurveParameters> {
  if (name == "secp256r1") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P256, 32};
  } else if (name == "secp384r1") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P384, 48};
  } else if (name == "secp521r1") {
    return CurveParameters{sourcemeta::core::EllipticCurve::P521, 66};
  } else {
    return std::nullopt;
  }
}

// Each vector reduces to a closure that asserts on data captured at
// registration time, so a single runner carrying that closure stands in for
// every mechanism, matching the pyca-cryptography harness
auto run_wycheproof_test_case(const std::function<void()> &assertion) -> void {
  assertion();
}

auto register_case(const std::string &suite_name, const std::string &test_name,
                   std::function<void()> assertion) -> void {
  sourcemeta::core::test_register(suite_name + "." + test_name,
                                  [assertion = std::move(assertion)]() -> void {
                                    run_wycheproof_test_case(assertion);
                                  });
}

auto load(const std::filesystem::path &path) -> sourcemeta::core::JSON {
  auto stream{sourcemeta::core::read_file(path)};
  return sourcemeta::core::parse_json(stream);
}

auto verify_pkcs1(const sourcemeta::core::SignatureHashFunction hash,
                  const std::string_view modulus,
                  const std::string_view exponent,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_rsa_public_key(modulus, exponent)};
  return key.has_value() && sourcemeta::core::rsassa_pkcs1_v15_verify(
                                key.value(), hash, message, signature);
}

auto verify_pss(const sourcemeta::core::SignatureHashFunction hash,
                const std::string_view modulus, const std::string_view exponent,
                const std::string_view message,
                const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_rsa_public_key(modulus, exponent)};
  return key.has_value() && sourcemeta::core::rsassa_pss_verify(
                                key.value(), hash, message, signature);
}

auto verify_ecdsa(const sourcemeta::core::EllipticCurve curve,
                  const sourcemeta::core::SignatureHashFunction hash,
                  const std::string_view coordinate_x,
                  const std::string_view coordinate_y,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto key{
      sourcemeta::core::make_ec_public_key(curve, coordinate_x, coordinate_y)};
  return key.has_value() &&
         sourcemeta::core::ecdsa_verify(key.value(), hash, message, signature);
}

auto verify_eddsa(const sourcemeta::core::EdwardsCurve curve,
                  const std::string_view public_key,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_eddsa_public_key(curve, public_key)};
  return key.has_value() &&
         sourcemeta::core::eddsa_verify(key.value(), message, signature);
}

using RsaVerify = auto (*)(const sourcemeta::core::SignatureHashFunction,
                           const std::string_view, const std::string_view,
                           const std::string_view, const std::string_view)
    -> bool;

auto register_rsa_tests(const std::filesystem::path &path,
                        const std::string &suite_name, const RsaVerify verify)
    -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    const auto hash{to_signature_hash_function(group.at("sha").to_string())};
    if (!hash.has_value()) {
      continue;
    }

    const auto hash_function{hash.value()};
    const std::string modulus_hex{
        group.at("publicKey").at("modulus").to_string()};
    const std::string exponent_hex{
        group.at("publicKey").at("publicExponent").to_string()};
    for (const auto &test : group.at("tests").as_array()) {
      const std::string result{test.at("result").to_string()};
      if (result == "acceptable") {
        continue;
      }

      const auto expected{result == "valid"};
      const std::string message_hex{test.at("msg").to_string()};
      const std::string signature_hex{test.at("sig").to_string()};
      const auto identifier{test.at("tcId").to_integer()};
      register_case(
          suite_name, stem + "_tc" + std::to_string(identifier), [=]() {
            const auto modulus{sourcemeta::core::hex_to_bytes(modulus_hex)};
            const auto exponent{sourcemeta::core::hex_to_bytes(exponent_hex)};
            const auto message{sourcemeta::core::hex_to_bytes(message_hex)};
            const auto signature{sourcemeta::core::hex_to_bytes(signature_hex)};
            const auto got{
                modulus.has_value() && exponent.has_value() &&
                message.has_value() && signature.has_value() &&
                // The modulus carries an ASN.1 leading zero sign byte, which is
                // stripped so the byte length is not inflated past the key
                // limit
                verify(hash_function,
                       sourcemeta::core::strip_left(modulus.value(), '\x00'),
                       exponent.value(), message.value(), signature.value())};
            EXPECT_EQ(got, expected);
          });
    }
  }
}

auto register_ecdsa_tests(const std::filesystem::path &path,
                          const std::string &suite_name) -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    const auto curve{to_curve(group.at("publicKey").at("curve").to_string())};
    const auto hash{to_signature_hash_function(group.at("sha").to_string())};
    if (!curve.has_value() || !hash.has_value()) {
      continue;
    }

    const auto width{curve.value().coordinate_bytes};
    const auto curve_identifier{curve.value().curve};
    const auto hash_function{hash.value()};
    const std::string point_hex{
        group.at("publicKey").at("uncompressed").to_string()};
    for (const auto &test : group.at("tests").as_array()) {
      const std::string result{test.at("result").to_string()};
      if (result == "acceptable") {
        continue;
      }

      const auto expected{result == "valid"};
      const std::string message_hex{test.at("msg").to_string()};
      const std::string signature_hex{test.at("sig").to_string()};
      const auto identifier{test.at("tcId").to_integer()};
      register_case(
          suite_name, stem + "_tc" + std::to_string(identifier), [=]() {
            const auto point{sourcemeta::core::hex_to_bytes(point_hex)};
            const auto message{sourcemeta::core::hex_to_bytes(message_hex)};
            const auto signature{sourcemeta::core::hex_to_bytes(signature_hex)};
            // The uncompressed point is a 0x04 prefix followed by the two
            // field-width coordinates
            const auto got{
                point.has_value() && point.value().size() == 1 + (2 * width) &&
                message.has_value() && signature.has_value() &&
                verify_ecdsa(
                    curve_identifier, hash_function,
                    std::string_view{point.value()}.substr(1, width),
                    std::string_view{point.value()}.substr(1 + width, width),
                    message.value(), signature.value())};
            EXPECT_EQ(got, expected);
          });
    }
  }
}

auto register_eddsa_tests(const std::filesystem::path &path,
                          const std::string &suite_name,
                          const sourcemeta::core::EdwardsCurve curve) -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    const std::string public_key_hex{
        group.at("publicKey").at("pk").to_string()};
    for (const auto &test : group.at("tests").as_array()) {
      const std::string result{test.at("result").to_string()};
      if (result == "acceptable") {
        continue;
      }

      const auto expected{result == "valid"};
      const std::string message_hex{test.at("msg").to_string()};
      const std::string signature_hex{test.at("sig").to_string()};
      const auto identifier{test.at("tcId").to_integer()};
      register_case(
          suite_name, stem + "_tc" + std::to_string(identifier), [=]() {
            const auto public_key{
                sourcemeta::core::hex_to_bytes(public_key_hex)};
            const auto message{sourcemeta::core::hex_to_bytes(message_hex)};
            const auto signature{sourcemeta::core::hex_to_bytes(signature_hex)};
            const auto got{public_key.has_value() && message.has_value() &&
                           signature.has_value() &&
                           verify_eddsa(curve, public_key.value(),
                                        message.value(), signature.value())};
            EXPECT_EQ(got, expected);
          });
    }
  }
}
} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path base{WYCHEPROOF_PATH};
  const auto vectors{base / "testvectors_v1"};

  constexpr std::array<std::string_view, 9> rsa_pkcs1{
      {"rsa_signature_2048_sha256_test", "rsa_signature_2048_sha384_test",
       "rsa_signature_2048_sha512_test", "rsa_signature_3072_sha256_test",
       "rsa_signature_3072_sha384_test", "rsa_signature_3072_sha512_test",
       "rsa_signature_4096_sha256_test", "rsa_signature_4096_sha384_test",
       "rsa_signature_4096_sha512_test"}};
  for (const auto name : rsa_pkcs1) {
    register_rsa_tests(vectors / (std::string{name} + ".json"),
                       "Wycheproof_RSA_PKCS1", verify_pkcs1);
  }

  constexpr std::array<std::string_view, 6> rsa_pss{
      {"rsa_pss_2048_sha256_mgf1_32_test", "rsa_pss_2048_sha384_mgf1_48_test",
       "rsa_pss_3072_sha256_mgf1_32_test", "rsa_pss_4096_sha256_mgf1_32_test",
       "rsa_pss_4096_sha384_mgf1_48_test", "rsa_pss_4096_sha512_mgf1_64_test"}};
  for (const auto name : rsa_pss) {
    register_rsa_tests(vectors / (std::string{name} + ".json"),
                       "Wycheproof_RSA_PSS", verify_pss);
  }

  constexpr std::array<std::string_view, 5> ecdsa{
      {"ecdsa_secp256r1_sha256_p1363_test", "ecdsa_secp256r1_sha512_p1363_test",
       "ecdsa_secp384r1_sha384_p1363_test", "ecdsa_secp384r1_sha512_p1363_test",
       "ecdsa_secp521r1_sha512_p1363_test"}};
  for (const auto name : ecdsa) {
    register_ecdsa_tests(vectors / (std::string{name} + ".json"),
                         "Wycheproof_ECDSA");
  }

  register_eddsa_tests(vectors / "ed25519_test.json", "Wycheproof_EdDSA",
                       sourcemeta::core::EdwardsCurve::Ed25519);
  register_eddsa_tests(vectors / "ed448_test.json", "Wycheproof_EdDSA",
                       sourcemeta::core::EdwardsCurve::Ed448);

  return sourcemeta::core::test_run(argc, argv);
}
