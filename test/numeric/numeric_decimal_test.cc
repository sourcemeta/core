#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cmath>   // std::isnan, std::isinf
#include <cstdint> // std::int32_t, std::int64_t, std::uint32_t, std::uint64_t
#include <limits>  // std::numeric_limits
#include <string>  // std::string
#include <thread>  // std::thread
#include <vector>  // std::vector

TEST(Numeric_decimal, add_small_integers) {
  const sourcemeta::core::Decimal left{2};
  const sourcemeta::core::Decimal right{3};
  const sourcemeta::core::Decimal result{5};
  EXPECT_EQ(left + right, result);
}

TEST(Numeric_decimal, add_large_integers) {
  const sourcemeta::core::Decimal left{1000000000};
  const sourcemeta::core::Decimal right{2000000000};
  const sourcemeta::core::Decimal result{3000000000};
  EXPECT_EQ(left + right, result);
}

TEST(Numeric_decimal, add_negative_integers) {
  const sourcemeta::core::Decimal left{-5};
  const sourcemeta::core::Decimal right{-10};
  const sourcemeta::core::Decimal result{-15};
  EXPECT_EQ(left + right, result);
}

TEST(Numeric_decimal, add_mixed_sign_integers) {
  const sourcemeta::core::Decimal left{100};
  const sourcemeta::core::Decimal right{-50};
  const sourcemeta::core::Decimal result{50};
  EXPECT_EQ(left + right, result);
}

TEST(Numeric_decimal, add_decimals) {
  const sourcemeta::core::Decimal left{"1.5"};
  const sourcemeta::core::Decimal right{"2.3"};
  const sourcemeta::core::Decimal result{"3.8"};
  EXPECT_EQ(left + right, result);
}

TEST(Numeric_decimal, add_very_large_numbers) {
  const sourcemeta::core::Decimal left{"999999999999999999999999999999"};
  const sourcemeta::core::Decimal right{"1"};
  const sourcemeta::core::Decimal result{"1000000000000000000000000000000"};
  EXPECT_EQ(left + right, result);
}

TEST(Numeric_decimal, add_high_precision_decimals) {
  const sourcemeta::core::Decimal left{"0.123456789012345"};
  const sourcemeta::core::Decimal right{"0.987654321098765"};
  const sourcemeta::core::Decimal result{"1.111111110111110"};
  EXPECT_EQ(left + right, result);
}

TEST(Numeric_decimal, subtract_small_integers) {
  const sourcemeta::core::Decimal left{10};
  const sourcemeta::core::Decimal right{3};
  const sourcemeta::core::Decimal result{7};
  EXPECT_EQ(left - right, result);
}

TEST(Numeric_decimal, subtract_to_negative) {
  const sourcemeta::core::Decimal left{5};
  const sourcemeta::core::Decimal right{15};
  const sourcemeta::core::Decimal result{-10};
  EXPECT_EQ(left - right, result);
}

TEST(Numeric_decimal, subtract_decimals) {
  const sourcemeta::core::Decimal left{"10.5"};
  const sourcemeta::core::Decimal right{"3.2"};
  const sourcemeta::core::Decimal result{"7.3"};
  EXPECT_EQ(left - right, result);
}

TEST(Numeric_decimal, subtract_negative_numbers) {
  const sourcemeta::core::Decimal left{-5};
  const sourcemeta::core::Decimal right{-3};
  const sourcemeta::core::Decimal result{-2};
  EXPECT_EQ(left - right, result);
}

TEST(Numeric_decimal, multiply_small_integers) {
  const sourcemeta::core::Decimal left{7};
  const sourcemeta::core::Decimal right{8};
  const sourcemeta::core::Decimal result{56};
  EXPECT_EQ(left * right, result);
}

TEST(Numeric_decimal, multiply_by_zero) {
  const sourcemeta::core::Decimal left{12345};
  const sourcemeta::core::Decimal right{0};
  const sourcemeta::core::Decimal result{0};
  EXPECT_EQ(left * right, result);
}

TEST(Numeric_decimal, multiply_negative_numbers) {
  const sourcemeta::core::Decimal left{-5};
  const sourcemeta::core::Decimal right{-4};
  const sourcemeta::core::Decimal result{20};
  EXPECT_EQ(left * right, result);
}

TEST(Numeric_decimal, multiply_mixed_sign) {
  const sourcemeta::core::Decimal left{6};
  const sourcemeta::core::Decimal right{-7};
  const sourcemeta::core::Decimal result{-42};
  EXPECT_EQ(left * right, result);
}

TEST(Numeric_decimal, multiply_decimals) {
  const sourcemeta::core::Decimal left{"2.5"};
  const sourcemeta::core::Decimal right{"4.0"};
  const sourcemeta::core::Decimal result{"10.0"};
  EXPECT_EQ(left * right, result);
}

TEST(Numeric_decimal, multiply_large_numbers) {
  const sourcemeta::core::Decimal left{"123456789"};
  const sourcemeta::core::Decimal right{"987654321"};
  const sourcemeta::core::Decimal result{"1.219326311126353E+17"};
  EXPECT_EQ(left * right, result);
}

TEST(Numeric_decimal, divide_exact) {
  const sourcemeta::core::Decimal left{15};
  const sourcemeta::core::Decimal right{3};
  const sourcemeta::core::Decimal result{5};
  EXPECT_EQ(left / right, result);
}

TEST(Numeric_decimal, divide_decimals) {
  const sourcemeta::core::Decimal left{"10.0"};
  const sourcemeta::core::Decimal right{"4.0"};
  const sourcemeta::core::Decimal result{"2.5"};
  EXPECT_EQ(left / right, result);
}

TEST(Numeric_decimal, divide_negative_numbers) {
  const sourcemeta::core::Decimal left{-20};
  const sourcemeta::core::Decimal right{4};
  const sourcemeta::core::Decimal result{-5};
  EXPECT_EQ(left / right, result);
}

TEST(Numeric_decimal, divide_both_negative) {
  const sourcemeta::core::Decimal left{-30};
  const sourcemeta::core::Decimal right{-6};
  const sourcemeta::core::Decimal result{5};
  EXPECT_EQ(left / right, result);
}

TEST(Numeric_decimal, divide_one) {
  const sourcemeta::core::Decimal left{"7.5"};
  const sourcemeta::core::Decimal right{1};
  const sourcemeta::core::Decimal result{"7.5"};
  EXPECT_EQ(left / right, result);
}

TEST(Numeric_decimal, modulo_basic) {
  const sourcemeta::core::Decimal left{17};
  const sourcemeta::core::Decimal right{5};
  const sourcemeta::core::Decimal result{2};
  EXPECT_EQ(left % right, result);
}

TEST(Numeric_decimal, modulo_exact) {
  const sourcemeta::core::Decimal left{20};
  const sourcemeta::core::Decimal right{4};
  const sourcemeta::core::Decimal result{0};
  EXPECT_EQ(left % right, result);
}

TEST(Numeric_decimal, modulo_negative_dividend) {
  const sourcemeta::core::Decimal left{-17};
  const sourcemeta::core::Decimal right{5};
  const sourcemeta::core::Decimal result{-2};
  EXPECT_EQ(left % right, result);
}

TEST(Numeric_decimal, equal_integers) {
  const sourcemeta::core::Decimal left{42};
  const sourcemeta::core::Decimal right{42};
  EXPECT_TRUE(left == right);
  EXPECT_FALSE(left != right);
}

TEST(Numeric_decimal, not_equal_integers) {
  const sourcemeta::core::Decimal left{42};
  const sourcemeta::core::Decimal right{43};
  EXPECT_TRUE(left != right);
  EXPECT_FALSE(left == right);
}

TEST(Numeric_decimal, less_than) {
  const sourcemeta::core::Decimal left{10};
  const sourcemeta::core::Decimal right{20};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(Numeric_decimal, less_than_or_equal) {
  const sourcemeta::core::Decimal left{10};
  const sourcemeta::core::Decimal right{10};
  EXPECT_TRUE(left <= right);
  EXPECT_TRUE(left <= sourcemeta::core::Decimal{11});
}

TEST(Numeric_decimal, greater_than) {
  const sourcemeta::core::Decimal left{50};
  const sourcemeta::core::Decimal right{25};
  EXPECT_TRUE(left > right);
  EXPECT_FALSE(right > left);
}

TEST(Numeric_decimal, greater_than_or_equal) {
  const sourcemeta::core::Decimal left{100};
  const sourcemeta::core::Decimal right{100};
  EXPECT_TRUE(left >= right);
  EXPECT_TRUE(left >= sourcemeta::core::Decimal{99});
}

TEST(Numeric_decimal, compare_decimals) {
  const sourcemeta::core::Decimal left{"1.5"};
  const sourcemeta::core::Decimal right{"1.6"};
  EXPECT_TRUE(left < right);
  EXPECT_TRUE(right > left);
}

TEST(Numeric_decimal, compare_negative_numbers) {
  const sourcemeta::core::Decimal left{-10};
  const sourcemeta::core::Decimal right{-5};
  EXPECT_TRUE(left < right);
  EXPECT_TRUE(right > left);
}

TEST(Numeric_decimal, unary_minus) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::Decimal result{-42};
  EXPECT_EQ(-value, result);
}

TEST(Numeric_decimal, unary_minus_negative) {
  const sourcemeta::core::Decimal value{-42};
  const sourcemeta::core::Decimal result{42};
  EXPECT_EQ(-value, result);
}

TEST(Numeric_decimal, unary_plus) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_EQ(+value, value);
}

TEST(Numeric_decimal, parse_integer_string) {
  const sourcemeta::core::Decimal value{"12345"};
  const sourcemeta::core::Decimal expected{12345};
  EXPECT_EQ(value, expected);
}

TEST(Numeric_decimal, parse_decimal_string) {
  const sourcemeta::core::Decimal value{"123.456"};
  const std::string str{value.to_scientific_string()};
  EXPECT_EQ(str, "1.23456e+2");
}

TEST(Numeric_decimal, parse_negative_string) {
  const sourcemeta::core::Decimal value{"-987.654"};
  const std::string str{value.to_scientific_string()};
  EXPECT_EQ(str, "-9.87654e+2");
}

TEST(Numeric_decimal, parse_scientific_notation) {
  const sourcemeta::core::Decimal value{"1.23E+10"};
  const sourcemeta::core::Decimal expected{"12300000000"};
  EXPECT_EQ(value, expected);
}

TEST(Numeric_decimal, parse_negative_exponent) {
  const sourcemeta::core::Decimal value{"1.23E-3"};
  const sourcemeta::core::Decimal expected{"0.00123"};
  EXPECT_EQ(value, expected);
}

TEST(Numeric_decimal, parse_zero) {
  const sourcemeta::core::Decimal value{"0"};
  const sourcemeta::core::Decimal expected{0};
  EXPECT_EQ(value, expected);
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, parse_very_small_number) {
  const sourcemeta::core::Decimal value{"0.000000001"};
  const std::string str{value.to_scientific_string()};
  EXPECT_EQ(str, "1e-9");
}

TEST(Numeric_decimal, convert_to_int64) {
  const sourcemeta::core::Decimal value{123456789};
  EXPECT_EQ(value.to_int64(), 123456789);
}

TEST(Numeric_decimal, convert_to_int32) {
  const sourcemeta::core::Decimal value{12345};
  EXPECT_EQ(value.to_int32(), 12345);
}

TEST(Numeric_decimal, convert_to_uint64) {
  const sourcemeta::core::Decimal value{987654321};
  EXPECT_EQ(value.to_uint64(), 987654321ULL);
}

TEST(Numeric_decimal, convert_to_uint32) {
  const sourcemeta::core::Decimal value{54321};
  EXPECT_EQ(value.to_uint32(), 54321U);
}

TEST(Numeric_decimal, convert_negative_to_int64) {
  const sourcemeta::core::Decimal value{-999999};
  EXPECT_EQ(value.to_int64(), -999999);
}

TEST(Numeric_decimal, very_large_integer) {
  const sourcemeta::core::Decimal value{
      "99999999999999999999999999999999999999"};
  const sourcemeta::core::Decimal increment{1};
  const sourcemeta::core::Decimal expected{
      "100000000000000000000000000000000000000"};
  EXPECT_EQ(value + increment, expected);
}

TEST(Numeric_decimal, large_multiplication) {
  const sourcemeta::core::Decimal left{"999999999999999999"};
  const sourcemeta::core::Decimal right{"999999999999999999"};
  const sourcemeta::core::Decimal expected{"1.000000000000000E+36"};
  EXPECT_EQ(left * right, expected);
}

TEST(Numeric_decimal, high_precision_addition) {
  const sourcemeta::core::Decimal left{"0.1111111111111111111111111111"};
  const sourcemeta::core::Decimal right{"0.2222222222222222222222222222"};
  const sourcemeta::core::Decimal expected{"0.3333333333333333"};
  EXPECT_EQ(left + right, expected);
}

TEST(Numeric_decimal, preserve_trailing_zeros) {
  const sourcemeta::core::Decimal value{"1.000"};
  const std::string str{value.to_scientific_string()};
  EXPECT_EQ(str, "1.000e+0");
}

TEST(Numeric_decimal, add_zero) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::Decimal zero{0};
  EXPECT_EQ(value + zero, value);
}

