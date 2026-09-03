#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(suite_valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("([abc])+\\s+$"));
}

TEST(suite_invalid_unclosed_paren) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("^(abc]"));
}

// Node reads this as literal text through Annex B of ECMA-262
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

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(valid_unicode_braced_out_of_range) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\u{110000}"));
}

TEST(valid_control_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\cA"));
}

TEST(valid_unicode_property) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Letter}"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_alarm_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\a"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_escape_e) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\e")); }

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_escape_h) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\h")); }

// Node reads this as literal text through Annex B of ECMA-262
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

TEST(invalid_possessive_range_quantifier) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{2,3}+"));
}

TEST(invalid_possessive_exact_quantifier) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{3}+"));
}

TEST(invalid_possessive_open_quantifier) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{3,}+"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(invalid_plus_after_unescaped_brace) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a}+"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(invalid_plus_after_escaped_brace_quantifier) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a\\{2,3}+"));
}

TEST(valid_variable_width_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<=a+)b"));
}

TEST(valid_variable_width_negative_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<!a+)b"));
}

TEST(valid_bounded_variable_width_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<=a{2,5})b"));
}

TEST(valid_nested_variable_width_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<=(?<=a)b)c"));
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

TEST(valid_inline_option_scoped) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?i:foo)"));
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

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_backreference_uppercase_k) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("foo\\Kbar"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_line_break_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\R"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_quote_sequence) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\Qfoo\\E"));
}

TEST(posix_class_alpha_reads_as_a_nested_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[[:alpha:]]"));
}

TEST(invalid_possessive_quantifier) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a*+"));
}

TEST(invalid_backtracking_control) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(*FAIL)"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_perl_g_backreference) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(foo)\\g{1}"));
}

TEST(valid_literal_open_bracket_colon_in_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[[:abc]"));
}

TEST(valid_literal_colon_inside_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[a:b]"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_unterminated_named_backreference) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\k<name"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(invalid_empty_named_backreference) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\k<>"));
}

TEST(atomic_atomic_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?>abc)"));
}

TEST(atomic_atomic_alternation) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?>a|ab)c"));
}

TEST(backref_ecma_named_backref) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<n>a)\\k<n>"));
}

TEST(backref_net_named_backref) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<n>a)\\k'n'"));
}

TEST(backref_python_named_backref) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?P<n>a)(?P=n)"));
}

TEST(backref_numeric_backref) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(a)\\1"));
}

TEST(backref_forward_numeric_backref) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\1(a)"));
}

TEST(class_uprop_in_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\p{L}]"));
}

TEST(class_v_flag_intersection) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[a&&b]"));
}

TEST(class_reversed_range) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[z-a]"));
}

TEST(class_trailing_dash) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[a-]"));
}

TEST(class_leading_dash) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[-a]"));
}

TEST(class_backspace_in_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\b]"));
}

TEST(comment_comment_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?#comment)a"));
}

TEST(comment_mid_comment) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a(?#c)b"));
}

TEST(conditional_numeric_conditional) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?(1)a|b)"));
}

TEST(conditional_named_conditional) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?(name)a|b)"));
}

TEST(conditional_assertion_conditional) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?(?=x)a|b)"));
}

TEST(conditional_conditional_no_else) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?(1)a)"));
}

TEST(escape_uprop) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{L}")); }

TEST(escape_neg_uprop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\P{L}"));
}

TEST(escape_script_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Script=Greek}"));
}

TEST(escape_binary_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Alphabetic}"));
}

TEST(escape_general_category_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{General_Category=Lu}"));
}

TEST(escape_general_category_alias_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{gc=Lu}"));
}

TEST(escape_general_category_long_value_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{gc=Uppercase_Letter}"));
}

TEST(escape_negated_general_category_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\P{gc=Lu}"));
}

TEST(escape_general_category_prop_in_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\p{gc=Lu}]"));
}

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(escape_general_category_prop_unknown_value) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\p{gc=NotAProperty}"));
}

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(escape_general_category_prop_empty_value) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\p{gc=}"));
}

