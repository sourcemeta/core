#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
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

static constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> ALLOWED_HS256{
    {sourcemeta::core::JWSAlgorithm::HS256}};

TEST(pairing_is_valid_for_at_most_one) {
  EXPECT_TRUE(sourcemeta::core::oidc_request_object_pairing_is_valid("", ""));
  EXPECT_TRUE(
      sourcemeta::core::oidc_request_object_pairing_is_valid("eyJ...", ""));
  EXPECT_TRUE(sourcemeta::core::oidc_request_object_pairing_is_valid(
      "", "https://client.example/request.jwt"));
  EXPECT_FALSE(sourcemeta::core::oidc_request_object_pairing_is_valid(
      "eyJ...", "https://client.example/request.jwt"));
}

TEST(build_and_verify_round_trip) {
  const auto parameters{sourcemeta::core::parse_json(
      R"JSON({
    "iss": "client",
    "aud": "https://op.example",
    "scope": "openid",
    "state": "xyz"
  })JSON")};
  const auto object{sourcemeta::core::oidc_build_request_object(
      parameters, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  EXPECT_TRUE(object.has_value());

  const auto token{sourcemeta::core::JWT::from(object.value())};
  EXPECT_TRUE(token.has_value());
  const auto verified{sourcemeta::core::oidc_verify_request_object(
      token.value(), oct_key_set(), ALLOWED_HS256, "client",
      "https://op.example")};
  EXPECT_TRUE(verified.has_value());
  EXPECT_EQ(verified.value().at("state").to_string(), "xyz");
}

TEST(verify_rejects_a_wrong_issuer) {
  const auto parameters{sourcemeta::core::parse_json(
      R"JSON({
    "iss": "attacker",
    "aud": "https://op.example",
    "scope": "openid"
  })JSON")};
  const auto object{sourcemeta::core::oidc_build_request_object(
      parameters, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(object.value())};
  EXPECT_TRUE(token.has_value());
  const auto verified{sourcemeta::core::oidc_verify_request_object(
      token.value(), oct_key_set(), ALLOWED_HS256, "client",
      "https://op.example")};
  EXPECT_FALSE(verified.has_value());
}

TEST(verify_rejects_a_wrong_audience) {
  const auto parameters{sourcemeta::core::parse_json(
      R"JSON({
    "iss": "client",
    "aud": "https://attacker.example",
    "scope": "openid"
  })JSON")};
  const auto object{sourcemeta::core::oidc_build_request_object(
      parameters, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(object.value())};
  EXPECT_TRUE(token.has_value());
  const auto verified{sourcemeta::core::oidc_verify_request_object(
      token.value(), oct_key_set(), ALLOWED_HS256, "client",
      "https://op.example")};
  EXPECT_FALSE(verified.has_value());
}

TEST(verify_accepts_a_missing_issuer) {
  // OpenID Connect Core 1.0 Section 6.1: iss is only a SHOULD, so its absence
  // is accepted when the present audience still addresses the provider
  const auto parameters{sourcemeta::core::parse_json(
      R"JSON({
    "aud": "https://op.example",
    "scope": "openid",
    "state": "xyz"
  })JSON")};
  const auto object{sourcemeta::core::oidc_build_request_object(
      parameters, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(object.value())};
  EXPECT_TRUE(token.has_value());
  const auto verified{sourcemeta::core::oidc_verify_request_object(
      token.value(), oct_key_set(), ALLOWED_HS256, "client",
      "https://op.example")};
  EXPECT_TRUE(verified.has_value());
  EXPECT_EQ(verified.value().at("state").to_string(), "xyz");
}

TEST(verify_rejects_a_missing_audience) {
  // OpenID Connect Core 1.0 Section 6.1: the aud binds the object to the
  // provider, so a signed object without it could be replayed to another
  // provider and is rejected even when the present issuer identifies the client
  const auto parameters{sourcemeta::core::parse_json(
      R"JSON({
    "iss": "client",
    "scope": "openid",
    "state": "xyz"
  })JSON")};
  const auto object{sourcemeta::core::oidc_build_request_object(
      parameters, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(object.value())};
  EXPECT_TRUE(token.has_value());
  const auto verified{sourcemeta::core::oidc_verify_request_object(
      token.value(), oct_key_set(), ALLOWED_HS256, "client",
      "https://op.example")};
  EXPECT_FALSE(verified.has_value());
}

TEST(verify_rejects_a_missing_issuer_and_audience) {
  const auto parameters{sourcemeta::core::parse_json(
      R"JSON({
    "scope": "openid",
    "state": "xyz"
  })JSON")};
  const auto object{sourcemeta::core::oidc_build_request_object(
      parameters, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(object.value())};
  EXPECT_TRUE(token.has_value());
  const auto verified{sourcemeta::core::oidc_verify_request_object(
      token.value(), oct_key_set(), ALLOWED_HS256, "client",
      "https://op.example")};
  EXPECT_FALSE(verified.has_value());
}

TEST(verify_rejects_an_unknown_kid) {
  const auto object{
      sourcemeta::core::jwt_sign(sourcemeta::core::parse_json(R"JSON({
    "alg": "HS256",
    "kid": "unknown"
  })JSON"),
                                 sourcemeta::core::parse_json(R"JSON({
    "iss": "client",
    "aud": "https://op.example",
    "scope": "openid"
  })JSON"),
                                 oct_private_key())};
  const auto token{sourcemeta::core::JWT::from(object.value())};
  EXPECT_TRUE(token.has_value());
  const auto verified{sourcemeta::core::oidc_verify_request_object(
      token.value(), oct_key_set(), ALLOWED_HS256, "client",
      "https://op.example")};
  EXPECT_FALSE(verified.has_value());
}

TEST(verify_rejects_an_algorithm_outside_the_allow_list) {
  const auto parameters{sourcemeta::core::parse_json(
      R"JSON({
    "iss": "client",
    "aud": "https://op.example",
    "scope": "openid"
  })JSON")};
  const auto object{sourcemeta::core::oidc_build_request_object(
      parameters, oct_private_key(), sourcemeta::core::JWSAlgorithm::HS256)};
  const auto token{sourcemeta::core::JWT::from(object.value())};
  EXPECT_TRUE(token.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> only_rs256{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto verified{sourcemeta::core::oidc_verify_request_object(
      token.value(), oct_key_set(), only_rs256, "client",
      "https://op.example")};
  EXPECT_FALSE(verified.has_value());
}
