#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

// These tests exercise the client build side and the authorization server parse
// side against each other, so that the two halves of the code plus PKCE spine
// agree on the wire rather than only in isolation.

namespace {

// Recover the raw query a builder appended to an endpoint, owning it so the
// view fed to the parser outlives the temporary URI it came from
auto query_of(const std::string_view url) -> std::string {
  const sourcemeta::core::URI parsed{url};
  const auto query{parsed.query()};
  return query.has_value() ? std::string{query.value().raw()} : std::string{};
}

} // namespace

TEST(conformance_authorization_request_round_trips) {
  sourcemeta::core::OAuthAuthorizationRequest built;
  built.client_id = "s6BhdRkqt3";
  built.redirect_uri = "https://client.example/cb";
  built.scope = "openid profile";
  built.state = "xyz";
  built.code_challenge = "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM";
  built.code_challenge_method = "S256";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url(
      "https://server.example/authorize", built, url);

  const auto query{query_of(url)};
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest parsed;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      query, storage, parsed, [](std::string_view, std::string_view) {}));
  EXPECT_EQ(parsed.response_type, "code");
  EXPECT_EQ(parsed.client_id, "s6BhdRkqt3");
  EXPECT_EQ(parsed.redirect_uri, "https://client.example/cb");
  EXPECT_EQ(parsed.scope, "openid profile");
  EXPECT_EQ(parsed.state, "xyz");
  EXPECT_EQ(parsed.code_challenge,
            "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM");
  EXPECT_EQ(parsed.code_challenge_method, "S256");
}

TEST(conformance_authorization_request_resources_round_trip) {
  const std::array<sourcemeta::core::OAuthParameter, 2> resources{
      {{"resource", "https://a.example"}, {"resource", "https://b.example"}}};
  sourcemeta::core::OAuthAuthorizationRequest built;
  built.client_id = "s6BhdRkqt3";
  built.resources = resources;
  std::string url;
  sourcemeta::core::oauth_build_authorization_url(
      "https://server.example/authorize", built, url);

  const auto query{query_of(url)};
  std::string storage;
  sourcemeta::core::OAuthAuthorizationRequest parsed;
  std::string collected;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      query, storage, parsed,
      [&collected](std::string_view name, std::string_view value) {
        collected.append(name);
        collected.push_back('=');
        collected.append(value);
        collected.push_back(';');
      }));
  EXPECT_EQ(collected,
            "resource=https://a.example;resource=https://b.example;");
}

TEST(conformance_token_request_code_round_trips) {
  const std::span<const sourcemeta::core::OAuthParameter> resources;
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_code(
      "SplxlOBeZQQYbYS6WxSbIA", "https://client.example/cb",
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk", resources, body);

  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthTokenRequest parsed;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_token_request(
      std::string_view{body}, storage, parsed,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(parsed.grant_type, "authorization_code");
  EXPECT_EQ(parsed.code, "SplxlOBeZQQYbYS6WxSbIA");
  EXPECT_EQ(parsed.redirect_uri, "https://client.example/cb");
  EXPECT_EQ(parsed.code_verifier,
            "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk");
}

TEST(conformance_client_secret_basic_round_trips) {
  sourcemeta::core::SecureString header;
  sourcemeta::core::oauth_client_secret_basic("s6BhdRkqt3", "gX1fBat3bV",
                                              header);

  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      std::string_view{header}, "", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Basic);
  EXPECT_EQ(credentials.client_id, "s6BhdRkqt3");
  EXPECT_EQ(credentials.client_secret, "gX1fBat3bV");
}

TEST(conformance_client_secret_post_round_trips) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_client_secret_post("s6BhdRkqt3", "gX1fBat3bV", body);

  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "", std::string_view{body}, storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Post);
  EXPECT_EQ(credentials.client_id, "s6BhdRkqt3");
  EXPECT_EQ(credentials.client_secret, "gX1fBat3bV");
}

TEST(conformance_token_response_round_trips) {
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "2YotnFZFEjr1zCsicMWpAA";
  grant.token_type = "Bearer";
  grant.expires_in = std::chrono::seconds{3600};
  grant.refresh_token = "tGzv3JOkF0XG5Qx2TlKWIA";
  const auto document{sourcemeta::core::oauth_make_token_response(grant)};

  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.is_bearer_token_type());
  EXPECT_EQ(response.access_token().value(), "2YotnFZFEjr1zCsicMWpAA");
  EXPECT_EQ(response.expires_in().value(), std::chrono::seconds{3600});
  EXPECT_EQ(response.refresh_token().value(), "tGzv3JOkF0XG5Qx2TlKWIA");
}

