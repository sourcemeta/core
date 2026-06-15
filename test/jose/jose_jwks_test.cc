#include <gtest/gtest.h>

#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

#include <optional> // std::optional
#include <string>   // std::string
#include <utility>  // std::move

TEST(JOSE_JWKS, parses_single_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "RSA", "n": "dGVzdA", "e": "AQAB",
                       "alg": "RS256", "kid": "rsa-1" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);
  EXPECT_FALSE(keys.value().empty());

  const auto *key{keys.value().find("rsa-1")};
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key->modulus(), "test");
  EXPECT_EQ(key->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "rsa-1");
}

TEST(JOSE_JWKS, parses_multiple_keys) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [
             { "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "RS256",
               "kid": "rsa-1" },
             { "kty": "EC", "crv": "P-256",
               "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
               "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
               "alg": "ES256", "kid": "ec-1" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 2);

  const auto *rsa{keys.value().find("rsa-1")};
  ASSERT_NE(rsa, nullptr);
  EXPECT_EQ(rsa->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(rsa->modulus(), "test");
  EXPECT_EQ(rsa->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(rsa->algorithm().has_value());
  EXPECT_EQ(rsa->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(rsa->key_id().has_value());
  EXPECT_EQ(rsa->key_id().value(), "rsa-1");

  const auto *ec{keys.value().find("ec-1")};
  ASSERT_NE(ec, nullptr);
  EXPECT_EQ(ec->type(), sourcemeta::core::JWK::Type::EllipticCurve);
  EXPECT_EQ(ec->curve(), "P-256");
  EXPECT_EQ(ec->coordinate_x().size(), 32);
  EXPECT_EQ(ec->coordinate_y().size(), 32);
  ASSERT_TRUE(ec->algorithm().has_value());
  EXPECT_EQ(ec->algorithm().value(), sourcemeta::core::JWSAlgorithm::ES256);
  ASSERT_TRUE(ec->key_id().has_value());
  EXPECT_EQ(ec->key_id().value(), "ec-1");
}

TEST(JOSE_JWKS, skips_unsupported_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [
             { "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "RS256",
               "kid": "good" },
             { "kty": "oct", "k": "dGVzdA" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);

  const auto *key{keys.value().find("good")};
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key->modulus(), "test");
  EXPECT_EQ(key->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "good");
}

TEST(JOSE_JWKS, skips_structurally_invalid_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [
             { "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "RS256",
               "kid": "good" },
             { "kty": "RSA", "n": "", "e": "AQAB", "kid": "bad" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);
  EXPECT_EQ(keys.value().find("bad"), nullptr);

  const auto *key{keys.value().find("good")};
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key->modulus(), "test");
  EXPECT_EQ(key->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "good");
}

TEST(JOSE_JWKS, skips_non_object_entries) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ "not a key",
             { "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "RS256",
               "kid": "good" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);

  const auto *key{keys.value().find("good")};
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key->modulus(), "test");
  EXPECT_EQ(key->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "good");
}

TEST(JOSE_JWKS, rejects_all_keys_unsupported) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "oct", "k": "dGVzdA" } ] })")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(JOSE_JWKS, rejects_empty_key_array) {
  const auto document{sourcemeta::core::parse_json(R"({ "keys": [] })")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(JOSE_JWKS, rejects_missing_keys) {
  const auto document{sourcemeta::core::parse_json(R"({ "foo": "bar" })")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(JOSE_JWKS, rejects_non_array_keys) {
  const auto document{
      sourcemeta::core::parse_json(R"({ "keys": "not an array" })")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(JOSE_JWKS, rejects_non_object) {
  const auto document{sourcemeta::core::parse_json(R"("not an object")")};
  EXPECT_FALSE(sourcemeta::core::JWKS::from(document).has_value());
}

TEST(JOSE_JWKS, find_returns_null_for_unknown_key_id) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "RSA", "n": "dGVzdA", "e": "AQAB",
                       "alg": "RS256", "kid": "present" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().find("absent"), nullptr);

  const auto *key{keys.value().find("present")};
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key->modulus(), "test");
  EXPECT_EQ(key->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "present");
}

TEST(JOSE_JWKS, find_ignores_keys_without_key_id) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "RSA", "n": "dGVzdA", "e": "AQAB" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);
  EXPECT_EQ(keys.value().find(""), nullptr);
  EXPECT_EQ(keys.value().find("anything"), nullptr);

  const auto &key{*keys.value().begin()};
  EXPECT_EQ(key.type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key.modulus(), "test");
  EXPECT_EQ(key.exponent(), std::string("\x01\x00\x01", 3));
  EXPECT_FALSE(key.algorithm().has_value());
  EXPECT_FALSE(key.key_id().has_value());
}