TEST(Numeric_decimal, multiply_by_one) {
  const sourcemeta::core::Decimal value{"12.34"};
  const sourcemeta::core::Decimal one{1};
  EXPECT_EQ(value * one, value);
}

TEST(Numeric_decimal, subtract_self) {
  const sourcemeta::core::Decimal value{123};
  const sourcemeta::core::Decimal zero{0};
  EXPECT_EQ(value - value, zero);
}

TEST(Numeric_decimal, is_zero_predicate) {
  const sourcemeta::core::Decimal zero{0};
  const sourcemeta::core::Decimal nonzero{1};
  EXPECT_TRUE(zero.is_zero());
  EXPECT_FALSE(nonzero.is_zero());
}

TEST(Numeric_decimal, is_integer_predicate) {
  const sourcemeta::core::Decimal integer{42};
  const sourcemeta::core::Decimal decimal{"42.5"};
  EXPECT_TRUE(integer.is_integer());
  EXPECT_FALSE(decimal.is_integer());
}

TEST(Numeric_decimal, is_integer_real_with_zero_positive) {
  const sourcemeta::core::Decimal value{"42.0"};
  EXPECT_FALSE(value.is_integer());
}

TEST(Numeric_decimal, is_integer_real_with_zero_negative) {
  const sourcemeta::core::Decimal value{"-42.0"};
  EXPECT_FALSE(value.is_integer());
}

TEST(Numeric_decimal, is_integer_real_with_zero_zero) {
  const sourcemeta::core::Decimal value{"0.0"};
  EXPECT_FALSE(value.is_integer());
}

TEST(Numeric_decimal, is_integral_integer) {
  const sourcemeta::core::Decimal value{3};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_integer_negative) {
  const sourcemeta::core::Decimal value{-42};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_real_with_zero_positive) {
  const sourcemeta::core::Decimal value{"3.0"};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_real_with_zero_negative) {
  const sourcemeta::core::Decimal value{"-42.0"};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_real_with_zero_zero) {
  const sourcemeta::core::Decimal value{"0.0"};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_non_integral_positive) {
  const sourcemeta::core::Decimal value{"3.1"};
  EXPECT_FALSE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_non_integral_negative) {
  const sourcemeta::core::Decimal value{"-42.5"};
  EXPECT_FALSE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_large_integer) {
  const sourcemeta::core::Decimal value{"999999999999999999999"};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_large_real_with_zero) {
  const sourcemeta::core::Decimal value{"999999999999999999999.0"};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_large_non_integral) {
  const sourcemeta::core::Decimal value{"999999999999999999999.1"};
  EXPECT_FALSE(value.is_integral());
}

TEST(Numeric_decimal, is_finite_predicate) {
  const sourcemeta::core::Decimal value{123};
  EXPECT_TRUE(value.is_finite());
}

TEST(Numeric_decimal, is_real_not_integer) {
  const sourcemeta::core::Decimal positive{3};
  const sourcemeta::core::Decimal negative{-5};
  const sourcemeta::core::Decimal zero{0};
  EXPECT_FALSE(positive.is_real());
  EXPECT_FALSE(negative.is_real());
  EXPECT_FALSE(zero.is_real());
}

TEST(Numeric_decimal, is_real_decimal) {
  const sourcemeta::core::Decimal value1{"3.1"};
  const sourcemeta::core::Decimal value2{"-2.5"};
  EXPECT_TRUE(value1.is_real());
  EXPECT_TRUE(value2.is_real());
}

TEST(Numeric_decimal, is_real_decimal_with_zero_fraction) {
  const sourcemeta::core::Decimal value{"3.0"};
  EXPECT_FALSE(value.is_real());
}

TEST(Numeric_decimal, is_real_special_values) {
  const sourcemeta::core::Decimal nan_value{"NaN"};
  const sourcemeta::core::Decimal inf_value{"Infinity"};
  const sourcemeta::core::Decimal neg_inf_value{"-Infinity"};
  EXPECT_FALSE(nan_value.is_real());
  EXPECT_FALSE(inf_value.is_real());
  EXPECT_FALSE(neg_inf_value.is_real());
}

TEST(Numeric_decimal, is_nan_predicate) {
  const sourcemeta::core::Decimal nan_value{"NaN"};
  const sourcemeta::core::Decimal normal_value{42};
  EXPECT_TRUE(nan_value.is_nan());
  EXPECT_FALSE(normal_value.is_nan());
}

TEST(Numeric_decimal, is_infinite_predicate) {
  const sourcemeta::core::Decimal inf_value{"Infinity"};
  const sourcemeta::core::Decimal normal_value{42};
  EXPECT_TRUE(inf_value.is_infinite());
  EXPECT_FALSE(normal_value.is_infinite());
}

TEST(Numeric_decimal, is_signed_predicate) {
  const sourcemeta::core::Decimal positive{42};
  const sourcemeta::core::Decimal negative{-42};
  const sourcemeta::core::Decimal zero{0};
  const sourcemeta::core::Decimal negative_zero{"-0"};
  EXPECT_FALSE(positive.is_signed());
  EXPECT_TRUE(negative.is_signed());
  EXPECT_FALSE(zero.is_signed());
  EXPECT_TRUE(negative_zero.is_signed());
}

TEST(Numeric_decimal, to_integral_rounds_down) {
  const sourcemeta::core::Decimal value{"42.3"};
  const sourcemeta::core::Decimal expected{42};
  EXPECT_EQ(value.to_integral(), expected);
}

TEST(Numeric_decimal, to_integral_rounds_up) {
  const sourcemeta::core::Decimal value{"42.7"};
  const sourcemeta::core::Decimal expected{43};
  EXPECT_EQ(value.to_integral(), expected);
}

TEST(Numeric_decimal, to_integral_half_even) {
  const sourcemeta::core::Decimal value1{"42.5"};
  const sourcemeta::core::Decimal expected1{42};
  const sourcemeta::core::Decimal value2{"43.5"};
  const sourcemeta::core::Decimal expected2{44};
  EXPECT_EQ(value1.to_integral(), expected1);
  EXPECT_EQ(value2.to_integral(), expected2);
}

TEST(Numeric_decimal, to_integral_already_integer) {
  const sourcemeta::core::Decimal value{100};
  const sourcemeta::core::Decimal expected{100};
  EXPECT_EQ(value.to_integral(), expected);
}

TEST(Numeric_decimal, factory_nan) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(nan_value.is_nan());
  EXPECT_FALSE(nan_value.is_finite());
  EXPECT_FALSE(nan_value.is_infinite());
}

TEST(Numeric_decimal, factory_infinity) {
  const auto inf_value{sourcemeta::core::Decimal::infinity()};
  EXPECT_TRUE(inf_value.is_infinite());
  EXPECT_FALSE(inf_value.is_nan());
  EXPECT_FALSE(inf_value.is_finite());
  EXPECT_FALSE(inf_value.is_signed());
}

TEST(Numeric_decimal, factory_negative_infinity) {
  const auto neg_inf{sourcemeta::core::Decimal::negative_infinity()};
  EXPECT_TRUE(neg_inf.is_infinite());
  EXPECT_FALSE(neg_inf.is_nan());
  EXPECT_FALSE(neg_inf.is_finite());
  EXPECT_TRUE(neg_inf.is_signed());
}

TEST(Numeric_decimal, is_float_simple_values) {
  const sourcemeta::core::Decimal value1{3};
  const sourcemeta::core::Decimal value2{"3.14"};
  const sourcemeta::core::Decimal value3{"-2.5"};
  EXPECT_TRUE(value1.is_float());
  EXPECT_FALSE(value2.is_float());
  EXPECT_TRUE(value3.is_float());
}

TEST(Numeric_decimal, is_float_special_values) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  const auto inf_value{sourcemeta::core::Decimal::infinity()};
  const auto neg_inf{sourcemeta::core::Decimal::negative_infinity()};
  EXPECT_TRUE(nan_value.is_float());
  EXPECT_TRUE(inf_value.is_float());
  EXPECT_TRUE(neg_inf.is_float());
}

