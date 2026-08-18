#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

#include <chrono>      // std::chrono::system_clock, std::chrono::seconds
#include <optional>    // std::optional, std::nullopt
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

TEST(expiration_near_clock_maximum_does_not_overflow) {
  // A NumericDate just inside the clock's maximum, plus a positive skew, would
  // overflow the time point if the skew were added to the attacker claim (§4)
  const auto maximum{std::chrono::duration_cast<std::chrono::duration<double>>(
                         std::chrono::system_clock::duration::max())
                         .count()};
  const auto payload{
      std::string{R"({ "iss": "acme", "aud": "client", "exp": )"} +
      std::to_string(maximum - 2.0) + " }"};
  const auto input{make_token(R"({ "alg": "RS256" })", payload, "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{60})};
  // The far-future expiry means the token is not expired
  EXPECT_FALSE(error.has_value());
}

TEST(not_before_near_clock_minimum_does_not_underflow) {
  const auto minimum{std::chrono::duration_cast<std::chrono::duration<double>>(
                         std::chrono::system_clock::duration::min())
                         .count()};
  const auto payload{
      std::string{R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": )"} +
      std::to_string(minimum + 2.0) + " }"};
  const auto input{make_token(R"({ "alg": "RS256" })", payload, "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{60})};
  // The far-past not-before means the token is already valid
  EXPECT_FALSE(error.has_value());
}

TEST(issued_at_near_clock_minimum_does_not_underflow) {
  const auto minimum{std::chrono::duration_cast<std::chrono::duration<double>>(
                         std::chrono::system_clock::duration::min())
                         .count()};
  const auto payload{
      std::string{R"({ "iss": "acme", "aud": "client", "exp": 2000, "iat": )"} +
      std::to_string(minimum + 2.0) + " }"};
  const auto input{make_token(R"({ "alg": "RS256" })", payload, "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{60})};
  // The far-past issued-at is not in the future, so the token is valid
  EXPECT_FALSE(error.has_value());
}

TEST(now_at_clock_maximum_with_skew_does_not_overflow) {
  // A caller passing now at the clock's maximum, plus a skew, would overflow
  // the shifted server clock without the saturating guard
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::time_point::max(), std::chrono::seconds{60})};
  // now is far past the expiry, so the token is expired
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(negative_clock_skew_is_clamped_to_zero) {
  // A negative skew is clamped to zero rather than extending validity into the
  // past, so a not-yet-expired token stays valid
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500),
      std::chrono::seconds{-1000})};
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

TEST(expiration_skew_applies_to_expiration_only) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto accepted{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(2030),
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{60},
                                     std::chrono::seconds{0},
                                     std::chrono::seconds{0}})};
  EXPECT_FALSE(accepted.has_value());
  const auto rejected{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(2030),
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{0},
                                     std::chrono::seconds{60},
                                     std::chrono::seconds{60}})};
  EXPECT_TRUE(rejected.has_value());
  EXPECT_EQ(rejected.value(), sourcemeta::core::JWTClaimError::Expiration);
}

TEST(not_before_skew_applies_to_not_before_only) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 4000, "nbf": 2100 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto accepted{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(2050),
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{0},
                                     std::chrono::seconds{60},
                                     std::chrono::seconds{0}})};
  EXPECT_FALSE(accepted.has_value());
  const auto rejected{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(2050),
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{60},
                                     std::chrono::seconds{0},
                                     std::chrono::seconds{60}})};
  EXPECT_TRUE(rejected.has_value());
  EXPECT_EQ(rejected.value(), sourcemeta::core::JWTClaimError::NotBefore);
}

TEST(issued_at_skew_applies_to_issued_at_only) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 4000, "iat": 2100 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto accepted{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(2050),
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{0},
                                     std::chrono::seconds{0},
                                     std::chrono::seconds{60}})};
  EXPECT_FALSE(accepted.has_value());
  const auto rejected{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(2050),
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{60},
                                     std::chrono::seconds{60},
                                     std::chrono::seconds{0}})};
  EXPECT_TRUE(rejected.has_value());
  EXPECT_EQ(rejected.value(), sourcemeta::core::JWTClaimError::IssuedAt);
}

TEST(per_claim_skew_isolated_with_every_claim_present) {
  // The token is expired by 30 seconds while its not-before and issued-at
  // times lie 20 and 30 seconds in the future, so acceptance requires every
  // claim to be forgiven by exactly its own tolerance
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "iss": "acme", "aud": "client",
                                   "exp": 2000, "nbf": 2050, "iat": 2060 })",
                              "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto now{std::chrono::system_clock::from_time_t(2030)};
  const auto accepted{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client", now,
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{60},
                                     std::chrono::seconds{30},
                                     std::chrono::seconds{40}})};
  EXPECT_FALSE(accepted.has_value());
  const auto expired{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client", now,
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{0},
                                     std::chrono::seconds{30},
                                     std::chrono::seconds{40}})};
  EXPECT_TRUE(expired.has_value());
  EXPECT_EQ(expired.value(), sourcemeta::core::JWTClaimError::Expiration);
  const auto premature{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client", now,
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{60},
                                     std::chrono::seconds{0},
                                     std::chrono::seconds{40}})};
  EXPECT_TRUE(premature.has_value());
  EXPECT_EQ(premature.value(), sourcemeta::core::JWTClaimError::NotBefore);
  const auto future_issued{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client", now,
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{60},
                                     std::chrono::seconds{30},
                                     std::chrono::seconds{0}})};
  EXPECT_TRUE(future_issued.has_value());
  EXPECT_EQ(future_issued.value(), sourcemeta::core::JWTClaimError::IssuedAt);
}

