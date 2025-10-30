#include <gtest/gtest.h>

#include <sourcemeta/core/regex.h>

#include <string>

TEST(Regex_matches, rfc9485_literal_single_char) {
  const auto regex{sourcemeta::core::to_regex("a")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xyz"));
}

TEST(Regex_matches, rfc9485_literal_sequence) {
  const auto regex{sourcemeta::core::to_regex("hello")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hello"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hello world"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "world"));
}

TEST(Regex_matches, rfc9485_dot_wildcard) {
  const auto regex{sourcemeta::core::to_regex("a.c")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "axc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a1c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ac"));
}

TEST(Regex_matches, rfc9485_quantifier_star) {
  const auto regex{sourcemeta::core::to_regex("ab*c")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ac"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abbc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abbbc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "adc"));
}

TEST(Regex_matches, rfc9485_quantifier_plus) {
  const auto regex{sourcemeta::core::to_regex("ab+c")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abbc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abbbc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ac"));
}

TEST(Regex_matches, rfc9485_quantifier_question) {
  const auto regex{sourcemeta::core::to_regex("ab?c")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ac"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abbc"));
}

TEST(Regex_matches, rfc9485_quantifier_exact) {
  const auto regex{sourcemeta::core::to_regex("a{3}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aa"));
}

TEST(Regex_matches, rfc9485_quantifier_at_least) {
  const auto regex{sourcemeta::core::to_regex("a{2,}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_quantifier_range) {
  const auto regex{sourcemeta::core::to_regex("a{2,4}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_alternation_simple) {
  const auto regex{sourcemeta::core::to_regex("cat|dog")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "cat"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "dog"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bird"));
}

TEST(Regex_matches, rfc9485_alternation_multiple) {
  const auto regex{sourcemeta::core::to_regex("red|green|blue")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "red"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "green"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "blue"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "yellow"));
}

TEST(Regex_matches, rfc9485_alternation_empty_branch) {
  const auto regex{sourcemeta::core::to_regex("a|")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xyz"));
}

TEST(Regex_matches, rfc9485_charclass_simple) {
  const auto regex{sourcemeta::core::to_regex("[abc]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(Regex_matches, rfc9485_charclass_range) {
  const auto regex{sourcemeta::core::to_regex("[a-z]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, rfc9485_charclass_multiple_ranges) {
  const auto regex{sourcemeta::core::to_regex("[a-zA-Z0-9]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "-"));
}

TEST(Regex_matches, rfc9485_charclass_negated) {
  const auto regex{sourcemeta::core::to_regex("[^abc]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, rfc9485_charclass_dash_at_start) {
  const auto regex{sourcemeta::core::to_regex("[-abc]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(Regex_matches, rfc9485_charclass_dash_at_end) {
  const auto regex{sourcemeta::core::to_regex("[abc-]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(Regex_matches, rfc9485_escape_left_paren) {
  const auto regex{sourcemeta::core::to_regex("\\(test")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "(test"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "test"));
}

TEST(Regex_matches, rfc9485_escape_right_paren) {
  const auto regex{sourcemeta::core::to_regex("test\\)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test)"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "test"));
}

TEST(Regex_matches, rfc9485_escape_left_bracket) {
  const auto regex{sourcemeta::core::to_regex("\\[test")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "[test"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "test"));
}

TEST(Regex_matches, rfc9485_escape_right_bracket) {
  const auto regex{sourcemeta::core::to_regex("test\\]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "test"));
}

TEST(Regex_matches, rfc9485_escape_left_brace) {
  const auto regex{sourcemeta::core::to_regex("\\{test")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "{test"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "test"));
}

TEST(Regex_matches, rfc9485_escape_right_brace) {
  const auto regex{sourcemeta::core::to_regex("test\\}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test}"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "test"));
}

TEST(Regex_matches, rfc9485_escape_asterisk) {
  const auto regex{sourcemeta::core::to_regex("a\\*b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a*b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aab"));
}

TEST(Regex_matches, rfc9485_escape_plus) {
  const auto regex{sourcemeta::core::to_regex("a\\+b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a+b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_escape_question) {
  const auto regex{sourcemeta::core::to_regex("a\\?b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a?b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_escape_dot) {
  const auto regex{sourcemeta::core::to_regex("a\\.b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a.b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "axb"));
}

TEST(Regex_matches, rfc9485_escape_dash) {
  const auto regex{sourcemeta::core::to_regex("a\\-b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a-b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_escape_newline) {
  const auto regex{sourcemeta::core::to_regex("a\\nb")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\nb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "anb"));
}

TEST(Regex_matches, rfc9485_escape_carriage_return) {
  const auto regex{sourcemeta::core::to_regex("a\\rb")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\rb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "arb"));
}

TEST(Regex_matches, rfc9485_escape_tab) {
  const auto regex{sourcemeta::core::to_regex("a\\tb")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\tb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "atb"));
}

TEST(Regex_matches, rfc9485_unicode_property_letter) {
  const auto regex{sourcemeta::core::to_regex("\\p{L}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "é"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "中"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!"));
}

TEST(Regex_matches, rfc9485_unicode_property_uppercase_letter) {
  const auto regex{sourcemeta::core::to_regex("\\p{Lu}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, rfc9485_unicode_property_lowercase_letter) {
  const auto regex{sourcemeta::core::to_regex("\\p{Ll}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, rfc9485_unicode_property_titlecase_letter) {
  const auto regex{sourcemeta::core::to_regex("\\p{Lt}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u01C5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_modifier_letter) {
  const auto regex{sourcemeta::core::to_regex("\\p{Lm}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u02B0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_other_letter) {
  const auto regex{sourcemeta::core::to_regex("\\p{Lo}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "中"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_mark) {
  const auto regex{sourcemeta::core::to_regex("\\p{M}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0300"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_nonspacing_mark) {
  const auto regex{sourcemeta::core::to_regex("\\p{Mn}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0300"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_spacing_mark) {
  const auto regex{sourcemeta::core::to_regex("\\p{Mc}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0903"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_enclosing_mark) {
  const auto regex{sourcemeta::core::to_regex("\\p{Me}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0488"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_number) {
  const auto regex{sourcemeta::core::to_regex("\\p{N}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0660"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_decimal_number) {
  const auto regex{sourcemeta::core::to_regex("\\p{Nd}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0660"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_letter_number) {
  const auto regex{sourcemeta::core::to_regex("\\p{Nl}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u2160"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, rfc9485_unicode_property_other_number) {
  const auto regex{sourcemeta::core::to_regex("\\p{No}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u00B2"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, rfc9485_unicode_property_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\p{P}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "."));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ","));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_connector_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\p{Pc}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "."));
}

TEST(Regex_matches, rfc9485_unicode_property_dash_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\p{Pd}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "."));
}

TEST(Regex_matches, rfc9485_unicode_property_open_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\p{Ps}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "("));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "["));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ")"));
}

TEST(Regex_matches, rfc9485_unicode_property_close_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\p{Pe}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ")"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "("));
}

TEST(Regex_matches, rfc9485_unicode_property_initial_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\p{Pi}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u00AB"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "."));
}

TEST(Regex_matches, rfc9485_unicode_property_final_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\p{Pf}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u00BB"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "."));
}

TEST(Regex_matches, rfc9485_unicode_property_other_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\p{Po}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "."));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "("));
}

TEST(Regex_matches, rfc9485_unicode_property_separator) {
  const auto regex{sourcemeta::core::to_regex("\\p{Z}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u00A0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_space_separator) {
  const auto regex{sourcemeta::core::to_regex("\\p{Zs}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u00A0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\n"));
}

TEST(Regex_matches, rfc9485_unicode_property_line_separator) {
  const auto regex{sourcemeta::core::to_regex("\\p{Zl}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u2028"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
}

TEST(Regex_matches, rfc9485_unicode_property_paragraph_separator) {
  const auto regex{sourcemeta::core::to_regex("\\p{Zp}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u2029"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
}

TEST(Regex_matches, rfc9485_unicode_property_symbol) {
  const auto regex{sourcemeta::core::to_regex("\\p{S}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "$"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "+"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "©"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_math_symbol) {
  const auto regex{sourcemeta::core::to_regex("\\p{Sm}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "+"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "="));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "$"));
}

TEST(Regex_matches, rfc9485_unicode_property_currency_symbol) {
  const auto regex{sourcemeta::core::to_regex("\\p{Sc}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "$"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "€"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "+"));
}

TEST(Regex_matches, rfc9485_unicode_property_modifier_symbol) {
  const auto regex{sourcemeta::core::to_regex("\\p{Sk}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "^"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "$"));
}

TEST(Regex_matches, rfc9485_unicode_property_other_symbol) {
  const auto regex{sourcemeta::core::to_regex("\\p{So}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "©"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "$"));
}

TEST(Regex_matches, rfc9485_unicode_property_other) {
  const auto regex{sourcemeta::core::to_regex("\\p{C}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string(1, '\0')));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_control) {
  const auto regex{sourcemeta::core::to_regex("\\p{Cc}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string(1, '\0')));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x1F"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_format) {
  const auto regex{sourcemeta::core::to_regex("\\p{Cf}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u00AD"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_unassigned) {
  const auto regex{sourcemeta::core::to_regex("\\p{Cn}")};
  EXPECT_TRUE(regex.has_value());
}

TEST(Regex_matches, rfc9485_unicode_property_private_use) {
  const auto regex{sourcemeta::core::to_regex("\\p{Co}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\uE000"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_complement_letter) {
  const auto regex{sourcemeta::core::to_regex("\\P{L}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_unicode_property_complement_number) {
  const auto regex{sourcemeta::core::to_regex("\\P{N}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, rfc9485_unicode_property_complement_punctuation) {
  const auto regex{sourcemeta::core::to_regex("\\P{P}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!"));
}

TEST(Regex_matches, rfc9485_group_simple) {
  const auto regex{sourcemeta::core::to_regex("(ab)c")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ac"));
}

TEST(Regex_matches, rfc9485_group_nested) {
  const auto regex{sourcemeta::core::to_regex("((ab)c)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ac"));
}

TEST(Regex_matches, rfc9485_group_with_quantifier) {
  const auto regex{sourcemeta::core::to_regex("(ab)+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ababab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_group_with_alternation) {
  const auto regex{sourcemeta::core::to_regex("(cat|dog)s")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "cats"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "dogs"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "birds"));
}

TEST(Regex_matches, rfc9485_complex_email_like) {
  const auto regex{sourcemeta::core::to_regex("[a-z]+@[a-z]+\\.[a-z]+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "user@example.com"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test@test.org"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "invalid"));
}

TEST(Regex_matches, rfc9485_complex_identifier) {
  const auto regex{sourcemeta::core::to_regex("[a-zA-Z_][a-zA-Z0-9_]*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "variable1"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_private"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "CONSTANT"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123invalid"));
}

TEST(Regex_matches, rfc9485_complex_unicode_word) {
  const auto regex{sourcemeta::core::to_regex("\\p{L}+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hello"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "世界"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "مرحبا"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123"));
}

TEST(Regex_matches, rfc9485_complex_mixed_quantifiers) {
  const auto regex{sourcemeta::core::to_regex("a{2,3}b+c*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aabbb"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaabbcc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_complex_nested_groups_quantifiers) {
  const auto regex{sourcemeta::core::to_regex("((ab)+c)*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ababc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abcabc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_edge_empty_pattern) {
  const auto regex{sourcemeta::core::to_regex("")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "anything"));
}

TEST(Regex_matches, rfc9485_edge_alternation_empty_first) {
  const auto regex{sourcemeta::core::to_regex("|abc")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xyz"));
}

TEST(Regex_matches, rfc9485_edge_alternation_empty_middle) {
  const auto regex{sourcemeta::core::to_regex("a||b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xyz"));
}

TEST(Regex_matches, rfc9485_edge_alternation_all_empty) {
  const auto regex{sourcemeta::core::to_regex("||")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "anything"));
}

TEST(Regex_matches, rfc9485_edge_escape_backslash) {
  const auto regex{sourcemeta::core::to_regex("a\\\\b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\\b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_edge_escape_pipe) {
  const auto regex{sourcemeta::core::to_regex("a\\|b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a|b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_edge_caret_literal) {
  const auto regex{sourcemeta::core::to_regex("a\\^b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a^b"));
}

TEST(Regex_matches, rfc9485_edge_dollar_literal) {
  const auto regex{sourcemeta::core::to_regex("a$b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a$b"));
}

TEST(Regex_matches, rfc9485_edge_quantifier_zero_exact) {
  const auto regex{sourcemeta::core::to_regex("a{0}b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_edge_quantifier_zero_or_more) {
  const auto regex{sourcemeta::core::to_regex("a{0,}b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaab"));
}

TEST(Regex_matches, rfc9485_edge_quantifier_zero_to_n) {
  const auto regex{sourcemeta::core::to_regex("a{0,3}b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaab"));
}

TEST(Regex_matches, rfc9485_edge_nested_groups_deep) {
  const auto regex{sourcemeta::core::to_regex("(((a)))")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, rfc9485_edge_charclass_only_dash) {
  const auto regex{sourcemeta::core::to_regex("[-]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_edge_charclass_dash_both_ends) {
  const auto regex{sourcemeta::core::to_regex("[-a-]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, rfc9485_edge_charclass_mixed_content) {
  const auto regex{sourcemeta::core::to_regex("[a-z\\p{N}._-]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "."));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(Regex_matches, rfc9485_edge_charclass_escaped_bracket) {
  const auto regex{sourcemeta::core::to_regex("[\\[\\]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "["));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_edge_charclass_multiple_ranges) {
  const auto regex{sourcemeta::core::to_regex("[a-cA-C0-2]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "B"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "C"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "2"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "D"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "3"));
}

TEST(Regex_matches, rfc9485_edge_group_with_alternation_quantified) {
  const auto regex{sourcemeta::core::to_regex("(a|b){2}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ba"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_edge_multiple_dots) {
  const auto regex{sourcemeta::core::to_regex("...")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\nb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, rfc9485_edge_quantifier_large_range) {
  const auto regex{sourcemeta::core::to_regex("a{100,200}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string(100, 'a')));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string(150, 'a')));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string(200, 'a')));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), std::string(99, 'a')));
}

TEST(Regex_matches, rfc9485_edge_alternation_with_groups) {
  const auto regex{sourcemeta::core::to_regex("(abc)|(def)|(ghi)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "def"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ghi"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "jkl"));
}

TEST(Regex_matches, rfc9485_edge_charclass_with_unicode_properties) {
  const auto regex{sourcemeta::core::to_regex("[\\p{Lu}\\p{Nd}]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, rfc9485_all_major_categories_combined) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\p{L}\\p{M}\\p{N}\\p{P}\\p{Z}\\p{S}\\p{C}]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "$"));
}

TEST(Regex_matches, rfc9485_complement_all_major_categories) {
  const auto regex{sourcemeta::core::to_regex("\\P{L}\\P{N}\\P{P}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "   "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a  "));
}

TEST(Regex_matches, rfc9485_edge_nested_quantifiers_group) {
  const auto regex{sourcemeta::core::to_regex("((a+)+)+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, rfc9485_edge_charclass_negated_with_range) {
  const auto regex{sourcemeta::core::to_regex("[^a-z]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, rfc9485_edge_consecutive_alternations) {
  const auto regex{sourcemeta::core::to_regex("a|b|c|d|e")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "f"));
}

TEST(Regex_matches, rfc9485_edge_group_quantifier_zero_or_one) {
  const auto regex{sourcemeta::core::to_regex("(abc)?")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xyz"));
}

TEST(Regex_matches, rfc9485_edge_mixed_quantifiers_sequence) {
  const auto regex{sourcemeta::core::to_regex("a?b+c*d{2}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bdd"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abdd"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abbcdd"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abbccccdd"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ad"));
}

TEST(Regex_matches, rfc9485_edge_unicode_in_alternation) {
  const auto regex{sourcemeta::core::to_regex("\\p{Lu}|\\p{Ll}|\\p{Nd}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!"));
}

TEST(Regex_matches, rfc9485_edge_charclass_single_char) {
  const auto regex{sourcemeta::core::to_regex("[a]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, rfc9485_edge_complex_real_world_email_subset) {
  const auto regex{sourcemeta::core::to_regex(
      "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-z]{2,}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test@example.com"));
  EXPECT_TRUE(
      sourcemeta::core::matches(regex.value(), "user.name+tag@example.co.uk"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "@example.com"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "test@"));
}

TEST(Regex_matches, rfc9485_edge_complex_real_world_hex_color) {
  const auto regex{sourcemeta::core::to_regex("#[0-9a-fA-F]{6}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "#FF5733"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "#000000"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "#ffffff"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "#GGG"));
}

TEST(Regex_matches, rfc9485_edge_complex_real_world_uuid_partial) {
  const auto regex{sourcemeta::core::to_regex(
      "[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(
      regex.value(), "550e8400-e29b-41d4-a716-446655440000"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "invalid-uuid"));
}

TEST(Regex_matches, rfc9485_preprocessing_literal_backslash_p) {
  const auto regex{sourcemeta::core::to_regex("\\\\p\\{L\\}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\p{L}"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(Regex_matches, rfc9485_preprocessing_literal_backslash_P) {
  const auto regex{sourcemeta::core::to_regex("\\\\P\\{N\\}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\P{N}"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, rfc9485_preprocessing_mixed_escaped_and_unicode) {
  const auto regex{sourcemeta::core::to_regex("\\\\p\\{L\\} \\p{L}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\p{L} A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A A"));
}

TEST(Regex_matches, rfc9485_preprocessing_escaped_backslash_dollar) {
  const auto regex{sourcemeta::core::to_regex("\\\\$foo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\$foo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "$foo"));
}

TEST(Regex_matches, rfc9485_quantifier_star_any_string) {
  const auto regex{sourcemeta::core::to_regex(".*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(Regex_matches, rfc9485_quantifier_plus_non_empty) {
  const auto regex{sourcemeta::core::to_regex(".+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(Regex_matches, rfc9485_dot_matches_newline) {
  const auto regex{sourcemeta::core::to_regex(".")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
}

TEST(Regex_matches, rfc9485_dot_matches_carriage_return) {
  const auto regex{sourcemeta::core::to_regex(".")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\r"));
}

TEST(Regex_matches, rfc9485_unicode_range_4byte_deseret) {
  const auto regex{sourcemeta::core::to_regex("[\\u{10400}-\\u{1044F}]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U00010400"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U0001044F"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}