TEST(escape_cased_letter_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Cased_Letter}"));
}

TEST(escape_combining_mark_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Combining_Mark}"));
}

TEST(escape_surrogate_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Surrogate}"));
}

TEST(escape_punct_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{punct}"));
}

TEST(escape_cntrl_prop) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{cntrl}"));
}

TEST(escape_uxxxx) { EXPECT_TRUE(sourcemeta::core::is_regex_ecma("A")); }

TEST(escape_escaped_slash) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\/"));
}

TEST(inline_flag_flag_i) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?i)abc"));
}

TEST(inline_flag_flags_ims) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?ims)abc"));
}

TEST(inline_flag_flag_u) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?u)abc"));
}

TEST(inline_flag_scoped_flag_on) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?i:abc)"));
}

TEST(inline_flag_scoped_flag_off) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?-i:abc)"));
}

TEST(inline_flag_scoped_on_off) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?i-s:abc)"));
}

TEST(inline_flag_verbose_extended) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?x) a b c"));
}

TEST(inline_flag_inline_flag_in_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?P<n>(?i)a)"));
}

TEST(lookaround_lookahead) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?=x)"));
}

TEST(lookaround_neg_lookahead) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?!x)"));
}

TEST(lookaround_fixed_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<=x)"));
}

TEST(lookaround_neg_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<!x)"));
}

TEST(lookaround_group_in_lookbehind) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<=(?:ab)+)c"));
}

TEST(misc_branch_reset) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?|(a)|(b))"));
}

TEST(misc_pcre_callout) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?C1)"));
}

TEST(misc_non_capturing_control) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?:abc)"));
}

TEST(misc_unclosed_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?P<n>abc"));
}

TEST(misc_unmatched_close) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("abc)"));
}

TEST(misc_trailing_alternation) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a|"));
}

TEST(misc_leading_alternation) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("|a"));
}

TEST(misc_empty_named_group) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<n>)"));
}

TEST(misc_escaped_backslash_control_valid) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\\\"));
}

TEST(misc_unclosed_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[a"));
}

TEST(misc_lone_open) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(")); }

TEST(named_group_ecma_named_group) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<name>x)"));
}

TEST(named_group_python_pcre_named_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?P<name>x)"));
}

TEST(named_group_net_named_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?'name'x)"));
}

TEST(named_group_duplicate_name_ecma_v_flag_allows_in_alternati) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<n>a)(?<n>b)"));
}

TEST(named_group_digit_leading_name) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<1a>x)"));
}

TEST(named_group_empty_name) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<>x)"));
}

TEST(possessive_possessive) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a++"));
}

TEST(possessive_possessive_2) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a?+"));
}

TEST(quantifier_reversed_interval) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{2,1}"));
}

TEST(quantifier_open_upper_control) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{2,}"));
}

TEST(quantifier_exact_at_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{65535}"));
}

TEST(quantifier_exact_past_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{65536}"));
}

TEST(quantifier_exact_past_unsigned_range) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{4294967295}"));
}

TEST(quantifier_exact_padded_past_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{0000070000}"));
}

TEST(quantifier_interval_past_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{1,99999}"));
}

TEST(quantifier_open_past_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a{99999,}"));
}

TEST(quantifier_reversed_interval_past_pcre2_limit) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{99999,2}"));
}

TEST(quantifier_reversed_interval_both_past_pcre2_limit) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{100000,99999}"));
}

TEST(quantifier_class_past_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[a-z]{70000}"));
}

TEST(quantifier_group_past_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?:ab){70000}"));
}

TEST(quantifier_capturing_group_past_pcre2_size) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(ab){10000}"));
}

TEST(quantifier_escaped_braces_past_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("a\\{70000\\}"));
}

TEST(quantifier_class_member_braces_past_pcre2_limit) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[a{70000}]"));
}

TEST(quantifier_no_atom) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("{2}"));
}

TEST(quantifier_leading_star) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("*abc"));
}

TEST(quantifier_leading_plus) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("+abc"));
}

TEST(quantifier_leading_question) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("?abc"));
}

TEST(quantifier_double_star) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a**"));
}

