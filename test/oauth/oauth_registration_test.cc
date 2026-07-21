#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds
#include <optional>    // std::optional
#include <string_view> // std::string_view

TEST(client_metadata_parses_a_minimal_request) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example.org/callback" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(
      metadata.value().has_redirect_uri("https://client.example.org/callback"));
  EXPECT_FALSE(
      metadata.value().has_redirect_uri("https://client.example.org/other"));
}

TEST(client_metadata_rejects_a_non_object) {
  auto document{sourcemeta::core::parse_json(R"JSON([ "not an object" ])JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(client_metadata_rejects_jwks_and_jwks_uri_together) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "jwks_uri": "https://client.example.org/keys.jwks",
    "jwks": { "keys": [] }
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(client_metadata_accepts_jwks_uri_alone) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "jwks_uri": "https://client.example.org/keys.jwks"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().jwks_uri().has_value());
  EXPECT_EQ(metadata.value().jwks_uri().value(),
            "https://client.example.org/keys.jwks");
}

TEST(client_metadata_accepts_jwks_alone) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "jwks": { "keys": [] }
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
}

TEST(client_metadata_rejects_a_non_array_grant_types) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": "authorization_code"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(client_metadata_rejects_a_non_array_response_types) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "response_types": "code"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(client_metadata_rejects_a_grant_types_with_a_non_string_element) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": [ "authorization_code", 123 ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(client_metadata_rejects_a_response_types_with_a_non_string_element) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "response_types": [ {} ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(client_metadata_rejects_a_non_string_token_endpoint_auth_method) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "token_endpoint_auth_method": [ "none" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_FALSE(metadata.has_value());
}

TEST(client_metadata_defaults_the_token_endpoint_auth_method) {
  auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().token_endpoint_auth_method(),
            "client_secret_basic");
}

TEST(client_metadata_reads_an_explicit_token_endpoint_auth_method) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "token_endpoint_auth_method": "none"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().token_endpoint_auth_method(), "none");
}

TEST(client_metadata_defaults_the_grant_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_FALSE(metadata.value().supports_grant_type("refresh_token"));
}

TEST(client_metadata_reads_explicit_grant_types) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": [ "authorization_code", "refresh_token" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("refresh_token"));
  EXPECT_FALSE(metadata.value().supports_grant_type("client_credentials"));
}

TEST(client_metadata_reads_a_grant_types_list_without_the_default) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": [ "client_credentials" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_grant_type("client_credentials"));
  EXPECT_FALSE(metadata.value().supports_grant_type("authorization_code"));
}

TEST(client_metadata_empty_grant_types_suppress_the_default) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": []
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_grant_type("authorization_code"));
}

TEST(client_metadata_empty_response_types_suppress_the_default) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "response_types": []
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_response_type("code"));
}

TEST(client_metadata_defaults_the_response_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("token"));
}

TEST(client_metadata_reads_explicit_response_types) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "response_types": [ "token" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_response_type("token"));
  EXPECT_FALSE(metadata.value().supports_response_type("code"));
}

TEST(client_metadata_reads_the_human_readable_fields) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "client_name": "My Example Client",
    "client_uri": "https://client.example.org/",
    "logo_uri": "https://client.example.org/logo.png",
    "scope": "read write",
    "tos_uri": "https://client.example.org/tos",
    "policy_uri": "https://client.example.org/policy"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().client_name().value(), "My Example Client");
  EXPECT_EQ(metadata.value().client_uri().value(),
            "https://client.example.org/");
  EXPECT_EQ(metadata.value().logo_uri().value(),
            "https://client.example.org/logo.png");
  EXPECT_EQ(metadata.value().scope().value(), "read write");
  EXPECT_EQ(metadata.value().tos_uri().value(),
            "https://client.example.org/tos");
  EXPECT_EQ(metadata.value().policy_uri().value(),
            "https://client.example.org/policy");
}

