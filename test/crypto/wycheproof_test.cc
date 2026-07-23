#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <functional>  // std::function
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
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

// The vectors carry the private key as raw PKCS#8, so it is base64 wrapped into
// the PEM document that the key parser consumes
auto to_pkcs8_pem(const std::string_view der) -> std::string {
  const auto encoded{sourcemeta::core::base64_encode(der)};
  std::string pem{"-----BEGIN PRIVATE KEY-----\n"};
  for (std::size_t offset{0}; offset < encoded.size(); offset += 64) {
    pem.append(encoded.substr(offset, 64));
    pem.push_back('\n');
  }

  pem.append("-----END PRIVATE KEY-----\n");
  return pem;
}

auto sign_pkcs1(const sourcemeta::core::SignatureHashFunction hash,
                const std::string_view pkcs8_pem,
                const std::string_view message) -> std::optional<std::string> {
  const auto key{sourcemeta::core::make_private_key(pkcs8_pem)};
  if (!key.has_value()) {
    return std::nullopt;
  }

  return sourcemeta::core::rsassa_pkcs1_v15_sign(key.value(), hash, message);
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

// The RSASSA-PKCS1-v1_5 generation vectors carry a private key and the exact
// signature it must produce, so signing is checked against a fixed known answer
auto register_rsa_sign_tests(const std::filesystem::path &path,
                             const std::string &suite_name) -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    const auto hash{to_signature_hash_function(group.at("sha").to_string())};
    // Only the SHA-2 family is supported, and a small public exponent key is
    // not loadable on every backend, so both are skipped
    if (!hash.has_value() ||
        group.at("privateKey").at("publicExponent").to_string() != "010001") {
      continue;
    }

    const auto hash_function{hash.value()};
    const std::string private_key_hex{group.at("privateKeyPkcs8").to_string()};
    for (const auto &test : group.at("tests").as_array()) {
      const std::string message_hex{test.at("msg").to_string()};
      const std::string signature_hex{test.at("sig").to_string()};
      const auto identifier{test.at("tcId").to_integer()};
      register_case(
          suite_name, stem + "_tc" + std::to_string(identifier), [=]() {
            const auto der{sourcemeta::core::hex_to_bytes(private_key_hex)};
            const auto message{sourcemeta::core::hex_to_bytes(message_hex)};
            std::string signature_hex_got;
            if (der.has_value() && message.has_value()) {
              const auto signature{sign_pkcs1(
                  hash_function, to_pkcs8_pem(der.value()), message.value())};
              if (signature.has_value()) {
                signature_hex_got =
                    sourcemeta::core::bytes_to_hex(signature.value());
              }
            }

            EXPECT_EQ(signature_hex_got, signature_hex);
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

// The MAC functions are overloaded, so the registrars name the variant that
// returns a hex string through this alias
using MacFunction = auto (*)(const std::string_view, const std::string_view)
    -> std::string;

// Some MAC functions also expose a variant that consumes the message as a
// sequence of parts and returns raw digest bytes. Where available, the
// registrar additionally runs the vectors through it with the message split
// in half, to exercise the streaming code paths
using MultiPartMacFunction = auto (*)(const std::string_view,
                                      std::span<const std::string_view>)
    -> std::array<std::uint8_t, 32>;

auto digest_to_hex(const std::array<std::uint8_t, 32> &digest) -> std::string {
  return sourcemeta::core::bytes_to_hex(
      {reinterpret_cast<const char *>(digest.data()), digest.size()});
}

auto split_in_halves(const std::string_view message)
    -> std::array<std::string_view, 2> {
  const auto half{message.size() / 2};
  return {{message.substr(0, half), message.substr(half)}};
}

auto register_hmac_tests(const std::filesystem::path &path,
                         const std::string &suite_name,
                         const MacFunction function,
                         const MultiPartMacFunction multi_part_function)
    -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    // The tag is the leading bytes of the digest, truncated to the group tag
    // size (RFC 2104 Section 5). The size arrives in bits and each hex
    // character encodes four of them
    const auto tag_characters{
        static_cast<std::size_t>(group.at("tagSize").to_integer()) / 4};
    for (const auto &test : group.at("tests").as_array()) {
      const auto expected{test.at("result").to_string() == "valid"};
      const std::string key_hex{test.at("key").to_string()};
      const std::string message_hex{test.at("msg").to_string()};
      const std::string tag_hex{test.at("tag").to_string()};
      const auto identifier{test.at("tcId").to_integer()};
      register_case(
          suite_name, stem + "_tc" + std::to_string(identifier), [=]() {
            const auto key{sourcemeta::core::hex_to_bytes(key_hex)};
            const auto message{sourcemeta::core::hex_to_bytes(message_hex)};
            auto got{false};
            if (key.has_value() && message.has_value()) {
              const auto computed{function(key.value(), message.value())};
              got = computed.substr(0, tag_characters) == tag_hex;
            }

            EXPECT_EQ(got, expected);

            if (multi_part_function != nullptr && key.has_value() &&
                message.has_value()) {
              const auto parts{split_in_halves(message.value())};
              const auto computed{
                  digest_to_hex(multi_part_function(key.value(), parts))};
              EXPECT_EQ(computed.substr(0, tag_characters) == tag_hex,
                        expected);
            }
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

// A valid vector must decrypt to exactly the plaintext and reproduce exactly
// under encryption. An invalid vector must be rejected outright, never
// decrypted to some other value, so that an authentication failure cannot be
// masked by a wrong plaintext
auto check_aes_gcm(const std::string_view key, const std::string_view iv,
                   const std::string_view associated_data,
                   const std::string_view ciphertext,
                   const std::string_view tag, const std::string_view plaintext,
                   const bool expected) -> void {
  const auto opened{sourcemeta::core::aes_gcm_decrypt(key, iv, associated_data,
                                                      ciphertext, tag)};
  if (expected) {
    EXPECT_TRUE(opened.has_value());
    EXPECT_EQ(opened.value(), plaintext);
    const auto sealed{
        sourcemeta::core::aes_gcm_encrypt(key, iv, associated_data, plaintext)};
    EXPECT_TRUE(sealed.has_value());
    EXPECT_EQ(sealed.value().ciphertext(), ciphertext);
    EXPECT_EQ(sealed.value().tag(), tag);
  } else {
    EXPECT_FALSE(opened.has_value());
  }
}

auto register_aes_gcm_tests(const std::filesystem::path &path,
                            const std::string &suite_name) -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    // The primitive fixes a 96-bit initialization vector and a 128-bit tag over
    // 128, 192, and 256-bit keys, and authenticates associated data
    const auto key_size{group.at("keySize").to_integer()};
    if ((key_size != 128 && key_size != 192 && key_size != 256) ||
        group.at("ivSize").to_integer() != 96 ||
        group.at("tagSize").to_integer() != 128) {
      continue;
    }

    for (const auto &test : group.at("tests").as_array()) {
      const std::string result{test.at("result").to_string()};
      if (result == "acceptable") {
        continue;
      }

      const auto key{
          sourcemeta::core::hex_to_bytes(test.at("key").to_string())};
      const auto iv{sourcemeta::core::hex_to_bytes(test.at("iv").to_string())};
      const auto associated_data{
          sourcemeta::core::hex_to_bytes(test.at("aad").to_string())};
      const auto message{
          sourcemeta::core::hex_to_bytes(test.at("msg").to_string())};
      const auto ciphertext{
          sourcemeta::core::hex_to_bytes(test.at("ct").to_string())};
      const auto tag{
          sourcemeta::core::hex_to_bytes(test.at("tag").to_string())};
      if (!key.has_value() || !iv.has_value() || !associated_data.has_value() ||
          !message.has_value() || !ciphertext.has_value() || !tag.has_value()) {
        continue;
      }

      register_case(suite_name,
                    stem + "_tc" + std::to_string(test.at("tcId").to_integer()),
                    [key = key.value(), iv = iv.value(),
                     associated_data = associated_data.value(),
                     ciphertext = ciphertext.value(), tag = tag.value(),
                     plaintext = message.value(),
                     expected = (result == "valid")]() {
                      check_aes_gcm(key, iv, associated_data, ciphertext, tag,
                                    plaintext, expected);
                    });
    }
  }
}

// A valid vector must decrypt to exactly the plaintext and reproduce exactly
// under encryption. An invalid vector must be rejected outright, never
// decrypted to some other value, so that an authentication failure cannot be
// masked by a wrong plaintext
auto check_aes_cbc_hmac(const std::string_view key, const std::string_view iv,
                        const std::string_view associated_data,
                        const std::string_view ciphertext,
                        const std::string_view tag,
                        const std::string_view plaintext, const bool expected)
    -> void {
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      key, iv, associated_data, ciphertext, tag)};
  if (expected) {
    EXPECT_TRUE(opened.has_value());
    EXPECT_EQ(opened.value(), plaintext);
    const auto sealed{sourcemeta::core::aes_cbc_hmac_encrypt(
        key, iv, associated_data, plaintext)};
    EXPECT_TRUE(sealed.has_value());
    EXPECT_EQ(sealed.value().ciphertext(), ciphertext);
    EXPECT_EQ(sealed.value().tag(), tag);
  } else {
    EXPECT_FALSE(opened.has_value());
  }
}

auto register_aes_cbc_hmac_tests(const std::filesystem::path &path,
                                 const std::string &suite_name) -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    // The variants use a 256, 384, or 512-bit key and a 128-bit initialization
    // vector, and authenticate associated data
    const auto key_size{group.at("keySize").to_integer()};
    if ((key_size != 256 && key_size != 384 && key_size != 512) ||
        group.at("ivSize").to_integer() != 128) {
      continue;
    }

    for (const auto &test : group.at("tests").as_array()) {
      const std::string result{test.at("result").to_string()};
      if (result == "acceptable") {
        continue;
      }

      const auto key{
          sourcemeta::core::hex_to_bytes(test.at("key").to_string())};
      const auto iv{sourcemeta::core::hex_to_bytes(test.at("iv").to_string())};
      const auto associated_data{
          sourcemeta::core::hex_to_bytes(test.at("aad").to_string())};
      const auto message{
          sourcemeta::core::hex_to_bytes(test.at("msg").to_string())};
      const auto ciphertext{
          sourcemeta::core::hex_to_bytes(test.at("ct").to_string())};
      const auto tag{
          sourcemeta::core::hex_to_bytes(test.at("tag").to_string())};
      if (!key.has_value() || !iv.has_value() || !associated_data.has_value() ||
          !message.has_value() || !ciphertext.has_value() || !tag.has_value()) {
        continue;
      }

      register_case(suite_name,
                    stem + "_tc" + std::to_string(test.at("tcId").to_integer()),
                    [key = key.value(), iv = iv.value(),
                     associated_data = associated_data.value(),
                     ciphertext = ciphertext.value(), tag = tag.value(),
                     plaintext = message.value(),
                     expected = (result == "valid")]() {
                      check_aes_cbc_hmac(key, iv, associated_data, ciphertext,
                                         tag, plaintext, expected);
                    });
    }
  }
}

// A valid vector must unwrap to exactly the key and rewrap to exactly the
// wrapping. An invalid vector must be rejected outright, never unwrapped to
// some other value, so that a failed integrity check cannot be masked
auto check_aes_kw(const std::string_view key_encryption_key,
                  const std::string_view key, const std::string_view wrapped,
                  const bool expected) -> void {
  const auto unwrapped{
      sourcemeta::core::aes_key_unwrap(key_encryption_key, wrapped)};
  if (expected) {
    EXPECT_TRUE(unwrapped.has_value());
    EXPECT_EQ(unwrapped.value(), key);
    const auto rewrapped{
        sourcemeta::core::aes_key_wrap(key_encryption_key, key)};
    EXPECT_TRUE(rewrapped.has_value());
    EXPECT_EQ(rewrapped.value(), wrapped);
  } else {
    EXPECT_FALSE(unwrapped.has_value());
  }
}

auto register_aes_kw_tests(const std::filesystem::path &path,
                           const std::string &suite_name) -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    const auto key_size{group.at("keySize").to_integer()};
    if (key_size != 128 && key_size != 192 && key_size != 256) {
      continue;
    }

    for (const auto &test : group.at("tests").as_array()) {
      const std::string result{test.at("result").to_string()};
      if (result == "acceptable") {
        continue;
      }

      const auto key_encryption_key{
          sourcemeta::core::hex_to_bytes(test.at("key").to_string())};
      const auto key{
          sourcemeta::core::hex_to_bytes(test.at("msg").to_string())};
      const auto wrapped{
          sourcemeta::core::hex_to_bytes(test.at("ct").to_string())};
      if (!key_encryption_key.has_value() || !key.has_value() ||
          !wrapped.has_value()) {
        continue;
      }

      register_case(suite_name,
                    stem + "_tc" + std::to_string(test.at("tcId").to_integer()),
                    [key_encryption_key = key_encryption_key.value(),
                     key = key.value(), wrapped = wrapped.value(),
                     expected = (result == "valid")]() {
                      check_aes_kw(key_encryption_key, key, wrapped, expected);
                    });
    }
  }
}

