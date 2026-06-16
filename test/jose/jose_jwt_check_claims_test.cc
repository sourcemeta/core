#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>

#include <chrono>      // std::chrono::system_clock, std::chrono::seconds
#include <string>      // std::string
#include <string_view> // std::string_view

// Build a compact JSON Web Token from its three component parts, base64url
// encoding each so the inputs are exact rather than hand-encoded
static auto make_token(const std::string_view header,
                       const std::string_view payload,
                       const std::string_view signature) -> std::string {
  return sourcemeta::core::base64url_encode(header) + "." +
         sourcemeta::core::base64url_encode(payload) + "." +
         sourcemeta::core::base64url_encode(signature);
}

TEST(JOSE_jwt_check_claims, valid) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(JOSE_jwt_check_claims, missing_issuer) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Issuer);
}

TEST(JOSE_jwt_check_claims, wrong_issuer) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "evil", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Issuer);
}

TEST(JOSE_jwt_check_claims, missing_audience) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "iss": "acme", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Audience);
}

TEST(JOSE_jwt_check_claims, wrong_audience) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "other", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Audience);
}

TEST(JOSE_jwt_check_claims, audience_array_contains) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": [ "a", "client", "b" ], "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(JOSE_jwt_check_claims, valid_with_subject) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "sub": "user", "aud": "client", "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{0},
      "user")};
  EXPECT_FALSE(error.has_value());
}

TEST(JOSE_jwt_check_claims, wrong_subject) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "sub": "user", "aud": "client", "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{0},
      "other")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Subject);
}

TEST(JOSE_jwt_check_claims, missing_subject_when_expected) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{0},
      "user")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Subject);
}

TEST(JOSE_jwt_check_claims, subject_ignored_when_not_pinned) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "sub": "user", "aud": "client", "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(JOSE_jwt_check_claims, missing_expiration) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "iss": "acme", "aud": "client" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(JOSE_jwt_check_claims, malformed_expiration) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": "soon" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(JOSE_jwt_check_claims, expired) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 500 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(JOSE_jwt_check_claims, expired_within_skew) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 900 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{200})};
  EXPECT_FALSE(error.has_value());
}

TEST(JOSE_jwt_check_claims, expired_beyond_skew) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 900 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{50})};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(JOSE_jwt_check_claims, not_before_satisfied) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": 500 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(JOSE_jwt_check_claims, not_yet_valid) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": 1500 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::NotBefore);
}

TEST(JOSE_jwt_check_claims, not_yet_valid_within_skew) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": 1100 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{200})};
  EXPECT_FALSE(error.has_value());
}

// A present but malformed not-before fails closed rather than being ignored.
// Borrowed in spirit from jwt-cpp's claim handling
// (https://github.com/Thalhammer/jwt-cpp)
TEST(JOSE_jwt_check_claims, malformed_not_before) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": "soon" })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::NotBefore);
}

TEST(JOSE_jwt_check_claims, issued_in_the_past) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": 500 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(JOSE_jwt_check_claims, issued_in_the_future) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": 1500 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::IssuedAt);
}

TEST(JOSE_jwt_check_claims, issued_in_the_future_within_skew) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": 1100 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{200})};
  EXPECT_FALSE(error.has_value());
}

TEST(JOSE_jwt_check_claims, malformed_issued_at) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": "soon" })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::IssuedAt);
}

TEST(JOSE_jwt_check_claims, issuer_checked_before_audience) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "evil", "aud": "other", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Issuer);
}

TEST(JOSE_jwt_check_claims, audience_checked_before_expiration) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "other", "exp": 500 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  ASSERT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Audience);
}
