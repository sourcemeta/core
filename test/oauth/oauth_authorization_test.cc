#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>  // std::array
#include <string> // std::string

TEST(build_url_minimal_emits_response_type_and_client_id) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "s6BhdRkqt3";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url(
      "https://server.example/authorize", request, url);
  EXPECT_EQ(url, "https://server.example/authorize?response_type=code"
                 "&client_id=s6BhdRkqt3");
}

TEST(build_url_full_code_flow_with_pkce) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid profile";
  request.state = "xyz";
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  request.code_challenge_method = "S256";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url(
      "https://server.example/authorize", request, url);
  EXPECT_EQ(
      url,
      "https://server.example/authorize?response_type=code"
      "&client_id=s6BhdRkqt3&redirect_uri=https%3A%2F%2Fclient.example%2Fcb"
      "&scope=openid%20profile&state=xyz"
      "&code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM"
      "&code_challenge_method=S256");
}

TEST(build_url_escapes_the_redirect_uri) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.redirect_uri = "https://client.example/cb?x=1";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth?response_type=code&client_id=abc"
                 "&redirect_uri=https%3A%2F%2Fclient.example%2Fcb%3Fx%3D1");
}

TEST(build_url_escapes_spaces_in_scope) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.scope = "read write";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth?response_type=code&client_id=abc"
                 "&scope=read%20write");
}

TEST(build_url_omits_empty_fields) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.redirect_uri = "";
  request.scope = "";
  request.state = "";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url,
            "https://server.example/auth?response_type=code&client_id=abc");
}

TEST(build_url_omits_the_method_without_a_challenge) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.code_challenge_method = "S256";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url,
            "https://server.example/auth?response_type=code&client_id=abc");
}

TEST(build_url_honors_an_existing_query) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url(
      "https://server.example/auth?foo=bar", request, url);
  EXPECT_EQ(url, "https://server.example/auth?foo=bar&response_type=code"
                 "&client_id=abc");
}

TEST(build_url_emits_repeatable_resources) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  const std::array<sourcemeta::core::OAuthParameter, 2> resources{
      {{"resource", "https://api.a"}, {"resource", "https://api.b"}}};
  request.resources = resources;
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth?response_type=code&client_id=abc"
                 "&resource=https%3A%2F%2Fapi.a&resource=https%3A%2F%2Fapi.b");
}

TEST(build_url_emits_extra_parameters) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  const std::array<sourcemeta::core::OAuthParameter, 1> extra{
      {{"nonce", "n-0S6_WzA2Mj"}}};
  request.extra = extra;
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth?response_type=code&client_id=abc"
                 "&nonce=n-0S6_WzA2Mj");
}

TEST(build_url_emits_a_request_uri_reference) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.request_uri = "urn:ietf:params:oauth:request_uri:6esc";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url,
            "https://server.example/auth?response_type=code&client_id=abc"
            "&request_uri=urn%3Aietf%3Aparams%3Aoauth%3Arequest_uri%3A6esc");
}

TEST(build_url_emits_a_dpop_jkt) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.dpop_jkt = "NzbLsXh8uDCcd-6MNwXF4W_7noWXFZAfHkxZsRGC9Xs";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth?response_type=code&client_id=abc"
                 "&dpop_jkt=NzbLsXh8uDCcd-6MNwXF4W_7noWXFZAfHkxZsRGC9Xs");
}

TEST(build_url_appends_to_an_existing_sink) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  std::string url{"redirect: "};
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "redirect: https://server.example/auth?response_type=code"
                 "&client_id=abc");
}

TEST(parse_authorization_response_reads_code_and_state) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=SplxlOBeZQQYbYS6WxSbIA&state=xyz", storage, response));
  EXPECT_EQ(response.code, "SplxlOBeZQQYbYS6WxSbIA");
  EXPECT_EQ(response.state, "xyz");
  EXPECT_TRUE(response.iss.empty());
  EXPECT_TRUE(response.error.empty());
}

TEST(parse_authorization_response_reads_iss) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=abc&state=xyz&iss=https%3A%2F%2Fserver.example", storage,
      response));
  EXPECT_EQ(response.iss, "https://server.example");
}

TEST(parse_authorization_response_reads_an_error) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "error=access_denied&state=xyz", storage, response));
  EXPECT_EQ(response.error, "access_denied");
  EXPECT_TRUE(response.code.empty());
}

