#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono
#include <cstdint>     // std::int64_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

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
  auto keys{sourcemeta::core::JSON::make_array()};
  keys.push_back(sourcemeta::core::parse_json(OCT_JWK));
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("keys", std::move(keys));
  return sourcemeta::core::JWKS::from(std::move(document)).value();
}

static auto sign_id_token(const sourcemeta::core::JSON &payload)
    -> std::string {
  return sourcemeta::core::jwt_sign(
             sourcemeta::core::parse_json(R"JSON({ "alg": "HS256" })JSON"),
             payload, oct_private_key())
      .value();
}

static auto sign_id_token(const std::string_view payload) -> std::string {
  return sign_id_token(sourcemeta::core::parse_json(payload));
}

static constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> ALLOWED_HS256{
    {sourcemeta::core::JWSAlgorithm::HS256}};

// A fixed reference time, 2023-11-14T22:13:20Z
static const auto REFERENCE_NOW{
    std::chrono::system_clock::from_time_t(1700000000)};

// The oldest whole second the clock can represent and a conversion still
// admits. The tick period differs across standard libraries, so the bound is
// derived rather than written out
static auto lowest_representable_second() -> std::int64_t {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::duration::min())
             .count() +
         2;
}

static auto id_token_claims() -> sourcemeta::core::JSON {
  auto payload{sourcemeta::core::JSON::make_object()};
  payload.assign("iss", sourcemeta::core::JSON{"https://issuer.example"});
  payload.assign("sub", sourcemeta::core::JSON{"user-1"});
  payload.assign("aud", sourcemeta::core::JSON{"client-id"});
  payload.assign("exp", sourcemeta::core::JSON{std::int64_t{2000000000}});
  return payload;
}

static auto id_token_issued_at(const std::int64_t issued_at)
    -> sourcemeta::core::JSON {
  auto payload{id_token_claims()};
  payload.assign("iat", sourcemeta::core::JSON{issued_at});
  return payload;
}

static auto id_token_authenticated_at(const std::int64_t authenticated_at)
    -> sourcemeta::core::JSON {
  auto payload{id_token_claims()};
  payload.assign("iat", sourcemeta::core::JSON{std::int64_t{1699996400}});
  payload.assign("auth_time", sourcemeta::core::JSON{authenticated_at});
  return payload;
}

TEST(mint_and_validate_round_trip) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_TRUE(identity.has_value());
  EXPECT_EQ(identity.value().subject, "user-1");
  EXPECT_EQ(identity.value().issuer, "https://issuer.example");
}

TEST(mint_rejects_a_code_binding_under_eddsa) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
  claims.nonce = "n-abc";
  claims.code = "the-authorization-code";
  // EdDSA has no defined c_hash digest, so minting fails rather than emitting a
  // token missing the REQUIRED binding
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::EdDSA)};
  EXPECT_FALSE(compact.has_value());
}

TEST(mint_rejects_an_access_token_binding_under_eddsa) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
  claims.nonce = "n-abc";
  claims.access_token = "the-access-token";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::EdDSA)};
  EXPECT_FALSE(compact.has_value());
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW + std::chrono::hours{2})};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://attacker.example",
      "client-id", REFERENCE_NOW)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "other-client", REFERENCE_NOW)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_TRUE(identity.has_value());
  EXPECT_EQ(identity.value().subject, "user-1");
}

TEST(validate_rejects_an_untrusted_additional_audience) {
  // With the default empty trusted set every audience beyond the client is
  // untrusted, so a multi-audience token is rejected even with a matching azp
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_accepts_a_trusted_additional_audience_with_matching_azp) {
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
  const std::array<std::string_view, 1> trusted{{"other"}};
  sourcemeta::core::OIDCValidationOptions options;
  options.trusted_audiences = trusted;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_TRUE(identity.has_value());
  EXPECT_EQ(identity.value().subject, "user-1");
}

