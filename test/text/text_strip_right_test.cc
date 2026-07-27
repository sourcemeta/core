#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

TEST(no_trailing_character) {
  EXPECT_EQ(sourcemeta::core::strip_right("abc", '0'), "abc");
}

TEST(single_trailing_character) {
  EXPECT_EQ(sourcemeta::core::strip_right("abc0", '0'), "abc");
}

TEST(multiple_trailing_characters) {
  EXPECT_EQ(sourcemeta::core::strip_right("123000", '0'), "123");
}

TEST(only_trailing_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_right("0a0b00", '0'), "0a0b");
}

TEST(all_characters_stripped) {
  EXPECT_EQ(sourcemeta::core::strip_right("0000", '0'), "");
}

TEST(empty_input) { EXPECT_EQ(sourcemeta::core::strip_right("", '0'), ""); }

TEST(strips_carriage_return) {
  EXPECT_EQ(sourcemeta::core::strip_right("line\r", '\r'), "line");
}

TEST(strips_trailing_spaces) {
  EXPECT_EQ(sourcemeta::core::strip_right("value   ", ' '), "value");
}
