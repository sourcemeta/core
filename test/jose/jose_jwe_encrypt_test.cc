#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include "jose_jwe_test_keys.h"

#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view PLAINTEXT{
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

// A round trip carrying an explicit plaintext through the symmetric direct path
static auto direct_round_trips(const std::string_view encryption,
                               const std::string_view secret,
                               const std::string_view plaintext) -> bool {
  const auto object{sourcemeta::core::jwe_encrypt(
      header_for("dir", encryption), plaintext,
      sourcemeta::core::JWK::from_octets(secret))};
  if (!object.has_value()) {
    return false;
  }

  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  if (!parsed.has_value()) {
    return false;
  }

  const auto recovered{sourcemeta::core::jwe_decrypt(
      parsed.value(), sourcemeta::core::JWKPrivate::from_octets(secret))};
  return recovered.has_value() && recovered.value() == plaintext;
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

TEST(encrypt_rejects_a_compression_header) {
  auto header{header_for("dir", "A128GCM")};
  header.assign("zip", sourcemeta::core::JSON{std::string{"DEF"}});
  const auto secret{sourcemeta::core::JWK::from_octets(std::string(16, 'k'))};
  EXPECT_FALSE(
      sourcemeta::core::jwe_encrypt(header, "hello", secret).has_value());
}

TEST(encrypt_rejects_a_non_string_algorithm) {
  auto header{sourcemeta::core::JSON::make_object()};
  header.assign("alg", sourcemeta::core::JSON{1});
  header.assign("enc", sourcemeta::core::JSON{std::string{"A128GCM"}});
  const auto secret{sourcemeta::core::JWK::from_octets(std::string(16, 'k'))};
  EXPECT_FALSE(
      sourcemeta::core::jwe_encrypt(header, "hello", secret).has_value());
}

TEST(empty_plaintext_round_trips_gcm) {
  EXPECT_TRUE(direct_round_trips("A128GCM", std::string(16, 'k'), ""));
}

TEST(empty_plaintext_round_trips_cbc) {
  EXPECT_TRUE(direct_round_trips("A256CBC-HS512", std::string(64, 'k'), ""));
}

TEST(binary_plaintext_round_trips) {
  const std::string plaintext{std::string("\x00\x01\xff\x00\xfe", 5)};
  EXPECT_TRUE(direct_round_trips("A128GCM", std::string(16, 'k'), plaintext));
}

TEST(ecdh_es_round_trips_with_party_information) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PUBLIC_JWK}))};
  const auto private_key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PRIVATE_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(private_key.has_value());
  auto header{header_for("ECDH-ES", "A128GCM")};
  // "Alice" and "Bob" base64url-encoded (RFC 7518 Section 4.6.2)
  header.assign("apu", sourcemeta::core::JSON{std::string{"QWxpY2U"}});
  header.assign("apv", sourcemeta::core::JSON{std::string{"Qm9i"}});
  const auto object{
      sourcemeta::core::jwe_encrypt(header, PLAINTEXT, public_key.value())};
  EXPECT_TRUE(object.has_value());
  const auto parsed{sourcemeta::core::JWE::from(object.value())};
  EXPECT_TRUE(parsed.has_value());
  const auto recovered{
      sourcemeta::core::jwe_decrypt(parsed.value(), private_key.value())};
  EXPECT_TRUE(recovered.has_value());
  EXPECT_EQ(recovered.value(), PLAINTEXT);
}

TEST(encrypt_rejects_duplicate_header_members) {
  const auto secret{sourcemeta::core::JWK::from_octets(std::string(16, 'k'))};
  const auto header{sourcemeta::core::parse_json(
      R"({"alg":"dir","enc":"A128GCM","enc":"A256GCM"})")};
  EXPECT_FALSE(
      sourcemeta::core::jwe_encrypt(header, "hello", secret).has_value());
}

TEST(encrypt_is_randomized) {
  const auto secret{sourcemeta::core::JWK::from_octets(std::string(16, 'k'))};
  const auto first{sourcemeta::core::jwe_encrypt(header_for("dir", "A128GCM"),
                                                 PLAINTEXT, secret)};
  const auto second{sourcemeta::core::jwe_encrypt(header_for("dir", "A128GCM"),
                                                  PLAINTEXT, secret)};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  // A fresh initialization vector makes each object distinct
  EXPECT_NE(first.value(), second.value());
}

TEST(encrypt_generates_a_fresh_ephemeral_key) {
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(std::string{JWE_EC_PUBLIC_JWK}))};
  EXPECT_TRUE(public_key.has_value());
  const auto first{sourcemeta::core::jwe_encrypt(
      header_for("ECDH-ES", "A128GCM"), PLAINTEXT, public_key.value())};
  const auto second{sourcemeta::core::jwe_encrypt(
      header_for("ECDH-ES", "A128GCM"), PLAINTEXT, public_key.value())};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  // A fresh ephemeral key changes the epk and thus the whole object
  EXPECT_NE(first.value(), second.value());
}
