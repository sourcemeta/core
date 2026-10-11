#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string> // std::string

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

// The same questions asked of values the compiler cannot fold, so the answers
// come from the code that ships rather than from constant evaluation
TEST(strings_at_runtime) {
  std::string empty;
  EXPECT_FALSE(sourcemeta::core::is_digit(empty));
  std::string accepted{"123"};
  EXPECT_TRUE(sourcemeta::core::is_digit(accepted));
  std::string refused{"12a"};
  EXPECT_FALSE(sourcemeta::core::is_digit(refused));
}
