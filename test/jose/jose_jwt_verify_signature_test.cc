#include <gtest/gtest.h>

#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// The positive vectors are the worked examples from RFC 7515 Appendix A.2
// (RS256) and A.3 (ES256). The guard scenarios are adapted from jwt-cpp
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
constexpr std::string_view ES256_TOKEN{
    "eyJhbGciOiJFUzI1NiJ9."
    "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS"
    "9pc19yb290Ijp0cnVlfQ.DtEhU3ljbEg8L38VWAfUAqOyKAM6-Xx-"
    "F4GawxaepmXFCgfTjDxw5djxLa8ISlSApmWQxfKTUJqPP3-Kg6NU1Q"};
constexpr std::string_view ES512_TOKEN{
    "eyJhbGciOiJFUzUxMiJ9."
    "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS"
    "9pc19yb290Ijp0cnVlfQ.DtEhU3ljbEg8L38VWAfUAqOyKAM6-Xx-"
    "F4GawxaepmXFCgfTjDxw5djxLa8ISlSApmWQxfKTUJqPP3-Kg6NU1Q"};
constexpr std::string_view HS256_TOKEN{
    "eyJhbGciOiJIUzI1NiJ9."
    "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS"
    "9pc19yb290Ijp0cnVlfQ.c2ln"};
constexpr std::string_view RSA_JWK{
    R"JSON({ "kty": "RSA", "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ", "e": "AQAB" })JSON"};
constexpr std::string_view RSA_JWK_OTHER_ALGORITHM{
    R"JSON({ "kty": "RSA", "n": "AQAB", "e": "AQAB", "alg": "RS384" })JSON"};
constexpr std::string_view EC_JWK{
    R"JSON({ "kty": "EC", "crv": "P-256", "x": "f83OJ3D2xF1Bg8vub9tLe1gHMzV76e8Tus9uPHvRVEU", "y": "x_FEzRu9m36HLN_tue659LNpXW6pCyStikYjKIWI5a0" })JSON"};
} // namespace

TEST(JOSE_jwt_verify_signature, rs256_valid) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_TRUE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, rs256_tampered_signature) {
  std::string tampered{RS256_TOKEN};
  const auto signature_start{tampered.rfind('.') + 1};
  tampered[signature_start] = (tampered[signature_start] == 'A') ? 'B' : 'A';
  const auto token{sourcemeta::core::JWT::from(tampered)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, es256_valid) {
  const auto token{sourcemeta::core::JWT::from(ES256_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_TRUE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, es256_tampered_signature) {
  std::string tampered{ES256_TOKEN};
  const auto signature_start{tampered.rfind('.') + 1};
  tampered[signature_start] = (tampered[signature_start] == 'A') ? 'B' : 'A';
  const auto token{sourcemeta::core::JWT::from(tampered)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, rsa_token_with_elliptic_curve_key) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, elliptic_curve_token_with_rsa_key) {
  const auto token{sourcemeta::core::JWT::from(ES256_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, elliptic_curve_algorithm_curve_mismatch) {
  const auto token{sourcemeta::core::JWT::from(ES512_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, unrecognized_algorithm) {
  const auto token{sourcemeta::core::JWT::from(HS256_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, contradicting_key_algorithm) {
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(RSA_JWK_OTHER_ALGORITHM))};
  ASSERT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}