TEST(Numeric_decimal, is_float_high_precision_loss) {
  const sourcemeta::core::Decimal value{"3.141592653589793238462643383279"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_large_exponent_in_range) {
  const sourcemeta::core::Decimal value{"1.5e30"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_large_negative_exponent_in_range) {
  const sourcemeta::core::Decimal value{"1.5e-30"};
  EXPECT_TRUE(value.is_float());
}

TEST(Numeric_decimal, is_float_too_large) {
  const sourcemeta::core::Decimal value{"1e100"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_too_small) {
  const sourcemeta::core::Decimal value{"1e-100"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_near_max_float) {
  const sourcemeta::core::Decimal value{"3.4e38"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_exceeds_max_float) {
  const sourcemeta::core::Decimal value{"4e38"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_near_min_float) {
  const sourcemeta::core::Decimal value{"1.2e-38"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_below_min_float) {
  const sourcemeta::core::Decimal value{"1e-50"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_6_significant_digits) {
  const sourcemeta::core::Decimal value{"1.234375"};
  EXPECT_TRUE(value.is_float());
}

TEST(Numeric_decimal, is_float_7_significant_digits_no_loss) {
  const sourcemeta::core::Decimal value{"1.2343750"};
  EXPECT_TRUE(value.is_float());
}

TEST(Numeric_decimal, is_float_many_digits_with_loss) {
  const sourcemeta::core::Decimal value{"1.23456789"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_big_integer_in_float_range) {
  const sourcemeta::core::Decimal value{"16777216"};
  EXPECT_TRUE(value.is_float());
}

TEST(Numeric_decimal, is_float_big_integer_exceeds_precision) {
  const sourcemeta::core::Decimal value{"16777217"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_small_value_with_exponent) {
  const sourcemeta::core::Decimal value{"1.0e-10"};
  EXPECT_FALSE(value.is_float());
}

TEST(Numeric_decimal, is_float_zero) {
  const sourcemeta::core::Decimal value{"0.0"};
  EXPECT_TRUE(value.is_float());
}

TEST(Numeric_decimal, is_float_negative_zero) {
  const sourcemeta::core::Decimal value{"-0.0"};
  EXPECT_TRUE(value.is_float());
}

TEST(Numeric_decimal, is_double_simple_values) {
  const sourcemeta::core::Decimal value1{3};
  const sourcemeta::core::Decimal value2{"3.14"};
  const sourcemeta::core::Decimal value3{"-2.5"};
  EXPECT_TRUE(value1.is_double());
  EXPECT_FALSE(value2.is_double());
  EXPECT_TRUE(value3.is_double());
}

TEST(Numeric_decimal, is_double_special_values) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  const auto inf_value{sourcemeta::core::Decimal::infinity()};
  const auto neg_inf{sourcemeta::core::Decimal::negative_infinity()};
  EXPECT_TRUE(nan_value.is_double());
  EXPECT_TRUE(inf_value.is_double());
  EXPECT_TRUE(neg_inf.is_double());
}

TEST(Numeric_decimal, is_double_high_precision_loss) {
  const sourcemeta::core::Decimal value{
      "3.14159265358979323846264338327950288419716939937510"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_large_exponent_in_range) {
  const sourcemeta::core::Decimal value{"1.5e100"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_large_negative_exponent_in_range) {
  const sourcemeta::core::Decimal value{"1.5e-100"};
  EXPECT_TRUE(value.is_double());
}

TEST(Numeric_decimal, is_double_too_large) {
  const sourcemeta::core::Decimal value{"1e500"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_too_small) {
  const sourcemeta::core::Decimal value{"1e-500"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_near_max_double) {
  const sourcemeta::core::Decimal value{"1.7e308"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_exceeds_max_double) {
  const sourcemeta::core::Decimal value{"2e308"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_near_min_double) {
  const sourcemeta::core::Decimal value{"2.2e-308"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_below_min_double) {
  const sourcemeta::core::Decimal value{"1e-400"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_15_significant_digits) {
  const sourcemeta::core::Decimal value{"1.23456789012345"};
  EXPECT_TRUE(value.is_double());
}

TEST(Numeric_decimal, is_double_16_significant_digits_no_loss) {
  const sourcemeta::core::Decimal value{"1.234567890123456"};
  EXPECT_TRUE(value.is_double());
}

TEST(Numeric_decimal, is_double_many_digits_with_loss) {
  const sourcemeta::core::Decimal value{"1.234567890123456789"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_big_integer_in_double_range) {
  const sourcemeta::core::Decimal value{"9007199254740992"};
  EXPECT_TRUE(value.is_double());
}

TEST(Numeric_decimal, is_double_big_integer_exceeds_precision) {
  const sourcemeta::core::Decimal value{"9007199254740993"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_small_value_with_exponent) {
  const sourcemeta::core::Decimal value{"1.0e-28"};
  EXPECT_FALSE(value.is_double());
}

TEST(Numeric_decimal, is_double_zero) {
  const sourcemeta::core::Decimal value{"0.0"};
  EXPECT_TRUE(value.is_double());
}

TEST(Numeric_decimal, is_double_negative_zero) {
  const sourcemeta::core::Decimal value{"-0.0"};
  EXPECT_TRUE(value.is_double());
}

TEST(Numeric_decimal, to_float_simple) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_FLOAT_EQ(value.to_float(), 3.14f);
}

TEST(Numeric_decimal, to_float_integer) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_FLOAT_EQ(value.to_float(), 42.0f);
}

TEST(Numeric_decimal, to_double_simple) {
  const sourcemeta::core::Decimal value{"3.5"};
  EXPECT_DOUBLE_EQ(value.to_double(), 3.5);
}

TEST(Numeric_decimal, to_double_integer) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_DOUBLE_EQ(value.to_double(), 42.0);
}

TEST(Numeric_decimal, to_float_nan) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  const float result{nan_value.to_float()};
  EXPECT_TRUE(std::isnan(result));
}

TEST(Numeric_decimal, to_float_infinity) {
  const auto inf_value{sourcemeta::core::Decimal::infinity()};
  const float result{inf_value.to_float()};
  EXPECT_TRUE(std::isinf(result));
  EXPECT_GT(result, 0.0f);
}

TEST(Numeric_decimal, to_float_negative_infinity) {
  const auto neg_inf{sourcemeta::core::Decimal::negative_infinity()};
  const float result{neg_inf.to_float()};
  EXPECT_TRUE(std::isinf(result));
  EXPECT_LT(result, 0.0f);
}

TEST(Numeric_decimal, to_double_nan) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  const double result{nan_value.to_double()};
  EXPECT_TRUE(std::isnan(result));
}

TEST(Numeric_decimal, to_double_infinity) {
  const auto inf_value{sourcemeta::core::Decimal::infinity()};
  const double result{inf_value.to_double()};
  EXPECT_TRUE(std::isinf(result));
  EXPECT_GT(result, 0.0);
}

TEST(Numeric_decimal, to_double_negative_infinity) {
  const auto neg_inf{sourcemeta::core::Decimal::negative_infinity()};
  const double result{neg_inf.to_double()};
  EXPECT_TRUE(std::isinf(result));
  EXPECT_LT(result, 0.0);
}

TEST(Numeric_decimal, to_float_not_exactly_representable_gets_rounded) {
  const sourcemeta::core::Decimal value{"3.2"};
  EXPECT_FALSE(value.is_float());
  const float result{value.to_float()};
  EXPECT_FLOAT_EQ(result, 3.2f);
}

TEST(Numeric_decimal, to_double_not_exactly_representable_gets_rounded) {
  const sourcemeta::core::Decimal value{"3.2"};
  EXPECT_FALSE(value.is_double());
  const double result{value.to_double()};
  EXPECT_DOUBLE_EQ(result, 3.2);
}

TEST(Numeric_decimal, to_float_exceeds_max_float_throws) {
  const sourcemeta::core::Decimal value{"1e100"};
  EXPECT_FALSE(value.is_float());
  EXPECT_THROW(
      { [[maybe_unused]] const float result = value.to_float(); },
      std::out_of_range);
}

TEST(Numeric_decimal, to_double_exceeds_max_double_throws) {
  const sourcemeta::core::Decimal value{"1e500"};
  EXPECT_FALSE(value.is_double());
  EXPECT_THROW(
      { [[maybe_unused]] const double result = value.to_double(); },
      std::out_of_range);
}

TEST(Numeric_decimal, to_float_below_min_float_throws) {
  const sourcemeta::core::Decimal value{"1e-100"};
  EXPECT_FALSE(value.is_float());
  EXPECT_THROW(
      { [[maybe_unused]] const float result = value.to_float(); },
      std::out_of_range);
}

TEST(Numeric_decimal, to_double_below_min_double_throws) {
  const sourcemeta::core::Decimal value{"1e-500"};
  EXPECT_FALSE(value.is_double());
  EXPECT_THROW(
      { [[maybe_unused]] const double result = value.to_double(); },
      std::out_of_range);
}

TEST(Numeric_decimal, to_float_high_precision_gets_rounded) {
  const sourcemeta::core::Decimal value{"1.23456789"};
  EXPECT_FALSE(value.is_float());
  const float result{value.to_float()};
  EXPECT_FLOAT_EQ(result, 1.23456789f);
}

TEST(Numeric_decimal, to_double_high_precision_gets_rounded) {
  const sourcemeta::core::Decimal value{"1.234567890123456789"};
  EXPECT_FALSE(value.is_double());
  const double result{value.to_double()};
  EXPECT_DOUBLE_EQ(result, 1.234567890123456789);
}

TEST(Numeric_decimal, divisible_by_integer_true) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_integer_false) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_zero) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{0};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_decimal_true) {
  const sourcemeta::core::Decimal dividend{"4.5"};
  const sourcemeta::core::Decimal divisor{"1.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_decimal_false) {
  const sourcemeta::core::Decimal dividend{"5.0"};
  const sourcemeta::core::Decimal divisor{"1.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_half) {
  const sourcemeta::core::Decimal dividend{7};
  const sourcemeta::core::Decimal divisor{"0.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_half_odd) {
  const sourcemeta::core::Decimal dividend{8};
  const sourcemeta::core::Decimal divisor{"0.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_self) {
  const sourcemeta::core::Decimal value{"3.14159"};
  EXPECT_TRUE(value.divisible_by(value));
}

TEST(Numeric_decimal, divisible_by_one) {
  const sourcemeta::core::Decimal dividend{123};
  const sourcemeta::core::Decimal divisor{1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_negative) {
  const sourcemeta::core::Decimal dividend{-10};
  const sourcemeta::core::Decimal divisor{5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_very_large_number_not_divisible_false) {
  const sourcemeta::core::Decimal dividend{"1e308"};
  const sourcemeta::core::Decimal divisor{"0.123456789"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_very_large_number_divisible_true) {
  const sourcemeta::core::Decimal dividend{"1e308"};
  const sourcemeta::core::Decimal divisor{"1e154"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal,
     divisible_by_very_large_negative_number_not_divisible_false) {
  const sourcemeta::core::Decimal dividend{"-1e308"};
  const sourcemeta::core::Decimal divisor{"0.123456789"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_very_large_negative_number_divisible_true) {
  const sourcemeta::core::Decimal dividend{"-1e308"};
  const sourcemeta::core::Decimal divisor{"-1e154"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_very_small_fractional_not_divisible_false) {
  const sourcemeta::core::Decimal dividend{"1e-100"};
  const sourcemeta::core::Decimal divisor{"3e-101"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_very_small_fractional_divisible_true) {
  const sourcemeta::core::Decimal dividend{"1e-100"};
  const sourcemeta::core::Decimal divisor{"1e-200"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_large_fractional_not_divisible_false) {
  const sourcemeta::core::Decimal dividend{"9999999999999999.9999999999999999"};
  const sourcemeta::core::Decimal divisor{"0.33333333333333"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_mixed_scale_not_divisible_false) {
  const sourcemeta::core::Decimal dividend{"1e100"};
  const sourcemeta::core::Decimal divisor{"0.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_mixed_scale_divisible_true) {
  const sourcemeta::core::Decimal dividend{"1e100"};
  const sourcemeta::core::Decimal divisor{"0.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, add_integer_to_decimal) {
  const sourcemeta::core::Decimal left{10};
  const sourcemeta::core::Decimal right{"2.5"};
  const sourcemeta::core::Decimal expected{"12.5"};
  EXPECT_EQ(left + right, expected);
}

TEST(Numeric_decimal, multiply_integer_by_decimal) {
  const sourcemeta::core::Decimal left{5};
  const sourcemeta::core::Decimal right{"1.5"};
  const sourcemeta::core::Decimal expected{"7.5"};
  EXPECT_EQ(left * right, expected);
}

TEST(Numeric_decimal, add_plain_int_to_decimal) {
  const sourcemeta::core::Decimal value{"10.5"};
  const sourcemeta::core::Decimal expected{"15.5"};
  EXPECT_EQ(value + 5, expected);
}

TEST(Numeric_decimal, add_decimal_to_plain_int) {
  const sourcemeta::core::Decimal value{"3.5"};
  const sourcemeta::core::Decimal expected{"13.5"};
  EXPECT_EQ(10 + value, expected);
}

TEST(Numeric_decimal, subtract_plain_int_from_decimal) {
  const sourcemeta::core::Decimal value{"20.5"};
  const sourcemeta::core::Decimal expected{"10.5"};
  EXPECT_EQ(value - 10, expected);
}

TEST(Numeric_decimal, subtract_decimal_from_plain_int) {
  const sourcemeta::core::Decimal value{"3.5"};
  const sourcemeta::core::Decimal expected{"6.5"};
  EXPECT_EQ(10 - value, expected);
}

TEST(Numeric_decimal, multiply_decimal_by_plain_int) {
  const sourcemeta::core::Decimal value{"4.5"};
  const sourcemeta::core::Decimal expected{"13.5"};
  EXPECT_EQ(value * 3, expected);
}

TEST(Numeric_decimal, multiply_plain_int_by_decimal) {
  const sourcemeta::core::Decimal value{"2.5"};
  const sourcemeta::core::Decimal expected{"10.0"};
  EXPECT_EQ(4 * value, expected);
}

TEST(Numeric_decimal, divide_decimal_by_plain_int) {
  const sourcemeta::core::Decimal value{"15.0"};
  const sourcemeta::core::Decimal expected{"3.0"};
  EXPECT_EQ(value / 5, expected);
}

TEST(Numeric_decimal, divide_plain_int_by_decimal) {
  const sourcemeta::core::Decimal value{"4.0"};
  const sourcemeta::core::Decimal expected{"5.0"};
  EXPECT_EQ(20 / value, expected);
}

TEST(Numeric_decimal, modulo_decimal_by_plain_int) {
  const sourcemeta::core::Decimal value{17};
  const sourcemeta::core::Decimal expected{2};
  EXPECT_EQ(value % 5, expected);
}

TEST(Numeric_decimal, modulo_plain_int_by_decimal) {
  const sourcemeta::core::Decimal value{5};
  const sourcemeta::core::Decimal expected{2};
  EXPECT_EQ(17 % value, expected);
}

TEST(Numeric_decimal, compare_decimal_equal_plain_int) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_TRUE(value == 42);
  EXPECT_TRUE(42 == value);
}

TEST(Numeric_decimal, compare_decimal_not_equal_plain_int) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_TRUE(value != 43);
  EXPECT_TRUE(43 != value);
}

TEST(Numeric_decimal, compare_decimal_less_than_plain_int) {
  const sourcemeta::core::Decimal value{10};
  EXPECT_TRUE(value < 20);
  EXPECT_TRUE(5 < value);
}

TEST(Numeric_decimal, compare_decimal_greater_than_plain_int) {
  const sourcemeta::core::Decimal value{50};
  EXPECT_TRUE(value > 25);
  EXPECT_TRUE(75 > value);
}

TEST(Numeric_decimal, compare_decimal_with_negative_int) {
  const sourcemeta::core::Decimal value{10};
  EXPECT_TRUE(value > -5);
  EXPECT_TRUE(-5 < value);
}

TEST(Numeric_decimal, mixed_operation_with_int64) {
  const sourcemeta::core::Decimal value{"1000000000000"};
  const int64_t large{1000000000000LL};
  const sourcemeta::core::Decimal expected{"2000000000000"};
  EXPECT_EQ(value + large, expected);
}

TEST(Numeric_decimal, mixed_operation_with_uint32) {
  const sourcemeta::core::Decimal value{"100.5"};
  const uint32_t integer{50U};
  const sourcemeta::core::Decimal expected{"150.5"};
  EXPECT_EQ(value + integer, expected);
}

TEST(Numeric_decimal, prefix_increment_integer) {
  sourcemeta::core::Decimal value{10};
  const sourcemeta::core::Decimal result{++value};
  EXPECT_EQ(value, sourcemeta::core::Decimal{11});
  EXPECT_EQ(result, sourcemeta::core::Decimal{11});
}

TEST(Numeric_decimal, postfix_increment_integer) {
  sourcemeta::core::Decimal value{10};
  const sourcemeta::core::Decimal result{value++};
  EXPECT_EQ(value, sourcemeta::core::Decimal{11});
  EXPECT_EQ(result, sourcemeta::core::Decimal{10});
}

TEST(Numeric_decimal, prefix_decrement_integer) {
  sourcemeta::core::Decimal value{10};
  const sourcemeta::core::Decimal result{--value};
  EXPECT_EQ(value, sourcemeta::core::Decimal{9});
  EXPECT_EQ(result, sourcemeta::core::Decimal{9});
}

TEST(Numeric_decimal, postfix_decrement_integer) {
  sourcemeta::core::Decimal value{10};
  const sourcemeta::core::Decimal result{value--};
  EXPECT_EQ(value, sourcemeta::core::Decimal{9});
  EXPECT_EQ(result, sourcemeta::core::Decimal{10});
}

TEST(Numeric_decimal, prefix_increment_decimal) {
  sourcemeta::core::Decimal value{"5.5"};
  ++value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{"6.5"});
}

TEST(Numeric_decimal, postfix_increment_decimal) {
  sourcemeta::core::Decimal value{"5.5"};
  value++;
  EXPECT_EQ(value, sourcemeta::core::Decimal{"6.5"});
}

TEST(Numeric_decimal, prefix_decrement_decimal) {
  sourcemeta::core::Decimal value{"5.5"};
  --value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{"4.5"});
}

TEST(Numeric_decimal, postfix_decrement_decimal) {
  sourcemeta::core::Decimal value{"5.5"};
  value--;
  EXPECT_EQ(value, sourcemeta::core::Decimal{"4.5"});
}

TEST(Numeric_decimal, multiple_increments) {
  sourcemeta::core::Decimal value{0};
  ++value;
  ++value;
  ++value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{3});
}

TEST(Numeric_decimal, multiple_decrements) {
  sourcemeta::core::Decimal value{10};
  value--;
  value--;
  value--;
  EXPECT_EQ(value, sourcemeta::core::Decimal{7});
}

TEST(Numeric_decimal, increment_negative_to_zero) {
  sourcemeta::core::Decimal value{-1};
  ++value;
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, decrement_to_negative) {
  sourcemeta::core::Decimal value{0};
  --value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{-1});
}

TEST(Numeric_decimal, chained_prefix_operations) {
  sourcemeta::core::Decimal value{5};
  const sourcemeta::core::Decimal result{++(++value)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{7});
  EXPECT_EQ(result, sourcemeta::core::Decimal{7});
}

TEST(Numeric_decimal, default_constructor) {
  const sourcemeta::core::Decimal value;
  EXPECT_TRUE(value.is_zero());
  EXPECT_EQ(value, sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, copy_constructor) {
  const sourcemeta::core::Decimal original{42};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_EQ(copy, original);
  EXPECT_EQ(copy, sourcemeta::core::Decimal{42});
}

TEST(Numeric_decimal, copy_constructor_decimal_value) {
  const sourcemeta::core::Decimal original{"3.14159"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_EQ(copy, original);
}

TEST(Numeric_decimal, copy_assignment) {
  const sourcemeta::core::Decimal original{100};
  sourcemeta::core::Decimal copy{0};
  copy = original;
  EXPECT_EQ(copy, original);
  EXPECT_EQ(copy, sourcemeta::core::Decimal{100});
}

TEST(Numeric_decimal, move_constructor) {
  sourcemeta::core::Decimal original{999};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved, sourcemeta::core::Decimal{999});
}

TEST(Numeric_decimal, move_constructor_decimal_value) {
  sourcemeta::core::Decimal original{"2.71828"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved, sourcemeta::core::Decimal{"2.71828"});
}

TEST(Numeric_decimal, move_assignment) {
  sourcemeta::core::Decimal original{500};
  sourcemeta::core::Decimal moved{0};
  moved = std::move(original);
  EXPECT_EQ(moved, sourcemeta::core::Decimal{500});
}

TEST(Numeric_decimal, copy_then_modify) {
  const sourcemeta::core::Decimal original{10};
  sourcemeta::core::Decimal copy{original};
  copy += sourcemeta::core::Decimal{5};
  EXPECT_EQ(original, sourcemeta::core::Decimal{10});
  EXPECT_EQ(copy, sourcemeta::core::Decimal{15});
}

TEST(Numeric_decimal, multiple_copies) {
  const sourcemeta::core::Decimal value1{7};
  const sourcemeta::core::Decimal value2{value1};
  const sourcemeta::core::Decimal value3{value2};
  EXPECT_EQ(value1, value2);
  EXPECT_EQ(value2, value3);
  EXPECT_EQ(value1, value3);
}

TEST(Numeric_decimal, compound_assignment_add) {
  sourcemeta::core::Decimal value{10};
  value += sourcemeta::core::Decimal{5};
  EXPECT_EQ(value, sourcemeta::core::Decimal{15});
}

TEST(Numeric_decimal, compound_assignment_subtract) {
  sourcemeta::core::Decimal value{20};
  value -= sourcemeta::core::Decimal{8};
  EXPECT_EQ(value, sourcemeta::core::Decimal{12});
}

TEST(Numeric_decimal, compound_assignment_multiply) {
  sourcemeta::core::Decimal value{6};
  value *= sourcemeta::core::Decimal{7};
  EXPECT_EQ(value, sourcemeta::core::Decimal{42});
}

TEST(Numeric_decimal, compound_assignment_divide) {
  sourcemeta::core::Decimal value{100};
  value /= sourcemeta::core::Decimal{5};
  EXPECT_EQ(value, sourcemeta::core::Decimal{20});
}

TEST(Numeric_decimal, compound_assignment_modulo) {
  sourcemeta::core::Decimal value{17};
  value %= sourcemeta::core::Decimal{5};
  EXPECT_EQ(value, sourcemeta::core::Decimal{2});
}

TEST(Numeric_decimal, compound_assignment_chain) {
  sourcemeta::core::Decimal value{10};
  value += sourcemeta::core::Decimal{5};
  value *= sourcemeta::core::Decimal{2};
  value -= sourcemeta::core::Decimal{10};
  EXPECT_EQ(value, sourcemeta::core::Decimal{20});
}

TEST(Numeric_decimal, to_string_integer) {
  const sourcemeta::core::Decimal value{12345};
  EXPECT_EQ(value.to_string(), "12345");
}

TEST(Numeric_decimal, to_string_decimal) {
  const sourcemeta::core::Decimal value{"123.456"};
  EXPECT_EQ(value.to_string(), "123.456");
}

TEST(Numeric_decimal, to_string_negative) {
  const sourcemeta::core::Decimal value{-999};
  EXPECT_EQ(value.to_string(), "-999");
}

TEST(Numeric_decimal, to_string_negative_decimal) {
  const sourcemeta::core::Decimal value{"-67.89"};
  EXPECT_EQ(value.to_string(), "-67.89");
}

TEST(Numeric_decimal, to_string_zero_fractional) {
  const sourcemeta::core::Decimal value{"7.0"};
  EXPECT_EQ(value.to_string(), "7.0");
}

TEST(Numeric_decimal, to_string_large_number) {
  const sourcemeta::core::Decimal value{"123456789012345678901234567890"};
  EXPECT_EQ(value.to_string(), "123456789012345678901234567890");
}

TEST(Numeric_decimal, construct_from_string_view) {
  const std::string_view view{"42.5"};
  const sourcemeta::core::Decimal value{view};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"42.5"});
}

TEST(Numeric_decimal, construct_from_string_view_large) {
  const std::string_view view{"999999999999999999999"};
  const sourcemeta::core::Decimal value{view};
  EXPECT_EQ(value.to_string(), "999999999999999999999");
}

TEST(Numeric_decimal, is_int32_true) {
  const sourcemeta::core::Decimal value{1000};
  EXPECT_TRUE(value.is_int32());
}

TEST(Numeric_decimal, is_int32_false_too_large) {
  const sourcemeta::core::Decimal value{"3000000000"};
  EXPECT_FALSE(value.is_int32());
}

TEST(Numeric_decimal, is_int32_min_max) {
  const sourcemeta::core::Decimal min{-2147483648LL};
  const sourcemeta::core::Decimal max{2147483647};
  EXPECT_TRUE(min.is_int32());
  EXPECT_TRUE(max.is_int32());
}

TEST(Numeric_decimal, is_int64_true) {
  const sourcemeta::core::Decimal value{123456789012345LL};
  EXPECT_TRUE(value.is_int64());
}

TEST(Numeric_decimal, is_int64_false_too_large) {
  const sourcemeta::core::Decimal value{"99999999999999999999"};
  EXPECT_FALSE(value.is_int64());
}

TEST(Numeric_decimal, is_uint32_true) {
  const sourcemeta::core::Decimal value{4000000000U};
  EXPECT_TRUE(value.is_uint32());
}

TEST(Numeric_decimal, is_uint32_false_negative) {
  const sourcemeta::core::Decimal value{-1};
  EXPECT_FALSE(value.is_uint32());
}

TEST(Numeric_decimal, is_uint32_false_too_large) {
  const sourcemeta::core::Decimal value{"5000000000"};
  EXPECT_FALSE(value.is_uint32());
}

TEST(Numeric_decimal, is_uint32_max) {
  const sourcemeta::core::Decimal max{4294967295U};
  EXPECT_TRUE(max.is_uint32());
}

TEST(Numeric_decimal, is_uint64_true) {
  const sourcemeta::core::Decimal value{"18446744073709551615"};
  EXPECT_TRUE(value.is_uint64());
}

TEST(Numeric_decimal, is_uint64_false_negative) {
  const sourcemeta::core::Decimal value{-100};
  EXPECT_FALSE(value.is_uint64());
}

TEST(Numeric_decimal, is_uint64_false_too_large) {
  const sourcemeta::core::Decimal value{"18446744073709551616"};
  EXPECT_FALSE(value.is_uint64());
}

TEST(Numeric_decimal, exception_conversion_syntax_invalid_string) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"not_a_number"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, exception_conversion_syntax_empty_string) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{""}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, exception_conversion_syntax_invalid_exponent) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"123e"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, exception_conversion_syntax_multiple_dots) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"12.34.56"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, exception_division_by_zero_divide) {
  const sourcemeta::core::Decimal numerator{10};
  const sourcemeta::core::Decimal denominator{0};
  EXPECT_THROW(
      { const auto result = numerator / denominator; },
      sourcemeta::core::NumericDivisionByZeroError);
}

TEST(Numeric_decimal, exception_division_by_zero_divide_assign) {
  sourcemeta::core::Decimal numerator{10};
  const sourcemeta::core::Decimal denominator{0};
  EXPECT_THROW(
      { numerator /= denominator; },
      sourcemeta::core::NumericDivisionByZeroError);
}

TEST(Numeric_decimal, exception_invalid_operation_modulo_by_zero) {
  const sourcemeta::core::Decimal numerator{10};
  const sourcemeta::core::Decimal denominator{0};
  EXPECT_THROW(
      { const auto result = numerator % denominator; },
      sourcemeta::core::NumericInvalidOperationError);
}

TEST(Numeric_decimal, exception_invalid_operation_modulo_assign_by_zero) {
  sourcemeta::core::Decimal numerator{10};
  const sourcemeta::core::Decimal denominator{0};
  EXPECT_THROW(
      { numerator %= denominator; },
      sourcemeta::core::NumericInvalidOperationError);
}

TEST(Numeric_decimal, exception_invalid_operation_zero_divided_by_zero) {
  const sourcemeta::core::Decimal zero{0};
  EXPECT_THROW(
      { const auto result = zero / zero; },
      sourcemeta::core::NumericInvalidOperationError);
}

TEST(Numeric_decimal, exception_invalid_operation_zero_modulo_zero) {
  const sourcemeta::core::Decimal zero{0};
  EXPECT_THROW(
      { const auto result = zero % zero; },
      sourcemeta::core::NumericInvalidOperationError);
}

TEST(Numeric_decimal, exception_overflow_multiplication) {
  const sourcemeta::core::Decimal large{"9e999999999999999999"};
  const sourcemeta::core::Decimal multiplier{10};
  EXPECT_THROW(
      { const auto result = large * multiplier; },
      sourcemeta::core::NumericOverflowError);
}

TEST(Numeric_decimal, exception_overflow_addition) {
  const sourcemeta::core::Decimal large{"9e999999999999999999"};
  const sourcemeta::core::Decimal addend{"9e999999999999999999"};
  EXPECT_THROW(
      { const auto result = large + addend; },
      sourcemeta::core::NumericOverflowError);
}

TEST(Numeric_decimal, copy_constructor_preserves_negative_sign) {
  const sourcemeta::core::Decimal original{"-123.456"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_EQ(copy.to_string(), "-123.456");
  EXPECT_TRUE(copy.is_signed());
}

TEST(Numeric_decimal, move_constructor_preserves_negative_sign) {
  sourcemeta::core::Decimal original{"-789.012"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved.to_string(), "-789.012");
  EXPECT_TRUE(moved.is_signed());
}

TEST(Numeric_decimal, copy_assignment_preserves_negative_sign) {
  const sourcemeta::core::Decimal original{"-999.999"};
  sourcemeta::core::Decimal target{42};
  target = original;
  EXPECT_EQ(target.to_string(), "-999.999");
  EXPECT_TRUE(target.is_signed());
}

TEST(Numeric_decimal, move_assignment_preserves_negative_sign) {
  sourcemeta::core::Decimal original{"-111.222"};
  sourcemeta::core::Decimal target{99};
  target = std::move(original);
  EXPECT_EQ(target.to_string(), "-111.222");
  EXPECT_TRUE(target.is_signed());
}

TEST(Numeric_decimal, multiple_copies_preserve_negative_sign) {
  const sourcemeta::core::Decimal first{"-55.55"};
  const sourcemeta::core::Decimal second{first};
  const sourcemeta::core::Decimal third{second};
  const sourcemeta::core::Decimal fourth{third};
  EXPECT_EQ(fourth.to_string(), "-55.55");
  EXPECT_TRUE(fourth.is_signed());
}

TEST(Numeric_decimal, copy_then_move_preserves_negative_sign) {
  const sourcemeta::core::Decimal original{"-333.333"};
  sourcemeta::core::Decimal copy{original};
  const sourcemeta::core::Decimal moved{std::move(copy)};
  EXPECT_EQ(moved.to_string(), "-333.333");
  EXPECT_TRUE(moved.is_signed());
}

TEST(Numeric_decimal, move_then_copy_preserves_negative_sign) {
  sourcemeta::core::Decimal original{"-444.444"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  const sourcemeta::core::Decimal copy{moved};
  EXPECT_EQ(copy.to_string(), "-444.444");
  EXPECT_TRUE(copy.is_signed());
}

TEST(Numeric_decimal, negative_integer_copy) {
  const sourcemeta::core::Decimal original{-12345};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_EQ(copy.to_int64(), -12345);
  EXPECT_TRUE(copy.is_signed());
}

TEST(Numeric_decimal, negative_integer_move) {
  sourcemeta::core::Decimal original{-98765};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved.to_int64(), -98765);
  EXPECT_TRUE(moved.is_signed());
}

TEST(Numeric_decimal, very_small_negative_copy) {
  const sourcemeta::core::Decimal original{"-0.000000000000001"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_TRUE(copy.is_signed());
  EXPECT_TRUE(copy < sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, very_small_negative_move) {
  sourcemeta::core::Decimal original{"-0.000000000000001"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_TRUE(moved.is_signed());
  EXPECT_TRUE(moved < sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, large_negative_number_copy) {
  const sourcemeta::core::Decimal original{"-999999999999999999999999999999"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_EQ(copy.to_string(), "-999999999999999999999999999999");
  EXPECT_TRUE(copy.is_signed());
}

TEST(Numeric_decimal, large_negative_number_move) {
  sourcemeta::core::Decimal original{"-888888888888888888888888888888"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved.to_string(), "-888888888888888888888888888888");
  EXPECT_TRUE(moved.is_signed());
}

TEST(Numeric_decimal, negative_scientific_notation_copy) {
  const sourcemeta::core::Decimal original{"-1.23e50"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_TRUE(copy.is_signed());
  EXPECT_TRUE(copy < sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, negative_scientific_notation_move) {
  sourcemeta::core::Decimal original{"-9.87e-20"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_TRUE(moved.is_signed());
  EXPECT_TRUE(moved < sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, assignment_chain_with_negative) {
  const sourcemeta::core::Decimal original{"-777.777"};
  sourcemeta::core::Decimal first{1};
  sourcemeta::core::Decimal second{2};
  sourcemeta::core::Decimal third{3};
  first = original;
  second = first;
  third = second;
  EXPECT_EQ(third.to_string(), "-777.777");
  EXPECT_TRUE(third.is_signed());
}

TEST(Numeric_decimal, positive_not_signed_after_copy) {
  const sourcemeta::core::Decimal original{"123.456"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_FALSE(copy.is_signed());
}

TEST(Numeric_decimal, positive_not_signed_after_move) {
  sourcemeta::core::Decimal original{"789.012"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_FALSE(moved.is_signed());
}

TEST(Numeric_decimal, zero_not_signed_after_copy) {
  const sourcemeta::core::Decimal original{0};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_FALSE(copy.is_signed());
}

TEST(Numeric_decimal, negative_zero_preserves_sign) {
  const sourcemeta::core::Decimal original{"-0"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_TRUE(copy.is_signed());
  EXPECT_TRUE(copy.is_zero());
}

TEST(Numeric_decimal, construct_from_float_simple) {
  const float value{3.5f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  EXPECT_TRUE(decimal.is_double());
  EXPECT_EQ(decimal.to_float(), 3.5f);
  EXPECT_EQ(decimal.to_double(), 3.5);
  EXPECT_EQ(decimal.to_string(), "3.5");
}

TEST(Numeric_decimal, construct_from_float_negative) {
  const float value{-7.25f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  EXPECT_TRUE(decimal.is_double());
  EXPECT_EQ(decimal.to_float(), -7.25f);
  EXPECT_EQ(decimal.to_double(), -7.25);
  EXPECT_EQ(decimal.to_string(), "-7.25");
}

TEST(Numeric_decimal, construct_from_float_zero) {
  const float value{0.0f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  EXPECT_TRUE(decimal.is_double());
  EXPECT_TRUE(decimal.is_zero());
  EXPECT_EQ(decimal.to_float(), 0.0f);
  EXPECT_EQ(decimal.to_double(), 0.0);
}

TEST(Numeric_decimal, construct_from_float_very_small) {
  const float value{0.125f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  EXPECT_TRUE(decimal.is_double());
  EXPECT_EQ(decimal.to_float(), 0.125f);
  EXPECT_EQ(decimal.to_double(), 0.125);
  EXPECT_EQ(decimal.to_string(), "0.125");
}

TEST(Numeric_decimal, construct_from_double_simple) {
  const double value{3.5};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_double());
  EXPECT_EQ(decimal.to_double(), 3.5);
  EXPECT_EQ(decimal.to_string(), "3.5");
}

TEST(Numeric_decimal, construct_from_double_negative) {
  const double value{-7.25};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_double());
  EXPECT_EQ(decimal.to_double(), -7.25);
  EXPECT_EQ(decimal.to_string(), "-7.25");
}

TEST(Numeric_decimal, construct_from_double_zero) {
  const double value{0.0};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_double());
  EXPECT_TRUE(decimal.is_zero());
  EXPECT_EQ(decimal.to_double(), 0.0);
}

TEST(Numeric_decimal, construct_from_double_very_small) {
  const double value{0.125};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_double());
  EXPECT_EQ(decimal.to_double(), 0.125);
  EXPECT_EQ(decimal.to_string(), "0.125");
}

TEST(Numeric_decimal, construct_from_double_roundtrip) {
  const double value{3.2};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_double());
  const double roundtrip{decimal.to_double()};
  EXPECT_EQ(roundtrip, value);
}

TEST(Numeric_decimal, construct_from_float_roundtrip) {
  const float value{3.2f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  const float roundtrip{decimal.to_float()};
  EXPECT_EQ(roundtrip, value);
}

TEST(Numeric_decimal, construct_from_double_high_precision) {
  const double value{1.234567890123456};
  const sourcemeta::core::Decimal decimal{value};
  const double roundtrip{decimal.to_double()};
  EXPECT_EQ(roundtrip, value);
}

TEST(Numeric_decimal, construct_from_float_high_precision) {
  const float value{1.234567f};
  const sourcemeta::core::Decimal decimal{value};
  const float roundtrip{decimal.to_float()};
  EXPECT_EQ(roundtrip, value);
}

TEST(Numeric_decimal, multithreaded_construction) {
  constexpr int number_of_threads{4};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal value{42};
      const sourcemeta::core::Decimal result{value +
                                             sourcemeta::core::Decimal{1}};
      EXPECT_EQ(result, sourcemeta::core::Decimal{43});
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(Numeric_decimal, factory_snan) {
  const auto snan_value{sourcemeta::core::Decimal::snan()};
  EXPECT_TRUE(snan_value.is_nan());
  EXPECT_TRUE(snan_value.is_snan());
  EXPECT_FALSE(snan_value.is_qnan());
  EXPECT_FALSE(snan_value.is_finite());
  EXPECT_FALSE(snan_value.is_infinite());
}

TEST(Numeric_decimal, factory_nan_with_payload) {
  const auto nan_value{sourcemeta::core::Decimal::nan(123)};
  EXPECT_TRUE(nan_value.is_nan());
  EXPECT_TRUE(nan_value.is_qnan());
  EXPECT_FALSE(nan_value.is_snan());
  EXPECT_EQ(nan_value.nan_payload(), 123);
}

TEST(Numeric_decimal, factory_snan_with_payload) {
  const auto snan_value{sourcemeta::core::Decimal::snan(456)};
  EXPECT_TRUE(snan_value.is_nan());
  EXPECT_TRUE(snan_value.is_snan());
  EXPECT_FALSE(snan_value.is_qnan());
  EXPECT_EQ(snan_value.nan_payload(), 456);
}

TEST(Numeric_decimal, is_snan_false_for_qnan) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value.is_snan());
  EXPECT_TRUE(nan_value.is_qnan());
}

TEST(Numeric_decimal, is_qnan_false_for_snan) {
  const auto snan_value{sourcemeta::core::Decimal::snan()};
  EXPECT_FALSE(snan_value.is_qnan());
  EXPECT_TRUE(snan_value.is_snan());
}

TEST(Numeric_decimal, is_nan_true_for_both) {
  EXPECT_TRUE(sourcemeta::core::Decimal::nan().is_nan());
  EXPECT_TRUE(sourcemeta::core::Decimal::snan().is_nan());
}

TEST(Numeric_decimal, nan_payload_zero_for_plain_nan) {
  EXPECT_EQ(sourcemeta::core::Decimal::nan().nan_payload(), 0);
}

TEST(Numeric_decimal, nan_payload_zero_for_plain_snan) {
  EXPECT_EQ(sourcemeta::core::Decimal::snan().nan_payload(), 0);
}

TEST(Numeric_decimal, parse_snan_string) {
  const sourcemeta::core::Decimal value{"sNaN"};
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_snan());
  EXPECT_EQ(value.nan_payload(), 0);
}

TEST(Numeric_decimal, parse_nan_with_payload_string) {
  const sourcemeta::core::Decimal value{"NaN123"};
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_qnan());
  EXPECT_EQ(value.nan_payload(), 123);
}

TEST(Numeric_decimal, parse_snan_with_payload_string) {
  const sourcemeta::core::Decimal value{"sNaN789"};
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_snan());
  EXPECT_EQ(value.nan_payload(), 789);
}

TEST(Numeric_decimal, snan_arithmetic_produces_qnan) {
  const auto snan_value{sourcemeta::core::Decimal::snan()};
  const auto result{snan_value + sourcemeta::core::Decimal{1}};
  EXPECT_TRUE(result.is_nan());
  EXPECT_TRUE(result.is_qnan());
  EXPECT_FALSE(result.is_snan());
}

TEST(Numeric_decimal, equal_different_scale_integer) {
  EXPECT_EQ(sourcemeta::core::Decimal{"1.0"}, sourcemeta::core::Decimal{1});
  EXPECT_EQ(sourcemeta::core::Decimal{"2.00"}, sourcemeta::core::Decimal{2});
}

TEST(Numeric_decimal, equal_different_scale_decimal) {
  EXPECT_EQ(sourcemeta::core::Decimal{"0.10"},
            sourcemeta::core::Decimal{"0.1"});
}

TEST(Numeric_decimal, equal_different_scale_zero) {
  EXPECT_EQ(sourcemeta::core::Decimal{"0.0"}, sourcemeta::core::Decimal{0});
  EXPECT_EQ(sourcemeta::core::Decimal{"0.00"}, sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, equal_integer_vs_exponent_form) {
  EXPECT_EQ(sourcemeta::core::Decimal{100}, sourcemeta::core::Decimal{"1e2"});
}

TEST(Numeric_decimal, equal_decimal_vs_exponent_form) {
  EXPECT_EQ(sourcemeta::core::Decimal{"0.1"},
            sourcemeta::core::Decimal{"1e-1"});
}

TEST(Numeric_decimal, compare_nan_not_equal_to_self) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value == nan_value);
}

TEST(Numeric_decimal, compare_nan_not_equal_to_nan) {
  EXPECT_FALSE(sourcemeta::core::Decimal::nan() ==
               sourcemeta::core::Decimal::nan());
}

TEST(Numeric_decimal, compare_nan_not_less_than_anything) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value < sourcemeta::core::Decimal{5});
  EXPECT_FALSE(nan_value < sourcemeta::core::Decimal::infinity());
}

TEST(Numeric_decimal, compare_nan_not_greater_than_anything) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value > sourcemeta::core::Decimal{5});
  EXPECT_FALSE(nan_value > sourcemeta::core::Decimal::negative_infinity());
}

TEST(Numeric_decimal, compare_nan_not_less_equal_anything) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value <= sourcemeta::core::Decimal{5});
}

TEST(Numeric_decimal, compare_nan_not_greater_equal_anything) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value >= sourcemeta::core::Decimal{5});
}

TEST(Numeric_decimal, compare_nan_not_equal_finite) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(nan_value != sourcemeta::core::Decimal{0});
  EXPECT_TRUE(nan_value != sourcemeta::core::Decimal{42});
}

TEST(Numeric_decimal, compare_nan_not_equal_infinity) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(nan_value != sourcemeta::core::Decimal::infinity());
  EXPECT_TRUE(nan_value != sourcemeta::core::Decimal::negative_infinity());
}

TEST(Numeric_decimal, compare_infinity_equal_infinity) {
  EXPECT_EQ(sourcemeta::core::Decimal::infinity(),
            sourcemeta::core::Decimal::infinity());
}

TEST(Numeric_decimal, compare_negative_infinity_equal_negative_infinity) {
  EXPECT_EQ(sourcemeta::core::Decimal::negative_infinity(),
            sourcemeta::core::Decimal::negative_infinity());
}

TEST(Numeric_decimal, compare_infinity_not_equal_negative_infinity) {
  EXPECT_NE(sourcemeta::core::Decimal::infinity(),
            sourcemeta::core::Decimal::negative_infinity());
}

TEST(Numeric_decimal, compare_infinity_greater_than_finite) {
  EXPECT_TRUE(sourcemeta::core::Decimal::infinity() >
              sourcemeta::core::Decimal{999999});
}

TEST(Numeric_decimal, compare_negative_infinity_less_than_finite) {
  EXPECT_TRUE(sourcemeta::core::Decimal::negative_infinity() <
              sourcemeta::core::Decimal{-999999});
}

TEST(Numeric_decimal, compare_infinity_greater_than_large_number) {
  EXPECT_TRUE(sourcemeta::core::Decimal::infinity() >
              sourcemeta::core::Decimal{"9e999999"});
}

TEST(Numeric_decimal, compare_negative_infinity_less_than_large_negative) {
  EXPECT_TRUE(sourcemeta::core::Decimal::negative_infinity() <
              sourcemeta::core::Decimal{"-9e999999"});
}

TEST(Numeric_decimal, compare_negative_zero_equal_zero) {
  EXPECT_EQ(sourcemeta::core::Decimal{"-0"}, sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, compare_negative_zero_less_equal_zero) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"-0"} <= sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, compare_negative_zero_greater_equal_zero) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"-0"} >= sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, compare_very_different_magnitudes) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"1e-300"} <
              sourcemeta::core::Decimal{"1e300"});
}

TEST(Numeric_decimal, compare_adjacent_values) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"1.000000000000000"} <
              sourcemeta::core::Decimal{"1.000000000000001"});
}

TEST(Numeric_decimal, compare_sign_difference) {
  EXPECT_TRUE(sourcemeta::core::Decimal{5} > sourcemeta::core::Decimal{-5});
  EXPECT_TRUE(sourcemeta::core::Decimal{-5} < sourcemeta::core::Decimal{5});
}

TEST(Numeric_decimal, compare_reflexive_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_EQ(value, value);
}

TEST(Numeric_decimal, compare_reflexive_positive) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_EQ(value, value);
}

TEST(Numeric_decimal, compare_reflexive_negative) {
  const sourcemeta::core::Decimal value{-1};
  EXPECT_EQ(value, value);
}

TEST(Numeric_decimal, compare_reflexive_decimal) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_EQ(value, value);
}

TEST(Numeric_decimal, compare_symmetric) {
  const sourcemeta::core::Decimal small{3};
  const sourcemeta::core::Decimal large{7};
  EXPECT_TRUE(small < large);
  EXPECT_TRUE(large > small);
}

TEST(Numeric_decimal, compare_transitive) {
  const sourcemeta::core::Decimal first{1};
  const sourcemeta::core::Decimal second{5};
  const sourcemeta::core::Decimal third{10};
  EXPECT_TRUE(first < second);
  EXPECT_TRUE(second < third);
  EXPECT_TRUE(first < third);
}

TEST(Numeric_decimal, compare_positive_greater_than_zero) {
  EXPECT_TRUE(sourcemeta::core::Decimal{1} > sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, compare_negative_less_than_zero) {
  EXPECT_TRUE(sourcemeta::core::Decimal{-1} < sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, compare_zero_equals_zero) {
  EXPECT_EQ(sourcemeta::core::Decimal{0}, sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, divisible_by_zero_dividend_nonzero_divisor) {
  const sourcemeta::core::Decimal dividend{0};
  const sourcemeta::core::Decimal divisor{5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_zero_dividend_zero_divisor) {
  const sourcemeta::core::Decimal dividend{0};
  const sourcemeta::core::Decimal divisor{0};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_negative_divisor_true) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{-5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_negative_divisor_false) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{-3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_both_negative_true) {
  const sourcemeta::core::Decimal dividend{-10};
  const sourcemeta::core::Decimal divisor{-5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_both_negative_false) {
  const sourcemeta::core::Decimal dividend{-7};
  const sourcemeta::core::Decimal divisor{-3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_fractional_tenth_true) {
  const sourcemeta::core::Decimal dividend{"1.0"};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_fractional_tenth_false_repeating) {
  const sourcemeta::core::Decimal dividend{"1.0"};
  const sourcemeta::core::Decimal divisor{"0.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_fractional_quarter_true) {
  const sourcemeta::core::Decimal dividend{"0.75"};
  const sourcemeta::core::Decimal divisor{"0.25"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_fractional_hundredth_true) {
  const sourcemeta::core::Decimal dividend{"0.1"};
  const sourcemeta::core::Decimal divisor{"0.01"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_one_tenth) {
  const sourcemeta::core::Decimal dividend{1};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_two_tenths) {
  const sourcemeta::core::Decimal dividend{2};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_three_tenths_true) {
  const sourcemeta::core::Decimal dividend{"0.3"};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_six_tenths_by_two_tenths_true) {
  const sourcemeta::core::Decimal dividend{"0.6"};
  const sourcemeta::core::Decimal divisor{"0.2"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_twelve_tenths_by_four_tenths_true) {
  const sourcemeta::core::Decimal dividend{"1.2"};
  const sourcemeta::core::Decimal divisor{"0.4"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_ten_by_tenth_true) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_ten_by_third_false) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{"0.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_one_by_thousandth_true) {
  const sourcemeta::core::Decimal dividend{1};
  const sourcemeta::core::Decimal divisor{"0.001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_hundredths_true) {
  const sourcemeta::core::Decimal dividend{"0.0075"};
  const sourcemeta::core::Decimal divisor{"0.0001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_hundredths_false) {
  const sourcemeta::core::Decimal dividend{"0.00751"};
  const sourcemeta::core::Decimal divisor{"0.0001"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_hundred_by_hundredth_true) {
  const sourcemeta::core::Decimal dividend{100};
  const sourcemeta::core::Decimal divisor{"0.01"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_tiny_self_divisible) {
  const sourcemeta::core::Decimal dividend{"0.0000001"};
  const sourcemeta::core::Decimal divisor{"0.0000001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_large_integer_by_three_true) {
  const sourcemeta::core::Decimal dividend{"999999999999999999"};
  const sourcemeta::core::Decimal divisor{3};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_large_integer_by_three_false) {
  const sourcemeta::core::Decimal dividend{"999999999999999997"};
  const sourcemeta::core::Decimal divisor{3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_large_quotient_exceeding_int64) {
  const sourcemeta::core::Decimal dividend{"99999999999999999999999999"};
  const sourcemeta::core::Decimal divisor{1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_different_scale_alignment) {
  const sourcemeta::core::Decimal dividend{"1000000.000000"};
  const sourcemeta::core::Decimal divisor{"0.000001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_seven_point_five_by_two_point_five_true) {
  const sourcemeta::core::Decimal dividend{"7.5"};
  const sourcemeta::core::Decimal divisor{"2.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_mixed_integer_decimal_true) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{"2.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_mixed_integer_decimal_false) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{"3.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal,
     divisible_by_point_zero_zero_seven_five_by_point_zero_zero_zero_one_true) {
  const sourcemeta::core::Decimal dividend{"0.0075"};
  const sourcemeta::core::Decimal divisor{"0.0001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(
    Numeric_decimal,
    divisible_by_point_zero_zero_seven_five_one_by_point_zero_zero_zero_one_false) {
  const sourcemeta::core::Decimal dividend{"0.00751"};
  const sourcemeta::core::Decimal divisor{"0.0001"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_infinity_dividend) {
  const auto dividend{sourcemeta::core::Decimal::infinity()};
  const sourcemeta::core::Decimal divisor{5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_infinity_divisor) {
  const sourcemeta::core::Decimal dividend{5};
  const auto divisor{sourcemeta::core::Decimal::infinity()};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, divisible_by_nan_dividend) {
  const auto dividend{sourcemeta::core::Decimal::nan()};
  const sourcemeta::core::Decimal divisor{5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(Numeric_decimal, parse_leading_zeros) {
  const sourcemeta::core::Decimal value{"007"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{7});
}

TEST(Numeric_decimal, parse_leading_zeros_decimal) {
  const sourcemeta::core::Decimal value{"00.5"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"0.5"});
}

TEST(Numeric_decimal, parse_explicit_plus_sign) {
  const sourcemeta::core::Decimal value{"+123"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{123});
}

TEST(Numeric_decimal, parse_explicit_plus_zero) {
  const sourcemeta::core::Decimal value{"+0"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, parse_explicit_plus_decimal) {
  const sourcemeta::core::Decimal value{"+1.5"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"1.5"});
}

TEST(Numeric_decimal, parse_scientific_uppercase_no_sign) {
  const sourcemeta::core::Decimal value{"1E10"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"10000000000"});
}

TEST(Numeric_decimal, parse_scientific_lowercase_no_sign) {
  const sourcemeta::core::Decimal value{"1e10"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"10000000000"});
}

TEST(Numeric_decimal, parse_scientific_uppercase_plus) {
  const sourcemeta::core::Decimal value{"1E+10"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"10000000000"});
}

TEST(Numeric_decimal, parse_scientific_lowercase_plus) {
  const sourcemeta::core::Decimal value{"1e+10"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"10000000000"});
}

TEST(Numeric_decimal, parse_scientific_uppercase_minus) {
  const sourcemeta::core::Decimal value{"1E-10"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"0.0000000001"});
}

TEST(Numeric_decimal, parse_scientific_lowercase_minus) {
  const sourcemeta::core::Decimal value{"1e-10"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"0.0000000001"});
}

TEST(Numeric_decimal, parse_bare_decimal_point_leading) {
  const sourcemeta::core::Decimal value{".5"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"0.5"});
}

TEST(Numeric_decimal, parse_bare_decimal_point_leading_negative) {
  const sourcemeta::core::Decimal value{"-.5"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"-0.5"});
}

TEST(Numeric_decimal, parse_bare_decimal_point_trailing) {
  const sourcemeta::core::Decimal value{"5."};
  EXPECT_EQ(value, sourcemeta::core::Decimal{5});
}

TEST(Numeric_decimal, parse_bare_decimal_point_trailing_negative) {
  const sourcemeta::core::Decimal value{"-5."};
  EXPECT_EQ(value, sourcemeta::core::Decimal{-5});
}

TEST(Numeric_decimal, parse_zero_point_zero) {
  const sourcemeta::core::Decimal value{"0.0"};
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, parse_zero_point_zero_zero) {
  const sourcemeta::core::Decimal value{"0.00"};
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, parse_zero_e_zero) {
  const sourcemeta::core::Decimal value{"0e0"};
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, parse_negative_zero) {
  const sourcemeta::core::Decimal value{"-0"};
  EXPECT_TRUE(value.is_signed());
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, parse_negative_zero_point_zero) {
  const sourcemeta::core::Decimal value{"-0.0"};
  EXPECT_TRUE(value.is_signed());
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, parse_plus_zero) {
  const sourcemeta::core::Decimal value{"+0"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{0});
  EXPECT_FALSE(value.is_signed());
}

TEST(Numeric_decimal, parse_zero_with_large_positive_exponent) {
  const sourcemeta::core::Decimal value{"0e100"};
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, parse_zero_with_large_negative_exponent) {
  const sourcemeta::core::Decimal value{"0e-100"};
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, parse_special_nan) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"NaN"}.is_nan());
  EXPECT_TRUE(sourcemeta::core::Decimal{"nan"}.is_nan());
  EXPECT_TRUE(sourcemeta::core::Decimal{"NAN"}.is_nan());
}

TEST(Numeric_decimal, parse_special_infinity) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"Infinity"}.is_infinite());
  EXPECT_TRUE(sourcemeta::core::Decimal{"infinity"}.is_infinite());
  EXPECT_TRUE(sourcemeta::core::Decimal{"INFINITY"}.is_infinite());
}

TEST(Numeric_decimal, parse_special_negative_infinity) {
  const sourcemeta::core::Decimal value{"-Infinity"};
  EXPECT_TRUE(value.is_infinite());
  EXPECT_TRUE(value.is_signed());
}

TEST(Numeric_decimal, parse_special_inf_shorthand) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"Inf"}.is_infinite());
}

TEST(Numeric_decimal, parse_special_negative_inf_shorthand) {
  const sourcemeta::core::Decimal value{"-Inf"};
  EXPECT_TRUE(value.is_infinite());
  EXPECT_TRUE(value.is_signed());
}

TEST(Numeric_decimal, parse_special_plus_infinity) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"+Infinity"}.is_infinite());
  EXPECT_TRUE(sourcemeta::core::Decimal{"+Inf"}.is_infinite());
}

TEST(Numeric_decimal, parse_many_digits_integer) {
  const sourcemeta::core::Decimal value{
      "12345678901234567890123456789012345678901234567890"};
  EXPECT_FALSE(value.is_zero());
  EXPECT_TRUE(value.is_finite());
}

TEST(Numeric_decimal, parse_many_digits_fractional) {
  const sourcemeta::core::Decimal value{
      "0.12345678901234567890123456789012345678901234567890"};
  EXPECT_FALSE(value.is_zero());
  EXPECT_TRUE(value.is_finite());
}

TEST(Numeric_decimal, parse_very_large_exponent) {
  const sourcemeta::core::Decimal value{"1e999999"};
  EXPECT_FALSE(value.is_zero());
  EXPECT_TRUE(value.is_finite());
}

TEST(Numeric_decimal, parse_very_large_negative_exponent) {
  const sourcemeta::core::Decimal value{"1e-999999"};
  EXPECT_FALSE(value.is_zero());
  EXPECT_TRUE(value.is_finite());
}

TEST(Numeric_decimal, parse_reject_whitespace_leading) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{" 123"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_whitespace_trailing) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"123 "}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_whitespace_both) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{" 123 "}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_bare_dot) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"."}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_bare_e) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"e5"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_double_dots) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"1.2.3"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_double_negative) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"--1"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_double_plus) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"++1"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_e_plus_only) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"1e+"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_e_minus_only) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"1e-"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_double_e) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"1ee5"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, parse_reject_trailing_alpha) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"123abc"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(Numeric_decimal, to_string_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_EQ(value.to_string(), "0");
}

