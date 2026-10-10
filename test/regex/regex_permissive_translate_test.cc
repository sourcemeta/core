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
