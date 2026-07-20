#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

TEST(build_par_request_emits_a_minimal_body) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.redirect_uri = "https://client.example/cb";
  request.scope = "read";
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  EXPECT_EQ(body, "response_type=code"
                  "&redirect_uri=https%3A%2F%2Fclient.example%2Fcb"
                  "&scope=read");
}

TEST(build_par_request_omits_client_id_and_request_uri) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.request_uri = "urn:ietf:params:oauth:request_uri:6esc";
  request.scope = "read";
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  const std::string_view view{body};
  EXPECT_TRUE(view.find("client_id") == std::string_view::npos);
  EXPECT_TRUE(view.find("request_uri") == std::string_view::npos);
  EXPECT_TRUE(view.find("scope=read") != std::string_view::npos);
}

TEST(build_par_request_emits_every_authorization_parameter) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.redirect_uri = "https://client.example/cb";
  request.scope = "read";
  request.state = "af0ifjsldkj";
  request.code_challenge = "K2-ltc83acc4h0c9w6ESC";
  request.code_challenge_method = "S256";
  request.dpop_jkt = "NzbLsXh8uDCcd-6MNwXF4W";
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  EXPECT_EQ(body, "response_type=code"
                  "&redirect_uri=https%3A%2F%2Fclient.example%2Fcb"
                  "&scope=read"
                  "&state=af0ifjsldkj"
                  "&code_challenge=K2-ltc83acc4h0c9w6ESC"
                  "&code_challenge_method=S256"
                  "&dpop_jkt=NzbLsXh8uDCcd-6MNwXF4W");
}

TEST(build_par_request_emits_a_challenge_without_a_method) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.code_challenge = "K2-ltc83acc4h0c9w6ESC";
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  EXPECT_EQ(body, "response_type=code&code_challenge=K2-ltc83acc4h0c9w6ESC");
  EXPECT_TRUE(std::string_view{body}.find("code_challenge_method") ==
              std::string_view::npos);
}

TEST(build_par_request_emits_extension_parameters) {
  const std::array<sourcemeta::core::OAuthParameter, 1> extra{
      {{"nonce", "n-0S6_WzA2Mj"}}};
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.scope = "read";
  request.extra = extra;
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  EXPECT_EQ(body, "response_type=code&scope=read&nonce=n-0S6_WzA2Mj");
}

TEST(build_par_request_composes_with_client_authentication) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.redirect_uri = "https://client.example/cb";
  request.scope = "read";
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  sourcemeta::core::oauth_client_id_only("s6BhdRkqt3", body);
  EXPECT_EQ(body, "response_type=code"
                  "&redirect_uri=https%3A%2F%2Fclient.example%2Fcb"
                  "&scope=read&client_id=s6BhdRkqt3");

  std::string collected;
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthAuthorizationRequest parsed;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_par_request(
      std::string_view{body}, storage, parsed,
      [&collected](std::string_view name, std::string_view) {
        collected.append(name);
      }));
  EXPECT_EQ(parsed.client_id, "s6BhdRkqt3");
  EXPECT_EQ(parsed.redirect_uri, "https://client.example/cb");
  EXPECT_EQ(parsed.scope, "read");
  EXPECT_TRUE(collected.empty());
}

TEST(build_par_request_honors_a_custom_response_type) {
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.response_type = "code id_token";
  request.scope = "openid";
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  EXPECT_EQ(body, "response_type=code%20id_token&scope=openid");
}

TEST(build_par_request_emits_repeatable_resources) {
  const std::array<sourcemeta::core::OAuthParameter, 2> resources{
      {{"resource", "https://api.example"},
       {"resource", "https://other.example"}}};
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.scope = "read";
  request.resources = resources;
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  EXPECT_EQ(body, "response_type=code&scope=read"
                  "&resource=https%3A%2F%2Fapi.example"
                  "&resource=https%3A%2F%2Fother.example");
}

