#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

// The permissive dialect rewrites a pattern before handing it over, so every
// escape it reads has to behave when the pattern stops in the middle of one

TEST(permissive_class_hex_escape_without_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\x]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_hex_escape_with_one_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\x4]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_class_hex_escape_with_a_non_hex_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\xZZ]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_unicode_escape_without_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_unicode_escape_with_an_unclosed_brace) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u{41]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_unicode_escape_with_an_empty_brace) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u{}]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_unicode_escape_with_four_non_hex_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\uZZZZ]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_control_escape_without_a_letter) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\c]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_control_escape_with_a_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\c1]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_class_control_escape_with_a_lowercase_letter) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\ca]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_unicode_escape_without_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "\\u", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_unicode_escape_with_an_unclosed_brace) {
  const auto regex{sourcemeta::core::to_regex(
      "\\u{41", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_unicode_escape_with_a_non_hex_digit_in_the_brace) {
  const auto regex{sourcemeta::core::to_regex(
      "\\u{4Z}", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_unicode_escape_with_one_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "\\u4", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_backreference_escape_without_a_name) {
  const auto regex{sourcemeta::core::to_regex(
      "\\k", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_backreference_escape_without_an_angle_bracket) {
  const auto regex{sourcemeta::core::to_regex(
      "\\ka", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_backreference_escape_with_an_unclosed_name) {
  const auto regex{sourcemeta::core::to_regex(
      "(?<a>x)\\k<a", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_backreference_escape_with_an_empty_name) {
  const auto regex{sourcemeta::core::to_regex(
      "\\k<>", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_property_escape_without_a_brace) {
  const auto regex{sourcemeta::core::to_regex(
      "\\p", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_property_escape_with_a_single_letter_name) {
  const auto regex{sourcemeta::core::to_regex(
      "\\pL", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_group_that_stops_after_its_question_mark) {
  const auto regex{sourcemeta::core::to_regex(
      "(?", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_group_that_stops_after_its_angle_bracket) {
  const auto regex{sourcemeta::core::to_regex(
      "(?<", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_group_name_starting_with_a_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "(?<1a>x)", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_group_name_starting_with_a_dollar_sign) {
  const auto regex{sourcemeta::core::to_regex(
      "(?<$a>x)", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_plus_at_the_start_of_the_pattern) {
  const auto regex{sourcemeta::core::to_regex(
      "+a", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_that_never_closes) {
  const auto regex{sourcemeta::core::to_regex(
      "[abc", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_closing_bracket_on_its_own) {
  const auto regex{sourcemeta::core::to_regex(
      "a]b", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_class_with_a_closing_bracket_inside_a_nested_one) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

// A class is only expanded when it carries a set operation or a nested
// bracket, so every escape above is read by the expansion only in that company

TEST(permissive_set_difference_with_a_hex_escape_without_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\x--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_hex_escape_with_a_non_hex_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\xZZ--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_unicode_escape_without_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_an_unclosed_brace_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u{41--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_an_empty_brace_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u{}--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_four_non_hex_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\uZZZZ--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_control_escape_without_a_letter) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\c--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_control_escape_with_a_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\c1--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_control_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\cA--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_brace_escape_beyond_ascii) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u{100}--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_set_difference_with_a_brace_escape_beyond_the_last_code_point) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u{110000}--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_an_escape_beyond_ascii) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u00FF--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_set_difference_with_a_shorthand_operand) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\d--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_of_two_nested_brackets) {
  const auto regex{sourcemeta::core::to_regex(
      "[[abc]--[b]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_operations_that_do_not_agree) {
  const auto regex{sourcemeta::core::to_regex(
      "[[abc]--[b]&&[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_set_difference_with_a_range_ending_in_an_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[a-\\u{100}--b]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_set_difference_that_removes_everything) {
  const auto regex{sourcemeta::core::to_regex(
      "[a--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_non_hex_digit_in_a_brace_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u{4Z}--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_three_hex_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\u004--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_control_escape_beyond_the_letters) {
  const auto regex{sourcemeta::core::to_regex(
      "[\\c{--a]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

// A shorthand stands for a class of its own, so it cannot close a range
TEST(permissive_set_difference_with_a_shorthand_closing_a_range) {
  const auto regex{sourcemeta::core::to_regex(
      "[a-\\d--b]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_set_difference_with_a_bracket_left_open_by_its_operand) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a]]--[b]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_class_with_a_nested_bracket_after_a_character) {
  const auto regex{sourcemeta::core::to_regex(
      "[x[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

TEST(permissive_set_difference_with_a_dash_closing_its_operand) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-]--[b]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_left_open_by_a_trailing_backslash) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a]--[b\\", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
}

// An operand written as a bracket of its own is validated before it is read,
// which weighs the same escapes a second time

TEST(permissive_bracketed_operand_with_a_hex_escape_without_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[[\\x]--[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_bracketed_operand_with_a_hex_escape_with_a_non_hex_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "[[\\xZZ]--[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_bracketed_operand_with_a_unicode_escape_without_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[[\\u]--[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_bracketed_operand_with_an_unclosed_brace_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[[\\u{41]--[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_bracketed_operand_with_three_hex_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[[\\u004]--[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_bracketed_operand_with_a_control_escape_without_a_letter) {
  const auto regex{sourcemeta::core::to_regex(
      "[[\\c]--[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_bracketed_operand_with_a_control_escape_with_a_digit) {
  const auto regex{sourcemeta::core::to_regex(
      "[[\\c1]--[a]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

// A class whose content merely carries the two characters of a set operator
// without one standing in it is handed over as it was written
TEST(permissive_class_with_an_escaped_dash_before_a_range) {
  const auto regex{sourcemeta::core::to_regex(
      "[a\\--b]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "-"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "Z"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), " "));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
}

TEST(permissive_class_with_three_intersection_operands) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[b-y]&&[c-x]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "x"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "y"));
}

TEST(permissive_class_with_three_subtraction_operands) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]--[b]--[c]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "d"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "c"));
}

// One class carries one kind of operator, so a class naming two of them names
// no set this can work out
TEST(permissive_class_mixing_intersection_and_subtraction) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[b-y]--[c]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_intersected_with_a_bare_shorthand) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&\\d]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "5"));
}

TEST(permissive_class_operand_with_an_unknown_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\q]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_operand_carrying_a_pipe) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[|]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_operand_ending_in_a_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[a-]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_operand_without_brackets_carrying_a_range) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&a-b]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

// The set a class is expanded into reaches as far as the last ASCII code
// point, so a member named past it cannot be carried and the rewrite is
// declined rather than handing back a class that turns such input away
TEST(permissive_class_operand_naming_a_code_point_past_ascii) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\u{1F600}]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_range_ending_past_ascii) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[a-\\u{1F600}]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

// A value past the last code point names no character at all, so it is read
// the way anything malformed is rather than reported as uncarriable
TEST(permissive_class_operand_naming_a_value_past_every_code_point) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\u{110000}]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "u"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(permissive_class_range_opening_on_a_control_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-c]&&[\\cA-c]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(permissive_class_range_of_hexadecimal_escapes) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\x61-\\x63]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(permissive_class_range_of_four_digit_escapes) {
  const auto regex{
      sourcemeta::core::to_regex("[[a-z]&&[\\u0061-\\u0063]]",
                                 sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "d"));
}

TEST(permissive_class_range_of_braced_escapes) {
  const auto regex{
      sourcemeta::core::to_regex("[[a-z]&&[\\u{41}-\\u{43}]]",
                                 sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "B"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(permissive_class_operand_carrying_an_escaped_bracket) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\]]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "]"));
}

TEST(permissive_class_operand_with_an_empty_braced_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\u{}]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(permissive_class_operand_with_a_braced_escape_of_no_hex_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\u{ZZ}]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_operand_with_a_four_digit_escape_of_no_hex_digits) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\uZZZZ]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_operand_with_a_control_escape_of_no_letter) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\c{]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_operand_with_a_lowercase_control_escape) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[\\cz]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "z"));
}

// Two classes standing side by side name the union of what each holds
TEST(permissive_class_holding_two_classes_without_an_operator) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z][A-Z]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "A"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(permissive_class_whose_operand_carries_a_set_operator_of_its_own) {
  const auto regex{sourcemeta::core::to_regex(
      "[[[a-z]--[b]]&&[a-c]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "c"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(permissive_class_nested_operand_mixing_two_operators) {
  const auto regex{sourcemeta::core::to_regex(
      "[[x][a&&b--c]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_class_intersected_with_its_own_complement) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[^a-z]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "0"));
}

TEST(permissive_class_operand_ending_in_a_lone_backslash) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&a\\]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(permissive_class_bracketed_operand_ending_in_a_lone_backslash) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[a\\]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

// A shorthand class stands for no single code point, so one closing a range
// is read as a member of its own alongside the dash before it
TEST(permissive_class_range_closed_by_a_shorthand) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[a-\\d]]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

TEST(permissive_class_operand_without_brackets_ending_in_a_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&b-]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(permissive_class_operand_without_brackets_opening_on_a_dash) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&-b]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "b"));
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "a"));
}

TEST(permissive_class_operand_that_is_an_opening_bracket_alone) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a-z]&&[b", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), "b"));
}

// A class left unterminated is handed over as it stands, which is turned down
// where it names a range the wrong way round
TEST(permissive_unterminated_class_naming_a_descending_range) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a--b", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_unterminated_class_ending_in_a_lone_backslash) {
  const auto regex{sourcemeta::core::to_regex(
      "[[a--\\x", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_unterminated_standard_class) {
  const auto regex{sourcemeta::core::to_regex(
      "[a", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

// A possessive quantifier belongs to PCRE rather than ECMA-262, and is handed
// over as it stands
TEST(permissive_possessive_quantifier_on_a_plus) {
  const auto regex{sourcemeta::core::to_regex(
      "a++", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a++"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "aa"));
}

TEST(permissive_possessive_quantifier_on_an_optional) {
  const auto regex{sourcemeta::core::to_regex(
      "a?+", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a?+"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a"));
}

// A closing brace that no quantifier opened is a character of its own, so the
// quantifier after it is nothing possessive
TEST(permissive_plus_after_a_brace_that_opens_nothing) {
  const auto regex{sourcemeta::core::to_regex(
      "12}+", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "12}}"));
}

TEST(permissive_plus_after_a_brace_whose_bound_opens_nothing) {
  const auto regex{sourcemeta::core::to_regex(
      "2,3}+", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "2,3}"));
}

TEST(permissive_plus_after_a_brace_quantifier_whose_brace_is_escaped) {
  const auto regex{sourcemeta::core::to_regex(
      "a\\{2}+", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a{2}"));
}

TEST(permissive_escaped_opening_bracket_outside_a_class) {
  const auto regex{sourcemeta::core::to_regex(
      "a\\[b]", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a[b]"));
}

TEST(permissive_group_indicator_at_the_end_of_the_pattern) {
  const auto regex{sourcemeta::core::to_regex(
      "a(", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

TEST(permissive_group_extension_whose_name_opens_on_no_letter) {
  const auto regex{sourcemeta::core::to_regex(
      "(?<{a>b)", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_FALSE(regex.has_value());
}

// An escape of a character that ECMA-262 does not name is handed over as it
// stands, which PCRE reads as that character
TEST(permissive_escaped_space) {
  const auto regex{sourcemeta::core::to_regex(
      "a\\ b", sourcemeta::core::RegexDialect::Permissive)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::is_regex_ecma("a\\ b"));
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "a b"));
}
