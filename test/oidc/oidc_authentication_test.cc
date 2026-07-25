#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

static auto contains(const std::string &haystack, const std::string_view needle)
    -> bool {
  return haystack.find(needle) != std::string::npos;
}

TEST(build_includes_the_core_parameters) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid profile";
  request.state = "xyz";
  request.nonce = "n-0S6_WzA2Mj";
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  request.code_challenge_method = "S256";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
  EXPECT_TRUE(contains(url, "response_type=code"));
  EXPECT_TRUE(contains(url, "client_id=s6BhdRkqt3"));
  EXPECT_TRUE(contains(url, "scope=openid%20profile"));
  EXPECT_TRUE(contains(url, "state=xyz"));
  EXPECT_TRUE(contains(url, "nonce=n-0S6_WzA2Mj"));
}

TEST(build_appends_the_openid_connect_parameters) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.display = "page";
  request.prompt = "consent";
  request.max_age = "3600";
  request.ui_locales = "en-US";
  request.login_hint = "user@example.com";
  request.acr_values = "urn:acr:1";
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  request.code_challenge_method = "S256";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
  EXPECT_TRUE(contains(url, "display=page"));
  EXPECT_TRUE(contains(url, "prompt=consent"));
  EXPECT_TRUE(contains(url, "max_age=3600"));
  EXPECT_TRUE(contains(url, "ui_locales=en-US"));
  EXPECT_TRUE(contains(url, "login_hint=user%40example.com"));
  EXPECT_TRUE(contains(url, "acr_values=urn%3Aacr%3A1"));
}

TEST(build_rejects_a_scope_without_openid) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "profile email";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_rejects_an_empty_scope) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_rejects_an_empty_client_id) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_rejects_an_empty_redirect_uri) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.scope = "openid";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_rejects_a_non_exclusive_none_prompt) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.prompt = "none consent";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_accepts_an_exclusive_none_prompt) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.prompt = "none";
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  request.code_challenge_method = "S256";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
  EXPECT_TRUE(contains(url, "prompt=none"));
}

TEST(build_rejects_offline_access_with_a_none_prompt) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid offline_access";
  request.prompt = "none";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_accepts_offline_access_with_a_consent_prompt) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid offline_access";
  request.prompt = "consent";
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  request.code_challenge_method = "S256";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
  EXPECT_TRUE(contains(url, "scope=openid%20offline_access"));
}

TEST(build_accepts_a_code_response_type_under_strict) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code";
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  request.code_challenge_method = "S256";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
  EXPECT_TRUE(contains(url, "response_type=code"));
}

TEST(build_rejects_a_missing_pkce_under_strict) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_rejects_a_plain_pkce_method_under_strict) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code";
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  request.code_challenge_method = "plain";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_rejects_a_short_code_challenge_under_strict) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code";
  request.code_challenge = "too-short";
  request.code_challenge_method = "S256";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_rejects_a_code_challenge_with_invalid_characters_under_strict) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code";
  // Forty-three characters, but the trailing "!" is outside the unreserved set
  request.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-c!";
  request.code_challenge_method = "S256";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_accepts_a_missing_pkce_under_legacy) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url,
      sourcemeta::core::OIDCProfile::Legacy));
  EXPECT_TRUE(contains(url, "response_type=code"));
}

TEST(build_rejects_a_hybrid_response_type_under_strict) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code id_token";
  request.nonce = "n-0S6";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url));
}

TEST(build_accepts_a_hybrid_response_type_under_legacy) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code id_token";
  request.nonce = "n-0S6";
  std::string url;
  EXPECT_TRUE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url,
      sourcemeta::core::OIDCProfile::Legacy));
  EXPECT_TRUE(contains(url, "response_type=code%20id_token"));
}

TEST(build_rejects_a_hybrid_without_a_nonce_under_legacy) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code id_token";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url,
      sourcemeta::core::OIDCProfile::Legacy));
}

TEST(build_rejects_an_implicit_response_type_under_legacy) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "id_token";
  request.nonce = "n-0S6";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url,
      sourcemeta::core::OIDCProfile::Legacy));
}

TEST(build_rejects_an_access_token_response_type_under_legacy) {
  sourcemeta::core::OIDCAuthenticationRequest request;
  request.client_id = "s6BhdRkqt3";
  request.redirect_uri = "https://client.example/cb";
  request.scope = "openid";
  request.response_type = "code token";
  request.nonce = "n-0S6";
  std::string url;
  EXPECT_FALSE(sourcemeta::core::oidc_build_authentication_url(
      "https://server.example/authorize", request, url,
      sourcemeta::core::OIDCProfile::Legacy));
}

