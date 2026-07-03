#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

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

auto pem_wrap(const std::string_view der) -> std::string {
  const auto encoded{sourcemeta::core::base64_encode(der)};
  std::string pem{"-----BEGIN PRIVATE KEY-----\n"};
  for (std::size_t offset{0}; offset < encoded.size(); offset += 64) {
    pem.append(encoded.substr(offset, 64));
    pem.push_back('\n');
  }

  pem.append("-----END PRIVATE KEY-----\n");
  return pem;
}

auto load(const std::filesystem::path &path) -> sourcemeta::core::JSON {
  auto stream{sourcemeta::core::read_file(path)};
  return sourcemeta::core::parse_json(stream);
}

const std::filesystem::path wycheproof{WYCHEPROOF_PATH};
const std::filesystem::path pyca{PYCA_CRYPTOGRAPHY_PATH};

} // namespace

TEST(ed25519_sign_round_trips_through_verify) {
  auto stream{sourcemeta::core::read_file(pyca / "asymmetric" / "Ed25519" /
                                          "sign.input")};
  std::string line;
  std::size_t count{0};
  while (std::getline(stream, line) && count < 128) {
    const auto fields{sourcemeta::core::split(line, ':')};
    if (fields.size() < 4) {
      continue;
    }

    const auto key{sourcemeta::core::make_edwards_private_key(
        sourcemeta::core::EdwardsCurve::Ed25519,
        sourcemeta::core::hex_to_bytes(fields.at(0).substr(0, 64)).value())};
    EXPECT_TRUE(key.has_value());
    if (!key.has_value()) {
      continue;
    }

    EXPECT_TRUE(key.value().type() ==
                sourcemeta::core::PrivateKey::Type::Edwards);
    const std::string message_hex{fields.at(2)};
    const auto message{message_hex.empty()
                           ? std::optional<std::string>{std::string{}}
                           : sourcemeta::core::hex_to_bytes(message_hex)};
    const auto signature{
        sourcemeta::core::eddsa_sign(key.value(), message.value())};
    EXPECT_TRUE(signature.has_value());
    // The Apple backend produces valid but non-deterministic Ed25519
    // signatures, so the signature is round-tripped rather than compared to
    // the fixed known answer
    const auto public_key{sourcemeta::core::make_eddsa_public_key(
        sourcemeta::core::EdwardsCurve::Ed25519,
        sourcemeta::core::hex_to_bytes(fields.at(1)).value())};
    EXPECT_TRUE(public_key.has_value());
    if (signature.has_value()) {
      EXPECT_TRUE(sourcemeta::core::eddsa_verify(
          public_key.value(), message.value(), signature.value()));
    }

    count += 1;
  }

  EXPECT_TRUE(count > 0);
}

TEST(rsa_pkcs1_v15_sign_matches_wycheproof_known_answers) {
  std::size_t verified{0};
  for (const auto *file :
       {"rsa_pkcs1_2048_sig_gen_test.json", "rsa_pkcs1_3072_sig_gen_test.json",
        "rsa_pkcs1_4096_sig_gen_test.json"}) {
    const auto document{load(wycheproof / "testvectors_v1" / file)};
    for (const auto &group : document.at("testGroups").as_array()) {
      const auto hash{to_signature_hash_function(group.at("sha").to_string())};
      if (!hash.has_value()) {
        continue;
      }

      const auto der{sourcemeta::core::hex_to_bytes(
          group.at("privateKeyPkcs8").to_string())};
      EXPECT_TRUE(der.has_value());
      const auto key{sourcemeta::core::make_private_key(pem_wrap(der.value()))};
      // Some backends do not accept every key, such as the Apple Security
      // framework rejecting a small public exponent, so an unsupported key is
      // skipped rather than signed
      if (!key.has_value()) {
        continue;
      }

      EXPECT_TRUE(key.value().type() ==
                  sourcemeta::core::PrivateKey::Type::RSA);
      for (const auto &test : group.at("tests").as_array()) {
        const auto message{
            sourcemeta::core::hex_to_bytes(test.at("msg").to_string())};
        const auto signature{sourcemeta::core::rsassa_pkcs1_v15_sign(
            key.value(), hash.value(), message.value())};
        EXPECT_TRUE(signature.has_value());
        if (signature.has_value()) {
          EXPECT_EQ(sourcemeta::core::bytes_to_hex(signature.value()),
                    test.at("sig").to_string());
          verified += 1;
        }
      }
    }
  }

  EXPECT_TRUE(verified > 0);
}

