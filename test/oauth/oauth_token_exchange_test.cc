#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string_view> // std::string_view

TEST(token_exchange_valid_accepts_a_minimal_request) {
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token = "eyJ";
  request.subject_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN;
  EXPECT_TRUE(sourcemeta::core::oauth_token_exchange_valid(request));
}

TEST(token_exchange_valid_rejects_a_missing_subject_token) {
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN;
  EXPECT_FALSE(sourcemeta::core::oauth_token_exchange_valid(request));
}

TEST(token_exchange_valid_rejects_an_actor_token_without_a_type) {
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token = "eyJ";
  request.subject_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN;
  request.actor_token = "act";
  EXPECT_FALSE(sourcemeta::core::oauth_token_exchange_valid(request));
}

TEST(token_exchange_valid_rejects_an_actor_type_without_a_token) {
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token = "eyJ";
  request.subject_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN;
  request.actor_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_JWT;
  EXPECT_FALSE(sourcemeta::core::oauth_token_exchange_valid(request));
}

TEST(build_token_request_exchange_emits_a_minimal_body) {
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token = "accVkjcJyb4BWCxGsndESCJQbdFMogUC5PbRDqceLTC";
  request.subject_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN;
  sourcemeta::core::SecureString body;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_token_request_exchange(request, body));
  EXPECT_TRUE(
      body ==
      "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Atoken-exchange"
      "&subject_token=accVkjcJyb4BWCxGsndESCJQbdFMogUC5PbRDqceLTC"
      "&subject_token_type=urn%3Aietf%3Aparams%3Aoauth%3Atoken-type%3A"
      "access_token");
}

TEST(build_token_request_exchange_emits_the_actor_pair_and_extras) {
  const std::array<sourcemeta::core::OAuthParameter, 1> audiences{
      {{"audience", "https://api.example"}}};
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token = "subj";
  request.subject_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN;
  request.actor_token = "act";
  request.actor_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_JWT;
  request.requested_token_type =
      sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN;
  request.scope = "read";
  request.audiences = audiences;
  sourcemeta::core::SecureString body;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_token_request_exchange(request, body));
  const std::string_view view{body};
  EXPECT_TRUE(view.find("&actor_token=act&actor_token_type=") !=
              std::string_view::npos);
  EXPECT_TRUE(view.find("&scope=read") != std::string_view::npos);
  EXPECT_TRUE(view.find("&audience=https%3A%2F%2Fapi.example") !=
              std::string_view::npos);
}

TEST(build_token_request_exchange_rejects_an_invalid_request) {
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token = "subj";
  sourcemeta::core::SecureString body;
  EXPECT_FALSE(
      sourcemeta::core::oauth_build_token_request_exchange(request, body));
  EXPECT_TRUE(body.empty());
}

TEST(issued_token_type_reads_the_member) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "access_token": "eyJ",
    "token_type": "Bearer",
    "issued_token_type": "urn:ietf:params:oauth:token-type:access_token"
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oauth_issued_token_type(document).has_value());
  EXPECT_EQ(sourcemeta::core::oauth_issued_token_type(document).value(),
            sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN);
}

TEST(issued_token_type_is_absent_when_missing) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "access_token": "eyJ", "token_type": "Bearer"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_issued_token_type(document).has_value());
}

TEST(token_exchange_valid_rejects_a_subject_token_without_a_type) {
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token = "eyJ";
  EXPECT_FALSE(sourcemeta::core::oauth_token_exchange_valid(request));
}

TEST(build_token_request_exchange_emits_resources_and_requested_type) {
  const std::array<sourcemeta::core::OAuthParameter, 1> resources{
      {{"resource", "https://api.example/foo"}}};
  sourcemeta::core::OAuthTokenExchangeRequest request;
  request.subject_token = "subj";
  request.subject_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN;
  request.requested_token_type = sourcemeta::core::OAUTH_TOKEN_TYPE_JWT;
  request.resources = resources;
  sourcemeta::core::SecureString body;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_token_request_exchange(request, body));
  const std::string_view view{body};
  EXPECT_TRUE(view.find("&requested_token_type=") != std::string_view::npos);
  EXPECT_TRUE(view.find("&resource=https%3A%2F%2Fapi.example%2Ffoo") !=
              std::string_view::npos);
  EXPECT_TRUE(view.find("audience=") == std::string_view::npos);
}
