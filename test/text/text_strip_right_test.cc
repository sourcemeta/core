#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

TEST(Text_strip_right, no_trailing_character) {
  EXPECT_EQ(sourcemeta::core::strip_right("abc", '0'), "abc");
}

TEST(Text_strip_right, single_trailing_character) {
  EXPECT_EQ(sourcemeta::core::strip_right("abc0", '0'), "abc");
}

TEST(Text_strip_right, multiple_trailing_characters) {
  EXPECT_EQ(sourcemeta::core::strip_right("123000", '0'), "123");
}

TEST(Text_strip_right, only_trailing_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_right("0a0b00", '0'), "0a0b");
}

TEST(Text_strip_right, all_characters_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_right("0000", '0'), "");
}

TEST(Text_strip_right, empty_input) {
  EXPECT_EQ(sourcemeta::core::strip_right("", '0'), "");
}

TEST(Text_strip_right, strips_carriage_return) {
  EXPECT_EQ(sourcemeta::core::strip_right("line\r", '\r'), "line");
}

TEST(Text_strip_right, strips_trailing_spaces) {
  EXPECT_EQ(sourcemeta::core::strip_right("value   ", ' '), "value");
}
