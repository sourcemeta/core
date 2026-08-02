#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

TEST(well_known_url_authorization_server_without_a_path) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
  EXPECT_EQ(url, "https://example.com/.well-known/oauth-authorization-server");
}

TEST(well_known_url_authorization_server_with_a_path) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/issuer1",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
  EXPECT_EQ(
      url,
      "https://example.com/.well-known/oauth-authorization-server/issuer1");
}

TEST(well_known_url_strips_a_terminating_slash) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
  EXPECT_EQ(url, "https://example.com/.well-known/oauth-authorization-server");
}

TEST(well_known_url_protected_resource) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/resource1",
      sourcemeta::core::OAuthWellKnownKind::ProtectedResource, url));
  EXPECT_EQ(
      url,
      "https://example.com/.well-known/oauth-protected-resource/resource1");
}

TEST(well_known_url_protected_resource_preserves_a_query) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/path?tenant=a",
      sourcemeta::core::OAuthWellKnownKind::ProtectedResource, url));
  EXPECT_EQ(
      url,
      "https://example.com/.well-known/oauth-protected-resource/path?tenant=a");
}

TEST(well_known_url_openid_configuration_inserted) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/issuer1",
      sourcemeta::core::OAuthWellKnownKind::OpenIDConfigurationInserted, url));
  EXPECT_EQ(url,
            "https://example.com/.well-known/openid-configuration/issuer1");
}

TEST(well_known_url_openid_configuration_appended) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/issuer1",
      sourcemeta::core::OAuthWellKnownKind::OpenIDConfigurationAppended, url));
  EXPECT_EQ(url,
            "https://example.com/issuer1/.well-known/openid-configuration");
}

TEST(well_known_url_openid_configuration_appended_without_a_path) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::OpenIDConfigurationAppended, url));
  EXPECT_EQ(url, "https://example.com/.well-known/openid-configuration");
}

