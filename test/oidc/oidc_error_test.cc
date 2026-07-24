#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

TEST(serialize_authentication_error_codes) {
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::InteractionRequired),
            "interaction_required");
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::LoginRequired),
            "login_required");
  EXPECT_EQ(
      sourcemeta::core::oidc_error_code(
          sourcemeta::core::OIDCAuthenticationError::AccountSelectionRequired),
      "account_selection_required");
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::ConsentRequired),
            "consent_required");
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::InvalidRequestURI),
            "invalid_request_uri");
  EXPECT_EQ(
      sourcemeta::core::oidc_error_code(
          sourcemeta::core::OIDCAuthenticationError::InvalidRequestObject),
      "invalid_request_object");
  EXPECT_EQ(sourcemeta::core::oidc_error_code(
                sourcemeta::core::OIDCAuthenticationError::RequestNotSupported),
            "request_not_supported");
  EXPECT_EQ(
      sourcemeta::core::oidc_error_code(
          sourcemeta::core::OIDCAuthenticationError::RequestURINotSupported),
      "request_uri_not_supported");
  EXPECT_EQ(
      sourcemeta::core::oidc_error_code(
          sourcemeta::core::OIDCAuthenticationError::RegistrationNotSupported),
      "registration_not_supported");
}

TEST(parse_authentication_error_codes) {
  const auto interaction_required{
      sourcemeta::core::to_oidc_authentication_error("interaction_required")};
  EXPECT_TRUE(interaction_required.has_value());
  EXPECT_EQ(interaction_required.value(),
            sourcemeta::core::OIDCAuthenticationError::InteractionRequired);
  const auto login_required{
      sourcemeta::core::to_oidc_authentication_error("login_required")};
  EXPECT_TRUE(login_required.has_value());
  EXPECT_EQ(login_required.value(),
            sourcemeta::core::OIDCAuthenticationError::LoginRequired);
  const auto account_selection_required{
      sourcemeta::core::to_oidc_authentication_error(
          "account_selection_required")};
  EXPECT_TRUE(account_selection_required.has_value());
  EXPECT_EQ(
      account_selection_required.value(),
      sourcemeta::core::OIDCAuthenticationError::AccountSelectionRequired);
  const auto consent_required{
      sourcemeta::core::to_oidc_authentication_error("consent_required")};
  EXPECT_TRUE(consent_required.has_value());
  EXPECT_EQ(consent_required.value(),
            sourcemeta::core::OIDCAuthenticationError::ConsentRequired);
  const auto invalid_request_uri{
      sourcemeta::core::to_oidc_authentication_error("invalid_request_uri")};
  EXPECT_TRUE(invalid_request_uri.has_value());
  EXPECT_EQ(invalid_request_uri.value(),
            sourcemeta::core::OIDCAuthenticationError::InvalidRequestURI);
  const auto invalid_request_object{
      sourcemeta::core::to_oidc_authentication_error("invalid_request_object")};
  EXPECT_TRUE(invalid_request_object.has_value());
  EXPECT_EQ(invalid_request_object.value(),
            sourcemeta::core::OIDCAuthenticationError::InvalidRequestObject);
  const auto request_not_supported{
      sourcemeta::core::to_oidc_authentication_error("request_not_supported")};
  EXPECT_TRUE(request_not_supported.has_value());
  EXPECT_EQ(request_not_supported.value(),
            sourcemeta::core::OIDCAuthenticationError::RequestNotSupported);
  const auto request_uri_not_supported{
      sourcemeta::core::to_oidc_authentication_error(
          "request_uri_not_supported")};
  EXPECT_TRUE(request_uri_not_supported.has_value());
  EXPECT_EQ(request_uri_not_supported.value(),
            sourcemeta::core::OIDCAuthenticationError::RequestURINotSupported);
  const auto registration_not_supported{
      sourcemeta::core::to_oidc_authentication_error(
          "registration_not_supported")};
  EXPECT_TRUE(registration_not_supported.has_value());
  EXPECT_EQ(
      registration_not_supported.value(),
      sourcemeta::core::OIDCAuthenticationError::RegistrationNotSupported);
}

TEST(parse_authentication_error_rejects_unknown) {
  EXPECT_FALSE(
      sourcemeta::core::to_oidc_authentication_error("nonexistent_error")
          .has_value());
  EXPECT_FALSE(sourcemeta::core::to_oidc_authentication_error("").has_value());
  EXPECT_FALSE(sourcemeta::core::to_oidc_authentication_error("access_denied")
                   .has_value());
}
