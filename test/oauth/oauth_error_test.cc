#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

TEST(serialize_authorization_error_codes) {
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthAuthorizationError::InvalidRequest),
            "invalid_request");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthAuthorizationError::UnauthorizedClient),
            "unauthorized_client");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthAuthorizationError::AccessDenied),
            "access_denied");
  EXPECT_EQ(
      sourcemeta::core::oauth_error_code(
          sourcemeta::core::OAuthAuthorizationError::UnsupportedResponseType),
      "unsupported_response_type");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthAuthorizationError::InvalidScope),
            "invalid_scope");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthAuthorizationError::ServerError),
            "server_error");
  EXPECT_EQ(
      sourcemeta::core::oauth_error_code(
          sourcemeta::core::OAuthAuthorizationError::TemporarilyUnavailable),
      "temporarily_unavailable");
}

TEST(parse_authorization_error_codes) {
  const auto invalid_request{
      sourcemeta::core::to_oauth_authorization_error("invalid_request")};
  EXPECT_TRUE(invalid_request.has_value());
  EXPECT_TRUE(invalid_request.value() ==
              sourcemeta::core::OAuthAuthorizationError::InvalidRequest);
  const auto unauthorized_client{
      sourcemeta::core::to_oauth_authorization_error("unauthorized_client")};
  EXPECT_TRUE(unauthorized_client.has_value());
  EXPECT_TRUE(unauthorized_client.value() ==
              sourcemeta::core::OAuthAuthorizationError::UnauthorizedClient);
  const auto access_denied{
      sourcemeta::core::to_oauth_authorization_error("access_denied")};
  EXPECT_TRUE(access_denied.has_value());
  EXPECT_TRUE(access_denied.value() ==
              sourcemeta::core::OAuthAuthorizationError::AccessDenied);
  const auto unsupported_response_type{
      sourcemeta::core::to_oauth_authorization_error(
          "unsupported_response_type")};
  EXPECT_TRUE(unsupported_response_type.has_value());
  EXPECT_TRUE(
      unsupported_response_type.value() ==
      sourcemeta::core::OAuthAuthorizationError::UnsupportedResponseType);
  const auto invalid_scope{
      sourcemeta::core::to_oauth_authorization_error("invalid_scope")};
  EXPECT_TRUE(invalid_scope.has_value());
  EXPECT_TRUE(invalid_scope.value() ==
              sourcemeta::core::OAuthAuthorizationError::InvalidScope);
  const auto server_error{
      sourcemeta::core::to_oauth_authorization_error("server_error")};
  EXPECT_TRUE(server_error.has_value());
  EXPECT_TRUE(server_error.value() ==
              sourcemeta::core::OAuthAuthorizationError::ServerError);
  const auto temporarily_unavailable{
      sourcemeta::core::to_oauth_authorization_error(
          "temporarily_unavailable")};
  EXPECT_TRUE(temporarily_unavailable.has_value());
  EXPECT_TRUE(
      temporarily_unavailable.value() ==
      sourcemeta::core::OAuthAuthorizationError::TemporarilyUnavailable);
}

TEST(parse_authorization_error_rejects_an_unknown_code) {
  EXPECT_FALSE(sourcemeta::core::to_oauth_authorization_error("invalid_client")
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::to_oauth_authorization_error("").has_value());
}

TEST(serialize_token_error_codes) {
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::InvalidRequest),
            "invalid_request");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::InvalidClient),
            "invalid_client");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::InvalidGrant),
            "invalid_grant");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::UnauthorizedClient),
            "unauthorized_client");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::UnsupportedGrantType),
            "unsupported_grant_type");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::InvalidScope),
            "invalid_scope");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::AuthorizationPending),
            "authorization_pending");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::SlowDown),
            "slow_down");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::AccessDenied),
            "access_denied");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::ExpiredToken),
            "expired_token");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::InvalidTarget),
            "invalid_target");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::InvalidDPoPProof),
            "invalid_dpop_proof");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::UseDPoPNonce),
            "use_dpop_nonce");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthTokenError::UnsupportedTokenType),
            "unsupported_token_type");
}

