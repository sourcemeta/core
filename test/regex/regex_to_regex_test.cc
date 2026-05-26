#include <gtest/gtest.h>

#include <sourcemeta/core/regex.h>

TEST(Regex_to_regex, valid_1) {
  const auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
}

TEST(Regex_to_regex, invalid_1) {
  const auto regex{sourcemeta::core::to_regex("(abc")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_to_regex, posix_character_class) {
  const auto regex{sourcemeta::core::to_regex("^[[:digit:]]+$")};
  ASSERT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "123"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "abc"));
}

TEST(Regex_to_regex, posix_alpha_character_class) {
  const auto regex{sourcemeta::core::to_regex("^[[:alpha:]]+$")};
  ASSERT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "123"));
}

TEST(Regex_to_regex, posix_class_in_osv_style_pattern) {
  const auto regex{
      sourcemeta::core::to_regex("^(AlmaLinux|Alpine)(:[[:digit:]]+)?")};
  ASSERT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "AlmaLinux:9"));
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "Alpine:3"));
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "AlmaLinux"));
}

TEST(Regex_to_regex, pcre_named_capture_group) {
  const auto regex{sourcemeta::core::to_regex("^(?P<year>[0-9]+)$")};
  ASSERT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "2026"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "abc"));
}

TEST(Regex_to_regex, pcre_possessive_quantifier) {
  const auto regex{sourcemeta::core::to_regex("^a*+b$")};
  ASSERT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "aaab"));
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "b"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "aaa"));
}

TEST(Regex_to_regex, pcre_backtracking_control_verb) {
  const auto regex{sourcemeta::core::to_regex("^(*COMMIT)abc$")};
  ASSERT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "xyz"));
}

TEST(Regex_to_regex, pcre_unknown_escape) {
  const auto regex{sourcemeta::core::to_regex("\\Aabc$")};
  ASSERT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "xabc"));
}