TEST(JOSE_JWKS, iterates_in_order) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "keys": [
             { "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "RS256",
               "kid": "rsa-1" },
             { "kty": "EC", "crv": "P-256",
               "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
               "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
               "alg": "ES256", "kid": "ec-1" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(document)};
  ASSERT_TRUE(keys.has_value());

  auto iterator{keys.value().begin()};
  ASSERT_NE(iterator, keys.value().end());
  EXPECT_EQ(iterator->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(iterator->modulus(), "test");
  EXPECT_EQ(iterator->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(iterator->algorithm().has_value());
  EXPECT_EQ(iterator->algorithm().value(),
            sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(iterator->key_id().has_value());
  EXPECT_EQ(iterator->key_id().value(), "rsa-1");

  ++iterator;
  ASSERT_NE(iterator, keys.value().end());
  EXPECT_EQ(iterator->type(), sourcemeta::core::JWK::Type::EllipticCurve);
  EXPECT_EQ(iterator->curve(), "P-256");
  EXPECT_EQ(iterator->coordinate_x().size(), 32);
  EXPECT_EQ(iterator->coordinate_y().size(), 32);
  ASSERT_TRUE(iterator->algorithm().has_value());
  EXPECT_EQ(iterator->algorithm().value(),
            sourcemeta::core::JWSAlgorithm::ES256);
  ASSERT_TRUE(iterator->key_id().has_value());
  EXPECT_EQ(iterator->key_id().value(), "ec-1");

  ++iterator;
  EXPECT_EQ(iterator, keys.value().end());
}

TEST(JOSE_JWKS, from_accepts_rvalue) {
  auto document{sourcemeta::core::parse_json(
      R"({ "keys": [ { "kty": "RSA", "n": "dGVzdA", "e": "AQAB",
                       "alg": "RS256", "kid": "rsa-1" } ] })")};
  const auto keys{sourcemeta::core::JWKS::from(std::move(document))};
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(keys.value().size(), 1);

  const auto *key{keys.value().find("rsa-1")};
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key->modulus(), "test");
  EXPECT_EQ(key->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "rsa-1");
}

TEST(JOSE_JWKS, constructor_throws_on_invalid_input) {
  const auto document{sourcemeta::core::parse_json(R"({ "keys": [] })")};
  EXPECT_THROW(sourcemeta::core::JWKS{document},
               sourcemeta::core::JWKSParseError);
}

TEST(JOSE_JWKS, owns_keys_after_source_destroyed) {
  std::optional<sourcemeta::core::JWKS> keys;
  {
    const auto document{sourcemeta::core::parse_json(
        R"({ "keys": [ { "kty": "RSA", "n": "dGVzdA", "e": "AQAB",
                         "alg": "RS256", "kid": "scoped" } ] })")};
    keys = sourcemeta::core::JWKS::from(document);
  }

  ASSERT_TRUE(keys.has_value());
  const auto *key{keys.value().find("scoped")};
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key->modulus(), "test");
  EXPECT_EQ(key->exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(key->algorithm().has_value());
  EXPECT_EQ(key->algorithm().value(), sourcemeta::core::JWSAlgorithm::RS256);
  ASSERT_TRUE(key->key_id().has_value());
  EXPECT_EQ(key->key_id().value(), "scoped");
}
