#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <cstdint>   // std::int64_t
#include <limits>    // std::numeric_limits
#include <stdexcept> // std::out_of_range

TEST(is_number_zero) {
  const sourcemeta::core::JSON document{0};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_zero_real) {
  const sourcemeta::core::JSON document{0.0};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_positive_integer) {
  const sourcemeta::core::JSON document{5};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_positive_real) {
  const sourcemeta::core::JSON document{5.3};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_negative_integer) {
  const sourcemeta::core::JSON document{-5};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_negative_real) {
  const sourcemeta::core::JSON document{-5.3};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_string) {
  const sourcemeta::core::JSON document{"0"};
  EXPECT_FALSE(document.is_number());
}

TEST(is_number_null) {
  const sourcemeta::core::JSON document{nullptr};
  EXPECT_FALSE(document.is_number());
}

TEST(is_positive_negative_integer) {
  const sourcemeta::core::JSON document{-3};
  EXPECT_FALSE(document.is_positive());
}

TEST(is_positive_zero_integer) {
  const sourcemeta::core::JSON document{0};
  EXPECT_TRUE(document.is_positive());
}

TEST(is_positive_positive_integer) {
  const sourcemeta::core::JSON document{5};
  EXPECT_TRUE(document.is_positive());
}

TEST(is_positive_negative_real) {
  const sourcemeta::core::JSON document{-3.2};
  EXPECT_FALSE(document.is_positive());
}

TEST(is_positive_zero_real) {
  const sourcemeta::core::JSON document{0.0};
  EXPECT_TRUE(document.is_positive());
}

TEST(is_positive_positive_real) {
  const sourcemeta::core::JSON document{5.1};
  EXPECT_TRUE(document.is_positive());
}

TEST(add_integer_integer) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{3};
  const sourcemeta::core::JSON document{left + right};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 8);
}

TEST(add_integer_real) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{3.2};
  const sourcemeta::core::JSON document{left + right};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 8.2);
}

TEST(add_real_integer) {
  const sourcemeta::core::JSON left{3.2};
  const sourcemeta::core::JSON right{2};
  const sourcemeta::core::JSON document{left + right};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 5.2);
}

TEST(add_real_real) {
  const sourcemeta::core::JSON left{3.2};
  const sourcemeta::core::JSON right{2.0};
  const sourcemeta::core::JSON document{left + right};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 5.2);
}

TEST(substract_integer_integer) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{3};
  const sourcemeta::core::JSON document{left - right};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 2);
}

TEST(substract_integer_real) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{3.2};
  const sourcemeta::core::JSON document{left - right};
  EXPECT_TRUE(document.is_real());
  EXPECT_DOUBLE_EQ(document.to_real(), 1.8);
}

TEST(substract_real_integer) {
  const sourcemeta::core::JSON left{3.2};
  const sourcemeta::core::JSON right{2};
  const sourcemeta::core::JSON document{left - right};
  EXPECT_TRUE(document.is_real());
  EXPECT_DOUBLE_EQ(document.to_real(), 1.2);
}

TEST(substract_real_real) {
  const sourcemeta::core::JSON left{3.2};
  const sourcemeta::core::JSON right{2.0};
  const sourcemeta::core::JSON document{left - right};
  EXPECT_TRUE(document.is_real());
  EXPECT_DOUBLE_EQ(document.to_real(), 1.2);
}

TEST(increment_integer_integer) {
  sourcemeta::core::JSON document{5};
  const sourcemeta::core::JSON value{3};
  document += value;
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 8);
}

TEST(increment_integer_real) {
  sourcemeta::core::JSON document{5};
  const sourcemeta::core::JSON value{3.2};
  document += value;
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 8.2);
}

TEST(increment_real_integer) {
  sourcemeta::core::JSON document{3.2};
  const sourcemeta::core::JSON value{2};
  document += value;
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 5.2);
}

