#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

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
