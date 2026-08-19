#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <string_view>

TEST(valid_zero) { EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0")); }

TEST(valid_single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("1"));
}

TEST(valid_multi_digit) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("120"));
}

TEST(valid_large_integer) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("1234567890"));
}

TEST(valid_zero_with_pointer) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/foo/bar"));
}

TEST(valid_zero_with_empty_pointer) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/"));
}

TEST(suite_valid_up_and_then_down) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("2/0/baz/1/zip"));
}

TEST(suite_valid_member_or_index_name) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0#"));
}

TEST(valid_multi_digit_hash) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("42#"));
}

TEST(valid_pointer_with_escape) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/~0/~1"));
}

TEST(suite_invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer(""));
}

TEST(invalid_leading_zero_with_digit) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("01"));
}

TEST(suite_invalid_leading_zero_with_pointer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("01/a"));
}

TEST(suite_invalid_leading_zero_with_hash) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("01#"));
}

TEST(invalid_double_zero) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("00"));
}

TEST(suite_invalid_negative_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("-1/foo/bar"));
}

TEST(suite_invalid_positive_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("+1/foo/bar"));
}

TEST(suite_invalid_valid_json_pointer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("/foo/bar"));
}

TEST(invalid_just_hash) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("#"));
}

TEST(suite_invalid_double_hash) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0##"));
}

TEST(invalid_hash_with_trailing) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0#a"));
}

TEST(invalid_integer_then_letter) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0x"));
}

TEST(invalid_bad_pointer_escape) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0/~"));
}

TEST(invalid_bad_pointer_escape_digit) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0/~2"));
}

TEST(invalid_letter_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("a/foo"));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer(" 0"));
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0 "));
}

TEST(suite_valid_upwards) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("1"));
}

TEST(suite_valid_downwards) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/foo/bar"));
}

TEST(suite_valid_multi_digit_prefix) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("120/foo/bar"));
}

TEST(valid_max_digit_alone) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("9"));
}

TEST(valid_single_nonzero_hash) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("1#"));
}

TEST(valid_multiple_empty_tokens) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0//"));
}

TEST(valid_trailing_empty_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/foo/"));
}

TEST(valid_tilde_zero_escape) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/~0"));
}

TEST(valid_tilde_one_escape) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/~1"));
}

TEST(valid_adjacent_escapes) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/~0~1"));
}

TEST(valid_escape_then_literal_digit) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/~01"));
}

TEST(valid_space_inside_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/foo bar"));
}

TEST(valid_hash_inside_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/#"));
}

TEST(valid_non_ascii_inside_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/caf\xc3\xa9"));
}

TEST(invalid_lone_tilde) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0/~"));
}

TEST(invalid_double_tilde) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0/~~"));
}

TEST(invalid_hash_followed_by_digit) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0#0"));
}

TEST(invalid_hash_followed_by_space) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0# "));
}

TEST(invalid_hash_followed_by_slash) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0#/foo"));
}

TEST(invalid_only_space) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer(" "));
}

TEST(invalid_tab_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\t0"));
}

TEST(invalid_newline) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\n"));
}

TEST(invalid_trailing_newline) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0\n"));
}

TEST(invalid_bengali_digit_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xe0\xa7\xa8"
                                                     "/foo"));
}

TEST(invalid_decimal_point) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0.5"));
}

TEST(invalid_exponent_notation) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("1e5"));
}

TEST(invalid_hex_digits) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0xa"));
}

// integer magnitude - the ABNF puts no upper bound on non-negative-integer
TEST(valid_integer_uint32_max) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("4294967295"));
}

TEST(valid_integer_uint32_max_plus_one) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("4294967296"));
}

TEST(valid_integer_uint32_max_hash) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("4294967295#"));
}

TEST(valid_integer_uint32_max_plus_one_hash) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("4294967296#"));
}

TEST(valid_integer_int64_max) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("9223372036854775807"));
}

TEST(valid_integer_int64_max_plus_one_hash) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("9223372036854775808#"));
}

TEST(valid_integer_uint64_max_plus_one) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("18446744073709551616"));
}

TEST(valid_integer_hundred_digits) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer(
      "111111111111111111111111111111111111111111111111111111111111111111111111"
      "1111111111111111111111111111"));
}

TEST(valid_integer_hundred_digits_pointer) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer(
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "9999999999999999999999999999/foo"));
}

// non-ASCII digit families in the integer - every script, not just one
TEST(invalid_arabic_indic_digit_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xd9\xa1/foo"));
}

TEST(invalid_fullwidth_digit_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer(
      "\xef\xbc\x91\xef\xbc\x92\xef\xbc\x93#"));
}

TEST(invalid_superscript_digit_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xc2\xb2/foo"));
}

TEST(invalid_circled_digit_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xe2\x91\xa0/foo"));
}

TEST(invalid_mathematical_bold_digit_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xf0\x9d\x9f\x8f/foo"));
}

TEST(invalid_ethiopic_digit_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xe1\x8d\xa9/foo"));
}

TEST(invalid_non_ascii_digit_after_ascii_digit) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("1\xd9\xa0/foo"));
}

TEST(invalid_non_ascii_digit_then_ascii_digit) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xc2\xb2"
                                                     "3/foo"));
}

TEST(invalid_devanagari_digit_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xe0\xa5\xa7/foo"));
}

TEST(invalid_fullwidth_digit_alone) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xef\xbc\x90"));
}

// index manipulation - no such production in
// draft-handrews-relative-json-pointer-01
TEST(invalid_index_manipulation_plus) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0+1"));
}

TEST(invalid_index_manipulation_minus) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("1-2"));
}

TEST(invalid_index_manipulation_with_pointer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0+1/foo"));
}

