#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(Text_to_title_case, empty_string) {
  std::string value{""};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "");
}

TEST(Text_to_title_case, single_lowercase_character) {
  std::string value{"a"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "A");
}

TEST(Text_to_title_case, single_uppercase_character) {
  std::string value{"A"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "A");
}

TEST(Text_to_title_case, single_underscore_is_empty) {
  std::string value{"_"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "");
}

TEST(Text_to_title_case, single_dash_is_empty) {
  std::string value{"-"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "");
}

TEST(Text_to_title_case, only_separators_is_empty) {
  std::string value{"___"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "");
}

TEST(Text_to_title_case, single_lowercase_word) {
  std::string value{"hello"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, already_title_cased_word) {
  std::string value{"Hello"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, all_uppercase_word) {
  std::string value{"HELLO"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "HELLO");
}

TEST(Text_to_title_case, snake_case_two_words) {
  std::string value{"hello_world"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello World");
}

TEST(Text_to_title_case, kebab_case_two_words) {
  std::string value{"hello-world"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello World");
}

TEST(Text_to_title_case, mixed_snake_and_kebab_separators) {
  std::string value{"hello_world-test"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello World Test");
}

TEST(Text_to_title_case, snake_case_three_words) {
  std::string value{"abc_def_ghi"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Abc Def Ghi");
}

TEST(Text_to_title_case, preserves_existing_uppercase_after_separator) {
  std::string value{"Hello_World"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello World");
}

TEST(Text_to_title_case, leading_underscore_is_stripped) {
  std::string value{"_hello"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, leading_dash_is_stripped) {
  std::string value{"-hello"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, trailing_underscore_is_stripped) {
  std::string value{"hello_"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, trailing_dash_is_stripped) {
  std::string value{"hello-"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, separators_around_word_are_stripped) {
  std::string value{"_hello_"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, multiple_leading_separators_are_stripped) {
  std::string value{"__hello"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, multiple_trailing_separators_are_stripped) {
  std::string value{"hello__"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello");
}

TEST(Text_to_title_case, consecutive_separators_collapse_to_single_space) {
  std::string value{"hello__world"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello World");
}

TEST(Text_to_title_case, mixed_consecutive_separators_collapse) {
  std::string value{"hello_-world"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello World");
}

TEST(Text_to_title_case, single_letter_words) {
  std::string value{"a_b_c"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "A B C");
}

TEST(Text_to_title_case, digits_pass_through) {
  std::string value{"abc123def"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Abc123def");
}

TEST(Text_to_title_case, digit_after_separator) {
  std::string value{"abc_123"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Abc 123");
}

TEST(Text_to_title_case, letter_after_leading_digits_in_segment) {
  std::string value{"abc_123def"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Abc 123Def");
}

TEST(Text_to_title_case, leading_digits_then_letter_at_start) {
  std::string value{"123abc"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "123Abc");
}

TEST(Text_to_title_case, space_in_input_is_not_a_separator) {
  std::string value{"hello world"};
  sourcemeta::core::to_title_case(value);
  EXPECT_EQ(value, "Hello world");
}

TEST(Text_to_title_case, non_ascii_byte_is_not_uppercased) {
  std::string value{};
  value.push_back(static_cast<char>(0xE9));
  value.append("hello");
  sourcemeta::core::to_title_case(value);
  std::string expected{};
  expected.push_back(static_cast<char>(0xE9));
  expected.append("Hello");
  EXPECT_EQ(value, expected);
}
