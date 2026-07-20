#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>  // std::array
#include <chrono> // std::chrono::seconds

TEST(build_token_request_code_minimal) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_code("SplxlOBeZQQYbYS6WxSbIA", "",
                                                   "", {}, body);
  EXPECT_TRUE(body ==
              "grant_type=authorization_code&code=SplxlOBeZQQYbYS6WxSbIA");
}

TEST(build_token_request_code_with_redirect_and_verifier) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_code(
      "SplxlOBeZQQYbYS6WxSbIA", "https://client.example/cb",
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk", {}, body);
  EXPECT_TRUE(body ==
              "grant_type=authorization_code&code=SplxlOBeZQQYbYS6WxSbIA"
              "&redirect_uri=https%3A%2F%2Fclient.example%2Fcb"
              "&code_verifier=dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk");
}

TEST(build_token_request_code_escapes_the_code) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_code("a/b+c", "", "", {}, body);
  EXPECT_TRUE(body == "grant_type=authorization_code&code=a%2Fb%2Bc");
}

TEST(build_token_request_code_with_resources) {
  const std::array<sourcemeta::core::OAuthParameter, 1> resources{
      {{"resource", "https://api.example"}}};
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_code("abc", "", "", resources,
                                                   body);
  EXPECT_TRUE(body == "grant_type=authorization_code&code=abc"
                      "&resource=https%3A%2F%2Fapi.example");
}

TEST(build_token_request_refresh_minimal) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_refresh("tGzv3JOkF0XG5Qx2TlKWIA",
                                                      "", {}, body);
  EXPECT_TRUE(body ==
              "grant_type=refresh_token&refresh_token=tGzv3JOkF0XG5Qx2TlKWIA");
}

TEST(build_token_request_refresh_with_scope) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_refresh("tGzv3JOkF0XG5Qx2TlKWIA",
                                                      "read write", {}, body);
  EXPECT_TRUE(body ==
              "grant_type=refresh_token&refresh_token=tGzv3JOkF0XG5Qx2TlKWIA"
              "&scope=read%20write");
}

TEST(build_token_request_client_credentials_minimal) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_client_credentials("", {}, body);
  EXPECT_TRUE(body == "grant_type=client_credentials");
}

TEST(build_token_request_client_credentials_with_scope) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_client_credentials("read", {},
                                                                 body);
  EXPECT_TRUE(body == "grant_type=client_credentials&scope=read");
}

TEST(build_token_request_composes_with_client_secret_post) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_code(
      "SplxlOBeZQQYbYS6WxSbIA", "https://client.example/cb", "", {}, body);
  sourcemeta::core::oauth_client_secret_post("s6BhdRkqt3", "gX1fBat3bV", body);
  EXPECT_TRUE(body ==
              "grant_type=authorization_code&code=SplxlOBeZQQYbYS6WxSbIA"
              "&redirect_uri=https%3A%2F%2Fclient.example%2Fcb"
              "&client_id=s6BhdRkqt3&client_secret=gX1fBat3bV");
}

TEST(token_response_reads_the_rfc6749_example) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "access_token": "2YotnFZFEjr1zCsicMWpAA",
    "token_type": "Bearer",
    "expires_in": 3600,
    "refresh_token": "tGzv3JOkF0XG5Qx2TlKWIA"
  })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.access_token().has_value());
  EXPECT_EQ(response.access_token().value(), "2YotnFZFEjr1zCsicMWpAA");
  EXPECT_TRUE(response.token_type().has_value());
  EXPECT_EQ(response.token_type().value(), "Bearer");
  EXPECT_TRUE(response.expires_in().has_value());
  EXPECT_TRUE(response.expires_in().value() == std::chrono::seconds{3600});
  EXPECT_TRUE(response.refresh_token().has_value());
  EXPECT_EQ(response.refresh_token().value(), "tGzv3JOkF0XG5Qx2TlKWIA");
}

TEST(token_response_bearer_type_is_case_insensitive) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "token_type": "bearer" })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.is_bearer_token_type());
}

TEST(token_response_non_bearer_type) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "token_type": "DPoP" })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_FALSE(response.is_bearer_token_type());
}

TEST(token_response_absent_fields_are_empty) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "access_token": "abc" })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_FALSE(response.token_type().has_value());
  EXPECT_FALSE(response.expires_in().has_value());
  EXPECT_FALSE(response.refresh_token().has_value());
  EXPECT_FALSE(response.scope().has_value());
  EXPECT_FALSE(response.is_bearer_token_type());
}

TEST(token_response_rejects_a_negative_expires_in) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "expires_in": -1 })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_FALSE(response.expires_in().has_value());
}

TEST(token_response_rejects_a_non_integer_expires_in) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "expires_in": "3600" })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_FALSE(response.expires_in().has_value());
}

TEST(token_response_on_a_non_object_is_empty) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_FALSE(response.access_token().has_value());
}

