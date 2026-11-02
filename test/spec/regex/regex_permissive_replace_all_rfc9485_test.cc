#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

static auto compile(const std::string &pattern) -> sourcemeta::core::Regex {
  return sourcemeta::core::to_regex(
             pattern, sourcemeta::core::RegexDialect::Permissive, false)
      .value();
}

TEST(rfc9485_replace_all_literal_single_char) {
  const auto regex{compile("a")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "banana", "#"), "b#n#n#");
}

TEST(rfc9485_replace_all_literal_sequence) {
  const auto regex{compile("an")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "banana", "#"), "b##a");
}

TEST(rfc9485_replace_all_dot_wildcard) {
  const auto regex{compile(".")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "#"), "###");
}

TEST(rfc9485_replace_all_quantifier_star) {
  const auto regex{compile("ab*")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "aab", "#"), "##");
}

TEST(rfc9485_replace_all_quantifier_plus) {
  const auto regex{compile("ab+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "ab abb a", "#"), "# # a");
}

TEST(rfc9485_replace_all_quantifier_question) {
  const auto regex{compile("ab?")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a ab", "#"), "# #");
}

TEST(rfc9485_replace_all_quantifier_exact) {
  const auto regex{compile("a{2}")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "aaa", "#"), "#a");
}

TEST(rfc9485_replace_all_quantifier_at_least) {
  const auto regex{compile("a{2,}")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "aaa", "#"), "#");
}

TEST(rfc9485_replace_all_quantifier_range) {
  const auto regex{compile("a{2,3}")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "aaaa", "#"), "#a");
}

TEST(rfc9485_replace_all_alternation_simple) {
  const auto regex{compile("cat|dog")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a cat and a dog", "#"),
            "a # and a #");
}

TEST(rfc9485_replace_all_charclass_simple) {
  const auto regex{compile("[abc]")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abcd", "#"), "###d");
}

TEST(rfc9485_replace_all_charclass_range) {
  const auto regex{compile("[a-c]")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abcd", "#"), "###d");
}

TEST(rfc9485_replace_all_charclass_negated) {
  const auto regex{compile("[^a]")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "#"), "a##");
}

TEST(rfc9485_replace_all_escape_dot) {
  const auto regex{compile("\\.")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a.b.c", "#"), "a#b#c");
}

TEST(rfc9485_replace_all_escape_plus) {
  const auto regex{compile("\\+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a+b", "#"), "a#b");
}

TEST(rfc9485_replace_all_escape_left_paren) {
  const auto regex{compile("\\(")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a(b", "#"), "a#b");
}

TEST(rfc9485_replace_all_escape_left_bracket) {
  const auto regex{compile("\\[")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a[b", "#"), "a#b");
}

TEST(rfc9485_replace_all_escape_tab) {
  const auto regex{compile("\\t")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a\tb", "#"), "a#b");
}

TEST(rfc9485_replace_all_unicode_property_letter) {
  const auto regex{compile("\\p{L}")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1é", "#"), "#1#");
}

TEST(rfc9485_replace_all_unicode_property_decimal_number) {
  const auto regex{compile("\\p{Nd}")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b2", "#"), "a#b#");
}

TEST(rfc9485_replace_all_dollar_is_an_assertion_so_nothing_is_replaced) {
  const auto regex{compile("a$b")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a$b", "#"), "a$b");
}
