#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

TEST(digit_zero) { EXPECT_EQ(sourcemeta::core::hex_digit_value('0'), 0); }

TEST(digit_nine) { EXPECT_EQ(sourcemeta::core::hex_digit_value('9'), 9); }

TEST(lowercase_a) { EXPECT_EQ(sourcemeta::core::hex_digit_value('a'), 10); }

TEST(lowercase_f) { EXPECT_EQ(sourcemeta::core::hex_digit_value('f'), 15); }

TEST(uppercase_a) { EXPECT_EQ(sourcemeta::core::hex_digit_value('A'), 10); }

TEST(uppercase_f) { EXPECT_EQ(sourcemeta::core::hex_digit_value('F'), 15); }

TEST(lowercase_g) { EXPECT_EQ(sourcemeta::core::hex_digit_value('g'), -1); }

TEST(uppercase_g) { EXPECT_EQ(sourcemeta::core::hex_digit_value('G'), -1); }

TEST(space) { EXPECT_EQ(sourcemeta::core::hex_digit_value(' '), -1); }

TEST(slash) { EXPECT_EQ(sourcemeta::core::hex_digit_value('/'), -1); }

TEST(colon) { EXPECT_EQ(sourcemeta::core::hex_digit_value(':'), -1); }

TEST(null) { EXPECT_EQ(sourcemeta::core::hex_digit_value('\0'), -1); }

TEST(high_bit_byte) {
  EXPECT_EQ(sourcemeta::core::hex_digit_value(static_cast<char>(0xFF)), -1);
}
