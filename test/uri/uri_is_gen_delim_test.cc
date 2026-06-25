#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_is_gen_delim, all_generic_delimiters) {
  EXPECT_TRUE(sourcemeta::core::URI::is_gen_delim(':'));
  EXPECT_TRUE(sourcemeta::core::URI::is_gen_delim('/'));
  EXPECT_TRUE(sourcemeta::core::URI::is_gen_delim('?'));
  EXPECT_TRUE(sourcemeta::core::URI::is_gen_delim('#'));
  EXPECT_TRUE(sourcemeta::core::URI::is_gen_delim('['));
  EXPECT_TRUE(sourcemeta::core::URI::is_gen_delim(']'));
  EXPECT_TRUE(sourcemeta::core::URI::is_gen_delim('@'));
}

TEST(URI_is_gen_delim, sub_delimiters_are_not_generic) {
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('!'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('$'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('&'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('+'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('='));
}

TEST(URI_is_gen_delim, unreserved_are_not_generic) {
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('a'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('Z'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('0'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('-'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('.'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('_'));
  EXPECT_FALSE(sourcemeta::core::URI::is_gen_delim('~'));
}
