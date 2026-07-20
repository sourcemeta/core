#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(build_revocation_request_emits_the_token) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_revocation_request("45ghiukldjahdnhzdauz", "",
                                                   body);
  EXPECT_TRUE(body == "token=45ghiukldjahdnhzdauz");
}

TEST(build_revocation_request_emits_a_hint) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_revocation_request(
      "45ghiukldjahdnhzdauz",
      sourcemeta::core::OAUTH_TOKEN_TYPE_HINT_REFRESH_TOKEN, body);
  EXPECT_TRUE(body ==
              "token=45ghiukldjahdnhzdauz&token_type_hint=refresh_token");
}

TEST(parse_revocation_request_reads_the_token_and_hint) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthTokenLookupRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_revocation_request(
      "token=45ghiukldjahdnhzdauz&token_type_hint=access_token", storage,
      request, [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.token, "45ghiukldjahdnhzdauz");
  EXPECT_EQ(request.token_type_hint, "access_token");
}

TEST(parse_revocation_request_rejects_a_missing_token) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthTokenLookupRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_revocation_request(
      "token_type_hint=access_token", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(parse_revocation_request_surfaces_client_auth_through_the_callback) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthTokenLookupRequest request;
  std::string collected;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_revocation_request(
      "token=abc&client_id=s6BhdRkqt3", storage, request,
      [&collected](std::string_view name, std::string_view value) {
        collected.append(name);
        collected.push_back('=');
        collected.append(value);
      }));
  EXPECT_EQ(collected, "client_id=s6BhdRkqt3");
}

TEST(revocation_outcome_maps_the_status) {
  EXPECT_EQ(sourcemeta::core::oauth_revocation_outcome(
                sourcemeta::core::HTTP_STATUS_OK),
            sourcemeta::core::OAuthRevocationOutcome::Success);
  EXPECT_EQ(sourcemeta::core::oauth_revocation_outcome(
                sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE),
            sourcemeta::core::OAuthRevocationOutcome::Retry);
  EXPECT_EQ(sourcemeta::core::oauth_revocation_outcome(
                sourcemeta::core::HTTP_STATUS_BAD_REQUEST),
            sourcemeta::core::OAuthRevocationOutcome::Error);
}

TEST(revocation_outcome_treats_other_statuses_as_error) {
  EXPECT_EQ(sourcemeta::core::oauth_revocation_outcome(
                sourcemeta::core::HTTP_STATUS_NO_CONTENT),
            sourcemeta::core::OAuthRevocationOutcome::Error);
  EXPECT_EQ(sourcemeta::core::oauth_revocation_outcome(
                sourcemeta::core::HTTP_STATUS_UNAUTHORIZED),
            sourcemeta::core::OAuthRevocationOutcome::Error);
  EXPECT_EQ(sourcemeta::core::oauth_revocation_outcome(
                sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR),
            sourcemeta::core::OAuthRevocationOutcome::Error);
}

TEST(parse_revocation_request_rejects_an_empty_token_value) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthTokenLookupRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_revocation_request(
      "token=", storage, request, [](std::string_view, std::string_view) {}));
}

TEST(parse_revocation_request_rejects_a_duplicate_token) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthTokenLookupRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_revocation_request(
      "token=abc&token=def", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(parse_revocation_request_decodes_a_percent_encoded_name) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthTokenLookupRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_revocation_request(
      "%74oken=abc", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.token, "abc");
}

TEST(parse_revocation_request_rejects_a_malformed_escape) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthTokenLookupRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_revocation_request(
      "token=%zz", storage, request,
      [](std::string_view, std::string_view) {}));
}
