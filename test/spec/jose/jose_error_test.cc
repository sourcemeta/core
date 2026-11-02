#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

#include <type_traits> // std::is_nothrow_default_constructible_v

static_assert(
    std::is_nothrow_default_constructible_v<sourcemeta::core::JWKParseError>);
static_assert(
    std::is_nothrow_default_constructible_v<sourcemeta::core::JWKSParseError>);
static_assert(
    std::is_nothrow_default_constructible_v<sourcemeta::core::JWTParseError>);
static_assert(
    std::is_nothrow_default_constructible_v<sourcemeta::core::JWEParseError>);

TEST(jwk_parse_error_message) {
  const sourcemeta::core::JWKParseError error{};
  EXPECT_STREQ(error.what(), "The input is not a valid JSON Web Key");
}

TEST(jwks_parse_error_message) {
  const sourcemeta::core::JWKSParseError error{};
  EXPECT_STREQ(error.what(), "The input is not a valid JSON Web Key Set");
}

TEST(jwt_parse_error_message) {
  const sourcemeta::core::JWTParseError error{};
  EXPECT_STREQ(error.what(), "The input is not a valid JSON Web Token");
}

TEST(jwe_parse_error_message) {
  const sourcemeta::core::JWEParseError error{};
  EXPECT_STREQ(error.what(),
               "The input is not a valid JSON Web Encryption object");
}
