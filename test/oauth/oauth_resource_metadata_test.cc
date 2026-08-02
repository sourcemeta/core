#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string_view> // std::string_view
#include <utility>     // std::move

TEST(resource_metadata_rejects_a_cleartext_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "jwks_uri": "http://api.example.com/jwks"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_jwks_uri_with_a_fragment) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "jwks_uri": "https://api.example.com/jwks#keys"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "jwks_uri": 42
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_accepts_an_https_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "jwks_uri": "https://api.example.com/jwks"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().jwks_uri().value(),
            "https://api.example.com/jwks");
}

TEST(resource_metadata_parses_a_valid_document) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "https://auth.example.com" ],
    "jwks_uri": "https://api.example.com/jwks",
    "scopes_supported": [ "read", "write" ],
    "bearer_methods_supported": [ "header" ],
    "dpop_bound_access_tokens_required": true
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().resource(), "https://api.example.com");
  EXPECT_TRUE(metadata.value().first_authorization_server().has_value());
  EXPECT_EQ(metadata.value().first_authorization_server().value(),
            "https://auth.example.com");
  EXPECT_TRUE(metadata.value().jwks_uri().has_value());
  EXPECT_EQ(metadata.value().jwks_uri().value(),
            "https://api.example.com/jwks");
  EXPECT_TRUE(metadata.value().dpop_bound_access_tokens_required());
}

TEST(resource_metadata_rejects_a_missing_resource) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "authorization_servers": [ "https://auth.example.com" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_resource) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": 42
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_mismatched_resource) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://other.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_https_resource) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "http://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "http://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_resource_with_a_fragment) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com/path#section"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com/path#section")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_allows_a_resource_with_a_query) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com/path?tenant=a"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com/path?tenant=a")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().resource(),
            "https://api.example.com/path?tenant=a");
}

TEST(resource_metadata_rejects_an_empty_authority) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https:///path"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https:///path")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_signing_alg_none) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_signing_alg_values_supported": [ "RS256", "none" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_an_empty_signing_alg_list) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_signing_alg_values_supported": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_first_authorization_server_when_absent) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().first_authorization_server().has_value());
}

TEST(resource_metadata_supports_authorization_server) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "https://a.example.com", "https://b.example.com" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(
      metadata.value().supports_authorization_server("https://b.example.com"));
  EXPECT_FALSE(
      metadata.value().supports_authorization_server("https://c.example.com"));
  EXPECT_EQ(metadata.value().first_authorization_server().value(),
            "https://a.example.com");
}

TEST(resource_metadata_supports_bearer_method) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "bearer_methods_supported": [ "header", "body" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_bearer_method("header"));
  EXPECT_FALSE(metadata.value().supports_bearer_method("query"));
}

TEST(resource_metadata_bearer_method_absent_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_bearer_method("header"));
}

TEST(resource_metadata_supports_scope) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "scopes_supported": [ "read", "write" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_scope("read"));
  EXPECT_FALSE(metadata.value().supports_scope("admin"));
}

TEST(resource_metadata_dpop_bound_required_defaults_to_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().dpop_bound_access_tokens_required());
}

TEST(resource_metadata_data_exposes_untyped_members) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_name": "Example API"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().data().defines("resource_name"));
}

TEST(resource_metadata_rejects_an_uppercase_scheme) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "HTTPS://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "HTTPS://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_resource_with_a_space) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_authorization_server) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ 42, "https://auth.example.com" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_trailing_non_string_authorization_server) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "https://auth.example.com", 42 ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_all_non_string_authorization_servers) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ 1, 2, 3 ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_an_empty_authorization_server_array) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_array_authorization_servers) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": "https://auth.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_cleartext_authorization_server) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "http://auth.example.com" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_second_cleartext_authorization_server) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [
      "https://auth.example.com", "http://evil.example.com" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_an_authorization_server_with_a_query) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "https://auth.example.com?tenant=1" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_an_authorization_server_with_a_fragment) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "https://auth.example.com#tenant" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_accepts_an_uppercase_authorization_server_scheme) {
  // An advertised issuer is matched against nothing at parse time, so its
  // scheme is case-insensitive, unlike the resource identifier the document is
  // checked against
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "HTTPS://auth.example.com" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().first_authorization_server().value(),
            "HTTPS://auth.example.com");
}

TEST(resource_metadata_accepts_multiple_authorization_servers) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [
      "https://auth.example.com", "https://other.example.net/issuer" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().first_authorization_server().has_value());
  EXPECT_EQ(metadata.value().first_authorization_server().value(),
            "https://auth.example.com");
  EXPECT_TRUE(metadata.value().supports_authorization_server(
      "https://other.example.net/issuer"));
}

