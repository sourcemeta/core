#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

TEST(Text_trim, no_whitespace_returns_input_unchanged) {
  EXPECT_EQ(sourcemeta::core::trim("hello"), "hello");
}

TEST(Text_trim, leading_spaces) {
  EXPECT_EQ(sourcemeta::core::trim("   hello"), "hello");
}

TEST(Text_trim, trailing_spaces) {
  EXPECT_EQ(sourcemeta::core::trim("hello   "), "hello");
}

TEST(Text_trim, both_ends_spaces) {
  EXPECT_EQ(sourcemeta::core::trim("  hello  "), "hello");
}

TEST(Text_trim, preserves_internal_whitespace) {
  EXPECT_EQ(sourcemeta::core::trim("  hello world  "), "hello world");
}

TEST(Text_trim, tabs) {
  EXPECT_EQ(sourcemeta::core::trim("\thello\t"), "hello");
}

TEST(Text_trim, line_feed) {
  EXPECT_EQ(sourcemeta::core::trim("\nhello\n"), "hello");
}

TEST(Text_trim, carriage_return) {
  EXPECT_EQ(sourcemeta::core::trim("\rhello\r"), "hello");
}

TEST(Text_trim, vertical_tab) {
  EXPECT_EQ(sourcemeta::core::trim("\vhello\v"), "hello");
}

TEST(Text_trim, form_feed) {
  EXPECT_EQ(sourcemeta::core::trim("\fhello\f"), "hello");
}

TEST(Text_trim, mixed_whitespace_characters) {
  EXPECT_EQ(sourcemeta::core::trim("\t\n\r\v\f hello \f\v\r\n\t"), "hello");
}

TEST(Text_trim, all_whitespace_returns_empty) {
  EXPECT_EQ(sourcemeta::core::trim("    "), "");
}

TEST(Text_trim, all_whitespace_mixed_returns_empty) {
  EXPECT_EQ(sourcemeta::core::trim("\t\n \r"), "");
}

TEST(Text_trim, empty_input_returns_empty) {
  EXPECT_EQ(sourcemeta::core::trim(""), "");
}

TEST(Text_trim, single_non_whitespace_character) {
  EXPECT_EQ(sourcemeta::core::trim("a"), "a");
}

TEST(Text_trim, single_whitespace_character) {
  EXPECT_EQ(sourcemeta::core::trim(" "), "");
}

TEST(Text_trim, non_ascii_bytes_pass_through) {
  const std::string_view input{"\xC3\xA9 caf\xC3\xA9 "};
  EXPECT_EQ(sourcemeta::core::trim(input), "\xC3\xA9 caf\xC3\xA9");
}

TEST(Text_trim, null_byte_is_not_whitespace) {
  const std::string_view input{"\0hello\0", 7};
  EXPECT_EQ(sourcemeta::core::trim(input), input);
}