TEST(Numeric_decimal, to_string_negative_zero) {
  const sourcemeta::core::Decimal value{"-0"};
  EXPECT_EQ(value.to_string(), "-0");
}

TEST(Numeric_decimal, to_string_zero_point_zero) {
  const sourcemeta::core::Decimal value{"0.0"};
  EXPECT_EQ(value.to_string(), "0.0");
}

TEST(Numeric_decimal, to_string_trailing_zeros_preserved) {
  const sourcemeta::core::Decimal value{"1.000"};
  EXPECT_EQ(value.to_string(), "1.000");
}

TEST(Numeric_decimal, to_string_very_small_decimal) {
  const sourcemeta::core::Decimal value{"0.000000001"};
  EXPECT_EQ(value.to_string(), "1e-9");
}

TEST(Numeric_decimal, to_string_nan) {
  EXPECT_EQ(sourcemeta::core::Decimal::nan().to_string(), "NaN");
}

TEST(Numeric_decimal, to_string_infinity) {
  EXPECT_EQ(sourcemeta::core::Decimal::infinity().to_string(), "Infinity");
}

TEST(Numeric_decimal, to_string_negative_infinity) {
  EXPECT_EQ(sourcemeta::core::Decimal::negative_infinity().to_string(),
            "-Infinity");
}

