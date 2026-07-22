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

TEST(build_url_defaults_response_type_to_code_when_unset) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url,
            "https://server.example/auth?response_type=code&client_id=abc");
}

TEST(build_url_honors_an_explicit_code_response_type) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.response_type = "code";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url,
            "https://server.example/auth?response_type=code&client_id=abc");
}

TEST(build_url_honors_an_id_token_response_type) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.response_type = "id_token";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url,
            "https://server.example/auth?response_type=id_token&client_id=abc");
}

TEST(build_url_honors_a_token_response_type) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.response_type = "token";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url,
            "https://server.example/auth?response_type=token&client_id=abc");
}

TEST(build_url_honors_a_hybrid_code_id_token_response_type) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.response_type = "code id_token";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth"
                 "?response_type=code%20id_token&client_id=abc");
}

TEST(build_url_honors_an_implicit_id_token_token_response_type) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.response_type = "id_token token";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth"
                 "?response_type=id_token%20token&client_id=abc");
}

TEST(build_url_honors_a_hybrid_code_id_token_token_response_type) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "abc";
  request.response_type = "code id_token token";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth"
                 "?response_type=code%20id_token%20token&client_id=abc");
}

TEST(build_url_emits_the_response_type_before_other_parameters) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.response_type = "code id_token";
  request.client_id = "abc";
  request.state = "xyz";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url("https://server.example/auth",
                                                  request, url);
  EXPECT_EQ(url, "https://server.example/auth"
                 "?response_type=code%20id_token&client_id=abc&state=xyz");
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

TEST(parse_authorization_response_decodes_a_percent_encoded_name) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationResponse response;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      "%63ode=SplxlOBeZQQYbYS6WxSbIA&state=xyz", storage, response));
  EXPECT_EQ(response.code, "SplxlOBeZQQYbYS6WxSbIA");
  EXPECT_EQ(response.state, "xyz");
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

TEST(parse_request_reads_the_recognized_parameters) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "response_type=code&client_id=s6BhdRkqt3&redirect_uri=https%3A%2F%2Fc."
      "example"
      "%2Fcb&scope=openid&state=xyz",
      storage, request, [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.response_type, "code");
  EXPECT_EQ(request.client_id, "s6BhdRkqt3");
  EXPECT_EQ(request.redirect_uri, "https://c.example/cb");
  EXPECT_EQ(request.scope, "openid");
  EXPECT_EQ(request.state, "xyz");
}

TEST(parse_request_rejects_a_duplicate_parameter) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_request(
      "client_id=a&client_id=b", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(parse_request_defaults_the_challenge_method_to_plain) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "code_challenge=abc", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.code_challenge, "abc");
  EXPECT_EQ(request.code_challenge_method, "plain");
}

TEST(parse_request_keeps_an_explicit_challenge_method) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "code_challenge=abc&code_challenge_method=S256", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.code_challenge_method, "S256");
}

TEST(parse_request_surfaces_resources_and_extras_through_the_callback) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  std::string collected;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "resource=https%3A%2F%2Fa.example&resource=https%3A%2F%2Fb.example&nonce="
      "n1",
      storage, request,
      [&collected](std::string_view name, std::string_view value) {
        collected.append(name);
        collected.push_back('=');
        collected.append(value);
        collected.push_back(';');
      }));
  EXPECT_EQ(collected,
            "resource=https://a.example;resource=https://b.example;nonce=n1;");
}

TEST(parse_request_accepts_a_challenge_method_without_a_challenge) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "code_challenge_method=S256", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.code_challenge_method, "S256");
  EXPECT_TRUE(request.code_challenge.empty());
}

TEST(redirect_uri_matches_an_exact_uri) {
  EXPECT_TRUE(sourcemeta::core::oauth_redirect_uri_matches(
      "https://client.example/cb", "https://client.example/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_rejects_a_different_path) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "https://client.example/cb", "https://client.example/other",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_allows_a_loopback_port_to_vary) {
  EXPECT_TRUE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://127.0.0.1:49152/cb", "http://127.0.0.1:51004/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_allows_a_loopback_ipv6_port_to_vary) {
  EXPECT_TRUE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://[::1]:49152/cb", "http://[::1]:51004/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_rejects_a_loopback_with_a_different_path) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://127.0.0.1:49152/cb", "http://127.0.0.1:51004/other",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_rejects_localhost_port_variation_under_strict) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://localhost:49152/cb", "http://localhost:51004/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_allows_localhost_port_variation_under_compatible) {
  EXPECT_TRUE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://localhost:49152/cb", "http://localhost:51004/cb",
      sourcemeta::core::OAuthProfile::Compatible));
}

