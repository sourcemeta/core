#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view OCT_JWK{R"JSON({
  "kty": "oct",
  "k": "AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"
})JSON"};

static auto oct_private_key() -> sourcemeta::core::JWKPrivate {
  return sourcemeta::core::JWKPrivate::from(
             sourcemeta::core::parse_json(OCT_JWK))
      .value();
}

static auto oct_key_set() -> sourcemeta::core::JWKS {
  return sourcemeta::core::JWKS::from(
             sourcemeta::core::parse_json(std::string{R"({ "keys": [ )"} +
                                          std::string{OCT_JWK} + R"( ] })"))
      .value();
}

static auto sign_id_token(const std::string_view payload) -> std::string {
  return sourcemeta::core::jwt_sign(
             sourcemeta::core::parse_json(R"JSON({ "alg": "HS256" })JSON"),
             sourcemeta::core::parse_json(payload), oct_private_key())
      .value();
}

static constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> allowed_hs256{
    {sourcemeta::core::JWSAlgorithm::HS256}};

// A fixed reference time, 2023-11-14T22:13:20Z
static const auto reference_now{
    std::chrono::system_clock::from_time_t(1700000000)};

TEST(mint_and_validate_round_trip) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = reference_now;
  claims.expiration = reference_now + std::chrono::hours{1};
  claims.nonce = "n-abc";
  claims.access_token = "the-access-token";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  EXPECT_TRUE(compact.has_value());

  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().payload().defines("at_hash"));

  sourcemeta::core::OIDCValidationOptions options;
  options.nonce = "n-abc";
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_TRUE(identity.has_value());
  EXPECT_EQ(identity.value().subject, "user-1");
  EXPECT_EQ(identity.value().issuer, "https://issuer.example");
}

TEST(validate_rejects_an_expired_token) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 1700000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now + std::chrono::hours{2})};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_wrong_issuer) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://attacker.example",
      "client-id", reference_now)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_wrong_audience) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "other-client", reference_now)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_missing_subject) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_missing_iat) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_malformed_audience_array) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": [ "client-id", 123 ],
    "azp": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_mismatched_nonce) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000,
    "nonce": "n-abc"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.nonce = "n-wrong";
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_missing_nonce_when_one_was_sent) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.nonce = "n-abc";
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_echoes_a_matching_nonce) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000,
    "nonce": "n-abc"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.nonce = "n-abc";
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_TRUE(identity.has_value());
  EXPECT_EQ(identity.value().subject, "user-1");
}

TEST(validate_accepts_a_multi_audience_token_with_matching_azp) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": [ "client-id", "other" ],
    "azp": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now)};
  EXPECT_TRUE(identity.has_value());
  EXPECT_EQ(identity.value().subject, "user-1");
}

TEST(validate_rejects_a_multi_audience_token_without_azp) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": [ "client-id", "other" ],
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_mismatched_azp) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "azp": "other",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_enforces_an_acceptable_acr) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "acr": "silver",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const std::array<std::string_view, 1> acceptable{{"gold"}};
  sourcemeta::core::OIDCValidationOptions options;
  options.acceptable_authentication_context_classes = acceptable;
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_FALSE(rejected.has_value());

  const std::array<std::string_view, 2> allowed_classes{{"silver", "gold"}};
  options.acceptable_authentication_context_classes = allowed_classes;
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_TRUE(accepted.has_value());
  EXPECT_EQ(accepted.value().authentication_context_class.value(), "silver");
}

TEST(validate_enforces_the_maximum_authentication_age) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "auth_time": 1699996400,
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_authentication_age = std::chrono::minutes{5};
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_FALSE(rejected.has_value());

  options.maximum_authentication_age = std::chrono::hours{2};
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_TRUE(accepted.has_value());
}

TEST(validate_accepts_a_fractional_auth_time) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "auth_time": 1699996400.5,
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_authentication_age = std::chrono::hours{2};
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_TRUE(accepted.has_value());
}

TEST(validate_rejects_a_future_auth_time) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "auth_time": 1900000000,
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_authentication_age = std::chrono::hours{2};
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_an_algorithm_outside_the_allow_list) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> only_rs256{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), only_rs256, "https://issuer.example",
      "client-id", reference_now)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_enforces_the_access_token_hash) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = reference_now;
  claims.expiration = reference_now + std::chrono::hours{1};
  claims.access_token = "the-access-token";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());

  sourcemeta::core::OIDCValidationOptions matching;
  matching.access_token = "the-access-token";
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, matching)};
  EXPECT_TRUE(accepted.has_value());

  sourcemeta::core::OIDCValidationOptions mismatched;
  mismatched.access_token = "the-wrong-access-token";
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, mismatched)};
  EXPECT_FALSE(rejected.has_value());
}

TEST(validate_requires_the_access_token_hash_when_configured) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.access_token = "the-access-token";
  options.require_access_token_hash = true;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_accepts_a_required_access_token_hash_when_present) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = reference_now;
  claims.expiration = reference_now + std::chrono::hours{1};
  claims.access_token = "the-access-token";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());

  sourcemeta::core::OIDCValidationOptions options;
  options.access_token = "the-access-token";
  options.require_access_token_hash = true;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_TRUE(identity.has_value());
}

TEST(validate_enforces_the_code_hash) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = reference_now;
  claims.expiration = reference_now + std::chrono::hours{1};
  claims.code = "the-authorization-code";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());

  sourcemeta::core::OIDCValidationOptions mismatched;
  mismatched.code = "the-wrong-code";
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, mismatched)};
  EXPECT_FALSE(rejected.has_value());
}

TEST(validate_requires_the_code_hash_when_configured) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.code = "the-authorization-code";
  options.require_code_hash = true;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(parse_id_token_extracts_the_member) {
  const auto response{sourcemeta::core::parse_json(R"JSON({
    "access_token": "at",
    "token_type": "Bearer",
    "id_token": "eyJ.aaa.bbb"
  })JSON")};
  const auto id_token{sourcemeta::core::oidc_parse_id_token(response)};
  EXPECT_TRUE(id_token.has_value());
  EXPECT_EQ(id_token.value(), "eyJ.aaa.bbb");
}

TEST(parse_id_token_rejects_an_absent_member) {
  const auto response{sourcemeta::core::parse_json(R"JSON({
    "access_token": "at",
    "token_type": "Bearer"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_parse_id_token(response).has_value());
}

TEST(parse_id_token_rejects_a_non_string_member) {
  const auto response{sourcemeta::core::parse_json(R"JSON({
    "id_token": 123
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_parse_id_token(response).has_value());
}

TEST(mint_embeds_a_code_hash) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = reference_now;
  claims.expiration = reference_now + std::chrono::hours{1};
  claims.code = "the-authorization-code";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  EXPECT_TRUE(compact.has_value());
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().payload().defines("c_hash"));
  EXPECT_TRUE(sourcemeta::core::oidc_verify_token_hash(
      "the-authorization-code", sourcemeta::core::JWSAlgorithm::HS256,
      token.value().payload().at("c_hash").to_string()));
}
