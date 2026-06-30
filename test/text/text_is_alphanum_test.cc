#include <sourcemeta/core/test.h>

#include <sourcemeta/core/text.h>

TEST(letters) {
  EXPECT_TRUE(sourcemeta::core::is_alphanum('a'));
  EXPECT_TRUE(sourcemeta::core::is_alphanum('Z'));
}

TEST(digits) {
  EXPECT_TRUE(sourcemeta::core::is_alphanum('0'));
  EXPECT_TRUE(sourcemeta::core::is_alphanum('9'));
}

TEST(punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_alphanum('-'));
  EXPECT_FALSE(sourcemeta::core::is_alphanum('_'));
  EXPECT_FALSE(sourcemeta::core::is_alphanum('.'));
  EXPECT_FALSE(sourcemeta::core::is_alphanum(' '));
}

TEST(string_letters_and_digits) {
  EXPECT_TRUE(sourcemeta::core::is_alphanum("abc123XYZ"));
}

TEST(string_with_punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_alphanum("abc-123"));
}

TEST(empty_string) { EXPECT_FALSE(sourcemeta::core::is_alphanum("")); }
