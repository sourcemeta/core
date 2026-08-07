#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds
#include <string_view> // std::string_view

TEST(from_parses_a_valid_client_document) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "subject_type": "pairwise",
    "sector_identifier_uri": "https://client.example/sector.json",
    "id_token_signed_response_alg": "ES256",
    "default_max_age": 3600,
    "require_auth_time": true
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().has_redirect_uri("https://client.example/cb"));
  EXPECT_EQ(metadata.value().application_type(), "web");
  EXPECT_TRUE(metadata.value().subject_type().has_value());
  EXPECT_EQ(metadata.value().subject_type().value(), "pairwise");
  EXPECT_TRUE(metadata.value().sector_identifier_uri().has_value());
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "ES256");
  EXPECT_TRUE(metadata.value().default_max_age().has_value());
  EXPECT_EQ(metadata.value().default_max_age().value(),
            std::chrono::seconds{3600});
  EXPECT_TRUE(metadata.value().require_auth_time());
}

TEST(from_applies_the_defaults) {
  auto document{sourcemeta::core::parse_json(
      R"JSON({ "redirect_uris": [ "https://client.example/cb" ] })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().application_type(), "web");
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "RS256");
  EXPECT_FALSE(metadata.value().require_auth_time());
  EXPECT_FALSE(metadata.value().subject_type().has_value());
  EXPECT_FALSE(metadata.value().default_max_age().has_value());
}

TEST(from_rejects_a_missing_redirect_uris) {
  auto document{
      sourcemeta::core::parse_json(R"JSON({ "subject_type": "public" })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_an_empty_redirect_uris) {
  auto document{
      sourcemeta::core::parse_json(R"JSON({ "redirect_uris": [] })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_relative_redirect_uri) {
  auto document{sourcemeta::core::parse_json(
      R"JSON({ "redirect_uris": [ "/cb" ] })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_fragment_bearing_redirect_uri) {
  auto document{sourcemeta::core::parse_json(
      R"JSON({ "redirect_uris": [ "https://client.example/cb#x" ] })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_native_client_with_an_https_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "native",
    "redirect_uris": [ "https://client.example/cb" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_web_implicit_client_with_a_hostless_https_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "grant_types": [ "implicit" ],
    "response_types": [ "token" ],
    "redirect_uris": [ "https:///cb" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_an_unknown_application_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "desktop",
    "redirect_uris": [ "https://client.example/cb" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_accepts_a_native_client_with_a_custom_scheme_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "native",
    "redirect_uris": [ "com.example.app:/cb" ]
  })JSON")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "application_type": "native",
    "redirect_uris": [ "com.example.app:/cb" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().application_type(), "native");
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "RS256");
  EXPECT_FALSE(metadata.value().require_auth_time());
  EXPECT_FALSE(metadata.value().subject_type().has_value());
  EXPECT_FALSE(metadata.value().default_max_age().has_value());
  EXPECT_TRUE(metadata.value().has_redirect_uri("com.example.app:/cb"));
}

TEST(from_accepts_a_native_client_with_a_loopback_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "native",
    "redirect_uris": [ "http://127.0.0.1/cb" ]
  })JSON")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "application_type": "native",
    "redirect_uris": [ "http://127.0.0.1/cb" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().application_type(), "native");
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "RS256");
  EXPECT_FALSE(metadata.value().require_auth_time());
  EXPECT_TRUE(metadata.value().has_redirect_uri("http://127.0.0.1/cb"));
}

TEST(from_rejects_a_web_implicit_client_with_a_localhost_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "grant_types": [ "implicit" ],
    "redirect_uris": [ "http://localhost/cb" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_web_implicit_client_with_a_cleartext_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "grant_types": [ "implicit" ],
    "redirect_uris": [ "http://client.example/cb" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_accepts_a_web_implicit_client_with_an_https_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "grant_types": [ "implicit" ],
    "redirect_uris": [ "https://client.example/cb" ]
  })JSON")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "grant_types": [ "implicit" ],
    "redirect_uris": [ "https://client.example/cb" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().application_type(), "web");
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "RS256");
  EXPECT_FALSE(metadata.value().require_auth_time());
  EXPECT_TRUE(metadata.value().has_redirect_uri("https://client.example/cb"));
}

TEST(from_accepts_a_web_authorization_code_client_with_an_https_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "grant_types": [ "authorization_code" ],
    "redirect_uris": [ "https://client.example/cb" ]
  })JSON")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "grant_types": [ "authorization_code" ],
    "redirect_uris": [ "https://client.example/cb" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().application_type(), "web");
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "RS256");
  EXPECT_FALSE(metadata.value().require_auth_time());
  EXPECT_TRUE(metadata.value().has_redirect_uri("https://client.example/cb"));
}

TEST(
    from_accepts_a_web_authorization_code_client_with_a_localhost_redirect_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "redirect_uris": [ "http://localhost/cb" ]
  })JSON")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "application_type": "web",
    "redirect_uris": [ "http://localhost/cb" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().application_type(), "web");
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "RS256");
  EXPECT_FALSE(metadata.value().require_auth_time());
  EXPECT_TRUE(metadata.value().has_redirect_uri("http://localhost/cb"));
}

