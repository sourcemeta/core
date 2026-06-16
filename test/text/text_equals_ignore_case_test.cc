#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

TEST(Text_equals_ignore_case, identical) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("hello", "hello"));
}

TEST(Text_equals_ignore_case, differing_case) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("Hello", "hELLO"));
}

TEST(Text_equals_ignore_case, all_uppercase_against_all_lowercase) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("AT+JWT", "at+jwt"));
}

TEST(Text_equals_ignore_case, different_content) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("foo", "bar"));
}

TEST(Text_equals_ignore_case, different_length) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("hello", "hell"));
}

TEST(Text_equals_ignore_case, prefix_is_not_equal) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("hell", "hello"));
}

TEST(Text_equals_ignore_case, both_empty) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("", ""));
}

TEST(Text_equals_ignore_case, one_empty) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("", "x"));
}

TEST(Text_equals_ignore_case, non_alphabetic_characters_match) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("a-1_2.3", "A-1_2.3"));
}

TEST(Text_equals_ignore_case, digits_are_not_case_folded) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("123", "124"));
}
