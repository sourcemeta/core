#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(ascii_null_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0x00));
}

TEST(ascii_letter_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0x41));
}

TEST(ascii_high_boundary_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0x7F));
}

TEST(low_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0x80));
}

TEST(just_above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0x81));
}

TEST(mid_range_a0) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0xA0));
}

TEST(mid_range_b0) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0xB0));
}

TEST(high_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_utf8_continuation(0xBF));
}

TEST(two_byte_lead_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0xC0));
}

TEST(three_byte_lead_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0xE0));
}

TEST(four_byte_lead_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0xF0));
}

TEST(max_byte_rejected) {
  EXPECT_FALSE(sourcemeta::core::is_utf8_continuation(0xFF));
}
