#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

TEST(Text_is_alphanum, letters) {
  EXPECT_TRUE(sourcemeta::core::is_alphanum('a'));
  EXPECT_TRUE(sourcemeta::core::is_alphanum('Z'));
}

TEST(Text_is_alphanum, digits) {
  EXPECT_TRUE(sourcemeta::core::is_alphanum('0'));
  EXPECT_TRUE(sourcemeta::core::is_alphanum('9'));
}

TEST(Text_is_alphanum, punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_alphanum('-'));
  EXPECT_FALSE(sourcemeta::core::is_alphanum('_'));
  EXPECT_FALSE(sourcemeta::core::is_alphanum('.'));
  EXPECT_FALSE(sourcemeta::core::is_alphanum(' '));
}