TEST(increment_real_real) {
  sourcemeta::core::JSON document{3.2};
  const sourcemeta::core::JSON value{2.0};
  document += value;
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 5.2);
}

TEST(decrement_integer_integer) {
  sourcemeta::core::JSON document{5};
  const sourcemeta::core::JSON value{3};
  document -= value;
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 2);
}

TEST(decrement_integer_real) {
  sourcemeta::core::JSON document{5};
  const sourcemeta::core::JSON value{3.2};
  document -= value;
  EXPECT_TRUE(document.is_real());
  EXPECT_DOUBLE_EQ(document.to_real(), 1.8);
}

TEST(decrement_real_integer) {
  sourcemeta::core::JSON document{3.2};
  const sourcemeta::core::JSON value{2};
  document -= value;
  EXPECT_TRUE(document.is_real());
  EXPECT_DOUBLE_EQ(document.to_real(), 1.2);
}

TEST(decrement_real_real) {
  sourcemeta::core::JSON document{3.2};
  const sourcemeta::core::JSON value{2.0};
  document -= value;
  EXPECT_TRUE(document.is_real());
  EXPECT_DOUBLE_EQ(document.to_real(), 1.2);
}

TEST(add_integer_integer_within_object) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\": 5}");
  const sourcemeta::core::JSON value{3};
  document.at("foo") += value;
  EXPECT_TRUE(document.at("foo").is_integer());
  EXPECT_EQ(document.at("foo").to_integer(), 8);
}

TEST(add_integer_real_within_object) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\": 5}");
  const sourcemeta::core::JSON value{3.2};
  document.at("foo") += value;
  EXPECT_TRUE(document.at("foo").is_real());
  EXPECT_EQ(document.at("foo").to_real(), 8.2);
}

TEST(add_real_integer_within_object) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\": 3.5}");
  const sourcemeta::core::JSON value{2};
  document.at("foo") += value;
  EXPECT_TRUE(document.at("foo").is_real());
  EXPECT_EQ(document.at("foo").to_real(), 5.5);
}

TEST(add_real_real_within_object) {
  sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\": 3.5}");
  const sourcemeta::core::JSON value{2.0};
  document.at("foo") += value;
  EXPECT_TRUE(document.at("foo").is_real());
  EXPECT_EQ(document.at("foo").to_real(), 5.5);
}

