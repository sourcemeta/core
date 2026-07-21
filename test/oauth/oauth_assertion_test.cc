#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <optional>    // std::optional
#include <string_view> // std::string_view

static constexpr std::string_view EC_PRIVATE_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ",)"
    R"("d":"ttepxcp-OwXCj4-v4sGcxRxQRXA8D5Svu02yhcHvbd0"})"};

static constexpr std::string_view JWKS_JSON{
    R"({"keys":[{"kty":"EC","crv":"P-256",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ"}]})"};

// A different P-256 key, so an assertion signed with the key above does not
// verify against it
static constexpr std::string_view OTHER_JWKS_JSON{
    R"({"keys":[{"kty":"EC","crv":"P-256",)"
    R"("x":"l8tFrhx-34tV3hRICRDY9zCkDlpBhF42UQUfWVAWBFs",)"
    R"("y":"9VE4jf_Ok_o64zbTTlcuNJajHmt6v9TDVrU0CdvGRDA"}]})"};

// The signing key carries a key identifier, so the built assertion names it in
// the header
static constexpr std::string_view EC_PRIVATE_JWK_WITH_KID{
    R"({"kty":"EC","crv":"P-256","kid":"key-1",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ",)"
    R"("d":"ttepxcp-OwXCj4-v4sGcxRxQRXA8D5Svu02yhcHvbd0"})"};

// A different key published under the same identifier, so a named-key lookup
// finds it but its signature does not verify
static constexpr std::string_view OTHER_JWKS_WITH_KID{
    R"({"keys":[{"kty":"EC","crv":"P-256","kid":"key-1",)"
    R"("x":"l8tFrhx-34tV3hRICRDY9zCkDlpBhF42UQUfWVAWBFs",)"
    R"("y":"9VE4jf_Ok_o64zbTTlcuNJajHmt6v9TDVrU0CdvGRDA"}]})"};

static constexpr std::string_view OCT_JWK{
    R"({"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T)"
    R"(-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})"};

static constexpr std::string_view OCT_JWKS_JSON{
    R"({"keys":[{"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQ)"
    R"(Lr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"}]})"};

static constexpr std::string_view OTHER_OCT_JWKS_JSON{
    R"({"keys":[{"kty":"oct","k":"b3RoZXItc2VjcmV0LWtleS1mb3ItaG1hYy10ZXN0aW5n"}]})"};

static const auto FIXED_TIME{
    std::chrono::system_clock::from_time_t(1562262616)};

static const std::array<std::string_view, 1> AUDIENCES{
    {"https://server.example/token"}};
static const std::array<sourcemeta::core::JWSAlgorithm, 1> ALLOWED{
    {sourcemeta::core::JWSAlgorithm::ES256}};

TEST(grant_type_and_assertion_type_constants) {
  EXPECT_EQ(sourcemeta::core::OAUTH_GRANT_TYPE_JWT_BEARER,
            "urn:ietf:params:oauth:grant-type:jwt-bearer");
  EXPECT_EQ(sourcemeta::core::OAUTH_CLIENT_ASSERTION_TYPE_JWT_BEARER,
            "urn:ietf:params:oauth:client-assertion-type:jwt-bearer");
}

TEST(client_assertion_body_emits_the_parameters) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_client_assertion("eyJhbGc", body);
  EXPECT_EQ(body, "client_assertion_type=urn%3Aietf%3Aparams%3Aoauth%3Aclient-"
                  "assertion-type%3Ajwt-bearer&client_assertion=eyJhbGc");
}

TEST(jwt_bearer_body_emits_the_grant_and_scope) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_jwt_bearer("eyJhbGc", "read",
                                                         body);
  EXPECT_EQ(body, "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-"
                  "bearer&assertion=eyJhbGc&scope=read");
}

TEST(jwt_bearer_body_omits_an_empty_scope) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_jwt_bearer("eyJhbGc", "", body);
  EXPECT_EQ(body, "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-"
                  "bearer&assertion=eyJhbGc");
}

TEST(build_client_assertion_carries_the_expected_claims) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());

  const auto token{sourcemeta::core::JWT::from(assertion.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::ES256);
  EXPECT_EQ(token.value().issuer().value(), "s6BhdRkqt3");
  EXPECT_EQ(token.value().subject().value(), "s6BhdRkqt3");
  EXPECT_TRUE(token.value().has_audience("https://server.example/token"));
  EXPECT_TRUE(token.value().token_id().has_value());
  EXPECT_EQ(token.value().issued_at().value(), FIXED_TIME);
  EXPECT_EQ(token.value().expires_at().value(),
            FIXED_TIME + std::chrono::seconds{300});
}

TEST(verify_client_assertion_accepts_a_valid_assertion) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());

  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME, options)
                   .has_value());
}

TEST(verify_client_assertion_accepts_without_a_presented_client_id) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_FALSE(
      sourcemeta::core::oauth_verify_client_assertion(
          assertion.value(), AUDIENCES, "", keys.value(), FIXED_TIME, options)
          .has_value());
}

