#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <chrono> // std::chrono::seconds

TEST(build_introspection_request_emits_the_token) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_introspection_request("mF_9.B5f-4.1JqM", "",
                                                      body);
  EXPECT_TRUE(body == "token=mF_9.B5f-4.1JqM");
}

TEST(introspection_response_reads_an_active_token) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "active": true,
    "scope": "read write",
    "client_id": "l238j323ds-23ij4",
    "username": "jdoe",
    "token_type": "Bearer",
    "sub": "Z5O3upPC88QrAjx00dis",
    "iss": "https://server.example",
    "exp": 1419356238
  })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_TRUE(response.active());
  EXPECT_EQ(response.scope().value(), "read write");
  EXPECT_EQ(response.client_id().value(), "l238j323ds-23ij4");
  EXPECT_EQ(response.username().value(), "jdoe");
  EXPECT_EQ(response.token_type().value(), "Bearer");
  EXPECT_EQ(response.subject().value(), "Z5O3upPC88QrAjx00dis");
  EXPECT_EQ(response.issuer().value(), "https://server.example");
  EXPECT_EQ(response.expiration().value(), std::chrono::seconds{1419356238});
}

TEST(introspection_response_reads_an_inactive_token) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "active": false })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_FALSE(response.active());
  EXPECT_FALSE(response.scope().has_value());
}

TEST(introspection_response_treats_a_malformed_active_as_inactive) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "active": "true" })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_FALSE(response.active());
}

TEST(introspection_response_treats_a_missing_active_as_inactive) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "scope": "read" })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_FALSE(response.active());
}

TEST(make_introspection_inactive_emits_active_false) {
  const auto response{sourcemeta::core::oauth_make_introspection_inactive()};
  EXPECT_TRUE(response.defines("active"));
  EXPECT_FALSE(response.at("active").to_boolean());
}

TEST(introspection_response_active_with_no_other_members) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "active": true })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_TRUE(response.active());
  EXPECT_FALSE(response.scope().has_value());
  EXPECT_FALSE(response.expiration().has_value());
}

TEST(introspection_response_ignores_non_integer_timestamps) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "active": true, "exp": "1419356238", "iat": 1419350238.5
  })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_FALSE(response.expiration().has_value());
  EXPECT_FALSE(response.issued_at().has_value());
}

TEST(introspection_response_treats_numeric_active_as_inactive) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "active": 1 })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_FALSE(response.active());
}

TEST(introspection_response_treats_null_active_as_inactive) {
  const auto document{
      sourcemeta::core::parse_json(R"JSON({ "active": null })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_FALSE(response.active());
}

TEST(introspection_response_reads_a_dpop_token_type) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "active": true, "token_type": "DPoP"
  })JSON")};
  const sourcemeta::core::OAuthIntrospectionResponse response{document};
  EXPECT_EQ(response.token_type().value(), "DPoP");
}