auto to_oaep_hash(const std::string_view name)
    -> std::optional<sourcemeta::core::RSAOAEPHash> {
  if (name == "SHA-1") {
    return sourcemeta::core::RSAOAEPHash::SHA1;
  } else if (name == "SHA-256") {
    return sourcemeta::core::RSAOAEPHash::SHA256;
  } else {
    return std::nullopt;
  }
}

// A valid vector must decrypt to exactly the plaintext. An invalid vector must
// be rejected outright, never decrypted to some other value, so that a failed
// padding check cannot be masked
auto check_rsa_oaep(const std::string_view pem,
                    const sourcemeta::core::RSAOAEPHash hash,
                    const std::string_view ciphertext,
                    const std::string_view plaintext, const bool expected)
    -> void {
  const auto private_key{sourcemeta::core::make_private_key(pem)};
  EXPECT_TRUE(private_key.has_value());
  if (!private_key.has_value()) {
    return;
  }

  const auto decrypted{sourcemeta::core::rsa_oaep_decrypt(private_key.value(),
                                                          hash, ciphertext)};
  if (expected) {
    EXPECT_TRUE(decrypted.has_value());
    EXPECT_EQ(decrypted.value(), plaintext);
  } else {
    EXPECT_FALSE(decrypted.has_value());
  }
}