TEST(redirect_uri_rejects_a_non_loopback_host_port_variation) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://example.com:49152/cb", "http://example.com:51004/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(is_private_use_scheme_accepts_a_reverse_domain) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_private_use_scheme("com.example.app"));
}

TEST(is_private_use_scheme_rejects_a_scheme_without_a_period) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_private_use_scheme("myapp"));
}

TEST(is_private_use_scheme_rejects_a_leading_non_alpha) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_private_use_scheme("1com.example"));
}

TEST(is_private_use_scheme_rejects_an_invalid_character) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_private_use_scheme("com.exa mple"));
}

TEST(build_authorization_redirect_emits_code_and_state) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "SplxlOBeZQQYbYS6WxSbIA";
  response.state = "xyz";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb", response, url));
  EXPECT_EQ(url, "https://client.example/cb?code=SplxlOBeZQQYbYS6WxSbIA"
                 "&state=xyz");
}

TEST(build_authorization_redirect_emits_a_valid_iss) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  response.iss = "https://server.example";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb", response, url));
  EXPECT_EQ(url, "https://client.example/cb?code=abc"
                 "&iss=https%3A%2F%2Fserver.example");
}

TEST(build_authorization_redirect_rejects_an_invalid_iss) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  response.iss = "https://server.example?x=1";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb", response, url));
  EXPECT_TRUE(url.empty());
}

TEST(build_authorization_redirect_rejects_a_missing_code) {
  const sourcemeta::core::OAuthAuthorizationResponse response;
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb", response, url));
}

TEST(build_authorization_redirect_honors_an_existing_query) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb?ui=1", response, url));
  EXPECT_EQ(url, "https://client.example/cb?ui=1&code=abc");
}

TEST(build_authorization_error_redirect_emits_error_and_state) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.state = "xyz";
  response.error = "access_denied";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_error_redirect(
      "https://client.example/cb", response, url));
  EXPECT_EQ(url, "https://client.example/cb?error=access_denied&state=xyz");
}

TEST(build_authorization_error_redirect_rejects_a_missing_error) {
  const sourcemeta::core::OAuthAuthorizationResponse response;
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_error_redirect(
      "https://client.example/cb", response, url));
}

TEST(redirect_uri_rejects_userinfo_smuggling_a_loopback_host) {
  // A colon inside the userinfo must not let a non-loopback host masquerade as
  // the loopback literal (RFC 3986 Section 3.2.1). The user agent would
  // navigate to evil.example, so this must not match a registered loopback.
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://127.0.0.1:49152/cb", "http://127.0.0.1:2@evil.example/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_rejects_userinfo_on_a_presented_loopback) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://127.0.0.1:5000/cb", "http://user@127.0.0.1:5000/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_rejects_a_loopback_without_a_path_differing_query) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://127.0.0.1:5000?foo=1", "http://127.0.0.1:6000?bar=2",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_rejects_a_loopback_with_a_differing_query) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://127.0.0.1:5000/cb?x=1", "http://127.0.0.1:6000/cb?x=2",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_rejects_an_https_loopback_port_variation) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "https://127.0.0.1:5000/cb", "https://127.0.0.1:6000/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_rejects_a_non_http_presented_scheme) {
  EXPECT_FALSE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://127.0.0.1:5000/cb", "com.example.app://127.0.0.1:5000/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(redirect_uri_matches_ipv6_loopback_with_and_without_a_port) {
  EXPECT_TRUE(sourcemeta::core::oauth_redirect_uri_matches(
      "http://[::1]/cb", "http://[::1]:51004/cb",
      sourcemeta::core::OAuthProfile::Strict));
}

TEST(parse_request_treats_an_empty_first_value_as_omitted) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "state=&state=xyz", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.state, "xyz");
}

TEST(parse_request_empty_challenge_does_not_default_the_method) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "code_challenge=", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_TRUE(request.code_challenge.empty());
  EXPECT_TRUE(request.code_challenge_method.empty());
}

TEST(parse_request_accepts_an_empty_query) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "", storage, request, [](std::string_view, std::string_view) {}));
  EXPECT_TRUE(request.client_id.empty());
  EXPECT_TRUE(request.response_type.empty());
}

TEST(parse_request_rejects_a_duplicate_scope) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_request(
      "scope=a&scope=b", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(build_authorization_error_redirect_emits_all_fields_in_order) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.error = "access_denied";
  response.error_description = "User denied";
  response.error_uri = "https://x/e";
  response.state = "xyz";
  response.iss = "https://server.example";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_error_redirect(
      "https://client.example/cb", response, url));
  EXPECT_EQ(url,
            "https://client.example/cb?error=access_denied"
            "&error_description=User%20denied&error_uri=https%3A%2F%2Fx%2Fe"
            "&state=xyz&iss=https%3A%2F%2Fserver.example");
}

