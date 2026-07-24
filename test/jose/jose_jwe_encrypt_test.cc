#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include "jose_jwe_test_keys.h"

#include <string>      // std::string
#include <string_view> // std::string_view

static const std::string PLAINTEXT{
    "The true sign of intelligence is not knowledge but imagination."};

static auto header_for(const std::string_view algorithm,
                       const std::string_view encryption)
    -> sourcemeta::core::JSON {
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{std::string{algorithm}});
  header.assign("enc", sourcemeta::core::JSON{std::string{encryption}});
  return header;
}

// Encryption is randomized, so a round trip is the assertion: the plaintext
// recovered from the object equals what was encrypted
static auto asymmetric_round_trips(const std::string_view algorithm,
                                   const std::string_view encryption,
                                   const std::string_view public_json,
                                   const std::string_view private_json)
    -> bool {
  const auto public_key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(public_json))};
  const auto private_key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(private_json))};
  if (!public_key.has_value() || !private_key.has_value()) {
    return false;
  }

  const auto object{sourcemeta::core::jwe_encrypt(
      header_for(algorithm, encryption), PLAINTEXT, public_key.value())};
  if (!object.has_value()) {
    return false;
  }

  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  if (!parsed.has_value()) {
    return false;
  }

  const auto plaintext{
      sourcemeta::core::jwe_decrypt(parsed.value(), private_key.value())};
  return plaintext.has_value() && plaintext.value() == PLAINTEXT;
}

static auto symmetric_round_trips(const std::string_view algorithm,
                                  const std::string_view encryption,
                                  const std::string_view secret) -> bool {
  const auto public_key{sourcemeta::core::JWK::from_octets(secret)};
  const auto private_key{sourcemeta::core::JWKPrivate::from_octets(secret)};
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for(algorithm, encryption), PLAINTEXT, public_key)};
  if (!object.has_value()) {
    return false;
  }

  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  if (!parsed.has_value()) {
    return false;
  }

  const auto plaintext{
      sourcemeta::core::jwe_decrypt(parsed.value(), private_key)};
  return plaintext.has_value() && plaintext.value() == PLAINTEXT;
}

TEST(rsa_oaep_a128gcm_round_trips) {
  EXPECT_TRUE(asymmetric_round_trips("RSA-OAEP", "A128GCM", JWE_RSA_PUBLIC_JWK,
                                     JWE_RSA_PRIVATE_JWK));
}

TEST(rsa_oaep_256_a128gcm_round_trips) {
  EXPECT_TRUE(asymmetric_round_trips("RSA-OAEP-256", "A128GCM",
                                     JWE_RSA_PUBLIC_JWK, JWE_RSA_PRIVATE_JWK));
}

TEST(rsa_oaep_256_a256cbc_hs512_round_trips) {
  EXPECT_TRUE(asymmetric_round_trips("RSA-OAEP-256", "A256CBC-HS512",
                                     JWE_RSA_PUBLIC_JWK, JWE_RSA_PRIVATE_JWK));
}

TEST(ecdh_es_a128gcm_round_trips) {
  EXPECT_TRUE(asymmetric_round_trips("ECDH-ES", "A128GCM", JWE_EC_PUBLIC_JWK,
                                     JWE_EC_PRIVATE_JWK));
}

TEST(ecdh_es_a256gcm_round_trips) {
  EXPECT_TRUE(asymmetric_round_trips("ECDH-ES", "A256GCM", JWE_EC_PUBLIC_JWK,
                                     JWE_EC_PRIVATE_JWK));
}

TEST(ecdh_es_a128kw_a128gcm_round_trips) {
  EXPECT_TRUE(asymmetric_round_trips("ECDH-ES+A128KW", "A128GCM",
                                     JWE_EC_PUBLIC_JWK, JWE_EC_PRIVATE_JWK));
}

TEST(ecdh_es_a192kw_a128gcm_round_trips) {
  EXPECT_TRUE(asymmetric_round_trips("ECDH-ES+A192KW", "A128GCM",
                                     JWE_EC_PUBLIC_JWK, JWE_EC_PRIVATE_JWK));
}