TEST(well_known_url_rejects_a_non_https_scheme) {
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_well_known_url(
      "http://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
  EXPECT_TRUE(url.empty());
}

TEST(well_known_url_rejects_a_fragment) {
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/issuer#x",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
}

TEST(well_known_url_rejects_a_query_for_an_issuer) {
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/issuer?x=1",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
}

TEST(well_known_url_appends_to_an_existing_sink) {
  std::string url{"GET "};
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
  EXPECT_EQ(url,
            "GET https://example.com/.well-known/oauth-authorization-server");
}

TEST(server_metadata_parses_a_valid_document) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "registration_endpoint": "https://example.com/register",
    "jwks_uri": "https://example.com/jwks"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_TRUE(metadata.value().authorization_endpoint().has_value());
  EXPECT_EQ(metadata.value().authorization_endpoint().value(),
            "https://example.com/authorize");
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://example.com/token");
  EXPECT_TRUE(metadata.value().registration_endpoint().has_value());
  EXPECT_EQ(metadata.value().registration_endpoint().value(),
            "https://example.com/register");
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_TRUE(metadata.value().jwks_uri().has_value());
  EXPECT_EQ(metadata.value().jwks_uri().value(), "https://example.com/jwks");
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_rejects_an_issuer_mismatch) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://evil.example")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_missing_issuer) {
  auto document{sourcemeta::core::parse_json(
      R"JSON({ "response_types_supported": [ "code" ] })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_missing_response_types) {
  auto document{sourcemeta::core::parse_json(
      R"JSON({ "issuer": "https://example.com" })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_non_https_issuer) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "http://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "http://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_non_object) {
  auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_requires_signing_algs_for_private_key_jwt) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint_auth_methods_supported": [ "private_key_jwt" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_accepts_signing_algs_for_private_key_jwt) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint_auth_methods_supported": [ "private_key_jwt" ],
    "token_endpoint_auth_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_FALSE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_TRUE(
      metadata.value().supports_token_endpoint_auth_method("private_key_jwt"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_rejects_none_in_signing_algs) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint_auth_methods_supported": [ "client_secret_jwt" ],
    "token_endpoint_auth_signing_alg_values_supported": [ "none" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_iss_parameter_supported_defaults_to_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_iss_parameter_supported_reads_true) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_response_iss_parameter_supported": true
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_TRUE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_grant_types_default) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_grant_types_explicit) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "grant_types_supported": [ "authorization_code", "refresh_token" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_FALSE(metadata.value().supports_grant_type("implicit"));
  EXPECT_TRUE(metadata.value().supports_grant_type("refresh_token"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_code_challenge_methods_absent_is_unsupported) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_code_challenge_methods_present) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "code_challenge_methods_supported": [ "S256" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_token_auth_method_default) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_token_auth_method_explicit) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint_auth_methods_supported":
      [ "client_secret_post", "none" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_FALSE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_post"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method("none"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_supports_response_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code", "token" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_TRUE(metadata.value().supports_response_type("token"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_absent_endpoints_are_empty) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_passthrough) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "service_documentation": "https://example.com/docs"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
  const auto *documentation{
      metadata.value().data().try_at("service_documentation")};
  EXPECT_TRUE(documentation != nullptr);
  EXPECT_EQ(documentation->to_string(), "https://example.com/docs");
}

TEST(server_metadata_rejects_an_empty_response_types) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(well_known_url_rejects_an_empty_authority) {
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_well_known_url(
      "https:///path",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
  EXPECT_TRUE(url.empty());
}

TEST(well_known_url_rejects_an_empty_authority_with_query) {
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_well_known_url(
      "https://?tenant=a",
      sourcemeta::core::OAuthWellKnownKind::ProtectedResource, url));
}

TEST(server_metadata_accepts_an_issuer_with_a_path) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com/tenant",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com/tenant")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com/tenant");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_accepts_an_issuer_with_a_trailing_slash) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com/",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com/")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com/");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_rejects_an_empty_issuer) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OAuthServerMetadata::from(std::move(document), "")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_grant_types_wrong_type_falls_back_to_default) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "grant_types_supported": "authorization_code"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_iss_parameter_supported_wrong_type_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_response_iss_parameter_supported": "true"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_non_string_response_type_elements_do_not_match) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ 123 ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(well_known_url_authorization_server_with_a_port) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com:8443/tenant",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
  EXPECT_EQ(
      url,
      "https://example.com:8443/.well-known/oauth-authorization-server/tenant");
}

TEST(well_known_url_multi_segment_path) {
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_well_known_url(
      "https://example.com/a/b/c",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
  EXPECT_EQ(url,
            "https://example.com/.well-known/oauth-authorization-server/a/b/c");
}

TEST(server_metadata_rejects_an_empty_authority_issuer) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https:///path",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https:///path")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_an_empty_signing_alg_list) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint_auth_methods_supported": [ "private_key_jwt" ],
    "token_endpoint_auth_signing_alg_values_supported": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cleartext_authorization_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_endpoint": "http://example.com/authorize"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cleartext_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "http://example.com/token"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cleartext_registration_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "registration_endpoint": "http://example.com/register"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cleartext_par_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "pushed_authorization_request_endpoint": "http://example.com/par"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cleartext_device_authorization_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "device_authorization_endpoint": "http://example.com/device"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_non_string_device_authorization_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "device_authorization_endpoint": 42
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_accepts_an_https_device_authorization_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "device_authorization_endpoint": "https://example.com/device"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_TRUE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_EQ(metadata.value().device_authorization_endpoint().value(),
            "https://example.com/device");
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
  EXPECT_EQ(
      metadata.value().data().at("device_authorization_endpoint").to_string(),
      "https://example.com/device");
}