TEST(resource_metadata_rejects_a_non_object) {
  auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_array_signing_algs) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_signing_alg_values_supported": "RS256"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_boolean_dpop_bound_required) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "dpop_bound_access_tokens_required": "true"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_empty_bearer_methods_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "bearer_methods_supported": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_bearer_method("header"));
}

TEST(resource_metadata_accepts_userinfo_and_port) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://user@api.example.com:8443/path"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://user@api.example.com:8443/path")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().resource(),
            "https://user@api.example.com:8443/path");
}

TEST(resource_metadata_rejects_an_empty_host) {
  // https://:443/path has an authority but no host, so it is not a valid
  // resource identifier (RFC 3986 Section 3.2)
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://:443/path"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://:443/path")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_an_empty_host_with_userinfo) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://user@:443/path"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://user@:443/path")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_port_above_the_limit) {
  // The port fits the RFC 3986 grammar but exceeds what a URI can hold, so
  // parsing must be reported as an invalid document rather than escaping as an
  // exception
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com:99999999999999/path"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com:99999999999999/path")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_accepts_the_maximum_port) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com:4294967295/path"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com:4294967295/path")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().resource(),
            "https://api.example.com:4294967295/path");
}

TEST(make_resource_metadata_emits_the_required_member) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_emits_every_member) {
  const std::array<std::string_view, 2> servers{
      {"https://auth.example.com", "https://backup.example.net"}};
  const std::array<std::string_view, 2> scopes{{"read", "write"}};
  const std::array<std::string_view, 2> bearer_methods{{"header", "body"}};
  const std::array<std::string_view, 2> signing_algorithms{{"RS256", "ES256"}};
  const std::array<std::string_view, 1> details_types{{"payment_initiation"}};
  const std::array<std::string_view, 2> dpop_algorithms{{"ES256", "PS256"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.authorization_servers = servers;
  config.jwks_uri = "https://api.example.com/jwks";
  config.scopes_supported = scopes;
  config.bearer_methods_supported = bearer_methods;
  config.resource_signing_alg_values_supported = signing_algorithms;
  config.resource_name = "Example API";
  config.resource_documentation = "https://api.example.com/docs";
  config.resource_policy_uri = "https://api.example.com/policy";
  config.resource_tos_uri = "https://api.example.com/terms";
  config.tls_client_certificate_bound_access_tokens = true;
  config.authorization_details_types_supported = details_types;
  config.dpop_signing_alg_values_supported = dpop_algorithms;
  config.dpop_bound_access_tokens_required = true;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers":
      [ "https://auth.example.com", "https://backup.example.net" ],
    "jwks_uri": "https://api.example.com/jwks",
    "scopes_supported": [ "read", "write" ],
    "bearer_methods_supported": [ "header", "body" ],
    "resource_signing_alg_values_supported": [ "RS256", "ES256" ],
    "resource_name": "Example API",
    "resource_documentation": "https://api.example.com/docs",
    "resource_policy_uri": "https://api.example.com/policy",
    "resource_tos_uri": "https://api.example.com/terms",
    "tls_client_certificate_bound_access_tokens": true,
    "authorization_details_types_supported": [ "payment_initiation" ],
    "dpop_signing_alg_values_supported": [ "ES256", "PS256" ],
    "dpop_bound_access_tokens_required": true
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_rejects_a_cleartext_resource) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "http://api.example.com";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_a_resource_with_a_fragment) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com#section";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_an_empty_resource) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_an_uppercase_resource_scheme) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "HTTPS://api.example.com";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_a_resource_without_a_host) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_a_relative_resource) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "/mcp";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_accepts_a_resource_with_a_query) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com/mcp?tenant=1";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com/mcp?tenant=1"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_accepts_a_resource_with_a_port) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com:8443/mcp";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com:8443/mcp"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_rejects_a_cleartext_authorization_server) {
  const std::array<std::string_view, 1> servers{{"http://auth.example.com"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.authorization_servers = servers;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_an_authorization_server_with_a_query) {
  const std::array<std::string_view, 1> servers{
      {"https://auth.example.com/?tenant=1"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.authorization_servers = servers;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_an_authorization_server_with_a_fragment) {
  const std::array<std::string_view, 1> servers{
      {"https://auth.example.com#main"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.authorization_servers = servers;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_a_trailing_invalid_authorization_server) {
  const std::array<std::string_view, 2> servers{
      {"https://auth.example.com", "http://cleartext.example.com"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.authorization_servers = servers;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_accepts_an_uppercase_authorization_server_scheme) {
  const std::array<std::string_view, 1> servers{{"HTTPS://auth.example.com"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.authorization_servers = servers;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "HTTPS://auth.example.com" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_rejects_a_cleartext_jwks_uri) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.jwks_uri = "http://api.example.com/jwks";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_a_jwks_uri_with_a_fragment) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.jwks_uri = "https://api.example.com/jwks#keys";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_accepts_an_https_jwks_uri) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.jwks_uri = "https://api.example.com/jwks";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "jwks_uri": "https://api.example.com/jwks"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_rejects_none_in_resource_signing_algs) {
  const std::array<std::string_view, 2> algorithms{{"RS256", "none"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.resource_signing_alg_values_supported = algorithms;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_none_in_dpop_signing_algs) {
  const std::array<std::string_view, 2> algorithms{{"ES256", "none"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.dpop_signing_alg_values_supported = algorithms;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_hs256_in_dpop_signing_algs) {
  const std::array<std::string_view, 2> algorithms{{"ES256", "HS256"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.dpop_signing_alg_values_supported = algorithms;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_hs384_in_dpop_signing_algs) {
  const std::array<std::string_view, 1> algorithms{{"HS384"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.dpop_signing_alg_values_supported = algorithms;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_hs512_in_dpop_signing_algs) {
  const std::array<std::string_view, 1> algorithms{{"HS512"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.dpop_signing_alg_values_supported = algorithms;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_accepts_asymmetric_dpop_signing_algs) {
  const std::array<std::string_view, 3> algorithms{{"ES256", "PS256", "EdDSA"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.dpop_signing_alg_values_supported = algorithms;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "dpop_signing_alg_values_supported": [ "ES256", "PS256", "EdDSA" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_rejects_a_schemeless_documentation) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.resource_documentation = "/docs";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_a_schemeless_policy_uri) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.resource_policy_uri = "policy.html";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_a_schemeless_tos_uri) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.resource_tos_uri = "//example.com/terms";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_rejects_a_malformed_documentation) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.resource_documentation = "https://ex ample.com/docs";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_resource_metadata_accepts_a_cleartext_documentation) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.resource_documentation = "http://api.example.com/docs";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_documentation": "http://api.example.com/docs"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_accepts_a_documentation_with_a_fragment) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.resource_documentation = "https://api.example.com/docs#auth";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_documentation": "https://api.example.com/docs#auth"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_omits_absent_members) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_omits_empty_arrays) {
  const std::array<std::string_view, 0> empty{};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.authorization_servers = empty;
  config.scopes_supported = empty;
  config.resource_signing_alg_values_supported = empty;
  config.authorization_details_types_supported = empty;
  config.dpop_signing_alg_values_supported = empty;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_omits_bearer_methods_when_disengaged) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_emits_an_empty_bearer_methods_when_engaged_empty) {
  const std::array<std::string_view, 0> methods{};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.bearer_methods_supported = methods;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "bearer_methods_supported": []
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_emits_bearer_methods_values) {
  const std::array<std::string_view, 3> methods{{"header", "body", "query"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.bearer_methods_supported = methods;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "bearer_methods_supported": [ "header", "body", "query" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_omits_false_booleans) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.tls_client_certificate_bound_access_tokens = false;
  config.dpop_bound_access_tokens_required = false;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_emits_true_booleans) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.tls_client_certificate_bound_access_tokens = true;
  config.dpop_bound_access_tokens_required = true;
  const auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "tls_client_certificate_bound_access_tokens": true,
    "dpop_bound_access_tokens_required": true
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_resource_metadata_minimal_round_trips) {
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document.value()), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().resource(), "https://api.example.com");
  EXPECT_FALSE(metadata.value().first_authorization_server().has_value());
}

TEST(make_resource_metadata_round_trips) {
  const std::array<std::string_view, 2> servers{
      {"https://auth.example.com", "https://backup.example.net"}};
  const std::array<std::string_view, 2> scopes{{"read", "write"}};
  const std::array<std::string_view, 1> bearer_methods{{"header"}};
  const std::array<std::string_view, 1> signing_algorithms{{"ES256"}};
  const std::array<std::string_view, 1> details_types{{"payment_initiation"}};
  const std::array<std::string_view, 1> dpop_algorithms{{"ES256"}};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com/mcp?tenant=1";
  config.authorization_servers = servers;
  config.jwks_uri = "https://api.example.com/jwks";
  config.scopes_supported = scopes;
  config.bearer_methods_supported = bearer_methods;
  config.resource_signing_alg_values_supported = signing_algorithms;
  config.resource_name = "Example API";
  config.resource_documentation = "https://api.example.com/docs";
  config.resource_policy_uri = "https://api.example.com/policy";
  config.resource_tos_uri = "https://api.example.com/terms";
  config.tls_client_certificate_bound_access_tokens = true;
  config.authorization_details_types_supported = details_types;
  config.dpop_signing_alg_values_supported = dpop_algorithms;
  config.dpop_bound_access_tokens_required = true;
  auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document.value()), "https://api.example.com/mcp?tenant=1")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().resource(),
            "https://api.example.com/mcp?tenant=1");
  EXPECT_EQ(metadata.value().first_authorization_server().value(),
            "https://auth.example.com");
  EXPECT_TRUE(metadata.value().supports_authorization_server(
      "https://backup.example.net"));
  EXPECT_EQ(metadata.value().jwks_uri().value(),
            "https://api.example.com/jwks");
  EXPECT_TRUE(metadata.value().supports_scope("read"));
  EXPECT_TRUE(metadata.value().supports_scope("write"));
  EXPECT_TRUE(metadata.value().supports_bearer_method("header"));
  EXPECT_TRUE(metadata.value().supports_resource_signing_alg("ES256"));
  EXPECT_EQ(metadata.value().resource_name().value(), "Example API");
  EXPECT_EQ(metadata.value().resource_documentation().value(),
            "https://api.example.com/docs");
  EXPECT_EQ(metadata.value().resource_policy_uri().value(),
            "https://api.example.com/policy");
  EXPECT_EQ(metadata.value().resource_tos_uri().value(),
            "https://api.example.com/terms");
  EXPECT_TRUE(metadata.value().tls_client_certificate_bound_access_tokens());
  EXPECT_TRUE(metadata.value().supports_authorization_details_type(
      "payment_initiation"));
  EXPECT_TRUE(metadata.value().supports_dpop_signing_alg("ES256"));
  EXPECT_TRUE(metadata.value().dpop_bound_access_tokens_required());
}

TEST(make_resource_metadata_empty_bearer_methods_round_trips) {
  const std::array<std::string_view, 0> methods{};
  sourcemeta::core::OAuthResourceMetadataConfig config;
  config.resource = "https://api.example.com";
  config.bearer_methods_supported = methods;
  auto document{sourcemeta::core::oauth_make_resource_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document.value()), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_bearer_method("header"));
  EXPECT_FALSE(metadata.value().supports_bearer_method("body"));
  EXPECT_FALSE(metadata.value().supports_bearer_method("query"));
}

TEST(resource_metadata_resource_name_present) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_name": "Example API"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().resource_name().has_value());
  EXPECT_EQ(metadata.value().resource_name().value(), "Example API");
}

TEST(resource_metadata_resource_name_absent) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().resource_name().has_value());
}

TEST(resource_metadata_language_tagged_name_yields_absent_untagged) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_name#it": "API di esempio"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().resource_name().has_value());
  EXPECT_TRUE(metadata.value().data().defines("resource_name#it"));
  EXPECT_EQ(metadata.value().data().at("resource_name#it").to_string(),
            "API di esempio");
}

TEST(resource_metadata_resource_documentation_present) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_documentation": "https://api.example.com/docs"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().resource_documentation().has_value());
  EXPECT_EQ(metadata.value().resource_documentation().value(),
            "https://api.example.com/docs");
}

TEST(resource_metadata_resource_documentation_absent) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().resource_documentation().has_value());
}

TEST(resource_metadata_resource_policy_uri_present) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_policy_uri": "https://api.example.com/policy"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().resource_policy_uri().has_value());
  EXPECT_EQ(metadata.value().resource_policy_uri().value(),
            "https://api.example.com/policy");
}

TEST(resource_metadata_resource_policy_uri_absent) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().resource_policy_uri().has_value());
}

