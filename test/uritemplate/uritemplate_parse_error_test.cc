#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uritemplate.h>

#define EXPECT_URITEMPLATE_PARSE_ERROR(input, expected_column)                 \
  try {                                                                        \
    const sourcemeta::core::URITemplate uri_template{input};                   \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::URITemplateParseError &error) {             \
    EXPECT_EQ(error.column(), expected_column);                                \
  } catch (...) {                                                              \
    FAIL();                                                                    \
  }                                                                            \
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_uritemplate(input))

TEST(unclosed_brace) { EXPECT_URITEMPLATE_PARSE_ERROR("{var", 1); }

TEST(unclosed_brace_at_end) { EXPECT_URITEMPLATE_PARSE_ERROR("foo{", 4); }

TEST(empty_variable) { EXPECT_URITEMPLATE_PARSE_ERROR("{}", 2); }

TEST(unmatched_close_brace) { EXPECT_URITEMPLATE_PARSE_ERROR("foo}bar", 4); }

TEST(invalid_varname_start) { EXPECT_URITEMPLATE_PARSE_ERROR("{-var}", 2); }

TEST(invalid_character_in_varname) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{var!name}", 5);
}

TEST(dot_at_end) { EXPECT_URITEMPLATE_PARSE_ERROR("{var.}", 6); }

TEST(consecutive_dots) { EXPECT_URITEMPLATE_PARSE_ERROR("{foo..bar}", 6); }

TEST(incomplete_percent_encoding) { EXPECT_URITEMPLATE_PARSE_ERROR("{%3}", 2); }

TEST(invalid_percent_encoding) { EXPECT_URITEMPLATE_PARSE_ERROR("{%GG}", 2); }

TEST(reserved_expansion_empty) { EXPECT_URITEMPLATE_PARSE_ERROR("{+}", 3); }

TEST(reserved_expansion_unclosed) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{+var", 1);
}

TEST(fragment_expansion_empty) { EXPECT_URITEMPLATE_PARSE_ERROR("{#}", 3); }

TEST(fragment_expansion_unclosed) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{#var", 1);
}

TEST(variable_list_trailing_comma) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{x,}", 4);
}

TEST(variable_list_empty_between_commas) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{x,,y}", 4);
}

TEST(prefix_zero) { EXPECT_URITEMPLATE_PARSE_ERROR("{var:0}", 6); }

TEST(prefix_too_large) { EXPECT_URITEMPLATE_PARSE_ERROR("{var:10000}", 10); }

TEST(prefix_no_digits) { EXPECT_URITEMPLATE_PARSE_ERROR("{var:}", 6); }

TEST(reserved_operator_equals) { EXPECT_URITEMPLATE_PARSE_ERROR("{=var}", 2); }

TEST(reserved_operator_pipe) { EXPECT_URITEMPLATE_PARSE_ERROR("{|var}", 2); }

TEST(reserved_operator_exclamation) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{!var}", 2);
}

TEST(reserved_operator_at) { EXPECT_URITEMPLATE_PARSE_ERROR("{@var}", 2); }

TEST(prefix_and_explode) { EXPECT_URITEMPLATE_PARSE_ERROR("{var:3*}", 7); }

TEST(explode_and_prefix) { EXPECT_URITEMPLATE_PARSE_ERROR("{var*:3}", 6); }

TEST(variable_list_leading_comma) { EXPECT_URITEMPLATE_PARSE_ERROR("{,x}", 2); }

TEST(nested_open_brace) { EXPECT_URITEMPLATE_PARSE_ERROR("{{var}", 2); }

TEST(space_in_varname) { EXPECT_URITEMPLATE_PARSE_ERROR("{var name}", 5); }

TEST(space_before_varname) { EXPECT_URITEMPLATE_PARSE_ERROR("{ var}", 2); }

TEST(space_after_varname) { EXPECT_URITEMPLATE_PARSE_ERROR("{var }", 5); }

TEST(percent_at_end_of_varname) { EXPECT_URITEMPLATE_PARSE_ERROR("{var%}", 5); }

TEST(percent_one_hex_at_end) { EXPECT_URITEMPLATE_PARSE_ERROR("{var%3}", 5); }

TEST(double_explode) { EXPECT_URITEMPLATE_PARSE_ERROR("{var**}", 6); }

TEST(caret_in_varname) { EXPECT_URITEMPLATE_PARSE_ERROR("{var^name}", 5); }

TEST(backtick_in_varname) { EXPECT_URITEMPLATE_PARSE_ERROR("{var`name}", 5); }

TEST(backslash_in_varname) { EXPECT_URITEMPLATE_PARSE_ERROR("{var\\name}", 5); }

TEST(bracket_in_varname) { EXPECT_URITEMPLATE_PARSE_ERROR("{var[0]}", 5); }

TEST(dot_start_in_list) { EXPECT_URITEMPLATE_PARSE_ERROR("{x,.y}", 4); }

TEST(label_expansion_empty) { EXPECT_URITEMPLATE_PARSE_ERROR("{.}", 3); }

TEST(path_expansion_empty) { EXPECT_URITEMPLATE_PARSE_ERROR("{/}", 3); }

TEST(path_parameter_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{;}", 3);
}

TEST(query_expansion_empty) { EXPECT_URITEMPLATE_PARSE_ERROR("{?}", 3); }

TEST(query_continuation_expansion_empty) {
  EXPECT_URITEMPLATE_PARSE_ERROR("{&}", 3);
}
