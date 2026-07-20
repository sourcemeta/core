#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>  // std::array
#include <chrono> // std::chrono::seconds
#include <string> // std::string

TEST(build_device_authorization_request_emits_client_id_and_scope) {
  std::string body;
  sourcemeta::core::oauth_build_device_authorization_request("1406020730",
                                                             "read", {}, body);
  EXPECT_EQ(body, "client_id=1406020730&scope=read");
}

TEST(build_token_request_device_emits_the_grant_and_code) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_device("GmRhmhcxhwAzkoEqiMEg", {},
                                                     body);
  EXPECT_TRUE(body == "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3A"
                      "device_code&device_code=GmRhmhcxhwAzkoEqiMEg");
}

TEST(device_authorization_response_reads_the_members) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "device_code": "GmRhmhcxhwAzkoEqiMEg",
    "user_code": "WDJB-MJHT",
    "verification_uri": "https://example.com/device",
    "verification_uri_complete": "https://example.com/device?user_code=WDJB-MJHT",
    "expires_in": 1800,
    "interval": 5
  })JSON")};
  const sourcemeta::core::OAuthDeviceAuthorizationResponse response{document};
  EXPECT_EQ(response.device_code().value(), "GmRhmhcxhwAzkoEqiMEg");
  EXPECT_EQ(response.user_code().value(), "WDJB-MJHT");
  EXPECT_EQ(response.verification_uri().value(), "https://example.com/device");
  EXPECT_EQ(response.verification_uri_complete().value(),
            "https://example.com/device?user_code=WDJB-MJHT");
  EXPECT_EQ(response.expires_in().value(), std::chrono::seconds{1800});
  EXPECT_EQ(response.interval(), std::chrono::seconds{5});
}

TEST(device_authorization_response_defaults_the_interval) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "device_code": "GmRh", "user_code": "WDJB-MJHT",
    "verification_uri": "https://example.com/device", "expires_in": 1800
  })JSON")};
  const sourcemeta::core::OAuthDeviceAuthorizationResponse response{document};
  EXPECT_EQ(response.interval(), std::chrono::seconds{5});
}

TEST(device_poller_defaults_a_zero_interval_to_five_seconds) {
  const sourcemeta::core::OAuthDevicePoller poller{
      std::chrono::seconds{0}, std::chrono::seconds{1800},
      std::chrono::steady_clock::time_point{}};
  EXPECT_EQ(poller.interval(), std::chrono::seconds{5});
}

TEST(device_poller_slow_down_grows_the_interval_and_continues) {
  sourcemeta::core::OAuthDevicePoller poller{
      std::chrono::seconds{5}, std::chrono::seconds{1800},
      std::chrono::steady_clock::time_point{}};
  EXPECT_EQ(poller.observe(sourcemeta::core::OAuthTokenError::SlowDown),
            sourcemeta::core::OAuthDevicePollDecision::Continue);
  EXPECT_EQ(poller.interval(), std::chrono::seconds{10});
  EXPECT_EQ(poller.observe(sourcemeta::core::OAuthTokenError::SlowDown),
            sourcemeta::core::OAuthDevicePollDecision::Continue);
  EXPECT_EQ(poller.interval(), std::chrono::seconds{15});
}

TEST(device_poller_maps_the_terminal_errors) {
  sourcemeta::core::OAuthDevicePoller poller{
      std::chrono::seconds{5}, std::chrono::seconds{1800},
      std::chrono::steady_clock::time_point{}};
  EXPECT_EQ(
      poller.observe(sourcemeta::core::OAuthTokenError::AuthorizationPending),
      sourcemeta::core::OAuthDevicePollDecision::Continue);
  EXPECT_EQ(poller.observe(sourcemeta::core::OAuthTokenError::AccessDenied),
            sourcemeta::core::OAuthDevicePollDecision::Denied);
  EXPECT_EQ(poller.observe(sourcemeta::core::OAuthTokenError::ExpiredToken),
            sourcemeta::core::OAuthDevicePollDecision::Expired);
  EXPECT_EQ(poller.observe(sourcemeta::core::OAuthTokenError::InvalidGrant),
            sourcemeta::core::OAuthDevicePollDecision::Error);
}

