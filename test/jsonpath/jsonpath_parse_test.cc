#include <sourcemeta/core/jsonpath.h>
#include <sourcemeta/core/test.h>

#include <cstddef>
#include <string>

#define EXPECT_JSONPATH_PARSE_ERROR(input, expected_column)                    \
  try {                                                                        \
    const sourcemeta::core::JSONPath path{input};                              \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::JSONPathParseError &error) {                \
    EXPECT_EQ(error.column(), expected_column);                                \
  } catch (...) {                                                              \
    FAIL();                                                                    \
  }

#define EXPECT_JSONPATH_VALID(input)                                           \
  {                                                                            \
    const sourcemeta::core::JSONPath path{input};                              \
    EXPECT_TRUE(true);                                                         \
  }

TEST(jsonpath_parse_root_only) { EXPECT_JSONPATH_VALID("$"); }

TEST(jsonpath_parse_shorthand_name) { EXPECT_JSONPATH_VALID("$.foo"); }

TEST(jsonpath_parse_shorthand_chain) { EXPECT_JSONPATH_VALID("$.foo.bar"); }

TEST(jsonpath_parse_shorthand_underscore) { EXPECT_JSONPATH_VALID("$._foo"); }

TEST(jsonpath_parse_shorthand_non_ascii) { EXPECT_JSONPATH_VALID("$.á"); }

TEST(jsonpath_parse_bracket_single_quoted) { EXPECT_JSONPATH_VALID("$['a']"); }

TEST(jsonpath_parse_bracket_double_quoted) {
  EXPECT_JSONPATH_VALID("$[\"a\"]");
}

TEST(jsonpath_parse_bracket_index) { EXPECT_JSONPATH_VALID("$[0]"); }

TEST(jsonpath_parse_bracket_negative_index) { EXPECT_JSONPATH_VALID("$[-1]"); }

TEST(jsonpath_parse_wildcard_shorthand) { EXPECT_JSONPATH_VALID("$.*"); }

TEST(jsonpath_parse_wildcard_bracket) { EXPECT_JSONPATH_VALID("$[*]"); }

TEST(jsonpath_parse_descendant_shorthand) { EXPECT_JSONPATH_VALID("$..foo"); }

TEST(jsonpath_parse_descendant_wildcard) { EXPECT_JSONPATH_VALID("$..*"); }

TEST(jsonpath_parse_descendant_bracket) { EXPECT_JSONPATH_VALID("$..[0]"); }

TEST(jsonpath_parse_slice_full) { EXPECT_JSONPATH_VALID("$[1:5:2]"); }

TEST(jsonpath_parse_slice_no_step) { EXPECT_JSONPATH_VALID("$[1:5]"); }

TEST(jsonpath_parse_slice_open_ended) { EXPECT_JSONPATH_VALID("$[1:]"); }

TEST(jsonpath_parse_slice_open_start) { EXPECT_JSONPATH_VALID("$[:5]"); }

TEST(jsonpath_parse_slice_bare_colon) { EXPECT_JSONPATH_VALID("$[:]"); }

TEST(jsonpath_parse_slice_negative_step) { EXPECT_JSONPATH_VALID("$[::-1]"); }

TEST(jsonpath_parse_multiple_selectors) {
  EXPECT_JSONPATH_VALID("$['a', 'b', 1]");
}

TEST(jsonpath_parse_filter_existence) { EXPECT_JSONPATH_VALID("$[?@.a]"); }

TEST(jsonpath_parse_filter_comparison) {
  EXPECT_JSONPATH_VALID("$[?@.a == 1]");
}

TEST(jsonpath_parse_filter_logical) {
  EXPECT_JSONPATH_VALID("$[?@.a && (@.b || !@.c)]");
}

TEST(jsonpath_parse_filter_functions) {
  EXPECT_JSONPATH_VALID("$[?length(@.a) >= 2 && match(@.b, 'x.*')]");
}

TEST(jsonpath_parse_filter_nested_function) {
  EXPECT_JSONPATH_VALID("$[?length(value($..a)) == 3]");
}

TEST(jsonpath_parse_name_with_escapes) {
  EXPECT_JSONPATH_VALID("$['\\n\\t\\\\\\'']");
}

TEST(jsonpath_parse_name_with_unicode_escape) {
  EXPECT_JSONPATH_VALID("$['\\u0041']");
}