TEST(client_metadata_absent_human_readable_fields_are_empty) {
  auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().client_name().has_value());
  EXPECT_FALSE(metadata.value().client_uri().has_value());
  EXPECT_FALSE(metadata.value().logo_uri().has_value());
  EXPECT_FALSE(metadata.value().scope().has_value());
  EXPECT_FALSE(metadata.value().tos_uri().has_value());
  EXPECT_FALSE(metadata.value().policy_uri().has_value());
}

TEST(client_metadata_reads_the_contacts) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "contacts": [ "support@client.example.org" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().has_contact("support@client.example.org"));
  EXPECT_FALSE(metadata.value().has_contact("nobody@client.example.org"));
}

TEST(client_metadata_reads_the_software_fields) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "software_id": "4NRB1-0XZABZI9E6-5SM3R",
    "software_version": "2.1.0",
    "software_statement": "eyJhbGciOiJSUzI1NiJ9.eyJ9.abc"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().software_id().value(), "4NRB1-0XZABZI9E6-5SM3R");
  EXPECT_EQ(metadata.value().software_version().value(), "2.1.0");
  EXPECT_EQ(metadata.value().software_statement().value(),
            "eyJhbGciOiJSUzI1NiJ9.eyJ9.abc");
}

TEST(client_metadata_absent_software_fields_are_empty) {
  auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().software_id().has_value());
  EXPECT_FALSE(metadata.value().software_version().has_value());
  EXPECT_FALSE(metadata.value().software_statement().has_value());
}

TEST(client_metadata_reads_the_response_only_fields) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "client_id": "s6BhdRkqt3",
    "client_secret": "cf136dc3c1fc93f31185e5885805d",
    "client_id_issued_at": 2893256800,
    "client_secret_expires_at": 2893276800,
    "registration_access_token": "reg-23410913-abewfq",
    "registration_client_uri": "https://server.example/register/s6BhdRkqt3"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().client_id().value(), "s6BhdRkqt3");
  EXPECT_EQ(metadata.value().client_secret().value(),
            "cf136dc3c1fc93f31185e5885805d");
  EXPECT_EQ(metadata.value().client_id_issued_at().value(),
            std::chrono::seconds{2893256800});
  EXPECT_EQ(metadata.value().client_secret_expires_at().value(),
            std::chrono::seconds{2893276800});
  EXPECT_EQ(metadata.value().registration_access_token().value(),
            "reg-23410913-abewfq");
  EXPECT_EQ(metadata.value().registration_client_uri().value(),
            "https://server.example/register/s6BhdRkqt3");
}

TEST(client_metadata_absent_response_only_fields_are_empty) {
  auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().client_id().has_value());
  EXPECT_FALSE(metadata.value().client_secret().has_value());
  EXPECT_FALSE(metadata.value().client_id_issued_at().has_value());
  EXPECT_FALSE(metadata.value().client_secret_expires_at().has_value());
  EXPECT_FALSE(metadata.value().registration_access_token().has_value());
  EXPECT_FALSE(metadata.value().registration_client_uri().has_value());
}

TEST(client_metadata_reads_a_never_expiring_secret) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "client_secret": "cf136dc3c1fc93f31185e5885805d",
    "client_secret_expires_at": 0
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().client_secret_expires_at().has_value());
  EXPECT_EQ(metadata.value().client_secret_expires_at().value(),
            std::chrono::seconds{0});
}

TEST(client_metadata_rejects_a_negative_expiry) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "client_secret_expires_at": -1
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().client_secret_expires_at().has_value());
}

TEST(client_metadata_exposes_the_underlying_document) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "client_name#ja-Jpan-JP": "クライアント名"
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().data().defines("client_name#ja-Jpan-JP"));
}

TEST(make_registration_request_emits_a_minimal_body) {
  const std::array<std::string_view, 1> redirect_uris{
      {"https://client.example.org/callback"}};
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.redirect_uris = redirect_uris;
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_TRUE(body.has_value());
  EXPECT_TRUE(body.value().defines("redirect_uris"));
  EXPECT_EQ(body.value().at("redirect_uris").at(0).to_string(),
            "https://client.example.org/callback");
  EXPECT_FALSE(body.value().defines("token_endpoint_auth_method"));
}

