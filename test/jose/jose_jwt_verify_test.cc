#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::system_clock
#include <optional>    // std::nullopt
#include <string_view> // std::string_view

// The signed token is the RS256 worked example from RFC 7515 Appendix A.2. Its
// payload is expired and carries no audience, so the success path can only be
// exercised up to the claim checks. The failure scenarios follow jwt-cpp
// (https://github.com/Thalhammer/jwt-cpp)

namespace {
constexpr std::string_view RS256_TOKEN{
    "eyJhbGciOiJSUzI1NiJ9."
    "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS"
    "9pc19yb290Ijp0cnVlfQ.cC4hiUPoj9Eetdgtv3hF80EGrhuB__"
    "dzERat0XF9g2VtQgr9PJbu3XOiZj5RZmh7AAuHIm4Bh-0Qc_lF5YKt_"
    "O8W2Fp5jujGbds9uJdbF9CUAr7t1dnZcAcQjbKBYNX4BAynRFdiuB--f_"
    "nZLgrnbyTyWzO75vRK5h6xBArLIARNPvkSjtQBMHlb1L07Qe7K0GarZRmB_eSN9383LcOLn6_"
    "dO--xi12jzDwusC-"
    "eOkHWEsqtFZESc6BfI7noOPqvhJ1phCnvWh6IeYI2w9QOYEUipUTI8np6LbgGY9Fs98rqVt5AX"
    "LIhWkWywlVmtVrBp0igcN_IoypGlUPQGe77Rw"};
constexpr std::string_view RS256_TOKEN_WITH_KEY_ID{
    "eyJhbGciOiJSUzI1NiIsImtpZCI6ImsxIn0."
    "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS"
    "9pc19yb290Ijp0cnVlfQ.cC4hiUPoj9Eetdgtv3hF80EGrhuB__"
    "dzERat0XF9g2VtQgr9PJbu3XOiZj5RZmh7AAuHIm4Bh-0Qc_lF5YKt_"
    "O8W2Fp5jujGbds9uJdbF9CUAr7t1dnZcAcQjbKBYNX4BAynRFdiuB--f_"
    "nZLgrnbyTyWzO75vRK5h6xBArLIARNPvkSjtQBMHlb1L07Qe7K0GarZRmB_eSN9383LcOLn6_"
    "dO--xi12jzDwusC-"
    "eOkHWEsqtFZESc6BfI7noOPqvhJ1phCnvWh6IeYI2w9QOYEUipUTI8np6LbgGY9Fs98rqVt5AX"
    "LIhWkWywlVmtVrBp0igcN_IoypGlUPQGe77Rw"};
constexpr std::string_view RSA_KEYS{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ", "e": "AQAB" } ] })JSON"};
constexpr std::string_view RSA_KEYS_KEY_ID_K1{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ", "e": "AQAB", "kid": "k1" } ] })JSON"};
constexpr std::string_view RSA_KEYS_KEY_ID_OTHER{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ", "e": "AQAB", "kid": "other" } ] })JSON"};
constexpr std::string_view ELLIPTIC_CURVE_KEYS{
    R"JSON({ "keys": [ { "kty": "EC", "crv": "P-256", "x": "f83OJ3D2xF1Bg8vub9tLe1gHMzV76e8Tus9uPHvRVEU", "y": "x_FEzRu9m36HLN_tue659LNpXW6pCyStikYjKIWI5a0" } ] })JSON"};
constexpr std::string_view SIGNED_TOKEN{
    "eyJhbGciOiJSUzI1NiIsInR5cCI6ImF0K2p3dCJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "U3ZBo7MvSW0U099gJ_"
    "vIA5T8HJ2XnKSzYmqkx7SDxgxQfmxQyu3QZIeKT68AAH7wQjWRvNWQ7f3Es57UUNUQAMs-"
    "z5TWlVBKtYZf5ZcbYqc4KrQ-ApwpjoFGJxurnd1R_"
    "tz02WssnvrZNKnxNPuGoYIkJKNCl59yLFJwRLf3nK_Jcxs-"
    "1m2MvKsm647PuXqhYOKlZkHOvkIV0RV8cLJ56_gDVjj7TlKQgwbTdW_"
    "71QLwLWRFGftU2EAWuqayTSpPeUA6kB4sfn7JNsweqDs7uev30m6y8BE9uzwzHuuovaN1cZz0o"
    "TAGXcx64sfbPs6HEMp5_FoU0SccxArAbnHSjA"};
constexpr std::string_view SIGNED_KEYS{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "oHTpl-jfNfBuXmBp58sW8s_77UP6j2jA0mjjKjhDkxhp7Agk-xLNGgfPCS_bjdZ6YU6FGeab8uVjkSgo9_0OCJUaF4vzEGwXmNuGawANxnZtiYjWvbJlq-2mn_L7rsqGQcSkMmyM0g4aX7dF8wB6DVrXShJ78fcrNtpeoU72YGEdjehA8qVclDFwBdpCGynxxnWJePk72lQb6gkVMqKMc3jBF8GkWf8oP_sjss-fpOjSUMR1c8_0JlTYWO46KWOZa0EO2t8H1V3imMyzbhoxRd_qZHmo46gJkG-ZdebjX0vGQllaCwu0z4kLcXIfAZhqPEkdssDGhC_txwJuhaPDFQ", "e": "AQAB" } ] })JSON"};
} // namespace

