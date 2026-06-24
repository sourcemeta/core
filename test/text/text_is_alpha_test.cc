#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

TEST(Text_is_alpha, lowercase_letters) {
  EXPECT_TRUE(sourcemeta::core::is_alpha('a'));
  EXPECT_TRUE(sourcemeta::core::is_alpha('m'));
  EXPECT_TRUE(sourcemeta::core::is_alpha('z'));
}

TEST(Text_is_alpha, uppercase_letters) {
  EXPECT_TRUE(sourcemeta::core::is_alpha('A'));
  EXPECT_TRUE(sourcemeta::core::is_alpha('M'));
  EXPECT_TRUE(sourcemeta::core::is_alpha('Z'));
}

TEST(Text_is_alpha, digits) {
  EXPECT_FALSE(sourcemeta::core::is_alpha('0'));
  EXPECT_FALSE(sourcemeta::core::is_alpha('9'));
}

TEST(Text_is_alpha, punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_alpha('-'));
  EXPECT_FALSE(sourcemeta::core::is_alpha('_'));
  EXPECT_FALSE(sourcemeta::core::is_alpha(' '));
}

TEST(Text_is_alpha, ascii_boundaries) {
  // The ASCII characters immediately outside the two letter ranges
  EXPECT_FALSE(sourcemeta::core::is_alpha('@'));
  EXPECT_FALSE(sourcemeta::core::is_alpha('['));
  EXPECT_FALSE(sourcemeta::core::is_alpha('`'));
  EXPECT_FALSE(sourcemeta::core::is_alpha('{'));
}