TEST(server_metadata_rejects_a_cleartext_revocation_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "revocation_endpoint": "http://example.com/revoke"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cleartext_introspection_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "introspection_endpoint": "http://example.com/introspect"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cleartext_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "jwks_uri": "http://example.com/jwks"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cleartext_endpoint_under_an_https_issuer) {
  // The shape a provider behind a misconfigured reverse proxy advertises, an
  // https issuer alongside cleartext endpoints
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "http://internal.example.com:8080/token",
    "jwks_uri": "https://example.com/jwks"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_accepts_every_https_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "registration_endpoint": "https://example.com/register",
    "pushed_authorization_request_endpoint": "https://example.com/par",
    "revocation_endpoint": "https://example.com/revoke",
    "introspection_endpoint": "https://example.com/introspect",
    "jwks_uri": "https://example.com/jwks"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_TRUE(metadata.value().authorization_endpoint().has_value());
  EXPECT_EQ(metadata.value().authorization_endpoint().value(),
            "https://example.com/authorize");
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://example.com/token");
  EXPECT_TRUE(metadata.value().registration_endpoint().has_value());
  EXPECT_EQ(metadata.value().registration_endpoint().value(),
            "https://example.com/register");
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_TRUE(metadata.value().revocation_endpoint().has_value());
  EXPECT_EQ(metadata.value().revocation_endpoint().value(),
            "https://example.com/revoke");
  EXPECT_TRUE(metadata.value().introspection_endpoint().has_value());
  EXPECT_EQ(metadata.value().introspection_endpoint().value(),
            "https://example.com/introspect");
  EXPECT_TRUE(metadata.value().jwks_uri().has_value());
  EXPECT_EQ(metadata.value().jwks_uri().value(), "https://example.com/jwks");
  EXPECT_TRUE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_EQ(metadata.value().pushed_authorization_request_endpoint().value(),
            "https://example.com/par");
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_rejects_a_token_endpoint_with_a_fragment) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "https://example.com/token#section"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_jwks_uri_with_a_fragment) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "jwks_uri": "https://example.com/jwks#keys"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_accepts_a_token_endpoint_with_a_query) {
  // RFC 6749 Section 3.2: the endpoint URI "MAY include an
  // "application/x-www-form-urlencoded" formatted query component"
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "https://example.com/token?tenant=1"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://example.com/token?tenant=1");
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_accepts_a_token_endpoint_with_a_port) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "https://example.com:8443/token"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://example.com:8443/token");
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_accepts_an_uppercase_endpoint_scheme) {
  // RFC 3986 Section 3.1 makes the scheme case-insensitive, and unlike the
  // issuer an endpoint is a location rather than an identifier compared by code
  // points
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "HTTPS://example.com/token"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "HTTPS://example.com/token");
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_accepts_a_mixed_case_endpoint_scheme) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "HtTpS://example.com/token"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "HtTpS://example.com/token");
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_rejects_a_non_string_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": 42
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_null_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": null
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_an_object_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": { "url": "https://example.com/token" }
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_an_empty_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": ""
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_relative_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "/token"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_scheme_relative_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "//example.com/token"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_token_endpoint_without_a_host) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "https:///token"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_token_endpoint_with_an_empty_authority) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "https://:8443/token"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_malformed_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "https://example com/token"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_file_scheme_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "file://example.com/token"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_javascript_scheme_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "javascript:alert(1)"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_an_https_prefixed_scheme_token_endpoint) {
  // A scheme merely starting with the expected one is a different scheme
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "httpsx://example.com/token"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_cross_origin_cleartext_jwks_uri) {
  // The key set authenticates every assertion the server makes, so a cleartext
  // location on an unrelated host is refused even when everything else is https
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "http://attacker.example.net/jwks"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(
    resource_metadata_uppercase_authorization_server_does_not_match_lowercase) {
  // The membership test compares by code points, so a case-varied entry never
  // reports a false match for a caller holding the canonical form
  auto document{sourcemeta::core::parse_json(R"JSON({
    "resource": "https://api.example.com",
    "authorization_servers": [ "HTTPS://auth.example.com" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthResourceMetadata::from(
      std::move(document), "https://api.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().resource(), "https://api.example.com");
  EXPECT_TRUE(metadata.value().first_authorization_server().has_value());
  EXPECT_EQ(metadata.value().first_authorization_server().value(),
            "HTTPS://auth.example.com");
  EXPECT_TRUE(metadata.value().supports_authorization_server(
      "HTTPS://auth.example.com"));
  EXPECT_FALSE(metadata.value().supports_authorization_server(
      "https://nonexistent.example.invalid"));
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(metadata.value().supports_bearer_method("header"));
  EXPECT_FALSE(metadata.value().supports_bearer_method("body"));
  EXPECT_FALSE(metadata.value().supports_bearer_method("query"));
  EXPECT_FALSE(metadata.value().supports_scope("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_resource_signing_alg("nonexistent"));
  EXPECT_FALSE(metadata.value().resource_name().has_value());
  EXPECT_FALSE(metadata.value().resource_documentation().has_value());
  EXPECT_FALSE(metadata.value().resource_policy_uri().has_value());
  EXPECT_FALSE(metadata.value().resource_tos_uri().has_value());
  EXPECT_FALSE(metadata.value().tls_client_certificate_bound_access_tokens());
  EXPECT_FALSE(
      metadata.value().supports_authorization_details_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_dpop_signing_alg("nonexistent"));
  EXPECT_FALSE(metadata.value().dpop_bound_access_tokens_required());
}

TEST(server_metadata_rejects_an_empty_host) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://:443",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://:443")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(well_known_url_rejects_an_empty_host) {
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_well_known_url(
      "https://:443", sourcemeta::core::OAuthWellKnownKind::ProtectedResource,
      url));
  EXPECT_TRUE(url.empty());
}

TEST(server_metadata_rejects_a_port_above_the_limit) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com:99999999999999",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com:99999999999999")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(well_known_url_rejects_a_port_above_the_limit) {
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_well_known_url(
      "https://example.com:99999999999999",
      sourcemeta::core::OAuthWellKnownKind::ProtectedResource, url));
  EXPECT_TRUE(url.empty());
}

TEST(make_server_metadata_emits_the_required_members) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "response_types_supported": [ "code" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_emits_endpoints_and_arrays) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 2> grant_types{
      {"authorization_code", "refresh_token"}};
  const std::array<std::string_view, 1> challenge_methods{{"S256"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.grant_types_supported = grant_types;
  config.code_challenge_methods_supported = challenge_methods;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "response_types_supported": [ "code" ],
    "grant_types_supported": [ "authorization_code", "refresh_token" ],
    "code_challenge_methods_supported": [ "S256" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_rejects_an_invalid_issuer) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "http://server.example";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_rejects_empty_response_types) {
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_omits_absent_members) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "response_types_supported": [ "code" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_emits_the_par_members) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.pushed_authorization_request_endpoint = "https://server.example/par";
  config.require_pushed_authorization_requests = true;
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "pushed_authorization_request_endpoint": "https://server.example/par",
    "response_types_supported": [ "code" ],
    "require_pushed_authorization_requests": true
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_omits_require_par_when_false) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.pushed_authorization_request_endpoint = "https://server.example/par";
  config.require_pushed_authorization_requests = false;
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "pushed_authorization_request_endpoint": "https://server.example/par",
    "response_types_supported": [ "code" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_rejects_a_non_https_par_endpoint) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.pushed_authorization_request_endpoint = "http://server.example/par";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_rejects_require_par_without_an_endpoint) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.require_pushed_authorization_requests = true;
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(server_metadata_reads_the_par_members) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "pushed_authorization_request_endpoint": "https://example.com/par",
    "require_pushed_authorization_requests": true
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_TRUE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_EQ(metadata.value().pushed_authorization_request_endpoint().value(),
            "https://example.com/par");
  EXPECT_TRUE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_require_par_defaults_to_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_require_par_wrong_type_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "require_pushed_authorization_requests": "true"
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(make_server_metadata_par_round_trips) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.pushed_authorization_request_endpoint = "https://server.example/par";
  config.require_pushed_authorization_requests = true;
  config.response_types_supported = response_types;
  auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{document.value()};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document.value()), "https://server.example")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://server.example");
  EXPECT_TRUE(metadata.value().authorization_endpoint().has_value());
  EXPECT_EQ(metadata.value().authorization_endpoint().value(),
            "https://server.example/authorize");
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://server.example/token");
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_TRUE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_EQ(metadata.value().pushed_authorization_request_endpoint().value(),
            "https://server.example/par");
  EXPECT_TRUE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(make_server_metadata_rejects_jwt_auth_without_signing_algs) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> auth_methods{{"private_key_jwt"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.token_endpoint_auth_methods_supported = auth_methods;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_accepts_jwt_auth_with_signing_algs) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> auth_methods{{"private_key_jwt"}};
  const std::array<std::string_view, 1> algs{{"RS256"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.token_endpoint_auth_methods_supported = auth_methods;
  config.token_endpoint_auth_signing_alg_values_supported = algs;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "response_types_supported": [ "code" ],
    "token_endpoint_auth_methods_supported": [ "private_key_jwt" ],
    "token_endpoint_auth_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_rejects_jwt_auth_with_a_none_alg) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> auth_methods{{"client_secret_jwt"}};
  const std::array<std::string_view, 2> algs{{"HS256", "none"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.token_endpoint_auth_methods_supported = auth_methods;
  config.token_endpoint_auth_signing_alg_values_supported = algs;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_rejects_an_issuer_with_a_query) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example?x=1";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_rejects_a_missing_authorization_endpoint) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_rejects_a_missing_token_endpoint) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_allows_a_missing_token_endpoint_for_implicit_only) {
  const std::array<std::string_view, 1> response_types{{"token"}};
  const std::array<std::string_view, 1> grant_types{{"implicit"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.response_types_supported = response_types;
  config.grant_types_supported = grant_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "response_types_supported": [ "token" ],
    "grant_types_supported": [ "implicit" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(
    make_server_metadata_allows_omitting_the_authorization_endpoint_for_client_credentials_only) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> grant_types{{"client_credentials"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.grant_types_supported = grant_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "token_endpoint": "https://server.example/token",
    "response_types_supported": [ "code" ],
    "grant_types_supported": [ "client_credentials" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_requires_response_types_even_for_client_credentials) {
  const std::array<std::string_view, 1> grant_types{{"client_credentials"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.token_endpoint = "https://server.example/token";
  config.grant_types_supported = grant_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(
    make_server_metadata_requires_the_authorization_endpoint_for_authorization_code) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> grant_types{{"authorization_code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.grant_types_supported = grant_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_requires_response_types_for_authorization_code) {
  const std::array<std::string_view, 1> grant_types{{"authorization_code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.grant_types_supported = grant_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_rejects_none_without_jwt_methods) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> algs{{"none"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.token_endpoint_auth_signing_alg_values_supported = algs;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_rejects_a_non_https_jwks_uri) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.jwks_uri = "http://server.example/jwks";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(is_endpoint_url_accepts_an_https_url) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_endpoint_url("https://example.com"));
  EXPECT_TRUE(
      sourcemeta::core::oauth_is_endpoint_url("https://example.com/token"));
}

TEST(is_endpoint_url_accepts_a_port_and_a_query) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_endpoint_url(
      "https://example.com:8443/token?tenant=1"));
}

TEST(is_endpoint_url_accepts_an_uppercase_scheme) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_endpoint_url("HTTPS://example.com"));
  EXPECT_TRUE(sourcemeta::core::oauth_is_endpoint_url("HtTpS://example.com"));
}

TEST(is_endpoint_url_rejects_a_cleartext_url) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("http://example.com"));
}

TEST(is_endpoint_url_rejects_a_fragment) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_is_endpoint_url("https://example.com/token#a"));
}

TEST(is_endpoint_url_rejects_a_missing_host) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("https:///token"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("https://:8443/token"));
}

TEST(is_endpoint_url_rejects_a_relative_reference) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("/token"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("//example.com/token"));
}

TEST(is_endpoint_url_rejects_another_scheme) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("file://example.com/a"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("javascript:alert(1)"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("httpsx://example.com"));
}

TEST(is_endpoint_url_rejects_a_malformed_url) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url(""));
  EXPECT_FALSE(sourcemeta::core::oauth_is_endpoint_url("https://ex ample.com"));
}

TEST(is_resource_identifier_accepts_an_https_url) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_resource_identifier(
      "https://api.example.com"));
}

TEST(is_resource_identifier_accepts_a_path) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_resource_identifier(
      "https://api.example.com/mcp"));
}