TEST(build_par_request_drops_a_smuggled_request_uri) {
  const std::array<sourcemeta::core::OAuthParameter, 2> extra{
      {{"request_uri", "urn:evil"}, {"nonce", "n-0S6"}}};
  sourcemeta::core::OAuthAuthorizationRequest request;
  request.scope = "read";
  request.extra = extra;
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_par_request(request, body);
  EXPECT_EQ(body, "response_type=code&scope=read&nonce=n-0S6");
}

TEST(build_par_authorization_url_emits_the_reference_form) {
  std::string url;
  sourcemeta::core::oauth_build_par_authorization_url(
      "https://server.example/authorize", "s6BhdRkqt3",
      "urn:ietf:params:oauth:request_uri:6esc", url);
  EXPECT_EQ(url,
            "https://server.example/authorize?client_id=s6BhdRkqt3&request_uri="
            "urn%3Aietf%3Aparams%3Aoauth%3Arequest_uri%3A6esc");
}

TEST(build_par_authorization_url_continues_an_existing_query) {
  std::string url;
  sourcemeta::core::oauth_build_par_authorization_url(
      "https://server.example/authorize?foo=bar", "s6BhdRkqt3", "urn:x", url);
  EXPECT_EQ(url, "https://server.example/authorize?foo=bar&client_id=s6BhdRkqt3"
                 "&request_uri=urn%3Ax");
}

TEST(build_par_authorization_url_endpoint_ending_in_a_separator) {
  std::string opened;
  sourcemeta::core::oauth_build_par_authorization_url(
      "https://server.example/authorize?", "s6BhdRkqt3", "urn:x", opened);
  EXPECT_EQ(opened, "https://server.example/authorize?client_id=s6BhdRkqt3"
                    "&request_uri=urn%3Ax");

  std::string continued;
  sourcemeta::core::oauth_build_par_authorization_url(
      "https://server.example/authorize?foo=bar&", "s6BhdRkqt3", "urn:x",
      continued);
  EXPECT_EQ(continued,
            "https://server.example/authorize?foo=bar&client_id=s6BhdRkqt3"
            "&request_uri=urn%3Ax");
}

TEST(par_response_reads_the_members) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "request_uri": "urn:ietf:params:oauth:request_uri:6esc",
    "expires_in": 60
  })JSON")};
  const sourcemeta::core::OAuthPARResponse response{document};
  EXPECT_EQ(response.request_uri().value(),
            "urn:ietf:params:oauth:request_uri:6esc");
  EXPECT_EQ(response.expires_in().value(), std::chrono::seconds{60});
}

TEST(par_response_rejects_a_non_positive_lifetime) {
  const auto zero{sourcemeta::core::parse_json(R"JSON({
    "request_uri": "urn:x", "expires_in": 0
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::OAuthPARResponse{zero}.expires_in().has_value());
  const auto negative{sourcemeta::core::parse_json(R"JSON({
    "request_uri": "urn:x", "expires_in": -1
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::OAuthPARResponse{negative}.expires_in().has_value());
}

TEST(par_response_absent_members_are_empty) {
  const auto document{sourcemeta::core::parse_json(R"JSON({})JSON")};
  const sourcemeta::core::OAuthPARResponse response{document};
  EXPECT_FALSE(response.request_uri().has_value());
  EXPECT_FALSE(response.expires_in().has_value());
}

TEST(parse_par_request_parses_a_valid_body) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_par_request(
      "response_type=code&client_id=s6BhdRkqt3&scope=read", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.response_type, "code");
  EXPECT_EQ(request.client_id, "s6BhdRkqt3");
  EXPECT_EQ(request.scope, "read");
}

TEST(parse_par_request_rejects_a_request_uri) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_par_request(
      "response_type=code&client_id=s6BhdRkqt3&request_uri=urn%3Ax", storage,
      request, [](std::string_view, std::string_view) {}));
}

