#include <sourcemeta/core/test.h>

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

TEST(valid) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(missing_issuer) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Issuer);
}

TEST(wrong_issuer) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "evil", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Issuer);
}

TEST(missing_audience) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "iss": "acme", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Audience);
}

TEST(wrong_audience) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "other", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Audience);
}

TEST(audience_array_contains) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": [ "a", "client", "b" ], "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(valid_with_subject) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "sub": "user", "aud": "client", "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{0},
      "user")};
  EXPECT_FALSE(error.has_value());
}

TEST(wrong_subject) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "sub": "user", "aud": "client", "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{0},
      "other")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Subject);
}

TEST(missing_subject_when_expected) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{0},
      "user")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Subject);
}

TEST(subject_ignored_when_not_pinned) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "sub": "user", "aud": "client", "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(missing_expiration) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "iss": "acme", "aud": "client" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(malformed_expiration) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": "soon" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(expired) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 500 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(expired_within_skew) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 900 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{200})};
  EXPECT_FALSE(error.has_value());
}

TEST(expired_beyond_skew) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 900 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{50})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(not_before_satisfied) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": 500 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(not_yet_valid) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": 1500 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::NotBefore);
}

TEST(not_yet_valid_within_skew) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": 1100 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{200})};
  EXPECT_FALSE(error.has_value());
}

// A present but malformed not-before fails closed rather than being ignored.
// Borrowed in spirit from jwt-cpp's claim handling
// (https://github.com/Thalhammer/jwt-cpp)
TEST(malformed_not_before) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": "soon" })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::NotBefore);
}

TEST(issued_in_the_past) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": 500 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_FALSE(error.has_value());
}

TEST(issued_in_the_future) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": 1500 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::IssuedAt);
}

TEST(issued_in_the_future_within_skew) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": 1100 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{200})};
  EXPECT_FALSE(error.has_value());
}

TEST(malformed_issued_at) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": "soon" })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::IssuedAt);
}

TEST(issuer_checked_before_audience) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "evil", "aud": "other", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Issuer);
}

TEST(audience_checked_before_expiration) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "other", "exp": 500 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Audience);
}