TEST(is_resource_identifier_accepts_a_query) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_resource_identifier(
      "https://api.example.com/mcp?tenant=1"));
}

TEST(is_resource_identifier_accepts_a_port) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_resource_identifier(
      "https://api.example.com:8443/mcp"));
}

TEST(is_resource_identifier_rejects_a_cleartext_url) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_is_resource_identifier("http://api.example.com"));
}

TEST(is_resource_identifier_rejects_a_fragment) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_resource_identifier(
      "https://api.example.com/mcp#section"));
}

TEST(is_resource_identifier_rejects_an_uppercase_scheme) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_resource_identifier(
      "HTTPS://api.example.com"));
}

TEST(is_resource_identifier_rejects_a_missing_host) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_resource_identifier("https://"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_resource_identifier("https://:443"));
}

TEST(is_resource_identifier_rejects_a_relative_reference) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_resource_identifier("/mcp"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_resource_identifier(""));
}

TEST(is_issuer_identifier_accepts_an_https_url) {
  EXPECT_TRUE(
      sourcemeta::core::oauth_is_issuer_identifier("https://auth.example.com"));
}

TEST(is_issuer_identifier_accepts_a_path) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_issuer_identifier(
      "https://auth.example.com/tenant"));
}

TEST(is_issuer_identifier_rejects_an_uppercase_scheme) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_is_issuer_identifier("HTTPS://auth.example.com"));
}

