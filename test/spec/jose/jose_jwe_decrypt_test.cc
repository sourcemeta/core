#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include "jose_jwe_test_keys.h"

#include <sourcemeta/core/crypto.h>
#include <sstream> // std::ostringstream

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view PLAINTEXT{"Encrypt then authenticate."};

static auto header_for(const std::string_view algorithm,
                       const std::string_view encryption)
    -> sourcemeta::core::JSON {
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{std::string{algorithm}});
  header.assign("enc", sourcemeta::core::JSON{std::string{encryption}});
  return header;
}

static auto encrypt_symmetric(const std::string_view secret)
    -> std::optional<std::string> {
  return sourcemeta::core::jwe_encrypt(
      header_for("A256KW", "A128GCM"), PLAINTEXT,
      sourcemeta::core::JWK::from_octets(secret));
}

namespace {
constexpr std::string_view AUTHENTICATED_PLAINTEXT{
    "Authenticated rejection control"};

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
      content_key, iv, protected_header, AUTHENTICATED_PLAINTEXT)};
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

TEST(decrypt_round_trips_a_valid_object) {
  const std::string secret(32, 'k');
  const auto object{encrypt_symmetric(secret)};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(
      parsed.value(), sourcemeta::core::JWKPrivate::from_octets(secret))};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), PLAINTEXT);
}

TEST(decrypt_rejects_a_tampered_tag) {
  const std::string secret(32, 'k');
  const auto object{encrypt_symmetric(secret)};
  EXPECT_TRUE(object.has_value());
  // Flip the first character of the tag segment (the fifth of five), avoiding
  // the final base64url character whose low bits are non-significant padding
  auto tampered{object.value()};
  const auto first{tampered.find('.')};
  const auto second{tampered.find('.', first + 1)};
  const auto third{tampered.find('.', second + 1)};
  const auto fourth{tampered.find('.', third + 1)};
  EXPECT_NE(fourth, std::string::npos);
  tampered[fourth + 1] = (tampered[fourth + 1] == 'A') ? 'B' : 'A';
  const auto parsed{sourcemeta::core::JWE::from(tampered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(
          parsed.value(), sourcemeta::core::JWKPrivate::from_octets(secret))
          .has_value());
}

TEST(decrypt_rejects_a_tampered_ciphertext) {
  const std::string secret(32, 'k');
  const auto object{encrypt_symmetric(secret)};
  EXPECT_TRUE(object.has_value());
  // Flip a character in the ciphertext segment (the fourth of five)
  auto tampered{object.value()};
  const auto first{tampered.find('.')};
  const auto second{tampered.find('.', first + 1)};
  const auto third{tampered.find('.', second + 1)};
  const auto fourth{tampered.find('.', third + 1)};
  tampered[third + 1] = (tampered[third + 1] == 'A') ? 'B' : 'A';
  EXPECT_NE(fourth, std::string::npos);
  const auto parsed{sourcemeta::core::JWE::from(tampered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(
          parsed.value(), sourcemeta::core::JWKPrivate::from_octets(secret))
          .has_value());
}

TEST(decrypt_rejects_a_tampered_protected_header) {
  const std::string secret(32, 'k');
  const auto object{encrypt_symmetric(secret)};
  EXPECT_TRUE(object.has_value());
  // Re-encode the protected header with an extra parameter, so the same
  // algorithms are named but the authenticated bytes no longer match
  const auto dot{object.value().find('.')};
  std::string tampered{sourcemeta::core::base64url_encode(
      R"({"alg":"A256KW","enc":"A128GCM","kid":"x"})")};
  tampered.append(object.value().substr(dot));
  const auto parsed{sourcemeta::core::JWE::from(tampered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(
          parsed.value(), sourcemeta::core::JWKPrivate::from_octets(secret))
          .has_value());
}

TEST(decrypt_rejects_the_wrong_key) {
  const auto object{encrypt_symmetric(std::string(32, 'k'))};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(
                   parsed.value(), sourcemeta::core::JWKPrivate::from_octets(
                                       std::string(32, 'm')))
                   .has_value());
}

TEST(decrypt_rejects_a_key_type_mismatch) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_RSA_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("RSA-OAEP", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  // An octet secret cannot serve an RSA algorithm
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(
                   parsed.value(), sourcemeta::core::JWKPrivate::from_octets(
                                       std::string(32, 'k')))
                   .has_value());
}

TEST(decrypt_rejects_an_unsupported_algorithm) {
  const std::string secret(32, 'k');
  const auto object{encrypt_symmetric(secret)};
  EXPECT_TRUE(object.has_value());
  // Re-encode the header naming an unsupported algorithm
  const auto dot{object.value().find('.')};
  std::string altered{sourcemeta::core::base64url_encode(
      R"({"alg":"RSA1_5","enc":"A128GCM"})")};
  altered.append(object.value().substr(dot));
  const auto parsed{sourcemeta::core::JWE::from(altered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(
          parsed.value(), sourcemeta::core::JWKPrivate::from_octets(secret))
          .has_value());
}

TEST(decrypt_rejects_a_tampered_initialization_vector) {
  const std::string secret(32, 'k');
  const auto object{encrypt_symmetric(secret)};
  EXPECT_TRUE(object.has_value());
  // Flip a character in the IV segment (the third of five)
  auto tampered{object.value()};
  const auto first{tampered.find('.')};
  const auto second{tampered.find('.', first + 1)};
  EXPECT_NE(second, std::string::npos);
  tampered[second + 1] = (tampered[second + 1] == 'A') ? 'B' : 'A';
  const auto parsed{sourcemeta::core::JWE::from(tampered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(
          parsed.value(), sourcemeta::core::JWKPrivate::from_octets(secret))
          .has_value());
}

TEST(decrypt_rejects_ecdh_es_with_a_non_empty_encrypted_key) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PUBLIC_JWK}))};
  const auto private_key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PRIVATE_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(private_key.has_value());
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("ECDH-ES", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  // Direct Key Agreement leaves the Encrypted Key segment empty; inject one so
  // the object violates RFC 7516 Section 5.2 step 10
  const auto first{object.value().find('.')};
  std::string tampered{object.value().substr(0, first + 1)};
  tampered.append("QUFB");
  tampered.append(object.value().substr(first + 1));
  const auto parsed{sourcemeta::core::JWE::from(tampered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(parsed.value(), private_key.value())
          .has_value());
}

TEST(decrypt_rejects_ecdh_es_with_a_missing_ephemeral_key) {
  const auto private_key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PRIVATE_JWK}))};
  EXPECT_TRUE(private_key.has_value());
  // A structurally valid object whose header names ECDH-ES but carries no epk
  std::string compact{sourcemeta::core::base64url_encode(
      R"({"alg":"ECDH-ES","enc":"A128GCM"})")};
  compact.append(".QUFB.QUFB.QUFB.QUFB");
  const auto parsed{sourcemeta::core::JWE::from(compact)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(parsed.value(), private_key.value())
          .has_value());
}

