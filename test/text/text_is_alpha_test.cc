#include <sourcemeta/core/test.h>

#include <sourcemeta/core/text.h>

TEST(lowercase_letters) {
  EXPECT_TRUE(sourcemeta::core::is_alpha('a'));
  EXPECT_TRUE(sourcemeta::core::is_alpha('m'));
  EXPECT_TRUE(sourcemeta::core::is_alpha('z'));
}

TEST(uppercase_letters) {
  EXPECT_TRUE(sourcemeta::core::is_alpha('A'));
  EXPECT_TRUE(sourcemeta::core::is_alpha('M'));
  EXPECT_TRUE(sourcemeta::core::is_alpha('Z'));
}

TEST(digits) {
  EXPECT_FALSE(sourcemeta::core::is_alpha('0'));
  EXPECT_FALSE(sourcemeta::core::is_alpha('9'));
}

TEST(punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_alpha('-'));
  EXPECT_FALSE(sourcemeta::core::is_alpha('_'));
  EXPECT_FALSE(sourcemeta::core::is_alpha(' '));
}

TEST(ascii_boundaries) {
  // The ASCII characters immediately outside the two letter ranges
  EXPECT_FALSE(sourcemeta::core::is_alpha('@'));
  EXPECT_FALSE(sourcemeta::core::is_alpha('['));
  EXPECT_FALSE(sourcemeta::core::is_alpha('`'));
  EXPECT_FALSE(sourcemeta::core::is_alpha('{'));
}

TEST(string_all_letters) { EXPECT_TRUE(sourcemeta::core::is_alpha("abcXYZ")); }

TEST(string_with_digit) { EXPECT_FALSE(sourcemeta::core::is_alpha("abc1")); }

TEST(string_with_punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_alpha("ab-cd"));
}

TEST(empty_string) { EXPECT_FALSE(sourcemeta::core::is_alpha("")); }
