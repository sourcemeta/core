#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include "jose_jwe_test_keys.h"

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

static const std::string PLAINTEXT{"Encrypt then authenticate."};

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
  EXPECT_TRUE(fourth != std::string::npos);
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
  EXPECT_TRUE(fourth != std::string::npos);
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