TEST(ecdh_es_a256kw_a256cbc_hs512_round_trips) {
  EXPECT_TRUE(asymmetric_round_trips("ECDH-ES+A256KW", "A256CBC-HS512",
                                     JWE_EC_PUBLIC_JWK, JWE_EC_PRIVATE_JWK));
}

TEST(a128kw_a128gcm_round_trips) {
  EXPECT_TRUE(symmetric_round_trips("A128KW", "A128GCM", std::string(16, 'k')));
}

TEST(a192kw_a128gcm_round_trips) {
  EXPECT_TRUE(symmetric_round_trips("A192KW", "A128GCM", std::string(24, 'k')));
}

TEST(a256kw_a128gcm_round_trips) {
  EXPECT_TRUE(symmetric_round_trips("A256KW", "A128GCM", std::string(32, 'k')));
}

TEST(a256kw_a192gcm_round_trips) {
  EXPECT_TRUE(symmetric_round_trips("A256KW", "A192GCM", std::string(32, 'k')));
}

TEST(a256kw_a256gcm_round_trips) {
  EXPECT_TRUE(symmetric_round_trips("A256KW", "A256GCM", std::string(32, 'k')));
}

TEST(a256kw_a128cbc_hs256_round_trips) {
  EXPECT_TRUE(
      symmetric_round_trips("A256KW", "A128CBC-HS256", std::string(32, 'k')));
}

TEST(a256kw_a192cbc_hs384_round_trips) {
  EXPECT_TRUE(
      symmetric_round_trips("A256KW", "A192CBC-HS384", std::string(32, 'k')));
}

TEST(a256kw_a256cbc_hs512_round_trips) {
  EXPECT_TRUE(
      symmetric_round_trips("A256KW", "A256CBC-HS512", std::string(32, 'k')));
}

TEST(dir_a128gcm_round_trips) {
  EXPECT_TRUE(symmetric_round_trips("dir", "A128GCM", std::string(16, 'k')));
}

TEST(dir_a256cbc_hs512_round_trips) {
  EXPECT_TRUE(
      symmetric_round_trips("dir", "A256CBC-HS512", std::string(64, 'k')));
}

TEST(encrypt_rejects_a_non_object_header) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_RSA_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_encrypt(sourcemeta::core::JSON{true},
                                             PLAINTEXT, public_key.value())
                   .has_value());
}

TEST(encrypt_rejects_a_critical_header) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_RSA_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  auto header{header_for("RSA-OAEP", "A128GCM")};
  header.assign("crit", sourcemeta::core::JSON::make_array());
  EXPECT_FALSE(
      sourcemeta::core::jwe_encrypt(header, PLAINTEXT, public_key.value())
          .has_value());
}

TEST(encrypt_rejects_a_missing_encryption) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_RSA_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{std::string{"RSA-OAEP"}});
  EXPECT_FALSE(
      sourcemeta::core::jwe_encrypt(header, PLAINTEXT, public_key.value())
          .has_value());
}

TEST(encrypt_rejects_an_unsupported_algorithm) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_RSA_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_FALSE(sourcemeta::core::jwe_encrypt(header_for("RSA1_5", "A128GCM"),
                                             PLAINTEXT, public_key.value())
                   .has_value());
}

TEST(encrypt_rejects_a_key_type_mismatch) {
  const auto secret{sourcemeta::core::JWK::from_octets(std::string(32, 'k'))};
  EXPECT_FALSE(sourcemeta::core::jwe_encrypt(header_for("RSA-OAEP", "A128GCM"),
                                             PLAINTEXT, secret)
                   .has_value());
}

TEST(encrypt_rejects_a_wrong_size_key_wrap_secret) {
  const auto secret{sourcemeta::core::JWK::from_octets(std::string(32, 'k'))};
  EXPECT_FALSE(sourcemeta::core::jwe_encrypt(header_for("A128KW", "A128GCM"),
                                             PLAINTEXT, secret)
                   .has_value());
}

TEST(encrypt_rejects_a_wrong_size_direct_secret) {
  const auto secret{sourcemeta::core::JWK::from_octets(std::string(16, 'k'))};
  EXPECT_FALSE(sourcemeta::core::jwe_encrypt(header_for("dir", "A256GCM"),
                                             PLAINTEXT, secret)
                   .has_value());
}
