#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(ascii_null) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x0000), 1u);
}

TEST(ascii_letter) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x0041), 1u);
}

TEST(ascii_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x007F), 1u);
}

TEST(two_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x0080), 2u);
}

TEST(two_byte_latin_e_acute) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x00E9), 2u);
}

TEST(two_byte_greek_alpha) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x03B1), 2u);
}

TEST(two_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x07FF), 2u);
}

TEST(three_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x0800), 3u);
}

TEST(three_byte_cjk) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x4E2D), 3u);
}

TEST(three_byte_korean_si) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0xC2E4), 3u);
}

TEST(three_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0xFFFF), 3u);
}

TEST(four_byte_low_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x10000), 4u);
}

TEST(four_byte_emoji_grinning) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x1F600), 4u);
}

TEST(four_byte_smp_mid) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x40000), 4u);
}

TEST(four_byte_high_boundary) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_byte_count(0x10FFFF), 4u);
}
