#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string_view> // std::string_view
#include <utility>     // std::move

// NOLINTNEXTLINE(cert-err58-cpp,bugprone-throwing-static-initialization)
static const auto VALID_PROVIDER_DOCUMENT{sourcemeta::core::parse_json(R"JSON({
  "issuer": "https://example.com",
  "authorization_endpoint": "https://example.com/authorize",
  "token_endpoint": "https://example.com/token",
  "userinfo_endpoint": "https://example.com/userinfo",
  "jwks_uri": "https://example.com/jwks",
  "response_types_supported": [ "code" ],
  "subject_types_supported": [ "public", "pairwise" ],
  "id_token_signing_alg_values_supported": [ "RS256", "ES256" ],
  "scopes_supported": [ "openid", "profile" ],
  "claims_supported": [ "sub", "name" ]
})JSON")};

TEST(from_parses_a_valid_provider_document) {
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      sourcemeta::core::JSON{VALID_PROVIDER_DOCUMENT}, "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_TRUE(metadata.value().authorization_endpoint().has_value());
  EXPECT_EQ(metadata.value().authorization_endpoint().value(),
            "https://example.com/authorize");
  EXPECT_TRUE(metadata.value().token_endpoint().has_value());
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://example.com/token");
  EXPECT_TRUE(metadata.value().userinfo_endpoint().has_value());
  EXPECT_EQ(metadata.value().userinfo_endpoint().value(),
            "https://example.com/userinfo");
  EXPECT_EQ(metadata.value().jwks_uri(), "https://example.com/jwks");
}

TEST(from_exposes_the_supported_predicates) {
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      sourcemeta::core::JSON{VALID_PROVIDER_DOCUMENT}, "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_subject_type("public"));
  EXPECT_TRUE(metadata.value().supports_subject_type("pairwise"));
  EXPECT_FALSE(metadata.value().supports_subject_type("nonexistent"));
  EXPECT_TRUE(metadata.value().supports_id_token_signing_alg("RS256"));
  EXPECT_TRUE(metadata.value().supports_id_token_signing_alg("ES256"));
  EXPECT_FALSE(metadata.value().supports_id_token_signing_alg("none"));
  EXPECT_TRUE(metadata.value().supports_response_type("code"));
  EXPECT_FALSE(metadata.value().supports_response_type("token"));
  EXPECT_TRUE(metadata.value().supports_scope("openid"));
  EXPECT_TRUE(metadata.value().supports_scope("profile"));
  EXPECT_FALSE(metadata.value().supports_scope("email"));
  EXPECT_TRUE(metadata.value().supports_claim("sub"));
  EXPECT_FALSE(metadata.value().supports_claim("email"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
  EXPECT_FALSE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_post"));
}

TEST(from_exposes_explicit_token_endpoint_auth_methods) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ],
    "token_endpoint_auth_methods_supported":
      [ "client_secret_post", "none" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_post"));
  EXPECT_TRUE(metadata.value().supports_token_endpoint_auth_method("none"));
  EXPECT_FALSE(metadata.value().supports_token_endpoint_auth_method(
      "client_secret_basic"));
}

TEST(from_delegates_to_the_oauth_metadata) {
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      sourcemeta::core::JSON{VALID_PROVIDER_DOCUMENT}, "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().oauth().issuer(), "https://example.com");
  EXPECT_TRUE(metadata.value().oauth().jwks_uri().has_value());
  EXPECT_EQ(metadata.value().oauth().jwks_uri().value(),
            "https://example.com/jwks");
}

TEST(from_reports_no_optional_endpoints_when_absent) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().userinfo_endpoint().has_value());
  EXPECT_FALSE(metadata.value().end_session_endpoint().has_value());
  EXPECT_FALSE(metadata.value().check_session_iframe().has_value());
}

TEST(from_rejects_a_mismatched_issuer) {
  EXPECT_FALSE(sourcemeta::core::OIDCProviderMetadata::from(
                   sourcemeta::core::JSON{VALID_PROVIDER_DOCUMENT},
                   "https://attacker.example")
                   .has_value());
}

