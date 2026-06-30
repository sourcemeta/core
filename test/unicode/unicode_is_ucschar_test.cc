#include <sourcemeta/core/test.h>

#include <sourcemeta/core/unicode.h>

// ASCII range is excluded from ucschar (those are in the URI ASCII set)
TEST(ascii_letter_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(U'A'));
}

TEST(ascii_digit_excluded) { EXPECT_FALSE(sourcemeta::core::is_ucschar(U'0')); }

TEST(null_excluded) { EXPECT_FALSE(sourcemeta::core::is_ucschar(0x0000)); }

TEST(c0_control_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0x001F));
}

TEST(latin1_below_a0_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0x009F));
}

// BMP non-ASCII letters are in ucschar
TEST(nbsp_lower_bound) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0x00A0)); }

TEST(latin_extended) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0x00C0)); }

TEST(greek_alpha) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0x03B1)); }

TEST(hebrew_alef) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0x05D0)); }

TEST(han_zhong) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0x4E2D)); }

TEST(before_surrogates) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0xD7FF)); }

// Surrogates are excluded
TEST(surrogate_high_excluded) {
  // U+D800 is a high (leading) surrogate
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xD800));
}

TEST(surrogate_low_excluded) {
  // U+DFFF is a low (trailing) surrogate
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xDFFF));
}

// BMP private use area is iprivate, not ucschar
TEST(bmp_private_use_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xE000));
}

TEST(bmp_private_use_top_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xF8FF));
}

// Compatibility ideographs gap
TEST(before_compatibility_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xF8FF));
}

TEST(compatibility_lower_bound) {
  EXPECT_TRUE(sourcemeta::core::is_ucschar(0xF900));
}

TEST(compatibility_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_ucschar(0xFDCF));
}

// Arabic presentation forms-A noncharacters gap
TEST(fdd0_noncharacter_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xFDD0));
}

TEST(fdef_noncharacter_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xFDEF));
}

TEST(after_fdef_gap) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0xFDF0)); }

TEST(ffef_upper_bound) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0xFFEF)); }

// End-of-plane-0 noncharacters
TEST(fff0_excluded) { EXPECT_FALSE(sourcemeta::core::is_ucschar(0xFFF0)); }

TEST(fffe_excluded) { EXPECT_FALSE(sourcemeta::core::is_ucschar(0xFFFE)); }

TEST(ffff_excluded) { EXPECT_FALSE(sourcemeta::core::is_ucschar(0xFFFF)); }

// Supplementary planes 1..13 each allow 0..FFFD
TEST(plane1_lower_bound) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0x10000)); }

TEST(plane1_upper_bound) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0x1FFFD)); }

TEST(plane1_fffe_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0x1FFFE));
}

TEST(plane1_ffff_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0x1FFFF));
}

TEST(plane2) { EXPECT_TRUE(sourcemeta::core::is_ucschar(0x20000)); }

TEST(plane13_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_ucschar(0xDFFFD));
}

// Plane 14 starts at offset 0x1000 (E1000), not E0000
TEST(plane14_e0000_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xE0000));
}

TEST(plane14_e0fff_excluded) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0xE0FFF));
}

TEST(plane14_lower_bound) {
  EXPECT_TRUE(sourcemeta::core::is_ucschar(0xE1000));
}

TEST(plane14_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_ucschar(0xEFFFD));
}

// Planes 15 and 16 are iprivate, not ucschar
TEST(plane15_excluded) { EXPECT_FALSE(sourcemeta::core::is_ucschar(0xF0000)); }

TEST(plane16_excluded) { EXPECT_FALSE(sourcemeta::core::is_ucschar(0x100000)); }

TEST(beyond_max_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_ucschar(0x110000));
}
