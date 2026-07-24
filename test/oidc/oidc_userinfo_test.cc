#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view OCT_JWK{
    R"JSON({"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})JSON"};

static auto oct_key_set() -> sourcemeta::core::JWKS {
  return sourcemeta::core::JWKS::from(
             sourcemeta::core::parse_json(std::string{R"({"keys":[)"} +
                                          std::string{OCT_JWK} + R"(]})"))
      .value();
}

static auto sign_userinfo(const std::string_view payload) -> std::string {
  return sourcemeta::core::jwt_sign(
             sourcemeta::core::parse_json(R"({"alg":"HS256"})"),
             sourcemeta::core::parse_json(payload),
             sourcemeta::core::JWKPrivate::from(
                 sourcemeta::core::parse_json(OCT_JWK))
                 .value())
      .value();
}

static constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> allowed_hs256{
    {sourcemeta::core::JWSAlgorithm::HS256}};

TEST(build_userinfo_ensures_the_subject) {
  const auto claims{
      sourcemeta::core::parse_json(R"JSON({"email":"a@b.example"})JSON")};
  const auto document{sourcemeta::core::oidc_build_userinfo("user-1", claims)};
  EXPECT_TRUE(document.defines("sub"));
  EXPECT_EQ(document.at("sub").to_string(), "user-1");
  EXPECT_EQ(document.at("email").to_string(), "a@b.example");
}

TEST(build_userinfo_overrides_a_conflicting_subject) {
  const auto claims{
      sourcemeta::core::parse_json(R"JSON({"sub":"attacker"})JSON")};
  const auto document{sourcemeta::core::oidc_build_userinfo("user-1", claims)};
  EXPECT_EQ(document.at("sub").to_string(), "user-1");
}

TEST(matches_subject_accepts_a_matching_response) {
  const auto userinfo{sourcemeta::core::parse_json(
      R"JSON({"sub":"user-1","email":"a@b.example"})JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oidc_userinfo_matches_subject(userinfo, "user-1"));
}

TEST(matches_subject_rejects_a_substituted_response) {
  const auto userinfo{
      sourcemeta::core::parse_json(R"JSON({"sub":"attacker"})JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oidc_userinfo_matches_subject(userinfo, "user-1"));
}

TEST(matches_subject_rejects_a_missing_subject) {
  const auto userinfo{
      sourcemeta::core::parse_json(R"JSON({"email":"a@b.example"})JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oidc_userinfo_matches_subject(userinfo, "user-1"));
}

TEST(verify_userinfo_accepts_a_valid_signed_response) {
  const auto compact{
      sign_userinfo(R"({"sub":"user-1","email":"a@b.example"})")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), allowed_hs256, "user-1")};
  EXPECT_TRUE(claims.has_value());
  EXPECT_EQ(claims.value().at("email").to_string(), "a@b.example");
}

TEST(verify_userinfo_rejects_a_substituted_subject) {
  const auto compact{sign_userinfo(R"({"sub":"attacker"})")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), allowed_hs256, "user-1")};
  EXPECT_FALSE(claims.has_value());
}

TEST(verify_userinfo_rejects_an_algorithm_outside_the_allow_list) {
  const auto compact{sign_userinfo(R"({"sub":"user-1"})")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> only_rs256{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto claims{sourcemeta::core::oidc_verify_userinfo(
      token.value(), oct_key_set(), only_rs256, "user-1")};
  EXPECT_FALSE(claims.has_value());
}
