#include <sourcemeta/core/test.h>

#include <sourcemeta/core/unicode.h>

TEST(null) { EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x0000)); }

TEST(ascii_letter) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x0041));
}

TEST(ascii_high_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x007F));
}

TEST(latin_extended) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x00E9));
}

TEST(just_below_surrogate_range) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0xD7FF));
}

TEST(low_surrogate_low_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0xD800));
}

TEST(low_surrogate_high_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0xDBFF));
}

TEST(high_surrogate_low_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0xDC00));
}

TEST(high_surrogate_high_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0xDFFF));
}

TEST(just_above_surrogate_range) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0xE000));
}

TEST(max_bmp) { EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0xFFFF)); }

TEST(smp_low_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x10000));
}

TEST(emoji_grinning_face) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x1F600));
}

TEST(max_codepoint) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x10FFFF));
}

TEST(just_above_max_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0x110000));
}

TEST(far_above_max_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0x1FFFFF));
}