TEST(conformance_pkce_mint_round_trips) {
  const auto verifier{sourcemeta::core::oauth_pkce_verifier()};
  const std::string_view verifier_view{verifier.data(), verifier.size()};
  const auto challenge{sourcemeta::core::oauth_pkce_challenge(verifier_view)};
  const std::string_view challenge_view{challenge.data(), challenge.size()};
  EXPECT_EQ(sourcemeta::core::oauth_pkce_verify(
                verifier_view, challenge_view,
                sourcemeta::core::OAuthPKCEMethod::S256,
                sourcemeta::core::OAuthProfile::Strict),
            sourcemeta::core::OAuthPKCEOutcome::Match);
}

TEST(conformance_full_code_pkce_exchange) {
  // The client mints a PKCE pair and drives a code request through the server.
  static constexpr std::string_view verifier{
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk"};
  const auto challenge{sourcemeta::core::oauth_pkce_challenge(verifier)};
  const std::string_view challenge_view{challenge.data(), challenge.size()};

  sourcemeta::core::OAuthAuthorizationRequest authorization;
  authorization.client_id = "s6BhdRkqt3";
  authorization.redirect_uri = "https://client.example/cb";
  authorization.state = "xyz";
  authorization.code_challenge = challenge_view;
  authorization.code_challenge_method = "S256";
  std::string url;
  sourcemeta::core::oauth_build_authorization_url(
      "https://server.example/authorize", authorization, url);

  // The server parses the authorization request and captures the challenge it
  // will pair with the issued code.
  const auto authorization_query{query_of(url)};
  std::string authorization_storage;
  sourcemeta::core::OAuthAuthorizationRequest parsed_authorization;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_request(
      authorization_query, authorization_storage, parsed_authorization,
      [](std::string_view, std::string_view) {}));
  const std::string stored_challenge{parsed_authorization.code_challenge};

  // The server issues a redirect carrying the code and the echoed state.
  sourcemeta::core::OAuthAuthorizationResponse issued;
  issued.code = "SplxlOBeZQQYbYS6WxSbIA";
  issued.state = parsed_authorization.state;
  issued.iss = "https://server.example";
  std::string redirect;
  EXPECT_TRUE(sourcemeta::core::oauth_build_authorization_redirect(
      parsed_authorization.redirect_uri, issued, redirect));

  // The client parses the redirect and recovers the code and its state.
  const auto redirect_query{query_of(redirect)};
  std::string redirect_storage;
  sourcemeta::core::OAuthAuthorizationResponse callback;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_authorization_response(
      redirect_query, redirect_storage, callback));
  EXPECT_EQ(callback.code, "SplxlOBeZQQYbYS6WxSbIA");
  EXPECT_EQ(callback.state, "xyz");
  EXPECT_EQ(callback.iss, "https://server.example");

  // The client exchanges the code, presenting the verifier and a Basic secret.
  const std::span<const sourcemeta::core::OAuthParameter> resources;
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_code(
      callback.code, "https://client.example/cb", verifier, resources, body);
  sourcemeta::core::SecureString header;
  sourcemeta::core::oauth_client_secret_basic("s6BhdRkqt3", "gX1fBat3bV",
                                              header);

  // The server parses the token request and the client authentication.
  sourcemeta::core::SecureString token_storage;
  sourcemeta::core::OAuthTokenRequest token_request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_token_request(
      std::string_view{body}, token_storage, token_request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(token_request.grant_type, "authorization_code");
  EXPECT_EQ(token_request.code, "SplxlOBeZQQYbYS6WxSbIA");

  sourcemeta::core::SecureString credentials_storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      std::string_view{header}, std::string_view{body}, credentials_storage,
      credentials));
  EXPECT_EQ(credentials.client_id, "s6BhdRkqt3");

  // The server verifies the presented verifier against the stored challenge.
  EXPECT_EQ(sourcemeta::core::oauth_pkce_verify(
                token_request.code_verifier, stored_challenge,
                sourcemeta::core::OAuthPKCEMethod::S256,
                sourcemeta::core::OAuthProfile::Strict),
            sourcemeta::core::OAuthPKCEOutcome::Match);

  // The server issues the token, and the client reads it back.
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "2YotnFZFEjr1zCsicMWpAA";
  grant.token_type = "Bearer";
  grant.expires_in = std::chrono::seconds{3600};
  const auto response_document{
      sourcemeta::core::oauth_make_token_response(grant)};
  const sourcemeta::core::OAuthTokenResponse response{response_document};
  EXPECT_TRUE(response.is_bearer_token_type());
  EXPECT_EQ(response.access_token().value(), "2YotnFZFEjr1zCsicMWpAA");
}