TEST(parse_par_request_rejects_a_percent_encoded_request_uri) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  // RFC 6749 Appendix B: the parameter name is form-decoded before it is
  // recognized, so request%5Furi is the request_uri parameter and is rejected
  EXPECT_FALSE(sourcemeta::core::oauth_parse_par_request(
      "client_id=s6BhdRkqt3&request%5Furi=urn%3Ax", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(parse_par_request_rejects_an_empty_request_uri) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  // RFC 9126 Section 2.1: the parameter MUST NOT be provided, so its presence
  // is rejected even with an empty value
  EXPECT_FALSE(sourcemeta::core::oauth_parse_par_request(
      "client_id=s6BhdRkqt3&request_uri=", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(parse_par_request_surfaces_client_authentication_parameters) {
  std::string collected;
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthAuthorizationRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_par_request(
      "client_id=s6BhdRkqt3&client_secret=secret", storage, request,
      [&collected](std::string_view name, std::string_view value) {
        collected.append(name);
        collected.push_back('=');
        collected.append(value);
      }));
  EXPECT_EQ(collected, "client_secret=secret");
}

TEST(par_request_uri_uses_the_urn_form) {
  const auto request_uri{sourcemeta::core::oauth_par_request_uri()};
  EXPECT_TRUE(std::string_view{request_uri}.starts_with(
      "urn:ietf:params:oauth:request_uri:"));
  EXPECT_EQ(request_uri.size(),
            std::string_view{"urn:ietf:params:oauth:request_uri:"}.size() + 43);
}

TEST(par_request_uri_is_unpredictable) {
  EXPECT_NE(sourcemeta::core::oauth_par_request_uri(),
            sourcemeta::core::oauth_par_request_uri());
}

TEST(make_par_response_emits_the_members) {
  const auto response{sourcemeta::core::oauth_make_par_response(
      "urn:ietf:params:oauth:request_uri:6esc", std::chrono::seconds{60})};
  EXPECT_EQ(response.value().at("request_uri").to_string(),
            "urn:ietf:params:oauth:request_uri:6esc");
  EXPECT_EQ(response.value().at("expires_in").to_integer(), 60);
}

TEST(make_par_response_rejects_an_empty_request_uri) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_make_par_response("", std::chrono::seconds{60})
          .has_value());
}

TEST(make_par_response_rejects_a_non_positive_lifetime) {
  EXPECT_FALSE(sourcemeta::core::oauth_make_par_response(
                   "urn:x", std::chrono::seconds{0})
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_make_par_response(
                   "urn:x", std::chrono::seconds{-1})
                   .has_value());
}

TEST(par_dpop_binding_unifies_matching_sources) {
  EXPECT_EQ(sourcemeta::core::oauth_par_dpop_binding("abc", "abc").value(),
            "abc");
}

TEST(par_dpop_binding_rejects_a_mismatch) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_par_dpop_binding("abc", "xyz").has_value());
}

TEST(par_dpop_binding_uses_the_only_present_source) {
  EXPECT_EQ(
      sourcemeta::core::oauth_par_dpop_binding("abc", std::nullopt).value(),
      "abc");
  EXPECT_EQ(sourcemeta::core::oauth_par_dpop_binding("", "xyz").value(), "xyz");
}

TEST(par_dpop_binding_is_empty_when_neither_is_present) {
  const auto binding{
      sourcemeta::core::oauth_par_dpop_binding("", std::nullopt)};
  EXPECT_TRUE(binding.has_value());
  EXPECT_TRUE(binding.value().empty());
}

TEST(par_dpop_binding_ignores_an_empty_proof) {
  // An empty proof thumbprint is treated as absent, so it neither mismatches a
  // present dpop_jkt nor counts as a binding on its own
  EXPECT_EQ(sourcemeta::core::oauth_par_dpop_binding(
                "abc", std::optional<std::string_view>{""})
                .value(),
            "abc");
  EXPECT_TRUE(sourcemeta::core::oauth_par_dpop_binding(
                  "", std::optional<std::string_view>{""})
                  .value()
                  .empty());
}
