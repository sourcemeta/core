#include <sourcemeta/core/test.h>

#include <sourcemeta/core/regex.h>

TEST(suite_valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("([abc])+\\s+$"));
}

TEST(suite_invalid_unclosed_paren) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("^(abc]"));
}

TEST(suite_invalid_perl_extension_alarm) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\a"));
}

TEST(valid_empty) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("")); }

TEST(valid_anchor_start) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("^foo"));
}

TEST(valid_anchor_end) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("foo$")); }

TEST(valid_anchor_both) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("^foo$"));
}

TEST(valid_character_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[abc]"));
}

TEST(valid_negated_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[^abc]"));
}

TEST(valid_range_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[a-z]"));
}

TEST(invalid_unclosed_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[abc"));
}

TEST(valid_empty_class) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[]")); }

TEST(valid_empty_class_negated) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[^]"));
}

TEST(valid_digit_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\d"));
}

TEST(valid_word_escape) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\w")); }

TEST(valid_space_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\s"));
}

TEST(valid_word_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\bfoo\\b"));
}

TEST(valid_newline_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\n"));
}

TEST(valid_tab_escape) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\t")); }

TEST(valid_carriage_return_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\r"));
}

TEST(valid_form_feed_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\f"));
}

TEST(valid_vertical_tab_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\v"));
}

TEST(valid_null_escape) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\0")); }

TEST(valid_hex_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\x41"));
}

TEST(valid_unicode_4digit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\u0041"));
}

TEST(valid_unicode_braced) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\u{1F600}"));
}

TEST(valid_control_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\cA"));
}

TEST(valid_unicode_property) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Letter}"));
}

TEST(invalid_alarm_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\a"));
}

TEST(invalid_escape_e) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\e")); }

TEST(invalid_escape_h) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\h")); }

TEST(invalid_escape_q) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\q")); }

TEST(valid_quantifier_star) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a*"));
}

TEST(valid_quantifier_plus) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a+"));
}

TEST(valid_quantifier_optional) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a?"));
}

TEST(valid_quantifier_range) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{2,5}"));
}

TEST(valid_quantifier_exact) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{3}"));
}

TEST(valid_quantifier_open) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{3,}"));
}

TEST(invalid_quantifier_reversed) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{5,2}"));
}

TEST(invalid_quantifier_no_target) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("+"));
}

TEST(valid_group) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(foo)")); }

TEST(valid_non_capturing_group) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?:foo)"));
}

TEST(valid_alternation) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("foo|bar"));
}

TEST(invalid_unclosed_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(foo"));
}

TEST(invalid_unopened_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("foo)"));
}

TEST(valid_lookahead) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("foo(?=bar)"));
}

TEST(valid_negative_lookahead) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("foo(?!bar)"));
}

TEST(valid_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<=foo)bar"));
}

TEST(valid_negative_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<!foo)bar"));
}

TEST(valid_dot_star) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma(".*")); }

TEST(valid_anchored_dot_star) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("^.*$"));
}

TEST(valid_dot_plus) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma(".+")); }

TEST(valid_single_dot) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma(".")); }

TEST(valid_uuid_pattern) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma(
      "^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$"));
}

TEST(valid_email_loose) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma(
      "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"));
}

TEST(valid_iso_date) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("^\\d{4}-\\d{2}-\\d{2}$"));
}

TEST(valid_named_group) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<name>foo)"));
}

TEST(valid_named_backreference) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<name>foo)\\k<name>"));
}

TEST(invalid_python_named_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?P<name>foo)"));
}

TEST(invalid_atomic_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?>foo)"));
}

TEST(invalid_inline_option_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?i)foo"));
}

TEST(invalid_inline_option_scoped) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?i:foo)"));
}

TEST(invalid_branch_reset_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?|a|b)"));
}

TEST(invalid_conditional_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?(1)yes|no)"));
}

TEST(invalid_subroutine_call) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?&name)"));
}

TEST(invalid_recursion) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?R)"));
}

TEST(invalid_backreference_uppercase_k) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("foo\\Kbar"));
}

TEST(invalid_line_break_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\R"));
}

TEST(invalid_quote_sequence) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\Qfoo\\E"));
}

TEST(invalid_posix_class_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[[:alpha:]]"));
}

TEST(invalid_possessive_quantifier) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a*+"));
}

TEST(invalid_backtracking_control) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(*FAIL)"));
}

TEST(invalid_perl_g_backreference) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(foo)\\g{1}"));
}

TEST(valid_literal_open_bracket_colon_in_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[[:abc]"));
}

TEST(valid_literal_colon_inside_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[a:b]"));
}

TEST(invalid_unterminated_named_backreference) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\k<name"));
}

TEST(invalid_empty_named_backreference) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\k<>"));
}
