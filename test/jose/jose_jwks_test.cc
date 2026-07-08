#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include "jose_test_keys.h"

#include <optional> // std::optional
#include <string>   // std::string
#include <utility>  // std::move

TEST(parses_single_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB",
                       "alg": "RS256", "kid": "rsa-1" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  EXPECT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);
  EXPECT_FALSE(keys.value().empty());

  const auto *key{keys.value().find("rsa-1")};
  EXPECT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "rsa-1");
}

TEST(parses_multiple_keys) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [
             { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB", "alg": "RS256",
               "kid": "rsa-1" },
             { "kty": "EC", "crv": "P-256",
               "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
               "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
               "alg": "ES256", "kid": "ec-1" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  EXPECT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 2);

  const auto *rsa{keys.value().find("rsa-1")};
  EXPECT_NE(rsa, nullptr);
  EXPECT_EQ(rsa->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(rsa->algorithm().has_value());
  EXPECT_EQ(rsa->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(rsa->key_id().has_value());
  EXPECT_EQ(rsa->key_id().value(), "rsa-1");

  const auto *ec{keys.value().find("ec-1")};
  EXPECT_NE(ec, nullptr);
  EXPECT_EQ(ec->type(), sourcemeta::core::JWK::Type::EllipticCurve);
  EXPECT_EQ(ec->curve(), "P-256");
  EXPECT_TRUE(ec->algorithm().has_value());
  EXPECT_EQ(ec->algorithm().value(), sourcemeta::core::JWSAlgorithm::ES256);
  EXPECT_TRUE(ec->key_id().has_value());
  EXPECT_EQ(ec->key_id().value(), "ec-1");
}

TEST(skips_unsupported_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [
             { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB", "alg": "RS256",
               "kid": "good" },
             { "kty": "unsupported" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  EXPECT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);

  const auto *key{keys.value().find("good")};
  EXPECT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "good");
}

TEST(skips_structurally_invalid_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [
             { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB", "alg": "RS256",
               "kid": "good" },
             { "kty": "RSA", "n": "", "e": "AQAB", "kid": "bad" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  EXPECT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);
  EXPECT_EQ(keys.value().find("bad"), nullptr);

  const auto *key{keys.value().find("good")};
  EXPECT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "good");
}

TEST(skips_non_object_entries) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ "not a key",
             { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB", "alg": "RS256",
               "kid": "good" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  EXPECT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);

  const auto *key{keys.value().find("good")};
  EXPECT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "good");
}

TEST(rejects_all_keys_unsupported) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "unsupported" } ] })")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(rejects_empty_key_array) {
  const auto document{sourcemeta::core::parse_json(R"({ "keys": [] })")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(rejects_missing_keys) {
  const auto document{sourcemeta::core::parse_json(R"({ "foo": "bar" })")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(rejects_non_array_keys) {
  const auto document{
      sourcemeta::core::parse_json(R"({ "keys": "not an array" })")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(rejects_non_object) {
  const auto document{sourcemeta::core::parse_json(R"("not an object")")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(find_returns_null_for_unknown_key_id) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB",
                       "alg": "RS256", "kid": "present" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  EXPECT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().find("absent"), nullptr);

  const auto *key{keys.value().find("present")};
  EXPECT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "present");
}

TEST(find_ignores_keys_without_key_id) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  EXPECT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);
  EXPECT_EQ(keys.value().find(""), nullptr);
  EXPECT_EQ(keys.value().find("anything"), nullptr);

  const auto &key{*keys.value().begin()};
  EXPECT_EQ(key.type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_FALSE(key.algorithm().has_value());
  EXPECT_FALSE(key.key_id().has_value());
}

TEST(iterates_in_order) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [
             { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB", "alg": "RS256",
               "kid": "rsa-1" },
             { "kty": "EC", "crv": "P-256",
               "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
               "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
               "alg": "ES256", "kid": "ec-1" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  EXPECT_TRUE(keys.has_value());

  auto iterator{keys.value().begin()};
  EXPECT_NE(iterator, keys.value().end());
  EXPECT_EQ(iterator->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(iterator->algorithm().has_value());
  EXPECT_EQ(iterator->algorithm().value(),
            sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(iterator->key_id().has_value());
  EXPECT_EQ(iterator->key_id().value(), "rsa-1");

  ++iterator;
  EXPECT_NE(iterator, keys.value().end());
  EXPECT_EQ(iterator->type(), sourcemeta::core::JWK::Type::EllipticCurve);
  EXPECT_EQ(iterator->curve(), "P-256");
  EXPECT_TRUE(iterator->algorithm().has_value());
  EXPECT_EQ(iterator->algorithm().value(),
            sourcemeta::core::JWSAlgorithm::ES256);
  EXPECT_TRUE(iterator->key_id().has_value());
  EXPECT_EQ(iterator->key_id().value(), "ec-1");

  ++iterator;
  EXPECT_EQ(iterator, keys.value().end());
}

TEST(from_accepts_rvalue) {
  auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
      R"(", "e": "AQAB",
                       "alg": "RS256", "kid": "rsa-1" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(std::move(document))};
  EXPECT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);

  const auto *key{keys.value().find("rsa-1")};
  EXPECT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "rsa-1");
}

TEST(constructor_throws_on_invalid_input) {
  const auto document{sourcemeta::core::parse_json(R"({ "keys": [] })")};
  try {
    sourcemeta::core::JWKS{document};
    FAIL();
  } catch (const sourcemeta::core::JWKSParseError &error) {
    EXPECT_STREQ(error.what(), "The input is not a valid JSON Web Key Set");
  }
}

TEST(owns_keys_after_source_destroyed) {
  std::optional<sourcemeta::core::JWKS> keys;
  {
    const auto document{sourcemeta::core::parse_json(
        R"({ "keys": [ { "kty": "RSA", "n": ")" JOSE_TEST_RSA_MODULUS
        R"(", "e": "AQAB",
                         "alg": "RS256", "kid": "scoped" } ] })")};
    keys = sourcemeta::core::JWKS::from(document);
  }

  EXPECT_TRUE(keys.has_value());
  const auto *key{keys.value().find("scoped")};
  EXPECT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "scoped");
}