TEST(resource_metadata_resource_tos_uri_present) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_tos_uri": "https://api.example.com/terms"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().resource_tos_uri().has_value());
  EXPECT_EQ(metadata.value().resource_tos_uri().value(),
            "https://api.example.com/terms");
}

TEST(resource_metadata_resource_tos_uri_absent) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().resource_tos_uri().has_value());
}

TEST(resource_metadata_tls_client_certificate_bound_defaults_to_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().tls_client_certificate_bound_access_tokens());
}

TEST(resource_metadata_tls_client_certificate_bound_reads_true) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "tls_client_certificate_bound_access_tokens": true
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().tls_client_certificate_bound_access_tokens());
}

TEST(resource_metadata_tls_client_certificate_bound_reads_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "tls_client_certificate_bound_access_tokens": false
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().tls_client_certificate_bound_access_tokens());
}

TEST(resource_metadata_supports_resource_signing_alg) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_signing_alg_values_supported": [ "RS256", "ES256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_resource_signing_alg("RS256"));
  EXPECT_TRUE(metadata.value().supports_resource_signing_alg("ES256"));
  EXPECT_FALSE(metadata.value().supports_resource_signing_alg("PS256"));
}

TEST(resource_metadata_resource_signing_alg_absent_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_resource_signing_alg("RS256"));
}

