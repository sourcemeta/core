#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

TEST(authentication_error_interaction_required) {
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::InteractionRequired),
            "interaction_required");
  const auto parsed{
      sourcemeta::core::to_oidc_authentication_error("interaction_required")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(),
            sourcemeta::core::OIDCAuthenticationError::InteractionRequired);
}

TEST(authentication_error_login_required) {
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::LoginRequired),
            "login_required");
  const auto parsed{
      sourcemeta::core::to_oidc_authentication_error("login_required")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(),
            sourcemeta::core::OIDCAuthenticationError::LoginRequired);
}

TEST(authentication_error_account_selection_required) {
  EXPECT_EQ(
      sourcemeta::core::oidc_error_code(
          sourcemeta::core::OIDCAuthenticationError::AccountSelectionRequired),
      "account_selection_required");
  const auto parsed{sourcemeta::core::to_oidc_authentication_error(
      "account_selection_required")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(
      parsed.value(),
      sourcemeta::core::OIDCAuthenticationError::AccountSelectionRequired);
}

TEST(authentication_error_consent_required) {
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::ConsentRequired),
            "consent_required");
  const auto parsed{
      sourcemeta::core::to_oidc_authentication_error("consent_required")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(),
            sourcemeta::core::OIDCAuthenticationError::ConsentRequired);
}

TEST(authentication_error_invalid_request_uri) {
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::InvalidRequestURI),
            "invalid_request_uri");
  const auto parsed{
      sourcemeta::core::to_oidc_authentication_error("invalid_request_uri")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(),
            sourcemeta::core::OIDCAuthenticationError::InvalidRequestURI);
}

TEST(authentication_error_invalid_request_object) {
  EXPECT_EQ(
      sourcemeta::core::oidc_error_code(
          sourcemeta::core::OIDCAuthenticationError::InvalidRequestObject),
      "invalid_request_object");
  const auto parsed{
      sourcemeta::core::to_oidc_authentication_error("invalid_request_object")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(),
            sourcemeta::core::OIDCAuthenticationError::InvalidRequestObject);
}

TEST(authentication_error_request_not_supported) {
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::RequestNotSupported),
            "request_not_supported");
  const auto parsed{
      sourcemeta::core::to_oidc_authentication_error("request_not_supported")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(),
            sourcemeta::core::OIDCAuthenticationError::RequestNotSupported);
}

TEST(authentication_error_request_uri_not_supported) {
  EXPECT_EQ(
      sourcemeta::core::oidc_error_code(
          sourcemeta::core::OIDCAuthenticationError::RequestURINotSupported),
      "request_uri_not_supported");
  const auto parsed{sourcemeta::core::to_oidc_authentication_error(
      "request_uri_not_supported")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(),
            sourcemeta::core::OIDCAuthenticationError::RequestURINotSupported);
}

TEST(authentication_error_registration_not_supported) {
  EXPECT_EQ(
      sourcemeta::core::oidc_error_code(
          sourcemeta::core::OIDCAuthenticationError::RegistrationNotSupported),
      "registration_not_supported");
  const auto parsed{sourcemeta::core::to_oidc_authentication_error(
      "registration_not_supported")};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(
      parsed.value(),
      sourcemeta::core::OIDCAuthenticationError::RegistrationNotSupported);
}

TEST(to_authentication_error_rejects_an_unknown_code) {
  EXPECT_FALSE(
      sourcemeta::core::to_oidc_authentication_error("nonexistent_error")
          .has_value());
}

TEST(to_authentication_error_rejects_an_empty_code) {
  EXPECT_FALSE(sourcemeta::core::to_oidc_authentication_error("").has_value());
}

TEST(to_authentication_error_rejects_an_oauth_code) {
  EXPECT_FALSE(sourcemeta::core::to_oidc_authentication_error("access_denied")
                   .has_value());
}