TEST(decrypt_rejects_a_key_wrap_secret_of_the_wrong_size) {
  const auto object{encrypt_symmetric(std::string(32, 'k'))};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  // The wrapping algorithm names the exact key length, so a shorter secret is
  // a different key rather than a truncated one
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(
                   parsed.value(), sourcemeta::core::JWKPrivate::from_octets(
                                       std::string(16, 'k')))
                   .has_value());
}

TEST(decrypt_rejects_a_direct_secret_of_the_wrong_size) {
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("dir", "A128GCM"), PLAINTEXT,
      sourcemeta::core::JWK::from_octets(std::string(16, 'k')))};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(
                   parsed.value(), sourcemeta::core::JWKPrivate::from_octets(
                                       std::string(32, 'k')))
                   .has_value());
}

TEST(decrypt_rejects_an_octet_secret_for_an_agreement_algorithm) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("ECDH-ES", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(
                   parsed.value(), sourcemeta::core::JWKPrivate::from_octets(
                                       std::string(32, 'k')))
                   .has_value());
}

TEST(decrypt_rejects_an_ephemeral_key_that_is_not_an_elliptic_curve_key) {
  const auto private_key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PRIVATE_JWK}))};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("ECDH-ES", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  // The ephemeral key travels in the authenticated protected header, so
  // replacing it also breaks the tag. This asserts only that an object whose
  // ephemeral key names a key type the agreement cannot use is refused, not
  // which of the two checks refuses it
  const auto dot{object.value().find('.')};
  std::string tampered{sourcemeta::core::base64url_encode(
      R"({"alg":"ECDH-ES","enc":"A128GCM",)"
      R"("epk":{"kty":"oct","k":"c2VjcmV0"}})")};
  tampered.append(object.value().substr(dot));
  const auto parsed{sourcemeta::core::JWE::from(tampered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(parsed.value(), private_key.value())
          .has_value());
}

TEST(decrypt_rejects_an_ephemeral_key_on_a_different_curve) {
  const auto private_key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PRIVATE_JWK}))};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("ECDH-ES", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  // As above, the replaced header breaks the tag as well, so this asserts only
  // that an object whose ephemeral key sits on another curve is refused
  const auto dot{object.value().find('.')};
  std::string tampered{sourcemeta::core::base64url_encode(
      R"({"alg":"ECDH-ES","enc":"A128GCM","epk":{"kty":"EC","crv":"P-384",)"
      R"("x":"nAyG5AafvBM0A05nRrojvOTCgNNrwUckW0Ivl2fta_lgwfXhMTjb_59y0BLO3GH_",)"
      R"("y":"htZkNv1iPf0Ac1TvJB0LRdFID1KV9WxOhiQ0Ec32cdbTWaSu8-NusKWs6md80V8G"}})")};
  tampered.append(object.value().substr(dot));
  const auto parsed{sourcemeta::core::JWE::from(tampered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(parsed.value(), private_key.value())
          .has_value());
}

TEST(ecdh_es_decrypt_accepts_absent_party_information) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{seal_agreement("", sourcemeta::core::JSON{""})};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{
      sourcemeta::core::jwe_decrypt(object.value(), key.value())};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), AUTHENTICATED_PLAINTEXT);
}

