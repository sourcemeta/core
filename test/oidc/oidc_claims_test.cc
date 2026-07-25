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

TEST(claims_parameter_requests_rejects_a_malformed_entry) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "userinfo": {
      "email": "not-an-object-or-null"
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_requests(
      claims, "userinfo", "email"));
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

TEST(build_claims_parameter_emits_value_and_values) {
  const sourcemeta::core::JSON acr_value{"urn:mace:incommon:iap:silver"};
  const std::array<sourcemeta::core::JSON, 2> amr_values{
      {sourcemeta::core::JSON{"pwd"}, sourcemeta::core::JSON{"otp"}}};
  const std::array<sourcemeta::core::OIDCClaimRequest, 2> id_token{
      {{.name = "acr", .essential = true, .value = &acr_value},
       {.name = "amr", .values = amr_values}}};
  const auto document{
      sourcemeta::core::oidc_build_claims_parameter({}, id_token)};
  EXPECT_TRUE(document.at("id_token").at("acr").at("essential").to_boolean());
  EXPECT_EQ(document.at("id_token").at("acr").at("value").to_string(),
            "urn:mace:incommon:iap:silver");
  EXPECT_TRUE(document.at("id_token").at("amr").at("values").is_array());
  EXPECT_EQ(document.at("id_token").at("amr").at("values").size(), 2);
  EXPECT_EQ(document.at("id_token").at("amr").at("values").at(0).to_string(),
            "pwd");
  EXPECT_EQ(document.at("id_token").at("amr").at("values").at(1).to_string(),
            "otp");
}

TEST(claims_parameter_value_reads_the_requested_value) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "acr": { "value": "urn:mace:silver" }
    }
  })JSON")};
  const auto *value{
      sourcemeta::core::oidc_claims_parameter_value(claims, "id_token", "acr")};
  EXPECT_TRUE(value != nullptr);
  EXPECT_EQ(value->to_string(), "urn:mace:silver");
  EXPECT_EQ(
      sourcemeta::core::oidc_claims_parameter_value(claims, "id_token", "amr"),
      nullptr);
}

TEST(claims_parameter_accepts_an_exact_value) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "acr": { "value": "gold" }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"gold"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"silver"}));
}

TEST(claims_parameter_accepts_one_of_values) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "acr": { "values": [ "gold", "silver" ] }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"silver"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"bronze"}));
}

TEST(claims_parameter_accepts_any_value_when_unconstrained) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "acr": null,
      "auth_time": { "essential": true }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"anything"}));
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "auth_time", sourcemeta::core::JSON{123}));
}

TEST(claims_parameter_accepts_rejects_an_unrequested_claim) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "acr": null
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "amr", sourcemeta::core::JSON{"x"}));
}

TEST(claims_parameter_accepts_rejects_a_malformed_entry) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "acr": "not-an-object-or-null"
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"x"}));
}

TEST(claims_parameter_accepts_rejects_a_malformed_values_constraint) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "acr": { "values": "gold" }
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"gold"}));
}
