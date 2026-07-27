#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

TEST(wrapped_in_quotes) {
  EXPECT_EQ(sourcemeta::core::unquote("\"abc\"", '"'), "abc");
}

TEST(not_wrapped) { EXPECT_EQ(sourcemeta::core::unquote("abc", '"'), "abc"); }

TEST(lone_quote_is_unchanged) {
  EXPECT_EQ(sourcemeta::core::unquote("\"", '"'), "\"");
}

TEST(empty_pair_yields_empty) {
  EXPECT_EQ(sourcemeta::core::unquote("\"\"", '"'), "");
}

TEST(only_leading_quote_is_unchanged) {
  EXPECT_EQ(sourcemeta::core::unquote("\"abc", '"'), "\"abc");
}

TEST(only_trailing_quote_is_unchanged) {
  EXPECT_EQ(sourcemeta::core::unquote("abc\"", '"'), "abc\"");
}

TEST(empty_input_is_unchanged) {
  EXPECT_EQ(sourcemeta::core::unquote("", '"'), "");
}

TEST(inner_quotes_are_preserved) {
  EXPECT_EQ(sourcemeta::core::unquote("\"a\"b\"", '"'), "a\"b");
}

TEST(honours_the_given_quote_character) {
  EXPECT_EQ(sourcemeta::core::unquote("'abc'", '\''), "abc");
}

TEST(mismatched_quote_character_is_unchanged) {
  EXPECT_EQ(sourcemeta::core::unquote("\"abc\"", '\''), "\"abc\"");
}

TEST(single_character_between_quotes) {
  EXPECT_EQ(sourcemeta::core::unquote("\"a\"", '"'), "a");
}