TEST(ecdh_es_decrypt_accepts_empty_apu_party_information) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{seal_agreement("apu", sourcemeta::core::JSON{""})};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{
      sourcemeta::core::jwe_decrypt(object.value(), key.value())};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), AUTHENTICATED_PLAINTEXT);
}

TEST(ecdh_es_decrypt_accepts_empty_apv_party_information) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{seal_agreement("apv", sourcemeta::core::JSON{""})};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{
      sourcemeta::core::jwe_decrypt(object.value(), key.value())};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), AUTHENTICATED_PLAINTEXT);
}

TEST(ecdh_es_decrypt_rejects_null_apu_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apu", sourcemeta::core::parse_json(R"JSON(null)JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_boolean_apu_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apu", sourcemeta::core::parse_json(R"JSON(false)JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_number_apu_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apu", sourcemeta::core::parse_json(R"JSON(42)JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_array_apu_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apu", sourcemeta::core::parse_json(R"JSON([])JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_object_apu_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apu", sourcemeta::core::parse_json(R"JSON({})JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_invalid_alphabet_apu_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apu", sourcemeta::core::parse_json(R"JSON("+")JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_invalid_length_apu_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apu", sourcemeta::core::parse_json(R"JSON("A")JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_null_apv_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apv", sourcemeta::core::parse_json(R"JSON(null)JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_boolean_apv_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apv", sourcemeta::core::parse_json(R"JSON(false)JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_number_apv_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apv", sourcemeta::core::parse_json(R"JSON(42)JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_array_apv_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apv", sourcemeta::core::parse_json(R"JSON([])JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_object_apv_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apv", sourcemeta::core::parse_json(R"JSON({})JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_invalid_alphabet_apv_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apv", sourcemeta::core::parse_json(R"JSON("+")JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_invalid_length_apv_with_authenticated_header) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto object{
      seal_agreement("apv", sourcemeta::core::parse_json(R"JSON("A")JSON"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(decrypt_a128gcm_accepts_unwrapped_128_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A128GCM"})JSON")};
  const std::string content_key(16, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), AUTHENTICATED_PLAINTEXT);
}

TEST(decrypt_a128gcm_rejects_unwrapped_192_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A128GCM"})JSON")};
  const std::string content_key(24, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_FALSE(plaintext.has_value());
}

TEST(decrypt_a128gcm_rejects_unwrapped_256_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A128GCM"})JSON")};
  const std::string content_key(32, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_FALSE(plaintext.has_value());
}

