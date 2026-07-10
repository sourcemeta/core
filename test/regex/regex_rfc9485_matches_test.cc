#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <string>
#include <variant>

TEST(rfc9485_matches_empty_pattern) {
  const auto regex{
      sourcemeta::core::to_regex("", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "anything"));
}

TEST(rfc9485_matches_literal_single) {
  const auto regex{
      sourcemeta::core::to_regex("a", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(rfc9485_matches_literal_word) {
  const auto regex{sourcemeta::core::to_regex(
      "hello", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hello"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "hello world"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "say hello"));
}

TEST(rfc9485_matches_literal_astral) {
  const auto regex{sourcemeta::core::to_regex(
      "🤔", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "🤔"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_empty_branch_trailing) {
  const auto regex{sourcemeta::core::to_regex(
      "a|", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xyz"));
}

TEST(rfc9485_matches_empty_branch_leading) {
  const auto regex{sourcemeta::core::to_regex(
      "|b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xyz"));
}

TEST(rfc9485_matches_empty_branches_only) {
  const auto regex{sourcemeta::core::to_regex(
      "||", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(rfc9485_matches_literal_leading_caret) {
  const auto regex{sourcemeta::core::to_regex(
      "^ab.*", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "^ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "^abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(rfc9485_matches_literal_trailing_dollar) {
  const auto regex{sourcemeta::core::to_regex(
      ".*bc$", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc$"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bc$"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bc$x"));
}

TEST(rfc9485_matches_literal_dollar_at_end) {
  const auto regex{sourcemeta::core::to_regex(
      "ab$", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab$"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab\n"));
}

TEST(rfc9485_matches_literal_middle_caret) {
  const auto regex{sourcemeta::core::to_regex(
      "a^b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a^b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_literal_middle_dollar) {
  const auto regex{sourcemeta::core::to_regex(
      "a$b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a$b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_dollar_in_class) {
  const auto regex{sourcemeta::core::to_regex(
      "a[$]b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a$b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_bare_comma) {
  const auto regex{sourcemeta::core::to_regex(
      "a,b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a,b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_bare_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "a-b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a-b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_single_char_escapes_punctuation) {
  const auto regex{sourcemeta::core::to_regex(
      "\\(\\)\\*\\+\\.\\?\\[\\\\", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "()*+.?[\\"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "()*+.?["));
}

TEST(rfc9485_matches_single_char_escapes_delimiters) {
  const auto regex{sourcemeta::core::to_regex(
      "\\]\\{\\|\\}\\t\\r\\n", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]{|}\t\r\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "]{|}trn"));
}

TEST(rfc9485_matches_escaped_caret) {
  const auto regex{sourcemeta::core::to_regex(
      "a\\^b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a^b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_escaped_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "a\\-b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a-b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_escaped_newline) {
  const auto regex{sourcemeta::core::to_regex(
      "a\\nb", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\nb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "anb"));
}

TEST(rfc9485_matches_quantifier_star) {
  const auto regex{sourcemeta::core::to_regex(
      "ab*", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abbb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xab"));
}

TEST(rfc9485_matches_quantifier_plus) {
  const auto regex{sourcemeta::core::to_regex(
      "ab+", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abbb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_quantifier_question) {
  const auto regex{sourcemeta::core::to_regex(
      "ab?", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abb"));
}

TEST(rfc9485_matches_quantifier_exact) {
  const auto regex{sourcemeta::core::to_regex(
      "a{3}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aaaa"));
}

TEST(rfc9485_matches_quantifier_at_least) {
  const auto regex{sourcemeta::core::to_regex(
      "a{2,}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_quantifier_bounded) {
  const auto regex{sourcemeta::core::to_regex(
      "a{2,4}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aaaaa"));
}

TEST(rfc9485_matches_quantifier_zero) {
  const auto regex{sourcemeta::core::to_regex(
      "a{0}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_quantifier_leading_zeros) {
  const auto regex{sourcemeta::core::to_regex(
      "a{002,004}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_quantifier_on_astral) {
  const auto regex{sourcemeta::core::to_regex(
      "🤔*", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "🤔🤔"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_quantifier_on_group) {
  const auto regex{sourcemeta::core::to_regex(
      "(ab)+", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aba"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(rfc9485_matches_quantifier_on_class) {
  const auto regex{sourcemeta::core::to_regex(
      "[ab]{2}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ba"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(rfc9485_matches_quantifier_on_category) {
  const auto regex{sourcemeta::core::to_regex(
      "\\p{Lu}+", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "AB"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "Ab"));
}

TEST(rfc9485_matches_alternation) {
  const auto regex{sourcemeta::core::to_regex(
      "cat|dog", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "cat"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "dog"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "cats"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a cat"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bird"));
}

TEST(rfc9485_matches_alternation_quantified) {
  const auto regex{sourcemeta::core::to_regex(
      "aa|bb{6,8}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bbbbbbb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bb"));
}

TEST(rfc9485_matches_group) {
  const auto regex{sourcemeta::core::to_regex(
      "(aa)", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_group_nested) {
  const auto regex{sourcemeta::core::to_regex(
      "((a)b)", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_group_alternation) {
  const auto regex{sourcemeta::core::to_regex(
      "aa(bb|cc)dd", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aabbdd"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaccdd"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aadd"));
}

TEST(rfc9485_matches_class_simple) {
  const auto regex{sourcemeta::core::to_regex(
      "[abc]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_class_negated) {
  const auto regex{sourcemeta::core::to_regex(
      "[^abc]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_class_range) {
  const auto regex{sourcemeta::core::to_regex(
      "[a-z]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "M"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(rfc9485_matches_class_astral_member) {
  const auto regex{sourcemeta::core::to_regex(
      "[a🤔b]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "🤔"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
}

TEST(rfc9485_matches_class_astral_range) {
  const auto regex{sourcemeta::core::to_regex(
      "[α-ω]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "β"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_class_leading_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[-ab]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(rfc9485_matches_class_trailing_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[ab-]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(rfc9485_matches_class_negated_leading_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[^-ab]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_class_only_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[-]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_class_escaped_range_endpoints) {
  const auto regex{sourcemeta::core::to_regex(
      "[a\\n-\\r-]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x0B"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\r"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(rfc9485_matches_class_escaped_brackets) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\[\\]]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "["));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_class_escaped_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[a\\-z]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "m"));
}

TEST(rfc9485_matches_class_category) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\p{Nd}]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_class_category_complement) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\P{Nd}]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(rfc9485_matches_class_negated_categories) {
  const auto regex{sourcemeta::core::to_regex(
      "[^\\p{Nd}\\p{Ll}]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_class_literal_dot) {
  const auto regex{sourcemeta::core::to_regex(
      "a[.b]c", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a.c"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "axc"));
}

TEST(rfc9485_matches_category_letter) {
  const auto regex{sourcemeta::core::to_regex(
      "\\p{L}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "é"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "中"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_category_uppercase) {
  const auto regex{sourcemeta::core::to_regex(
      "\\p{Lu}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "AB"));
}

TEST(rfc9485_matches_category_complement) {
  const auto regex{sourcemeta::core::to_regex(
      "\\P{Lu}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(rfc9485_matches_category_private_use) {
  const auto regex{sourcemeta::core::to_regex(
      "\\p{Co}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\xEE\x80\x80"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_dot) {
  const auto regex{
      sourcemeta::core::to_regex(".", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "🤔"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\r"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_dot_line_separator) {
  const auto regex{
      sourcemeta::core::to_regex(".", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\xE2\x80\xA8"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\xE2\x80\xA9"));
}

TEST(rfc9485_matches_dot_star) {
  const auto regex{sourcemeta::core::to_regex(
      ".*", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a\nb"));
}

TEST(rfc9485_matches_dot_plus) {
  const auto regex{sourcemeta::core::to_regex(
      ".+", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(rfc9485_matches_dot_star_is_not_noop) {
  const auto regex{sourcemeta::core::to_regex(
      ".*", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      std::holds_alternative<sourcemeta::core::RegexTypePCRE2>(regex.value()));
}

TEST(rfc9485_matches_dot_plus_is_not_non_empty) {
  const auto regex{sourcemeta::core::to_regex(
      ".+", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      std::holds_alternative<sourcemeta::core::RegexTypePCRE2>(regex.value()));
}

TEST(rfc9485_matches_dot_between_letters_astral) {
  const auto regex{sourcemeta::core::to_regex(
      "a.b", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\xF0\x90\x84\x81"
                                                       "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(rfc9485_matches_escaped_dot) {
  const auto regex{sourcemeta::core::to_regex(
      "a\\.c", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a.c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "axc"));
}

TEST(rfc9485_matches_dot_after_escaped_backslash) {
  const auto regex{sourcemeta::core::to_regex(
      "a\\\\.c", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\\xc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\\.c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "axc"));
}

TEST(rfc9485_matches_if_valid_dialect) {
  EXPECT_TRUE(sourcemeta::core::matches_if_valid(
      "ab*", "abb", sourcemeta::core::RegexDialect::IRegexp));
  EXPECT_FALSE(sourcemeta::core::matches_if_valid(
      "ab*", "xabb", sourcemeta::core::RegexDialect::IRegexp));
  EXPECT_FALSE(sourcemeta::core::matches_if_valid(
      "\\d+", "5", sourcemeta::core::RegexDialect::IRegexp));
}

TEST(rfc9485_invalid_trailing_backslash) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_v) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\v", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_f) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\f", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_b) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\b", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_uppercase_b) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\B", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_d) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\d", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_uppercase_d) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\D", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_s) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\s", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_uppercase_s) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\S", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_w) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\w", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_uppercase_w) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\W", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_uppercase_a) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\Aabc", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_escape_z) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "abc\\z", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_escape_uppercase_z) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "abc\\Z", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_escape_zero) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\0", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_backreference) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "(a)\\1", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_escape_hex) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\x41", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_escape_unicode) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\u0041", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_escape_control) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\ca", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_escape_dollar) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\$", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_slash) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("\\/", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_escape_after_literal_backslash) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\\\\\v", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_odd_backslashes) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\\\\\", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_double_star) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a**", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_lazy_quantifier) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a*?", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_possessive_quantifier) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a*+", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_leading_star) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("*a", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_leading_plus) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("+a", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_quantifier_after_alternation) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a|*", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_bare_range_quantifier) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("{3}", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_quantifier_missing_minimum) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "a{,3}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_quantifier_empty) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a{}", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_quantifier_non_digit) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "a{b}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_quantifier_unterminated) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a{3", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_quantifier_reversed) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "a{3,2}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_quantifier_huge) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a{99999999999999999999}",
                                 sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_quantifier_above_engine_bound) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "a{70000}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_bare_open_brace) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a{b", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_bare_close_brace) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a}b", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_unclosed_group) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("(aa", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_stray_close_paren) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("aa)", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_unclosed_group_alternation) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "aa(bb|ccdd", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_stray_close_paren_alternation) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "aabb|cc)dd", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_non_capturing_group) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "(?:a)", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_lookahead) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "(?=a)", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_negative_lookahead) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "(?!a)", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_lookbehind) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "(?<=a)b", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_named_group) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "(?<year>a)", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_inline_flag) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "(?i)a", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_backtracking_verb) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "(*COMMIT)a", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_group_nesting_overflow) {
  const std::string pattern{std::string(100, '(') + "a" +
                            std::string(100, ')')};
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   pattern, sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_unterminated_class) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("[a", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_stray_close_bracket) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("a]", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_empty_class) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("[]", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_negated_empty_class) {
  EXPECT_FALSE(
      sourcemeta::core::to_regex("[^]", sourcemeta::core::RegexDialect::IRegexp)
          .has_value());
}

TEST(rfc9485_invalid_nested_class) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[[a]]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_posix_class) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[[:alpha:]]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_class_subtraction) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[a-z-[aeiou]]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_class_shorthand_digit) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[\\d]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_class_shorthand_space) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[\\s]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_class_dash_dash_element) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[a--]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_class_element_after_trailing_dash) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[a-b-c]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_class_reversed_range) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[z-a]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_block) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\p{IsBasicLatin}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_script) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\p{Greek}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_surrogate) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\p{Cs}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_empty) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\p{}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\p{Xx}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_lowercase_major) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\p{l}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_no_brace) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\pL", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_unterminated) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\p{Ll", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_category_too_long) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\\p{Llx}", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_utf8_stray_continuation) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\x80", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_utf8_truncated_sequence) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\xF0\x9F", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_utf8_bad_lead) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "\xFF", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_invalid_utf8_in_class) {
  EXPECT_FALSE(sourcemeta::core::to_regex(
                   "[\xF0\x9F]", sourcemeta::core::RegexDialect::IRegexp)
                   .has_value());
}

TEST(rfc9485_matches_quantified_caret) {
  const auto regex{sourcemeta::core::to_regex(
      "^*", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "^"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "^^^"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_quantified_dollar) {
  const auto regex{sourcemeta::core::to_regex(
      "a$?", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a$"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a$$"));
}

TEST(rfc9485_matches_empty_group) {
  const auto regex{sourcemeta::core::to_regex(
      "()", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_matches_quantifier_zero_bounds) {
  const auto regex{sourcemeta::core::to_regex(
      "a{0,0}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(rfc9485_invalid_double_brace_quantifier) {
  const auto regex{sourcemeta::core::to_regex(
      "a{2}{3}", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_FALSE(regex.has_value());
}

TEST(rfc9485_invalid_category_as_range_start) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\p{Lu}-z]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_FALSE(regex.has_value());
}

TEST(rfc9485_invalid_close_bracket_first_in_class) {
  const auto regex{sourcemeta::core::to_regex(
      "[]-a]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_FALSE(regex.has_value());
}

TEST(rfc9485_matches_class_double_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[--]", sourcemeta::core::RegexDialect::IRegexp)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}
