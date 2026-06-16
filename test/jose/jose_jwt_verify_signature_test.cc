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
constexpr std::string_view ES512_TOKEN_CURVE_GUARD{
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
constexpr std::string_view PS256_TOKEN{
    "eyJhbGciOiJQUzI1NiJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "4Q3zQVGI1aoFXI0Wn5lhsWQFc0l7aS58JHqf5FpvfDQsjoQtXgtf0lXAsI8fi3DpJdONIn7ZIi"
    "WOjzEjzBGgFQonfQAn1oPiTvChNdTZa7gcVsireZAjGboL3AzZM-"
    "GUvKaiBfQt2YrIEsa4YhGzW7ibH3chMuShOUcG1Wi5OhP4DWqi4flm64MsZcgl9OA05_"
    "UHMvLRlX8n9UoMED6OotoVdv_qqVstJgsJlGcdk7lMOuarni2rHKc6IK8F_Fr2IC_7AX_"
    "XQE7ok-S7M_"
    "NRLkbf90DRtu5FkisOOIhIhDurc4yp7QTWKkq8Jhl2hibWgYCGGKqgBFt0QlL2UAp02Q"};
constexpr std::string_view PS256_JWK{
    R"JSON({ "kty": "RSA", "n": "65hX-bjlZcGs75IS0wJvI_JNB6jwKyK7vPgm9M1XCiPqfril2wDPGgLgIiBEyDdvnwn0e-T3Sb4pWv0O2ffZqYyAIPGgPABJ4DOOEj7UY9_6kFGCuKyRjtDRwi9_E6iC5qtHUQ6Z5zytNXym1qZLUNHvyS-o9cqS0yxHW9MY7Lj5vwZZ-SStqsJicH5B4Ef5CzwBfZjgUmSB9TjiLMYid1BtUWRDbKho8eGIz_LRnf5uX-fFHDMUN-Fk5BHvgbzNg3RQBhioElX228Om1emdisOvzABBklpD6P78Cyj6h_8EdbHjKN4_CUPPtE-7adVzsu-LtrAJKQhljRDa5LplTQ", "e": "AQAB" })JSON"};
constexpr std::string_view ES384_TOKEN{
    "eyJhbGciOiJFUzM4NCJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "SqRA6wgW8tCqSRJOkC6z7XaPnNT_"
    "4zvOHINFe5L40HfIk0kqAwWwARYKQ7r9igq5Uo3Cdo9s51eOYwvVDbdZTlmMH7wohcXwUtPtUL"
    "exjgpkVV0AMKkaGtWKBqLNp7Ur"};
constexpr std::string_view ES384_JWK{
    R"JSON({ "kty": "EC", "crv": "P-384", "x": "y4xoP3OyDy-Ezxyjb-tXb2wvAhRyzDmdZ96gzwNhxmVwTdb6IHkCXCkA4r6_A1RE", "y": "CFv8am3V9RP2s4Bd4yd-XHcyKJ-BGMxVxnVBiR2aWvlGWsYZ2YtnGNule0G2X5TN" })JSON"};
constexpr std::string_view ES512_TOKEN{
    "eyJhbGciOiJFUzUxMiJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "ALruSKdoKMV4CGm3rA18G2Jqc7fG0EhVVUVEBE6LXMJCDPDDFYUPxbddk_"
    "bMROd2FaVHX5MXL9UWPV-2ltnFpZ-qARxtA_gVaLAaLhYI2u-Sy74mR_-"
    "cCUUxtYG8qH09toBV2xQiMKPRLYp4O2RvpXZKYItp0dE-zaduwY6o_Z9wNoNJ"};
constexpr std::string_view ES512_JWK{
    R"JSON({ "kty": "EC", "crv": "P-521", "x": "ASTBdxhoeBK4-OqrpidcXbsz_Gsd-TRgqkZGD9ROv7r_kBHW1_is2HDctAOVBWC3ywSUS5MZJIl17xJuYCyZA-Rg", "y": "AOQCwwmrhHkVTm9BgloMWDc7T3hw5yQM5Z-YhVumpCYyFIGuyrziD5iqsH8uQvsNXGqnMKhOJ26X8ADhvP20RIAW" })JSON"};
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
  const auto token{sourcemeta::core::JWT::from(ES512_TOKEN_CURVE_GUARD)};
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

// The PS256, ES384, and ES512 vectors are self-signed with OpenSSL over a JSON
// object payload, exercising the RSA-PSS primitive and the SHA-384 / SHA-512
// and P-384 / P-521 mappings. No RFC worked example covers these as a JWT: RFC
// 7515 only provides RS256 and ES256, and the RFC 7520 cookbook signs a
// non-JSON payload that is not a valid JWT
TEST(JOSE_jwt_verify_signature, ps256_valid) {
  const auto token{sourcemeta::core::JWT::from(PS256_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(PS256_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_TRUE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, es384_valid) {
  const auto token{sourcemeta::core::JWT::from(ES384_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(ES384_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_TRUE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}

TEST(JOSE_jwt_verify_signature, es512_valid) {
  const auto token{sourcemeta::core::JWT::from(ES512_TOKEN)};
  ASSERT_TRUE(token.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(ES512_JWK))};
  ASSERT_TRUE(key.has_value());
  EXPECT_TRUE(
      sourcemeta::core::jwt_verify_signature(token.value(), key.value()));
}