TEST(invalid_index_manipulation_with_hash) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("2+10#"));
}

TEST(invalid_index_manipulation_leading_zero_operand) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0+01"));
}

TEST(invalid_index_manipulation_zero_operand) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0+0"));
}

TEST(invalid_index_manipulation_no_operand) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0+"));
}

TEST(invalid_index_manipulation_double_sign) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0+-1"));
}

TEST(invalid_index_manipulation_non_numeric) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0+A"));
}

// octothorpe placement - the "#" alternative is the whole remainder
TEST(invalid_hash_then_pointer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("1#/foo/bar"));
}

TEST(invalid_hash_then_slash_only) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0#/"));
}

TEST(invalid_hash_then_hash_then_pointer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0##/foo"));
}

TEST(valid_hash_after_multi_digit) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("120#"));
}

TEST(invalid_bare_hash_with_pointer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("#/foo"));
}

TEST(valid_hash_inside_reference_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/a#b"));
}

// integer prefix not followed by "/" or "#"
TEST(invalid_integer_then_letters_then_pointer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("2foo/bar"));
}

TEST(invalid_integer_then_space_then_pointer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0 /foo"));
}

TEST(invalid_integer_then_dot) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0.5/foo"));
}

TEST(invalid_integer_then_percent_encoding) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("%30/foo"));
}

// reference-token alphabet - unescaped = %x00-2E / %x30-7D / %x7F-10FFFF
TEST(valid_del_in_reference_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/\x7f"));
}

TEST(valid_right_brace_boundary_in_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/}"));
}

TEST(valid_period_boundary_in_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/."));
}

TEST(valid_astral_in_reference_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/\xf0\x9f\x98\x8e"));
}

TEST(valid_max_codepoint_in_reference_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/\xf4\x8f\xbf\xbf"));
}

TEST(valid_control_character_in_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/\x01"));
}

TEST(valid_newline_inside_reference_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/foo\nbar"));
}

TEST(valid_carriage_return_inside_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/foo\rbar"));
}

TEST(valid_tab_inside_reference_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/foo\tbar"));
}

// embedded NUL - legal in a token; a truncating validator would differ
TEST(valid_nul_inside_reference_token) {
  static constexpr char VALUE[]{"0/foo\x00"
                                "bar"};
  static_assert(sizeof(VALUE) - 1 == 9,
                "string literal length changed - check \\xNN escaping");
  EXPECT_TRUE(
      sourcemeta::core::is_relative_pointer(std::string_view{VALUE, 9}));
}

TEST(invalid_nul_then_dangling_tilde) {
  static constexpr char VALUE[]{"0/foo\x00~"};
  static_assert(sizeof(VALUE) - 1 == 7,
                "string literal length changed - check \\xNN escaping");
  EXPECT_FALSE(
      sourcemeta::core::is_relative_pointer(std::string_view{VALUE, 7}));
}

TEST(invalid_nul_before_integer) {
  static constexpr char VALUE[]{"\x00"
                                "0"};
  static_assert(sizeof(VALUE) - 1 == 2,
                "string literal length changed - check \\xNN escaping");
  EXPECT_FALSE(
      sourcemeta::core::is_relative_pointer(std::string_view{VALUE, 2}));
}

// escape sequences inherited from RFC 6901 section 3 through the wrapper
TEST(invalid_dangling_tilde_after_token) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0/foo/bar~"));
}

TEST(invalid_tilde_then_letter) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0/~a"));
}

TEST(invalid_tilde_then_slash) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0/~/foo"));
}

TEST(valid_escaped_slash_in_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/a~1b"));
}

TEST(valid_escaped_tilde_in_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/m~0n"));
}

TEST(valid_multiple_escapes_in_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/~1~0~0~1~1"));
}

TEST(valid_escape_then_fraction) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/~1.1"));
}

TEST(invalid_tilde_at_end_of_multi_token) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0/~0/~"));
}

// terminator and whitespace placement around the whole value
TEST(invalid_trailing_carriage_return) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0\r"));
}

TEST(invalid_trailing_crlf) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0\r\n"));
}

TEST(invalid_trailing_vertical_tab) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0\x0b"));
}

TEST(invalid_trailing_form_feed) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0\x0c"));
}

TEST(invalid_leading_nbsp) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xc2\xa0"
                                                     "0"));
}

TEST(invalid_trailing_nbsp) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0\xc2\xa0"));
}

TEST(invalid_leading_bom) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("\xef\xbb\xbf"
                                                     "0"));
}

TEST(invalid_line_separator_after_integer) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0\xe2\x80\xa8"));
}

TEST(valid_nbsp_inside_reference_token) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/a\xc2\xa0"
                                                    "b"));
}

// leading zero rule - only a first "0" before more digits is barred
TEST(valid_interior_zero_then_digit) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("100"));
}

TEST(valid_interior_zero_run) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("1000000"));
}

TEST(valid_terminal_zero) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("10"));
}

TEST(valid_interior_zero_with_hash) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("205#"));
}

TEST(invalid_leading_zero_long_run) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("0000001"));
}

TEST(invalid_double_zero_hash) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("00#"));
}

TEST(invalid_leading_zero_then_pointer_deep) {
  EXPECT_FALSE(sourcemeta::core::is_relative_pointer("01/foo/bar"));
}

// empty reference tokens - reference-token = *( unescaped / escaped )
TEST(valid_three_empty_tokens) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0///"));
}

TEST(valid_empty_token_between_named) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/foo//bar"));
}

TEST(valid_trailing_empty_after_escape) {
  EXPECT_TRUE(sourcemeta::core::is_relative_pointer("0/~0/"));
}
