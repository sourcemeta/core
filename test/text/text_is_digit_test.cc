#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

TEST(Text_is_digit, digits) {
  EXPECT_TRUE(sourcemeta::core::is_digit('0'));
  EXPECT_TRUE(sourcemeta::core::is_digit('5'));
  EXPECT_TRUE(sourcemeta::core::is_digit('9'));
}

TEST(Text_is_digit, letters) {
  EXPECT_FALSE(sourcemeta::core::is_digit('a'));
  EXPECT_FALSE(sourcemeta::core::is_digit('Z'));
}

TEST(Text_is_digit, punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_digit('-'));
  EXPECT_FALSE(sourcemeta::core::is_digit(' '));
}

TEST(Text_is_digit, ascii_boundaries) {
  // The ASCII characters immediately outside the digit range
  EXPECT_FALSE(sourcemeta::core::is_digit('/'));
  EXPECT_FALSE(sourcemeta::core::is_digit(':'));
}

TEST(Text_is_digit, string_all_digits) {
  EXPECT_TRUE(sourcemeta::core::is_digit("0123456789"));
}

TEST(Text_is_digit, string_with_letter) {
  EXPECT_FALSE(sourcemeta::core::is_digit("12a"));
}

TEST(Text_is_digit, empty_string) {
  EXPECT_FALSE(sourcemeta::core::is_digit(""));
}
