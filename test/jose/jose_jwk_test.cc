#include <gtest/gtest.h>

#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

#include <optional> // std::optional
#include <string>   // std::string
#include <utility>  // std::move

TEST(JOSE_JWK, rsa_public_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "RS256",
           "kid": "key-1" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key.value().modulus(), "test");
  EXPECT_EQ(key.value().exponent(), std::string("\x01\x00\x01", 3));
  ASSERT_TRUE(key.value().key_id().has_value());
  EXPECT_EQ(key.value().key_id().value(), "key-1");
  ASSERT_TRUE(key.value().algorithm().has_value());
  EXPECT_EQ(key.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::RS256);
}

TEST(JOSE_JWK, rsa_public_key_without_optional_metadata) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_FALSE(key.value().key_id().has_value());
  EXPECT_FALSE(key.value().algorithm().has_value());
}

TEST(JOSE_JWK, elliptic_curve_public_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "P-256",
           "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
           "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
           "use": "enc", "kid": "1" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWK::Type::EllipticCurve);
  EXPECT_EQ(key.value().curve(), "P-256");
  EXPECT_EQ(key.value().coordinate_x().size(), 32);
  EXPECT_EQ(key.value().coordinate_y().size(), 32);
  ASSERT_TRUE(key.value().key_id().has_value());
  EXPECT_EQ(key.value().key_id().value(), "1");
  EXPECT_FALSE(key.value().algorithm().has_value());
}

TEST(JOSE_JWK, unrecognized_algorithm_is_absent) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "HS256" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(key.value().algorithm().has_value());
}

TEST(JOSE_JWK, rejects_non_object) {
  const auto document{sourcemeta::core::parse_json(R"("not an object")")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_missing_key_type) {
  const auto document{
      sourcemeta::core::parse_json(R"({ "n": "dGVzdA", "e": "AQAB" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_unsupported_key_type) {
  const auto document{
      sourcemeta::core::parse_json(R"({ "kty": "oct", "k": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_rsa_private_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "d": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_elliptic_curve_private_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "P-256",
           "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
           "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
           "d": "870MB6gfuTJ4HtUnUvYMyJpr5eUZNP4Bk43bVdj3eAE" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

// The Ed25519 key is the public key from RFC 8037 Appendix A.2
TEST(JOSE_JWK, octet_key_pair_ed25519_public_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "OKP", "crv": "Ed25519",
           "x": "11qYAYKxCrfVS_7TyWQHOg7hcvPapiMlrwIaaPcHURo" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWK::Type::OctetKeyPair);
  EXPECT_EQ(key.value().curve(), "Ed25519");
  EXPECT_EQ(key.value().coordinate_x().size(), 32);
}

TEST(JOSE_JWK, octet_key_pair_ed448_public_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "OKP", "crv": "Ed448",
           "x": "E35kUtHOEUbcTPAayux0atDpqzE8jD1lGIdbrhR5I79Gm1bDz6JMUvrGk7zVusKM8FEDCWzJMjcA" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWK::Type::OctetKeyPair);
  EXPECT_EQ(key.value().curve(), "Ed448");
  EXPECT_EQ(key.value().coordinate_x().size(), 57);
}

TEST(JOSE_JWK, octet_key_pair_algorithm_eddsa) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "OKP", "crv": "Ed25519",
           "x": "11qYAYKxCrfVS_7TyWQHOg7hcvPapiMlrwIaaPcHURo",
           "alg": "EdDSA" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  ASSERT_TRUE(key.value().algorithm().has_value());
  EXPECT_EQ(key.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::EdDSA);
}

TEST(JOSE_JWK, rejects_octet_key_pair_private_key) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "OKP", "crv": "Ed25519",
           "x": "11qYAYKxCrfVS_7TyWQHOg7hcvPapiMlrwIaaPcHURo",
           "d": "nWGxne_9WmC6hEr0kuwsxERJxWl7MmkZcDusAxyuf2A" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_octet_key_pair_unsupported_curve) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "OKP", "crv": "X25519",
           "x": "11qYAYKxCrfVS_7TyWQHOg7hcvPapiMlrwIaaPcHURo" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_octet_key_pair_wrong_public_key_length) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "OKP", "crv": "Ed25519", "x": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_missing_rsa_modulus) {
  const auto document{
      sourcemeta::core::parse_json(R"({ "kty": "RSA", "e": "AQAB" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_invalid_base64url) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "not valid base64url", "e": "AQAB" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, constructor_throws_on_invalid_input) {
  const auto document{sourcemeta::core::parse_json(R"({ "kty": "oct" })")};
  EXPECT_THROW(sourcemeta::core::JWK{document},
               sourcemeta::core::JWKParseError);
}

TEST(JOSE_JWK, from_accepts_rvalue) {
  auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB" })")};
  const auto key{sourcemeta::core::JWK::from(std::move(document))};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWK::Type::RSA);
  EXPECT_EQ(key.value().modulus(), "test");
}

