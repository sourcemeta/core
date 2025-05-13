#include <gtest/gtest.h>

#include <exception>
#include <sstream>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/json_error.h>

#define __EXPECT_PARSE_ERROR(input, expected_line, expected_column,            \
                             expected_error, expected_message)                 \
  try {                                                                        \
    sourcemeta::core::parse_json((input));                                     \
    FAIL() << "The parse function was expected to throw";                      \
  } catch (const sourcemeta::core::expected_error &error) {                    \
    EXPECT_EQ(error.line(), expected_line);                                    \
    EXPECT_EQ(error.column(), expected_column);                                \
    EXPECT_STREQ(error.what(), expected_message);                              \
    SUCCEED();                                                                 \
  } catch (const std::exception &) {                                           \
    FAIL()                                                                     \
        << "The parse function was expected to throw the desired parse error"; \
  }

#define EXPECT_PARSE_ERROR(input, expected_line, expected_column)              \
  __EXPECT_PARSE_ERROR(input, expected_line, expected_column, JSONParseError,  \
                       "Failed to parse the JSON document");

#define EXPECT_PARSE_ERROR_INTEGER_LIMIT(input, expected_line,                 \
                                         expected_column)                      \
  __EXPECT_PARSE_ERROR(input, expected_line, expected_column,                  \
                       JSONParseIntegerLimitError,                             \
                       "The JSON value is not representable by the IETF RFC "  \
                       "8259 interoperable signed integer range");