TEST(validate_rejects_a_trusted_additional_audience_without_azp) {
  // The azp requirement for multi-audience tokens is retained even when every
  // additional audience is trusted
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": [ "client-id", "other" ],
    "iat": 1699996400,
    "exp": 2000000000
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const std::array<std::string_view, 1> trusted{{"other"}};
  sourcemeta::core::OIDCValidationOptions options;
  options.trusted_audiences = trusted;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_accepts_a_single_audience_under_an_empty_trusted_set) {
  // A single-string audience carries no additional audience, so the strict
  // default trusted set does not affect it
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(rejected.has_value());

  const std::array<std::string_view, 2> allowed_classes{{"silver", "gold"}};
  options.acceptable_authentication_context_classes = allowed_classes;
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(rejected.has_value());

  options.maximum_authentication_age = std::chrono::hours{2};
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_enforces_the_maximum_issued_at_age) {
  const auto compact{sign_id_token(id_token_issued_at(1699996400))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_issued_at_age = std::chrono::minutes{5};
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(rejected.has_value());

  options.maximum_issued_at_age = std::chrono::hours{2};
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_TRUE(accepted.has_value());
}

TEST(validate_accepts_an_issued_at_exactly_at_the_age_boundary) {
  const auto compact{sign_id_token(id_token_issued_at(1699996400))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_issued_at_age = std::chrono::seconds{3600};
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_TRUE(accepted.has_value());

  options.maximum_issued_at_age = std::chrono::seconds{3599};
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(rejected.has_value());
}

TEST(validate_rejects_an_issued_at_near_the_representable_bound) {
  // Subtracting a claim this far in the past from the server clock
  // overflows the tick type, so the window is applied to the clock instead
  // and the stale token is still refused
  const auto compact{
      sign_id_token(id_token_issued_at(lowest_representable_second()))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_issued_at_age = std::chrono::minutes{5};
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(rejected.has_value());
}

TEST(validate_rejects_an_auth_time_near_the_representable_bound) {
  const auto compact{
      sign_id_token(id_token_authenticated_at(lowest_representable_second()))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_authentication_age = std::chrono::minutes{5};
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(rejected.has_value());
}

TEST(validate_accepts_a_recent_issued_at_under_an_unbounded_age) {
  // A window wider than the clock saturates to the oldest representable
  // instant rather than wrapping, so nothing is rejected for being too old
  const auto compact{sign_id_token(id_token_issued_at(1699996400))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_issued_at_age = std::chrono::seconds::max();
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_TRUE(accepted.has_value());
}

TEST(validate_rejects_an_ancient_issued_at_under_an_unbounded_age) {
  // Even the widest window the clock can express is measured back from now, so
  // its oldest instant is later than the oldest the clock can represent and a
  // claim at that floor stays too old rather than wrapping into acceptance
  const auto compact{
      sign_id_token(id_token_issued_at(lowest_representable_second()))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_issued_at_age = std::chrono::seconds::max();
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(rejected.has_value());
}

TEST(validate_rejects_a_stale_issued_at_under_a_negative_age) {
  // A window that runs backwards cannot admit anything issued before now,
  // so it fails closed rather than wrapping into an unbounded window
  const auto compact{sign_id_token(id_token_issued_at(1699996400))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_issued_at_age = std::chrono::seconds::min();
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(rejected.has_value());
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
      "client-id", REFERENCE_NOW)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_enforces_the_access_token_hash) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
  claims.access_token = "the-access-token";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());

  sourcemeta::core::OIDCValidationOptions matching;
  matching.access_token = "the-access-token";
  const auto accepted{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, matching)};
  EXPECT_TRUE(accepted.has_value());

  sourcemeta::core::OIDCValidationOptions mismatched;
  mismatched.access_token = "the-wrong-access-token";
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, mismatched)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_accepts_a_required_access_token_hash_when_present) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
  claims.access_token = "the-access-token";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());

  sourcemeta::core::OIDCValidationOptions options;
  options.access_token = "the-access-token";
  options.require_access_token_hash = true;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_TRUE(identity.has_value());
}

TEST(validate_rejects_a_required_access_token_hash_without_the_token) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
  claims.access_token = "the-access-token";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().payload().defines("at_hash"));

  // The claim is present, but with no access token to verify it against a
  // required binding cannot be accepted unverified
  sourcemeta::core::OIDCValidationOptions options;
  options.require_access_token_hash = true;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_enforces_the_code_hash) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
  claims.code = "the-authorization-code";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());

  sourcemeta::core::OIDCValidationOptions mismatched;
  mismatched.code = "the-wrong-code";
  const auto rejected{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, mismatched)};
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
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_rejects_a_required_code_hash_without_the_code) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
  claims.code = "the-authorization-code";
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().payload().defines("c_hash"));

  // The claim is present, but with no code to verify it against a required
  // binding cannot be accepted unverified
  sourcemeta::core::OIDCValidationOptions options;
  options.require_code_hash = true;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
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
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
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

TEST(parse_id_token_rejects_a_non_object) {
  const sourcemeta::core::JSON response{"not an object"};
  EXPECT_FALSE(sourcemeta::core::oidc_parse_id_token(response).has_value());
}

TEST(parse_id_token_rejects_a_missing_member) {
  const auto response{sourcemeta::core::parse_json(R"JSON({
    "access_token": "abc"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_parse_id_token(response).has_value());
}

TEST(mint_emits_optional_claims) {
  sourcemeta::core::OIDCIdTokenClaims claims;
  claims.issuer = "https://issuer.example";
  claims.subject = "user-1";
  claims.audience = "client-id";
  claims.issued_at = REFERENCE_NOW;
  claims.expiration = REFERENCE_NOW + std::chrono::hours{1};
  claims.authorized_party = "client-id";
  claims.authentication_context_class = "urn:mace:incommon:iap:silver";
  claims.authentication_time = REFERENCE_NOW - std::chrono::minutes{5};
  const auto compact{sourcemeta::core::oidc_mint_id_token(
      claims, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  EXPECT_TRUE(compact.has_value());

  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().payload().at("iss").to_string(),
            "https://issuer.example");
  EXPECT_EQ(token.value().payload().at("sub").to_string(), "user-1");
  EXPECT_EQ(token.value().payload().at("aud").to_string(), "client-id");
  EXPECT_EQ(token.value().payload().at("iat").to_integer(), 1700000000);
  EXPECT_EQ(token.value().payload().at("exp").to_integer(), 1700003600);
  EXPECT_EQ(token.value().payload().at("azp").to_string(), "client-id");
  EXPECT_EQ(token.value().payload().at("acr").to_string(),
            "urn:mace:incommon:iap:silver");
  EXPECT_EQ(token.value().payload().at("auth_time").to_integer(), 1699999700);
}

TEST(validate_rejects_a_missing_acr_when_a_set_was_requested) {
  const auto compact{sign_id_token(id_token_issued_at(1699996400))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  const std::array<std::string_view, 1> classes{{"urn:example:gold"}};
  options.acceptable_authentication_context_classes = classes;
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(identity.has_value());
}

TEST(validate_treats_an_overflowing_auth_time_as_absent) {
  const auto compact{sign_id_token(R"JSON({
    "iss": "https://issuer.example",
    "sub": "user-1",
    "aud": "client-id",
    "iat": 1699996400,
    "exp": 2000000000,
    "auth_time": 1e300
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  sourcemeta::core::OIDCValidationOptions options;
  options.maximum_authentication_age = std::chrono::seconds{60};
  const auto identity{sourcemeta::core::oidc_validate_id_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, options)};
  EXPECT_FALSE(identity.has_value());
}