TEST(JOSE_JWK, constructor_accepts_rvalue) {
  auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "P-256",
           "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
           "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM" })")};
  const sourcemeta::core::JWK key{std::move(document)};
  EXPECT_EQ(key.type(), sourcemeta::core::JWK::Type::EllipticCurve);
  EXPECT_EQ(key.curve(), "P-256");
}

TEST(JOSE_JWK, owns_material_after_source_destroyed) {
  std::optional<sourcemeta::core::JWK> key;
  {
    const auto document{sourcemeta::core::parse_json(
        R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "kid": "scoped" })")};
    key = sourcemeta::core::JWK::from(document);
  }

  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().modulus(), "test");
  ASSERT_TRUE(key.value().key_id().has_value());
  EXPECT_EQ(key.value().key_id().value(), "scoped");
}

TEST(JOSE_JWK, rejects_rsa_private_key_prime_p) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "p": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_rsa_private_key_prime_q) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "q": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_rsa_private_key_exponent_dp) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "dp": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_rsa_private_key_exponent_dq) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "dq": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_rsa_private_key_coefficient_qi) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "qi": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_rsa_private_key_other_primes_oth) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "oth": [] })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_empty_rsa_modulus) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "", "e": "AQAB" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_empty_rsa_exponent) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_unknown_curve) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "secp256k1", "x": "dGVzdA",
           "y": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_ec_coordinate_wrong_length) {
  // "dGVzdA" decodes to 4 bytes, not the 32 a P-256 coordinate requires
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "P-256", "x": "dGVzdA", "y": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, accepts_ec_p384) {
  // 64 base64url characters decode to the 48 bytes a P-384 coordinate requires
  const std::string coordinate(64, 'A');
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign_assume_new("kty", sourcemeta::core::JSON{"EC"});
  document.assign_assume_new("crv", sourcemeta::core::JSON{"P-384"});
  document.assign_assume_new("x", sourcemeta::core::JSON{coordinate});
  document.assign_assume_new("y", sourcemeta::core::JSON{coordinate});
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().curve(), "P-384");
  EXPECT_EQ(key.value().coordinate_x().size(), 48);
  EXPECT_EQ(key.value().coordinate_y().size(), 48);
}

TEST(JOSE_JWK, accepts_ec_p521) {
  // 88 base64url characters decode to the 66 bytes a P-521 coordinate requires
  const std::string coordinate(88, 'A');
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign_assume_new("kty", sourcemeta::core::JSON{"EC"});
  document.assign_assume_new("crv", sourcemeta::core::JSON{"P-521"});
  document.assign_assume_new("x", sourcemeta::core::JSON{coordinate});
  document.assign_assume_new("y", sourcemeta::core::JSON{coordinate});
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().curve(), "P-521");
  EXPECT_EQ(key.value().coordinate_x().size(), 66);
  EXPECT_EQ(key.value().coordinate_y().size(), 66);
}

TEST(JOSE_JWK, rejects_missing_rsa_exponent) {
  const auto document{
      sourcemeta::core::parse_json(R"({ "kty": "RSA", "n": "dGVzdA" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_missing_ec_curve) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
           "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_missing_ec_coordinate_x) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "P-256",
           "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_missing_ec_coordinate_y) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "P-256",
           "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_non_string_key_type) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": 123, "n": "dGVzdA", "e": "AQAB" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_non_string_rsa_modulus) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": 123, "e": "AQAB" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_lowercase_key_type) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "rsa", "n": "dGVzdA", "e": "AQAB" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_invalid_base64url_exponent) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "not valid" })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_non_string_key_id) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "kid": 123 })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, rejects_non_string_algorithm) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": 123 })")};
  EXPECT_FALSE(sourcemeta::core::JWK::from(document).has_value());
}

TEST(JOSE_JWK, ignores_unknown_members) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "use": "sig",
           "key_ops": [ "verify" ], "x5t": "abc" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWK::Type::RSA);
}

TEST(JOSE_JWK, ignores_algorithm_inconsistent_with_key_type) {
  // An algorithm hint that contradicts the key type is not actionable, so it
  // is dropped while the otherwise valid key still parses
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "ES256" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(key.value().algorithm().has_value());
}

TEST(JOSE_JWK, tolerates_unsupported_algorithm) {
  // A valid but unsupported algorithm hint is advisory (RFC 7517 Section 4.4),
  // so it does not invalidate the key, it is simply not reported
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "RSA", "n": "dGVzdA", "e": "AQAB", "alg": "EdDSA" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(key.value().algorithm().has_value());
}

TEST(JOSE_JWK, stores_algorithm_matching_curve) {
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "P-256",
           "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
           "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
           "alg": "ES256" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  ASSERT_TRUE(key.value().algorithm().has_value());
  EXPECT_EQ(key.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::ES256);
}

TEST(JOSE_JWK, ignores_algorithm_not_matching_curve) {
  // ES512 is defined over P-521, so the hint is not actionable for a P-256
  // key and is dropped while the key still parses
  const auto document{sourcemeta::core::parse_json(
      R"({ "kty": "EC", "crv": "P-256",
           "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
           "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
           "alg": "ES512" })")};
  const auto key{sourcemeta::core::JWK::from(document)};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(key.value().algorithm().has_value());
}
