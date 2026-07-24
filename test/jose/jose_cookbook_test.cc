#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <filesystem>  // std::filesystem::path
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

// The IETF JOSE cookbook (RFC 7520) signs a plain text payload that is not a
// JSON object, so its signature examples are valid signatures but not tokens.
// They are exercised through the payload-agnostic signature engine rather than
// the token layer. Each file carries the key, the algorithm, and the signing
// input, so both verification and signing are driven from the same vectors. The
// encryption examples (Section 5) are randomized, so each is a decryption
// known-answer test that recovers the published plaintext from the published
// compact object
namespace {

auto load(const std::string_view subdirectory, const std::string_view filename)
    -> sourcemeta::core::JSON {
  const std::filesystem::path path{std::filesystem::path{JOSE_COOKBOOK_PATH} /
                                   subdirectory / filename};
  auto stream{sourcemeta::core::read_file(path)};
  return sourcemeta::core::parse_json(stream);
}

// The public key, built by keeping only the public parameters the example key
// carries alongside its private ones
auto cookbook_public_key(const sourcemeta::core::JSON &source_key)
    -> std::optional<sourcemeta::core::JWK> {
  const auto &type{source_key.at("kty").to_string()};
  auto key{sourcemeta::core::JSON::make_object()};
  key.assign("kty", source_key.at("kty"));
  if (type == "RSA") {
    key.assign("n", source_key.at("n"));
    key.assign("e", source_key.at("e"));
  } else if (type == "EC") {
    key.assign("crv", source_key.at("crv"));
    key.assign("x", source_key.at("x"));
    key.assign("y", source_key.at("y"));
  } else if (type == "oct") {
    key.assign("k", source_key.at("k"));
  } else {
    return std::nullopt;
  }

  return sourcemeta::core::JWK::from(key);
}

auto verify_example(const sourcemeta::core::JSON &document) -> bool {
  const auto &signing_input{document.at("signing").at("sig-input").to_string()};
  const auto signature{sourcemeta::core::base64url_decode(
      document.at("signing").at("sig").to_string())};
  const auto algorithm{sourcemeta::core::to_jws_algorithm(
      document.at("input").at("alg").to_string())};
  const auto key{cookbook_public_key(document.at("input").at("key"))};
  return signature.has_value() && key.has_value() &&
         sourcemeta::core::jws_verify_signature(algorithm, signing_input,
                                                signature.value(), key.value());
}

auto sign_example(const sourcemeta::core::JSON &document) -> bool {
  const auto &signing_input{document.at("signing").at("sig-input").to_string()};
  const auto algorithm{sourcemeta::core::to_jws_algorithm(
      document.at("input").at("alg").to_string())};
  const auto key{
      sourcemeta::core::JWKPrivate::from(document.at("input").at("key"))};
  if (!algorithm.has_value() || !key.has_value()) {
    return false;
  }

  const auto signature{sourcemeta::core::jws_sign(algorithm.value(),
                                                  signing_input, key.value())};
  if (!signature.has_value()) {
    return false;
  }

  // A reproducible example fixes its signature, while a randomized one can only
  // be verified through the public key
  if (document.defines("reproducible") &&
      document.at("reproducible").to_boolean()) {
    const auto expected{sourcemeta::core::base64url_decode(
        document.at("signing").at("sig").to_string())};
    return expected.has_value() && signature.value() == expected.value();
  }

  const auto verification_key{
      cookbook_public_key(document.at("input").at("key"))};
  return verification_key.has_value() &&
         sourcemeta::core::jws_verify_signature(algorithm, signing_input,
                                                signature.value(),
                                                verification_key.value());
}

auto decrypt_example(const sourcemeta::core::JSON &document) -> bool {
  const auto key{
      sourcemeta::core::JWKPrivate::from(document.at("input").at("key"))};
  const auto object{sourcemeta::core::JWE::from(
      document.at("output").at("compact").to_string())};
  if (!key.has_value() || !object.has_value()) {
    return false;
  }

  const auto plaintext{
      sourcemeta::core::jwe_decrypt(object.value(), key.value())};
  return plaintext.has_value() &&
         plaintext.value() == document.at("input").at("plaintext").to_string();
}

} // namespace

auto main(int argc, char **argv) -> int {
  // The single-signature compact examples, which our engine runs in full. The
  // detached, unprotected-header, and general serialization examples stay
  // masked out of the vendored tree
  constexpr std::array<std::string_view, 4> examples{
      {"4_1.rsa_v15_signature.json", "4_2.rsa-pss_signature.json",
       "4_3.ecdsa_signature.json", "4_4.hmac-sha2_integrity_protection.json"}};

  for (const auto example : examples) {
    const auto name{std::filesystem::path{example}.stem().string()};
    sourcemeta::core::test_register(name + ".verify", [example]() {
      EXPECT_TRUE(verify_example(load("jws", example)));
    });
    sourcemeta::core::test_register(name + ".sign", [example]() {
      EXPECT_TRUE(sign_example(load("jws", example)));
    });
  }

  // The compact JWE examples whose key-management and content encryption
  // algorithms this module implements. The masked-out examples use RSA1_5,
  // PBES2, AES-GCM key wrapping, compression, or the JSON serialization
  constexpr std::string_view ecdh_es_kw_example{
      "5_4.key_agreement_with_key_wrapping_using_ecdh-es_and_aes-keywrap_with_"
      "aes-gcm.json"};
  constexpr std::array<std::string_view, 5> encryption_examples{
      {"5_2.key_encryption_using_rsa-oaep_with_aes-gcm.json",
       ecdh_es_kw_example,
       "5_5.key_agreement_using_ecdh-es_with_aes-cbc-hmac-sha2.json",
       "5_6.direct_encryption_using_aes-gcm.json",
       "5_8.key_wrap_using_aes-keywrap_with_aes-gcm.json"}};

  for (const auto example : encryption_examples) {
    const auto name{std::filesystem::path{example}.stem().string()};
    sourcemeta::core::test_register(name + ".decrypt", [example]() {
      EXPECT_TRUE(decrypt_example(load("jwe", example)));
    });
  }

  return sourcemeta::core::test_run(argc, argv);
}