TEST(from_rejects_a_wrong_typed_require_auth_time) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "require_auth_time": "true"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_wrong_typed_application_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "application_type": 42
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_non_https_sector_identifier_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "sector_identifier_uri": "http://client.example/sector.json"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_accepts_a_mixed_case_https_sector_identifier_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "sector_identifier_uri": "HTTPS://client.example/sector.json"
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                  .has_value());
}

TEST(from_rejects_a_non_https_initiate_login_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "initiate_login_uri": "http://client.example/login"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_exposes_post_logout_redirect_uris) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "post_logout_redirect_uris": [ "https://client.example/after" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().has_post_logout_redirect_uri(
      "https://client.example/after"));
  EXPECT_FALSE(metadata.value().has_post_logout_redirect_uri(
      "https://attacker.example/after"));
}

TEST(sector_identifier_contains_all_registered_uris) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ "https://client.example/cb", "https://client.example/cb2" ])JSON")};
  const std::array<std::string_view, 2> registered{
      {"https://client.example/cb", "https://client.example/cb2"}};
  EXPECT_TRUE(
      sourcemeta::core::oidc_sector_identifier_contains(document, registered));
}

TEST(sector_identifier_rejects_a_missing_uri) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ "https://client.example/cb" ])JSON")};
  const std::array<std::string_view, 2> registered{
      {"https://client.example/cb", "https://client.example/other"}};
  EXPECT_FALSE(
      sourcemeta::core::oidc_sector_identifier_contains(document, registered));
}

TEST(sector_identifier_rejects_a_non_string_element) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ "https://client.example/cb", 123 ])JSON")};
  const std::array<std::string_view, 1> registered{
      {"https://client.example/cb"}};
  EXPECT_FALSE(
      sourcemeta::core::oidc_sector_identifier_contains(document, registered));
}

TEST(sector_identifier_rejects_a_non_array_document) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "uris": [ "https://client.example/cb" ] })JSON")};
  const std::array<std::string_view, 1> registered{
      {"https://client.example/cb"}};
  EXPECT_FALSE(
      sourcemeta::core::oidc_sector_identifier_contains(document, registered));
}

TEST(from_rejects_a_cleartext_client_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "jwks_uri": "http://client.example/jwks"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_non_string_client_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "jwks_uri": 42
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_hostless_client_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "jwks_uri": "https:///jwks"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_accepts_an_https_client_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "jwks_uri": "https://client.example/jwks"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().oauth().jwks_uri().value(),
            "https://client.example/jwks");
}

TEST(from_accepts_a_cleartext_request_uri) {
  // OpenID Connect Dynamic Client Registration 1.0 Section 2 makes the https
  // rule for request_uris conditional, "unless the target Request Object is
  // signed in a way that is verifiable by the OP", which is not knowable here
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "request_uris": [ "http://client.example/request.jwt" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
}

TEST(from_rejects_an_unparseable_redirect_uri) {
  auto document{
      sourcemeta::core::parse_json(R"JSON({ "redirect_uris": [ "%" ] })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_an_unparseable_sector_identifier_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "sector_identifier_uri": "%"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_an_unparseable_initiate_login_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "initiate_login_uri": "%"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_non_integer_default_max_age) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "default_max_age": "soon"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_negative_default_max_age) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "default_max_age": -1
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_non_boolean_require_auth_time) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "require_auth_time": 1
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_a_non_array_post_logout_redirect_uris) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "post_logout_redirect_uris": 42
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(encrypted_and_userinfo_algorithm_accessors) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "id_token_encrypted_response_alg": "RSA-OAEP",
    "userinfo_signed_response_alg": "ES256"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().id_token_encrypted_response_alg().has_value());
  EXPECT_EQ(metadata.value().id_token_encrypted_response_alg().value(),
            "RSA-OAEP");
  EXPECT_TRUE(metadata.value().userinfo_signed_response_alg().has_value());
  EXPECT_EQ(metadata.value().userinfo_signed_response_alg().value(), "ES256");
}