TEST(parse_token_error_codes) {
  const auto invalid_request{
      sourcemeta::core::to_oauth_token_error("invalid_request")};
  EXPECT_TRUE(invalid_request.has_value());
  EXPECT_TRUE(invalid_request.value() ==
              sourcemeta::core::OAuthTokenError::InvalidRequest);
  const auto invalid_client{
      sourcemeta::core::to_oauth_token_error("invalid_client")};
  EXPECT_TRUE(invalid_client.has_value());
  EXPECT_TRUE(invalid_client.value() ==
              sourcemeta::core::OAuthTokenError::InvalidClient);
  const auto invalid_grant{
      sourcemeta::core::to_oauth_token_error("invalid_grant")};
  EXPECT_TRUE(invalid_grant.has_value());
  EXPECT_TRUE(invalid_grant.value() ==
              sourcemeta::core::OAuthTokenError::InvalidGrant);
  const auto unauthorized_client{
      sourcemeta::core::to_oauth_token_error("unauthorized_client")};
  EXPECT_TRUE(unauthorized_client.has_value());
  EXPECT_TRUE(unauthorized_client.value() ==
              sourcemeta::core::OAuthTokenError::UnauthorizedClient);
  const auto unsupported_grant_type{
      sourcemeta::core::to_oauth_token_error("unsupported_grant_type")};
  EXPECT_TRUE(unsupported_grant_type.has_value());
  EXPECT_TRUE(unsupported_grant_type.value() ==
              sourcemeta::core::OAuthTokenError::UnsupportedGrantType);
  const auto invalid_scope{
      sourcemeta::core::to_oauth_token_error("invalid_scope")};
  EXPECT_TRUE(invalid_scope.has_value());
  EXPECT_TRUE(invalid_scope.value() ==
              sourcemeta::core::OAuthTokenError::InvalidScope);
  const auto authorization_pending{
      sourcemeta::core::to_oauth_token_error("authorization_pending")};
  EXPECT_TRUE(authorization_pending.has_value());
  EXPECT_TRUE(authorization_pending.value() ==
              sourcemeta::core::OAuthTokenError::AuthorizationPending);
  const auto slow_down{sourcemeta::core::to_oauth_token_error("slow_down")};
  EXPECT_TRUE(slow_down.has_value());
  EXPECT_TRUE(slow_down.value() == sourcemeta::core::OAuthTokenError::SlowDown);
  const auto access_denied{
      sourcemeta::core::to_oauth_token_error("access_denied")};
  EXPECT_TRUE(access_denied.has_value());
  EXPECT_TRUE(access_denied.value() ==
              sourcemeta::core::OAuthTokenError::AccessDenied);
  const auto expired_token{
      sourcemeta::core::to_oauth_token_error("expired_token")};
  EXPECT_TRUE(expired_token.has_value());
  EXPECT_TRUE(expired_token.value() ==
              sourcemeta::core::OAuthTokenError::ExpiredToken);
  const auto invalid_target{
      sourcemeta::core::to_oauth_token_error("invalid_target")};
  EXPECT_TRUE(invalid_target.has_value());
  EXPECT_TRUE(invalid_target.value() ==
              sourcemeta::core::OAuthTokenError::InvalidTarget);
  const auto invalid_dpop_proof{
      sourcemeta::core::to_oauth_token_error("invalid_dpop_proof")};
  EXPECT_TRUE(invalid_dpop_proof.has_value());
  EXPECT_TRUE(invalid_dpop_proof.value() ==
              sourcemeta::core::OAuthTokenError::InvalidDPoPProof);
  const auto use_dpop_nonce{
      sourcemeta::core::to_oauth_token_error("use_dpop_nonce")};
  EXPECT_TRUE(use_dpop_nonce.has_value());
  EXPECT_TRUE(use_dpop_nonce.value() ==
              sourcemeta::core::OAuthTokenError::UseDPoPNonce);
  const auto unsupported_token_type{
      sourcemeta::core::to_oauth_token_error("unsupported_token_type")};
  EXPECT_TRUE(unsupported_token_type.has_value());
  EXPECT_TRUE(unsupported_token_type.value() ==
              sourcemeta::core::OAuthTokenError::UnsupportedTokenType);
}

TEST(parse_token_error_rejects_an_unknown_code) {
  EXPECT_FALSE(sourcemeta::core::to_oauth_token_error("temporarily_unavailable")
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::to_oauth_token_error("").has_value());
}

TEST(serialize_bearer_error_codes) {
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthBearerError::InvalidRequest),
            "invalid_request");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthBearerError::InvalidToken),
            "invalid_token");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthBearerError::InsufficientScope),
            "insufficient_scope");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthBearerError::InvalidDPoPProof),
            "invalid_dpop_proof");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthBearerError::UseDPoPNonce),
            "use_dpop_nonce");
}

