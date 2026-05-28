#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint>     // std::int64_t, std::uint64_t
#include <string>      // std::string
#include <string_view> // std::string_view

TEST(Numeric_parse, to_double_positive_integer) {
  const std::string input{"42"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42.0);
}

TEST(Numeric_parse, to_double_negative_integer) {
  const std::string input{"-42"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), -42.0);
}

TEST(Numeric_parse, to_double_positive_decimal) {
  const std::string input{"3.14159"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_DOUBLE_EQ(result.value(), 3.14159);
}

TEST(Numeric_parse, to_double_negative_decimal) {
  const std::string input{"-2.71828"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_DOUBLE_EQ(result.value(), -2.71828);
}

TEST(Numeric_parse, to_double_zero) {
  const std::string input{"0"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0.0);
}

TEST(Numeric_parse, to_double_scientific_notation) {
  const std::string input{"1.5e10"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_DOUBLE_EQ(result.value(), 1.5e10);
}

TEST(Numeric_parse, to_double_negative_scientific_notation) {
  const std::string input{"-2.5e-5"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_DOUBLE_EQ(result.value(), -2.5e-5);
}

TEST(Numeric_parse, to_double_very_small) {
  const std::string input{"0.000001"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_DOUBLE_EQ(result.value(), 0.000001);
}

TEST(Numeric_parse, to_double_very_large_with_decimal) {
  const std::string input{"99999999999999999999999999999999999.1"};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_DOUBLE_EQ(result.value(), 9.9999999999999997e+34);
}

TEST(Numeric_parse, to_double_invalid_empty_string) {
  const std::string input{""};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_invalid_non_numeric) {
  const std::string input{"not_a_number"};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_invalid_letters) {
  const std::string input{"abc123"};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_invalid_only_dot) {
  const std::string input{"."};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_out_of_range_too_large) {
  const std::string input{"1.8e308"};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_out_of_range_too_small) {
  const std::string input{"-1.8e308"};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_invalid_trailing_junk) {
  const std::string input{"42abc"};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_invalid_trailing_decimal_junk) {
  const std::string input{"42.5abc"};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_invalid_trailing_whitespace) {
  const std::string input{"42 "};
  const auto result{sourcemeta::core::to_double(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_positive) {
  const std::string input{"123456"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 123456);
}

TEST(Numeric_parse, to_int64_t_negative) {
  const std::string input{"-987654"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), -987654);
}

TEST(Numeric_parse, to_int64_t_zero) {
  const std::string input{"0"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(Numeric_parse, to_int64_t_max_value) {
  const std::string input{"9223372036854775807"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 9223372036854775807LL);
}

TEST(Numeric_parse, to_int64_t_min_value) {
  const std::string input{"-9223372036854775808"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), -9223372036854775807LL - 1);
}

TEST(Numeric_parse, to_int64_t_large_positive) {
  const std::string input{"1000000000000"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 1000000000000LL);
}

TEST(Numeric_parse, to_int64_t_large_negative) {
  const std::string input{"-1000000000000"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), -1000000000000LL);
}

TEST(Numeric_parse, to_int64_t_invalid_empty_string) {
  const std::string input{""};
  const auto result{sourcemeta::core::to_int64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_invalid_non_numeric) {
  const std::string input{"not_a_number"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_invalid_letters) {
  const std::string input{"abc123"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_out_of_range_too_large) {
  const std::string input{"9223372036854775808"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_out_of_range_too_small) {
  const std::string input{"-9223372036854775809"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_out_of_range_way_too_large) {
  const std::string input{"99999999999999999999"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_invalid_trailing_junk) {
  const std::string input{"123abc"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_invalid_trailing_whitespace) {
  const std::string input{"42 "};
  const auto result{sourcemeta::core::to_int64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_base16_lowercase) {
  const std::string input{"ff"};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 255);
}

TEST(Numeric_parse, to_int64_t_base16_uppercase) {
  const std::string input{"FF"};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 255);
}

TEST(Numeric_parse, to_int64_t_base16_zero) {
  const std::string input{"0"};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(Numeric_parse, to_int64_t_base16_large) {
  const std::string input{"7FFFFFFFFFFFFFFF"};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 9223372036854775807LL);
}

TEST(Numeric_parse, to_int64_t_base16_invalid) {
  const std::string input{"xyz"};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_base16_out_of_range) {
  const std::string input{"FFFFFFFFFFFFFFFFFFFFFFFF"};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_base8_simple) {
  const std::string input{"77"};
  const auto result{sourcemeta::core::to_int64_t(input, 8)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 63);
}

TEST(Numeric_parse, to_int64_t_base8_zero) {
  const std::string input{"0"};
  const auto result{sourcemeta::core::to_int64_t(input, 8)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(Numeric_parse, to_int64_t_base8_large) {
  const std::string input{"777777777777777777777"};
  const auto result{sourcemeta::core::to_int64_t(input, 8)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 9223372036854775807LL);
}

TEST(Numeric_parse, to_int64_t_base8_invalid_digit) {
  const std::string input{"89"};
  const auto result{sourcemeta::core::to_int64_t(input, 8)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_base8_out_of_range) {
  const std::string input{"7777777777777777777777777"};
  const auto result{sourcemeta::core::to_int64_t(input, 8)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_base10_explicit) {
  const std::string input{"12345"};
  const auto result{sourcemeta::core::to_int64_t(input, 10)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 12345);
}

TEST(Numeric_parse, to_int64_t_base16_mixed_case) {
  const std::string input{"aB3f"};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0xAB3F);
}

TEST(Numeric_parse, to_int64_t_base16_empty) {
  const std::string input{""};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_int64_t_base8_empty) {
  const std::string input{""};
  const auto result{sourcemeta::core::to_int64_t(input, 8)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_zero) {
  const std::string input{"0"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(Numeric_parse, to_uint64_t_positive) {
  const std::string input{"42"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST(Numeric_parse, to_uint64_t_large_positive) {
  const std::string input{"1000000000000"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 1000000000000ULL);
}

TEST(Numeric_parse, to_uint64_t_max_value) {
  const std::string input{"18446744073709551615"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 18446744073709551615ULL);
}

TEST(Numeric_parse, to_uint64_t_out_of_range_too_large) {
  const std::string input{"18446744073709551616"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_out_of_range_way_too_large) {
  const std::string input{"99999999999999999999999"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_invalid_empty_string) {
  const std::string input{""};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_invalid_non_numeric) {
  const std::string input{"not_a_number"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_invalid_letters) {
  const std::string input{"abc123"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_invalid_trailing_junk) {
  const std::string input{"42abc"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_invalid_negative) {
  const std::string input{"-1"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_invalid_leading_plus) {
  const std::string input{"+1"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_leading_zero) {
  const std::string input{"050"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 50);
}

TEST(Numeric_parse, to_uint64_t_all_zeros) {
  const std::string input{"00"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(Numeric_parse, to_uint64_t_invalid_leading_whitespace) {
  const std::string input{"  42"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint64_t_invalid_trailing_whitespace) {
  const std::string input{"42 "};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_double_string_view_input) {
  const std::string_view input{"3.14"};
  const auto result{sourcemeta::core::to_double(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_DOUBLE_EQ(result.value(), 3.14);
}

TEST(Numeric_parse, to_double_string_view_substring_no_terminator) {
  const std::string_view input{"42abc"};
  const auto result{sourcemeta::core::to_double(input.substr(0, 2))};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42.0);
}

TEST(Numeric_parse, to_int64_t_string_view_input) {
  const std::string_view input{"-123"};
  const auto result{sourcemeta::core::to_int64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), -123);
}

TEST(Numeric_parse, to_int64_t_string_view_substring_no_terminator) {
  const std::string_view input{"123abc"};
  const auto result{sourcemeta::core::to_int64_t(input.substr(0, 3))};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 123);
}

TEST(Numeric_parse, to_int64_t_base16_string_view_input) {
  const std::string_view input{"deadbeef"};
  const auto result{sourcemeta::core::to_int64_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0xDEADBEEF);
}

TEST(Numeric_parse, to_int64_t_base16_string_view_substring_no_terminator) {
  const std::string_view input{"ABCDxyz"};
  const auto result{sourcemeta::core::to_int64_t(input.substr(0, 4), 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0xABCD);
}

TEST(Numeric_parse, to_uint64_t_string_view_input) {
  const std::string_view input{"42"};
  const auto result{sourcemeta::core::to_uint64_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST(Numeric_parse, to_uint64_t_string_view_substring_no_terminator) {
  const std::string_view input{"99xyz"};
  const auto result{sourcemeta::core::to_uint64_t(input.substr(0, 2))};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 99);
}

TEST(Numeric_parse, to_uint32_t_zero) {
  const std::string input{"0"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(Numeric_parse, to_uint32_t_positive) {
  const std::string input{"42"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST(Numeric_parse, to_uint32_t_max_value) {
  const std::string input{"4294967295"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 4294967295U);
}

TEST(Numeric_parse, to_uint32_t_out_of_range_too_large) {
  const std::string input{"4294967296"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_out_of_range_way_too_large) {
  const std::string input{"99999999999"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_invalid_empty_string) {
  const std::string input{""};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_invalid_non_numeric) {
  const std::string input{"not_a_number"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_invalid_trailing_junk) {
  const std::string input{"42abc"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_invalid_negative) {
  const std::string input{"-1"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_invalid_leading_plus) {
  const std::string input{"+1"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_invalid_leading_whitespace) {
  const std::string input{" 42"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_invalid_trailing_whitespace) {
  const std::string input{"42 "};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_string_view_input) {
  const std::string_view input{"42"};
  const auto result{sourcemeta::core::to_uint32_t(input)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42U);
}

TEST(Numeric_parse, to_uint32_t_string_view_substring_no_terminator) {
  const std::string_view input{"99xyz"};
  const auto result{sourcemeta::core::to_uint32_t(input.substr(0, 2))};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 99U);
}

TEST(Numeric_parse, to_uint32_t_base16_lowercase) {
  const std::string input{"ff"};
  const auto result{sourcemeta::core::to_uint32_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 255U);
}

TEST(Numeric_parse, to_uint32_t_base16_uppercase) {
  const std::string input{"FF"};
  const auto result{sourcemeta::core::to_uint32_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 255U);
}

TEST(Numeric_parse, to_uint32_t_base16_max) {
  const std::string input{"FFFFFFFF"};
  const auto result{sourcemeta::core::to_uint32_t(input, 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 4294967295U);
}

TEST(Numeric_parse, to_uint32_t_base16_out_of_range) {
  const std::string input{"100000000"};
  const auto result{sourcemeta::core::to_uint32_t(input, 16)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_base8_simple) {
  const std::string input{"77"};
  const auto result{sourcemeta::core::to_uint32_t(input, 8)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 63U);
}

TEST(Numeric_parse, to_uint32_t_base8_invalid_digit) {
  const std::string input{"89"};
  const auto result{sourcemeta::core::to_uint32_t(input, 8)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_base16_empty) {
  const std::string input{""};
  const auto result{sourcemeta::core::to_uint32_t(input, 16)};
  EXPECT_FALSE(result.has_value());
}

TEST(Numeric_parse, to_uint32_t_base16_string_view_substring_no_terminator) {
  const std::string_view input{"ABCDxyz"};
  const auto result{sourcemeta::core::to_uint32_t(input.substr(0, 4), 16)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0xABCDU);
}

TEST(Numeric_parse, to_uint32_t_base16_invalid_negative) {
  const std::string input{"-1"};
  const auto result{sourcemeta::core::to_uint32_t(input, 16)};
  EXPECT_FALSE(result.has_value());
}
