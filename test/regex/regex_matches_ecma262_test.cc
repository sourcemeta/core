#include <gtest/gtest.h>

#include <sourcemeta/core/regex.h>

#include <string>

TEST(Regex_matches, ecma262_anchor_start) {
  const auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "barfoo"));
}

TEST(Regex_matches, ecma262_anchor_end) {
  const auto regex{sourcemeta::core::to_regex("foo$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "barfoo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, ecma262_anchor_both) {
  const auto regex{sourcemeta::core::to_regex("^foo$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "barfoo"));
}

TEST(Regex_matches, ecma262_anchor_start_with_alternation) {
  const auto regex{sourcemeta::core::to_regex("^(foo|bar)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "baz"));
}

TEST(Regex_matches, ecma262_digit_escape) {
  const auto regex{sourcemeta::core::to_regex("\\d+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_non_digit_escape) {
  const auto regex{sourcemeta::core::to_regex("\\D+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123"));
}

TEST(Regex_matches, ecma262_word_escape) {
  const auto regex{sourcemeta::core::to_regex("\\w+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ABC"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a_b_c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!!!"));
}

TEST(Regex_matches, ecma262_non_word_escape) {
  const auto regex{sourcemeta::core::to_regex("\\W+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!!!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "   "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_whitespace_escape) {
  const auto regex{sourcemeta::core::to_regex("\\s+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\r"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_non_whitespace_escape) {
  const auto regex{sourcemeta::core::to_regex("\\S+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "   "));
}

TEST(Regex_matches, ecma262_word_boundary_start) {
  const auto regex{sourcemeta::core::to_regex("\\bfoo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo bar"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " foo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xfoo"));
}

TEST(Regex_matches, ecma262_word_boundary_end) {
  const auto regex{sourcemeta::core::to_regex("foo\\b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bar foo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foox"));
}

TEST(Regex_matches, ecma262_word_boundary_both) {
  const auto regex{sourcemeta::core::to_regex("\\bfoo\\b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo bar"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bar foo baz"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, ecma262_non_word_boundary) {
  const auto regex{sourcemeta::core::to_regex("\\Boo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "oo"));
}

TEST(Regex_matches, ecma262_hex_escape_2digit) {
  const auto regex{sourcemeta::core::to_regex("\\x41")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
}

TEST(Regex_matches, ecma262_hex_escape_in_pattern) {
  const auto regex{sourcemeta::core::to_regex("a\\x20b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, ecma262_unicode_escape_4digit) {
  const auto regex{sourcemeta::core::to_regex("\\u0041")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
}

TEST(Regex_matches, ecma262_unicode_escape_brace) {
  const auto regex{sourcemeta::core::to_regex("\\u{1F600}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "😀"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(Regex_matches, ecma262_null_escape) {
  const auto regex{sourcemeta::core::to_regex("a\\0b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string("a\0b", 3)));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, ecma262_non_capturing_group) {
  const auto regex{sourcemeta::core::to_regex("(?:abc)+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abcabc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, ecma262_lookahead_positive) {
  const auto regex{sourcemeta::core::to_regex("foo(?=bar)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foobaz"));
}

TEST(Regex_matches, ecma262_lookahead_negative) {
  const auto regex{sourcemeta::core::to_regex("foo(?!bar)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobaz"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, ecma262_lookbehind_positive) {
  const auto regex{sourcemeta::core::to_regex("(?<=foo)bar")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bazbar"));
}

TEST(Regex_matches, ecma262_lookbehind_negative) {
  const auto regex{sourcemeta::core::to_regex("(?<!foo)bar")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bazbar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, ecma262_combined_digit_word_anchors) {
  const auto regex{sourcemeta::core::to_regex("^\\w+@\\w+\\.\\w+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "user@example.com"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "invalid"));
}

TEST(Regex_matches, ecma262_digit_range_pattern) {
  const auto regex{sourcemeta::core::to_regex("\\d{3}-\\d{2}-\\d{4}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123-45-6789"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc-de-fghi"));
}

TEST(Regex_matches, ecma262_mixed_escapes) {
  const auto regex{sourcemeta::core::to_regex("\\w+\\s+\\d+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hello 123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test  456"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "hello"));
}

TEST(Regex_matches, ecma262_anchor_with_digit) {
  const auto regex{sourcemeta::core::to_regex("^\\d+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123abc"));
}

TEST(Regex_matches, ecma262_word_boundary_with_digit) {
  const auto regex{sourcemeta::core::to_regex("\\b\\d{3}\\b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc 123 def"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "1234"));
}

TEST(Regex_matches, ecma262_whitespace_variants) {
  const auto regex{sourcemeta::core::to_regex("a\\sb")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\tb"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\nb"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\rb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, ecma262_complex_email_pattern) {
  const auto regex{
      sourcemeta::core::to_regex("^[\\w.+-]+@[\\w.-]+\\.[a-zA-Z]{2,}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "user@example.com"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(),
                                        "test.user+tag@sub.example.org"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "@example.com"));
}

TEST(Regex_matches, ecma262_complex_url_pattern) {
  const auto regex{
      sourcemeta::core::to_regex("^https?://[\\w.-]+\\.[a-z]{2,}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "http://example.com"));
  EXPECT_TRUE(
      sourcemeta::core::matches(regex.value(), "https://sub.example.org"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ftp://example.com"));
}

TEST(Regex_matches, ecma262_phone_number_pattern) {
  const auto regex{
      sourcemeta::core::to_regex("^\\(\\d{3}\\)\\s*\\d{3}-\\d{4}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "(123) 456-7890"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "(123)456-7890"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123-456-7890"));
}

TEST(Regex_matches, ecma262_hex_color_with_anchor) {
  const auto regex{sourcemeta::core::to_regex("^#[0-9a-fA-F]{6}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "#FF5733"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "#000000"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "#FF57"));
}

TEST(Regex_matches, ecma262_non_capturing_with_alternation) {
  const auto regex{sourcemeta::core::to_regex("(?:red|green|blue)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "red"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "green"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "blue"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "yellow"));
}

TEST(Regex_matches, ecma262_lookahead_with_quantifier) {
  const auto regex{sourcemeta::core::to_regex("\\d+(?=px)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "100px"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "50px"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "100em"));
}

TEST(Regex_matches, ecma262_multiple_word_boundaries) {
  const auto regex{sourcemeta::core::to_regex("\\btest\\b.*\\bcase\\b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test case"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test this case"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "testing cases"));
}

TEST(Regex_matches, ecma262_unicode_with_anchors) {
  const auto regex{sourcemeta::core::to_regex("^\\u{1F600}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "😀"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "😀😀"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A😀"));
}

TEST(Regex_matches, ecma262_word_ascii_only_in_charclass) {
  const auto regex{sourcemeta::core::to_regex("^[\\w.-]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hello-world"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "test.file"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "café"));
}

TEST(Regex_matches, ecma262_digit_ascii_only_in_charclass) {
  const auto regex{sourcemeta::core::to_regex("^[\\d.-]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123-456"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u07C0"));
}

TEST(Regex_matches, ecma262_whitespace_in_charclass) {
  const auto regex{sourcemeta::core::to_regex("^[\\s]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " \t\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_dollar_anchor_no_trailing_newline) {
  const auto regex{sourcemeta::core::to_regex("^abc$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc\n"));
}

TEST(Regex_matches, ecma262_tab_escape) {
  const auto regex{sourcemeta::core::to_regex("^\\t$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\t"));
}

TEST(Regex_matches, ecma262_control_escape_upper) {
  const auto regex{sourcemeta::core::to_regex("^\\cC$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0003"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\cC"));
}

TEST(Regex_matches, ecma262_control_escape_lower) {
  const auto regex{sourcemeta::core::to_regex("^\\cc$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0003"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\cc"));
}

TEST(Regex_matches, ecma262_whitespace_unicode_nbsp) {
  const auto regex{sourcemeta::core::to_regex("^\\s$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u000b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u000c"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u00a0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\ufeff"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u000a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u2029"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u2003"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u0001"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u2013"));
}

TEST(Regex_matches, ecma262_non_whitespace_unicode) {
  const auto regex{sourcemeta::core::to_regex("^\\S$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u000b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u000c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u00a0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\ufeff"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u000a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u2029"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u2003"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0001"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u2013"));
}

TEST(Regex_matches, ecma262_digit_not_bengali) {
  const auto regex{sourcemeta::core::to_regex("^\\d+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "42"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "-%#"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "৪২"));
}

TEST(Regex_matches, ecma262_unicode_digit_property) {
  const auto regex{sourcemeta::core::to_regex("^\\p{Nd}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "42"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "-%#"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "৪২"));
}

TEST(Regex_matches, ecma262_unicode_digit_property_lowercase) {
  const auto regex{sourcemeta::core::to_regex("^\\p{digit}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "42"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "-%#"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "৪২"));
}

TEST(Regex_matches, ecma262_unicode_space_property_lowercase) {
  const auto regex{sourcemeta::core::to_regex("^\\p{space}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " \t"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_unicode_ASCII_property) {
  const auto regex{sourcemeta::core::to_regex("^\\p{ASCII}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc123"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "café"));
}

TEST(Regex_matches, ecma262_unicode_Hex_Digit_property) {
  const auto regex{sourcemeta::core::to_regex("^\\p{Hex_Digit}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      sourcemeta::core::matches(regex.value(), "0123456789abcdefABCDEF"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xyz"));
}

TEST(Regex_matches, ecma262_unicode_Alphabetic_property) {
  const auto regex{sourcemeta::core::to_regex("^\\p{Alphabetic}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ABC"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123"));
}

TEST(Regex_matches, ecma262_unicode_White_Space_property) {
  const auto regex{sourcemeta::core::to_regex("^\\p{White_Space}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " \t\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_escaped_backslash_before_charclass) {
  const auto regex{sourcemeta::core::to_regex("\\\\[abc]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "[abc]"));
}

TEST(Regex_matches, ecma262_escaped_backslash_before_digit_class) {
  const auto regex{sourcemeta::core::to_regex("\\\\d")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, ecma262_digit_hyphen_in_charclass) {
  const auto regex{sourcemeta::core::to_regex("^[\\d-]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1-2-3"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "---"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ":"));
}

TEST(Regex_matches, ecma262_word_boundary_ascii_only) {
  const auto regex{sourcemeta::core::to_regex("\\bcafé\\b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "café"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bon café"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "café123"));
}

TEST(Regex_matches, ecma262_non_greedy_quantifiers) {
  const auto regex{sourcemeta::core::to_regex("^a+?b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_quantifier_exact) {
  const auto regex{sourcemeta::core::to_regex("^a{3}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aaaa"));
}

TEST(Regex_matches, ecma262_quantifier_range) {
  const auto regex{sourcemeta::core::to_regex("^a{2,4}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aaaaa"));
}

TEST(Regex_matches, ecma262_quantifier_min_only) {
  const auto regex{sourcemeta::core::to_regex("^a{2,}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaaaaa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_negated_charclass) {
  const auto regex{sourcemeta::core::to_regex("^[^abc]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xyz"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xaz"));
}

TEST(Regex_matches, ecma262_escaped_metacharacters) {
  const auto regex{sourcemeta::core::to_regex("^\\.\\.\\.$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "..."));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_hex_escape_hi) {
  const auto regex{sourcemeta::core::to_regex("^\\x48\\x69$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Hi"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\x48\\x69"));
}

TEST(Regex_matches, ecma262_unicode_property_letter_all) {
  const auto regex{sourcemeta::core::to_regex("^\\p{L}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ABC"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "café"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "中文"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123"));
}

TEST(Regex_matches, ecma262_unicode_property_letter_uppercase) {
  const auto regex{sourcemeta::core::to_regex("^\\p{Lu}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ABC"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "Abc"));
}

TEST(Regex_matches, ecma262_unicode_property_letter_lowercase) {
  const auto regex{sourcemeta::core::to_regex("^\\p{Ll}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ABC"));
}

TEST(Regex_matches, ecma262_unicode_property_mark) {
  const auto regex{sourcemeta::core::to_regex("\\p{M}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\u0301"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "é"));
}

TEST(Regex_matches, ecma262_unicode_property_number) {
  const auto regex{sourcemeta::core::to_regex("^\\p{N}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "৪২"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_unicode_property_punctuation) {
  const auto regex{sourcemeta::core::to_regex("^\\p{P}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!.,;"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_unicode_property_symbol) {
  const auto regex{sourcemeta::core::to_regex("\\p{S}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "$"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "+"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "€"));
}

TEST(Regex_matches, ecma262_unicode_property_separator) {
  const auto regex{sourcemeta::core::to_regex("\\p{Z}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u00A0"));
}

TEST(Regex_matches, ecma262_unicode_property_other) {
  const auto regex{sourcemeta::core::to_regex("\\p{C}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string(1, '\0')));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0001"));
}

TEST(Regex_matches, ecma262_unicode_property_negated_letter) {
  const auto regex{sourcemeta::core::to_regex("^\\P{L}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!@#"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_unicode_property_negated_number) {
  const auto regex{sourcemeta::core::to_regex("^\\P{N}+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "৪২"));
}

TEST(Regex_matches, ecma262_quantifier_star_any_string) {
  const auto regex{sourcemeta::core::to_regex(".*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(Regex_matches, ecma262_quantifier_plus_non_empty) {
  const auto regex{sourcemeta::core::to_regex(".+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(Regex_matches, ecma262_dot_matches_newline) {
  const auto regex{sourcemeta::core::to_regex(".")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
}

TEST(Regex_matches, ecma262_dot_matches_carriage_return) {
  const auto regex{sourcemeta::core::to_regex(".")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\r"));
}

TEST(Regex_matches, ecma262_unicode_range_4byte_deseret) {
  const auto regex{sourcemeta::core::to_regex("[\\u{10400}-\\u{1044F}]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U00010400"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U0001044F"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(Regex_matches, ecma262_anchor_start_prefix_match) {
  const auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bar"));
}

TEST(Regex_matches, ecma262_anchor_both_full_match) {
  const auto regex{sourcemeta::core::to_regex("^.*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, ecma262_anchor_both_with_group) {
  const auto regex{sourcemeta::core::to_regex("^(.*)$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foobar"));
}

TEST(Regex_matches, ecma262_anchor_start_slash_path) {
  const auto regex{sourcemeta::core::to_regex("^/.*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "/foo/bar"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "/"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foo/bar"));
}

TEST(Regex_matches, ecma262_anchor_both_length_range) {
  const auto regex{sourcemeta::core::to_regex("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "f"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string(256, 'x')));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), std::string(257, 'x')));
}

TEST(Regex_matches, ecma262_anchor_start_at_sign) {
  const auto regex{sourcemeta::core::to_regex("^@")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "@foo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foo"));
}

TEST(Regex_matches, ecma262_anchor_end_letter_o) {
  const auto regex{sourcemeta::core::to_regex("o$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bar"));
}

TEST(Regex_matches, ecma262_anchor_both_identifier_pattern) {
  const auto regex{sourcemeta::core::to_regex("^[a-z][a-z0-9-_]{1,63}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abcd"));
}

TEST(Regex_matches, ecma262_anchor_both_package_name) {
  const auto regex{
      sourcemeta::core::to_regex("^(?:@[0-9a-z-_.]+\\/)?[a-z][0-9a-z-_.]*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "@namespace/mypackage"));
}

TEST(Regex_matches, ecma262_anchor_both_multiline_string) {
  const auto regex{sourcemeta::core::to_regex("^.+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo\nbar\r"));
}

TEST(Regex_matches, ecma262_unicode_escape_4digit_arabic_digit_range) {
  const auto regex{sourcemeta::core::to_regex("[\\u0660-\\u0669]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0660"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\u0669"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_unicode_property_letter_identifier) {
  const auto regex{
      sourcemeta::core::to_regex("^\\p{Letter}[\\p{Letter}\\p{Number}]*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hello"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "hello123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Àlement"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "中文"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U00010400test"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123hello"));
}

TEST(Regex_matches, ecma262_unicode_property_negative_lookahead_identifier) {
  const auto regex{sourcemeta::core::to_regex(
      "^(?!\\p{Number})\\p{Letter}[\\p{Letter}\\p{Number}\\-_.]*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "element123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "element٠"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "٠element"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0element"));
}

TEST(Regex_matches, ecma262_unicode_dot_single_codepoint) {
  const auto regex{sourcemeta::core::to_regex("^.$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "À"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "中"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U00010400"));
}

TEST(Regex_matches, ecma262_unicode_quantifier_three_codepoints) {
  const auto regex{sourcemeta::core::to_regex("^.{3}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ABC"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ÀÁÂ"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "中文字"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(),
                                        "\U00010400\U00010401\U00010402"));
}

TEST(Regex_matches, ecma262_digit_class_ascii_only) {
  const auto regex{sourcemeta::core::to_regex("^\\d$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\u07C0"));
}

TEST(Regex_matches, ecma262_word_class_ascii_only) {
  const auto regex{sourcemeta::core::to_regex("^\\w$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "é"));
}

TEST(Regex_matches, ecma262_nonbmp_emoji_quantifier_codepoint) {
  const auto regex{sourcemeta::core::to_regex("^\U0001F432*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U0001F432"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U0001F432\U0001F432"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\U0001F409"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "D"));
}

TEST(Regex_matches, ecma262_nonbmp_literal_exact_match) {
  const auto regex{sourcemeta::core::to_regex("^\U0001F432$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U0001F432"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\U0001F409"));
  EXPECT_FALSE(
      sourcemeta::core::matches(regex.value(), "\U0001F432\U0001F432"));
}

TEST(Regex_matches, ecma262_xml_ncname_unicode_identifier) {
  const auto regex{sourcemeta::core::to_regex(
      "^(?![:\\p{Nd}])[\\p{L}_][\\p{L}\\p{Nd}\\-._·]*$")};
  EXPECT_TRUE(regex.has_value());

  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "element"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_element"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "element123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "élément"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "element٠"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\U00010400element"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ":element"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0element"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "٠element"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\U000104A0element"));
}

TEST(Regex_matches, ecma262_escaped_backslash_before_bracket) {
  const auto regex{sourcemeta::core::to_regex("\\\\[abc]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_triple_backslash_before_bracket) {
  const auto regex{sourcemeta::core::to_regex("\\\\\\[")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\["));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "["));
}

TEST(Regex_matches, ecma262_dollar_in_middle_never_matches) {
  const auto regex{sourcemeta::core::to_regex("foo$bar")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foo$bar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foobar"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foo"));
}

TEST(Regex_matches, ecma262_dollar_in_character_class) {
  const auto regex{sourcemeta::core::to_regex("[$]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "$"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_escaped_dollar) {
  const auto regex{sourcemeta::core::to_regex("\\$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "$"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_multiple_dollars) {
  const auto regex{sourcemeta::core::to_regex("$$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "anything"));
}

TEST(Regex_matches, ecma262_negated_digit_in_char_class) {
  const auto regex{sourcemeta::core::to_regex("[\\D]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, ecma262_negated_word_in_char_class) {
  const auto regex{sourcemeta::core::to_regex("[\\W]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "@"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "_"));
}

TEST(Regex_matches, ecma262_negated_space_in_char_class) {
  const auto regex{sourcemeta::core::to_regex("[\\S]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\n"));
}

TEST(Regex_matches, ecma262_mixed_escapes_in_char_class) {
  const auto regex{sourcemeta::core::to_regex("[\\d\\D]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
}

TEST(Regex_matches, ecma262_escaped_bracket_in_char_class) {
  const auto regex{sourcemeta::core::to_regex("[\\]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "["));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_backslash_in_char_class) {
  const auto regex{sourcemeta::core::to_regex("[\\\\]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_complex_char_class_with_escaped_chars) {
  const auto regex{sourcemeta::core::to_regex("[a\\]b\\\\c]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "["));
}

TEST(Regex_matches, ecma262_empty_char_class) {
  const auto regex{sourcemeta::core::to_regex("[]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_incomplete_unicode_property) {
  const auto regex{sourcemeta::core::to_regex("\\p{Letter")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_incomplete_unicode_escape) {
  const auto regex{sourcemeta::core::to_regex("\\u{DEAD")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_backslash_at_end) {
  const auto regex{sourcemeta::core::to_regex("foo\\")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_nested_char_class_attempt) {
  const auto regex{sourcemeta::core::to_regex("[[abc]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "["));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "]"));
}

TEST(Regex_matches, ecma262_dollar_before_alternation) {
  const auto regex{sourcemeta::core::to_regex("foo$|bar")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bar"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "barxyz"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "foox"));
}

TEST(Regex_matches, ecma262_multiple_backslashes_complex) {
  const auto regex{sourcemeta::core::to_regex("\\\\\\\\\\\\\\\\")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\\\\\\\"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\\\"));
}

TEST(Regex_matches, ecma262_right_bracket_as_first_char_in_class) {
  const auto regex{sourcemeta::core::to_regex("[]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "["));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_right_bracket_negated_class) {
  const auto regex{sourcemeta::core::to_regex("[^]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "["));
}

TEST(Regex_matches, ecma262_dollar_before_opening_paren) {
  const auto regex{sourcemeta::core::to_regex("$(abc)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "$abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_caret_not_at_start_of_class) {
  const auto regex{sourcemeta::core::to_regex("[a^b]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "^"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
}

TEST(Regex_matches, ecma262_caret_in_middle_of_pattern) {
  const auto regex{sourcemeta::core::to_regex("a^b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a^b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_backslash_before_digit_escape) {
  const auto regex{sourcemeta::core::to_regex("\\\\\\d")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_dash_at_end_of_class) {
  const auto regex{sourcemeta::core::to_regex("[abc-]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(Regex_matches, ecma262_dash_at_start_of_class) {
  const auto regex{sourcemeta::core::to_regex("[-abc]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(Regex_matches, ecma262_double_caret) {
  const auto regex{sourcemeta::core::to_regex("^^abc")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abcdef"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "^abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xabc"));
}

TEST(Regex_matches, ecma262_caret_after_alternation) {
  const auto regex{sourcemeta::core::to_regex("a|^b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xb"));
}

TEST(Regex_matches, ecma262_caret_after_opening_paren) {
  const auto regex{sourcemeta::core::to_regex("(^abc)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xabc"));
}

TEST(Regex_matches, ecma262_dollar_then_caret) {
  const auto regex{sourcemeta::core::to_regex("a$^b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a$^b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, ecma262_backspace_in_char_class) {
  const auto regex{sourcemeta::core::to_regex("[\\b]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_whitespace_and_non_whitespace_class) {
  const auto regex{sourcemeta::core::to_regex("[\\s\\S]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
}

TEST(Regex_matches, ecma262_word_boundary_outside_class) {
  const auto regex{sourcemeta::core::to_regex("\\bword\\b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "word"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " word "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "sword"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "words"));
}

TEST(Regex_matches, ecma262_unmatched_closing_paren) {
  const auto regex{sourcemeta::core::to_regex("abc)")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_unmatched_opening_paren) {
  const auto regex{sourcemeta::core::to_regex("(abc")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_quantifier_on_assertion) {
  const auto regex{sourcemeta::core::to_regex("^*")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_escaped_closing_bracket_in_class) {
  const auto regex{sourcemeta::core::to_regex("[a\\]b]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
}

TEST(Regex_matches, ecma262_negated_empty_class) {
  const auto regex{sourcemeta::core::to_regex("[^]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_unicode_hex_too_large) {
  const auto regex{sourcemeta::core::to_regex("\\u{110000}")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_multiple_backslashes_before_d) {
  const auto regex{sourcemeta::core::to_regex("\\\\\\\\d")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\\\d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\\\\5"));
}

TEST(Regex_matches, ecma262_backslash_before_closing_bracket) {
  const auto regex{sourcemeta::core::to_regex("[abc\\]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_empty_alternation_branch) {
  const auto regex{sourcemeta::core::to_regex("a||b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(Regex_matches, ecma262_pipe_at_start) {
  const auto regex{sourcemeta::core::to_regex("|abc")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(Regex_matches, ecma262_pipe_at_end) {
  const auto regex{sourcemeta::core::to_regex("abc|")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
}

TEST(Regex_matches, ecma262_dot_matches_any_except_newline) {
  const auto regex{sourcemeta::core::to_regex("a.b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "axb"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a5b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a b"));
}

TEST(Regex_matches, ecma262_escaped_dot_in_char_class) {
  const auto regex{sourcemeta::core::to_regex("[.]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "."));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_non_greedy_star) {
  const auto regex{sourcemeta::core::to_regex("a*?")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
}

TEST(Regex_matches, ecma262_non_greedy_plus) {
  const auto regex{sourcemeta::core::to_regex("a+?")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aaa"));
}

TEST(Regex_matches, ecma262_lowercase_range) {
  const auto regex{sourcemeta::core::to_regex("[a-z]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, ecma262_hex_escape) {
  const auto regex{sourcemeta::core::to_regex("\\x41")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
}

TEST(Regex_matches, ecma262_dollar_before_bracket) {
  const auto regex{sourcemeta::core::to_regex("$[abc]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "$a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "$b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_quantifier_on_group) {
  const auto regex{sourcemeta::core::to_regex("(ab)+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_nested_groups) {
  const auto regex{sourcemeta::core::to_regex("((abc))")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, ecma262_escaped_pipe) {
  const auto regex{sourcemeta::core::to_regex("a\\|b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a|b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_escaped_paren) {
  const auto regex{sourcemeta::core::to_regex("\\(abc\\)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "(abc)"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_dollar_at_start) {
  const auto regex{sourcemeta::core::to_regex("$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "anything"));
}

TEST(Regex_matches, ecma262_caret_at_end) {
  const auto regex{sourcemeta::core::to_regex("abc^")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc^"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_escaped_quantifiers) {
  const auto regex{sourcemeta::core::to_regex("\\*\\+\\?")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "*+?"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_escaped_curly_brace) {
  const auto regex{sourcemeta::core::to_regex("\\{3\\}")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "{3}"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "333"));
}

TEST(Regex_matches, ecma262_question_quantifier) {
  const auto regex{sourcemeta::core::to_regex("ab?c")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ac"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abbc"));
}

TEST(Regex_matches, ecma262_zero_quantifier) {
  const auto regex{sourcemeta::core::to_regex("a{0}b")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xyzb"));
}

TEST(Regex_matches, ecma262_spaces_are_literal) {
  const auto regex{sourcemeta::core::to_regex("a b c")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a b c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc"));
}

TEST(Regex_matches, ecma262_tab_is_literal) {
  const auto regex{sourcemeta::core::to_regex("a\tb")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a\tb"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, ecma262_backslash_before_closing_paren) {
  const auto regex{sourcemeta::core::to_regex("(abc\\)")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_caret_dollar_together) {
  const auto regex{sourcemeta::core::to_regex("^$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_multiple_carets_at_start) {
  const auto regex{sourcemeta::core::to_regex("^^^abc")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abcdef"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "^^abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xabc"));
}

TEST(Regex_matches, ecma262_multiple_dollars_at_end) {
  const auto regex{sourcemeta::core::to_regex("abc$$$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "abc"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "abc$"));
}

TEST(Regex_matches, ecma262_v_flag_set_subtraction) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--a]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_set_intersection) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]&&[0-5]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_vowels) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--[aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "i"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "o"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "u"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_range) {
  const auto regex{sourcemeta::core::to_regex("[[0-9]--[3-7]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "2"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "8"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "3"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "7"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_from_digit) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]--[0]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_from_word) {
  const auto regex{sourcemeta::core::to_regex("[[\\w]--[0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_word_lowercase) {
  const auto regex{sourcemeta::core::to_regex("[[\\w]&&[a-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "_"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_even_digits) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]&&[02468]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "2"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "8"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_uppercase) {
  const auto regex{sourcemeta::core::to_regex("[[A-Za-z]--[A-Z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "Z"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_same_char) {
  const auto regex{sourcemeta::core::to_regex("[[a]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_same_range) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]&&[a-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_disjoint) {
  const auto regex{sourcemeta::core::to_regex("[[a-c]&&[x-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "m"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_hex_letters) {
  const auto regex{sourcemeta::core::to_regex("[[0-9a-f]--[a-f]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "f"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_partial_overlap) {
  const auto regex{sourcemeta::core::to_regex("[[a-m]&&[h-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "h"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "j"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "g"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_start_of_range) {
  const auto regex{sourcemeta::core::to_regex("[[a-e]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_end_of_range) {
  const auto regex{sourcemeta::core::to_regex("[[a-e]--[e]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_middle_of_range) {
  const auto regex{sourcemeta::core::to_regex("[[a-e]--[c]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_single_char) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]&&[m]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_subtraction_in_pattern) {
  const auto regex{sourcemeta::core::to_regex("^[[a-z]--[aeiou]]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bcdfg"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xyz"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "hello"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aaa"));
}

TEST(Regex_matches, ecma262_v_flag_intersection_in_pattern) {
  const auto regex{sourcemeta::core::to_regex("^[\\d&&[0-5]]+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "012345"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "6789"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123456"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_digit_from_word) {
  const auto regex{sourcemeta::core::to_regex("[[\\w]--[\\d]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_digit_word) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]&&[\\w]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "_"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_specific_punct) {
  const auto regex{sourcemeta::core::to_regex("[[\\!-\\/]--[#$%]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "&"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "/"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "#"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "$"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "%"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_empty) {
  const auto regex{sourcemeta::core::to_regex("[[a-c]--[x-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_printable_alpha) {
  const auto regex{sourcemeta::core::to_regex("[[ -~]&&[A-Z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "M"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
}

TEST(Regex_matches, ecma262_v_flag_subtract_multiple_ranges) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--[a-cx-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "w"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "y"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_space) {
  const auto regex{sourcemeta::core::to_regex("[[ -\\/]--[ ]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "/"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
}

TEST(Regex_matches, ecma262_v_flag_intersect_to_single) {
  const auto regex{sourcemeta::core::to_regex("[[a-c]&&[c-e]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_subtraction_with_quantifier) {
  const auto regex{sourcemeta::core::to_regex("^[[0-9]--[0]]{3}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "999"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "012"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "100"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "12"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_caret) {
  const auto regex{sourcemeta::core::to_regex("[[!-@]--[\\^]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "@"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "^"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_alphanum_upper) {
  const auto regex{sourcemeta::core::to_regex("[[0-9A-Za-z]&&[A-Z0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_entire_range) {
  const auto regex{sourcemeta::core::to_regex("[[a-c]--[a-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(Regex_matches, ecma262_v_flag_multiple_set_ops_pattern) {
  const auto regex{
      sourcemeta::core::to_regex("^[[a-z]--[aeiou]][[0-9]--[0]]$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b1"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a1"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e5"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_digit_ranges) {
  const auto regex{sourcemeta::core::to_regex("[[0-5]&&[3-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "3"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "4"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "2"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "6"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_preserving_hyphen) {
  const auto regex{sourcemeta::core::to_regex("[[0-9\\-]--[5-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "4"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_word_underscore) {
  const auto regex{sourcemeta::core::to_regex("[[\\w]&&[_]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_from_union) {
  const auto regex{sourcemeta::core::to_regex("[[a-zA-Z0-9]--[A-Z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "Z"));
}

TEST(Regex_matches, ecma262_v_flag_large_subtraction) {
  const auto regex{sourcemeta::core::to_regex("[[\\x20-\\x7e]--[a-zA-Z0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "@"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "~"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, ecma262_v_flag_hex_range_subtract_literal) {
  const auto regex{sourcemeta::core::to_regex("[[\\x61-\\x7a]--[aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_single_from_single) {
  const auto regex{sourcemeta::core::to_regex("[[a]--[b]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_single_chars_same) {
  const auto regex{sourcemeta::core::to_regex("[[a]&&[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_single_chars_different) {
  const auto regex{sourcemeta::core::to_regex("[[a]&&[b]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_hex_intersect_hex) {
  const auto regex{
      sourcemeta::core::to_regex("[[\\x30-\\x39]&&[\\x33-\\x37]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "3"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "7"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "2"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "8"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_all_digits_from_word) {
  const auto regex{sourcemeta::core::to_regex("[[[\\w]--[\\d]]--[_]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "_"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_with_escaped_backslash) {
  const auto regex{sourcemeta::core::to_regex("[[\\\\a-z]&&[\\\\]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_control_chars) {
  const auto regex{
      sourcemeta::core::to_regex("[[\\x00-\\x1f]--[\\x00-\\x08]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\x00"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\x08"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_with_tab_newline) {
  const auto regex{sourcemeta::core::to_regex("[[\\t\\n\\r ]&&[\\t\\n]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\r"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
}

TEST(Regex_matches, ecma262_v_flag_subtract_leaves_single) {
  const auto regex{sourcemeta::core::to_regex("[[a-c]--[ab]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_overlapping_three_char) {
  const auto regex{sourcemeta::core::to_regex("[[abc]&&[bcd]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(Regex_matches, ecma262_v_flag_mixed_hex_and_literal_in_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[\\x41-\\x5a]--[AEIOU]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "B"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "E"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_boundary_chars) {
  const auto regex{sourcemeta::core::to_regex("[[!-~]--[!~]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\""));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "}"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "~"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_complement_like) {
  const auto regex{
      sourcemeta::core::to_regex("[[\\x00-\\x60\\x7b-\\x7f]&&[\\x00-\\x7f]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_from_shorthand_with_hex) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]--[\\x30-\\x34]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "4"));
}

TEST(Regex_matches, ecma262_v_flag_complex_alphanumeric_filter) {
  const auto regex{sourcemeta::core::to_regex("[[0-9A-Fa-f]--[g-zG-Z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "f"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "F"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "g"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "G"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_ranges_at_boundary) {
  const auto regex{sourcemeta::core::to_regex("[[a-f]&&[f-k]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "f"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "g"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "k"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_all_but_one_from_range) {
  const auto regex{sourcemeta::core::to_regex("[[a-e]--[abde]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_multiple_hex_escapes_in_subtraction) {
  const auto regex{sourcemeta::core::to_regex(
      "[[\\x20-\\x7e]--[\\x30-\\x39]--[\\x41-\\x5a]--[\\x61-\\x7a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "@"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_mixed_subtract_then_intersect) {
  const auto regex{sourcemeta::core::to_regex("[[[a-z]--[aeiou]]&&[a-m]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_mixed_intersect_then_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[[a-z]&&[a-m]]--[aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "n"));
}

TEST(Regex_matches, ecma262_v_flag_nested_class_in_operand) {
  const auto regex{sourcemeta::core::to_regex("[[[a-z][0-9]]--[aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_deeply_nested_brackets) {
  const auto regex{sourcemeta::core::to_regex("[[[[a-z]]]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_nested_from_nested) {
  const auto regex{sourcemeta::core::to_regex("[[[a-z]]--[[x-z]]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "w"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_shorthand_nested_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[[\\d]]--[0]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_triple_chained_intersection) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]&&[a-m]&&[f-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "f"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_empty_result_in_chain) {
  const auto regex{sourcemeta::core::to_regex("[[a]--[a]--[b]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_empty_result_in_chain) {
  const auto regex{sourcemeta::core::to_regex("[[a-c]&&[x-z]&&[a-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(Regex_matches, ecma262_v_flag_operator_inside_nested_not_toplevel) {
  const auto regex{sourcemeta::core::to_regex("[[a\\-\\-b]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_alternation_after_set_op) {
  const auto regex{sourcemeta::core::to_regex("^([[a-z]--[aeiou]]|[0-9])$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
}

TEST(Regex_matches, ecma262_v_flag_set_op_with_dot_star) {
  const auto regex{sourcemeta::core::to_regex("^.*[[a-z]--[aeiou]]$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xxxb"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "123z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xxxa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "123e"));
}

TEST(Regex_matches, ecma262_v_flag_shorthand_only_intersection) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]&&[\\w]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "_"));
}

TEST(Regex_matches, ecma262_v_flag_double_nested_with_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[[a-z]]--[[aeiou]]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_union_in_nested) {
  const auto regex{sourcemeta::core::to_regex("[[[a-c][x-z]]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "w"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_from_union_nested) {
  const auto regex{sourcemeta::core::to_regex("[[[a-z][0-9]]--[aeiou0]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_four_way_intersection) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]&&[a-m]&&[f-z]&&[g-l]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "g"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "l"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "f"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "m"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_alternating_ops) {
  const auto regex{
      sourcemeta::core::to_regex("[[[[a-z]--[aeiou]]&&[a-m]]--[klm]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "j"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "k"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "n"));
}

TEST(Regex_matches, ecma262_v_flag_repeated_char_class) {
  const auto regex{sourcemeta::core::to_regex("^[[a-c]][[d-f]]$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ad"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "cf"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aa"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "dd"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_whitespace_subtract_space) {
  const auto regex{sourcemeta::core::to_regex("[[\\s]--[ ]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_negated_digit_intersect) {
  const auto regex{sourcemeta::core::to_regex("[[\\D]&&[a-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
}

TEST(Regex_matches, ecma262_v_flag_negated_word_intersect) {
  const auto regex{sourcemeta::core::to_regex("[[\\W]&&[\\!-\\/]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "/"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_negated_space_intersect) {
  const auto regex{sourcemeta::core::to_regex("[[\\S]&&[a-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\t"));
}

TEST(Regex_matches, ecma262_v_flag_caret_negation_with_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[^a-z]--[0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_single_char_range) {
  const auto regex{sourcemeta::core::to_regex("[[a-a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_literal_hyphen_at_end) {
  const auto regex{sourcemeta::core::to_regex("[[a-z\\-]--[aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_literal_hyphen_at_start) {
  const auto regex{sourcemeta::core::to_regex("[[\\-a-z]--[aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_with_caret_in_operand) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--[^aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "e"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_with_caret_in_operand) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]&&[^x-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "w"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_unicode_escape_4digit_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[\\u0041-\\u005A]--[ABC]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "D"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "C"));
}

TEST(Regex_matches, ecma262_v_flag_unicode_escape_brace_subtract) {
  const auto regex{
      sourcemeta::core::to_regex("[[\\u{0041}-\\u{005A}]--[ABC]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "D"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "C"));
}

TEST(Regex_matches, ecma262_v_flag_backspace_in_class) {
  const auto regex{sourcemeta::core::to_regex("[[\\b]--[\\x08]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_control_escape_intersect) {
  const auto regex{sourcemeta::core::to_regex("[[\\cA-\\cZ]&&[\\x01-\\x03]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x01"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x03"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\x04"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(Regex_matches, ecma262_v_flag_empty_first_operand) {
  const auto regex{sourcemeta::core::to_regex("[--[a-z]]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_empty_second_operand) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_double_operator_subtraction) {
  const auto regex{sourcemeta::core::to_regex("[a-z----abc]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_subtract_abc_from_az) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--[abc]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
}

TEST(Regex_matches, ecma262_v_flag_rightbracket_first_in_nested) {
  const auto regex{sourcemeta::core::to_regex("[[\\]-a]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_from_single_shorthand) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]--[5]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_two_shorthands) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]&&[\\w]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "_"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_shorthand_from_shorthand) {
  const auto regex{sourcemeta::core::to_regex("[[\\w]--[\\d]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
}

TEST(Regex_matches, ecma262_v_flag_triple_nested_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[[a-z]]--[[aeiou]]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_empty_nested) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--[]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_word_minus_alphanumeric) {
  const auto regex{sourcemeta::core::to_regex("[[\\w]--[a-zA-Z0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_multiple_escape_sequences_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[[\\t\\n\\r]]--[\\t]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\n"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\r"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\t"));
}

TEST(Regex_matches, ecma262_v_flag_mixed_unicode_escape_types) {
  const auto regex{sourcemeta::core::to_regex("[[\\u0041-\\u{005A}]--[ABC]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "D"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
}

TEST(Regex_matches, ecma262_v_flag_control_char_range_intersect) {
  const auto regex{
      sourcemeta::core::to_regex("[[\\x00-\\x1F]&&[\\x00-\\x0F]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), std::string("\x00", 1)));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x0F"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\x10"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\x1F"));
}

TEST(Regex_matches, ecma262_v_flag_del_char_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[\\x7E-\\x7F]--[\\x7E]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x7F"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "~"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_form_feed_and_vertical_tab) {
  const auto regex{sourcemeta::core::to_regex("[[\\f\\v]--[\\f]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\v"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\f"));
}

TEST(Regex_matches, ecma262_v_flag_null_char_in_operation) {
  const auto regex{sourcemeta::core::to_regex("[[\\x00-\\x02]--[\\x00]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x01"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x02"));
  EXPECT_FALSE(
      sourcemeta::core::matches(regex.value(), std::string("\x00", 1)));
}

TEST(Regex_matches, ecma262_v_flag_printable_minus_alphanumeric) {
  const auto regex{sourcemeta::core::to_regex("[[\\x20-\\x7E]--[a-zA-Z0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "@"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, ecma262_v_flag_five_way_chained_intersection) {
  const auto regex{
      sourcemeta::core::to_regex("[[a-z]&&[a-w]&&[a-t]&&[a-q]&&[a-n]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "n"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "o"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_lowercase_control_escape) {
  const auto regex{sourcemeta::core::to_regex("[[\\ca-\\cz]&&[\\x01-\\x03]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x01"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x03"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\x04"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_all_word_chars) {
  const auto regex{sourcemeta::core::to_regex("[[\\w]--[\\w]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "_"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_with_itself) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]&&[\\d]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_double_backslash_in_operation) {
  const auto regex{sourcemeta::core::to_regex("[[\\\\]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_from_full_ascii_printable) {
  const auto regex{
      sourcemeta::core::to_regex("[[[[[[\\x20-\\x7E]--[\\x20-\\x2F]]--[\\x3A-"
                                 "\\x40]]--[\\x5B-\\x60]]--[\\x7"
                                 "B-\\x7E]]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "@"));
}

TEST(Regex_matches, ecma262_v_flag_all_hex_digits_only) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]&&[a-fA-F0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "F"));
}

TEST(Regex_matches, ecma262_v_flag_hex_letter_only) {
  const auto regex{sourcemeta::core::to_regex("[[a-fA-F0-9]--[\\d]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "f"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "F"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "g"));
}

TEST(Regex_matches, ecma262_v_flag_escaped_special_regex_chars) {
  const auto regex{sourcemeta::core::to_regex("[[.+*?]--[.]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "+"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "*"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "?"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "."));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_escaped_open_bracket) {
  const auto regex{sourcemeta::core::to_regex("[[\\[]--[x]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "["));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "]"));
}

TEST(Regex_matches, ecma262_v_flag_only_special_punctuation) {
  const auto regex{sourcemeta::core::to_regex("[[\\!-\\/]&&[\\(-+]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "("));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ")"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "*"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "+"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "/"));
}

TEST(Regex_matches, ecma262_v_flag_range_with_same_start_end) {
  const auto regex{sourcemeta::core::to_regex("[[a-a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_at_sign_and_backtick) {
  const auto regex{sourcemeta::core::to_regex("[[@`]--[@]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "`"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "@"));
}

TEST(Regex_matches, ecma262_v_flag_escaped_hyphen_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[a\\-z]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(Regex_matches, ecma262_v_flag_hex_to_literal_range) {
  const auto regex{sourcemeta::core::to_regex("[[\\x41-Z]--[ABC]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "D"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "C"));
}

TEST(Regex_matches, ecma262_v_flag_literal_to_hex_range) {
  const auto regex{sourcemeta::core::to_regex("[[A-\\x5A]--[ABC]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "D"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "C"));
}

TEST(Regex_matches, ecma262_v_flag_multiple_char_classes_in_pattern) {
  const auto regex{
      sourcemeta::core::to_regex("[[a-z]--[aeiou]][[A-Z]--[AEIOU]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bB"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "cD"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aA"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bE"));
}

TEST(Regex_matches, ecma262_v_flag_in_group) {
  const auto regex{sourcemeta::core::to_regex("(x[[a-z]--[aeiou]]y)")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xby"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "xcy"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xay"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "xey"));
}

TEST(Regex_matches, ecma262_v_flag_with_quantifier_range) {
  const auto regex{sourcemeta::core::to_regex("^[[a-z]--[aeiou]]{2,4}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bc"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bcdf"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bcdfg"));
}

TEST(Regex_matches, ecma262_v_flag_anchored_both_ends) {
  const auto regex{sourcemeta::core::to_regex("^[[a-z]--[aeiou]]$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bb"));
}

TEST(Regex_matches, ecma262_v_flag_alternation_both_branches) {
  const auto regex{
      sourcemeta::core::to_regex("[[a-m]--[aeiou]]|[[n-z]--[aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "p"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "o"));
}

TEST(Regex_matches, ecma262_v_flag_ten_way_chained_subtraction) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]--[a]--[b]--[c]--[d]--[e]--[f]--[g]--[h]--[i]--[j]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "k"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "j"));
}

TEST(Regex_matches, ecma262_v_flag_single_char_minus_single_char_same) {
  const auto regex{sourcemeta::core::to_regex("[[x]--[x]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "y"));
}

TEST(Regex_matches, ecma262_v_flag_single_char_minus_single_char_different) {
  const auto regex{sourcemeta::core::to_regex("[[x]--[y]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "y"));
}

TEST(Regex_matches, ecma262_v_flag_caret_not_at_start_in_nested) {
  const auto regex{sourcemeta::core::to_regex("[[a^b]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "^"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_dollar_in_class) {
  const auto regex{sourcemeta::core::to_regex("[[$a]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "$"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_pipe_in_class) {
  const auto regex{sourcemeta::core::to_regex("[[\\|a]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "|"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_lookahead_with_operation) {
  const auto regex{sourcemeta::core::to_regex("(?=[[a-z]--[aeiou]]).")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "e"));
}

TEST(Regex_matches, ecma262_v_flag_lookbehind_with_operation) {
  const auto regex{sourcemeta::core::to_regex("(?<=[[a-z]--[aeiou]])x")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bx"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ax"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ex"));
}

TEST(Regex_matches, ecma262_v_flag_full_ascii_intersect_to_alpha) {
  const auto regex{sourcemeta::core::to_regex("[[\\x00-\\x7F]&&[a-zA-Z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "!"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_to_result_one_char) {
  const auto regex{sourcemeta::core::to_regex("[[a-c]--[ab]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_to_result_one_char) {
  const auto regex{sourcemeta::core::to_regex("[[abc]&&[cde]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(Regex_matches, ecma262_v_flag_space_in_operation) {
  const auto regex{sourcemeta::core::to_regex("[[ a]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_bell_char_subtract) {
  const auto regex{sourcemeta::core::to_regex("[[\\x07-\\x09]--[\\x08]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\x07"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\b"));
}

TEST(Regex_matches, ecma262_v_flag_escape_sequence_vs_hex) {
  const auto regex{sourcemeta::core::to_regex("[\\t--[\\x09]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\t"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "t"));
}

TEST(Regex_matches, ecma262_v_flag_carriage_return_line_feed) {
  const auto regex{sourcemeta::core::to_regex("[[\\r\\n]--[\\n]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\r"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "\n"));
}

TEST(Regex_matches, ecma262_v_flag_repeated_same_subtraction) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--[aeiou]--[aeiou]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_quadruple_nested) {
  const auto regex{sourcemeta::core::to_regex("[[[[a-z]]]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "A"));
}

TEST(Regex_matches, ecma262_v_flag_subtract_superset) {
  const auto regex{sourcemeta::core::to_regex("[[abc]--[a-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
}

TEST(Regex_matches, ecma262_v_flag_intersect_disjoint_sets) {
  const auto regex{sourcemeta::core::to_regex("[[abc]&&[xyz]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
}

TEST(Regex_matches, ecma262_v_flag_negative_lookahead_with_operation) {
  const auto regex{sourcemeta::core::to_regex("a(?![[a-z]--[aeiou]])")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ae"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a1"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "ab"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "az"));
}

TEST(Regex_matches, ecma262_v_flag_negative_lookbehind_with_operation) {
  const auto regex{sourcemeta::core::to_regex("(?<![[a-z]--[aeiou]])x")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "ax"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "bx"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "zx"));
}

TEST(Regex_matches, ecma262_v_flag_non_capturing_group) {
  const auto regex{sourcemeta::core::to_regex("(?:[[a-z]--[aeiou]])+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "bcdf"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "aeiou"));
}

TEST(Regex_matches, ecma262_v_flag_nested_groups_with_operation) {
  const auto regex{sourcemeta::core::to_regex("(([[a-z]--[aeiou]]))")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_unknown_escape_sequence) {
  const auto regex{sourcemeta::core::to_regex("[[\\q]--[x]]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_tilde_and_special_chars) {
  const auto regex{sourcemeta::core::to_regex("[[~!@#]--[~]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "@"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "#"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "~"));
}

TEST(Regex_matches, ecma262_v_flag_equals_and_less_greater) {
  const auto regex{sourcemeta::core::to_regex("[[<=>]--[=]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "<"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ">"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "="));
}

TEST(Regex_matches, ecma262_v_flag_curly_braces_in_class) {
  const auto regex{sourcemeta::core::to_regex("[[\\{\\}]--[\\{]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "}"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "{"));
}

TEST(Regex_matches, ecma262_v_flag_parentheses_in_class) {
  const auto regex{sourcemeta::core::to_regex("[[\\(\\)]--[\\(]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ")"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "("));
}

TEST(Regex_matches, ecma262_v_flag_complex_subtract_intersect_subtract) {
  const auto regex{
      sourcemeta::core::to_regex("[[[[a-z]--[aeiou]]&&[b-y]]--[xyz]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "w"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

TEST(Regex_matches, ecma262_v_flag_ampersand_in_class) {
  const auto regex{sourcemeta::core::to_regex("[[&a]--[a]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "&"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_percent_and_hash) {
  const auto regex{sourcemeta::core::to_regex("[[%#]--[%]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "#"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "%"));
}

TEST(Regex_matches, ecma262_v_flag_colon_semicolon_comma) {
  const auto regex{sourcemeta::core::to_regex("[[:;,]--[;]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ":"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), ","));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), ";"));
}

TEST(Regex_matches, ecma262_v_flag_question_exclamation) {
  const auto regex{sourcemeta::core::to_regex("[[?!]--[?]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "!"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "?"));
}

TEST(Regex_matches, ecma262_v_flag_underscore_intersect) {
  const auto regex{sourcemeta::core::to_regex("[[\\w]&&[^a-zA-Z0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "_"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_digits_odd_only) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]--[02468]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "3"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "9"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "8"));
}

TEST(Regex_matches, ecma262_v_flag_digits_even_only) {
  const auto regex{sourcemeta::core::to_regex("[[\\d]&&[02468]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "0"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "4"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "8"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "1"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "9"));
}

TEST(Regex_matches, ecma262_v_flag_single_quote_double_quote) {
  const auto regex{sourcemeta::core::to_regex("[['\"']--[']]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\""));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "'"));
}

TEST(Regex_matches, ecma262_v_flag_forward_slash_backslash) {
  const auto regex{sourcemeta::core::to_regex("[[\\\\]--[\\/]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "\\"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "/"));
}

TEST(Regex_matches, ecma262_v_flag_invalid_unbalanced_parens) {
  const auto regex{sourcemeta::core::to_regex("([[a-z]--[aeiou]]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_invalid_bad_quantifier) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--[aeiou]]{5,2}")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_shorthand_intersection_valid) {
  const auto regex{sourcemeta::core::to_regex("[\\d&&[0-5]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "3"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "7"));
}

TEST(Regex_matches, ecma262_v_flag_shorthand_subtraction_valid) {
  const auto regex{sourcemeta::core::to_regex("[\\w--[0-9]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, ecma262_v_flag_shorthand_intersection_with_range_valid) {
  const auto regex{sourcemeta::core::to_regex("[\\w&&[a-z]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
}

TEST(Regex_matches, ecma262_v_flag_shorthand_subtract_single_valid) {
  const auto regex{sourcemeta::core::to_regex("[\\d--[0]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(Regex_matches, ecma262_v_flag_shorthand_to_shorthand_subtract_valid) {
  const auto regex{sourcemeta::core::to_regex("[\\w--\\d]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(Regex_matches, ecma262_v_flag_shorthand_to_shorthand_intersect_valid) {
  const auto regex{sourcemeta::core::to_regex("[\\d&&\\w]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(Regex_matches, ecma262_v_flag_chained_shorthand_ops_valid) {
  const auto regex{sourcemeta::core::to_regex("[\\w--\\d--[_]]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "_"));
}

TEST(Regex_matches, ecma262_v_flag_invalid_range_without_nesting) {
  const auto regex{sourcemeta::core::to_regex("[\\x61-\\x7a--[aeiou]]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_invalid_mixed_ops_without_nesting) {
  const auto regex{sourcemeta::core::to_regex("[[a-z]--[aeiou]&&[a-m]]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_invalid_unescaped_hyphen_in_class) {
  const auto regex{sourcemeta::core::to_regex("[[a-z-]--[x]]")};
  EXPECT_FALSE(regex.has_value());
}

TEST(Regex_matches, ecma262_v_flag_invalid_unescaped_pipe) {
  const auto regex{sourcemeta::core::to_regex("[[|a]--[a]]")};
  EXPECT_FALSE(regex.has_value());
}