TEST(divisible_by_integer_integer_true) {
  const sourcemeta::core::JSON dividend{10};
  const sourcemeta::core::JSON divisor{5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_integer_false) {
  const sourcemeta::core::JSON dividend{11};
  const sourcemeta::core::JSON divisor{5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_real_true) {
  const sourcemeta::core::JSON dividend{6};
  const sourcemeta::core::JSON divisor{1.5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_real_false) {
  const sourcemeta::core::JSON dividend{7};
  const sourcemeta::core::JSON divisor{1.5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_integer_true) {
  const sourcemeta::core::JSON dividend{6.0};
  EXPECT_TRUE(dividend.is_real());
  const sourcemeta::core::JSON divisor{2};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_integer_false) {
  const sourcemeta::core::JSON dividend{6.0};
  EXPECT_TRUE(dividend.is_real());
  const sourcemeta::core::JSON divisor{2.5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_real_true) {
  const sourcemeta::core::JSON dividend{4.5};
  EXPECT_TRUE(dividend.is_real());
  const sourcemeta::core::JSON divisor{1.5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_real_false) {
  const sourcemeta::core::JSON dividend{4.7};
  EXPECT_TRUE(dividend.is_real());
  const sourcemeta::core::JSON divisor{1.5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_infinity) {
  const sourcemeta::core::JSON dividend{1e308};
  const sourcemeta::core::JSON divisor{0.123456789};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_tiny_true) {
  const sourcemeta::core::JSON dividend{0.0075};
  const sourcemeta::core::JSON divisor{0.0001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_tiny_false) {
  const sourcemeta::core::JSON dividend{0.00751};
  const sourcemeta::core::JSON divisor{0.0001};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_zero) {
  const sourcemeta::core::JSON dividend{0.00751};
  const sourcemeta::core::JSON divisor{0};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_zero) {
  const sourcemeta::core::JSON dividend{5};
  const sourcemeta::core::JSON divisor{0};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_zero_zero) {
  const sourcemeta::core::JSON dividend{0};
  const sourcemeta::core::JSON divisor{0};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_zero_interger) {
  const sourcemeta::core::JSON dividend{0};
  const sourcemeta::core::JSON divisor{5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_zero_real_zero_real) {
  const sourcemeta::core::JSON dividend{0.0};
  const sourcemeta::core::JSON divisor{0.0};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_float_overflow_integer_0_5) {
  const sourcemeta::core::JSON document{1e308};
  const sourcemeta::core::JSON divisor{0.5};
  EXPECT_TRUE(document.divisible_by(divisor));
}

TEST(divisible_by_float_overflow_integer_1) {
  const sourcemeta::core::JSON document{1e308};
  const sourcemeta::core::JSON divisor{1};
  EXPECT_TRUE(document.divisible_by(divisor));
}

TEST(divisible_by_float_overflow_integer_1_0) {
  const sourcemeta::core::JSON document{1e308};
  const sourcemeta::core::JSON divisor{1.0};
  EXPECT_TRUE(document.divisible_by(divisor));
}

TEST(divisible_by_real_0_1_true_1) {
  const sourcemeta::core::JSON dividend{3.5};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_1_true_2) {
  const sourcemeta::core::JSON dividend{10.0};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_1_true_3) {
  const sourcemeta::core::JSON dividend{100.3};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_1_true_4) {
  const sourcemeta::core::JSON dividend{0.5};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_1_false) {
  const sourcemeta::core::JSON dividend{3.55};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_01_true_1) {
  const sourcemeta::core::JSON dividend{1280.32};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_01_true_2) {
  const sourcemeta::core::JSON dividend{99.99};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_01_true_3) {
  const sourcemeta::core::JSON dividend{0.01};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_01_true_4) {
  const sourcemeta::core::JSON dividend{0.5};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_01_true_5) {
  const sourcemeta::core::JSON dividend{1.0};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_01_false) {
  const sourcemeta::core::JSON dividend{1.005};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_001_true_1) {
  const sourcemeta::core::JSON dividend{1.001};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_001_true_2) {
  const sourcemeta::core::JSON dividend{0.5};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_001_true_3) {
  const sourcemeta::core::JSON dividend{25.123};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_001_false) {
  const sourcemeta::core::JSON dividend{1.0005};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_0001_true_1) {
  const sourcemeta::core::JSON dividend{1.0001};
  const sourcemeta::core::JSON divisor{0.0001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_0001_true_2) {
  const sourcemeta::core::JSON dividend{99.9999};
  const sourcemeta::core::JSON divisor{0.0001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_0001_false) {
  const sourcemeta::core::JSON dividend{1.00005};
  const sourcemeta::core::JSON divisor{0.0001};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_real_0_1_true) {
  const sourcemeta::core::JSON dividend{10};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_real_0_1_true_2) {
  const sourcemeta::core::JSON dividend{1};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_real_0_01_true) {
  const sourcemeta::core::JSON dividend{1};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_real_0_01_true_2) {
  const sourcemeta::core::JSON dividend{100};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_real_0_001_true) {
  const sourcemeta::core::JSON dividend{1};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_real_0_0001_true) {
  const sourcemeta::core::JSON dividend{1};
  const sourcemeta::core::JSON divisor{0.0001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_3_true) {
  const sourcemeta::core::JSON dividend{0.9};
  const sourcemeta::core::JSON divisor{0.3};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_3_false) {
  const sourcemeta::core::JSON dividend{1.0};
  const sourcemeta::core::JSON divisor{0.3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_7_true) {
  const sourcemeta::core::JSON dividend{2.1};
  const sourcemeta::core::JSON divisor{0.7};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_0_7_false) {
  const sourcemeta::core::JSON dividend{2.0};
  const sourcemeta::core::JSON divisor{0.7};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_real_0_01_true) {
  const sourcemeta::core::JSON dividend{-1280.32};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_real_0_1_true) {
  const sourcemeta::core::JSON dividend{-5.5};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_real_0_001_true) {
  const sourcemeta::core::JSON dividend{-3.141};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_real_0_01_false) {
  const sourcemeta::core::JSON dividend{-1.005};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_large_real_0_01_true) {
  const sourcemeta::core::JSON dividend{999999.99};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_large_real_0_001_true) {
  const sourcemeta::core::JSON dividend{12345.678};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_large_real_0_1_true) {
  const sourcemeta::core::JSON dividend{99999.9};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_integer_real_0_1_true) {
  const sourcemeta::core::JSON dividend{-10};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_integer_real_0_01_true) {
  const sourcemeta::core::JSON dividend{-5};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(as_real_real) {
  const sourcemeta::core::JSON document{4.7};
  EXPECT_DOUBLE_EQ(document.as_real(), 4.7);
}

TEST(as_real_integer) {
  const sourcemeta::core::JSON document{4};
  EXPECT_DOUBLE_EQ(document.as_real(), 4.0);
}

TEST(as_real_decimal) {
  const auto document{sourcemeta::core::parse_json("1e9")};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_DOUBLE_EQ(document.as_real(), 1e9);
}

TEST(as_real_decimal_out_of_range_throws) {
  const auto document{sourcemeta::core::parse_json("1e400")};
  EXPECT_TRUE(document.is_decimal());
  try {
    [[maybe_unused]] const auto value = document.as_real();
    FAIL();
  } catch (const std::out_of_range &error) {
    EXPECT_STREQ(error.what(), "Numeric value is out of range");
  }
}

TEST(compare_int_real_equal) {
  const sourcemeta::core::JSON left{300};
  const sourcemeta::core::JSON right{300.0};
  EXPECT_TRUE(left.is_integer());
  EXPECT_TRUE(right.is_real());
  EXPECT_TRUE(left <= right);
  EXPECT_TRUE(right <= left);
  EXPECT_TRUE(left >= right);
  EXPECT_TRUE(right >= left);
}

TEST(big_real) {
  const sourcemeta::core::JSON document{1e308};
  EXPECT_TRUE(document.is_number());
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 1e308);
}

TEST(is_integral_integer) {
  const sourcemeta::core::JSON document{5};
  EXPECT_TRUE(document.is_integral());
}

TEST(is_integral_non_integer_real) {
  const sourcemeta::core::JSON document{5.3};
  EXPECT_FALSE(document.is_integral());
}

TEST(is_integral_integer_real) {
  const sourcemeta::core::JSON document{5.0};
  EXPECT_TRUE(document.is_integral());
}

TEST(is_integral_non_number) {
  const sourcemeta::core::JSON document{true};
  EXPECT_FALSE(document.is_integral());
}

TEST(as_integer_integer) {
  const sourcemeta::core::JSON document{5};
  EXPECT_EQ(document.as_integer(), 5);
}

TEST(as_integer_integer_real) {
  const sourcemeta::core::JSON document{5.0};
  EXPECT_EQ(document.as_integer(), 5);
}

TEST(as_integer_non_integer_real) {
  const sourcemeta::core::JSON document{5.5};
  EXPECT_EQ(document.as_integer(), 5);
}

TEST(as_integer_decimal) {
  const auto document{sourcemeta::core::parse_json("1e9")};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.as_integer(), 1000000000);
}

TEST(as_integer_decimal_fractional) {
  const auto document{sourcemeta::core::parse_json("1700000000.000000001")};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.as_integer(), 1700000000);
}

TEST(as_integer_decimal_out_of_range_throws) {
  const auto document{sourcemeta::core::parse_json("10000000000000000000")};
  EXPECT_TRUE(document.is_decimal());
  try {
    [[maybe_unused]] const auto value = document.as_integer();
    FAIL();
  } catch (const std::out_of_range &error) {
    EXPECT_STREQ(error.what(),
                 "The decimal number does not fit in a 64-bit integer");
  }
}

TEST(as_integer_real_out_of_range_throws) {
  const sourcemeta::core::JSON document{1e300};
  EXPECT_TRUE(document.is_real());
  try {
    [[maybe_unused]] const auto value = document.as_integer();
    FAIL();
  } catch (const std::out_of_range &error) {
    EXPECT_STREQ(error.what(),
                 "The real number does not fit in a 64-bit integer");
  }
}

TEST(is_integer_int_storage) {
  const sourcemeta::core::JSON document{5};
  EXPECT_TRUE(document.is_integer());
}

TEST(is_integer_real_storage_integer_valued) {
  const sourcemeta::core::JSON document{5.0};
  EXPECT_FALSE(document.is_integer());
}

TEST(is_integer_real_storage_fractional) {
  const sourcemeta::core::JSON document{3.14};
  EXPECT_FALSE(document.is_integer());
}

TEST(equal_integer_same_value) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{5};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_different_value) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{6};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_zero) {
  const sourcemeta::core::JSON left{0};
  const sourcemeta::core::JSON right{0};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_negative_same_value) {
  const sourcemeta::core::JSON left{-5};
  const sourcemeta::core::JSON right{-5};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_sign_differs) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{-5};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_maximum) {
  const sourcemeta::core::JSON left{std::numeric_limits<std::int64_t>::max()};
  const sourcemeta::core::JSON right{std::numeric_limits<std::int64_t>::max()};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_minimum) {
  const sourcemeta::core::JSON left{std::numeric_limits<std::int64_t>::min()};
  const sourcemeta::core::JSON right{std::numeric_limits<std::int64_t>::min()};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_minimum_and_maximum) {
  const sourcemeta::core::JSON left{std::numeric_limits<std::int64_t>::min()};
  const sourcemeta::core::JSON right{std::numeric_limits<std::int64_t>::max()};
  EXPECT_FALSE(left == right);
}

TEST(equal_real_same_value) {
  const sourcemeta::core::JSON left{5.5};
  const sourcemeta::core::JSON right{5.5};
  EXPECT_TRUE(left == right);
}

TEST(equal_real_different_value) {
  const sourcemeta::core::JSON left{5.5};
  const sourcemeta::core::JSON right{5.6};
  EXPECT_FALSE(left == right);
}

TEST(equal_real_positive_and_negative_zero) {
  const sourcemeta::core::JSON left{0.0};
  const sourcemeta::core::JSON right{-0.0};
  EXPECT_TRUE(left == right);
}

TEST(equal_decimal_same_value) {
  const sourcemeta::core::JSON left{sourcemeta::core::Decimal{"3.14"}};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"3.14"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_decimal_trailing_zeros) {
  const sourcemeta::core::JSON left{sourcemeta::core::Decimal{"1.0"}};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"1.00"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_decimal_positive_and_negative_zero) {
  const sourcemeta::core::JSON left{sourcemeta::core::Decimal{"0"}};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"-0"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_decimal_different_value) {
  const sourcemeta::core::JSON left{sourcemeta::core::Decimal{"3.14"}};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"3.15"}};
  EXPECT_FALSE(left == right);
}

TEST(equal_decimal_big_same_value) {
  const sourcemeta::core::JSON left{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}};
  const sourcemeta::core::JSON right{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_decimal_big_different_value) {
  const sourcemeta::core::JSON left{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}};
  const sourcemeta::core::JSON right{
      sourcemeta::core::Decimal{"123456789012345678901234567891"}};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_and_real_same_value) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{1.0};
  EXPECT_TRUE(left == right);
  EXPECT_TRUE(right == left);
}

TEST(equal_integer_and_real_thousand) {
  const sourcemeta::core::JSON left{1000};
  const sourcemeta::core::JSON right{1000.0};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_real_zero) {
  const sourcemeta::core::JSON left{0};
  const sourcemeta::core::JSON right{0.0};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_real_negative_zero) {
  const sourcemeta::core::JSON left{0};
  const sourcemeta::core::JSON right{-0.0};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_real_negative) {
  const sourcemeta::core::JSON left{-5};
  const sourcemeta::core::JSON right{-5.0};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_real_fractional_differs) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{1.5};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_and_real_close_but_distinct) {
  const sourcemeta::core::JSON left{1000};
  const sourcemeta::core::JSON right{1000.0001};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_and_real_large_exact_power_of_two) {
  const sourcemeta::core::JSON left{
      static_cast<std::int64_t>(4503599627370496)};
  const sourcemeta::core::JSON right{4503599627370496.0};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_maximum_and_real_two_pow_63) {
  const sourcemeta::core::JSON left{std::numeric_limits<std::int64_t>::max()};
  const sourcemeta::core::JSON right{9223372036854775808.0};
  EXPECT_FALSE(left == right);
  EXPECT_FALSE(right == left);
}

TEST(equal_integer_beyond_double_precision_and_real) {
  const sourcemeta::core::JSON left{
      static_cast<std::int64_t>(9007199254740993)};
  const sourcemeta::core::JSON right{9007199254740993.0};
  EXPECT_FALSE(left == right);
  EXPECT_FALSE(right == left);
}

TEST(equal_integer_minimum_and_real_negative_two_pow_63) {
  const sourcemeta::core::JSON left{std::numeric_limits<std::int64_t>::min()};
  const sourcemeta::core::JSON right{-9223372036854775808.0};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_decimal_same_value) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"1"}};
  EXPECT_TRUE(left == right);
  EXPECT_TRUE(right == left);
}