TEST(make_registration_request_emits_every_field) {
  const std::array<std::string_view, 2> redirect_uris{
      {"https://client.example.org/callback",
       "https://client.example.org/callback2"}};
  const std::array<std::string_view, 2> grant_types{
      {"authorization_code", "refresh_token"}};
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> contacts{
      {"support@client.example.org"}};
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.redirect_uris = redirect_uris;
  config.token_endpoint_auth_method = "client_secret_basic";
  config.grant_types = grant_types;
  config.response_types = response_types;
  config.client_name = "My Example Client";
  config.client_uri = "https://client.example.org/";
  config.logo_uri = "https://client.example.org/logo.png";
  config.scope = "read write";
  config.contacts = contacts;
  config.tos_uri = "https://client.example.org/tos";
  config.policy_uri = "https://client.example.org/policy";
  config.jwks_uri = "https://client.example.org/keys.jwks";
  config.software_id = "4NRB1-0XZABZI9E6-5SM3R";
  config.software_version = "2.1.0";
  config.software_statement = "eyJhbGciOiJSUzI1NiJ9.eyJ9.abc";
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("token_endpoint_auth_method").to_string(),
            "client_secret_basic");
  EXPECT_EQ(body.value().at("client_name").to_string(), "My Example Client");
  EXPECT_EQ(body.value().at("client_uri").to_string(),
            "https://client.example.org/");
  EXPECT_EQ(body.value().at("logo_uri").to_string(),
            "https://client.example.org/logo.png");
  EXPECT_EQ(body.value().at("scope").to_string(), "read write");
  EXPECT_EQ(body.value().at("contacts").at(0).to_string(),
            "support@client.example.org");
  EXPECT_EQ(body.value().at("tos_uri").to_string(),
            "https://client.example.org/tos");
  EXPECT_EQ(body.value().at("policy_uri").to_string(),
            "https://client.example.org/policy");
  EXPECT_EQ(body.value().at("jwks_uri").to_string(),
            "https://client.example.org/keys.jwks");
  EXPECT_EQ(body.value().at("software_id").to_string(),
            "4NRB1-0XZABZI9E6-5SM3R");
  EXPECT_EQ(body.value().at("software_version").to_string(), "2.1.0");
  EXPECT_EQ(body.value().at("software_statement").to_string(),
            "eyJhbGciOiJSUzI1NiJ9.eyJ9.abc");
  EXPECT_EQ(body.value().at("grant_types").at(1).to_string(), "refresh_token");
  EXPECT_EQ(body.value().at("response_types").at(0).to_string(), "code");
}

TEST(make_registration_request_rejects_a_malformed_jwks_uri) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.jwks_uri = "not a uri";
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_request_rejects_a_malformed_client_uri) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.client_uri = "not a uri";
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_request_rejects_a_malformed_logo_uri) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.logo_uri = "not a uri";
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_request_rejects_a_malformed_tos_uri) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.tos_uri = "not a uri";
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_request_rejects_a_malformed_policy_uri) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.policy_uri = "not a uri";
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_request_rejects_a_malformed_redirect_uri) {
  const std::array<std::string_view, 2> redirect_uris{
      {"https://client.example.org/callback", "not a uri"}};
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.redirect_uris = redirect_uris;
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_request_accepts_a_non_https_redirect_uri) {
  const std::array<std::string_view, 2> redirect_uris{
      {"http://127.0.0.1:8080/callback", "com.example.app:/callback"}};
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.redirect_uris = redirect_uris;
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("redirect_uris").at(0).to_string(),
            "http://127.0.0.1:8080/callback");
  EXPECT_EQ(body.value().at("redirect_uris").at(1).to_string(),
            "com.example.app:/callback");
}