TEST(Numeric_decimal, to_string_from_exponent_form) {
  const sourcemeta::core::Decimal value{"1e5"};
  EXPECT_EQ(value.to_string(), "100e+3");
}

TEST(Numeric_decimal, to_scientific_string_large_integer) {
  const sourcemeta::core::Decimal value{1000000};
  EXPECT_EQ(value.to_scientific_string(), "1.000000e+6");
}

TEST(Numeric_decimal, to_scientific_string_small_decimal) {
  const sourcemeta::core::Decimal value{"0.001"};
  EXPECT_EQ(value.to_scientific_string(), "1e-3");
}

TEST(Numeric_decimal, to_scientific_string_negative) {
  const sourcemeta::core::Decimal value{-42};
  EXPECT_EQ(value.to_scientific_string(), "-4.2e+1");
}

TEST(Numeric_decimal, to_scientific_string_nan) {
  EXPECT_EQ(sourcemeta::core::Decimal::nan().to_scientific_string(), "NaN");
}

TEST(Numeric_decimal, to_scientific_string_infinity) {
  EXPECT_EQ(sourcemeta::core::Decimal::infinity().to_scientific_string(),
            "Infinity");
}

TEST(Numeric_decimal, to_scientific_string_negative_infinity) {
  EXPECT_EQ(
      sourcemeta::core::Decimal::negative_infinity().to_scientific_string(),
      "-Infinity");
}