TEST(algorithm_not_allowed) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(RSA_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "joe", "client",
      std::chrono::system_clock::from_time_t(1300000000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(),
            sourcemeta::core::JWTVerificationError::AlgorithmNotAllowed);
}

TEST(unknown_key_by_identifier) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN_WITH_KEY_ID)};
  EXPECT_TRUE(token.has_value());
  const auto keys{sourcemeta::core::JWKS::from(
      sourcemeta::core::parse_json(RSA_KEYS_KEY_ID_OTHER))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "joe", "client",
      std::chrono::system_clock::from_time_t(1300000000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
}

TEST(unknown_key_no_compatible_key) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{sourcemeta::core::JWKS::from(
      sourcemeta::core::parse_json(ELLIPTIC_CURVE_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "joe", "client",
      std::chrono::system_clock::from_time_t(1300000000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
}

TEST(signature_failure) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN_WITH_KEY_ID)};
  EXPECT_TRUE(token.has_value());
  const auto keys{sourcemeta::core::JWKS::from(
      sourcemeta::core::parse_json(RSA_KEYS_KEY_ID_K1))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "joe", "client",
      std::chrono::system_clock::from_time_t(1300000000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Signature);
}

TEST(valid_signature_reaches_audience_check) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(RSA_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "joe", "client",
      std::chrono::system_clock::from_time_t(1300000000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Audience);
}

TEST(issuer_mismatch_after_valid_signature) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(RSA_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "wrong", "client",
      std::chrono::system_clock::from_time_t(1300000000))};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Issuer);
}

TEST(type_required_but_absent) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(RSA_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "joe", "client",
      std::chrono::system_clock::from_time_t(1300000000),
      std::chrono::seconds{0}, std::nullopt, "at+jwt")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Type);
}

// The signed token below is generated locally with OpenSSL (RS256, header
// typ "at+jwt", unexpired issuer and audience claims), since no RFC worked
// example pairs a valid signature with verifiable claims and an explicit type
TEST(valid) {
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(SIGNED_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "acme", "client",
      std::chrono::system_clock::from_time_t(1000000000))};
  EXPECT_FALSE(error.has_value());
}

TEST(type_compact_form_matches) {
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(SIGNED_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "acme", "client",
      std::chrono::system_clock::from_time_t(1000000000),
      std::chrono::seconds{0}, std::nullopt, "at+jwt")};
  EXPECT_FALSE(error.has_value());
}

TEST(type_application_prefix_matches) {
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(SIGNED_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "acme", "client",
      std::chrono::system_clock::from_time_t(1000000000),
      std::chrono::seconds{0}, std::nullopt, "application/at+jwt")};
  EXPECT_FALSE(error.has_value());
}

TEST(type_mismatch) {
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  EXPECT_TRUE(token.has_value());
  const auto keys{
      sourcemeta::core::JWKS::from(sourcemeta::core::parse_json(SIGNED_KEYS))};
  EXPECT_TRUE(keys.has_value());
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::RS256}};
  const auto error{sourcemeta::core::jwt_verify(
      token.value(), keys.value(), allowed, "acme", "client",
      std::chrono::system_clock::from_time_t(1000000000),
      std::chrono::seconds{0}, std::nullopt, "application/example")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Type);
}
