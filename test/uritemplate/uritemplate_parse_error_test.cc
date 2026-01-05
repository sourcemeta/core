#include <gtest/gtest.h>

#include <sourcemeta/core/uritemplate.h>

#define EXPECT_URITEMPLATE_PARSE_ERROR(input, expected_column)                 \
  try {                                                                        \
    const sourcemeta::core::URITemplate uri_template{input};                   \
    FAIL() << "Expected parse error for: " << input;                           \
  } catch (const sourcemeta::core::URITemplateParseError &error) {             \
    EXPECT_EQ(error.column(), expected_column);                                \
  }

TEST(URITemplate_parse_error, unclosed_brace) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var", 1);
}

TEST(URITemplate_parse_error, unclosed_brace_at_end) {
  EXPECT_URITEMPLATE_PARSE_ERROR("foo{", 4);
}

TEST(URITemplate_parse_error, empty_variable) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{}", 2);
}

TEST(URITemplate_parse_error, unmatched_close_brace) {
  EXPECT_URITEMPLATE_PARSE_ERROR("foo}bar", 4);
}

TEST(URITemplate_parse_error, invalid_varname_start) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{-var}", 2);
}

TEST(URITemplate_parse_error, invalid_character_in_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var!name}", 5);
}

TEST(URITemplate_parse_error, dot_at_end) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var.}", 6);
}

TEST(URITemplate_parse_error, consecutive_dots) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{foo..bar}", 6);
}

TEST(URITemplate_parse_error, incomplete_percent_encoding) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{%3}", 2);
}

TEST(URITemplate_parse_error, invalid_percent_encoding) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{%GG}", 2);
}

TEST(URITemplate_parse_error, reserved_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{+}", 3);
}

TEST(URITemplate_parse_error, reserved_expansion_unclosed) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{+var", 1);
}

TEST(URITemplate_parse_error, fragment_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{#}", 3);
}

TEST(URITemplate_parse_error, fragment_expansion_unclosed) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{#var", 1);
}

TEST(URITemplate_parse_error, variable_list_trailing_comma) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{x,}", 4);
}

TEST(URITemplate_parse_error, variable_list_empty_between_commas) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{x,,y}", 4);
}

TEST(URITemplate_parse_error, prefix_zero) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var:0}", 6);
}

TEST(URITemplate_parse_error, prefix_too_large) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var:10000}", 10);
}

TEST(URITemplate_parse_error, prefix_no_digits) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var:}", 6);
}

TEST(URITemplate_parse_error, reserved_operator_equals) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{=var}", 2);
}

TEST(URITemplate_parse_error, reserved_operator_pipe) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{|var}", 2);
}

TEST(URITemplate_parse_error, reserved_operator_exclamation) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{!var}", 2);
}

TEST(URITemplate_parse_error, reserved_operator_at) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{@var}", 2);
}

TEST(URITemplate_parse_error, prefix_and_explode) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var:3*}", 7);
}

TEST(URITemplate_parse_error, explode_and_prefix) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var*:3}", 6);
}

TEST(URITemplate_parse_error, variable_list_leading_comma) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{,x}", 2);
}

TEST(URITemplate_parse_error, nested_open_brace) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{{var}", 2);
}

TEST(URITemplate_parse_error, space_in_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var name}", 5);
}

TEST(URITemplate_parse_error, space_before_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{ var}", 2);
}

TEST(URITemplate_parse_error, space_after_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var }", 5);
}

TEST(URITemplate_parse_error, percent_at_end_of_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var%}", 5);
}

TEST(URITemplate_parse_error, percent_one_hex_at_end) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var%3}", 5);
}

TEST(URITemplate_parse_error, double_explode) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var**}", 6);
}

TEST(URITemplate_parse_error, caret_in_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var^name}", 5);
}

TEST(URITemplate_parse_error, backtick_in_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var`name}", 5);
}

TEST(URITemplate_parse_error, backslash_in_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var\\name}", 5);
}

TEST(URITemplate_parse_error, bracket_in_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var[0]}", 5);
}

TEST(URITemplate_parse_error, dot_start_in_list) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{x,.y}", 4);
}

TEST(URITemplate_parse_error, label_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{.}", 3);
}

TEST(URITemplate_parse_error, path_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{/}", 3);
}

TEST(URITemplate_parse_error, path_parameter_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{;}", 3);
}

TEST(URITemplate_parse_error, query_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{?}", 3);
}

TEST(URITemplate_parse_error, query_continuation_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{&}", 3);
}