TEST(quantifier_stacked_interval) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{2}{3}"));
}

TEST(recursion_subroutine_number) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a(?1)"));
}

TEST(recursion_python_subroutine) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?P<n>a)(?P>n)"));
}

TEST(recursion_pcre_subroutine) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<n>a)(?&n)"));
}

TEST(recursion_recursion_0) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?0)"));
}

TEST(redos_nested_quantifier_valid) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(a+)+$"));
}

TEST(redos_alternation_star_valid) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(a|a)*"));
}

TEST(redos_repeated_group_valid) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(.*a){20}"));
}

TEST(modifier_group_add_flags) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?ims:abc)"));
}

TEST(modifier_group_remove_flags) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?-ims:abc)"));
}

TEST(modifier_group_both_empty) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?-:abc)"));
}

TEST(modifier_group_repeated_flag) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?ii:abc)"));
}

TEST(modifier_group_flag_on_both_sides) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?i-i:abc)"));
}

TEST(modifier_group_unknown_flag) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?x:abc)"));
}

TEST(group_name_leading_dollar_sign) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<$foo>a)"));
}

TEST(group_name_escaped_code_point) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<\\u0041>a)"));
}

TEST(group_name_outside_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<caf\xc3\xa9>a)"));
}

TEST(group_name_not_an_identifier) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<a\xe2\x98\x83>x)"));
}

TEST(group_name_repeated_across_alternatives) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<x>a)|(?<x>b)"));
}

TEST(group_name_repeated_within_one_alternative) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<x>a)(?<x>b)"));
}

TEST(group_name_repeated_across_nested_alternatives) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?:(?<x>a)|(?<x>b))"));
}

TEST(group_name_repeated_after_an_alternation) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?:(?<x>a)|(?<x>b))(?<x>c)"));
}

TEST(group_name_repeated_beside_an_alternation) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?:(?<x>a)|(?<x>b))|(?<x>c)"));
}

TEST(group_name_repeated_across_wrapped_alternatives) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("((?<x>a))|(?<x>b)"));
}

TEST(group_name_repeated_within_its_own_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<x>(?<x>a))"));
}

TEST(group_name_repeated_within_its_own_alternation) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<x>a|(?<x>b))"));
}

TEST(group_name_repeated_through_an_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<\\u0041>a)(?<A>b)"));
}

TEST(group_name_repeated_through_an_escape_across_alternatives) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(?<\\u0041>a)|(?<A>b)"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(named_backreference_without_a_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\k<x>"));
}

// Annex B of ECMA-262 has Node read this as a legacy octal escape
TEST(numbered_backreference_past_the_group_count) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(a)\\2"));
}

TEST(lone_lead_surrogate_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\uD83D"));
}

TEST(surrogate_pair_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\uD83D\\uDE00"));
}

TEST(surrogate_pair_escape_in_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\uD83D\\uDE00]"));
}

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(code_point_escape_past_the_unicode_range) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\u{110000}"));
}

TEST(quantified_lookbehind) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<=a)*"));
}

TEST(quantified_negative_lookbehind) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?<!a)+"));
}

TEST(quantified_word_boundary) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\b*"));
}

TEST(quantified_non_word_boundary) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\B{2}"));
}

// Annex B of ECMA-262 lets Node put a quantifier on a lookahead
TEST(quantified_lookahead) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?=a)*"));
}

TEST(assigned_binary_property) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Assigned}"));
}

TEST(nfkc_casefold_binary_property) {
  EXPECT_TRUE(
      sourcemeta::core::is_regex_ecma("\\p{Changes_When_NFKC_Casefolded}"));
}

TEST(nfkc_casefold_binary_property_alias) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{CWKCF}"));
}

TEST(script_value_alias) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\p{Script=Grek}"));
}

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(script_value_unknown) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\p{Script=Nowhere}"));
}

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(general_category_value_rejected_for_script) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\p{Script=Lu}"));
}

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(script_value_rejected_for_general_category) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\p{gc=Greek}"));
}

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(lone_script_value) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\p{Greek}"));
}