TEST(jsonpath_parse_name_with_surrogate_pair) {
  EXPECT_JSONPATH_VALID("$['\\uD834\\uDD1E']");
}

TEST(jsonpath_parse_whitespace_between_segments) {
  EXPECT_JSONPATH_VALID("$ .foo");
}

TEST(jsonpath_parse_whitespace_in_brackets) {
  EXPECT_JSONPATH_VALID("$[ 'a' , 1 ]");
}

TEST(jsonpath_parse_empty) { EXPECT_JSONPATH_PARSE_ERROR("", 1); }

TEST(jsonpath_parse_missing_root) { EXPECT_JSONPATH_PARSE_ERROR(".foo", 1); }

TEST(jsonpath_parse_double_root) { EXPECT_JSONPATH_PARSE_ERROR("$$", 2); }

TEST(jsonpath_parse_trailing_garbage) {
  EXPECT_JSONPATH_PARSE_ERROR("$.a!", 4);
}

TEST(jsonpath_parse_trailing_whitespace) {
  EXPECT_JSONPATH_PARSE_ERROR("$.a ", 4);
}

TEST(jsonpath_parse_trailing_dot) { EXPECT_JSONPATH_PARSE_ERROR("$.", 3); }

TEST(jsonpath_parse_dot_before_bracket) {
  EXPECT_JSONPATH_PARSE_ERROR("$.['a']", 3);
}

TEST(jsonpath_parse_bare_descendant) { EXPECT_JSONPATH_PARSE_ERROR("$..", 4); }

TEST(jsonpath_parse_triple_dot) { EXPECT_JSONPATH_PARSE_ERROR("$...foo", 4); }

TEST(jsonpath_parse_shorthand_starting_digit) {
  EXPECT_JSONPATH_PARSE_ERROR("$.1foo", 3);
}

TEST(jsonpath_parse_whitespace_inside_shorthand) {
  EXPECT_JSONPATH_PARSE_ERROR("$. foo", 3);
}

TEST(jsonpath_parse_unterminated_bracket) {
  EXPECT_JSONPATH_PARSE_ERROR("$['a'", 6);
}

TEST(jsonpath_parse_empty_bracket) { EXPECT_JSONPATH_PARSE_ERROR("$[]", 3); }

TEST(jsonpath_parse_unterminated_string) {
  EXPECT_JSONPATH_PARSE_ERROR("$['a]", 6);
}

TEST(jsonpath_parse_raw_control_in_string) {
  EXPECT_JSONPATH_PARSE_ERROR("$['\n']", 4);
}

TEST(jsonpath_parse_invalid_escape) {
  EXPECT_JSONPATH_PARSE_ERROR("$['\\x']", 5);
}

TEST(jsonpath_parse_wrong_quote_escape) {
  EXPECT_JSONPATH_PARSE_ERROR("$[\"\\'\"]", 5);
}

TEST(jsonpath_parse_lone_high_surrogate) {
  EXPECT_JSONPATH_PARSE_ERROR("$['\\uD834']", 10);
}

TEST(jsonpath_parse_lone_low_surrogate) {
  EXPECT_JSONPATH_PARSE_ERROR("$['\\uDD1E']", 10);
}

TEST(jsonpath_parse_index_leading_zero) {
  EXPECT_JSONPATH_PARSE_ERROR("$[01]", 4);
}

TEST(jsonpath_parse_index_negative_zero) {
  EXPECT_JSONPATH_PARSE_ERROR("$[-0]", 5);
}

TEST(jsonpath_parse_index_too_large) {
  EXPECT_JSONPATH_PARSE_ERROR("$[9007199254740992]", 19);
}

TEST(jsonpath_parse_index_fractional) {
  EXPECT_JSONPATH_PARSE_ERROR("$[1.5]", 4);
}

TEST(jsonpath_parse_slice_step_leading_zero) {
  EXPECT_JSONPATH_PARSE_ERROR("$[1:2:03]", 8);
}

TEST(jsonpath_parse_filter_empty) { EXPECT_JSONPATH_PARSE_ERROR("$[?]", 4); }

TEST(jsonpath_parse_filter_literal_alone) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?42]", 6);
}

TEST(jsonpath_parse_filter_single_equal) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?@.a = 1]", 8);
}

TEST(jsonpath_parse_filter_non_singular_comparison) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?@.* == 1]", 8);
}

