#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

TEST(digit_zero) { EXPECT_TRUE(sourcemeta::core::is_hex_digit('0')); }

TEST(digit_nine) { EXPECT_TRUE(sourcemeta::core::is_hex_digit('9')); }

TEST(lowercase_a) { EXPECT_TRUE(sourcemeta::core::is_hex_digit('a')); }

TEST(lowercase_f) { EXPECT_TRUE(sourcemeta::core::is_hex_digit('f')); }

TEST(uppercase_a) { EXPECT_TRUE(sourcemeta::core::is_hex_digit('A')); }

TEST(uppercase_f) { EXPECT_TRUE(sourcemeta::core::is_hex_digit('F')); }

TEST(lowercase_g) { EXPECT_FALSE(sourcemeta::core::is_hex_digit('g')); }

TEST(uppercase_g) { EXPECT_FALSE(sourcemeta::core::is_hex_digit('G')); }

TEST(uppercase_z) { EXPECT_FALSE(sourcemeta::core::is_hex_digit('Z')); }

TEST(space) { EXPECT_FALSE(sourcemeta::core::is_hex_digit(' ')); }

TEST(slash) { EXPECT_FALSE(sourcemeta::core::is_hex_digit('/')); }

TEST(colon) { EXPECT_FALSE(sourcemeta::core::is_hex_digit(':')); }

TEST(at_sign) { EXPECT_FALSE(sourcemeta::core::is_hex_digit('@')); }

TEST(backtick) { EXPECT_FALSE(sourcemeta::core::is_hex_digit('`')); }

TEST(null) { EXPECT_FALSE(sourcemeta::core::is_hex_digit('\0')); }

TEST(high_bit_byte) {
  EXPECT_FALSE(sourcemeta::core::is_hex_digit(static_cast<char>(0xFF)));
}