TEST(Numeric_decimal, to_scientific_string_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_EQ(value.to_scientific_string(), "0e+0");
}

TEST(Numeric_decimal, roundtrip_to_string_parse_integer) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_EQ(sourcemeta::core::Decimal{value.to_string()}, value);
}

TEST(Numeric_decimal, roundtrip_to_string_parse_decimal) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_EQ(sourcemeta::core::Decimal{value.to_string()}, value);
}

TEST(Numeric_decimal, roundtrip_to_string_parse_negative) {
  const sourcemeta::core::Decimal value{"-999.999"};
  EXPECT_EQ(sourcemeta::core::Decimal{value.to_string()}, value);
}

TEST(Numeric_decimal, roundtrip_to_string_parse_large) {
  const sourcemeta::core::Decimal value{"123456789012345678901234567890"};
  EXPECT_EQ(sourcemeta::core::Decimal{value.to_string()}, value);
}

TEST(Numeric_decimal, roundtrip_to_string_parse_small) {
  const sourcemeta::core::Decimal value{"0.000000001"};
  EXPECT_EQ(sourcemeta::core::Decimal{value.to_string()}, value);
}

TEST(Numeric_decimal, add_infinity_to_infinity) {
  const auto result{sourcemeta::core::Decimal::infinity() +
                    sourcemeta::core::Decimal::infinity()};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_FALSE(result.is_signed());
}