TEST(from_rejects_a_missing_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCProviderMetadata::from(
                   std::move(document), "https://example.com")
                   .has_value());
}

TEST(from_rejects_a_missing_subject_types) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCProviderMetadata::from(
                   std::move(document), "https://example.com")
                   .has_value());
}

TEST(from_rejects_an_empty_subject_types) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCProviderMetadata::from(
                   std::move(document), "https://example.com")
                   .has_value());
}

TEST(from_rejects_a_missing_id_token_signing_algs) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCProviderMetadata::from(
                   std::move(document), "https://example.com")
                   .has_value());
}

TEST(from_rejects_an_unsigned_only_id_token_alg_list) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "none" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCProviderMetadata::from(
                   std::move(document), "https://example.com")
                   .has_value());
}

TEST(from_accepts_none_alongside_rs256) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256", "none" ]
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::OIDCProviderMetadata::from(
                  std::move(document), "https://example.com")
                  .has_value());
}

TEST(from_rejects_a_list_without_rs256) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "ES256" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCProviderMetadata::from(
                   std::move(document), "https://example.com")
                   .has_value());
}

TEST(from_rejects_a_cleartext_authorization_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "http://example.com/authorize",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_cleartext_token_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "token_endpoint": "http://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_cleartext_userinfo_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "http://example.com/userinfo",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_cleartext_registration_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "registration_endpoint": "http://example.com/register",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_cleartext_jwks_uri) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "jwks_uri": "http://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_cleartext_end_session_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "end_session_endpoint": "http://example.com/logout",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_cleartext_check_session_iframe) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "check_session_iframe": "http://example.com/checksession",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_cleartext_endpoint_under_an_https_issuer) {
  // The shape a provider behind a misconfigured reverse proxy advertises, an
  // https issuer alongside a cleartext endpoint that would receive the client
  // secret and the authorization code
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "http://internal.example.com:8080/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_cleartext_jwks_uri_on_another_host) {
  // The key set authenticates every user, so a cleartext location on an
  // unrelated host is refused even when every other endpoint is https
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "http://attacker.example.net/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_accepts_every_https_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "userinfo_endpoint": "https://example.com/userinfo",
    "registration_endpoint": "https://example.com/register",
    "end_session_endpoint": "https://example.com/logout",
    "check_session_iframe": "https://example.com/checksession",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().authorization_endpoint().value(),
            "https://example.com/authorize");
  EXPECT_EQ(metadata.value().token_endpoint().value(),
            "https://example.com/token");
  EXPECT_EQ(metadata.value().userinfo_endpoint().value(),
            "https://example.com/userinfo");
  EXPECT_EQ(metadata.value().registration_endpoint().value(),
            "https://example.com/register");
  EXPECT_EQ(metadata.value().end_session_endpoint().value(),
            "https://example.com/logout");
  EXPECT_EQ(metadata.value().check_session_iframe().value(),
            "https://example.com/checksession");
  EXPECT_EQ(metadata.value().jwks_uri(), "https://example.com/jwks");
}

TEST(from_rejects_a_userinfo_endpoint_with_a_fragment) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "https://example.com/userinfo#profile",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_accepts_a_userinfo_endpoint_with_a_query) {
  // OpenID Connect Discovery 1.0 Section 3: the endpoint "MAY contain port,
  // path, and query parameter components"
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "https://example.com:8443/userinfo?tenant=1",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().userinfo_endpoint().value(),
            "https://example.com:8443/userinfo?tenant=1");
}

TEST(from_accepts_an_uppercase_userinfo_scheme) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "HTTPS://example.com/userinfo",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().userinfo_endpoint().value(),
            "HTTPS://example.com/userinfo");
}

