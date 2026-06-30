#include <sourcemeta/core/test.h>

#include <sourcemeta/core/unicode.h>

TEST(ascii) {
  const auto result{sourcemeta::core::utf8_decode("A", 0)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x41U);
  EXPECT_EQ(result.value().second, 1U);
}

TEST(two_byte) {
  const auto result{sourcemeta::core::utf8_decode("\xCE\xB1", 0)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x03B1U);
  EXPECT_EQ(result.value().second, 2U);
}

TEST(three_byte) {
  const auto result{sourcemeta::core::utf8_decode("\xE4\xB8\xAD", 0)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x4E2DU);
  EXPECT_EQ(result.value().second, 3U);
}

TEST(four_byte) {
  const auto result{sourcemeta::core::utf8_decode("\xF0\x9F\x98\x80", 0)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x1F600U);
  EXPECT_EQ(result.value().second, 4U);
}

TEST(at_offset) {
  const auto result{sourcemeta::core::utf8_decode("A\xCE\xB1", 1)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x03B1U);
  EXPECT_EQ(result.value().second, 2U);
}

TEST(max_codepoint) {
  const auto result{sourcemeta::core::utf8_decode("\xF4\x8F\xBF\xBF", 0)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().first, 0x10FFFFU);
  EXPECT_EQ(result.value().second, 4U);
}

TEST(empty) { EXPECT_FALSE(sourcemeta::core::utf8_decode("", 0).has_value()); }

TEST(position_out_of_range) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("A", 5).has_value());
}

TEST(truncated) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("\xCE", 0).has_value());
}

TEST(lone_continuation) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("\x80", 0).has_value());
}

TEST(overlong) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("\xC0\xAF", 0).has_value());
}

TEST(surrogate) {
  EXPECT_FALSE(sourcemeta::core::utf8_decode("\xED\xA0\x80", 0).has_value());
}

TEST(above_max) {
  EXPECT_FALSE(
      sourcemeta::core::utf8_decode("\xF4\x90\x80\x80", 0).has_value());
}
