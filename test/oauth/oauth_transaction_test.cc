#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

TEST(transaction_mint_produces_distinct_secrets) {
  const auto secrets{sourcemeta::core::oauth_transaction_mint()};
  const std::string_view state{secrets.state.data(), secrets.state.size()};
  const std::string_view verifier{secrets.code_verifier.data(),
                                  secrets.code_verifier.size()};
  EXPECT_EQ(state.size(), 43);
  EXPECT_EQ(verifier.size(), 43);
  EXPECT_FALSE(state == verifier);
}

TEST(transaction_mint_differs_between_calls) {
  const auto first{sourcemeta::core::oauth_transaction_mint()};
  const auto second{sourcemeta::core::oauth_transaction_mint()};
  EXPECT_FALSE((std::string_view{first.state.data(), first.state.size()}) ==
               (std::string_view{second.state.data(), second.state.size()}));
}

TEST(transaction_check_accepts_a_valid_callback) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz", .code_verifier = "", .issuer = "", .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "SplxlOBeZQQYbYS6WxSbIA",
      .state = "xyz",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_FALSE(error.has_value());
  EXPECT_EQ(code, "SplxlOBeZQQYbYS6WxSbIA");
}

TEST(transaction_check_rejects_a_state_mismatch) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz", .code_verifier = "", .issuer = "", .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "different",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::State);
}

TEST(transaction_check_rejects_a_missing_state) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz", .code_verifier = "", .issuer = "", .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::State);
}

TEST(transaction_check_accepts_a_matching_received_uri) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "",
      .redirect_uri = "https://client.example/cb"};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xyz",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown,
      "https://client.example/cb", code)};
  EXPECT_FALSE(error.has_value());
  EXPECT_EQ(code, "abc");
}

TEST(transaction_check_rejects_a_received_uri_mismatch) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "",
      .redirect_uri = "https://client.example/cb"};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xyz",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown,
      "https://client.example/other", code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::ReceivedURI);
}

TEST(transaction_check_requires_iss_when_supported) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "https://server.example",
      .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xyz",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Supported,
      "", code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::Issuer);
}

TEST(transaction_check_accepts_a_matching_iss_when_supported) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "https://server.example",
      .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xyz",
      .iss = "https://server.example",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Supported,
      "", code)};
  EXPECT_FALSE(error.has_value());
  EXPECT_EQ(code, "abc");
}

TEST(transaction_check_rejects_a_mismatched_iss_when_supported) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "https://server.example",
      .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xyz",
      .iss = "https://evil.example",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Supported,
      "", code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::Issuer);
}

TEST(transaction_check_validates_a_present_iss_when_unknown) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "https://server.example",
      .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xyz",
      .iss = "https://evil.example",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::Issuer);
}

TEST(transaction_check_ignores_an_absent_iss_when_unknown) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "https://server.example",
      .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xyz",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_FALSE(error.has_value());
  EXPECT_EQ(code, "abc");
}

TEST(transaction_check_ignores_a_present_iss_when_not_supported) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "https://server.example",
      .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xyz",
      .iss = "https://evil.example",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::NotSupported,
      "", code)};
  EXPECT_FALSE(error.has_value());
  EXPECT_EQ(code, "abc");
}

TEST(transaction_check_reports_a_decline) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz", .code_verifier = "", .issuer = "", .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "",
      .state = "xyz",
      .iss = "",
      .error = "access_denied",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::Declined);
}

TEST(transaction_check_reports_a_missing_code) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz", .code_verifier = "", .issuer = "", .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "",
      .state = "xyz",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::MissingCode);
}

TEST(transaction_check_validates_state_before_a_decline) {
  // A crafted error response with a wrong state is a forgery, not a genuine
  // decline, so the state check takes precedence
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz", .code_verifier = "", .issuer = "", .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "",
      .state = "wrong",
      .iss = "",
      .error = "access_denied",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::State);
}

TEST(transaction_check_state_of_a_different_length_fails) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz", .code_verifier = "", .issuer = "", .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "xy",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::State);
}

TEST(transaction_check_empty_transaction_state_rejects) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "", .code_verifier = "", .issuer = "", .redirect_uri = ""};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "anything",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown, "",
      code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::State);
}

TEST(transaction_check_state_takes_precedence_over_received_uri) {
  const sourcemeta::core::OAuthTransaction transaction{
      .state = "xyz",
      .code_verifier = "",
      .issuer = "",
      .redirect_uri = "https://client.example/cb"};
  const sourcemeta::core::OAuthAuthorizationResponse response{
      .code = "abc",
      .state = "wrong",
      .iss = "",
      .error = "",
      .error_description = "",
      .error_uri = ""};
  std::string_view code;
  const auto error{sourcemeta::core::oauth_transaction_check(
      transaction, response, sourcemeta::core::OAuthIssuerSupport::Unknown,
      "https://client.example/other", code)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::OAuthCallbackError::State);
}