TEST(device_poller_reports_local_expiry) {
  const sourcemeta::core::OAuthDevicePoller poller{
      std::chrono::seconds{5}, std::chrono::seconds{1800},
      std::chrono::steady_clock::time_point{}};
  EXPECT_FALSE(poller.expired(std::chrono::steady_clock::time_point{} +
                              std::chrono::seconds{1799}));
  EXPECT_TRUE(poller.expired(std::chrono::steady_clock::time_point{} +
                             std::chrono::seconds{1800}));
}

TEST(make_device_authorization_response_emits_the_members) {
  const auto response{
      sourcemeta::core::oauth_make_device_authorization_response(
          "GmRh", "WDJB-MJHT", "https://example.com/device", "",
          std::chrono::seconds{1800}, std::chrono::seconds{5})};
  EXPECT_EQ(response.value().at("user_code").to_string(), "WDJB-MJHT");
  EXPECT_EQ(response.value().at("expires_in").to_integer(), 1800);
  EXPECT_FALSE(response.value().defines("interval"));
  EXPECT_FALSE(response.value().defines("verification_uri_complete"));
}

TEST(device_user_code_mints_from_the_alphabet) {
  const auto code{sourcemeta::core::oauth_device_user_code()};
  EXPECT_EQ(code.size(), 8);
  const std::string_view alphabet{"BCDFGHJKLMNPQRSTVWXZ"};
  EXPECT_TRUE(alphabet.find(code[0]) != std::string_view::npos);
  EXPECT_TRUE(alphabet.find(code[7]) != std::string_view::npos);
}

TEST(device_user_code_matches_normalizes_case_and_separators) {
  EXPECT_TRUE(sourcemeta::core::oauth_device_user_code_matches("wdjb-mjht",
                                                               "WDJBMJHT"));
  EXPECT_TRUE(sourcemeta::core::oauth_device_user_code_matches("WDJB MJHT",
                                                               "WDJBMJHT"));
  EXPECT_FALSE(sourcemeta::core::oauth_device_user_code_matches("wdjb-mjhx",
                                                                "WDJBMJHT"));
}

TEST(device_user_code_matches_rejects_empty_normalized_input) {
  // Empty or punctuation-only input normalizes to "" and must not match
  EXPECT_FALSE(sourcemeta::core::oauth_device_user_code_matches("", ""));
  EXPECT_FALSE(sourcemeta::core::oauth_device_user_code_matches("----", ""));
  EXPECT_FALSE(
      sourcemeta::core::oauth_device_user_code_matches("", "WDJBMJHT"));
  EXPECT_FALSE(
      sourcemeta::core::oauth_device_user_code_matches("----", "WDJBMJHT"));
}

TEST(device_user_code_matches_folds_the_stored_side_and_length) {
  EXPECT_TRUE(sourcemeta::core::oauth_device_user_code_matches("WDJB-MJHT",
                                                               "wdjbmjht"));
  EXPECT_FALSE(
      sourcemeta::core::oauth_device_user_code_matches("WDJBMJH", "WDJBMJHT"));
}

TEST(device_poller_defaults_a_negative_interval_to_five_seconds) {
  const sourcemeta::core::OAuthDevicePoller poller{
      std::chrono::seconds{-3}, std::chrono::seconds{1800},
      std::chrono::steady_clock::time_point{}};
  EXPECT_EQ(poller.interval(), std::chrono::seconds{5});
}

TEST(device_user_code_every_character_is_in_the_alphabet) {
  const auto code{sourcemeta::core::oauth_device_user_code()};
  const std::string_view alphabet{"BCDFGHJKLMNPQRSTVWXZ"};
  EXPECT_TRUE(alphabet.find(code[0]) != std::string_view::npos);
  EXPECT_TRUE(alphabet.find(code[1]) != std::string_view::npos);
  EXPECT_TRUE(alphabet.find(code[2]) != std::string_view::npos);
  EXPECT_TRUE(alphabet.find(code[3]) != std::string_view::npos);
  EXPECT_TRUE(alphabet.find(code[4]) != std::string_view::npos);
  EXPECT_TRUE(alphabet.find(code[5]) != std::string_view::npos);
  EXPECT_TRUE(alphabet.find(code[6]) != std::string_view::npos);
  EXPECT_TRUE(alphabet.find(code[7]) != std::string_view::npos);
}