auto register_rsa_oaep_tests(const std::filesystem::path &path,
                             const std::string &suite_name) -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    const auto hash{to_oaep_hash(group.at("sha").to_string())};
    // The supported algorithms use the same hash for the label and the mask,
    // and an empty label
    if (!hash.has_value() ||
        group.at("sha").to_string() != group.at("mgfSha").to_string()) {
      continue;
    }

    const auto key{sourcemeta::core::hex_to_bytes(
        group.at("privateKeyPkcs8").to_string())};
    if (!key.has_value()) {
      continue;
    }

    const auto pem{to_pkcs8_pem(key.value())};
    for (const auto &test : group.at("tests").as_array()) {
      const std::string result{test.at("result").to_string()};
      if (result == "acceptable" || !test.at("label").to_string().empty()) {
        continue;
      }

      const auto message{
          sourcemeta::core::hex_to_bytes(test.at("msg").to_string())};
      const auto ciphertext{
          sourcemeta::core::hex_to_bytes(test.at("ct").to_string())};
      if (!message.has_value() || !ciphertext.has_value()) {
        continue;
      }

      register_case(
          suite_name,
          stem + "_tc" + std::to_string(test.at("tcId").to_integer()),
          [pem, hash = hash.value(), ciphertext = ciphertext.value(),
           plaintext = message.value(), expected = (result == "valid")]() {
            check_rsa_oaep(pem, hash, ciphertext, plaintext, expected);
          });
    }
  }
}