TEST(JSON_parse_error, boolean_true_invalid) {
  std::istringstream input{"trrue"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, new_lines_before_invalid_true) {
  std::istringstream input{"\n\n\n\ntrrue"};
  EXPECT_PARSE_ERROR(input, 5, 3);
}

TEST(JSON_parse_error, whitespace_before_invalid_true) {
  std::istringstream input{" \t\rtrrue"};
  EXPECT_PARSE_ERROR(input, 1, 6);
}

TEST(JSON_parse_error, invalid_start_character) {
  std::istringstream input{"xxxxxxx"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, boolean_true_invalid_with_spaces) {
  std::istringstream input{"   trrue   "};
  EXPECT_PARSE_ERROR(input, 1, 6);
}

TEST(JSON_parse_error, invalid_object_delimiter) {
  std::istringstream input{R"EOF({
    "foo": {
      "bar" "baz"
    }
  })EOF"};
  EXPECT_PARSE_ERROR(input, 3, 13);
}

TEST(JSON_parse_error, array_comma_before_element) {
  std::istringstream input{"[,]"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, array_without_end) {
  std::istringstream input{"["};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, empty_array_incomplete_right_with_inner_space) {
  std::istringstream input{"[  "};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, array_empty_with_comma_and_no_right_bracket) {
  std::istringstream input{"[,"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, array_empty_with_comma_and_spacing) {
  std::istringstream input{"[   ,   ]"};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, array_boolean_incomplete) {
  std::istringstream input{"[true"};
  EXPECT_PARSE_ERROR(input, 1, 6);
}

TEST(JSON_parse_error, array_incomplete_boolean) {
  std::istringstream input{"[tru]"};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, array_without_comma_after_element) {
  std::istringstream input{"[1[2]]"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, array_unclosed_string_element) {
  std::istringstream input{"[\"foo]"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(JSON_parse_error, array_single_element_trailing_comma) {
  std::istringstream input{"[true,]"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(JSON_parse_error, array_single_element_trailing_commas) {
  std::istringstream input{"[true,,,]"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(JSON_parse_error, array_single_element_middle_comma) {
  std::istringstream input{"[true,,true]"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(JSON_parse_error, array_single_element_leading_comma) {
  std::istringstream input{"[,true]"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, array_single_element_leading_commas) {
  std::istringstream input{"[,,,true]"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, concat_array_middle_unbalanced_right) {
  std::istringstream input{"[true,[false]"};
  EXPECT_PARSE_ERROR(input, 1, 14);
}

TEST(JSON_parse_error, true_incomplete_1) {
  std::istringstream input{"tru"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, true_incomplete_2) {
  std::istringstream input{"tr"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, true_incomplete_3) {
  std::istringstream input{"t"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, false_incomplete_1) {
  std::istringstream input{"fals"};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, false_incomplete_2) {
  std::istringstream input{"fal"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, false_incomplete_3) {
  std::istringstream input{"fa"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, false_incomplete_4) {
  std::istringstream input{"f"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, null_incomplete_1) {
  std::istringstream input{"nul"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, null_incomplete_2) {
  std::istringstream input{"nu"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, null_incomplete_3) {
  std::istringstream input{"n"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, empty_object_missing_left_curly) {
  std::istringstream input{"}"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, empty_object_missing_right_curly) {
  std::istringstream input{"{"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, object_missing_closing_curly) {
  std::istringstream input{"{\"foo\":{}"};
  EXPECT_PARSE_ERROR(input, 1, 10);
}

TEST(JSON_parse_error, object_integer_key) {
  std::istringstream input{"{1:false}"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, object_colon_but_no_key) {
  std::istringstream input{"{:false}"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, object_colon_but_no_key_with_space) {
  std::istringstream input{"{  :  false  }"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, object_colon_but_no_value) {
  std::istringstream input{"{\"x\":}"};
  EXPECT_PARSE_ERROR(input, 1, 6);
}

TEST(JSON_parse_error, object_colon_but_no_value_with_space) {
  std::istringstream input{"{   \"x\"   :   }"};
  EXPECT_PARSE_ERROR(input, 1, 15);
}

TEST(JSON_parse_error, object_missing_key_left_quote) {
  std::istringstream input{"{foo\":true}"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, object_missing_key_right_quote) {
  std::istringstream input{"{\"foo:true}"};
  EXPECT_PARSE_ERROR(input, 1, 12);
}

TEST(JSON_parse_error, object_missing_key_both_quotes) {
  std::istringstream input{"{foo:true}"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, object_missing_colon) {
  std::istringstream input{"{\"foo\" true}"};
  EXPECT_PARSE_ERROR(input, 1, 8);
}

TEST(JSON_parse_error, object_multiple_colons) {
  std::istringstream input{"{\"foo\"::true}"};
  EXPECT_PARSE_ERROR(input, 1, 8);
}

TEST(JSON_parse_error, object_no_right_curly_bracket) {
  std::istringstream input{"{\"foo\":true"};
  EXPECT_PARSE_ERROR(input, 1, 12);
}

TEST(JSON_parse_error, object_incomplete_boolean) {
  std::istringstream input{"{\"foo\":tru}"};
  EXPECT_PARSE_ERROR(input, 1, 11);
}

TEST(JSON_parse_error, object_key_without_value) {
  std::istringstream input{"{\"foo\"}"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(JSON_parse_error, object_key_without_value_after_valid_key) {
  std::istringstream input{"{\"foo\": 1,\"bar\"}"};
  EXPECT_PARSE_ERROR(input, 1, 16);
}

TEST(JSON_parse_error, object_trailing_comma_after_element) {
  std::istringstream input{"{\"foo\": 1,\"bar\":0,}"};
  EXPECT_PARSE_ERROR(input, 1, 19);
}

TEST(JSON_parse_error, object_trailing_comma) {
  std::istringstream input{"{\"foo\": 1,}"};
  EXPECT_PARSE_ERROR(input, 1, 11);
}

TEST(JSON_parse_error, object_trailing_comma_with_spacing) {
  std::istringstream input{"{\"foo\": 1  ,   } "};
  EXPECT_PARSE_ERROR(input, 1, 16);
}

TEST(JSON_parse_error, object_trailing_comma_with_left_spacing) {
  std::istringstream input{"{\"foo\": 1  ,}"};
  EXPECT_PARSE_ERROR(input, 1, 13);
}

TEST(JSON_parse_error, object_multiple_commas) {
  std::istringstream input{"{\"foo\": 1,,,,,}"};
  EXPECT_PARSE_ERROR(input, 1, 11);
}

TEST(JSON_parse_error, object_multiple_commas_with_spacing) {
  std::istringstream input{"{\"foo\": 1 ,  ,  , , }"};
  EXPECT_PARSE_ERROR(input, 1, 14);
}

TEST(JSON_parse_error, string_no_right_quote) {
  std::istringstream input{"\"foo"};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, string_no_left_quote) {
  std::istringstream input{"foo\""};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, string_single_quotes) {
  std::istringstream input{"'foo'"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, string_only_quote) {
  std::istringstream input{"\""};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, string_escaped_invalid) {
  std::istringstream input{"\"foo\\xbar\""};
  EXPECT_PARSE_ERROR(input, 1, 6);
}

TEST(JSON_parse_error, string_escaped_incomplete) {
  std::istringstream input{"\"foo\\\""};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(JSON_parse_error, string_invalid_control_characters) {
  std::istringstream input00{"\"foo \u0000 bar\""};
  std::istringstream input01{"\"foo \u0001 bar\""};
  std::istringstream input02{"\"foo \u0002 bar\""};
  std::istringstream input03{"\"foo \u0003 bar\""};
  std::istringstream input04{"\"foo \u0004 bar\""};
  std::istringstream input05{"\"foo \u0005 bar\""};
  std::istringstream input06{"\"foo \u0006 bar\""};
  std::istringstream input07{"\"foo \u0007 bar\""};
  std::istringstream input08{"\"foo \u0008 bar\""};
  std::istringstream input09{"\"foo \u0009 bar\""};
  std::istringstream input0A{"\"foo \u000A bar\""};
  std::istringstream input0B{"\"foo \u000B bar\""};
  std::istringstream input0C{"\"foo \u000C bar\""};
  std::istringstream input0D{"\"foo \u000D bar\""};
  std::istringstream input0E{"\"foo \u000E bar\""};
  std::istringstream input0F{"\"foo \u000F bar\""};
  std::istringstream input10{"\"foo \u0010 bar\""};
  std::istringstream input11{"\"foo \u0011 bar\""};
  std::istringstream input12{"\"foo \u0012 bar\""};
  std::istringstream input13{"\"foo \u0013 bar\""};
  std::istringstream input14{"\"foo \u0014 bar\""};
  std::istringstream input15{"\"foo \u0015 bar\""};
  std::istringstream input16{"\"foo \u0016 bar\""};
  std::istringstream input17{"\"foo \u0017 bar\""};
  std::istringstream input18{"\"foo \u0018 bar\""};
  std::istringstream input19{"\"foo \u0019 bar\""};
  std::istringstream input1A{"\"foo \u001A bar\""};
  std::istringstream input1B{"\"foo \u001B bar\""};
  std::istringstream input1C{"\"foo \u001C bar\""};
  std::istringstream input1D{"\"foo \u001D bar\""};
  std::istringstream input1E{"\"foo \u001E bar\""};
  std::istringstream input1F{"\"foo \u001F bar\""};

  EXPECT_PARSE_ERROR(input00, 1, 6);
  EXPECT_PARSE_ERROR(input01, 1, 6);
  EXPECT_PARSE_ERROR(input02, 1, 6);
  EXPECT_PARSE_ERROR(input03, 1, 6);
  EXPECT_PARSE_ERROR(input04, 1, 6);
  EXPECT_PARSE_ERROR(input05, 1, 6);
  EXPECT_PARSE_ERROR(input06, 1, 6);
  EXPECT_PARSE_ERROR(input07, 1, 6);
  EXPECT_PARSE_ERROR(input08, 1, 6);
  EXPECT_PARSE_ERROR(input09, 1, 6);
  EXPECT_PARSE_ERROR(input0A, 1, 6);
  EXPECT_PARSE_ERROR(input0B, 1, 6);
  EXPECT_PARSE_ERROR(input0C, 1, 6);
  EXPECT_PARSE_ERROR(input0D, 1, 6);
  EXPECT_PARSE_ERROR(input0E, 1, 6);
  EXPECT_PARSE_ERROR(input0F, 1, 6);
  EXPECT_PARSE_ERROR(input10, 1, 6);
  EXPECT_PARSE_ERROR(input11, 1, 6);
  EXPECT_PARSE_ERROR(input12, 1, 6);
  EXPECT_PARSE_ERROR(input13, 1, 6);
  EXPECT_PARSE_ERROR(input14, 1, 6);
  EXPECT_PARSE_ERROR(input15, 1, 6);
  EXPECT_PARSE_ERROR(input16, 1, 6);
  EXPECT_PARSE_ERROR(input17, 1, 6);
  EXPECT_PARSE_ERROR(input18, 1, 6);
  EXPECT_PARSE_ERROR(input19, 1, 6);
  EXPECT_PARSE_ERROR(input1A, 1, 6);
  EXPECT_PARSE_ERROR(input1B, 1, 6);
  EXPECT_PARSE_ERROR(input1C, 1, 6);
  EXPECT_PARSE_ERROR(input1D, 1, 6);
  EXPECT_PARSE_ERROR(input1E, 1, 6);
  EXPECT_PARSE_ERROR(input1F, 1, 6);
}

TEST(JSON_parse_error, string_invalid_code_point) {
  std::istringstream input{"\"\\uXXXX\""};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, string_invalid_lowercase_code_point) {
  std::istringstream input{"\"\\uqqqq\""};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, string_incomplete_code_point_0) {
  std::istringstream input{"\"\\u\""};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, string_incomplete_code_point_1) {
  std::istringstream input{"\"\\u6\""};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, string_incomplete_code_point_2) {
  std::istringstream input{"\"\\u2F\""};
  EXPECT_PARSE_ERROR(input, 1, 6);
}

TEST(JSON_parse_error, string_incomplete_code_point_3) {
  std::istringstream input{"\"\\u02F\""};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(JSON_parse_error, string_incomplete_code_point_space) {
  std::istringstream input{"\"\\u0F 2F\""};
  EXPECT_PARSE_ERROR(input, 1, 6);
}

TEST(JSON_parse_error, invalid_integer_trailing_minus) {
  std::istringstream input{"[-0-]"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, invalid_integer_trailing_character) {
  std::istringstream input{"[-0x]"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, invalid_minus_in_between_integer) {
  std::istringstream input{"[123-45]"};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, integer_invalid_double_minus) {
  std::istringstream input{"--123"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, integer_two_zeroes) {
  std::istringstream input{"[00]"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, integer_multiple_zeroes) {
  std::istringstream input{"[000]"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, leading_zero_with_one_digit_integer) {
  std::istringstream input{"[01]"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, leading_zero_with_two_digits_integer) {
  std::istringstream input{"[012]"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, real_leading_period) {
  std::istringstream input{".0"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, real_trailing_period) {
  std::istringstream input{"0."};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, real_trailing_period_and_minus) {
  std::istringstream input{"[0.-]"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, integer_double_minus_at_start) {
  std::istringstream input{"--5"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, integer_leading_minus_and_period) {
  std::istringstream input{"-.0"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, integer_multiple_leading_plus) {
  std::istringstream input{"++5"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, real_multiple_sibling_periods) {
  std::istringstream input{"123..56"};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, real_multiple_separate_periods) {
  std::istringstream input{"[12.34.56]"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(JSON_parse_error, integer_with_plus) {
  std::istringstream input{"+5"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, integer_string_zero_with_plus) {
  std::istringstream input{"+0"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, integer_negative_with_leading_zero) {
  std::istringstream input{"-05"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, integer_slightly_over_64_bit_positive_limit) {
  std::istringstream input{"9223372036854776000"};
  EXPECT_PARSE_ERROR_INTEGER_LIMIT(input, 1, 1);
}

TEST(JSON_parse_error, integer_slightly_over_64_bit_positive_limit_in_object) {
  std::istringstream input{"{\"foo\":9223372036854776000}"};
  EXPECT_PARSE_ERROR_INTEGER_LIMIT(input, 1, 8);
}

TEST(JSON_parse_error, integer_negative_with_leading_zero_and_space) {
  std::istringstream input{"[-0 5]"};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, leading_zero_positive_integer_number) {
  std::istringstream input{"[02]"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, leading_zero_negative_integer_number) {
  std::istringstream input{"[-02]"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, two_leading_zeroes_real_number) {
  std::istringstream input{"[-00.2]"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, multiple_leading_zeroes_real_number) {
  std::istringstream input{"[-00000.2]"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, huge_negative_exponent) {
  std::istringstream input{"123.456e-789"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, exponential_notation_error_double_upper_e) {
  std::istringstream input{"3EE2"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, exponential_notation_error_double_lower_e) {
  std::istringstream input{"3ee2"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, exponential_notation_error_double_mixed_e) {
  std::istringstream input{"3eE2"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, exponential_notation_error_trailing_upper_e) {
  std::istringstream input{"3E"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, exponential_notation_error_trailing_lower_e) {
  std::istringstream input{"3e"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, exponential_notation_error_trailing_upper_e_minus) {
  std::istringstream input{"3E-"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, exponential_notation_error_trailing_lower_e_minus) {
  std::istringstream input{"3e-"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, exponential_notation_error_leading_upper_e) {
  std::istringstream input{"E2"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, exponential_notation_error_leading_lower_e) {
  std::istringstream input{"e2"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, exponential_notation_error_minus_leading_upper_e) {
  std::istringstream input{"-E2"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, exponential_notation_error_minus_leading_lower_e) {
  std::istringstream input{"-e2"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(JSON_parse_error, exponential_notation_error_double_e_with_digits) {
  std::istringstream input{"[3E1E2]"};
  EXPECT_PARSE_ERROR(input, 1, 5);
}

TEST(JSON_parse_error, exponential_notation_error_left_e_space) {
  std::istringstream input{"[3 E2]"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error, exponential_notation_error_right_e_space) {
  std::istringstream input{"3E 2"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(JSON_parse_error, exponential_notation_error_double_minus_after_e) {
  std::istringstream input{"3E--2"};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(JSON_parse_error,
     exponential_notation_error_double_minus_with_digits_after_e) {
  std::istringstream input{"[3E-2-2]"};
  EXPECT_PARSE_ERROR(input, 1, 6);
}

TEST(JSON_parse_error, backspace_is_not_whitespace) {
  std::istringstream input{"\bfalse\b"};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(JSON_parse_error, read_json_invalid_1) {
  try {
    sourcemeta::core::read_json(std::filesystem::path{TEST_DIRECTORY} /
                                "stub_invalid_1.json");
  } catch (const sourcemeta::core::JSONFileParseError &error) {
    EXPECT_EQ(error.path(),
              std::filesystem::path{TEST_DIRECTORY} / "stub_invalid_1.json");
    EXPECT_EQ(error.line(), 3);
    EXPECT_EQ(error.column(), 9);
    EXPECT_STREQ(error.what(), "Failed to parse the JSON document");
  } catch (...) {
    FAIL() << "The parse function was expected to throw a file parse error";
  }
}

TEST(JSON_parse_error, read_json_invalid_2) {
  try {
    sourcemeta::core::read_json(std::filesystem::path{TEST_DIRECTORY} /
                                "stub_invalid_2.json");
  } catch (const sourcemeta::core::JSONFileParseError &error) {
    EXPECT_EQ(error.path(),
              std::filesystem::path{TEST_DIRECTORY} / "stub_invalid_2.json");
    EXPECT_EQ(error.line(), 2);
    EXPECT_EQ(error.column(), 1);
    EXPECT_STREQ(error.what(), "Failed to parse the JSON document");
  } catch (...) {
    FAIL() << "The parse function was expected to throw a file parse error";
  }
}

TEST(JSON_parse_error, read_json_invalid_bigint) {
  try {
    sourcemeta::core::read_json(std::filesystem::path{TEST_DIRECTORY} /
                                "stub_bigint.json");
  } catch (const sourcemeta::core::JSONFileParseError &error) {
    EXPECT_EQ(error.path(),
              std::filesystem::path{TEST_DIRECTORY} / "stub_bigint.json");
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 1);
    EXPECT_STREQ(error.what(),
                 "The JSON value is not representable by the IETF RFC 8259 "
                 "interoperable signed integer range");
  } catch (...) {
    FAIL() << "The parse function was expected to throw a file parse error";
  }
}

TEST(JSON_parse_error, read_json_directory) {
  try {
    sourcemeta::core::read_json(std::filesystem::path{TEST_DIRECTORY});
  } catch (const std::filesystem::filesystem_error &error) {
    EXPECT_EQ(error.code(), std::errc::is_a_directory);
    EXPECT_EQ(error.path1(), std::filesystem::path{TEST_DIRECTORY});
  } catch (...) {
    FAIL() << "The parse function was expected to throw a filesystem error";
  }
}

TEST(JSON_parse_error, read_json_non_existent) {
  try {
    sourcemeta::core::read_json(std::filesystem::path{TEST_DIRECTORY} /
                                "i-dont-exist");
  } catch (const std::filesystem::filesystem_error &error) {
    EXPECT_EQ(error.code(), std::errc::no_such_file_or_directory);
    EXPECT_EQ(error.path1(),
              std::filesystem::path{TEST_DIRECTORY} / "i-dont-exist");
  } catch (...) {
    FAIL() << "The parse function was expected to throw a filesystem error";
  }
}
