#include <sourcemeta/core/test.h>

#include <sourcemeta/core/unicode.h>

TEST(ascii_letter_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(U'A'));
}

TEST(before_bmp_private_use_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xDFFF));
}

// BMP private use area: U+E000..U+F8FF
TEST(bmp_lower_bound) { EXPECT_TRUE(sourcemeta::core::is_iprivate(0xE000)); }

TEST(bmp_middle) { EXPECT_TRUE(sourcemeta::core::is_iprivate(0xE800)); }

TEST(bmp_upper_bound) { EXPECT_TRUE(sourcemeta::core::is_iprivate(0xF8FF)); }

TEST(just_above_bmp_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xF900));
}

// Plane 15 supplementary private use: U+F0000..U+FFFFD
TEST(plane15_lower_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0xF0000));
}

TEST(plane15_middle) { EXPECT_TRUE(sourcemeta::core::is_iprivate(0xF8000)); }

TEST(plane15_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0xFFFFD));
}

TEST(plane15_fffe_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xFFFFE));
}

TEST(plane15_ffff_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0xFFFFF));
}

// Plane 16 supplementary private use: U+100000..U+10FFFD
TEST(plane16_lower_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0x100000));
}

TEST(plane16_middle) { EXPECT_TRUE(sourcemeta::core::is_iprivate(0x108000)); }

TEST(plane16_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_iprivate(0x10FFFD));
}

TEST(plane16_fffe_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0x10FFFE));
}

TEST(beyond_max_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0x110000));
}

// Plane 14 (ucschar) is not iprivate
TEST(plane14_excluded) { EXPECT_FALSE(sourcemeta::core::is_iprivate(0xE1000)); }

// BMP non-ASCII letters are not iprivate
TEST(latin_extended_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_iprivate(0x00C0));
}

TEST(han_excluded) { EXPECT_FALSE(sourcemeta::core::is_iprivate(0x4E2D)); }