TEST(resource_metadata_supports_dpop_signing_alg) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "dpop_signing_alg_values_supported": [ "ES256", "PS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_dpop_signing_alg("ES256"));
  EXPECT_TRUE(metadata.value().supports_dpop_signing_alg("PS256"));
  EXPECT_FALSE(metadata.value().supports_dpop_signing_alg("RS256"));
}

TEST(resource_metadata_dpop_signing_alg_absent_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_dpop_signing_alg("ES256"));
}

TEST(resource_metadata_supports_authorization_details_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_details_types_supported": [ "payment_initiation" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_authorization_details_type(
      "payment_initiation"));
  EXPECT_FALSE(
      metadata.value().supports_authorization_details_type("account_access"));
}

TEST(resource_metadata_authorization_details_type_absent_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_authorization_details_type(
      "payment_initiation"));
}

TEST(resource_metadata_rejects_an_empty_scopes_supported) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "scopes_supported": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_array_scopes_supported) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "scopes_supported": "read"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_scope_element) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "scopes_supported": [ "read", 1 ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_an_empty_authorization_details_types) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_details_types_supported": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_authorization_details_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_details_types_supported": [ null ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_an_empty_dpop_signing_algs) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "dpop_signing_alg_values_supported": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_dpop_signing_alg) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "dpop_signing_alg_values_supported": [ "ES256", 42 ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_accepts_none_in_dpop_signing_algs) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "dpop_signing_alg_values_supported": [ "none" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_dpop_signing_alg("none"));
}

