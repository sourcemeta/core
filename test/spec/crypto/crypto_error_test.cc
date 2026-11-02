#include <sourcemeta/core/crypto_error.h>
#include <sourcemeta/core/test.h>

#include <exception>   // std::exception
#include <type_traits> // std::is_base_of_v

static_assert(
    std::is_base_of_v<sourcemeta::core::Error, sourcemeta::core::CryptoError>);
static_assert(std::is_base_of_v<std::exception, sourcemeta::core::CryptoError>);

TEST(message) {
  const sourcemeta::core::CryptoError error{"Could not compute SHA-256 digest"};
  EXPECT_STREQ(error.what(), "Could not compute SHA-256 digest");
}

TEST(catchable_as_the_base) {
  try {
    throw sourcemeta::core::CryptoError{"Could not generate random bytes"};
  } catch (const sourcemeta::core::Error &error) {
    EXPECT_STREQ(error.what(), "Could not generate random bytes");
  }
}
