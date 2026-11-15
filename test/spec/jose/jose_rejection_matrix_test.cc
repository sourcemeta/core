#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include "jose_jwe_test_keys.h"

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {
constexpr std::string_view MATRIX_PLAINTEXT{"Authenticated rejection control"};

struct ClaimCase {
  std::string_view member;
  std::string_view value;
  sourcemeta::core::JWTVerificationError error;
};

// Seal through the public primitive rather than jwe_encrypt, which correctly
// refuses the malformed headers and mismatched CEK sizes these tests need.
// The actual header, including any malformed member, is authenticated as AAD.
auto seal_gcm(const sourcemeta::core::JSON &header,
              const std::string_view encrypted_key,
              const std::string_view content_key)
    -> std::optional<sourcemeta::core::JWE> {
  std::ostringstream stream;
  sourcemeta::core::stringify(header, stream);
  const auto protected_header{sourcemeta::core::base64url_encode(stream.str())};
  const std::string iv(12, 'v');
  const auto sealed{sourcemeta::core::aes_gcm_encrypt(
      content_key, iv, protected_header, MATRIX_PLAINTEXT)};
  if (!sealed.has_value()) {
    return std::nullopt;
  }

  return sourcemeta::core::JWE::from(
      protected_header + "." +
      sourcemeta::core::base64url_encode(encrypted_key) + "." +
      sourcemeta::core::base64url_encode(iv) + "." +
      sourcemeta::core::base64url_encode(sealed->ciphertext()) + "." +
      sourcemeta::core::base64url_encode(sealed->tag()));
}

// Model an implementation that silently treats malformed party information as
// absent. Such an implementation could authenticate these objects, so a failed
// tag cannot mask a missing validation check. The published recipient key is
// also the fixed ephemeral key in this test-only construction.
auto seal_agreement(const std::string_view party,
                    const sourcemeta::core::JSON &value)
    -> std::optional<sourcemeta::core::JWE> {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(JWE_EC_PUBLIC_JWK))};
  const auto private_key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  if (!public_key.has_value() || !private_key.has_value()) {
    return std::nullopt;
  }

  const auto shared{sourcemeta::core::ecdh_derive(*private_key->private_key(),
                                                  *public_key->public_key())};
  if (!shared.has_value()) {
    return std::nullopt;
  }

  const auto content_key{
      sourcemeta::core::kdf_concat(shared.value(), "A128GCM", "", "", 16)};
  if (!content_key.has_value()) {
    return std::nullopt;
  }

  auto header{
      sourcemeta::core::parse_json(R"({"alg":"ECDH-ES","enc":"A128GCM"})")};
  header.assign("epk", sourcemeta::core::parse_json(JWE_EC_PUBLIC_JWK));
  if (!party.empty()) {
    header.assign(party, value);
  }

  return seal_gcm(header, "", content_key.value());
}
} // namespace

TEST(decrypt_rejects_malformed_agreement_parties_with_authenticated_headers) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Absence and an explicitly empty base64url string both mean empty party
  // information (RFC 7518 Sections 4.6.1.2, 4.6.1.3, and 4.6.2).
  for (const auto *const party : {"", "apu", "apv"}) {
    const auto control{seal_agreement(party, sourcemeta::core::JSON{""})};
    EXPECT_TRUE(control.has_value());
    const auto plaintext{
        sourcemeta::core::jwe_decrypt(control.value(), key.value())};
    EXPECT_TRUE(plaintext.has_value());
    EXPECT_EQ(plaintext.value(), MATRIX_PLAINTEXT);
  }

  for (const auto *const party : {"apu", "apv"}) {
    for (const auto *const malformed :
         {"null", "false", "42", "[]", "{}", R"("+")", R"("A")"}) {
      const auto object{
          seal_agreement(party, sourcemeta::core::parse_json(malformed))};
      EXPECT_TRUE(object.has_value());
      EXPECT_FALSE(sourcemeta::core::jwe_decrypt(object.value(), key.value())
                       .has_value());
    }
  }
}