// The suite gives each private key only as a scalar, but the platform backends
// build a private key from the scalar together with its public point, so the
// point of the primary key of each curve is precomputed here
struct EcdhCurve {
  sourcemeta::core::EllipticCurve curve;
  std::size_t field_bytes;
  std::string_view private_scalar;
  std::string_view coordinate_x;
  std::string_view coordinate_y;
};

auto to_ecdh_curve(const std::string_view name) -> std::optional<EcdhCurve> {
  if (name == "secp256r1") {
    return EcdhCurve{
        sourcemeta::core::EllipticCurve::P256, 32,
        "00809c461d8b39163537ff8f5ef5b977e4cdb980e70e38a7ee0b37cc876729e9ff",
        "0468ae7706221e5990f7484d34fbec5a99050179a6c11817bbed4aed962998ff",
        "b5228d89a1b448f12332376c8c7f080763532a055e07f14a5de0dc30104579e1"};
  } else if (name == "secp384r1") {
    return EcdhCurve{sourcemeta::core::EllipticCurve::P384, 48,
                     "00c1781d86cac2c052b7e4f48cef415c5c133052f4e504397e75e4d7"
                     "cd0ca149da0b4988b8a6ded5ceae4b580691376187",
                     "29eb9c63a12322cd99de0e8e3c11e6c8a486b1008181be4c825f38b3"
                     "71e6f03e2926cd60e9a9ccd42e1aa8799d2ef3a7",
                     "9d045f3a3df974743268be650fbaf68c7045a483093ce2e289903097"
                     "396f040058faacb723eb493b5b631bc365c5fc49"};
  } else if (name == "secp521r1") {
    return EcdhCurve{
        sourcemeta::core::EllipticCurve::P521, 66,
        "01781d86cac2c052b7e4f48cef415c5c1319e07db70db92a497c2ac764e9509ac0b073"
        "22801f5ae1f28c9d7db71f79e5f51bf646790af988d62339a6d1543192e327",
        "0026e4bc8e3dfcb2898f9fb71eb373199ecd58b13b66c221110819bd05159eb5e3b8d1"
        "376f7d5151db140b3cdae698bca9683f3d041164fe0e3dba0d2f02d6643adb",
        "010a3f1fcb6dbccc773d143d454347145a9bec498e7a5e2d412095b9350c58d8e9411e"
        "a85afe4c56053ad35d8be4230f5e2718365f43dbd2ed44b096a90bfc6227b6"};
  } else {
    return std::nullopt;
  }
}

