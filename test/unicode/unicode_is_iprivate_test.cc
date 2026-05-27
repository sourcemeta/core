#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_is_iprivate, ascii_letter_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(U'A'));
}

TEST(Unicode_is_iprivate, before_bmp_private_use_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xDFFF));
}

// BMP private use area: U+E000..U+F8FF
TEST(Unicode_is_iprivate, bmp_lower_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0xE000));
}

TEST(Unicode_is_iprivate, bmp_middle) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0xE800));
}

TEST(Unicode_is_iprivate, bmp_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0xF8FF));
}

TEST(Unicode_is_iprivate, just_above_bmp_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xF900));
}

// Plane 15 supplementary private use: U+F0000..U+FFFFD
TEST(Unicode_is_iprivate, plane15_lower_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0xF0000));
}

TEST(Unicode_is_iprivate, plane15_middle) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0xF8000));
}

TEST(Unicode_is_iprivate, plane15_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0xFFFFD));
}

TEST(Unicode_is_iprivate, plane15_fffe_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xFFFFE));
}

TEST(Unicode_is_iprivate, plane15_ffff_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xFFFFF));
}

// Plane 16 supplementary private use: U+100000..U+10FFFD
TEST(Unicode_is_iprivate, plane16_lower_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0x100000));
}

TEST(Unicode_is_iprivate, plane16_middle) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0x108000));
}

TEST(Unicode_is_iprivate, plane16_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0x10FFFD));
}

TEST(Unicode_is_iprivate, plane16_fffe_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0x10FFFE));
}

TEST(Unicode_is_iprivate, beyond_max_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0x110000));
}

// Plane 14 (ucschar) is not iprivate
TEST(Unicode_is_iprivate, plane14_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xE1000));
}

// BMP non-ASCII letters are not iprivate
TEST(Unicode_is_iprivate, latin_extended_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0x00C0));
}

TEST(Unicode_is_iprivate, han_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0x4E2D));
}