TEST(is_issuer_identifier_rejects_a_query) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_issuer_identifier(
      "https://auth.example.com/?tenant=1"));
}

TEST(is_issuer_identifier_rejects_a_fragment) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_issuer_identifier(
      "https://auth.example.com#main"));
}

TEST(is_issuer_identifier_rejects_a_cleartext_url) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_is_issuer_identifier("http://auth.example.com"));
}

TEST(is_issuer_identifier_rejects_a_missing_host) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_issuer_identifier("https://"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_issuer_identifier("https://:443"));
}

TEST(is_issuer_identifier_rejects_a_relative_reference) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_issuer_identifier("/issuer"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_issuer_identifier(""));
}

TEST(server_metadata_parses_the_rfc8414_example) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example.com",
    "authorization_endpoint": "https://server.example.com/authorize",
    "token_endpoint": "https://server.example.com/token",
    "token_endpoint_auth_methods_supported":
      ["client_secret_basic", "private_key_jwt"],
    "token_endpoint_auth_signing_alg_values_supported": ["RS256", "ES256"],
    "userinfo_endpoint": "https://server.example.com/userinfo",
    "jwks_uri": "https://server.example.com/jwks.json",
    "registration_endpoint": "https://server.example.com/register",
    "scopes_supported":
      ["openid", "profile", "email", "address", "phone", "offline_access"],
    "response_types_supported": ["code", "code token"],
    "service_documentation":
      "http://server.example.com/service_documentation.html",
    "ui_locales_supported": ["en-US", "en-GB", "en-CA", "fr-FR", "fr-CA"]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://server.example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://server.example.com");
  EXPECT_TRUE(metadata.value().authorization_endpoint().has_value());
  EXPECT_EQ(metadata.value().authorization_endpoint().value(),
            "https://server.example.com/authorize");
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://server.example.com/token");
  EXPECT_TRUE(metadata.value().registration_endpoint().has_value());
  EXPECT_EQ(metadata.value().registration_endpoint().value(),
            "https://server.example.com/register");
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_TRUE(metadata.value().jwks_uri().has_value());
  EXPECT_EQ(metadata.value().jwks_uri().value(),
            "https://server.example.com/jwks.json");
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_TRUE(metadata.value().supports_response_type("code token"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_TRUE(
      metadata.value().supports_token_endpoint_auth_method("private_key_jwt"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(make_server_metadata_emits_protected_resources) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 2> resources{
      {"https://api.example.com", "https://reports.example.com/v1"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.protected_resources = resources;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "response_types_supported": [ "code" ],
    "protected_resources":
      [ "https://api.example.com", "https://reports.example.com/v1" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_omits_empty_protected_resources) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "response_types_supported": [ "code" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_rejects_a_cleartext_protected_resource) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> resources{{"http://api.example.com"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.protected_resources = resources;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_rejects_a_protected_resource_with_a_fragment) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> resources{
      {"https://api.example.com#section"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.protected_resources = resources;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}

TEST(make_server_metadata_accepts_a_protected_resource_with_a_query) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> resources{
      {"https://api.example.com/mcp?tenant=1"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.protected_resources = resources;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "authorization_endpoint": "https://server.example/authorize",
    "token_endpoint": "https://server.example/token",
    "response_types_supported": [ "code" ],
    "protected_resources": [ "https://api.example.com/mcp?tenant=1" ]
  })JSON")};
  EXPECT_EQ(document.value(), expected);
}

TEST(make_server_metadata_protected_resources_round_trips) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> resources{{"https://api.example.com"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.protected_resources = resources;
  auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto expected{document.value()};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document.value()), "https://server.example")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://server.example");
  EXPECT_TRUE(metadata.value().authorization_endpoint().has_value());
  EXPECT_EQ(metadata.value().authorization_endpoint().value(),
            "https://server.example/authorize");
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://server.example/token");
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_TRUE(
      metadata.value().supports_protected_resource("https://api.example.com"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_supports_protected_resource) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "response_types_supported": [ "code" ],
    "protected_resources": [ "https://api.example.com" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://server.example")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://server.example");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_TRUE(
      metadata.value().supports_protected_resource("https://api.example.com"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_protected_resource_absent_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto expected{document};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://server.example")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().data(), expected);
  EXPECT_EQ(metadata.value().issuer(), "https://server.example");
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().device_authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
  EXPECT_FALSE(
      metadata.value().pushed_authorization_request_endpoint().has_value());
  EXPECT_FALSE(metadata.value().require_pushed_authorization_requests());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(
      metadata.value().supports_token_endpoint_auth_method("nonexistent"));
  EXPECT_FALSE(metadata.value().supports_protected_resource(
      "https://nonexistent.example.invalid"));
}

TEST(server_metadata_rejects_an_empty_protected_resources_array) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "response_types_supported": [ "code" ],
    "protected_resources": []
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://server.example")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_non_array_protected_resources) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "response_types_supported": [ "code" ],
    "protected_resources": "https://api.example.com"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://server.example")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(server_metadata_rejects_a_non_string_protected_resource_element) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://server.example",
    "response_types_supported": [ "code" ],
    "protected_resources": [ "https://api.example.com", 42 ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://server.example")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(make_server_metadata_rejects_an_uppercase_protected_resource_scheme) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> resources{{"HTTPS://api.example.com"}};
  sourcemeta::core::OAuthServerMetadataConfig config;
  config.issuer = "https://server.example";
  config.authorization_endpoint = "https://server.example/authorize";
  config.token_endpoint = "https://server.example/token";
  config.response_types_supported = response_types;
  config.protected_resources = resources;
  const auto document{sourcemeta::core::oauth_make_server_metadata(config)};
  EXPECT_FALSE(document.has_value());
}
