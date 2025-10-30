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
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a]"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b]"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c]"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "[]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "]"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
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
  // In ECMA-262, $ is always an end assertion
  // Pattern "$(abc)" means: assert end, then match "abc"
  // This can NEVER match anything
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
  // In ECMA-262, ^ is always a start assertion, never a literal
  // Pattern "a^b" means: match "a", assert start, match "b"
  // This can NEVER match anything
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
  // In ECMA-262, ^ is always a start assertion
  // Pattern "^^abc" means: assert start, assert start (redundant), match "abc"
  // This matches strings starting with "abc"
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
  // In ECMA-262, $ and ^ are always assertions
  // Pattern "a$^b" means: match "a", assert end, assert start, match "b"
  // This can NEVER match anything
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
  // In ECMA-262, $ is always an end assertion
  // Pattern "$[abc]" means: assert end, then match one of a/b/c
  // This can NEVER match anything
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
  // In ECMA-262, ^ is always a start assertion
  // Pattern "abc^" means: match "abc", then assert start
  // This can NEVER match anything
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
  // In ECMA-262, ^ is always a start assertion
  // Pattern "^^^abc" means: assert start three times (redundant), match "abc"
  // This matches strings starting with "abc"
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