TEST(Numeric_decimal, add_infinity_to_finite) {
  const auto result{sourcemeta::core::Decimal::infinity() +
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_FALSE(result.is_signed());
}

TEST(Numeric_decimal, add_negative_infinity_to_negative_infinity) {
  const auto result{sourcemeta::core::Decimal::negative_infinity() +
                    sourcemeta::core::Decimal::negative_infinity()};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_TRUE(result.is_signed());
}

TEST(Numeric_decimal, subtract_infinity_from_infinity_produces_nan) {
  const auto result{sourcemeta::core::Decimal::infinity() -
                    sourcemeta::core::Decimal::infinity()};
  EXPECT_TRUE(result.is_nan());
}

TEST(Numeric_decimal, multiply_infinity_by_positive) {
  const auto result{sourcemeta::core::Decimal::infinity() *
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_FALSE(result.is_signed());
}

TEST(Numeric_decimal, multiply_infinity_by_negative) {
  const auto result{sourcemeta::core::Decimal::infinity() *
                    sourcemeta::core::Decimal{-5}};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_TRUE(result.is_signed());
}

TEST(Numeric_decimal, multiply_infinity_by_zero_produces_nan) {
  const auto result{sourcemeta::core::Decimal::infinity() *
                    sourcemeta::core::Decimal{0}};
  EXPECT_TRUE(result.is_nan());
}

TEST(Numeric_decimal, divide_finite_by_infinity) {
  const auto result{sourcemeta::core::Decimal{5} /
                    sourcemeta::core::Decimal::infinity()};
  EXPECT_TRUE(result.is_zero());
}

TEST(Numeric_decimal, divide_infinity_by_finite) {
  const auto result{sourcemeta::core::Decimal::infinity() /
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_FALSE(result.is_signed());
}

TEST(Numeric_decimal, divide_infinity_by_infinity_throws) {
  EXPECT_THROW(
      {
        const auto result{sourcemeta::core::Decimal::infinity() /
                          sourcemeta::core::Decimal::infinity()};
      },
      sourcemeta::core::NumericInvalidOperationError);
}

TEST(Numeric_decimal, add_nan_propagates) {
  const auto result{sourcemeta::core::Decimal::nan() +
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_nan());
}

TEST(Numeric_decimal, multiply_nan_propagates) {
  const auto result{sourcemeta::core::Decimal::nan() *
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_nan());
}

TEST(Numeric_decimal, divide_nan_propagates) {
  const auto result{sourcemeta::core::Decimal::nan() /
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_nan());
}

TEST(Numeric_decimal, subtract_nan_propagates) {
  const auto result{sourcemeta::core::Decimal::nan() -
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_nan());
}

TEST(Numeric_decimal, add_different_scales) {
  EXPECT_EQ(sourcemeta::core::Decimal{"1.0"} + sourcemeta::core::Decimal{2},
            sourcemeta::core::Decimal{3});
  EXPECT_EQ(sourcemeta::core::Decimal{"10.00"} -
                sourcemeta::core::Decimal{"9.9"},
            sourcemeta::core::Decimal{"0.10"});
}

TEST(Numeric_decimal, subtract_different_scales) {
  EXPECT_EQ(sourcemeta::core::Decimal{"10.000"} -
                sourcemeta::core::Decimal{"0.1"},
            sourcemeta::core::Decimal{"9.900"});
}

TEST(Numeric_decimal, add_classic_fp_trouble_exact_in_decimal) {
  const auto result{sourcemeta::core::Decimal{"0.1"} +
                    sourcemeta::core::Decimal{"0.2"}};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"0.3"});
}

TEST(Numeric_decimal, commutativity_addition) {
  const sourcemeta::core::Decimal first{"3.14"};
  const sourcemeta::core::Decimal second{"2.71"};
  EXPECT_EQ(first + second, second + first);
}

TEST(Numeric_decimal, commutativity_multiplication) {
  const sourcemeta::core::Decimal first{"3.14"};
  const sourcemeta::core::Decimal second{"2.71"};
  EXPECT_EQ(first * second, second * first);
}

TEST(Numeric_decimal, additive_identity_positive) {
  EXPECT_EQ(sourcemeta::core::Decimal{42} + sourcemeta::core::Decimal{0},
            sourcemeta::core::Decimal{42});
}

TEST(Numeric_decimal, additive_identity_negative) {
  EXPECT_EQ(sourcemeta::core::Decimal{-17} + sourcemeta::core::Decimal{0},
            sourcemeta::core::Decimal{-17});
}

TEST(Numeric_decimal, additive_identity_decimal) {
  EXPECT_EQ(sourcemeta::core::Decimal{"3.14"} + sourcemeta::core::Decimal{0},
            sourcemeta::core::Decimal{"3.14"});
}

TEST(Numeric_decimal, additive_identity_large) {
  EXPECT_EQ(sourcemeta::core::Decimal{"1e100"} + sourcemeta::core::Decimal{0},
            sourcemeta::core::Decimal{"1e100"});
}

TEST(Numeric_decimal, multiplicative_identity_positive) {
  EXPECT_EQ(sourcemeta::core::Decimal{42} * sourcemeta::core::Decimal{1},
            sourcemeta::core::Decimal{42});
}

TEST(Numeric_decimal, multiplicative_identity_negative) {
  EXPECT_EQ(sourcemeta::core::Decimal{-17} * sourcemeta::core::Decimal{1},
            sourcemeta::core::Decimal{-17});
}

TEST(Numeric_decimal, multiplicative_identity_decimal) {
  EXPECT_EQ(sourcemeta::core::Decimal{"3.14"} * sourcemeta::core::Decimal{1},
            sourcemeta::core::Decimal{"3.14"});
}

TEST(Numeric_decimal, multiplicative_identity_large) {
  EXPECT_EQ(sourcemeta::core::Decimal{"1e100"} * sourcemeta::core::Decimal{1},
            sourcemeta::core::Decimal{"1e100"});
}

TEST(Numeric_decimal, subtract_from_zero_equals_negation) {
  const sourcemeta::core::Decimal zero{0};
  const sourcemeta::core::Decimal value{42};
  EXPECT_EQ(zero - value, -value);
}

TEST(Numeric_decimal, divide_self_equals_one_positive) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_EQ(value / value, sourcemeta::core::Decimal{1});
}

TEST(Numeric_decimal, divide_self_equals_one_negative) {
  const sourcemeta::core::Decimal value{-17};
  EXPECT_EQ(value / value, sourcemeta::core::Decimal{1});
}

TEST(Numeric_decimal, divide_self_equals_one_decimal) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_EQ(value / value, sourcemeta::core::Decimal{1});
}

TEST(Numeric_decimal, divide_self_equals_one_small) {
  const sourcemeta::core::Decimal value{"0.001"};
  EXPECT_EQ(value / value, sourcemeta::core::Decimal{1});
}

TEST(Numeric_decimal, modulo_negative_divisor) {
  const sourcemeta::core::Decimal result{sourcemeta::core::Decimal{17} %
                                         sourcemeta::core::Decimal{-5}};
  EXPECT_EQ(result, sourcemeta::core::Decimal{2});
}

TEST(Numeric_decimal, modulo_both_negative) {
  const sourcemeta::core::Decimal result{sourcemeta::core::Decimal{-17} %
                                         sourcemeta::core::Decimal{-5}};
  EXPECT_EQ(result, sourcemeta::core::Decimal{-2});
}

TEST(Numeric_decimal, modulo_decimal_operands) {
  EXPECT_EQ(sourcemeta::core::Decimal{"10.5"} % sourcemeta::core::Decimal{3},
            sourcemeta::core::Decimal{"1.5"});
  EXPECT_EQ(sourcemeta::core::Decimal{"7.5"} % sourcemeta::core::Decimal{"2.5"},
            sourcemeta::core::Decimal{"0.0"});
}