TEST(authorization_url_convenience_enforces_the_defaults) {
  const auto url{sourcemeta::core::oidc_authorization_url(
      "https://server.example/authorize", "s6BhdRkqt3",
      "https://client.example/cb", "xyz",
      "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM", "n-0S6_WzA2Mj")};
  EXPECT_TRUE(url.has_value());
  EXPECT_TRUE(contains(url.value(), "response_type=code"));
  EXPECT_TRUE(contains(url.value(), "scope=openid"));
  EXPECT_TRUE(
      contains(url.value(),
               "code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM"));
  EXPECT_TRUE(contains(url.value(), "code_challenge_method=S256"));
  EXPECT_TRUE(contains(url.value(), "nonce=n-0S6_WzA2Mj"));
  EXPECT_TRUE(contains(url.value(), "state=xyz"));
}

TEST(parse_extracts_the_base_and_openid_parameters) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_TRUE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid%20profile&"
      "nonce=n-0S6&prompt=consent&max_age=3600&acr_values=urn%3Aacr%3A1&"
      "code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM&code_"
      "challenge_method=S256",
      storage, request));
  EXPECT_EQ(request.response_type, "code");
  EXPECT_EQ(request.client_id, "s6BhdRkqt3");
  EXPECT_EQ(request.redirect_uri, "https://client.example/cb");
  EXPECT_EQ(request.scope, "openid profile");
  EXPECT_EQ(request.nonce, "n-0S6");
  EXPECT_EQ(request.prompt, "consent");
  EXPECT_EQ(request.max_age, "3600");
  EXPECT_EQ(request.acr_values, "urn:acr:1");
}

TEST(authorization_url_rejects_an_empty_code_challenge) {
  const auto url{sourcemeta::core::oidc_authorization_url(
      "https://server.example/authorize", "s6BhdRkqt3",
      "https://client.example/cb", "xyz", "", "n-0S6_WzA2Mj")};
  EXPECT_FALSE(url.has_value());
}

TEST(parse_rejects_a_missing_client_id) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&redirect_uri=https%3A%2F%2Fclient.example%2Fcb&"
      "scope=openid",
      storage, request));
}

TEST(parse_rejects_a_missing_redirect_uri) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=s6BhdRkqt3&scope=openid", storage,
      request));
}

TEST(parse_rejects_a_scope_without_openid) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=profile%20email",
      storage, request));
}

TEST(parse_rejects_a_non_exclusive_none_prompt) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid&"
      "prompt=none%20consent",
      storage, request));
}

TEST(parse_rejects_offline_access_with_a_none_prompt) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&"
      "scope=openid%20offline_access&prompt=none",
      storage, request));
}

TEST(parse_accepts_offline_access_with_a_consent_prompt) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_TRUE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&"
      "scope=openid%20offline_access&prompt=consent&"
      "code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM&code_"
      "challenge_method=S256",
      storage, request));
  EXPECT_EQ(request.scope, "openid offline_access");
}

TEST(parse_rejects_a_hybrid_response_type_under_strict) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code%20id_token&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid&nonce=n-0S6",
      storage, request));
}

TEST(parse_accepts_a_hybrid_response_type_under_legacy) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_TRUE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code%20id_token&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid&nonce=n-0S6",
      storage, request, sourcemeta::core::OIDCProfile::Legacy));
  EXPECT_EQ(request.response_type, "code id_token");
}

TEST(parse_rejects_a_hybrid_without_a_nonce_under_legacy) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code%20id_token&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid",
      storage, request, sourcemeta::core::OIDCProfile::Legacy));
}

TEST(parse_rejects_a_missing_response_type) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid&"
      "code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM&code_"
      "challenge_method=S256",
      storage, request));
}

TEST(parse_rejects_a_missing_pkce_under_strict) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid",
      storage, request));
}

TEST(parse_resets_stale_fields_across_reuse) {
  std::string first_storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_TRUE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=s6BhdRkqt3&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid&nonce=n-0S6&"
      "code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM&code_"
      "challenge_method=S256",
      first_storage, request));
  EXPECT_EQ(request.nonce, "n-0S6");

  // A second parse into the same object must not retain the earlier nonce
  std::string second_storage;
  EXPECT_TRUE(sourcemeta::core::oidc_parse_authentication_request(
      "response_type=code&client_id=other&"
      "redirect_uri=https%3A%2F%2Fclient.example%2Fcb&scope=openid&"
      "code_challenge=E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM&code_"
      "challenge_method=S256",
      second_storage, request));
  EXPECT_EQ(request.client_id, "other");
  EXPECT_TRUE(request.nonce.empty());
}

TEST(nonce_is_the_expected_length) {
  const auto nonce{sourcemeta::core::oidc_nonce()};
  EXPECT_EQ(nonce.size(), 43);
}

TEST(nonce_is_fresh_on_each_call) {
  const auto first{sourcemeta::core::oidc_nonce()};
  const auto second{sourcemeta::core::oidc_nonce()};
  EXPECT_FALSE(first == second);
}

TEST(parse_rejects_a_duplicated_parameter) {
  std::string storage;
  sourcemeta::core::OIDCAuthenticationRequest request;
  EXPECT_FALSE(sourcemeta::core::oidc_parse_authentication_request(
      "client_id=a&client_id=b&scope=openid", storage, request));
}