TEST(build_authorization_error_redirect_rejects_an_invalid_iss) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.error = "access_denied";
  response.iss = "https://server.example?x=1";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_error_redirect(
      "https://client.example/cb", response, url));
  EXPECT_TRUE(url.empty());
}

TEST(is_private_use_scheme_rejects_an_empty_label) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_private_use_scheme("com..example"));
}

TEST(is_private_use_scheme_rejects_a_trailing_period) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_private_use_scheme("com.example."));
}

TEST(is_private_use_scheme_rejects_a_leading_period) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_private_use_scheme(".com.example"));
}

TEST(parse_request_decodes_a_percent_encoded_name) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  // client%5Fid decodes to client_id, so it is recognized rather than ignored
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      "client%5Fid=s6BhdRkqt3", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.client_id, "s6BhdRkqt3");
}

TEST(parse_request_rejects_a_percent_encoded_duplicate_name) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  // client%5Fid decodes to client_id, so a second occurrence is a duplicate
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_request(
      "client_id=a&client%5Fid=b", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(parse_request_rejects_a_malformed_escape_in_a_name) {
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_authorization_request(
      "client%5=a", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(build_authorization_redirect_rejects_a_fragment_in_the_redirect_uri) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb#section", response, url));
  EXPECT_TRUE(url.empty());
}

TEST(
    build_authorization_error_redirect_rejects_a_fragment_in_the_redirect_uri) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.error = "access_denied";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_error_redirect(
      "https://client.example/cb#section", response, url));
  EXPECT_TRUE(url.empty());
}

TEST(default_response_mode_for_code_is_query) {
  const auto mode{sourcemeta::core::oauth_default_response_mode("code")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Query);
}

TEST(default_response_mode_for_none_is_query) {
  const auto mode{sourcemeta::core::oauth_default_response_mode("none")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Query);
}

TEST(default_response_mode_for_token_is_fragment) {
  const auto mode{sourcemeta::core::oauth_default_response_mode("token")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Fragment);
}

TEST(default_response_mode_for_id_token_is_fragment) {
  const auto mode{sourcemeta::core::oauth_default_response_mode("id_token")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Fragment);
}

TEST(default_response_mode_for_code_id_token_is_fragment) {
  const auto mode{
      sourcemeta::core::oauth_default_response_mode("code id_token")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Fragment);
}

TEST(default_response_mode_for_code_token_is_fragment) {
  const auto mode{sourcemeta::core::oauth_default_response_mode("code token")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Fragment);
}

TEST(default_response_mode_for_id_token_token_is_fragment) {
  const auto mode{
      sourcemeta::core::oauth_default_response_mode("id_token token")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Fragment);
}

TEST(default_response_mode_for_code_id_token_token_is_fragment) {
  const auto mode{
      sourcemeta::core::oauth_default_response_mode("code id_token token")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Fragment);
}

TEST(default_response_mode_ignores_the_value_order) {
  const auto mode{
      sourcemeta::core::oauth_default_response_mode("id_token code")};
  EXPECT_TRUE(mode.has_value());
  EXPECT_TRUE(mode.value() == sourcemeta::core::OAuthResponseMode::Fragment);
}

TEST(default_response_mode_rejects_an_empty_response_type) {
  EXPECT_FALSE(sourcemeta::core::oauth_default_response_mode("").has_value());
}

TEST(default_response_mode_rejects_an_unknown_response_type) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_default_response_mode("foo").has_value());
}

TEST(default_response_mode_rejects_an_unknown_value_in_a_list) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_default_response_mode("code foo").has_value());
}

TEST(default_response_mode_rejects_consecutive_spaces) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_default_response_mode("code  token").has_value());
}

TEST(default_response_mode_rejects_none_in_a_combination) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_default_response_mode("code none").has_value());
}

TEST(default_response_mode_rejects_a_duplicated_value) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_default_response_mode("code code").has_value());
}

TEST(response_mode_query_is_allowed_for_code) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_response_mode_allowed(
      "code", sourcemeta::core::OAuthResponseMode::Query));
}

TEST(response_mode_fragment_is_allowed_for_code) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_response_mode_allowed(
      "code", sourcemeta::core::OAuthResponseMode::Fragment));
}

TEST(response_mode_query_is_not_allowed_for_id_token) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_response_mode_allowed(
      "id_token", sourcemeta::core::OAuthResponseMode::Query));
}