TEST(parse_bearer_error_codes) {
  const auto invalid_request{
      sourcemeta::core::to_oauth_bearer_error("invalid_request")};
  EXPECT_TRUE(invalid_request.has_value());
  EXPECT_TRUE(invalid_request.value() ==
              sourcemeta::core::OAuthBearerError::InvalidRequest);
  const auto invalid_token{
      sourcemeta::core::to_oauth_bearer_error("invalid_token")};
  EXPECT_TRUE(invalid_token.has_value());
  EXPECT_TRUE(invalid_token.value() ==
              sourcemeta::core::OAuthBearerError::InvalidToken);
  const auto insufficient_scope{
      sourcemeta::core::to_oauth_bearer_error("insufficient_scope")};
  EXPECT_TRUE(insufficient_scope.has_value());
  EXPECT_TRUE(insufficient_scope.value() ==
              sourcemeta::core::OAuthBearerError::InsufficientScope);
  const auto invalid_dpop_proof{
      sourcemeta::core::to_oauth_bearer_error("invalid_dpop_proof")};
  EXPECT_TRUE(invalid_dpop_proof.has_value());
  EXPECT_TRUE(invalid_dpop_proof.value() ==
              sourcemeta::core::OAuthBearerError::InvalidDPoPProof);
  const auto use_dpop_nonce{
      sourcemeta::core::to_oauth_bearer_error("use_dpop_nonce")};
  EXPECT_TRUE(use_dpop_nonce.has_value());
  EXPECT_TRUE(use_dpop_nonce.value() ==
              sourcemeta::core::OAuthBearerError::UseDPoPNonce);
}

TEST(parse_bearer_error_rejects_an_unknown_code) {
  EXPECT_FALSE(
      sourcemeta::core::to_oauth_bearer_error("invalid_grant").has_value());
  EXPECT_FALSE(sourcemeta::core::to_oauth_bearer_error("").has_value());
}

TEST(serialize_registration_error_codes) {
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthRegistrationError::InvalidRedirectURI),
            "invalid_redirect_uri");
  EXPECT_EQ(
      sourcemeta::core::oauth_error_code(
          sourcemeta::core::OAuthRegistrationError::InvalidClientMetadata),
      "invalid_client_metadata");
  EXPECT_EQ(
      sourcemeta::core::oauth_error_code(
          sourcemeta::core::OAuthRegistrationError::InvalidSoftwareStatement),
      "invalid_software_statement");
  EXPECT_EQ(sourcemeta::core::oauth_error_code(
                sourcemeta::core::OAuthRegistrationError::
                    UnapprovedSoftwareStatement),
            "unapproved_software_statement");
}

TEST(parse_registration_error_codes) {
  const auto invalid_redirect_uri{
      sourcemeta::core::to_oauth_registration_error("invalid_redirect_uri")};
  EXPECT_TRUE(invalid_redirect_uri.has_value());
  EXPECT_TRUE(invalid_redirect_uri.value() ==
              sourcemeta::core::OAuthRegistrationError::InvalidRedirectURI);
  const auto invalid_client_metadata{
      sourcemeta::core::to_oauth_registration_error("invalid_client_metadata")};
  EXPECT_TRUE(invalid_client_metadata.has_value());
  EXPECT_TRUE(invalid_client_metadata.value() ==
              sourcemeta::core::OAuthRegistrationError::InvalidClientMetadata);
  const auto invalid_software_statement{
      sourcemeta::core::to_oauth_registration_error(
          "invalid_software_statement")};
  EXPECT_TRUE(invalid_software_statement.has_value());
  EXPECT_TRUE(
      invalid_software_statement.value() ==
      sourcemeta::core::OAuthRegistrationError::InvalidSoftwareStatement);
  const auto unapproved_software_statement{
      sourcemeta::core::to_oauth_registration_error(
          "unapproved_software_statement")};
  EXPECT_TRUE(unapproved_software_statement.has_value());
  EXPECT_TRUE(
      unapproved_software_statement.value() ==
      sourcemeta::core::OAuthRegistrationError::UnapprovedSoftwareStatement);
}

TEST(parse_registration_error_rejects_an_unknown_code) {
  EXPECT_FALSE(sourcemeta::core::to_oauth_registration_error("invalid_request")
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::to_oauth_registration_error("").has_value());
}

TEST(token_error_status_is_401_for_header_client_authentication_failure) {
  const auto status{sourcemeta::core::oauth_token_error_status(
      sourcemeta::core::OAuthTokenError::InvalidClient, true)};
  EXPECT_EQ(status.code, 401);
  EXPECT_EQ(status.phrase, "Unauthorized");
}

TEST(token_error_status_is_400_for_body_client_authentication_failure) {
  const auto status{sourcemeta::core::oauth_token_error_status(
      sourcemeta::core::OAuthTokenError::InvalidClient, false)};
  EXPECT_EQ(status.code, 400);
  EXPECT_EQ(status.phrase, "Bad Request");
}

TEST(token_error_status_is_400_for_other_header_errors) {
  EXPECT_EQ(sourcemeta::core::oauth_token_error_status(
                sourcemeta::core::OAuthTokenError::InvalidGrant, true)
                .code,
            400);
}
