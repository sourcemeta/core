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

static auto oct_key_set() -> sourcemeta::core::JWKS {
  auto keys{sourcemeta::core::JSON::make_array()};
  keys.push_back(sourcemeta::core::parse_json(OCT_JWK));
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("keys", std::move(keys));
  return sourcemeta::core::JWKS::from(std::move(document)).value();
}

static auto sign_userinfo(const std::string_view payload) -> std::string {
  return sourcemeta::core::jwt_sign(
             sourcemeta::core::parse_json(R"JSON({ "alg": "HS256" })JSON"),
             sourcemeta::core::parse_json(payload),
             sourcemeta::core::JWKPrivate::from(
                 sourcemeta::core::parse_json(OCT_JWK))
                 .value())
      .value();
}

static constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> ALLOWED_HS256{
    {sourcemeta::core::JWSAlgorithm::HS256}};

TEST(build_userinfo_ensures_the_subject) {
  const auto claims{
      sourcemeta::core::parse_json(R"JSON({ "email": "a@b.example" })JSON")};
  const auto document{sourcemeta::core::oidc_build_userinfo("user-1", claims)};
  EXPECT_TRUE(document.defines("sub"));
  EXPECT_EQ(document.at("sub").to_string(), "user-1");
  EXPECT_EQ(document.at("email").to_string(), "a@b.example");
}

TEST(build_userinfo_overrides_a_conflicting_subject) {
  const auto claims{
      sourcemeta::core::parse_json(R"JSON({ "sub": "attacker" })JSON")};
  const auto document{sourcemeta::core::oidc_build_userinfo("user-1", claims)};
  EXPECT_EQ(document.at("sub").to_string(), "user-1");
}

TEST(matches_subject_accepts_a_matching_response) {
  const auto userinfo{sourcemeta::core::parse_json(
      R"JSON({ "sub": "user-1", "email": "a@b.example" })JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oidc_userinfo_matches_subject(userinfo, "user-1"));
}

TEST(matches_subject_rejects_a_substituted_response) {
  const auto userinfo{
      sourcemeta::core::parse_json(R"JSON({ "sub": "attacker" })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oidc_userinfo_matches_subject(userinfo, "user-1"));
}

TEST(matches_subject_rejects_a_missing_subject) {
  const auto userinfo{
      sourcemeta::core::parse_json(R"JSON({ "email": "a@b.example" })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oidc_userinfo_matches_subject(userinfo, "user-1"));
}

TEST(verify_userinfo_accepts_a_valid_signed_response) {
  const auto compact{sign_userinfo(R"JSON({
    "sub": "user-1",
    "iss": "https://issuer.example",
    "aud": "client-id",
    "email": "a@b.example"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_TRUE(claims.has_value());
  EXPECT_EQ(claims.value().at("email").to_string(), "a@b.example");
}

TEST(verify_userinfo_rejects_a_missing_issuer) {
  const auto compact{sign_userinfo(R"JSON({
    "sub": "user-1",
    "aud": "client-id"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_rejects_a_missing_audience) {
  const auto compact{sign_userinfo(R"JSON({
    "sub": "user-1",
    "iss": "https://issuer.example"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_rejects_a_missing_issuer_and_audience) {
  const auto compact{
      sign_userinfo(R"JSON({ "sub": "user-1", "email": "a@b.example" })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_rejects_a_substituted_subject) {
  const auto compact{sign_userinfo(R"JSON({ "sub": "attacker" })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_rejects_an_unknown_kid) {
  // The key set carries no key identifier, so a response naming one is not
  // accepted on the strength of another key verifying its signature
  const auto compact{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(
          R"JSON({ "alg": "HS256", "kid": "unknown" })JSON"),
      sourcemeta::core::parse_json(R"JSON({ "sub": "user-1" })JSON"),
      sourcemeta::core::JWKPrivate::from(sourcemeta::core::parse_json(OCT_JWK))
          .value())};
  const auto token{sourcemeta::core::JWT::from(compact.value())};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_rejects_an_algorithm_outside_the_allow_list) {
  const auto compact{sign_userinfo(R"JSON({ "sub": "user-1" })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> only_rs256{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), only_rs256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_accepts_a_matching_issuer_and_audience) {
  const auto compact{sign_userinfo(R"JSON({
    "sub": "user-1",
    "iss": "https://issuer.example",
    "aud": "client-id"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_TRUE(claims.has_value());
}

TEST(verify_userinfo_accepts_an_audience_array_containing_the_client) {
  const auto compact{sign_userinfo(R"JSON({
    "sub": "user-1",
    "iss": "https://issuer.example",
    "aud": [ "other-client", "client-id" ]
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_TRUE(claims.has_value());
}

TEST(verify_userinfo_rejects_a_wrong_issuer) {
  const auto compact{sign_userinfo(R"JSON({
    "sub": "user-1",
    "iss": "https://attacker.example",
    "aud": "client-id"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_rejects_a_non_string_issuer) {
  const auto compact{sign_userinfo(R"JSON({
    "sub": "user-1",
    "iss": 42,
    "aud": "client-id"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_rejects_an_audience_for_another_client) {
  const auto compact{sign_userinfo(R"JSON({
    "sub": "user-1",
    "iss": "https://issuer.example",
    "aud": "other-client"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), ALLOWED_HS256, "user-1",
      "https://issuer.example", "client-id")};
  EXPECT_FALSE(claims.has_value());
}