TEST(response_mode_query_is_not_allowed_for_code_token) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_response_mode_allowed(
      "code token", sourcemeta::core::OAuthResponseMode::Query));
}

TEST(response_mode_fragment_is_allowed_for_id_token) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_response_mode_allowed(
      "id_token", sourcemeta::core::OAuthResponseMode::Fragment));
}

TEST(response_mode_form_post_is_allowed_for_code) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_response_mode_allowed(
      "code", sourcemeta::core::OAuthResponseMode::FormPost));
}

TEST(response_mode_form_post_is_allowed_for_id_token) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_response_mode_allowed(
      "id_token", sourcemeta::core::OAuthResponseMode::FormPost));
}

TEST(response_mode_is_not_allowed_for_an_unknown_response_type) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_response_mode_allowed(
      "foo", sourcemeta::core::OAuthResponseMode::Fragment));
}

TEST(build_authorization_redirect_query_mode_matches_the_default_builder) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "SplxlOBeZQQYbYS6WxSbIA";
  response.state = "xyz";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb", response,
      sourcemeta::core::OAuthResponseMode::Query, url));
  EXPECT_EQ(url, "https://client.example/cb?code=SplxlOBeZQQYbYS6WxSbIA"
                 "&state=xyz");
}

TEST(build_authorization_redirect_fragment_mode_emits_code_and_state) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "SplxlOBeZQQYbYS6WxSbIA";
  response.state = "xyz";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb", response,
      sourcemeta::core::OAuthResponseMode::Fragment, url));
  EXPECT_EQ(url, "https://client.example/cb#code=SplxlOBeZQQYbYS6WxSbIA"
                 "&state=xyz");
}

TEST(build_authorization_redirect_fragment_mode_follows_an_existing_query) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb?ui=1", response,
      sourcemeta::core::OAuthResponseMode::Fragment, url));
  EXPECT_EQ(url, "https://client.example/cb?ui=1#code=abc");
}

TEST(build_authorization_redirect_fragment_mode_escapes_the_values) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  response.iss = "https://server.example";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb", response,
      sourcemeta::core::OAuthResponseMode::Fragment, url));
  EXPECT_EQ(url, "https://client.example/cb#code=abc"
                 "&iss=https%3A%2F%2Fserver.example");
}

TEST(
    build_authorization_redirect_fragment_mode_rejects_a_fragment_in_the_redirect_uri) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb#section", response,
      sourcemeta::core::OAuthResponseMode::Fragment, url));
  EXPECT_TRUE(url.empty());
}

TEST(build_authorization_redirect_rejects_the_form_post_mode) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_redirect(
      "https://client.example/cb", response,
      sourcemeta::core::OAuthResponseMode::FormPost, url));
  EXPECT_TRUE(url.empty());
}

TEST(build_authorization_error_redirect_fragment_mode_emits_error_and_state) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.state = "xyz";
  response.error = "access_denied";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_error_redirect(
      "https://client.example/cb", response,
      sourcemeta::core::OAuthResponseMode::Fragment, url));
  EXPECT_EQ(url, "https://client.example/cb#error=access_denied&state=xyz");
}

TEST(build_authorization_error_redirect_rejects_the_form_post_mode) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.error = "access_denied";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_error_redirect(
      "https://client.example/cb", response,
      sourcemeta::core::OAuthResponseMode::FormPost, url));
  EXPECT_TRUE(url.empty());
}

TEST(build_authorization_form_post_emits_code_and_state) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "SplxlOBeZQQYbYS6WxSbIA";
  response.state = "xyz";
  std::string page;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb", response, page));
  EXPECT_EQ(page, "<html><head><title>Submit This Form</title></head>"
                  "<body onload=\"javascript:document.forms[0].submit()\">"
                  "<form method=\"post\" action=\"https://client.example/cb\">"
                  "<input type=\"hidden\" name=\"code\" "
                  "value=\"SplxlOBeZQQYbYS6WxSbIA\"/>"
                  "<input type=\"hidden\" name=\"state\" value=\"xyz\"/>"
                  "</form></body></html>");
}

TEST(build_authorization_form_post_emits_a_valid_iss) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  response.iss = "https://server.example";
  std::string page;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb", response, page));
  EXPECT_EQ(page, "<html><head><title>Submit This Form</title></head>"
                  "<body onload=\"javascript:document.forms[0].submit()\">"
                  "<form method=\"post\" action=\"https://client.example/cb\">"
                  "<input type=\"hidden\" name=\"code\" value=\"abc\"/>"
                  "<input type=\"hidden\" name=\"iss\" "
                  "value=\"https://server.example\"/>"
                  "</form></body></html>");
}

