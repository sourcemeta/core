#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <string> // std::string

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
  EXPECT_EQ(str, "123.456");
}

TEST(Numeric_decimal, parse_negative_string) {
  const sourcemeta::core::Decimal value{"-987.654"};
  const std::string str{value.to_scientific_string()};
  EXPECT_EQ(str, "-987.654");
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
  EXPECT_EQ(str, "1E-9");
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
  EXPECT_EQ(str, "1.000");
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

TEST(Numeric_decimal, is_finite_predicate) {
  const sourcemeta::core::Decimal value{123};
  EXPECT_TRUE(value.is_finite());
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
      decimal::ConversionSyntax);
}

TEST(Numeric_decimal, exception_conversion_syntax_empty_string) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{""}; },
      decimal::ConversionSyntax);
}

TEST(Numeric_decimal, exception_conversion_syntax_invalid_exponent) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"123e"}; },
      decimal::ConversionSyntax);
}

TEST(Numeric_decimal, exception_conversion_syntax_multiple_dots) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"12.34.56"}; },
      decimal::ConversionSyntax);
}

TEST(Numeric_decimal, exception_division_by_zero_divide) {
  const sourcemeta::core::Decimal numerator{10};
  const sourcemeta::core::Decimal denominator{0};
  EXPECT_THROW(
      { const auto result = numerator / denominator; },
      decimal::DivisionByZero);
}

TEST(Numeric_decimal, exception_division_by_zero_divide_assign) {
  sourcemeta::core::Decimal numerator{10};
  const sourcemeta::core::Decimal denominator{0};
  EXPECT_THROW({ numerator /= denominator; }, decimal::DivisionByZero);
}

TEST(Numeric_decimal, exception_invalid_operation_modulo_by_zero) {
  const sourcemeta::core::Decimal numerator{10};
  const sourcemeta::core::Decimal denominator{0};
  EXPECT_THROW(
      { const auto result = numerator % denominator; },
      decimal::InvalidOperation);
}

TEST(Numeric_decimal, exception_invalid_operation_modulo_assign_by_zero) {
  sourcemeta::core::Decimal numerator{10};
  const sourcemeta::core::Decimal denominator{0};
  EXPECT_THROW({ numerator %= denominator; }, decimal::InvalidOperation);
}

TEST(Numeric_decimal, exception_invalid_operation_zero_divided_by_zero) {
  const sourcemeta::core::Decimal zero{0};
  EXPECT_THROW(
      { const auto result = zero / zero; }, decimal::IEEEInvalidOperation);
}

TEST(Numeric_decimal, exception_invalid_operation_zero_modulo_zero) {
  const sourcemeta::core::Decimal zero{0};
  EXPECT_THROW(
      { const auto result = zero % zero; }, decimal::IEEEInvalidOperation);
}