TEST(make_registration_request_emits_an_inline_jwks) {
  const auto keys{sourcemeta::core::parse_json(R"JSON({ "keys": [] })JSON")};
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.jwks = &keys;
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_TRUE(body.has_value());
  EXPECT_TRUE(body.value().defines("jwks"));
  EXPECT_TRUE(body.value().at("jwks").defines("keys"));
  EXPECT_FALSE(body.value().defines("jwks_uri"));
}

TEST(make_registration_request_rejects_an_inline_jwks_with_a_location) {
  const auto keys{sourcemeta::core::parse_json(R"JSON({ "keys": [] })JSON")};
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.jwks = &keys;
  config.jwks_uri = "https://client.example.org/keys.jwks";
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_request_rejects_a_non_object_inline_jwks) {
  const auto keys{
      sourcemeta::core::parse_json(R"JSON([ "not an object" ])JSON")};
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.jwks = &keys;
  const auto body{sourcemeta::core::oauth_make_registration_request(config)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_error_response_emits_the_code) {
  const auto body{sourcemeta::core::oauth_make_registration_error_response(
      sourcemeta::core::oauth_error_code(
          sourcemeta::core::OAuthRegistrationError::InvalidClientMetadata),
      "")};
  EXPECT_EQ(body.at("error").to_string(), "invalid_client_metadata");
  EXPECT_FALSE(body.defines("error_description"));
}

TEST(make_registration_error_response_emits_the_description) {
  const auto body{sourcemeta::core::oauth_make_registration_error_response(
      sourcemeta::core::oauth_error_code(
          sourcemeta::core::OAuthRegistrationError::InvalidRedirectURI),
      "The redirection URI is not absolute")};
  EXPECT_EQ(body.at("error").to_string(), "invalid_redirect_uri");
  EXPECT_EQ(body.at("error_description").to_string(),
            "The redirection URI is not absolute");
}

TEST(grant_response_consistent_accepts_the_defaults) {
  auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(sourcemeta::core::oauth_registration_grant_response_consistent(
      metadata.value()));
}

TEST(grant_response_consistent_accepts_a_matching_pair) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": [ "authorization_code", "implicit" ],
    "response_types": [ "code", "token" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(sourcemeta::core::oauth_registration_grant_response_consistent(
      metadata.value()));
}

TEST(grant_response_consistent_accepts_a_grant_without_a_response_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": [ "client_credentials" ],
    "response_types": []
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(sourcemeta::core::oauth_registration_grant_response_consistent(
      metadata.value()));
}

TEST(grant_response_consistent_accepts_a_grant_only_client_omitting_responses) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": [ "client_credentials" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(sourcemeta::core::oauth_registration_grant_response_consistent(
      metadata.value()));
}

TEST(grant_response_consistent_rejects_a_grant_without_its_response_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": [ "authorization_code" ],
    "response_types": [ "token" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_registration_grant_response_consistent(
      metadata.value()));
}

TEST(grant_response_consistent_rejects_a_response_type_without_its_grant) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "grant_types": [ "authorization_code" ],
    "response_types": [ "code", "token" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_registration_grant_response_consistent(
      metadata.value()));
}

TEST(apply_software_statement_claims_overwrites_a_requested_value) {
  auto record{sourcemeta::core::parse_json(R"JSON({
    "client_name": "Requested Name",
    "scope": "read"
  })JSON")};
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "client_name": "Attested Name"
  })JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oauth_apply_software_statement_claims(record, claims));
  EXPECT_EQ(record.at("client_name").to_string(), "Attested Name");
  EXPECT_EQ(record.at("scope").to_string(), "read");
}

TEST(apply_software_statement_claims_adds_a_new_value) {
  auto record{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "software_id": "4NRB1-0XZABZI9E6-5SM3R"
  })JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oauth_apply_software_statement_claims(record, claims));
  EXPECT_EQ(record.at("software_id").to_string(), "4NRB1-0XZABZI9E6-5SM3R");
}