TEST(uniform_seconds_skew_still_converts) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "exp": 2000, "nbf": 1030 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), std::chrono::seconds{60})};
  EXPECT_FALSE(error.has_value());
}

// The lifetime a token claims is the interval between the instant it says it
// was issued and the instant it says it stops being honoured, so bounding it
// refuses a value claiming to outlive anything the caller ever mints
TEST(lifetime_within_the_bound) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 1000, "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500), {}, std::nullopt,
      std::chrono::seconds{1000})};
  EXPECT_FALSE(error.has_value());
}

TEST(lifetime_past_the_bound) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 1000, "exp": 2001 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500), {}, std::nullopt,
      std::chrono::seconds{1000})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Lifetime);
}

// A bound nothing carries the issuance instant cannot be applied at all, so
// leaving the claim out would otherwise lift the bound for whoever left it out
TEST(lifetime_bound_requires_an_issuance_time) {
  const auto input{
      make_token(R"({ "alg": "RS256" })",
                 R"({ "iss": "acme", "aud": "client", "exp": 2000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500), {}, std::nullopt,
      std::chrono::seconds{1000})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Lifetime);
}

// A token expiring before it was issued names no interval at all
TEST(lifetime_expiring_before_issuance) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 3000, "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500), {}, std::nullopt,
      std::chrono::seconds{1000})};
  EXPECT_TRUE(error.has_value());
}

// Without a bound the claim relationship is not examined, so nothing an
// existing caller passes changes meaning
TEST(lifetime_unbounded_by_default) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 1000, "exp": 2000000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500))};
  EXPECT_FALSE(error.has_value());
}

// The interval is a relationship between two claims, so the tolerance for a
// disagreeing server clock has no bearing on it
TEST(lifetime_is_unaffected_by_clock_skew) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 1000, "exp": 2001 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500), std::chrono::seconds{3600},
      std::nullopt, std::chrono::seconds{1000})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Lifetime);
}

// Bounding the interval is only worth anything alongside refusing a token
// issued in the future, since a lifetime measured from an instant that has not
// arrived would otherwise start whenever its holder chose
TEST(lifetime_bound_still_refuses_a_future_issuance) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 9000, "exp": 9500 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500), {}, std::nullopt,
      std::chrono::seconds{1000})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::IssuedAt);
}

// Both instants are attacker controlled and each is only held within the
// clock's representable window, so the span between the two extremes does not
// fit the tick count that would carry it. Subtracting them would wrap and read
// as within any bound, letting a token claim centuries and evade the check
TEST(lifetime_across_the_clock_range_does_not_overflow) {
  const auto maximum{std::chrono::duration_cast<std::chrono::duration<double>>(
                         std::chrono::system_clock::duration::max())
                         .count()};
  const auto minimum{std::chrono::duration_cast<std::chrono::duration<double>>(
                         std::chrono::system_clock::duration::min())
                         .count()};
  const auto payload{
      std::string{R"({ "iss": "acme", "aud": "client", "iat": )"} +
      std::to_string(minimum + 2.0) + R"(, "exp": )" +
      std::to_string(maximum - 2.0) + " }"};
  const auto input{make_token(R"({ "alg": "RS256" })", payload, "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), {}, std::nullopt,
      std::chrono::seconds{3600})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Lifetime);
}

// A bound that is not positive admits no interval, so it refuses rather than
// silently lifting the check
TEST(lifetime_bound_that_is_negative_admits_nothing) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 1000, "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500), {}, std::nullopt,
      std::chrono::seconds{-1})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Lifetime);
}

// A bound of zero says a token may claim no life, which the token expiring at
// the very instant it was issued satisfies exactly
TEST(lifetime_bound_of_zero_admits_a_zero_length_interval) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 2000, "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500),
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{0},
                                     std::chrono::seconds{0},
                                     std::chrono::seconds{600}},
      std::nullopt, std::chrono::seconds{0})};
  EXPECT_FALSE(error.has_value());
}

// A negative bound admits no interval at all, the empty one included
TEST(lifetime_bound_that_is_negative_refuses_a_zero_length_interval) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 2000, "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500),
      sourcemeta::core::JWTClockSkew{std::chrono::seconds{0},
                                     std::chrono::seconds{0},
                                     std::chrono::seconds{600}},
      std::nullopt, std::chrono::seconds{-1})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Lifetime);
}

// A bound wider than any interval two representable instants can span admits
// every one of them, where saturating the shift would refuse the widest
TEST(lifetime_bound_wider_than_the_clock_admits_every_interval) {
  const auto maximum{std::chrono::duration_cast<std::chrono::duration<double>>(
                         std::chrono::system_clock::duration::max())
                         .count()};
  const auto minimum{std::chrono::duration_cast<std::chrono::duration<double>>(
                         std::chrono::system_clock::duration::min())
                         .count()};
  const auto payload{
      std::string{R"({ "iss": "acme", "aud": "client", "iat": )"} +
      std::to_string(minimum + 2.0) + R"(, "exp": )" +
      std::to_string(maximum - 2.0) + " }"};
  const auto input{make_token(R"({ "alg": "RS256" })", payload, "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1000), {}, std::nullopt,
      std::chrono::seconds::max())};
  EXPECT_FALSE(error.has_value());
}

TEST(lifetime_bound_of_zero_admits_nothing_longer) {
  const auto input{make_token(
      R"({ "alg": "RS256" })",
      R"({ "iss": "acme", "aud": "client", "iat": 1000, "exp": 2000 })",
      "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto error{sourcemeta::core::jwt_check_claims(
      token.value(), "acme", "client",
      std::chrono::system_clock::from_time_t(1500), {}, std::nullopt,
      std::chrono::seconds{0})};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTClaimError::Lifetime);
}