TEST(decrypt_binds_unwrapped_content_key_size_to_the_declared_gcm_algorithm) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  // All three actual key sizes are valid for the primitive. Only the one
  // selected by enc may be used by JWE (RFC 7518 Section 5.3).
  for (const auto *const encryption : {"A128GCM", "A192GCM", "A256GCM"}) {
    auto header{sourcemeta::core::parse_json(R"({"alg":"A256KW"})")};
    header.assign("enc", sourcemeta::core::JSON{encryption});
    const auto algorithm{sourcemeta::core::to_jwe_encryption(encryption)};
    EXPECT_TRUE(algorithm.has_value());
    for (const auto bytes : {16, 24, 32}) {
      const std::string content_key(static_cast<std::size_t>(bytes), 'c');
      const auto wrapped{
          sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
      EXPECT_TRUE(wrapped.has_value());
      const auto object{seal_gcm(header, wrapped.value(), content_key)};
      EXPECT_TRUE(object.has_value());
      const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
      const auto expected{
          sourcemeta::core::jwe_encryption_key_bytes(algorithm.value())};
      if (content_key.size() == expected) {
        EXPECT_TRUE(plaintext.has_value());
        EXPECT_EQ(plaintext.value(), MATRIX_PLAINTEXT);
      } else {
        EXPECT_FALSE(plaintext.has_value());
      }
    }
  }
}

TEST(decrypt_direct_key_rejects_an_authenticated_nonempty_encrypted_key) {
  const std::string secret(16, 'k');
  const auto header{
      sourcemeta::core::parse_json(R"({"alg":"dir","enc":"A128GCM"})")};
  const auto key{sourcemeta::core::JWKPrivate::from_octets(secret)};
  const auto control{seal_gcm(header, "", secret)};
  EXPECT_TRUE(control.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(control.value(), key)};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), MATRIX_PLAINTEXT);
  // The encrypted-key segment is outside the AAD, so inserting it leaves the
  // tag valid. RFC 7518 Section 4.5 independently requires this segment empty.
  const auto invalid{seal_gcm(header, "unexpected", secret)};
  EXPECT_TRUE(invalid.has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(invalid.value(), key).has_value());
}

TEST(decrypt_rejects_unusable_ephemeral_key_headers) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  for (const auto *const invalid :
       {"null", "false", "42", "[]", R"("key")", "{}",
        R"({"kty":"EC","crv":"P-256","x":"+","y":"+"})"}) {
    auto header{
        sourcemeta::core::parse_json(R"({"alg":"ECDH-ES","enc":"A128GCM"})")};
    header.assign("epk", sourcemeta::core::parse_json(invalid));
    const auto object{seal_gcm(header, "", std::string(16, 'k'))};
    EXPECT_TRUE(object.has_value());
    // These cases assert structural rejection only: no valid agreement can
    // be completed with an unusable ephemeral key (RFC 7518 Section 4.6.1.1).
    EXPECT_FALSE(
        sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
  }
}

TEST(decrypt_rejects_an_unknown_encryption_with_a_supported_key_algorithm) {
  const auto object{
      seal_gcm(sourcemeta::core::parse_json(R"({"alg":"dir","enc":"unknown"})"),
               "", std::string(16, 'k'))};
  EXPECT_TRUE(object.has_value());
  EXPECT_TRUE(object->algorithm().has_value());
  EXPECT_FALSE(object->encryption().has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(
                   object.value(), sourcemeta::core::JWKPrivate::from_octets(
                                       std::string(16, 'k')))
                   .has_value());
}

TEST(encrypt_rejects_missing_or_malformed_algorithm_headers) {
  const auto key{sourcemeta::core::JWK::from_octets(std::string(16, 'k'))};
  const auto control{sourcemeta::core::jwe_encrypt(
      sourcemeta::core::parse_json(R"({"alg":"dir","enc":"A128GCM"})"),
      MATRIX_PLAINTEXT, key)};
  EXPECT_TRUE(control.has_value());
  // RFC 7516 Sections 4.1.1 and 4.1.2 require understood string values.
  for (const auto *const invalid :
       {R"({"enc":"A128GCM"})", R"({"alg":"dir","enc":null})",
        R"({"alg":"dir","enc":false})", R"({"alg":"dir","enc":42})",
        R"({"alg":"dir","enc":[]})", R"({"alg":"dir","enc":{}})",
        R"({"alg":"dir","enc":"unknown"})"}) {
    EXPECT_FALSE(
        sourcemeta::core::jwe_encrypt(sourcemeta::core::parse_json(invalid),
                                      MATRIX_PLAINTEXT, key)
            .has_value());
  }
}

TEST(jwk_rejects_nonstring_rsa_exponents_and_ec_parameters) {
  // Every other required parameter stays valid, so each typed-member check
  // must reject independently (RFC 7518 Sections 6.2.1 and 6.3.1.2).
  for (const auto *const member : {"e", "crv", "x", "y"}) {
    for (const auto *const invalid : {"null", "false", "42", "[]", "{}"}) {
      auto document{sourcemeta::core::parse_json(member == std::string_view{"e"}
                                                     ? JWE_RSA_PUBLIC_JWK
                                                     : JWE_EC_PUBLIC_JWK)};
      EXPECT_TRUE(sourcemeta::core::JWK::from(document).has_value());
      document.assign(member, sourcemeta::core::parse_json(invalid));
      EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
    }
  }
}