TEST(Numeric_decimal, modulo_result_zero) {
  EXPECT_EQ(sourcemeta::core::Decimal{100} % sourcemeta::core::Decimal{25},
            sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, modulo_zero_dividend) {
  EXPECT_EQ(sourcemeta::core::Decimal{0} % sourcemeta::core::Decimal{5},
            sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, modulo_dividend_less_than_divisor) {
  EXPECT_EQ(sourcemeta::core::Decimal{3} % sourcemeta::core::Decimal{7},
            sourcemeta::core::Decimal{3});
}

TEST(Numeric_decimal, modulo_dividend_equals_divisor) {
  EXPECT_EQ(sourcemeta::core::Decimal{7} % sourcemeta::core::Decimal{7},
            sourcemeta::core::Decimal{0});
}

TEST(Numeric_decimal, is_int32_below_min_boundary) {
  const sourcemeta::core::Decimal value{"-2147483649"};
  EXPECT_FALSE(value.is_int32());
}

TEST(Numeric_decimal, is_int32_above_max_boundary) {
  const sourcemeta::core::Decimal value{"2147483648"};
  EXPECT_FALSE(value.is_int32());
}

TEST(Numeric_decimal, is_int64_at_min_boundary) {
  const sourcemeta::core::Decimal value{"-9223372036854775808"};
  EXPECT_TRUE(value.is_int64());
}

TEST(Numeric_decimal, is_int64_below_min_boundary) {
  const sourcemeta::core::Decimal value{"-9223372036854775809"};
  EXPECT_FALSE(value.is_int64());
}

TEST(Numeric_decimal, is_int64_at_max_boundary) {
  const sourcemeta::core::Decimal value{"9223372036854775807"};
  EXPECT_TRUE(value.is_int64());
}

TEST(Numeric_decimal, is_int64_above_max_boundary) {
  const sourcemeta::core::Decimal value{"9223372036854775808"};
  EXPECT_FALSE(value.is_int64());
}

TEST(Numeric_decimal, is_uint32_at_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_TRUE(value.is_uint32());
}

TEST(Numeric_decimal, is_uint64_at_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_TRUE(value.is_uint64());
}

TEST(Numeric_decimal, to_int32_at_boundaries) {
  const sourcemeta::core::Decimal min_value{
      static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min())};
  const sourcemeta::core::Decimal max_value{
      std::numeric_limits<std::int32_t>::max()};
  EXPECT_EQ(min_value.to_int32(), std::numeric_limits<std::int32_t>::min());
  EXPECT_EQ(max_value.to_int32(), std::numeric_limits<std::int32_t>::max());
}

TEST(Numeric_decimal, to_int64_at_boundaries) {
  const sourcemeta::core::Decimal min_value{"-9223372036854775808"};
  const sourcemeta::core::Decimal max_value{"9223372036854775807"};
  EXPECT_EQ(min_value.to_int64(), std::numeric_limits<std::int64_t>::min());
  EXPECT_EQ(max_value.to_int64(), std::numeric_limits<std::int64_t>::max());
}

TEST(Numeric_decimal, to_uint32_at_boundaries) {
  const sourcemeta::core::Decimal zero{0};
  const sourcemeta::core::Decimal max_value{
      std::numeric_limits<std::uint32_t>::max()};
  EXPECT_EQ(zero.to_uint32(), 0U);
  EXPECT_EQ(max_value.to_uint32(), std::numeric_limits<std::uint32_t>::max());
}

TEST(Numeric_decimal, to_uint64_at_boundaries) {
  const sourcemeta::core::Decimal zero{0};
  const sourcemeta::core::Decimal max_value{"18446744073709551615"};
  EXPECT_EQ(zero.to_uint64(), 0ULL);
  EXPECT_EQ(max_value.to_uint64(), std::numeric_limits<std::uint64_t>::max());
}

TEST(Numeric_decimal, int_conversion_value_with_exponent_resolves_to_boundary) {
  const sourcemeta::core::Decimal value{"1e19"};
  EXPECT_TRUE(value.is_integer());
  EXPECT_FALSE(value.is_int64());
  EXPECT_TRUE(value.is_uint64());
}

TEST(Numeric_decimal, int_conversion_negative_zero_gives_zero) {
  const sourcemeta::core::Decimal value{"-0"};
  EXPECT_TRUE(value.is_integer());
  EXPECT_EQ(value.to_int64(), 0);
}

TEST(Numeric_decimal, int_conversion_power_of_ten_boundaries) {
  const sourcemeta::core::Decimal value{"10000000000000000000"};
  EXPECT_TRUE(value.is_integer());
  EXPECT_FALSE(value.is_int64());
  EXPECT_TRUE(value.is_uint64());
}

TEST(Numeric_decimal, construct_from_int64_min) {
  const sourcemeta::core::Decimal value{
      std::numeric_limits<std::int64_t>::min()};
  EXPECT_TRUE(value.is_signed());
  EXPECT_TRUE(value.is_integer());
  EXPECT_TRUE(value.is_int64());
  EXPECT_EQ(value.to_int64(), std::numeric_limits<std::int64_t>::min());
}

TEST(Numeric_decimal, construct_from_int64_max) {
  const sourcemeta::core::Decimal value{
      std::numeric_limits<std::int64_t>::max()};
  EXPECT_FALSE(value.is_signed());
  EXPECT_TRUE(value.is_integer());
  EXPECT_TRUE(value.is_int64());
  EXPECT_EQ(value.to_int64(), std::numeric_limits<std::int64_t>::max());
}

TEST(Numeric_decimal, construct_from_uint64_max) {
  const sourcemeta::core::Decimal value{
      std::numeric_limits<std::uint64_t>::max()};
  EXPECT_FALSE(value.is_signed());
  EXPECT_TRUE(value.is_integer());
  EXPECT_TRUE(value.is_uint64());
  EXPECT_EQ(value.to_uint64(), std::numeric_limits<std::uint64_t>::max());
}

TEST(Numeric_decimal, construct_from_int8) {
  const sourcemeta::core::Decimal value{static_cast<std::int8_t>(-42)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{-42});
}

TEST(Numeric_decimal, construct_from_int16) {
  const sourcemeta::core::Decimal value{static_cast<std::int16_t>(-1000)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{-1000});
}

TEST(Numeric_decimal, construct_from_uint8) {
  const sourcemeta::core::Decimal value{static_cast<std::uint8_t>(255)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{255});
}

TEST(Numeric_decimal, construct_from_uint16) {
  const sourcemeta::core::Decimal value{static_cast<std::uint16_t>(65535)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{65535});
}

TEST(Numeric_decimal, construct_from_int32) {
  const sourcemeta::core::Decimal value{static_cast<std::int32_t>(-2000000)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{-2000000});
}

TEST(Numeric_decimal, construct_from_uint32_explicit) {
  const sourcemeta::core::Decimal value{static_cast<std::uint32_t>(4000000000)};
  EXPECT_EQ(value.to_string(), "4000000000");
}

TEST(Numeric_decimal, construct_from_float_nan) {
  const sourcemeta::core::Decimal value{std::nanf("")};
  EXPECT_TRUE(value.is_nan());
}

TEST(Numeric_decimal, construct_from_float_infinity) {
  const sourcemeta::core::Decimal value{INFINITY};
  EXPECT_TRUE(value.is_infinite());
  EXPECT_FALSE(value.is_signed());
}

TEST(Numeric_decimal, construct_from_double_nan) {
  const sourcemeta::core::Decimal value{std::nan("")};
  EXPECT_TRUE(value.is_nan());
}

TEST(Numeric_decimal, construct_from_double_infinity) {
  const sourcemeta::core::Decimal value{static_cast<double>(INFINITY)};
  EXPECT_TRUE(value.is_infinite());
  EXPECT_FALSE(value.is_signed());
}

TEST(Numeric_decimal, self_copy_assignment) {
  sourcemeta::core::Decimal value{42};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
  value = value;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
  EXPECT_EQ(value, sourcemeta::core::Decimal{42});
}

TEST(Numeric_decimal, self_move_assignment) {
  sourcemeta::core::Decimal value{42};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
  value = std::move(value);
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
  EXPECT_EQ(value, sourcemeta::core::Decimal{42});
}

TEST(Numeric_decimal, move_from_state_is_valid) {
  sourcemeta::core::Decimal original{42};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved, sourcemeta::core::Decimal{42});
  original = sourcemeta::core::Decimal{99};
  EXPECT_EQ(original, sourcemeta::core::Decimal{99});
}

TEST(Numeric_decimal, unary_minus_zero) {
  const sourcemeta::core::Decimal value{0};
  const auto result{-value};
  EXPECT_TRUE(result.is_zero());
}

TEST(Numeric_decimal, unary_minus_negative_zero) {
  const sourcemeta::core::Decimal value{"-0"};
  const auto result{-value};
  EXPECT_TRUE(result.is_zero());
  EXPECT_FALSE(result.is_signed());
}

TEST(Numeric_decimal, unary_minus_nan) {
  const auto result{-sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(result.is_nan());
}

TEST(Numeric_decimal, unary_minus_infinity) {
  const auto result{-sourcemeta::core::Decimal::infinity()};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_TRUE(result.is_signed());
}

TEST(Numeric_decimal, unary_minus_negative_infinity) {
  const auto result{-sourcemeta::core::Decimal::negative_infinity()};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_FALSE(result.is_signed());
}

TEST(Numeric_decimal, unary_plus_negative) {
  const sourcemeta::core::Decimal value{-5};
  EXPECT_EQ(+value, sourcemeta::core::Decimal{-5});
}

TEST(Numeric_decimal, unary_plus_nan) {
  const auto result{+sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(result.is_nan());
}

TEST(Numeric_decimal, unary_plus_infinity) {
  const auto result{+sourcemeta::core::Decimal::infinity()};
  EXPECT_TRUE(result.is_infinite());
  EXPECT_FALSE(result.is_signed());
}

TEST(Numeric_decimal, double_negation_identity_positive) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_EQ(-(-value), value);
}

TEST(Numeric_decimal, double_negation_identity_negative) {
  const sourcemeta::core::Decimal value{-17};
  EXPECT_EQ(-(-value), value);
}

TEST(Numeric_decimal, double_negation_identity_decimal) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_EQ(-(-value), value);
}

TEST(Numeric_decimal, double_negation_identity_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_EQ(-(-value), value);
}

TEST(Numeric_decimal, is_zero_zero_point_zero) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"0.0"}.is_zero());
}

TEST(Numeric_decimal, is_zero_zero_point_zero_zero) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"0.00"}.is_zero());
}

TEST(Numeric_decimal, is_zero_zero_e_five) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"0e5"}.is_zero());
}

TEST(Numeric_decimal, is_zero_negative_zero) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"-0"}.is_zero());
}

TEST(Numeric_decimal, is_zero_negative_zero_point_zero) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"-0.0"}.is_zero());
}

TEST(Numeric_decimal, is_zero_zero_e_hundred) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"0e100"}.is_zero());
}

TEST(Numeric_decimal, is_zero_zero_e_negative_hundred) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"0e-100"}.is_zero());
}

TEST(Numeric_decimal, is_integral_via_positive_exponent) {
  const sourcemeta::core::Decimal value{"1e10"};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_via_negative_exponent_resolving) {
  const sourcemeta::core::Decimal value{"100e-2"};
  EXPECT_TRUE(value.is_integral());
}

TEST(Numeric_decimal, is_integral_via_negative_exponent_not_resolving) {
  const sourcemeta::core::Decimal value{"123e-3"};
  EXPECT_FALSE(value.is_integral());
}

TEST(Numeric_decimal, is_integer_vs_integral_distinction) {
  const sourcemeta::core::Decimal value{"3.0"};
  EXPECT_TRUE(value.is_integral());
  EXPECT_FALSE(value.is_integer());
}

TEST(Numeric_decimal, is_finite_nan) {
  EXPECT_FALSE(sourcemeta::core::Decimal::nan().is_finite());
}

TEST(Numeric_decimal, is_finite_infinity) {
  EXPECT_FALSE(sourcemeta::core::Decimal::infinity().is_finite());
}

TEST(Numeric_decimal, is_finite_negative_infinity) {
  EXPECT_FALSE(sourcemeta::core::Decimal::negative_infinity().is_finite());
}

TEST(Numeric_decimal, is_signed_nan) {
  EXPECT_FALSE(sourcemeta::core::Decimal::nan().is_signed());
}

TEST(Numeric_decimal, is_signed_infinity) {
  EXPECT_FALSE(sourcemeta::core::Decimal::infinity().is_signed());
}

TEST(Numeric_decimal, is_signed_negative_infinity) {
  EXPECT_TRUE(sourcemeta::core::Decimal::negative_infinity().is_signed());
}

TEST(Numeric_decimal, compound_add_self_doubles) {
  sourcemeta::core::Decimal value{25};
  value += value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{50});
}

TEST(Numeric_decimal, compound_subtract_self_zeros) {
  sourcemeta::core::Decimal value{25};
  value -= value;
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, compound_multiply_self_squares) {
  sourcemeta::core::Decimal value{5};
  value *= value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{25});
}

TEST(Numeric_decimal, compound_divide_self_gives_one) {
  sourcemeta::core::Decimal value{42};
  value /= value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{1});
}

TEST(Numeric_decimal, compound_modulo_self_gives_zero) {
  sourcemeta::core::Decimal value{42};
  value %= value;
  EXPECT_TRUE(value.is_zero());
}

TEST(Numeric_decimal, multithreaded_parsing) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal value{"3.14159265358979"};
      EXPECT_FALSE(value.is_zero());
      EXPECT_TRUE(value.is_finite());
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(Numeric_decimal, multithreaded_stringification) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal value{12345};
      EXPECT_EQ(value.to_string(), "12345");
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(Numeric_decimal, multithreaded_comparison) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal left{100};
      const sourcemeta::core::Decimal right{200};
      EXPECT_TRUE(left < right);
      EXPECT_TRUE(right > left);
      EXPECT_EQ(left, left);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(Numeric_decimal, multithreaded_divisibility) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal dividend{100};
      const sourcemeta::core::Decimal divisor{5};
      EXPECT_TRUE(dividend.divisible_by(divisor));
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(Numeric_decimal, multithreaded_mixed_operations) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([index]() {
      if (index % 4 == 0) {
        EXPECT_EQ(sourcemeta::core::Decimal{10} + sourcemeta::core::Decimal{5},
                  sourcemeta::core::Decimal{15});
      } else if (index % 4 == 1) {
        EXPECT_EQ(sourcemeta::core::Decimal{10} * sourcemeta::core::Decimal{3},
                  sourcemeta::core::Decimal{30});
      } else if (index % 4 == 2) {
        EXPECT_EQ(sourcemeta::core::Decimal{10} / sourcemeta::core::Decimal{2},
                  sourcemeta::core::Decimal{5});
      } else {
        EXPECT_EQ(sourcemeta::core::Decimal{"3.14"}.to_string(), "3.14");
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(Numeric_decimal, multithreaded_high_thread_count) {
  constexpr int number_of_threads{32};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal value{42};
      const sourcemeta::core::Decimal result{value +
                                             sourcemeta::core::Decimal{1}};
      EXPECT_EQ(result, sourcemeta::core::Decimal{43});
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}
