#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

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
