#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_utf8_decode, ascii) {
  const auto result{sourcemeta::core::utf8_decode("A", 0)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x41U);
  EXPECT_EQ(result.value().second, 1U);
}

TEST(Unicode_utf8_decode, two_byte) {
  const auto result{sourcemeta::core::utf8_decode("\xCE\xB1", 0)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x03B1U);
  EXPECT_EQ(result.value().second, 2U);
}

TEST(Unicode_utf8_decode, three_byte) {
  const auto result{sourcemeta::core::utf8_decode("\xE4\xB8\xAD", 0)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x4E2DU);
  EXPECT_EQ(result.value().second, 3U);
}

TEST(Unicode_utf8_decode, four_byte) {
  const auto result{sourcemeta::core::utf8_decode("\xF0\x9F\x98\x80", 0)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x1F600U);
  EXPECT_EQ(result.value().second, 4U);
}

TEST(Unicode_utf8_decode, at_offset) {
  const auto result{sourcemeta::core::utf8_decode("A\xCE\xB1", 1)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x03B1U);
  EXPECT_EQ(result.value().second, 2U);
}

TEST(Unicode_utf8_decode, max_codepoint) {
  const auto result{sourcemeta::core::utf8_decode("\xF4\x8F\xBF\xBF", 0)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x10FFFFU);
  EXPECT_EQ(result.value().second, 4U);
}

TEST(Unicode_utf8_decode, empty) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("", 0).has_value());
}

TEST(Unicode_utf8_decode, position_out_of_range) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("A", 5).has_value());
}

TEST(Unicode_utf8_decode, truncated) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("\xCE", 0).has_value());
}

TEST(Unicode_utf8_decode, lone_continuation) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("\x80", 0).has_value());
}

TEST(Unicode_utf8_decode, overlong) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("\xC0\xAF", 0).has_value());
}

TEST(Unicode_utf8_decode, surrogate) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("\xED\xA0\x80", 0).has_value());
}

TEST(Unicode_utf8_decode, above_max) {
  EXPECT_FALSE(
      sourcemeta::core::utf8_decode("\xF4\x90\x80\x80", 0).has_value());
}
