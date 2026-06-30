#include <sourcemeta/core/test.h>

#include <sourcemeta/core/text.h>

TEST(digits) {
  EXPECT_TRUE(sourcemeta::core::is_digit('0'));
  EXPECT_TRUE(sourcemeta::core::is_digit('5'));
  EXPECT_TRUE(sourcemeta::core::is_digit('9'));
}

TEST(letters) {
  EXPECT_FALSE(sourcemeta::core::is_digit('a'));
  EXPECT_FALSE(sourcemeta::core::is_digit('Z'));
}

TEST(punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_digit('-'));
  EXPECT_FALSE(sourcemeta::core::is_digit(' '));
}

TEST(ascii_boundaries) {
  // The ASCII characters immediately outside the digit range
  EXPECT_FALSE(sourcemeta::core::is_digit('/'));
  EXPECT_FALSE(sourcemeta::core::is_digit(':'));
}

TEST(string_all_digits) {
  EXPECT_TRUE(sourcemeta::core::is_digit("0123456789"));
}

TEST(string_with_letter) { EXPECT_FALSE(sourcemeta::core::is_digit("12a")); }

TEST(empty_string) { EXPECT_FALSE(sourcemeta::core::is_digit("")); }
