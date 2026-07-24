#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string_view> // std::string_view

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