TEST(equal_integer_and_decimal_trailing_zeros) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"1.00"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_decimal_exponent) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"1e0"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_decimal_negative) {
  const sourcemeta::core::JSON left{-5};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"-5"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_decimal_zero) {
  const sourcemeta::core::JSON left{0};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"0"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_decimal_negative_zero) {
  const sourcemeta::core::JSON left{0};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"-0"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_integer_and_decimal_fractional_differs) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_and_decimal_big_differs) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_maximum_and_decimal_same_value) {
  const sourcemeta::core::JSON left{std::numeric_limits<std::int64_t>::max()};
  const sourcemeta::core::JSON right{
      sourcemeta::core::Decimal{"9223372036854775807"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_real_and_decimal_integral) {
  const sourcemeta::core::JSON left{5.0};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"5"}};
  EXPECT_TRUE(left == right);
  EXPECT_TRUE(right == left);
}

TEST(equal_real_and_decimal_dyadic_half) {
  const sourcemeta::core::JSON left{0.5};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"0.5"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_real_and_decimal_dyadic_quarter) {
  const sourcemeta::core::JSON left{0.25};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"0.25"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_real_and_decimal_dyadic_trailing_zeros) {
  const sourcemeta::core::JSON left{2.5};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"2.50"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_real_and_decimal_negative) {
  const sourcemeta::core::JSON left{-2.5};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"-2.5"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_real_two_pow_63_and_decimal) {
  const sourcemeta::core::JSON left{9223372036854775808.0};
  const sourcemeta::core::JSON right{
      sourcemeta::core::Decimal{"9223372036854775808"}};
  EXPECT_TRUE(left == right);
}