// A valid vector must derive exactly the given shared secret. An invalid vector
// carries a peer point off the curve or otherwise malformed, so no shared
// secret can be produced
auto check_ecdh(const EcdhCurve &info, const std::string_view peer_point,
                const std::string_view shared, const bool expected) -> void {
  const auto scalar{sourcemeta::core::hex_to_bytes(info.private_scalar)};
  const auto coordinate_x{sourcemeta::core::hex_to_bytes(info.coordinate_x)};
  const auto coordinate_y{sourcemeta::core::hex_to_bytes(info.coordinate_y)};
  EXPECT_TRUE(scalar.has_value());
  EXPECT_TRUE(coordinate_x.has_value());
  EXPECT_TRUE(coordinate_y.has_value());
  if (!scalar.has_value() || !coordinate_x.has_value() ||
      !coordinate_y.has_value()) {
    return;
  }

  const auto private_key{sourcemeta::core::make_ec_private_key(
      info.curve, scalar.value(), coordinate_x.value(), coordinate_y.value())};
  EXPECT_TRUE(private_key.has_value());
  if (!private_key.has_value()) {
    return;
  }

  // The peer point is the uncompressed X9.63 encoding, splitting into its two
  // coordinates after the leading tag byte
  std::optional<sourcemeta::core::PublicKey> public_key;
  if (peer_point.size() == (2 * info.field_bytes) + 1 &&
      static_cast<unsigned char>(peer_point.front()) == 0x04) {
    public_key = sourcemeta::core::make_ec_public_key(
        info.curve, peer_point.substr(1, info.field_bytes),
        peer_point.substr(1 + info.field_bytes));
  }

  if (expected) {
    EXPECT_TRUE(public_key.has_value());
    if (!public_key.has_value()) {
      return;
    }

    const auto secret{
        sourcemeta::core::ecdh_derive(private_key.value(), public_key.value())};
    EXPECT_TRUE(secret.has_value());
    if (!secret.has_value()) {
      return;
    }

    EXPECT_EQ(secret.value(),
              sourcemeta::core::pad_left(shared, info.field_bytes, '\x00'));
  } else {
    const auto secret{public_key.has_value()
                          ? sourcemeta::core::ecdh_derive(private_key.value(),
                                                          public_key.value())
                          : std::nullopt};
    EXPECT_FALSE(secret.has_value());
  }
}

