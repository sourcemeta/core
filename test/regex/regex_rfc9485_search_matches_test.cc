#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

TEST(rfc9485_search_substring_at_start) {
  const auto regex{sourcemeta::core::to_regex(
      "ab.*", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abyy"));
}

TEST(rfc9485_search_substring_in_middle) {
  const auto regex{sourcemeta::core::to_regex(
      "ab", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xxabyy"));
}

TEST(rfc9485_search_substring_at_end) {
  const auto regex{sourcemeta::core::to_regex(
      "ab", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xxab"));
}

TEST(rfc9485_search_full_input_still_matches) {
  const auto regex{sourcemeta::core::to_regex(
      "ab", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_search_no_occurrence) {
  const auto regex{sourcemeta::core::to_regex(
      "ab", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xyz"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ba"));
}

TEST(rfc9485_search_empty_pattern_matches_everything) {
  const auto regex{sourcemeta::core::to_regex(
      "", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "anything"));
}

TEST(rfc9485_search_dot_excludes_line_feed) {
  const auto regex{sourcemeta::core::to_regex(
      ".", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\n\r"));
}

TEST(rfc9485_search_occurrence_after_line_feed) {
  const auto regex{sourcemeta::core::to_regex(
      "b.", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\nbc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a\nb\n"));
}

TEST(rfc9485_search_alternation) {
  const auto regex{sourcemeta::core::to_regex(
      "cat|dog", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "my cat sleeps"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hotdog"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bird"));
}

TEST(rfc9485_search_quantifier) {
  const auto regex{sourcemeta::core::to_regex(
      "a{2,3}", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xaax"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xax"));
}

TEST(rfc9485_search_class) {
  const auto regex{sourcemeta::core::to_regex(
      "[0-9]", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc5def"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abcdef"));
}

TEST(rfc9485_search_category) {
  const auto regex{sourcemeta::core::to_regex(
      "\\p{Lu}", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aBc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(rfc9485_search_astral) {
  const auto regex{sourcemeta::core::to_regex(
      "🤔", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a🤔b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_search_invalid_shorthand_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "\\d", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_FALSE(regex.has_value());
}

TEST(rfc9485_search_invalid_double_quantifier) {
  const auto regex{sourcemeta::core::to_regex(
      "a**", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_FALSE(regex.has_value());
}

TEST(rfc9485_search_invalid_reversed_range) {
  const auto regex{sourcemeta::core::to_regex(
      "[z-a]", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_FALSE(regex.has_value());
}

TEST(rfc9485_search_invalid_lookahead) {
  const auto regex{sourcemeta::core::to_regex(
      "(?=a)", sourcemeta::core::RegexDialect::IRegexpSearch)};
  EXPECT_FALSE(regex.has_value());
}
