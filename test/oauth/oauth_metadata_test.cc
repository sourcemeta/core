#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <string>  // std::string
#include <utility> // std::move

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
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_TRUE(metadata.value().authorization_endpoint().has_value());
  EXPECT_EQ(metadata.value().authorization_endpoint().value(),
            "https://example.com/authorize");
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://example.com/token");
  EXPECT_EQ(metadata.value().registration_endpoint().value(),
            "https://example.com/register");
  EXPECT_EQ(metadata.value().jwks_uri().value(), "https://example.com/jwks");
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
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
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
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
}

TEST(server_metadata_iss_parameter_supported_reads_true) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_response_iss_parameter_supported": true
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(
      metadata.value().authorization_response_iss_parameter_supported());
}

TEST(server_metadata_grant_types_default) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
  EXPECT_FALSE(metadata.value().supports_grant_type("refresh_token"));
}

TEST(server_metadata_grant_types_explicit) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "grant_types_supported": [ "authorization_code", "refresh_token" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("refresh_token"));
  EXPECT_FALSE(metadata.value().supports_grant_type("implicit"));
}

TEST(server_metadata_code_challenge_methods_absent_is_unsupported) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("S256"));
}

TEST(server_metadata_code_challenge_methods_present) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "code_challenge_methods_supported": [ "S256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.value().supports_code_challenge_method("S256"));
  EXPECT_FALSE(metadata.value().supports_code_challenge_method("plain"));
}

TEST(server_metadata_token_auth_method_default) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_post"));
}

TEST(server_metadata_supports_response_type) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code", "token" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_TRUE(metadata.value().supports_response_type("token"));
  EXPECT_FALSE(metadata.value().supports_response_type("id_token"));
}

TEST(server_metadata_absent_endpoints_are_empty) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.value().authorization_endpoint().has_value());
  EXPECT_FALSE(metadata.value().token_endpoint().has_value());
  EXPECT_FALSE(metadata.value().registration_endpoint().has_value());
  EXPECT_FALSE(metadata.value().revocation_endpoint().has_value());
  EXPECT_FALSE(metadata.value().introspection_endpoint().has_value());
  EXPECT_FALSE(metadata.value().jwks_uri().has_value());
}

TEST(server_metadata_passthrough) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "service_documentation": "https://example.com/docs"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
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
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com/tenant")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().issuer(), "https://example.com/tenant");
}

TEST(server_metadata_accepts_an_issuer_with_a_trailing_slash) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com/",
    "response_types_supported": [ "code" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com/")};
  EXPECT_TRUE(metadata.has_value());
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
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_grant_type("authorization_code"));
  EXPECT_TRUE(metadata.value().supports_grant_type("implicit"));
}

TEST(server_metadata_iss_parameter_supported_wrong_type_is_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "authorization_response_iss_parameter_supported": "true"
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(
      metadata.value().authorization_response_iss_parameter_supported());
}

TEST(server_metadata_non_string_response_type_elements_do_not_match) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ 123 ]
  })JSON")};
  const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_response_type("code"));
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