TEST(apply_software_statement_claims_skips_the_jwt_structural_claims) {
  auto record{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "iss": "https://statement-issuer.example",
    "sub": "subject",
    "aud": "audience",
    "exp": 2893276800,
    "nbf": 2893256800,
    "iat": 2893256800,
    "jti": "identifier",
    "client_name": "Attested Name"
  })JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oauth_apply_software_statement_claims(record, claims));
  EXPECT_EQ(record.at("client_name").to_string(), "Attested Name");
  EXPECT_FALSE(record.defines("iss"));
  EXPECT_FALSE(record.defines("sub"));
  EXPECT_FALSE(record.defines("aud"));
  EXPECT_FALSE(record.defines("exp"));
  EXPECT_FALSE(record.defines("nbf"));
  EXPECT_FALSE(record.defines("iat"));
  EXPECT_FALSE(record.defines("jti"));
}

TEST(apply_software_statement_claims_leaves_the_echoed_statement) {
  auto record{sourcemeta::core::parse_json(R"JSON({
    "software_statement": "eyJhbGciOiJSUzI1NiJ9.eyJ9.abc"
  })JSON")};
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "software_statement": "overwritten",
    "client_name": "Attested Name"
  })JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oauth_apply_software_statement_claims(record, claims));
  EXPECT_EQ(record.at("software_statement").to_string(),
            "eyJhbGciOiJSUzI1NiJ9.eyJ9.abc");
  EXPECT_EQ(record.at("client_name").to_string(), "Attested Name");
}

TEST(apply_software_statement_claims_rejects_non_object_claims) {
  auto record{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const auto claims{
      sourcemeta::core::parse_json(R"JSON([ "not an object" ])JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_apply_software_statement_claims(record, claims));
}

TEST(apply_software_statement_claims_rejects_a_non_object_record) {
  auto record{sourcemeta::core::parse_json(R"JSON([ "not an object" ])JSON")};
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "client_name": "Attested Name"
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_apply_software_statement_claims(record, claims));
}

TEST(make_registration_response_overlays_the_client_id) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example.org/callback" ]
  })JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("client_id").to_string(), "s6BhdRkqt3");
  EXPECT_EQ(body.value().at("redirect_uris").at(0).to_string(),
            "https://client.example.org/callback");
  EXPECT_FALSE(body.value().defines("client_secret"));
}

TEST(make_registration_response_emits_every_assigned_value) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({
    "client_name": "My Example Client"
  })JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.client_secret = "cf136dc3c1fc93f31185e5885805d";
  result.client_id_issued_at = std::chrono::seconds{2893256800};
  result.client_secret_expires_at = std::chrono::seconds{2893276800};
  result.registration_access_token = "reg-23410913-abewfq";
  result.registration_client_uri =
      "https://server.example.com/register/s6BhdRkqt3";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("client_id").to_string(), "s6BhdRkqt3");
  EXPECT_EQ(body.value().at("client_secret").to_string(),
            "cf136dc3c1fc93f31185e5885805d");
  EXPECT_EQ(body.value().at("client_id_issued_at").to_integer(), 2893256800);
  EXPECT_EQ(body.value().at("client_secret_expires_at").to_integer(),
            2893276800);
  EXPECT_EQ(body.value().at("registration_access_token").to_string(),
            "reg-23410913-abewfq");
  EXPECT_EQ(body.value().at("registration_client_uri").to_string(),
            "https://server.example.com/register/s6BhdRkqt3");
  EXPECT_EQ(body.value().at("client_name").to_string(), "My Example Client");
}

TEST(make_registration_response_overwrites_a_requested_value) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({
    "client_id": "requested-and-ignored"
  })JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("client_id").to_string(), "s6BhdRkqt3");
}