TEST(build_authorization_form_post_escapes_the_values) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  response.state = "\"><script>alert(1)</script>";
  std::string page;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb", response, page));
  EXPECT_EQ(page, "<html><head><title>Submit This Form</title></head>"
                  "<body onload=\"javascript:document.forms[0].submit()\">"
                  "<form method=\"post\" action=\"https://client.example/cb\">"
                  "<input type=\"hidden\" name=\"code\" value=\"abc\"/>"
                  "<input type=\"hidden\" name=\"state\" "
                  "value=\"&quot;&gt;&lt;script&gt;alert(1)&lt;/script&gt;\"/>"
                  "</form></body></html>");
}

TEST(build_authorization_form_post_escapes_the_redirect_uri) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string page;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb?ui=1&theme=dark", response, page));
  EXPECT_EQ(page, "<html><head><title>Submit This Form</title></head>"
                  "<body onload=\"javascript:document.forms[0].submit()\">"
                  "<form method=\"post\" "
                  "action=\"https://client.example/cb?ui=1&amp;theme=dark\">"
                  "<input type=\"hidden\" name=\"code\" value=\"abc\"/>"
                  "</form></body></html>");
}

TEST(build_authorization_form_post_honors_a_custom_title) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string page;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb", response, page, "Redirecting"));
  EXPECT_EQ(page, "<html><head><title>Redirecting</title></head>"
                  "<body onload=\"javascript:document.forms[0].submit()\">"
                  "<form method=\"post\" action=\"https://client.example/cb\">"
                  "<input type=\"hidden\" name=\"code\" value=\"abc\"/>"
                  "</form></body></html>");
}

TEST(build_authorization_form_post_escapes_the_title) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string page;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb", response, page, "Fast & Safe"));
  EXPECT_EQ(page, "<html><head><title>Fast &amp; Safe</title></head>"
                  "<body onload=\"javascript:document.forms[0].submit()\">"
                  "<form method=\"post\" action=\"https://client.example/cb\">"
                  "<input type=\"hidden\" name=\"code\" value=\"abc\"/>"
                  "</form></body></html>");
}

TEST(build_authorization_form_post_rejects_a_missing_code) {
  const sourcemeta::core::OAuthAuthorizationResponse response;
  std::string page;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb", response, page));
  EXPECT_TRUE(page.empty());
}

TEST(build_authorization_form_post_rejects_an_invalid_iss) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  response.iss = "https://server.example?x=1";
  std::string page;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb", response, page));
  EXPECT_TRUE(page.empty());
}

TEST(build_authorization_form_post_rejects_a_fragment_in_the_redirect_uri) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.code = "abc";
  std::string page;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_form_post(
      "https://client.example/cb#section", response, page));
  EXPECT_TRUE(page.empty());
}

TEST(build_authorization_error_form_post_emits_every_error_field) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.error = "access_denied";
  response.error_description = "The user declined";
  response.error_uri = "https://server.example/errors/denied";
  response.state = "xyz";
  std::string page;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_error_form_post(
      "https://client.example/cb", response, page));
  EXPECT_EQ(page,
            "<html><head><title>Submit This Form</title></head>"
            "<body onload=\"javascript:document.forms[0].submit()\">"
            "<form method=\"post\" action=\"https://client.example/cb\">"
            "<input type=\"hidden\" name=\"error\" value=\"access_denied\"/>"
            "<input type=\"hidden\" name=\"error_description\" "
            "value=\"The user declined\"/>"
            "<input type=\"hidden\" name=\"error_uri\" "
            "value=\"https://server.example/errors/denied\"/>"
            "<input type=\"hidden\" name=\"state\" value=\"xyz\"/>"
            "</form></body></html>");
}

TEST(build_authorization_error_form_post_rejects_a_missing_error) {
  const sourcemeta::core::OAuthAuthorizationResponse response;
  std::string page;
  EXPECT_FALSE(sourcemeta::core::oauth_build_authorization_error_form_post(
      "https://client.example/cb", response, page));
  EXPECT_TRUE(page.empty());
}

TEST(build_authorization_error_form_post_honors_a_custom_title) {
  sourcemeta::core::OAuthAuthorizationResponse response;
  response.error = "access_denied";
  std::string page;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_error_form_post(
      "https://client.example/cb", response, page, "Redirecting"));
  EXPECT_EQ(page,
            "<html><head><title>Redirecting</title></head>"
            "<body onload=\"javascript:document.forms[0].submit()\">"
            "<form method=\"post\" action=\"https://client.example/cb\">"
            "<input type=\"hidden\" name=\"error\" value=\"access_denied\"/>"
            "</form></body></html>");
}