auto register_ecdh_tests(const std::filesystem::path &path,
                         const std::string &suite_name) -> void {
  const auto document{load(path)};
  const auto stem{path.stem().string()};
  for (const auto &group : document.at("testGroups").as_array()) {
    const auto info{to_ecdh_curve(group.at("curve").to_string())};
    if (!info.has_value()) {
      continue;
    }

    for (const auto &test : group.at("tests").as_array()) {
      const std::string result{test.at("result").to_string()};
      // The suite provides only the scalar of each private key, so a valid
      // vector is checked only for the one key whose point is known here. Every
      // invalid vector is still checked, since its bad peer point is rejected
      // before the private key matters
      if (result == "acceptable" ||
          (result == "valid" &&
           test.at("private").to_string() != info.value().private_scalar)) {
        continue;
      }

      const auto peer{
          sourcemeta::core::hex_to_bytes(test.at("public").to_string())};
      if (!peer.has_value()) {
        continue;
      }

      const bool expected{result == "valid"};
      // The shared secret is only compared for a valid vector, so an invalid
      // vector keeps its coverage even when it carries no usable shared field
      std::string shared;
      if (expected) {
        const auto decoded{
            sourcemeta::core::hex_to_bytes(test.at("shared").to_string())};
        if (!decoded.has_value()) {
          continue;
        }

        shared = decoded.value();
      }

      register_case(suite_name,
                    stem + "_tc" + std::to_string(test.at("tcId").to_integer()),
                    [info = info.value(), peer = peer.value(), shared,
                     expected]() { check_ecdh(info, peer, shared, expected); });
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

  constexpr std::array<std::string_view, 5> rsa_pkcs1_sign{
      {"rsa_pkcs1_1024_sig_gen_test", "rsa_pkcs1_1536_sig_gen_test",
       "rsa_pkcs1_2048_sig_gen_test", "rsa_pkcs1_3072_sig_gen_test",
       "rsa_pkcs1_4096_sig_gen_test"}};
  for (const auto name : rsa_pkcs1_sign) {
    register_rsa_sign_tests(vectors / (std::string{name} + ".json"),
                            "Wycheproof_RSA_PKCS1_Sign");
  }

  constexpr std::array<std::string_view, 5> ecdsa{
      {"ecdsa_secp256r1_sha256_p1363_test", "ecdsa_secp256r1_sha512_p1363_test",
       "ecdsa_secp384r1_sha384_p1363_test", "ecdsa_secp384r1_sha512_p1363_test",
       "ecdsa_secp521r1_sha512_p1363_test"}};
  for (const auto name : ecdsa) {
    register_ecdsa_tests(vectors / (std::string{name} + ".json"),
                         "Wycheproof_ECDSA");
  }

  register_hmac_tests(
      vectors / "hmac_sha256_test.json", "Wycheproof_HMAC_SHA256",
      sourcemeta::core::hmac_sha256,
      static_cast<MultiPartMacFunction>(sourcemeta::core::hmac_sha256_digest));
  register_hmac_tests(vectors / "hmac_sha384_test.json",
                      "Wycheproof_HMAC_SHA384", sourcemeta::core::hmac_sha384,
                      nullptr);
  register_hmac_tests(vectors / "hmac_sha512_test.json",
                      "Wycheproof_HMAC_SHA512", sourcemeta::core::hmac_sha512,
                      nullptr);

  register_eddsa_tests(vectors / "ed25519_test.json", "Wycheproof_EdDSA",
                       sourcemeta::core::EdwardsCurve::Ed25519);
  register_eddsa_tests(vectors / "ed448_test.json", "Wycheproof_EdDSA",
                       sourcemeta::core::EdwardsCurve::Ed448);

  register_aes_gcm_tests(vectors / "aes_gcm_test.json", "Wycheproof_AES_GCM");
  register_aes_cbc_hmac_tests(vectors / "a128cbc_hs256_test.json",
                              "Wycheproof_A128CBC_HS256");
  register_aes_cbc_hmac_tests(vectors / "a192cbc_hs384_test.json",
                              "Wycheproof_A192CBC_HS384");
  register_aes_cbc_hmac_tests(vectors / "a256cbc_hs512_test.json",
                              "Wycheproof_A256CBC_HS512");
  register_aes_kw_tests(vectors / "aes_wrap_test.json", "Wycheproof_AES_KW");
  register_rsa_oaep_tests(vectors / "rsa_oaep_2048_sha1_mgf1sha1_test.json",
                          "Wycheproof_RSA_OAEP_SHA1");
  register_rsa_oaep_tests(vectors / "rsa_oaep_2048_sha256_mgf1sha256_test.json",
                          "Wycheproof_RSA_OAEP_2048_SHA256");
  register_rsa_oaep_tests(vectors / "rsa_oaep_3072_sha256_mgf1sha256_test.json",
                          "Wycheproof_RSA_OAEP_3072_SHA256");
  register_rsa_oaep_tests(vectors / "rsa_oaep_4096_sha256_mgf1sha256_test.json",
                          "Wycheproof_RSA_OAEP_4096_SHA256");

  register_ecdh_tests(vectors / "ecdh_secp256r1_ecpoint_test.json",
                      "Wycheproof_ECDH_P256");
  register_ecdh_tests(vectors / "ecdh_secp384r1_ecpoint_test.json",
                      "Wycheproof_ECDH_P384");
  register_ecdh_tests(vectors / "ecdh_secp521r1_ecpoint_test.json",
                      "Wycheproof_ECDH_P521");

  return sourcemeta::core::test_run(argc, argv);
}