TEST(property_of_strings_needs_set_notation) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\p{RGI_Emoji}]"));
}

// Without a Unicode reading Annex B of ECMA-262 has Node take this literally
TEST(property_of_strings_cannot_be_negated) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\P{RGI_Emoji}"));
}

TEST(set_notation_string_disjunction) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\q{abc|d}]"));
}

// Only the set notation reading rejects this, so Node accepts it
TEST(set_notation_strings_cannot_be_negated) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[^\\q{abc}]"));
}

// Only the set notation reading rejects this, so Node accepts it
TEST(set_notation_range_cannot_lead_an_operator) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[a-z--[aeiou]]"));
}

// Only the set notation reading rejects this, so Node accepts it
TEST(set_notation_doubled_punctuator) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\q{a}$$]"));
}

TEST(doubled_punctuator_outside_set_notation) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[a$$b]"));
}

TEST(deeply_nested_groups_within_the_depth_bound) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma(std::string(200, '(') + "a" +
                                              std::string(200, ')')));
}

TEST(deeply_nested_groups_past_the_depth_bound) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma(std::string(100000, '(') + "a" +
                                               std::string(100000, ')')));
}

TEST(deeply_nested_classes_past_the_depth_bound) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma(std::string(100000, '[') + "a" +
                                               std::string(100000, ']')));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(lone_closing_bracket) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("]"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(lone_closing_bracket_after_an_atom) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a]"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(closing_bracket_after_a_character_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[a]]"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(closing_bracket_between_literals) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("foo]bar"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(lone_closing_brace) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("}")); }

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(lone_closing_brace_after_an_atom) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a}"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(lone_opening_brace) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("{")); }

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(lone_opening_brace_after_an_atom) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(braces_in_reverse_order) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("}{"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(empty_braces) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("{}")); }

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(empty_braces_after_an_atom) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("x{}"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(brace_quantifier_without_a_lower_bound) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{,3}"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(brace_quantifier_left_unterminated) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{2,"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(brace_quantifier_with_three_bounds) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{1,2,3}"));
}

// Node reads a brace outside a quantifier as literal text through Annex B
TEST(brace_quantifier_with_a_negative_bound) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a{-1}"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(escaped_opening_bracket_without_a_closing_one) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\d+:\\[1-9]+|N"));
}

TEST(alternation_grouped_between_anchors) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("^\\d+:([1-9]+|N)$"));
}

TEST(lone_closing_paren) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma(")")); }

TEST(lone_opening_bracket) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("["));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_match_reset_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\K"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_quote_start_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\Q"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_quote_end_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\E"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_subroutine_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\g"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_absolute_start_anchor) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\A"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_absolute_end_anchor) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\z"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_end_of_input_anchor) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\Z"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_end_of_input_anchor_after_a_literal) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("foo\\Z"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_match_start_anchor) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\G"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_non_newline_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\N"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_grapheme_cluster_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\X"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_single_byte_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\C"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(escape_of_an_arbitrary_letter) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\m"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(alarm_escape_inside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\a]"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(escape_of_an_arbitrary_letter_inside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\q]"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(perl_end_of_input_anchor_inside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\Z]"));
}

// Only the reading with neither Unicode flag allows escaping punctuation
TEST(escaped_dash_outside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\-"));
}

// Only the reading with neither Unicode flag allows escaping punctuation
TEST(escaped_space) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\ ")); }

// Only the reading with neither Unicode flag allows escaping punctuation
TEST(escaped_tilde) { EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\~")); }

// Only the reading with neither Unicode flag allows escaping punctuation
TEST(escaped_percent_sign) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\%"));
}

TEST(escaped_dash_inside_a_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\-]"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(control_escape_without_a_letter) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\c"));
}

TEST(control_escape_with_a_lowercase_letter) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\ck"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(control_escape_with_a_digit) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\c1"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(control_escape_with_an_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\c_"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(control_escape_with_a_digit_inside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\c1]"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(control_escape_with_an_underscore_inside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\c_]"));
}

