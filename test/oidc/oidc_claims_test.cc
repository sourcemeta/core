#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static auto collect(const std::string_view scopes) -> std::vector<std::string> {
  std::vector<std::string> claims;
  sourcemeta::core::oidc_scope_to_claims(
      scopes,
      [&claims](std::string_view claim) { claims.emplace_back(claim); });
  return claims;
}

TEST(is_standard_claim_recognizes_registered_claims) {
  EXPECT_TRUE(sourcemeta::core::oidc_is_standard_claim("sub"));
  EXPECT_TRUE(sourcemeta::core::oidc_is_standard_claim("email"));
  EXPECT_TRUE(sourcemeta::core::oidc_is_standard_claim("address"));
  EXPECT_TRUE(sourcemeta::core::oidc_is_standard_claim("updated_at"));
  EXPECT_FALSE(sourcemeta::core::oidc_is_standard_claim("custom_claim"));
  EXPECT_FALSE(sourcemeta::core::oidc_is_standard_claim(""));
}

TEST(scope_to_claims_maps_openid_to_sub) {
  EXPECT_EQ(collect("openid"), (std::vector<std::string>{"sub"}));
}

TEST(scope_to_claims_maps_email) {
  EXPECT_EQ(collect("openid email"),
            (std::vector<std::string>{"sub", "email", "email_verified"}));
}

TEST(scope_to_claims_maps_profile_phone_address) {
  EXPECT_EQ(
      collect("openid profile phone address"),
      (std::vector<std::string>{
          "sub", "name", "family_name", "given_name", "middle_name", "nickname",
          "preferred_username", "profile", "picture", "website", "gender",
          "birthdate", "zoneinfo", "locale", "updated_at", "address",
          "phone_number", "phone_number_verified"}));
}

TEST(scope_to_claims_ignores_unknown_scopes) {
  EXPECT_EQ(collect("openid api:read"), (std::vector<std::string>{"sub"}));
}

TEST(build_claims_parameter_marks_essential_and_voluntary) {
  const std::array<sourcemeta::core::OIDCClaimRequest, 2> userinfo{
      {{.name = "email", .essential = true},
       {.name = "name", .essential = false}}};
  const std::array<sourcemeta::core::OIDCClaimRequest, 1> id_token{
      {{.name = "auth_time", .essential = true}}};
  const auto document{
      sourcemeta::core::oidc_build_claims_parameter(userinfo, id_token)};
  EXPECT_TRUE(document.defines("userinfo"));
  EXPECT_TRUE(document.defines("id_token"));
  EXPECT_TRUE(document.at("userinfo").at("email").at("essential").to_boolean());
  EXPECT_TRUE(document.at("userinfo").at("name").is_null());
  EXPECT_TRUE(
      document.at("id_token").at("auth_time").at("essential").to_boolean());
}

TEST(build_claims_parameter_omits_an_empty_target) {
  const std::array<sourcemeta::core::OIDCClaimRequest, 1> userinfo{
      {{.name = "email", .essential = false}}};
  const auto document{
      sourcemeta::core::oidc_build_claims_parameter(userinfo, {})};
  EXPECT_TRUE(document.defines("userinfo"));
  EXPECT_FALSE(document.defines("id_token"));
}

TEST(claims_parameter_requests_detects_a_requested_claim) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "auth_time": { "essential": true }
    },
    "userinfo": {
      "email": null
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_requests(
      claims, "id_token", "auth_time"));
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_requests(
      claims, "userinfo", "email"));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_requests(
      claims, "id_token", "email"));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_requests(
      claims, "userinfo", "phone_number"));
}

TEST(claims_parameter_is_essential_reads_the_flag) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "auth_time": { "essential": true }
    },
    "userinfo": {
      "email": null,
      "name": { "essential": false }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_is_essential(
      claims, "id_token", "auth_time"));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_is_essential(
      claims, "userinfo", "email"));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_is_essential(
      claims, "userinfo", "name"));
}
