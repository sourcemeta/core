#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_is_surrogate, null) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0x0000));
}

TEST(Unicode_is_surrogate, ascii_letter) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0x0041));
}

TEST(Unicode_is_surrogate, just_below_low_surrogate) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0xD7FF));
}

TEST(Unicode_is_surrogate, low_surrogate_low_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xD800));
}

TEST(Unicode_is_surrogate, low_surrogate_mid) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDA00));
}

TEST(Unicode_is_surrogate, low_surrogate_high_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDBFF));
}

TEST(Unicode_is_surrogate, high_surrogate_low_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDC00));
}

TEST(Unicode_is_surrogate, high_surrogate_mid) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDE00));
}

TEST(Unicode_is_surrogate, high_surrogate_high_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_surrogate(0xDFFF));
}

TEST(Unicode_is_surrogate, just_above_high_surrogate) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0xE000));
}

TEST(Unicode_is_surrogate, max_bmp) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0xFFFF));
}

TEST(Unicode_is_surrogate, emoji_grinning_face) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0x1F600));
}

TEST(Unicode_is_surrogate, max_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_surrogate(0x10FFFF));
}
