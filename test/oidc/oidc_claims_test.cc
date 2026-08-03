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

TEST(claim_request_accepts_any_value_for_a_null_request) {
  const auto request{sourcemeta::core::parse_json("null")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"anything"}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{123}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{nullptr}));
}

TEST(claim_request_accepts_any_value_for_an_empty_object_request) {
  const auto request{sourcemeta::core::parse_json("{}")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"anything"}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{123}));
}

TEST(claim_request_accepts_any_value_when_only_essential_is_present) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "essential": true })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"anything"}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{123}));
}

TEST(claim_request_accepts_an_exact_value) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "gold" })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"gold"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"silver"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"gol"}));
}

TEST(claim_request_accepts_one_of_values) {
  const auto request{sourcemeta::core::parse_json(
      R"JSON({ "values": [ "gold", "silver" ] })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"gold"}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"silver"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"bronze"}));
}

TEST(claim_request_accepts_either_constraint_when_both_are_present) {
  const auto request{sourcemeta::core::parse_json(
      R"JSON({ "value": "gold", "values": [ "silver" ] })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"gold"}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"silver"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"bronze"}));
}

TEST(claim_request_ignores_the_essential_flag) {
  const auto essential{sourcemeta::core::parse_json(
      R"JSON({ "essential": true, "value": "gold" })JSON")};
  const auto voluntary{sourcemeta::core::parse_json(
      R"JSON({ "essential": false, "value": "gold" })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      essential, sourcemeta::core::JSON{"gold"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      essential, sourcemeta::core::JSON{"silver"}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      voluntary, sourcemeta::core::JSON{"gold"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      voluntary, sourcemeta::core::JSON{"silver"}));
}

TEST(claim_request_compares_by_json_equality) {
  const auto request{sourcemeta::core::parse_json(
      R"JSON({ "values": [ 21, true, { "group": "admins" } ] })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{21}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{true}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      request,
      sourcemeta::core::parse_json(R"JSON({ "group": "admins" })JSON")));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"21"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request,
      sourcemeta::core::parse_json(R"JSON({ "group": "users" })JSON")));
}

TEST(claim_request_rejects_a_string_request) {
  const auto request{sourcemeta::core::parse_json(R"JSON("gold")JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"gold"}));
}

TEST(claim_request_rejects_a_numeric_request) {
  const auto request{sourcemeta::core::parse_json("42")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{42}));
}

TEST(claim_request_rejects_a_boolean_request) {
  const auto request{sourcemeta::core::parse_json("true")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{true}));
}

TEST(claim_request_rejects_an_array_request) {
  const auto request{sourcemeta::core::parse_json(R"JSON([ "gold" ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"gold"}));
}

TEST(claim_request_rejects_a_malformed_values_constraint) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "values": "gold" })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      request, sourcemeta::core::JSON{"gold"}));
}

TEST(claims_parameter_accepts_agrees_with_the_claim_request_predicate) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "id_token": {
      "acr": { "values": [ "gold" ] }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"gold"}));
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts(
      claims.at("id_token").at("acr"), sourcemeta::core::JSON{"gold"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claims_parameter_accepts(
      claims, "id_token", "acr", sourcemeta::core::JSON{"silver"}));
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts(
      claims.at("id_token").at("acr"), sourcemeta::core::JSON{"silver"}));
}

TEST(claim_to_scope_maps_sub_to_openid) {
  const auto scope{sourcemeta::core::oidc_claim_to_scope("sub")};
  EXPECT_TRUE(scope.has_value());
  EXPECT_EQ(scope.value(), "openid");
}

TEST(claim_to_scope_maps_the_email_claims) {
  const auto email{sourcemeta::core::oidc_claim_to_scope("email")};
  const auto email_verified{
      sourcemeta::core::oidc_claim_to_scope("email_verified")};
  EXPECT_TRUE(email.has_value());
  EXPECT_EQ(email.value(), "email");
  EXPECT_TRUE(email_verified.has_value());
  EXPECT_EQ(email_verified.value(), "email");
}

