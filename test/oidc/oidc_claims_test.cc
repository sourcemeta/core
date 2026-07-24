#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static auto has(const std::vector<std::string> &values,
                const std::string_view target) -> bool {
  for (const auto &value : values) {
    if (value == target) {
      return true;
    }
  }

  return false;
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
  std::vector<std::string> claims;
  sourcemeta::core::oidc_scope_to_claims(
      "openid",
      [&claims](std::string_view claim) { claims.emplace_back(claim); });
  EXPECT_EQ(claims.size(), 1);
  EXPECT_TRUE(has(claims, "sub"));
}

TEST(scope_to_claims_maps_email) {
  std::vector<std::string> claims;
  sourcemeta::core::oidc_scope_to_claims(
      "openid email",
      [&claims](std::string_view claim) { claims.emplace_back(claim); });
  EXPECT_TRUE(has(claims, "sub"));
  EXPECT_TRUE(has(claims, "email"));
  EXPECT_TRUE(has(claims, "email_verified"));
  EXPECT_FALSE(has(claims, "name"));
}

TEST(scope_to_claims_maps_profile_phone_address) {
  std::vector<std::string> claims;
  sourcemeta::core::oidc_scope_to_claims(
      "openid profile phone address",
      [&claims](std::string_view claim) { claims.emplace_back(claim); });
  EXPECT_TRUE(has(claims, "name"));
  EXPECT_TRUE(has(claims, "given_name"));
  EXPECT_TRUE(has(claims, "updated_at"));
  EXPECT_TRUE(has(claims, "phone_number"));
  EXPECT_TRUE(has(claims, "phone_number_verified"));
  EXPECT_TRUE(has(claims, "address"));
}

TEST(scope_to_claims_ignores_unknown_scopes) {
  std::vector<std::string> claims;
  sourcemeta::core::oidc_scope_to_claims(
      "openid api:read",
      [&claims](std::string_view claim) { claims.emplace_back(claim); });
  EXPECT_EQ(claims.size(), 1);
  EXPECT_TRUE(has(claims, "sub"));
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
  const auto claims{sourcemeta::core::parse_json(
      R"JSON({"id_token":{"auth_time":{"essential":true}},"userinfo":{"email":null}})JSON")};
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
  const auto claims{sourcemeta::core::parse_json(
      R"JSON({"id_token":{"auth_time":{"essential":true}},"userinfo":{"email":null,"name":{"essential":false}}})JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_is_essential(
      claims, "id_token", "auth_time"));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_is_essential(
      claims, "userinfo", "email"));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_is_essential(
      claims, "userinfo", "name"));
}
