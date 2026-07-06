#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string>      // std::string
#include <string_view> // std::string_view

TEST(empty_within_zero_zero) {
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within("", 0, 0));
}

TEST(empty_within_zero_five) {
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within("", 0, 5));
}

TEST(empty_below_minimum) {
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("", 1, 5));
}

TEST(fewer_bytes_than_minimum_ascii) {
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("ab", 3, 5));
}

TEST(fewer_bytes_than_minimum_two_byte) {
  // One code point, two bytes, still below a three-code-point minimum
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("\xc3\xa9", 3, 5));
}

TEST(one_byte_below_minimum) {
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("a", 2, 4));
}

TEST(too_many_bytes_ascii) {
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("abcdefghij", 0, 2));
}

TEST(too_many_bytes_four_byte) {
  // Five four-byte code points, twenty bytes, cannot be at most four
  const std::string_view input{
      "\xf0\x9f\x98\x80\xf0\x9f\x98\x80\xf0\x9f\x98\x80\xf0\x9f\x98\x80"
      "\xf0\x9f\x98\x80"};
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(input, 0, 4));
}

TEST(too_many_bytes_boundary_rejected) {
  // Nine bytes: ceil(9 / 4) == 3, which exceeds a maximum of two
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("aaaaaaaaa", 0, 2));
}

TEST(too_many_bytes_boundary_not_rejected) {
  // Eight bytes: ceil(8 / 4) == 2, which does not exceed a maximum of two, so
  // the byte-length shortcut cannot reject and the scan resolves it
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("aaaaaaaa", 0, 2));
}

TEST(accept_ascii_within) {
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within("hello", 1, 10));
}

TEST(accept_large_maximum) {
  const std::string input(100, 'a');
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(input, 1, 1000));
}

TEST(accept_same_byte_length_ascii) {
  // Eight ASCII bytes, eight code points, accepted by the shortcut
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within("abcdefgh", 1, 8));
}

TEST(accept_same_byte_length_two_byte) {
  // Eight bytes, four two-byte code points, accepted by the same shortcut
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(
      "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9", 1, 8));
}

TEST(accept_same_byte_length_four_byte) {
  // Eight bytes, two four-byte code points, accepted by the same shortcut
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(
      "\xf0\x9f\x98\x80\xf0\x9f\x98\x80", 1, 8));
}

TEST(accept_boundary_minimum_equals_lower_bound_ascii) {
  // Eight bytes, ceil(8 / 4) == 2 == minimum, byte length == maximum
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within("abcdefgh", 2, 8));
}

TEST(accept_boundary_minimum_equals_lower_bound_four_byte) {
  // Same bounds, but two code points; still accepted since 2 >= minimum 2
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(
      "\xf0\x9f\x98\x80\xf0\x9f\x98\x80", 2, 8));
}

TEST(scan_bytes_exceed_maximum_but_count_fits_two_byte) {
  // Six bytes, three code points, within [1, 4]
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(
      "\xc3\xa9\xc3\xa9\xc3\xa9", 1, 4));
}

TEST(scan_bytes_exceed_maximum_but_count_fits_four_byte) {
  // Twelve bytes, three code points, within [1, 4]
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(
      "\xf0\x9f\x98\x80\xf0\x9f\x98\x80\xf0\x9f\x98\x80", 1, 4));
}

TEST(scan_multibyte_below_minimum_two_byte) {
  // Six bytes (>= minimum 4) but only three code points
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(
      "\xc3\xa9\xc3\xa9\xc3\xa9", 4, 10));
}

TEST(scan_multibyte_below_minimum_four_byte) {
  // Eight bytes (>= minimum 3) but only two code points
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(
      "\xf0\x9f\x98\x80\xf0\x9f\x98\x80", 3, 10));
}

TEST(scan_ascii_early_exit_above_maximum) {
  // Ten ASCII code points, maximum four; ceil(10 / 4) == 3 does not reject, so
  // the scan runs and stops once the count passes four
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("abcdefghij", 0, 4));
}

TEST(scan_eight_bytes_minimum_three_ascii_within) {
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within("abcdefgh", 3, 8));
}

TEST(scan_eight_bytes_minimum_three_two_byte_within) {
  // Four code points, within [3, 8]
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(
      "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9", 3, 8));
}

TEST(scan_eight_bytes_minimum_three_four_byte_below) {
  // Two code points, below the minimum of three
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(
      "\xf0\x9f\x98\x80\xf0\x9f\x98\x80", 3, 8));
}

TEST(exact_range_ascii_matches) {
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within("abc", 3, 3));
}

TEST(exact_range_too_long) {
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("abcd", 3, 3));
}

TEST(exact_range_too_short) {
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within("ab", 3, 3));
}

TEST(exact_range_multibyte_matches) {
  // Three two-byte code points, exactly three
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(
      "\xc3\xa9\xc3\xa9\xc3\xa9", 3, 3));
}

TEST(mixed_widths_within) {
  // "a" + two-byte + three-byte + four-byte: four code points, ten bytes
  const std::string_view input{"a\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80"};
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(input, 3, 5));
}

TEST(mixed_widths_below_minimum) {
  const std::string_view input{"a\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80"};
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(input, 5, 10));
}

TEST(mixed_widths_above_maximum) {
  const std::string_view input{"a\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80"};
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(input, 1, 3));
}

TEST(agrees_with_count_ascii) {
  const std::string_view input{"hello world"};
  const auto count{sourcemeta::core::utf8_codepoint_count(input)};
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(input, count, count));
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(input, count + 1, 100));
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(input, 0, count - 1));
}

TEST(agrees_with_count_multibyte) {
  const std::string_view input{"a\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80"};
  const auto count{sourcemeta::core::utf8_codepoint_count(input)};
  EXPECT_TRUE(sourcemeta::core::utf8_codepoint_within(input, count, count));
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(input, count + 1, 100));
  EXPECT_FALSE(sourcemeta::core::utf8_codepoint_within(input, 0, count - 1));
}