TEST(jsonpath_parse_filter_descendant_comparison) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?@..a == 1]", 9);
}

TEST(jsonpath_parse_filter_unknown_function) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?foo(@.a)]", 7);
}

TEST(jsonpath_parse_filter_length_no_arguments) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?length() == 1]", 12);
}

TEST(jsonpath_parse_filter_length_two_arguments) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?length(@.a, @.b) == 1]", 20);
}

TEST(jsonpath_parse_filter_length_non_singular_argument) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?length(@.*) == 1]", 15);
}

TEST(jsonpath_parse_filter_count_literal_argument) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?count(1) == 1]", 12);
}

TEST(jsonpath_parse_filter_count_as_test) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?count(@..*)]", 15);
}

TEST(jsonpath_parse_filter_length_as_test) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?length(@.a)]", 15);
}

TEST(jsonpath_parse_filter_match_compared) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?match(@.a, 'x') == true]", 20);
}

TEST(jsonpath_parse_filter_match_one_argument) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?match(@.a)]", 14);
}

TEST(jsonpath_parse_filter_value_as_test) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?value(@..a)]", 15);
}

TEST(jsonpath_parse_filter_number_leading_zero) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?@.a == 01]", 12);
}

TEST(jsonpath_parse_filter_number_bare_fraction) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?@.a == 1.]", 13);
}

TEST(jsonpath_parse_filter_number_bare_exponent) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?@.a == 1e]", 13);
}

TEST(jsonpath_parse_filter_unterminated_parenthesis) {
  EXPECT_JSONPATH_PARSE_ERROR("$[?(@.a]", 8);
}

TEST(jsonpath_parse_filter_number_negative_zero_allowed) {
  EXPECT_JSONPATH_VALID("$[?@.a == -0]");
}

TEST(jsonpath_parse_filter_number_real_allowed) {
  EXPECT_JSONPATH_VALID("$[?@.a == 1.5e-2]");
}

TEST(jsonpath_parse_overlong_utf8_in_name){
    EXPECT_JSONPATH_PARSE_ERROR("$['\xE0\x80\xAF']", 4)}

TEST(jsonpath_parse_surrogate_utf8_in_name){
    EXPECT_JSONPATH_PARSE_ERROR("$['\xED\xA0\x80']", 4)}

TEST(jsonpath_parse_utf8_beyond_unicode_in_name){
    EXPECT_JSONPATH_PARSE_ERROR("$['\xF4\x90\x80\x80']", 4)}

TEST(jsonpath_parse_overlong_utf8_in_shorthand){
    EXPECT_JSONPATH_PARSE_ERROR("$.\xE0\x80\xAF", 3)}

TEST(jsonpath_parse_deep_parenthesis_nesting_rejected) {
  std::string expression{"$[?"};
  expression += std::string(100, '(');
  expression += "@.a";
  expression += std::string(100, ')');
  expression += "]";
  try {
    const sourcemeta::core::JSONPath path{expression};
    FAIL();
  } catch (const sourcemeta::core::JSONPathParseError &error) {
    EXPECT_TRUE(error.column() > 0);
  } catch (...) {
    FAIL();
  }
}

TEST(jsonpath_parse_deep_filter_nesting_rejected) {
  std::string expression{"$"};
  for (std::size_t index = 0; index < 100; ++index) {
    expression += "[?@";
  }

  expression += std::string(100, ']');
  try {
    const sourcemeta::core::JSONPath path{expression};
    FAIL();
  } catch (const sourcemeta::core::JSONPathParseError &error) {
    EXPECT_TRUE(error.column() > 0);
  } catch (...) {
    FAIL();
  }
}

TEST(jsonpath_parse_deep_function_nesting_rejected) {
  std::string expression{"$[?"};
  for (std::size_t index = 0; index < 100; ++index) {
    expression += "length(";
  }

  expression += "@.a";
  expression += std::string(100, ')');
  expression += " == 1]";
  try {
    const sourcemeta::core::JSONPath path{expression};
    FAIL();
  } catch (const sourcemeta::core::JSONPathParseError &error) {
    EXPECT_TRUE(error.column() > 0);
  } catch (...) {
    FAIL();
  }
}

TEST(jsonpath_parse_invalid_selector_character) {
  EXPECT_JSONPATH_PARSE_ERROR("$[!]", 3);
}
