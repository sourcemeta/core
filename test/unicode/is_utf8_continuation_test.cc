#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_is_utf8_continuation, ascii_null_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0x00));
}

TEST(Unicode_is_utf8_continuation, ascii_letter_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0x41));
}

TEST(Unicode_is_utf8_continuation, ascii_high_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0x7F));
}

TEST(Unicode_is_utf8_continuation, low_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0x80));
}

TEST(Unicode_is_utf8_continuation, just_above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0x81));
}

TEST(Unicode_is_utf8_continuation, mid_range_a0) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0xA0));
}

TEST(Unicode_is_utf8_continuation, mid_range_b0) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0xB0));
}

TEST(Unicode_is_utf8_continuation, high_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0xBF));
}

TEST(Unicode_is_utf8_continuation, two_byte_lead_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0xC0));
}

TEST(Unicode_is_utf8_continuation, three_byte_lead_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0xE0));
}

TEST(Unicode_is_utf8_continuation, four_byte_lead_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0xF0));
}

TEST(Unicode_is_utf8_continuation, max_byte_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0xFF));
}