TEST(rsa_pss_sign_round_trips_through_verify) {
  const auto document{
      load(wycheproof / "testvectors_v1" / "rsa_pkcs1_2048_sig_gen_test.json")};
  const auto &group{document.at("testGroups").at(0)};
  const auto hash{sourcemeta::core::SignatureHashFunction::SHA256};

  const auto der{
      sourcemeta::core::hex_to_bytes(group.at("privateKeyPkcs8").to_string())};
  const auto key{sourcemeta::core::make_private_key(pem_wrap(der.value()))};
  EXPECT_TRUE(key.has_value());

  const auto modulus{sourcemeta::core::hex_to_bytes(
      group.at("privateKey").at("modulus").to_string())};
  const auto exponent{sourcemeta::core::hex_to_bytes(
      group.at("privateKey").at("publicExponent").to_string())};
  const auto public_key{sourcemeta::core::make_rsa_public_key(
      sourcemeta::core::strip_left(modulus.value(), '\x00'), exponent.value())};
  EXPECT_TRUE(public_key.has_value());

  const std::string_view message{"rsa pss signing round trip"};
  const auto signature{
      sourcemeta::core::rsassa_pss_sign(key.value(), hash, message)};
  EXPECT_TRUE(signature.has_value());
  EXPECT_TRUE(sourcemeta::core::rsassa_pss_verify(public_key.value(), hash,
                                                  message, signature.value()));
}

TEST(sign_rejects_wrong_key_type) {
  const auto document{
      load(wycheproof / "testvectors_v1" / "rsa_pkcs1_2048_sig_gen_test.json")};
  const auto &group{document.at("testGroups").at(0)};
  const auto der{
      sourcemeta::core::hex_to_bytes(group.at("privateKeyPkcs8").to_string())};
  const auto key{sourcemeta::core::make_private_key(pem_wrap(der.value()))};
  EXPECT_TRUE(key.has_value());

  // An RSA key cannot produce an ECDSA or EdDSA signature
  EXPECT_FALSE(sourcemeta::core::ecdsa_sign(
                   key.value(), sourcemeta::core::SignatureHashFunction::SHA256,
                   "message")
                   .has_value());
  EXPECT_FALSE(
      sourcemeta::core::eddsa_sign(key.value(), "message").has_value());
}

TEST(ed448_sign_matches_rfc8032_known_answers) {
  auto stream{sourcemeta::core::read_file(pyca / "asymmetric" / "Ed448" /
                                          "rfc8032.txt")};
  std::string line;
  std::string secret;
  std::string public_field;
  std::string message;
  std::string context;
  std::size_t count{0};
  while (std::getline(stream, line)) {
    const auto separator{line.find('=')};
    if (separator == std::string::npos) {
      continue;
    }

    const auto name{
        sourcemeta::core::trim(std::string_view{line}.substr(0, separator))};
    const auto value{
        sourcemeta::core::trim(std::string_view{line}.substr(separator + 1))};
    if (name == "SECRET") {
      secret = value;
      context.clear();
    } else if (name == "MESSAGE") {
      message = value;
    } else if (name == "PUBLIC") {
      public_field = value;
    } else if (name == "CONTEXT") {
      context = value;
    } else if (name == "SIGNATURE") {
      // The signing interface uses an empty context, as JWS requires (RFC 8037
      // Section 3.1), so vectors carrying a context are not reproducible
      if (!context.empty()) {
        continue;
      }

      const auto key{sourcemeta::core::make_edwards_private_key(
          sourcemeta::core::EdwardsCurve::Ed448,
          sourcemeta::core::hex_to_bytes(secret).value())};
      EXPECT_TRUE(key.has_value());
      if (!key.has_value()) {
        continue;
      }

      EXPECT_TRUE(key.value().type() ==
                  sourcemeta::core::PrivateKey::Type::Edwards);
      const auto bytes{message.empty()
                           ? std::optional<std::string>{std::string{}}
                           : sourcemeta::core::hex_to_bytes(message)};
      const auto signature{
          sourcemeta::core::eddsa_sign(key.value(), bytes.value())};
      EXPECT_TRUE(signature.has_value());
      if (signature.has_value()) {
        const auto public_key{sourcemeta::core::make_eddsa_public_key(
            sourcemeta::core::EdwardsCurve::Ed448,
            sourcemeta::core::hex_to_bytes(public_field).value())};
        EXPECT_TRUE(public_key.has_value());
        EXPECT_TRUE(sourcemeta::core::eddsa_verify(
            public_key.value(), bytes.value(), signature.value()));
        EXPECT_EQ(sourcemeta::core::bytes_to_hex(signature.value()), value);
      }

      count += 1;
    }
  }

  EXPECT_TRUE(count > 0);
}