TEST(parse_authorization_response_decodes_plus_as_space) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=a+b&state=x", storage, response));
  EXPECT_EQ(response.code, "a b");
}

TEST(parse_authorization_response_borrows_an_unencoded_value) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  const std::string query{"code=plain&state=xyz"};
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      query, storage, response));
  // A value with no escape borrows from the input rather than the arena
  EXPECT_TRUE(response.code.data() >= query.data());
  EXPECT_TRUE(response.code.data() < query.data() + query.size());
}

TEST(parse_authorization_response_rejects_a_duplicate_parameter) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_response(
      "code=a&code=b", storage, response));
}

TEST(parse_authorization_response_ignores_an_unknown_parameter) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=abc&extra=ignored&state=xyz", storage, response));
  EXPECT_EQ(response.code, "abc");
  EXPECT_EQ(response.state, "xyz");
}

TEST(parse_authorization_response_rejects_a_malformed_escape) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_response(
      "code=a%2", storage, response));
}

TEST(parse_authorization_response_accepts_an_empty_query) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response("", storage,
                                                                   response));
  EXPECT_TRUE(response.code.empty());
}

TEST(build_url_escapes_an_extra_parameter_name) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  const std::array<sourcemeta::core::OAuthParameter, 1> extra{{{"a b", "c"}}};
  request.extra = extra;
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth?response_type=code&client_id=abc"
                 "&a%20b=c");
}

TEST(build_url_endpoint_ending_in_question_mark) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url(
      "https://server.example/auth?", request, url);
  EXPECT_EQ(url,
            "https://server.example/auth?response_type=code&client_id=abc");
}

TEST(build_url_endpoint_ending_in_ampersand) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url(
      "https://server.example/auth?foo=bar&", request, url);
  EXPECT_EQ(url, "https://server.example/auth?foo=bar&response_type=code"
                 "&client_id=abc");
}

TEST(build_url_challenge_without_a_method) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth?response_type=code&client_id=abc"
                 "&code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM");
}

TEST(build_url_with_no_client_id_still_emits_response_type) {
  const sourcemeta::core::OAuthAuthorizationRequest request;
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth?response_type=code");
}

TEST(parse_authorization_response_empty_value) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=&state=x", storage, response));
  EXPECT_TRUE(response.code.empty());
  EXPECT_EQ(response.state, "x");
}

TEST(parse_authorization_response_rejects_a_duplicate_iss) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_response(
      "iss=a&iss=b", storage, response));
}

TEST(parse_authorization_response_rejects_a_duplicate_error) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_response(
      "error=a&error=b", storage, response));
}

TEST(parse_authorization_response_rejects_a_duplicate_state) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_response(
      "state=a&state=b", storage, response));
}

TEST(parse_authorization_response_keeps_encoded_delimiters) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=a%26b%3Dc&state=x", storage, response));
  EXPECT_EQ(response.code, "a&b=c");
  EXPECT_EQ(response.state, "x");
}

TEST(parse_authorization_response_tolerates_empty_pairs) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "&code=x&&state=y", storage, response));
  EXPECT_EQ(response.code, "x");
  EXPECT_EQ(response.state, "y");
}

TEST(parse_authorization_response_captures_both_code_and_error) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=x&error=access_denied&state=y", storage, response));
  EXPECT_EQ(response.code, "x");
  EXPECT_EQ(response.error, "access_denied");
}

TEST(parse_authorization_response_reuses_the_storage_arena) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse first;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=a%20b&state=x", storage, first));
  EXPECT_EQ(first.code, "a b");
  sourcemeta::core::OAuthAuthorizationResponse second;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code=c%20d&state=y", storage, second));
  EXPECT_EQ(second.code, "c d");
  EXPECT_EQ(second.state, "y");
}

TEST(parse_authorization_response_bare_key_without_equals) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "code&state=x", storage, response));
  EXPECT_TRUE(response.code.empty());
  EXPECT_EQ(response.state, "x");
}

TEST(parse_authorization_response_reads_error_details) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "error=access_denied&error_description=User%20denied"
      "&error_uri=https%3A%2F%2Fx%2Fe&state=xyz",
      storage, response));
  EXPECT_EQ(response.error, "access_denied");
  EXPECT_EQ(response.error_description, "User denied");
  EXPECT_EQ(response.error_uri, "https://x/e");
}

TEST(parse_authorization_response_rejects_a_duplicate_error_description) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_response(
      "error_description=a&error_description=b", storage, response));
}