TEST(jwk_rejects_invalid_ec_coordinates_independently) {
  const auto control{sourcemeta::core::parse_json(JWE_EC_PUBLIC_JWK)};
  EXPECT_TRUE(sourcemeta::core::JWK::from(control).has_value());
  // Both coordinates must decode to the curve's full coordinate width.
  for (const auto *const member : {"x", "y"}) {
    for (const auto *const invalid : {"+", "A", "", "AA"}) {
      auto document{control};
      document.assign(member, sourcemeta::core::JSON{invalid});
      EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
    }
  }
}

TEST(jwt_reports_claim_rejections_only_after_a_valid_signature) {
  using sourcemeta::core::JWTVerificationError;
  const std::string secret(32, 'k');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(secret)};
  auto key_document{sourcemeta::core::parse_json(
      R"({"keys":[{"kty":"oct","kid":"matrix"}]})")};
  key_document.at("keys").at(0).assign(
      "k", sourcemeta::core::JSON{sourcemeta::core::base64url_encode(secret)});
  const auto keys{sourcemeta::core::JWKS::from(key_document)};
  EXPECT_TRUE(keys.has_value());
  const auto header{sourcemeta::core::parse_json(
      R"({"alg":"HS256","kid":"matrix","typ":"at+jwt"})")};
  const auto valid{sourcemeta::core::parse_json(
      R"({"iss":"issuer","sub":"user","aud":"api","iat":900,"exp":1100})")};
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::HS256}};
  const auto now{std::chrono::system_clock::from_time_t(1000)};
  const auto control{sourcemeta::core::jwt_sign(header, valid, key)};
  EXPECT_TRUE(control.has_value());
  EXPECT_FALSE(sourcemeta::core::jwt_verify(
                   sourcemeta::core::JWT{control.value()}, keys.value(),
                   allowed, "issuer", "api", now, {}, "user", "at+jwt",
                   std::chrono::seconds{200})
                   .has_value());

  // The clock and lifetime are fixed. A change to just one authenticated
  // claim must surface the corresponding verification error, not Signature.
  const std::array<ClaimCase, 7> cases{
      {ClaimCase{.member = "sub",
                 .value = R"("other")",
                 .error = JWTVerificationError::Subject},
       ClaimCase{.member = "exp",
                 .value = "1000",
                 .error = JWTVerificationError::Expiration},
       ClaimCase{.member = "nbf",
                 .value = "1001",
                 .error = JWTVerificationError::NotBefore},
       ClaimCase{.member = "nbf",
                 .value = "null",
                 .error = JWTVerificationError::NotBefore},
       ClaimCase{.member = "iat",
                 .value = "1001",
                 .error = JWTVerificationError::IssuedAt},
       ClaimCase{.member = "iat",
                 .value = "null",
                 .error = JWTVerificationError::IssuedAt},
       ClaimCase{.member = "exp",
                 .value = "1101",
                 .error = JWTVerificationError::Lifetime}}};
  for (const auto &entry : cases) {
    auto payload{valid};
    payload.assign(entry.member, sourcemeta::core::parse_json(entry.value));
    const auto encoded{sourcemeta::core::jwt_sign(header, payload, key)};
    EXPECT_TRUE(encoded.has_value());
    const sourcemeta::core::JWT token{encoded.value()};
    EXPECT_TRUE(sourcemeta::core::jwt_verify_signature(
        token, sourcemeta::core::JWK::from_octets(secret)));
    const auto error{sourcemeta::core::jwt_verify(
        token, keys.value(), allowed, "issuer", "api", now, {}, "user",
        "at+jwt", std::chrono::seconds{200})};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error.value(), entry.error);
  }
}

TEST(jwt_rejects_an_unrecognized_algorithm_before_key_selection) {
  const auto token{sourcemeta::core::JWT::from(
      sourcemeta::core::base64url_encode(R"({"alg":"none"})") + "." +
      sourcemeta::core::base64url_encode(
          R"({"iss":"issuer","aud":"api","exp":1100})") +
      ".")};
  EXPECT_TRUE(token.has_value());
  const auto keys{sourcemeta::core::JWKS::from(
      sourcemeta::core::parse_json(R"({"keys":[]})"))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::HS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "issuer", "api",
      std::chrono::system_clock::from_time_t(1000), {}, std::nullopt,
      std::nullopt)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(),
            sourcemeta::core::JWTVerificationError::AlgorithmNotAllowed);
}