TEST(ecdsa_sign_round_trips_through_verify) {
  auto stream{sourcemeta::core::read_file(pyca / "asymmetric" / "ECDSA" /
                                          "FIPS_186-3" / "SigGen.txt")};
  std::string line;
  std::optional<sourcemeta::core::EllipticCurve> curve;
  std::optional<sourcemeta::core::SignatureHashFunction> hash;
  std::string message;
  std::string scalar;
  std::string x;
  std::size_t count{0};
  while (std::getline(stream, line)) {
    if (line.starts_with("[P-256,SHA-256]")) {
      curve = sourcemeta::core::EllipticCurve::P256;
      hash = sourcemeta::core::SignatureHashFunction::SHA256;
      continue;
    } else if (line.starts_with("[P-384,SHA-384]")) {
      curve = sourcemeta::core::EllipticCurve::P384;
      hash = sourcemeta::core::SignatureHashFunction::SHA384;
      continue;
    } else if (line.starts_with("[P-521,SHA-512]")) {
      curve = sourcemeta::core::EllipticCurve::P521;
      hash = sourcemeta::core::SignatureHashFunction::SHA512;
      continue;
    } else if (line.starts_with("[")) {
      curve = std::nullopt;
      continue;
    }

    const auto separator{line.find('=')};
    if (separator == std::string::npos || !curve.has_value()) {
      continue;
    }

    const auto name{
        sourcemeta::core::trim(std::string_view{line}.substr(0, separator))};
    const auto value{std::string{
        sourcemeta::core::trim(std::string_view{line}.substr(separator + 1))}};
    if (name == "Msg") {
      message = value;
    } else if (name == "d") {
      scalar = value;
    } else if (name == "Qx") {
      x = value;
    } else if (name == "Qy") {
      const auto key{sourcemeta::core::make_ec_private_key(
          curve.value(), sourcemeta::core::hex_to_bytes(scalar, true).value(),
          sourcemeta::core::hex_to_bytes(x, true).value(),
          sourcemeta::core::hex_to_bytes(value, true).value())};
      EXPECT_TRUE(key.has_value());
      if (!key.has_value()) {
        continue;
      }

      EXPECT_TRUE(key.value().type() ==
                  sourcemeta::core::PrivateKey::Type::EllipticCurve);
      const auto public_key{sourcemeta::core::make_ec_public_key(
          curve.value(), sourcemeta::core::hex_to_bytes(x, true).value(),
          sourcemeta::core::hex_to_bytes(value, true).value())};
      EXPECT_TRUE(public_key.has_value());
      const auto bytes{sourcemeta::core::hex_to_bytes(message)};
      const auto signature{sourcemeta::core::ecdsa_sign(
          key.value(), hash.value(), bytes.value())};
      EXPECT_TRUE(signature.has_value());
      if (signature.has_value() && public_key.has_value()) {
        EXPECT_TRUE(sourcemeta::core::ecdsa_verify(public_key.value(),
                                                   hash.value(), bytes.value(),
                                                   signature.value()));
      }

      count += 1;
    }
  }

  EXPECT_TRUE(count > 0);
}

TEST(make_private_key_rejects_garbage) {
  EXPECT_FALSE(sourcemeta::core::make_private_key("not a pem").has_value());
}
