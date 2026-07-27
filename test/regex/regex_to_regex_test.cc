#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(valid_1) {
  const auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
}

TEST(deeply_nested_character_class_is_rejected) {
  const std::string pattern{std::string(100, '[') + std::string(100, ']')};
  EXPECT_FALSE(sourcemeta::core::to_regex(pattern).has_value());
}

TEST(invalid_1) {
  const auto regex{sourcemeta::core::to_regex("(abc")};
  EXPECT_FALSE(regex.has_value());
}

TEST(posix_character_class) {
  const auto regex{sourcemeta::core::to_regex("^[[:digit:]]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "123"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "abc"));
}

TEST(posix_alpha_character_class) {
  const auto regex{sourcemeta::core::to_regex("^[[:alpha:]]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "123"));
}

TEST(posix_class_in_osv_style_pattern) {
  const auto regex{
      sourcemeta::core::to_regex("^(AlmaLinux|Alpine)(:[[:digit:]]+)?")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "AlmaLinux:9"));
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "Alpine:3"));
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "AlmaLinux"));
}

TEST(pcre_named_capture_group) {
  const auto regex{sourcemeta::core::to_regex("^(?P<year>[0-9]+)$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "2026"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "abc"));
}

TEST(pcre_possessive_quantifier) {
  const auto regex{sourcemeta::core::to_regex("^a*+b$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "aaab"));
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "b"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "aaa"));
}

TEST(pcre_backtracking_control_verb) {
  const auto regex{sourcemeta::core::to_regex("^(*COMMIT)abc$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "xyz"));
}

TEST(pcre_unknown_escape) {
  const auto regex{sourcemeta::core::to_regex("\\Aabc$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "xabc"));
}

TEST(numbered_backreference) {
  const auto regex{sourcemeta::core::to_regex("^(a)\\1$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "aa"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "a"));
}

TEST(unicode_brace_escape_with_leading_zeros) {
  const auto regex{sourcemeta::core::to_regex("^\\u{00000041}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(*regex, "A"));
  EXPECT_FALSE(sourcemeta::core::matches(*regex, "B"));
}

TEST(oversized_unicode_brace_escape_does_not_overflow) {
  // A digit run long enough to overflow a 32-bit accumulator must be handled
  // without undefined behavior
  const auto regex{sourcemeta::core::to_regex("\\u{FFFFFFFFFFFFFFFFFFFFFFFF}")};
  EXPECT_FALSE(regex.has_value());
}