TEST(token_response_scope_membership) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "scope": "read write admin" })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.scope().has_value());
  EXPECT_EQ(response.scope().value(), "read write admin");
  EXPECT_TRUE(response.has_scope("read"));
  EXPECT_TRUE(response.has_scope("write"));
  EXPECT_TRUE(response.has_scope("admin"));
  EXPECT_FALSE(response.has_scope("delete"));
  EXPECT_FALSE(response.has_scope("wri"));
  EXPECT_FALSE(response.has_scope(""));
}

TEST(token_response_scope_membership_single_value) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "scope": "read" })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.has_scope("read"));
  EXPECT_FALSE(response.has_scope("write"));
}

TEST(token_response_id_token_via_passthrough) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "access_token": "abc",
    "id_token": "header.payload.signature"
  })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  const auto *id_token{response.data().try_at("id_token")};
  EXPECT_TRUE(id_token != nullptr);
  EXPECT_TRUE(id_token->is_string());
  EXPECT_EQ(id_token->to_string(), "header.payload.signature");
}

TEST(token_response_expires_in_zero_is_valid) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "expires_in": 0 })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.expires_in().has_value());
  EXPECT_TRUE(response.expires_in().value() == std::chrono::seconds{0});
}

TEST(token_response_rejects_a_fractional_expires_in) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "expires_in": 3600.5 })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_FALSE(response.expires_in().has_value());
}

TEST(token_response_empty_token_type_is_not_bearer) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "token_type": "" })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.token_type().has_value());
  EXPECT_FALSE(response.is_bearer_token_type());
}

TEST(token_response_non_string_access_token_is_empty) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "access_token": 123 })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_FALSE(response.access_token().has_value());
}

TEST(token_response_empty_scope_has_no_membership) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "scope": "" })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.scope().has_value());
  EXPECT_FALSE(response.has_scope("read"));
}

TEST(token_response_scope_with_trailing_and_double_spaces) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "scope": "read  write " })JSON")};
  const sourcemeta::core::OAuthTokenResponse response{document};
  EXPECT_TRUE(response.has_scope("read"));
  EXPECT_TRUE(response.has_scope("write"));
  EXPECT_FALSE(response.has_scope(""));
}

TEST(build_token_request_refresh_with_resources) {
  const std::array<sourcemeta::core::OAuthParameter, 1> resources{
      {{"resource", "https://api.example"}}};
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_refresh("rt", "", resources,
                                                      body);
  EXPECT_TRUE(body == "grant_type=refresh_token&refresh_token=rt"
                      "&resource=https%3A%2F%2Fapi.example");
}

TEST(build_token_request_client_credentials_with_two_resources) {
  const std::array<sourcemeta::core::OAuthParameter, 2> resources{
      {{"resource", "https://a"}, {"resource", "https://b"}}};
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_client_credentials(
      "read", resources, body);
  EXPECT_TRUE(body == "grant_type=client_credentials&scope=read"
                      "&resource=https%3A%2F%2Fa&resource=https%3A%2F%2Fb");
}

TEST(parse_token_request_reads_the_authorization_code_grant) {
  std::string storage;
  sourcemeta::core::OAuthTokenRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_token_request(
      "grant_type=authorization_code&code=SplxlOBeZQQYbYS6WxSbIA"
      "&redirect_uri=https%3A%2F%2Fc.example%2Fcb&code_verifier=abc",
      storage, request, [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.grant_type, "authorization_code");
  EXPECT_EQ(request.code, "SplxlOBeZQQYbYS6WxSbIA");
  EXPECT_EQ(request.redirect_uri, "https://c.example/cb");
  EXPECT_EQ(request.code_verifier, "abc");
}

TEST(parse_token_request_reads_the_refresh_grant) {
  std::string storage;
  sourcemeta::core::OAuthTokenRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_token_request(
      "grant_type=refresh_token&refresh_token=tGzv3JOkF0XG5Qx2TlKWIA&scope="
      "read",
      storage, request, [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.grant_type, "refresh_token");
  EXPECT_EQ(request.refresh_token, "tGzv3JOkF0XG5Qx2TlKWIA");
  EXPECT_EQ(request.scope, "read");
}

TEST(parse_token_request_rejects_a_duplicate_parameter) {
  std::string storage;
  sourcemeta::core::OAuthTokenRequest request;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_token_request(
      "grant_type=a&grant_type=b", storage, request,
      [](std::string_view, std::string_view) {}));
}

TEST(parse_token_request_surfaces_other_parameters_through_the_callback) {
  std::string storage;
  sourcemeta::core::OAuthTokenRequest request;
  std::string collected;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_token_request(
      "grant_type=client_credentials&resource=https%3A%2F%2Fa.example"
      "&audience=urn%3Aexample&client_id=s6BhdRkqt3",
      storage, request,
      [&collected](std::string_view name, std::string_view value) {
        collected.append(name);
        collected.push_back('=');
        collected.append(value);
        collected.push_back(';');
      }));
  EXPECT_EQ(request.grant_type, "client_credentials");
  EXPECT_EQ(collected, "resource=https://a.example;audience=urn:example;"
                       "client_id=s6BhdRkqt3;");
}