TEST(equal_real_and_decimal_non_dyadic_are_distinct) {
  // A double cannot store 2.1 exactly, so the real holds an approximation while
  // the decimal is exact. They are genuinely different stored values
  const sourcemeta::core::JSON left{2.1};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"2.1"}};
  EXPECT_FALSE(left == right);
}

TEST(equal_real_and_decimal_one_tenth_are_distinct) {
  const sourcemeta::core::JSON left{0.1};
  const sourcemeta::core::JSON right{sourcemeta::core::Decimal{"0.1"}};
  EXPECT_FALSE(left == right);
}

TEST(equal_real_and_decimal_big_differs) {
  const sourcemeta::core::JSON left{1.0};
  const sourcemeta::core::JSON right{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}};
  EXPECT_FALSE(left == right);
}

TEST(equal_transitive_integer_real_decimal) {
  const sourcemeta::core::JSON as_integer{1000};
  const sourcemeta::core::JSON as_real{1000.0};
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"1000"}};
  EXPECT_TRUE(as_integer == as_real);
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_TRUE(as_integer == as_decimal);
}

TEST(equal_transitivity_holds_at_two_pow_63_boundary) {
  const sourcemeta::core::JSON as_integer{
      std::numeric_limits<std::int64_t>::max()};
  const sourcemeta::core::JSON as_real{9223372036854775808.0};
  const sourcemeta::core::JSON as_decimal{
      sourcemeta::core::Decimal{"9223372036854775808"}};
  EXPECT_FALSE(as_integer == as_real);
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_FALSE(as_integer == as_decimal);
}