TEST(verify_client_assertion_rejects_a_client_id_mismatch) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "other-client", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Subject);
}

TEST(verify_client_assertion_rejects_a_non_self_issued_assertion) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Issuer and subject differ, which is invalid for client authentication
  const auto assertion{sourcemeta::core::oauth_build_assertion(
      "issuer", "s6BhdRkqt3", "https://server.example/token",
      std::chrono::seconds{300}, FIXED_TIME, key.value(),
      sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Issuer);
}

TEST(verify_client_assertion_rejects_a_wrong_audience) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://other.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Audience);
}

TEST(verify_client_assertion_matches_one_of_several_audiences) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/par", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  const std::array<std::string_view, 2> audiences{
      {"https://server.example/token", "https://server.example/par"}};
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   assertion.value(), audiences, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME, options)
                   .has_value());
}

TEST(verify_client_assertion_rejects_an_expired_assertion) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME + std::chrono::seconds{400}, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Expired);
}

TEST(verify_client_assertion_rejects_an_unsupported_algorithm) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::ES384}};
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = allowed;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::UnsupportedAlgorithm);
}

TEST(verify_client_assertion_rejects_a_malformed_assertion) {
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                "not-a-jwt", AUDIENCES, "s6BhdRkqt3", keys.value(), FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Malformed);
}

TEST(verify_client_assertion_rejects_an_unknown_key) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{sourcemeta::core::JWKS::from(
      sourcemeta::core::parse_json(OTHER_JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::UnknownKey);
}

TEST(verify_assertion_grant_accepts_a_valid_grant) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_assertion(
      "https://issuer.example", "user123", "https://server.example/token",
      std::chrono::seconds{300}, FIXED_TIME, key.value(),
      sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_FALSE(sourcemeta::core::oauth_verify_assertion_grant(
                   assertion.value(), "https://issuer.example", AUDIENCES,
                   keys.value(), FIXED_TIME, options)
                   .has_value());
}

TEST(verify_assertion_grant_rejects_a_wrong_issuer) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_assertion(
      "https://issuer.example", "user123", "https://server.example/token",
      std::chrono::seconds{300}, FIXED_TIME, key.value(),
      sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_assertion_grant(
                assertion.value(), "https://other.example", AUDIENCES,
                keys.value(), FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Issuer);
}

TEST(verify_assertion_rejects_a_replayed_identifier) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthDPoPReplayStore store;
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  options.replay_store = &store;
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME, options)
                   .has_value());
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Replay);
}

TEST(build_assertion_carries_distinct_issuer_and_subject) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_assertion(
      "https://issuer.example", "user123", "https://server.example/token",
      std::chrono::seconds{300}, FIXED_TIME, key.value(),
      sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());

  const auto token{sourcemeta::core::JWT::from(assertion.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().issuer().value(), "https://issuer.example");
  EXPECT_EQ(token.value().subject().value(), "user123");
  EXPECT_FALSE(token.value().key_id().has_value());
  EXPECT_EQ(token.value().issued_at().value(), FIXED_TIME);
  EXPECT_EQ(token.value().expires_at().value(),
            FIXED_TIME + std::chrono::seconds{300});
}

TEST(build_assertion_emits_the_key_id_header_when_present) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK_WITH_KID))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto token{sourcemeta::core::JWT::from(assertion.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().key_id().value(), "key-1");
}

TEST(verify_client_assertion_rejects_a_bad_signature_for_a_named_key) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK_WITH_KID))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{sourcemeta::core::JWKS::from(
      sourcemeta::core::parse_json(OTHER_JWKS_WITH_KID))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Signature);
}

TEST(verify_client_assertion_accepts_a_symmetric_assertion) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::HS256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{sourcemeta::core::JWKS::from(
      sourcemeta::core::parse_json(OCT_JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::HS256}};
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = allowed;
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME, options)
                   .has_value());
}

TEST(verify_client_assertion_rejects_a_wrong_symmetric_secret) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::HS256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{sourcemeta::core::JWKS::from(
      sourcemeta::core::parse_json(OTHER_OCT_JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::HS256}};
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = allowed;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::UnknownKey);
}

TEST(verify_client_assertion_rejects_when_no_algorithm_is_allowed) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  const sourcemeta::core::OAuthAssertionVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::UnsupportedAlgorithm);
}

TEST(verify_client_assertion_rejects_a_not_yet_valid_assertion) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto payload{sourcemeta::core::parse_json(
      R"({"iss":"s6BhdRkqt3","sub":"s6BhdRkqt3",)"
      R"("aud":"https://server.example/token",)"
      R"("exp":1562262916,"iat":1562262616,"nbf":1562263216})")};
  const auto assertion{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(R"({"alg":"ES256"})"), payload,
      key.value())};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::NotYetValid);
}