TEST(parse_token_request_leaves_absent_fields_empty) {
  std::string storage;
  sourcemeta::core::OAuthTokenRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_token_request(
      "grant_type=client_credentials", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_TRUE(request.code.empty());
  EXPECT_TRUE(request.refresh_token.empty());
  EXPECT_TRUE(request.scope.empty());
}

TEST(make_token_response_emits_the_required_members) {
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "2YotnFZFEjr1zCsicMWpAA";
  grant.token_type = "Bearer";
  grant.expires_in = std::chrono::seconds{3600};
  const auto response{sourcemeta::core::oauth_make_token_response(grant)};
  EXPECT_EQ(response.at("access_token").to_string(), "2YotnFZFEjr1zCsicMWpAA");
  EXPECT_EQ(response.at("token_type").to_string(), "Bearer");
  EXPECT_EQ(response.at("expires_in").to_integer(), 3600);
  EXPECT_FALSE(response.defines("refresh_token"));
  EXPECT_FALSE(response.defines("scope"));
}

TEST(make_token_response_emits_a_refresh_token) {
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "a";
  grant.token_type = "Bearer";
  grant.refresh_token = "tGzv3JOkF0XG5Qx2TlKWIA";
  const auto response{sourcemeta::core::oauth_make_token_response(grant)};
  EXPECT_EQ(response.at("refresh_token").to_string(), "tGzv3JOkF0XG5Qx2TlKWIA");
}

TEST(make_token_response_emits_scope_when_it_differs) {
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "a";
  grant.token_type = "Bearer";
  grant.scope = "read";
  grant.requested_scope = "read write";
  const auto response{sourcemeta::core::oauth_make_token_response(grant)};
  EXPECT_EQ(response.at("scope").to_string(), "read");
}

TEST(make_token_response_omits_scope_when_it_matches) {
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "a";
  grant.token_type = "Bearer";
  grant.scope = "read";
  grant.requested_scope = "read";
  const auto response{sourcemeta::core::oauth_make_token_response(grant)};
  EXPECT_FALSE(response.defines("scope"));
}

TEST(make_token_error_response_emits_the_error) {
  const auto response{sourcemeta::core::oauth_make_token_error_response(
      "invalid_grant", "The code expired", "")};
  EXPECT_EQ(response.at("error").to_string(), "invalid_grant");
  EXPECT_EQ(response.at("error_description").to_string(), "The code expired");
  EXPECT_FALSE(response.defines("error_uri"));
}

TEST(parse_token_request_treats_an_empty_first_value_as_omitted) {
  std::string storage;
  sourcemeta::core::OAuthTokenRequest request;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_token_request(
      "scope=&scope=read", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.scope, "read");
}

TEST(make_token_response_emits_scope_on_reordered_words) {
  // scope is an unordered set but the compare is textual, so a reordered but
  // equal set is still emitted, which is spec-permitted (RFC 6749 Section 5.1)
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "a";
  grant.token_type = "Bearer";
  grant.scope = "a b";
  grant.requested_scope = "b a";
  const auto response{sourcemeta::core::oauth_make_token_response(grant)};
  EXPECT_TRUE(response.defines("scope"));
  EXPECT_EQ(response.at("scope").to_string(), "a b");
}

TEST(make_token_response_emits_a_zero_expires_in) {
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "a";
  grant.token_type = "Bearer";
  grant.expires_in = std::chrono::seconds{0};
  const auto response{sourcemeta::core::oauth_make_token_response(grant)};
  EXPECT_TRUE(response.defines("expires_in"));
  EXPECT_EQ(response.at("expires_in").to_integer(), 0);
}

TEST(make_token_response_omits_a_negative_expires_in) {
  sourcemeta::core::OAuthTokenGrant grant;
  grant.access_token = "a";
  grant.token_type = "Bearer";
  grant.expires_in = std::chrono::seconds{-1};
  const auto response{sourcemeta::core::oauth_make_token_response(grant)};
  EXPECT_FALSE(response.defines("expires_in"));
}

TEST(parse_token_request_decodes_a_percent_encoded_name) {
  std::string storage;
  sourcemeta::core::OAuthTokenRequest request;
  // grant%5Ftype decodes to grant_type
  EXPECT_TRUE(sourcemeta::core::oauth_parse_token_request(
      "grant%5Ftype=client_credentials", storage, request,
      [](std::string_view, std::string_view) {}));
  EXPECT_EQ(request.grant_type, "client_credentials");
}