TEST(from_rejects_a_non_string_userinfo_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": 42,
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_null_userinfo_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": null,
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_relative_userinfo_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "/userinfo",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_userinfo_endpoint_without_a_host) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "https:///userinfo",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_malformed_userinfo_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "https://example com/userinfo",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_userinfo_endpoint_with_a_template_placeholder) {
  // Braces are outside the RFC 3986 grammar, so this must be refused on the
  // same terms as the endpoints the OAuth layer validates
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "https://example.com/{tenant}/userinfo",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_rejects_a_javascript_scheme_userinfo_endpoint) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "userinfo_endpoint": "javascript:alert(1)",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ]
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_FALSE(metadata.has_value());
}

TEST(make_builds_a_minimal_provider_document) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"RS256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;

  const auto document{sourcemeta::core::oidc_make_provider_metadata(config)};
  EXPECT_TRUE(document.has_value());
  EXPECT_EQ(document.value().at("issuer").to_string(),
            "https://server.example");
  EXPECT_EQ(document.value().at("jwks_uri").to_string(),
            "https://server.example/jwks");
  EXPECT_TRUE(document.value().defines("subject_types_supported"));
  EXPECT_TRUE(
      document.value().defines("id_token_signing_alg_values_supported"));
  EXPECT_FALSE(document.value().defines("userinfo_endpoint"));
  EXPECT_FALSE(document.value().defines("claims_parameter_supported"));
  EXPECT_FALSE(document.value().defines("request_uri_parameter_supported"));
}

TEST(make_emits_capability_flags_that_differ_from_default) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"RS256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;
  config.claims_parameter_supported = true;
  config.request_uri_parameter_supported = false;
  config.require_request_uri_registration = true;

  const auto document{sourcemeta::core::oidc_make_provider_metadata(config)};
  EXPECT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().at("claims_parameter_supported").to_boolean());
  EXPECT_FALSE(
      document.value().at("request_uri_parameter_supported").to_boolean());
  EXPECT_TRUE(
      document.value().at("require_request_uri_registration").to_boolean());
  EXPECT_FALSE(document.value().defines("request_parameter_supported"));
}

TEST(make_round_trips_through_from) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"RS256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;

  auto document{sourcemeta::core::oidc_make_provider_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document).value(), "https://server.example")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().jwks_uri(), "https://server.example/jwks");
}

TEST(make_rejects_a_missing_jwks_uri) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"RS256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;

  EXPECT_FALSE(
      sourcemeta::core::oidc_make_provider_metadata(config).has_value());
}

TEST(make_rejects_none_in_the_id_token_alg_list) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 2> id_token_algs{{"RS256", "none"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;

  EXPECT_FALSE(
      sourcemeta::core::oidc_make_provider_metadata(config).has_value());
}

TEST(make_rejects_a_non_https_userinfo_endpoint) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"RS256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;
  config.userinfo_endpoint = "http://server.example/userinfo";

  EXPECT_FALSE(
      sourcemeta::core::oidc_make_provider_metadata(config).has_value());
}

TEST(make_rejects_a_list_without_rs256) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"ES256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;

  EXPECT_FALSE(
      sourcemeta::core::oidc_make_provider_metadata(config).has_value());
}

TEST(make_rejects_a_fragment_bearing_endpoint) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"RS256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;
  config.userinfo_endpoint = "https://server.example/userinfo#v2";

  EXPECT_FALSE(
      sourcemeta::core::oidc_make_provider_metadata(config).has_value());
}

TEST(make_accepts_a_mixed_case_https_scheme) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"RS256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;
  config.userinfo_endpoint = "HTTPS://server.example/userinfo";

  EXPECT_TRUE(
      sourcemeta::core::oidc_make_provider_metadata(config).has_value());
}