TEST(equal_integer_and_string_differ) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{"1"};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_one_and_boolean_true_differ) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{true};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_zero_and_boolean_false_differ) {
  const sourcemeta::core::JSON left{0};
  const sourcemeta::core::JSON right{false};
  EXPECT_FALSE(left == right);
}

TEST(equal_real_and_null_differ) {
  const sourcemeta::core::JSON left{1.0};
  const sourcemeta::core::JSON right{nullptr};
  EXPECT_FALSE(left == right);
}

TEST(equal_decimal_and_string_differ) {
  const sourcemeta::core::JSON left{sourcemeta::core::Decimal{"1"}};
  const sourcemeta::core::JSON right{"1"};
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_and_array_differ) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[ 1 ]");
  EXPECT_FALSE(left == right);
}

TEST(equal_integer_and_object_differ) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("{ \"a\": 1 }");
  EXPECT_FALSE(left == right);
}

TEST(equal_array_with_integer_and_real_elements) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[ 1 ]");
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[ 1.0 ]");
  EXPECT_TRUE(left == right);
}

TEST(equal_array_with_real_and_decimal_precision_differ) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[ 2.1 ]");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("[ 2.1e0 ]");
  EXPECT_FALSE(left == right);
}

TEST(equal_object_with_integer_and_real_values) {
  const sourcemeta::core::JSON left =
      sourcemeta::core::parse_json("{ \"a\": 1 }");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("{ \"a\": 1.0 }");
  EXPECT_TRUE(left == right);
}