TEST(claim_to_scope_maps_the_profile_claims) {
  const auto name{sourcemeta::core::oidc_claim_to_scope("name")};
  const auto family_name{sourcemeta::core::oidc_claim_to_scope("family_name")};
  const auto given_name{sourcemeta::core::oidc_claim_to_scope("given_name")};
  const auto middle_name{sourcemeta::core::oidc_claim_to_scope("middle_name")};
  const auto nickname{sourcemeta::core::oidc_claim_to_scope("nickname")};
  const auto preferred_username{
      sourcemeta::core::oidc_claim_to_scope("preferred_username")};
  const auto profile{sourcemeta::core::oidc_claim_to_scope("profile")};
  const auto picture{sourcemeta::core::oidc_claim_to_scope("picture")};
  const auto website{sourcemeta::core::oidc_claim_to_scope("website")};
  const auto gender{sourcemeta::core::oidc_claim_to_scope("gender")};
  const auto birthdate{sourcemeta::core::oidc_claim_to_scope("birthdate")};
  const auto zoneinfo{sourcemeta::core::oidc_claim_to_scope("zoneinfo")};
  const auto locale{sourcemeta::core::oidc_claim_to_scope("locale")};
  const auto updated_at{sourcemeta::core::oidc_claim_to_scope("updated_at")};
  EXPECT_TRUE(name.has_value());
  EXPECT_EQ(name.value(), "profile");
  EXPECT_TRUE(family_name.has_value());
  EXPECT_EQ(family_name.value(), "profile");
  EXPECT_TRUE(given_name.has_value());
  EXPECT_EQ(given_name.value(), "profile");
  EXPECT_TRUE(middle_name.has_value());
  EXPECT_EQ(middle_name.value(), "profile");
  EXPECT_TRUE(nickname.has_value());
  EXPECT_EQ(nickname.value(), "profile");
  EXPECT_TRUE(preferred_username.has_value());
  EXPECT_EQ(preferred_username.value(), "profile");
  EXPECT_TRUE(profile.has_value());
  EXPECT_EQ(profile.value(), "profile");
  EXPECT_TRUE(picture.has_value());
  EXPECT_EQ(picture.value(), "profile");
  EXPECT_TRUE(website.has_value());
  EXPECT_EQ(website.value(), "profile");
  EXPECT_TRUE(gender.has_value());
  EXPECT_EQ(gender.value(), "profile");
  EXPECT_TRUE(birthdate.has_value());
  EXPECT_EQ(birthdate.value(), "profile");
  EXPECT_TRUE(zoneinfo.has_value());
  EXPECT_EQ(zoneinfo.value(), "profile");
  EXPECT_TRUE(locale.has_value());
  EXPECT_EQ(locale.value(), "profile");
  EXPECT_TRUE(updated_at.has_value());
  EXPECT_EQ(updated_at.value(), "profile");
}

TEST(claim_to_scope_maps_the_address_claim) {
  const auto address{sourcemeta::core::oidc_claim_to_scope("address")};
  EXPECT_TRUE(address.has_value());
  EXPECT_EQ(address.value(), "address");
}

TEST(claim_to_scope_maps_the_phone_claims) {
  const auto phone_number{
      sourcemeta::core::oidc_claim_to_scope("phone_number")};
  const auto phone_number_verified{
      sourcemeta::core::oidc_claim_to_scope("phone_number_verified")};
  EXPECT_TRUE(phone_number.has_value());
  EXPECT_EQ(phone_number.value(), "phone");
  EXPECT_TRUE(phone_number_verified.has_value());
  EXPECT_EQ(phone_number_verified.value(), "phone");
}

TEST(claim_to_scope_never_invents_a_scope_for_a_non_standard_claim) {
  EXPECT_FALSE(sourcemeta::core::oidc_claim_to_scope("groups").has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_claim_to_scope("roles").has_value());
  EXPECT_FALSE(
      sourcemeta::core::oidc_claim_to_scope("entitlements").has_value());
  EXPECT_FALSE(
      sourcemeta::core::oidc_claim_to_scope("custom_claim").has_value());
}

TEST(claim_to_scope_has_no_scope_for_a_claim_outside_the_scope_sets) {
  EXPECT_FALSE(sourcemeta::core::oidc_claim_to_scope("acr").has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_claim_to_scope("auth_time").has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_claim_to_scope("openid").has_value());
}

TEST(claim_to_scope_has_no_scope_for_an_empty_name) {
  EXPECT_FALSE(sourcemeta::core::oidc_claim_to_scope("").has_value());
}

TEST(claim_to_scope_is_case_sensitive) {
  EXPECT_FALSE(sourcemeta::core::oidc_claim_to_scope("Email").has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_claim_to_scope("SUB").has_value());
}
