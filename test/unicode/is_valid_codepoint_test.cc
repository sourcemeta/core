#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_is_valid_codepoint, null) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x0000));
}

TEST(Unicode_is_valid_codepoint, ascii_letter) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x0041));
}

TEST(Unicode_is_valid_codepoint, ascii_high_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x007F));
}

TEST(Unicode_is_valid_codepoint, latin_extended) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x00E9));
}

TEST(Unicode_is_valid_codepoint, just_below_surrogate_range) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0xD7FF));
}

TEST(Unicode_is_valid_codepoint, low_surrogate_low_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0xD800));
}

TEST(Unicode_is_valid_codepoint, low_surrogate_high_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0xDBFF));
}

TEST(Unicode_is_valid_codepoint, high_surrogate_low_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0xDC00));
}

TEST(Unicode_is_valid_codepoint, high_surrogate_high_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0xDFFF));
}

TEST(Unicode_is_valid_codepoint, just_above_surrogate_range) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0xE000));
}

TEST(Unicode_is_valid_codepoint, max_bmp) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0xFFFF));
}

TEST(Unicode_is_valid_codepoint, smp_low_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x10000));
}

TEST(Unicode_is_valid_codepoint, emoji_grinning_face) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x1F600));
}

TEST(Unicode_is_valid_codepoint, max_codepoint) {
  EXPECT_TRUE(sourcemeta::core::is_valid_codepoint(0x10FFFF));
}

TEST(Unicode_is_valid_codepoint, just_above_max_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0x110000));
}

TEST(Unicode_is_valid_codepoint, far_above_max_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_valid_codepoint(0x1FFFFF));
}