TEST(equal_nested_array_with_integer_and_real) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[ [ 1 ] ]");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("[ [ 1.0 ] ]");
  EXPECT_TRUE(left == right);
}

TEST(not_equal_operator_integer_and_real_distinct) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{1.5};
  EXPECT_TRUE(left != right);
}

TEST(not_equal_operator_integer_and_real_equal) {
  const sourcemeta::core::JSON left{1};
  const sourcemeta::core::JSON right{1.0};
  EXPECT_FALSE(left != right);
}

TEST(not_equal_operator_integer_maximum_and_real_two_pow_63) {
  const sourcemeta::core::JSON left{std::numeric_limits<std::int64_t>::max()};
  const sourcemeta::core::JSON right{9223372036854775808.0};
  EXPECT_TRUE(left != right);
}

TEST(less_integer_below_fractional_real) {
  const sourcemeta::core::JSON integer{2};
  const sourcemeta::core::JSON real{2.5};
  EXPECT_TRUE(integer < real);
  EXPECT_FALSE(real < integer);
}

TEST(less_integer_above_fractional_real) {
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON real{2.5};
  EXPECT_FALSE(integer < real);
  EXPECT_TRUE(real < integer);
}

TEST(less_integer_and_integral_real_are_neither) {
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON real{3.0};
  EXPECT_FALSE(integer < real);
  EXPECT_FALSE(real < integer);
  EXPECT_TRUE(integer == real);
}

