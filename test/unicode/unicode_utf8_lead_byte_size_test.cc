#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_utf8_lead_byte_size, ascii_null) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0x00), 1u);
}

TEST(Unicode_utf8_lead_byte_size, ascii_letter) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0x41), 1u);
}

TEST(Unicode_utf8_lead_byte_size, ascii_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0x7F), 1u);
}

TEST(Unicode_utf8_lead_byte_size, continuation_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0x80), 0u);
}

TEST(Unicode_utf8_lead_byte_size, continuation_mid) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xA0), 0u);
}

TEST(Unicode_utf8_lead_byte_size, continuation_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xBF), 0u);
}

TEST(Unicode_utf8_lead_byte_size, overlong_c0) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xC0), 0u);
}

TEST(Unicode_utf8_lead_byte_size, overlong_c1) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xC1), 0u);
}

TEST(Unicode_utf8_lead_byte_size, two_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xC2), 2u);
}

TEST(Unicode_utf8_lead_byte_size, two_byte_mid) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xCE), 2u);
}

TEST(Unicode_utf8_lead_byte_size, two_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xDF), 2u);
}

TEST(Unicode_utf8_lead_byte_size, three_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xE0), 3u);
}

TEST(Unicode_utf8_lead_byte_size, three_byte_mid_e4) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xE4), 3u);
}

TEST(Unicode_utf8_lead_byte_size, three_byte_ed) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xED), 3u);
}

TEST(Unicode_utf8_lead_byte_size, three_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xEF), 3u);
}

TEST(Unicode_utf8_lead_byte_size, four_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xF0), 4u);
}

TEST(Unicode_utf8_lead_byte_size, four_byte_mid_f2) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xF2), 4u);
}

TEST(Unicode_utf8_lead_byte_size, four_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xF4), 4u);
}

TEST(Unicode_utf8_lead_byte_size, above_range_f5) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xF5), 0u);
}

TEST(Unicode_utf8_lead_byte_size, above_range_fe) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xFE), 0u);
}

TEST(Unicode_utf8_lead_byte_size, above_range_ff) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xFF), 0u);
}