TEST(decrypt_a192gcm_rejects_unwrapped_128_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A192GCM"})JSON")};
  const std::string content_key(16, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_FALSE(plaintext.has_value());
}

TEST(decrypt_a192gcm_accepts_unwrapped_192_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A192GCM"})JSON")};
  const std::string content_key(24, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), AUTHENTICATED_PLAINTEXT);
}

TEST(decrypt_a192gcm_rejects_unwrapped_256_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A192GCM"})JSON")};
  const std::string content_key(32, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_FALSE(plaintext.has_value());
}

TEST(decrypt_a256gcm_rejects_unwrapped_128_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A256GCM"})JSON")};
  const std::string content_key(16, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_FALSE(plaintext.has_value());
}

TEST(decrypt_a256gcm_rejects_unwrapped_192_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A256GCM"})JSON")};
  const std::string content_key(24, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_FALSE(plaintext.has_value());
}

TEST(decrypt_a256gcm_accepts_unwrapped_256_bit_key) {
  const std::string wrapping_secret(32, 'w');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(wrapping_secret)};
  const auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"A256KW","enc":"A256GCM"})JSON")};
  const std::string content_key(32, 'c');
  const auto wrapped{
      sourcemeta::core::aes_key_wrap(wrapping_secret, content_key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto object{seal_gcm(header, wrapped.value(), content_key)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), AUTHENTICATED_PLAINTEXT);
}

TEST(decrypt_direct_key_accepts_empty_encrypted_key) {
  const std::string secret(16, 'k');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(secret)};
  const auto header{
      sourcemeta::core::parse_json(R"JSON({"alg":"dir","enc":"A128GCM"})JSON")};
  const auto object{seal_gcm(header, "", secret)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_TRUE(plaintext.has_value());
  EXPECT_EQ(plaintext.value(), AUTHENTICATED_PLAINTEXT);
}

TEST(decrypt_direct_key_rejects_nonempty_encrypted_key) {
  const std::string secret(16, 'k');
  const auto key{sourcemeta::core::JWKPrivate::from_octets(secret)};
  const auto header{
      sourcemeta::core::parse_json(R"JSON({"alg":"dir","enc":"A128GCM"})JSON")};
  const auto object{seal_gcm(header, "unexpected", secret)};
  EXPECT_TRUE(object.has_value());
  const auto plaintext{sourcemeta::core::jwe_decrypt(object.value(), key)};
  EXPECT_FALSE(plaintext.has_value());
}

TEST(ecdh_es_decrypt_rejects_null_ephemeral_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Structural rejection only: this header cannot form a valid agreement.
  auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"ECDH-ES","enc":"A128GCM"})JSON")};
  header.assign("epk", sourcemeta::core::parse_json(R"JSON(null)JSON"));
  const auto object{seal_gcm(header, "", std::string(16, 'k'))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_boolean_ephemeral_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Structural rejection only: this header cannot form a valid agreement.
  auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"ECDH-ES","enc":"A128GCM"})JSON")};
  header.assign("epk", sourcemeta::core::parse_json(R"JSON(false)JSON"));
  const auto object{seal_gcm(header, "", std::string(16, 'k'))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_number_ephemeral_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Structural rejection only: this header cannot form a valid agreement.
  auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"ECDH-ES","enc":"A128GCM"})JSON")};
  header.assign("epk", sourcemeta::core::parse_json(R"JSON(42)JSON"));
  const auto object{seal_gcm(header, "", std::string(16, 'k'))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_array_ephemeral_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Structural rejection only: this header cannot form a valid agreement.
  auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"ECDH-ES","enc":"A128GCM"})JSON")};
  header.assign("epk", sourcemeta::core::parse_json(R"JSON([])JSON"));
  const auto object{seal_gcm(header, "", std::string(16, 'k'))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_string_ephemeral_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Structural rejection only: this header cannot form a valid agreement.
  auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"ECDH-ES","enc":"A128GCM"})JSON")};
  header.assign("epk", sourcemeta::core::parse_json(R"JSON("key")JSON"));
  const auto object{seal_gcm(header, "", std::string(16, 'k'))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_empty_object_ephemeral_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Structural rejection only: this header cannot form a valid agreement.
  auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"ECDH-ES","enc":"A128GCM"})JSON")};
  header.assign("epk", sourcemeta::core::parse_json(R"JSON({})JSON"));
  const auto object{seal_gcm(header, "", std::string(16, 'k'))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
}