TEST(make_registration_response_rejects_an_empty_client_id) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_rejects_a_secret_without_an_expiry) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.client_secret = "cf136dc3c1fc93f31185e5885805d";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_rejects_an_expiry_without_a_secret) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.client_secret_expires_at = std::chrono::seconds{2893276800};
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_rejects_a_negative_issued_at) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.client_id_issued_at = std::chrono::seconds{-1};
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_rejects_a_negative_expiry) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.client_secret = "cf136dc3c1fc93f31185e5885805d";
  result.client_secret_expires_at = std::chrono::seconds{-1};
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_accepts_a_never_expiring_secret) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.client_secret = "cf136dc3c1fc93f31185e5885805d";
  result.client_secret_expires_at = std::chrono::seconds{0};
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("client_secret_expires_at").to_integer(), 0);
}

TEST(make_registration_response_rejects_a_malformed_management_uri) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.registration_access_token = "reg-23410913-abewfq";
  result.registration_client_uri = "not a uri";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_rejects_a_lone_management_token) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.registration_access_token = "reg-23410913-abewfq";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_rejects_a_lone_management_uri) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({})JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  result.registration_client_uri =
      "https://server.example.com/register/s6BhdRkqt3";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_rejects_a_non_object_metadata) {
  const auto metadata{
      sourcemeta::core::parse_json(R"JSON([ "not an object" ])JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_response_drops_a_client_supplied_secret) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example.org/callback" ],
    "client_secret": "forged-by-client",
    "registration_access_token": "forged-by-client"
  })JSON")};
  sourcemeta::core::OAuthClientRegistrationResult result;
  result.client_id = "s6BhdRkqt3";
  const auto body{
      sourcemeta::core::oauth_make_registration_response(metadata, result)};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("client_id").to_string(), "s6BhdRkqt3");
  EXPECT_FALSE(body.value().defines("client_secret"));
  EXPECT_FALSE(body.value().defines("registration_access_token"));
}

TEST(make_registration_update_request_carries_the_client_id) {
  const std::array<std::string_view, 1> redirect_uris{
      {"https://client.example.org/callback"}};
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.redirect_uris = redirect_uris;
  config.client_name = "My New Example";
  const auto body{sourcemeta::core::oauth_make_registration_update_request(
      config, "s6BhdRkqt3", "")};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("client_id").to_string(), "s6BhdRkqt3");
  EXPECT_EQ(body.value().at("client_name").to_string(), "My New Example");
  EXPECT_FALSE(body.value().defines("client_secret"));
}

TEST(make_registration_update_request_carries_the_client_secret) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  const auto body{sourcemeta::core::oauth_make_registration_update_request(
      config, "s6BhdRkqt3", "cf136dc3c1fc93f31185e5885805d")};
  EXPECT_TRUE(body.has_value());
  EXPECT_EQ(body.value().at("client_id").to_string(), "s6BhdRkqt3");
  EXPECT_EQ(body.value().at("client_secret").to_string(),
            "cf136dc3c1fc93f31185e5885805d");
}

TEST(make_registration_update_request_rejects_an_empty_client_id) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  const auto body{
      sourcemeta::core::oauth_make_registration_update_request(config, "", "")};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_update_request_rejects_a_malformed_field) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  config.jwks_uri = "not a uri";
  const auto body{sourcemeta::core::oauth_make_registration_update_request(
      config, "s6BhdRkqt3", "")};
  EXPECT_FALSE(body.has_value());
}

TEST(make_registration_update_request_excludes_server_assigned_members) {
  sourcemeta::core::OAuthClientRegistrationConfig config;
  const auto body{sourcemeta::core::oauth_make_registration_update_request(
      config, "s6BhdRkqt3", "")};
  EXPECT_TRUE(body.has_value());
  EXPECT_FALSE(body.value().defines("registration_access_token"));
  EXPECT_FALSE(body.value().defines("registration_client_uri"));
  EXPECT_FALSE(body.value().defines("client_id_issued_at"));
  EXPECT_FALSE(body.value().defines("client_secret_expires_at"));
}