TEST(resource_metadata_rejects_a_non_array_bearer_methods) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "bearer_methods_supported": "header"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_bearer_method) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "bearer_methods_supported": [ "header", true ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_signing_alg_element) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_signing_alg_values_supported": [ "RS256", 256 ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_boolean_tls_client_certificate_bound) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "tls_client_certificate_bound_access_tokens": "true"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_resource_name) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_name": 42
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_resource_documentation) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_documentation": [ "https://api.example.com/docs" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_resource_policy_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_policy_uri": null
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_rejects_a_non_string_resource_tos_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "resource_tos_uri": {}
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(resource_metadata_parses_the_rfc9728_example) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource":
      "https://resource.example.com",
    "authorization_servers":
      ["https://as1.example.com",
       "https://as2.example.net"],
    "bearer_methods_supported":
      ["header", "body"],
    "scopes_supported":
      ["profile", "email", "phone"],
    "resource_documentation":
      "https://resource.example.com/resource_documentation.html"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://resource.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().resource(), "https://resource.example.com");
  EXPECT_EQ(metadata.value().first_authorization_server().value(),
            "https://as1.example.com");
  EXPECT_TRUE(metadata.value().supports_authorization_server(
      "https://as1.example.com"));
  EXPECT_TRUE(metadata.value().supports_authorization_server(
      "https://as2.example.net"));
  EXPECT_TRUE(metadata.value().supports_bearer_method("header"));
  EXPECT_TRUE(metadata.value().supports_bearer_method("body"));
  EXPECT_FALSE(metadata.value().supports_bearer_method("query"));
  EXPECT_TRUE(metadata.value().supports_scope("profile"));
  EXPECT_TRUE(metadata.value().supports_scope("email"));
  EXPECT_TRUE(metadata.value().supports_scope("phone"));
  EXPECT_EQ(metadata.value().resource_documentation().value(),
            "https://resource.example.com/resource_documentation.html");
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(metadata.value().dpop_bound_access_tokens_required());
  EXPECT_FALSE(metadata.value().tls_client_certificate_bound_access_tokens());
}
