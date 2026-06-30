#include <sourcemeta/core/test.h>

#include <sourcemeta/core/unicode.h>

TEST(ascii_null) { EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0x00), 1u); }

TEST(ascii_letter) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0x41), 1u);
}

TEST(ascii_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0x7F), 1u);
}

TEST(continuation_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0x80), 0u);
}

TEST(continuation_mid) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xA0), 0u);
}

TEST(continuation_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xBF), 0u);
}

TEST(overlong_c0) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xC0), 0u);
}

TEST(overlong_c1) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xC1), 0u);
}

TEST(two_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xC2), 2u);
}

TEST(two_byte_mid) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xCE), 2u);
}

TEST(two_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xDF), 2u);
}

TEST(three_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xE0), 3u);
}

TEST(three_byte_mid_e4) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xE4), 3u);
}

TEST(three_byte_ed) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xED), 3u);
}

TEST(three_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xEF), 3u);
}

TEST(four_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xF0), 4u);
}

TEST(four_byte_mid_f2) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xF2), 4u);
}

TEST(four_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xF4), 4u);
}

TEST(above_range_f5) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xF5), 0u);
}

TEST(above_range_fe) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xFE), 0u);
}

TEST(above_range_ff) {
  EXPECT_EQ(sourcemeta::core::utf8_lead_byte_size(0xFF), 0u);
}