// Annex B of ECMA-262 has Node read this as a legacy octal escape
TEST(legacy_octal_escape) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\012"));
}

// Annex B of ECMA-262 has Node read this as a legacy octal escape
TEST(legacy_octal_escape_of_two_zeroes) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\00"));
}

// Annex B of ECMA-262 has Node read this as a legacy octal escape
TEST(null_escape_followed_by_a_digit) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\08"));
}

// Annex B of ECMA-262 has Node read this as a legacy octal escape
TEST(legacy_octal_escape_inside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\012]"));
}

// Annex B of ECMA-262 has Node read a backreference with no group as octal
TEST(backreference_without_any_group) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\1"));
}

// Annex B of ECMA-262 has Node read a backreference with no group as octal
TEST(backreference_inside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\1]"));
}

TEST(backreference_within_the_group_count) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("(a)(b)\\2"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(escape_of_the_digit_eight) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\8"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(escape_of_the_digit_nine) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("\\9"));
}

// Node reads this as literal text through Annex B of ECMA-262
TEST(escape_of_the_digit_eight_inside_a_class) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\8]"));
}

// Annex B of ECMA-262 lets Node put a quantifier on a lookahead
TEST(quantified_negative_lookahead) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?!a)+"));
}

// Annex B of ECMA-262 lets Node put a quantifier on a lookahead
TEST(optional_lookahead) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?=a)?"));
}

// Annex B of ECMA-262 lets Node put a quantifier on a lookahead
TEST(lookahead_with_a_brace_quantifier) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("(?=a){2}"));
}

TEST(quantified_start_anchor) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("^*"));
}

TEST(quantified_end_anchor) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("$*"));
}

// Annex B of ECMA-262 lets Node use a shorthand as a range endpoint
TEST(class_range_starting_with_a_shorthand) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\d-z]"));
}

// Annex B of ECMA-262 lets Node use a shorthand as a range endpoint
TEST(class_range_ending_with_a_shorthand) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[a-\\d]"));
}

// Annex B of ECMA-262 lets Node use a shorthand as a range endpoint
TEST(class_range_between_two_shorthands) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("[\\w-\\d]"));
}

TEST(shorthand_before_a_trailing_dash) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\d-]"));
}

TEST(leading_dash_before_a_shorthand) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[-\\d]"));
}

TEST(class_range_between_braced_code_point_escapes) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\u{1F600}-\\u{1F601}]"));
}

TEST(braced_code_point_escape_in_the_basic_plane) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\u{61}"));
}

TEST(braced_code_point_escape_inside_a_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\u{61}]"));
}

TEST(set_notation_difference_with_a_property) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\p{L}--[aeiou]]"));
}

TEST(set_notation_intersection_of_two_classes) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[[a-z]&&[b-d]]"));
}

TEST(set_notation_single_string) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\q{abc}]"));
}

TEST(basic_emoji_property_of_strings) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\\p{Basic_Emoji}]"));
}

TEST(lone_trailing_surrogate_escape) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\\uDE00"));
}

TEST(astral_character_as_a_literal) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("\xf0\x9f\x98\x80"));
}

TEST(astral_character_inside_a_class) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[\xf0\x9f\x98\x80]"));
}

TEST(class_range_between_astral_characters) {
  EXPECT_TRUE(
      sourcemeta::core::is_regex_ecma("[\xf0\x9f\x98\x80-\xf0\x9f\x98\x81]"));
}

TEST(base64_pattern) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma(
      "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$"));
}

TEST(capitalised_word_pattern) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("^\\p{Lu}\\p{Ll}+$"));
}

TEST(blank_string_pattern) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("^\\s*$"));
}

TEST(non_whitespace_pattern) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma("[^\\s]+"));
}

TEST(deeply_nested_groups_at_the_depth_bound) {
  EXPECT_TRUE(sourcemeta::core::is_regex_ecma(std::string(255, '(') + "a" +
                                              std::string(255, ')')));
}

TEST(deeply_nested_groups_one_past_the_depth_bound) {
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma(std::string(256, '(') + "a" +
                                               std::string(256, ')')));
}