TEST(make_device_authorization_response_emits_a_non_default_interval) {
  const auto response{
      sourcemeta::core::oauth_make_device_authorization_response(
          "GmRh", "WDJB-MJHT", "https://example.com/device",
          "https://example.com/device?user_code=WDJB-MJHT",
          std::chrono::seconds{1800}, std::chrono::seconds{7})};
  EXPECT_TRUE(response.value().defines("interval"));
  EXPECT_EQ(response.value().at("interval").to_integer(), 7);
  EXPECT_TRUE(response.value().defines("verification_uri_complete"));
}

TEST(make_device_authorization_response_requires_the_mandatory_members) {
  EXPECT_FALSE(sourcemeta::core::oauth_make_device_authorization_response(
                   "", "WDJB-MJHT", "https://example.com/device", "",
                   std::chrono::seconds{1800}, std::chrono::seconds{5})
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_make_device_authorization_response(
                   "GmRh", "", "https://example.com/device", "",
                   std::chrono::seconds{1800}, std::chrono::seconds{5})
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_make_device_authorization_response(
                   "GmRh", "WDJB-MJHT", "", "", std::chrono::seconds{1800},
                   std::chrono::seconds{5})
                   .has_value());
}

TEST(make_device_authorization_response_rejects_a_non_positive_lifetime) {
  EXPECT_FALSE(sourcemeta::core::oauth_make_device_authorization_response(
                   "GmRh", "WDJB-MJHT", "https://example.com/device", "",
                   std::chrono::seconds{0}, std::chrono::seconds{5})
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_make_device_authorization_response(
                   "GmRh", "WDJB-MJHT", "https://example.com/device", "",
                   std::chrono::seconds{-1}, std::chrono::seconds{5})
                   .has_value());
}

TEST(device_authorization_response_rejects_a_negative_lifetime) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "device_code": "GmRh", "user_code": "WDJB-MJHT",
    "verification_uri": "https://example.com/device", "expires_in": -1
  })JSON")};
  const sourcemeta::core::OAuthDeviceAuthorizationResponse response{document};
  EXPECT_FALSE(response.expires_in().has_value());
}

TEST(device_authorization_response_rejects_a_zero_lifetime) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "device_code": "GmRh", "user_code": "WDJB-MJHT",
    "verification_uri": "https://example.com/device", "expires_in": 0
  })JSON")};
  const sourcemeta::core::OAuthDeviceAuthorizationResponse response{document};
  EXPECT_FALSE(response.expires_in().has_value());
}

TEST(device_authorization_response_rejects_an_overflowing_lifetime) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "device_code": "GmRh", "user_code": "WDJB-MJHT",
    "verification_uri": "https://example.com/device",
    "expires_in": 9223372036854775807
  })JSON")};
  const sourcemeta::core::OAuthDeviceAuthorizationResponse response{document};
  EXPECT_TRUE(response.expires_in().value() ==
              std::chrono::seconds{9223372036854775807});
}

TEST(device_poller_does_not_expire_before_the_start) {
  const sourcemeta::core::OAuthDevicePoller poller{
      std::chrono::seconds{5}, std::chrono::seconds{0},
      std::chrono::steady_clock::time_point{} + std::chrono::seconds{100}};
  EXPECT_FALSE(poller.expired(std::chrono::steady_clock::time_point{} +
                              std::chrono::seconds{50}));
  EXPECT_TRUE(poller.expired(std::chrono::steady_clock::time_point{} +
                             std::chrono::seconds{100}));
}

TEST(device_authorization_response_defaults_a_negative_interval) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "device_code": "GmRh", "user_code": "WDJB-MJHT",
    "verification_uri": "https://example.com/device", "expires_in": 1800,
    "interval": -3
  })JSON")};
  const sourcemeta::core::OAuthDeviceAuthorizationResponse response{document};
  EXPECT_EQ(response.interval(), std::chrono::seconds{5});
}