TEST(from_lifts_validated_oauth_metadata) {
  // The shape a caching resolver hands back, lifted without a reparse
  auto oauth{sourcemeta::core::OAuthServerMetadata::from(
      sourcemeta::core::JSON{VALID_PROVIDER_DOCUMENT}, "https://example.com")};
  EXPECT_TRUE(oauth.has_value());
  const auto metadata{
      sourcemeta::core::OIDCProviderMetadata::from(std::move(oauth).value())};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().issuer(), "https://example.com");
  EXPECT_EQ(metadata.value().jwks_uri(), "https://example.com/jwks");
  EXPECT_EQ(metadata.value().userinfo_endpoint().value(),
            "https://example.com/userinfo");
  EXPECT_TRUE(metadata.value().supports_subject_type("public"));
}

TEST(from_oauth_rejects_a_document_missing_the_oidc_requirements) {
  // Valid as OAuth authorization server metadata, but OpenID Connect promotes
  // jwks_uri to REQUIRED and demands the subject and signing lists
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ]
  })JSON")};
  auto oauth{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(oauth.has_value());
  const auto metadata{
      sourcemeta::core::OIDCProviderMetadata::from(std::move(oauth).value())};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_oauth_rejects_a_signing_list_without_rs256) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "ES256" ]
  })JSON")};
  auto oauth{sourcemeta::core::OAuthServerMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(oauth.has_value());
  const auto metadata{
      sourcemeta::core::OIDCProviderMetadata::from(std::move(oauth).value())};
  EXPECT_FALSE(metadata.has_value());
}

TEST(from_oauth_preserves_the_underlying_oauth_view) {
  auto oauth{sourcemeta::core::OAuthServerMetadata::from(
      sourcemeta::core::JSON{VALID_PROVIDER_DOCUMENT}, "https://example.com")};
  EXPECT_TRUE(oauth.has_value());
  const auto metadata{
      sourcemeta::core::OIDCProviderMetadata::from(std::move(oauth).value())};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().oauth().token_endpoint().value(),
            "https://example.com/token");
  EXPECT_TRUE(metadata.value().oauth().supports_response_type("code"));
}

TEST(from_reports_claims_parameter_support_when_advertised_true) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ],
    "claims_parameter_supported": true
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_claims_parameter());
}

TEST(from_reports_no_claims_parameter_support_when_advertised_false) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ],
    "claims_parameter_supported": false
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_claims_parameter());
}

TEST(from_reports_no_claims_parameter_support_when_absent) {
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      sourcemeta::core::JSON{VALID_PROVIDER_DOCUMENT}, "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_claims_parameter());
}

TEST(from_reports_no_claims_parameter_support_when_malformed) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "issuer": "https://example.com",
    "authorization_endpoint": "https://example.com/authorize",
    "token_endpoint": "https://example.com/token",
    "jwks_uri": "https://example.com/jwks",
    "response_types_supported": [ "code" ],
    "subject_types_supported": [ "public" ],
    "id_token_signing_alg_values_supported": [ "RS256" ],
    "claims_parameter_supported": "true"
  })JSON")};
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document), "https://example.com")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_FALSE(metadata.value().supports_claims_parameter());
}

TEST(make_round_trips_the_claims_parameter_capability) {
  const std::array<std::string_view, 1> response_types{{"code"}};
  const std::array<std::string_view, 1> subject_types{{"public"}};
  const std::array<std::string_view, 1> id_token_algs{{"RS256"}};
  sourcemeta::core::OIDCProviderMetadataConfig config;
  config.base.issuer = "https://server.example";
  config.base.authorization_endpoint = "https://server.example/authorize";
  config.base.token_endpoint = "https://server.example/token";
  config.base.jwks_uri = "https://server.example/jwks";
  config.base.response_types_supported = response_types;
  config.subject_types_supported = subject_types;
  config.id_token_signing_alg_values_supported = id_token_algs;
  config.claims_parameter_supported = true;

  auto document{sourcemeta::core::oidc_make_provider_metadata(config)};
  EXPECT_TRUE(document.has_value());
  const auto metadata{sourcemeta::core::OIDCProviderMetadata::from(
      std::move(document).value(), "https://server.example")};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().supports_claims_parameter());
}
