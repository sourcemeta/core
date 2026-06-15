#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(Text_strip_left, no_leading_character) {
  EXPECT_EQ(sourcemeta::core::strip_left("abc", '0'), "abc");
}

TEST(Text_strip_left, single_leading_character) {
  EXPECT_EQ(sourcemeta::core::strip_left("0abc", '0'), "abc");
}

TEST(Text_strip_left, multiple_leading_characters) {
  EXPECT_EQ(sourcemeta::core::strip_left("000123", '0'), "123");
}

TEST(Text_strip_left, only_leading_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_left("00a0b0", '0'), "a0b0");
}

TEST(Text_strip_left, all_characters_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_left("0000", '0'), "");
}

TEST(Text_strip_left, empty_input) {
  EXPECT_EQ(sourcemeta::core::strip_left("", '0'), "");
}

TEST(Text_strip_left, strips_nul_bytes) {
  const std::string input("\x00\x00\x2a", 3);
  EXPECT_EQ(sourcemeta::core::strip_left(input, '\x00'), "\x2a");
}

TEST(Text_strip_left, strips_spaces) {
  EXPECT_EQ(sourcemeta::core::strip_left("   value", ' '), "value");
}
