#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <optional> // std::optional
#include <string>

namespace {

auto permissive(const std::string &pattern)
    -> std::optional<sourcemeta::core::Regex> {
  return sourcemeta::core::to_regex(pattern,
                                    sourcemeta::core::RegexDialect::Permissive);
}

} // namespace

TEST(class_intersection) {
  const auto regex{permissive(R"(^[\d&&[0-5]]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "3"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "7"));
}

TEST(class_subtraction) {
  const auto regex{permissive(R"(^[[a-z]--[aeiou]]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(class_intersection_of_shorthands) {
  const auto regex{permissive(R"(^[\d&&\w]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "4"));
}

TEST(class_intersection_of_nested_classes) {
  const auto regex{permissive(R"(^[[abc]&&[bcd]]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(class_intersection_with_a_hex_escape) {
  const auto regex{permissive(R"(^[\u0041&&[A-Z]]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
}

TEST(class_intersection_with_a_braced_hex_escape) {
  const auto regex{permissive(R"(^[\u{41}&&[A-Z]]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(class_intersection_with_a_braced_hex_escape_past_unicode) {
  const auto regex{permissive(R"(^[\u{110000}&&[A-Z]]$)")};
  EXPECT_TRUE(regex.has_value());
}

TEST(class_intersection_with_a_malformed_hex_escape) {
  const auto regex{permissive(R"(^[\u12G4&&[A-Z]]$)")};
  EXPECT_TRUE(regex.has_value());
}

TEST(class_intersection_with_a_null_escape) {
  const auto regex{permissive(R"(^[\0&&[\x00-\x10]]$)")};
  EXPECT_TRUE(regex.has_value());
}

TEST(class_intersection_with_a_control_escape) {
  const auto regex{permissive(R"(^[\cA&&[\x00-\x10]]$)")};
  EXPECT_TRUE(regex.has_value());
}

TEST(class_intersection_with_a_simple_escape) {
  const auto regex{permissive(R"(^[\t&&[\x00-\x10]]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\t"));
}

TEST(class_intersection_with_a_negated_shorthand) {
  const auto regex{permissive(R"(^[\D&&[a-z]]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(class_intersection_with_an_unmatched_nested_bracket) {
  const auto regex{permissive(R"(^[[a&&[ab]]$)")};
  EXPECT_TRUE(regex.has_value());
}

TEST(class_subtraction_of_shorthands) {
  const auto regex{permissive(R"(^[\w--\d]$)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "4"));
}

TEST(brace_quantifier_followed_by_a_plus) {
  const auto regex{permissive(R"(^a{2,3}+$)")};
  EXPECT_TRUE(regex.has_value());
}

TEST(brace_quantifier_with_only_a_minimum_followed_by_a_plus) {
  const auto regex{permissive(R"(^a{2}+$)")};
  EXPECT_TRUE(regex.has_value());
}

TEST(trailing_backslash) {
  const auto regex{permissive(R"(^a\)")};
  EXPECT_FALSE(regex.has_value());
}
