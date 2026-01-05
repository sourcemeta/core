#include <gtest/gtest.h>

#include <sourcemeta/core/regex.h>

#include <string>

TEST(Regex_matches_if_valid, match_pattern_true) {
  EXPECT_TRUE(sourcemeta::core::matches_if_valid("^foo", "foobar"));
}

TEST(Regex_matches_if_valid, match_pattern_false) {
  EXPECT_FALSE(sourcemeta::core::matches_if_valid("^bar", "foobar"));
}