TEST(less_negative_integer_below_fractional_real) {
  const sourcemeta::core::JSON integer{-3};
  const sourcemeta::core::JSON real{-2.5};
  EXPECT_TRUE(integer < real);
  EXPECT_FALSE(real < integer);
}

TEST(less_negative_integer_above_fractional_real) {
  const sourcemeta::core::JSON integer{-2};
  const sourcemeta::core::JSON real{-2.5};
  EXPECT_FALSE(integer < real);
  EXPECT_TRUE(real < integer);
}

TEST(less_integer_maximum_below_two_pow_63) {
  const sourcemeta::core::JSON integer{
      std::numeric_limits<std::int64_t>::max()};
  const sourcemeta::core::JSON real{9223372036854775808.0};
  EXPECT_TRUE(integer < real);
  EXPECT_FALSE(real < integer);
}

TEST(greater_integer_minimum_above_below_range_real) {
  const sourcemeta::core::JSON integer{
      std::numeric_limits<std::int64_t>::min()};
  const sourcemeta::core::JSON real{-1e19};
  EXPECT_FALSE(integer < real);
  EXPECT_TRUE(real < integer);
}

TEST(less_integer_beyond_double_precision_is_greater_than_real) {
  const sourcemeta::core::JSON integer{
      static_cast<std::int64_t>(9007199254740993)};
  const sourcemeta::core::JSON real{9007199254740992.0};
  EXPECT_FALSE(integer < real);
  EXPECT_TRUE(real < integer);
}

TEST(less_real_below_integral_decimal) {
  const sourcemeta::core::JSON real{2.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3"}};
  EXPECT_TRUE(real < decimal);
  EXPECT_FALSE(decimal < real);
}

TEST(less_integral_decimal_below_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3"}};
  const sourcemeta::core::JSON real{3.5};
  EXPECT_TRUE(decimal < real);
  EXPECT_FALSE(real < decimal);
}

TEST(less_real_below_fractional_decimal) {
  const sourcemeta::core::JSON real{2.0};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"2.5"}};
  EXPECT_TRUE(real < decimal);
  EXPECT_FALSE(decimal < real);
}

TEST(ordering_at_two_pow_63_boundary_is_trichotomous) {
  const sourcemeta::core::JSON integer{
      std::numeric_limits<std::int64_t>::max()};
  const sourcemeta::core::JSON real{9223372036854775808.0};
  EXPECT_FALSE(integer == real);
  EXPECT_TRUE(integer < real);
  EXPECT_FALSE(integer > real);
  EXPECT_FALSE(real < integer);
  EXPECT_TRUE(real > integer);
}

TEST(less_equal_and_greater_equal_cross_type_equal) {
  const sourcemeta::core::JSON integer{2};
  const sourcemeta::core::JSON real{2.0};
  EXPECT_TRUE(integer <= real);
  EXPECT_TRUE(integer >= real);
  EXPECT_TRUE(real <= integer);
  EXPECT_TRUE(real >= integer);
}

TEST(less_equal_and_greater_equal_cross_type_distinct) {
  const sourcemeta::core::JSON integer{2};
  const sourcemeta::core::JSON real{2.5};
  EXPECT_TRUE(integer <= real);
  EXPECT_FALSE(integer >= real);
  EXPECT_FALSE(real <= integer);
  EXPECT_TRUE(real >= integer);
}