TEST(verify_client_assertion_rejects_an_assertion_issued_in_the_future) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME - std::chrono::seconds{100}, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::IssuedInFuture);
}

TEST(verify_client_assertion_tolerates_expiry_within_clock_skew) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  options.clock_skew = std::chrono::seconds{60};
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME + std::chrono::seconds{330}, options)
                   .has_value());
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME + std::chrono::seconds{400}, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Expired);
}

TEST(verify_client_assertion_rejects_a_replay_within_clock_skew) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_client_assertion(
      "s6BhdRkqt3", "https://server.example/token", std::chrono::seconds{300},
      FIXED_TIME, key.value(), sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthDPoPReplayStore store;
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  options.clock_skew = std::chrono::seconds{60};
  options.replay_store = &store;
  // Accepted ten seconds past expiry, still within the sixty-second skew
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME + std::chrono::seconds{310}, options)
                   .has_value());
  // A replay twenty seconds past expiry, still within skew, is caught because
  // the replay entry outlives the grace window
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME + std::chrono::seconds{320}, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Replay);
}

TEST(verify_client_assertion_allows_reuse_without_an_identifier) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto payload{
      sourcemeta::core::parse_json(R"({"iss":"s6BhdRkqt3","sub":"s6BhdRkqt3",)"
                                   R"("aud":"https://server.example/token",)"
                                   R"("exp":1562262916,"iat":1562262616})")};
  const auto assertion{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(R"({"alg":"ES256"})"), payload,
      key.value())};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthDPoPReplayStore store;
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  options.replay_store = &store;
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME, options)
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME, options)
                   .has_value());
}

TEST(verify_assertion_grant_rejects_a_missing_subject) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto payload{
      sourcemeta::core::parse_json(R"({"iss":"https://issuer.example",)"
                                   R"("aud":"https://server.example/token",)"
                                   R"("exp":1562262916,"iat":1562262616})")};
  const auto assertion{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(R"({"alg":"ES256"})"), payload,
      key.value())};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_assertion_grant(
                assertion.value(), "https://issuer.example", AUDIENCES,
                keys.value(), FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Subject);
}

TEST(verify_assertion_grant_rejects_a_wrong_audience) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto assertion{sourcemeta::core::oauth_build_assertion(
      "https://issuer.example", "user123", "https://other.example/token",
      std::chrono::seconds{300}, FIXED_TIME, key.value(),
      sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_assertion_grant(
                assertion.value(), "https://issuer.example", AUDIENCES,
                keys.value(), FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Audience);
}

TEST(verify_client_assertion_rejects_a_malformed_audience_array) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // One element matches, but another is not a string, so the audience claim is
  // malformed and rejected rather than accepted on the match
  const auto payload{sourcemeta::core::parse_json(
      R"({"iss":"s6BhdRkqt3","sub":"s6BhdRkqt3",)"
      R"("aud":["https://server.example/token",123],)"
      R"("exp":1562262916,"iat":1562262616})")};
  const auto assertion{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(R"({"alg":"ES256"})"), payload,
      key.value())};
  EXPECT_TRUE(assertion.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  EXPECT_EQ(sourcemeta::core::oauth_verify_client_assertion(
                assertion.value(), AUDIENCES, "s6BhdRkqt3", keys.value(),
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthAssertionError::Audience);
}

TEST(verify_client_assertion_keys_replay_by_exact_audience) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  // Two assertions share an identifier but target audiences that differ only by
  // a default port, which URL normalization would collapse. The audience is
  // keyed verbatim, so both are accepted rather than the second being a replay
  const auto first{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(R"({"alg":"ES256"})"),
      sourcemeta::core::parse_json(
          R"({"iss":"s6BhdRkqt3","sub":"s6BhdRkqt3",)"
          R"("aud":"https://server.example/token",)"
          R"("exp":1562262916,"iat":1562262616,"jti":"shared"})"),
      key.value())};
  const auto second{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(R"({"alg":"ES256"})"),
      sourcemeta::core::parse_json(
          R"({"iss":"s6BhdRkqt3","sub":"s6BhdRkqt3",)"
          R"("aud":"https://server.example:443/token",)"
          R"("exp":1562262916,"iat":1562262616,"jti":"shared"})"),
      key.value())};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(JWKS_JSON))};
  EXPECT_TRUE(keys.has_value());
  const std::array<std::string_view, 2> audiences{
      {"https://server.example/token", "https://server.example:443/token"}};
  sourcemeta::core::OAuthDPoPReplayStore store;
  sourcemeta::core::OAuthAssertionVerifyOptions options;
  options.allowed_algorithms = ALLOWED;
  options.replay_store = &store;
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   first.value(), audiences, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME, options)
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_verify_client_assertion(
                   second.value(), audiences, "s6BhdRkqt3", keys.value(),
                   FIXED_TIME, options)
                   .has_value());
}