TEST(ecdh_es_decrypt_rejects_malformed_coordinates_ephemeral_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(JWE_EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Structural rejection only: this header cannot form a valid agreement.
  auto header{sourcemeta::core::parse_json(
      R"JSON({"alg":"ECDH-ES","enc":"A128GCM"})JSON")};
  header.assign("epk",
                sourcemeta::core::parse_json(
                    R"JSON({"kty":"EC","crv":"P-256","x":"+","y":"+"})JSON"));
  const auto object{seal_gcm(header, "", std::string(16, 'k'))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(object.value(), key.value()).has_value());
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

// Key unwrapping and direct decryption each take a secret, so a key of another
// kind is refused before the ciphertext is touched
TEST(decrypt_refuses_a_key_wrap_key_that_is_not_a_secret) {
  const auto object{encrypt_symmetric(std::string(32, 'k'))};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(std::string{JWE_RSA_PRIVATE_JWK}))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(parsed.value(), key.value()).has_value());
}

TEST(decrypt_refuses_a_direct_key_that_is_not_a_secret) {
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("dir", "A128GCM"), PLAINTEXT,
      sourcemeta::core::JWK::from_octets(std::string(16, 'k')))};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(std::string{JWE_RSA_PRIVATE_JWK}))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(parsed.value(), key.value()).has_value());
}

// A key agreement needs an elliptic curve key, so an octet secret is refused
TEST(decrypt_refuses_an_agreement_key_that_is_not_elliptic) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("ECDH-ES", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(
                   parsed.value(), sourcemeta::core::JWKPrivate::from_octets(
                                       std::string(32, 'k')))
                   .has_value());
}

// The ephemeral key travels in the header and is built through the same key
// machinery, so a point that lies on no curve forms nothing to agree with. The
// replaced header breaks the tag as well, so this asserts only the refusal
TEST(ecdh_es_decrypt_rejects_an_ephemeral_point_off_the_curve) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  const auto private_key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PRIVATE_JWK}))};
  EXPECT_TRUE(private_key.has_value());
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("ECDH-ES", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  const auto dot{object.value().find('.')};
  std::string tampered{sourcemeta::core::base64url_encode(
      R"({"alg":"ECDH-ES","enc":"A128GCM","epk":{"kty":"EC","crv":"P-256",)"
      R"("x":"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",)"
      R"("y":"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"}})")};
  tampered.append(object.value().substr(dot));
  const auto parsed{sourcemeta::core::JWE::from(tampered)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(parsed.value(), private_key.value())
          .has_value());
}

// RFC 7518 Section 4.5 ties the secret of a direct encryption to the width the
// content encryption asks for, and RFC 7516 Section 11.5 has a recipient give
// nothing away about which part was wrong, so a secret of another width fails
// at the tag rather than before it
TEST(direct_decrypt_rejects_a_secret_of_another_width) {
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("dir", "A128GCM"), PLAINTEXT,
      sourcemeta::core::JWK::from_octets(std::string(16, 'k')))};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_decrypt(
                   parsed.value(), sourcemeta::core::JWKPrivate::from_octets(
                                       std::string(32, 'k')))
                   .has_value());
}

// RFC 7518 Section 6.3.2 has a private RSA JWK carry its public members too,
// and the parse deliberately leaves their correspondence unchecked, so a
// document whose public exponent is outside the range RFC 8017 Section 3.1
// allows still parses while forming no platform key. Decryption refuses it
// rather than reaching the primitive
TEST(decrypt_refuses_an_rsa_key_that_formed_no_platform_key) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_RSA_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("RSA-OAEP", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());

  std::string document{JWE_RSA_PRIVATE_JWK};
  const auto exponent{document.find(R"("e":"AQAB")")};
  EXPECT_NE(exponent, std::string::npos);
  document.replace(exponent, 10, R"("e":"Ag"  )");
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(document))};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().private_key(), nullptr);
  EXPECT_FALSE(
      sourcemeta::core::jwe_decrypt(parsed.value(), key.value()).has_value());
}
