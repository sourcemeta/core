#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

TEST(positive) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 42);
}

TEST(type) {
  const sourcemeta::core::Decimal value{5};
  const sourcemeta::core::JSON document{value};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Decimal);
}

TEST(negative) {
  const sourcemeta::core::Decimal value{-10};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), -10);
}

TEST(zero) {
  const sourcemeta::core::Decimal value{0};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 0);
}

TEST(copy_constructor_from_json) {
  const sourcemeta::core::Decimal value{123};
  const sourcemeta::core::JSON decimal{value};
  const sourcemeta::core::JSON document{decimal};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 123);
}

TEST(copy_constructor_from_decimal) {
  const sourcemeta::core::Decimal value{789};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 789);
}

TEST(move_constructor) {
  sourcemeta::core::Decimal value{456};
  const sourcemeta::core::JSON document{std::move(value)};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 456);
}

TEST(copy_assignment) {
  const sourcemeta::core::Decimal value{999};
  const sourcemeta::core::JSON source{value};
  sourcemeta::core::JSON document{5};
  document = source;
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 999);
}

TEST(move_assignment) {
  sourcemeta::core::Decimal value{777};
  sourcemeta::core::JSON source{value};
  sourcemeta::core::JSON document{10};
  document = std::move(source);
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 777);
}

TEST(large_positive) {
  const sourcemeta::core::Decimal value{"123456789012345678901234567890"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(),
            "123456789012345678901234567890");
}

TEST(large_negative) {
  const sourcemeta::core::Decimal value{"-987654321098765432109876543210"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(),
            "-987654321098765432109876543210");
}

TEST(fractional) {
  const sourcemeta::core::Decimal value{"3.14159265358979323846"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "3.14159265358979323846");
}

TEST(negative_fractional) {
  const sourcemeta::core::Decimal value{"-2.71828182845904523536"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "-2.71828182845904523536");
}

TEST(is_not_integer) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_integer());
}

TEST(is_not_real) {
  const sourcemeta::core::Decimal value{"3.14"};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_real());
}

TEST(is_not_string) {
  const sourcemeta::core::Decimal value{100};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_string());
}

TEST(is_not_boolean) {
  const sourcemeta::core::Decimal value{1};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_boolean());
}

TEST(is_not_null) {
  const sourcemeta::core::Decimal value{0};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_null());
}

TEST(is_not_array) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_array());
}

TEST(is_not_object) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_object());
}

TEST(to_decimal_returns_reference) {
  const sourcemeta::core::Decimal value{555};
  const sourcemeta::core::JSON document{value};
  const sourcemeta::core::Decimal &reference{document.to_decimal()};
  EXPECT_EQ(reference.to_int64(), 555);
  EXPECT_EQ(&reference, &document.to_decimal());
}

TEST(copy_preserves_precision) {
  const sourcemeta::core::Decimal value{
      "12345678901234567890.123456789012345678901234567890"};
  const sourcemeta::core::JSON original{value};
  const sourcemeta::core::JSON copy{original};
  EXPECT_EQ(copy.to_decimal().to_string(), original.to_decimal().to_string());
}

TEST(assignment_from_other_type) {
  const sourcemeta::core::Decimal value{888};
  const sourcemeta::core::JSON decimal{value};
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document = decimal;
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 888);
}

TEST(assignment_to_other_type) {
  const sourcemeta::core::Decimal value{333};
  sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  document = sourcemeta::core::JSON{"hello"};
  EXPECT_TRUE(document.is_string());
  EXPECT_FALSE(document.is_decimal());
}

TEST(very_small_fractional) {
  const sourcemeta::core::Decimal value{"0.000000000000000000000000000001"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "1e-30");
}

TEST(scientific_notation) {
  const sourcemeta::core::Decimal value{"1.23e10"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "12.3e+9");
}

TEST(negative_scientific_notation) {
  const sourcemeta::core::Decimal value{"-4.56e-5"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "-0.0000456");
}

TEST(unsigned_integer_construction) {
  const sourcemeta::core::Decimal value{
      static_cast<std::uint64_t>(18446744073709551615ULL)};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_uint64(), 18446744073709551615ULL);
}

TEST(multiple_copies) {
  const sourcemeta::core::Decimal value{111};
  const sourcemeta::core::JSON first{value};
  const sourcemeta::core::JSON second{first};
  const sourcemeta::core::JSON third{second};
  EXPECT_TRUE(third.is_decimal());
  EXPECT_EQ(third.to_decimal().to_int64(), 111);
}

TEST(negative_in_object) {
  const sourcemeta::core::Decimal value{"-67.89"};
  const sourcemeta::core::JSON document{
      {"test", sourcemeta::core::JSON{value}}};
  EXPECT_TRUE(document.at("test").is_decimal());
  EXPECT_EQ(document.at("test").to_decimal().to_string(), "-67.89");
}

TEST(negative_copy_into_array) {
  const sourcemeta::core::Decimal value{"-67.89"};
  const sourcemeta::core::JSON decimal_json{value};
  sourcemeta::core::JSON document{sourcemeta::core::JSON::make_array()};
  document.push_back(decimal_json);
  EXPECT_TRUE(document.at(0).is_decimal());
  EXPECT_EQ(document.at(0).to_decimal().to_string(), "-67.89");
}

TEST(less_than_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(value1 < value2);
  EXPECT_FALSE(value2 < value1);
  EXPECT_FALSE(value1 < value1);
}

TEST(less_than_or_equal_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value3{sourcemeta::core::Decimal{"3.2"}};
  EXPECT_TRUE(value1 <= value2);
  EXPECT_FALSE(value2 <= value1);
  EXPECT_TRUE(value1 <= value3);
}

TEST(greater_than_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"3.2"}};
  EXPECT_TRUE(value1 > value2);
  EXPECT_FALSE(value2 > value1);
  EXPECT_FALSE(value1 > value1);
}

TEST(greater_than_or_equal_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value3{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(value1 >= value2);
  EXPECT_FALSE(value2 >= value1);
  EXPECT_TRUE(value1 >= value3);
}

TEST(less_than_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON integer{5};
  EXPECT_TRUE(decimal < integer);
  EXPECT_FALSE(integer < decimal);
}

TEST(less_than_or_equal_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"5"}};
  EXPECT_TRUE(decimal <= integer);
  EXPECT_FALSE(integer <= decimal);
  EXPECT_TRUE(equal <= integer);
}

TEST(greater_than_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON integer{3};
  EXPECT_TRUE(decimal > integer);
  EXPECT_FALSE(integer > decimal);
}

TEST(greater_than_or_equal_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"3"}};
  EXPECT_TRUE(decimal >= integer);
  EXPECT_FALSE(integer >= decimal);
  EXPECT_TRUE(equal >= integer);
}

TEST(less_than_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON real{5.5};
  EXPECT_TRUE(decimal < real);
  EXPECT_FALSE(real < decimal);
}

TEST(less_than_integral_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"2.0"}};
  const sourcemeta::core::JSON real{2.5};
  EXPECT_TRUE(decimal < real);
  EXPECT_FALSE(real < decimal);
}

TEST(less_than_or_equal_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"5.5"}};
  EXPECT_TRUE(decimal <= real);
  EXPECT_FALSE(real <= decimal);
  EXPECT_TRUE(equal <= real);
}

TEST(greater_than_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON real{3.5};
  EXPECT_TRUE(decimal > real);
  EXPECT_FALSE(real > decimal);
}

TEST(greater_than_or_equal_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"3.5"}};
  EXPECT_TRUE(decimal >= real);
  EXPECT_FALSE(real >= decimal);
  EXPECT_TRUE(equal >= real);
}

TEST(addition_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON result{value1 + value2};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "8.9");
}

TEST(subtraction_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON result{value1 - value2};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "2.5");
}

TEST(addition_assignment_decimal_decimal) {
  sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"5.7"}};
  value1 += value2;
  EXPECT_TRUE(value1.is_decimal());
  EXPECT_EQ(value1.to_decimal().to_string(), "8.9");
}

TEST(subtraction_assignment_decimal_decimal) {
  sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"3.2"}};
  value1 -= value2;
  EXPECT_TRUE(value1.is_decimal());
  EXPECT_EQ(value1.to_decimal().to_string(), "2.5");
}

TEST(addition_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON result{decimal + integer};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "8.2");
}

TEST(subtraction_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON result{decimal - integer};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "2.7");
}

TEST(addition_assignment_decimal_integer) {
  sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON integer{5};
  decimal += integer;
  EXPECT_TRUE(decimal.is_decimal());
  EXPECT_EQ(decimal.to_decimal().to_string(), "8.2");
}

TEST(subtraction_assignment_decimal_integer) {
  sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON integer{3};
  decimal -= integer;
  EXPECT_TRUE(decimal.is_decimal());
  EXPECT_EQ(decimal.to_decimal().to_string(), "2.7");
}

TEST(addition_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON result{decimal + real};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "8.7");
}

TEST(subtraction_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON result{decimal - real};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "2.2");
}

TEST(addition_assignment_decimal_real) {
  sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON real{5.5};
  decimal += real;
  EXPECT_TRUE(decimal.is_decimal());
  EXPECT_EQ(decimal.to_decimal().to_string(), "8.7");
}

TEST(subtraction_assignment_decimal_real) {
  sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON real{3.5};
  decimal -= real;
  EXPECT_TRUE(decimal.is_decimal());
  EXPECT_EQ(decimal.to_decimal().to_string(), "2.2");
}

TEST(is_number_positive) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_negative) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"-3.2"}};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_zero) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"0"}};
  EXPECT_TRUE(document.is_number());
}

TEST(is_number_very_large) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"123456789012345678901234567890.5"}};
  EXPECT_TRUE(document.is_number());
}

TEST(is_positive_positive) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(document.is_positive());
}

TEST(is_positive_negative) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"-3.2"}};
  EXPECT_FALSE(document.is_positive());
}

TEST(is_positive_zero) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"0"}};
  EXPECT_TRUE(document.is_positive());
}

TEST(is_positive_very_small_positive) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"0.0000000001"}};
  EXPECT_TRUE(document.is_positive());
}

TEST(is_positive_very_small_negative) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"-0.0000000001"}};
  EXPECT_FALSE(document.is_positive());
}

TEST(is_integral_positive_integer) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"1234567890123456789012345678900000"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_TRUE(document.is_integral());
}

TEST(is_integral_zero) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{0}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_TRUE(document.is_integral());
}

TEST(is_integral_negative_integer) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"-1234567890123456789012345678900000"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_TRUE(document.is_integral());
}

TEST(is_integral_positive_real_with_zero_fractional) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"1234567890123456789012345678900000.0"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_TRUE(document.is_integral());
}

TEST(is_integral_negative_real_with_zero_fractional) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"-1234567890123456789012345678900000.0"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_TRUE(document.is_integral());
}

TEST(is_integral_positive_real) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"1234567890123456789012345678900000.1"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_FALSE(document.is_integral());
}

TEST(is_integral_negative_real) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"-1234567890123456789012345678900000.1"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_FALSE(document.is_integral());
}

TEST(is_integral_zero_real) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"0.1"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_FALSE(document.is_integral());
}

TEST(construction_rejects_nan) {
  const sourcemeta::core::Decimal value{sourcemeta::core::Decimal::nan()};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(construction_rejects_positive_infinity) {
  const sourcemeta::core::Decimal value{sourcemeta::core::Decimal::infinity()};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(construction_rejects_negative_infinity) {
  const sourcemeta::core::Decimal value{
      sourcemeta::core::Decimal::negative_infinity()};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(construction_move_rejects_nan) {
  sourcemeta::core::Decimal value{sourcemeta::core::Decimal::nan()};
  try {
    sourcemeta::core::JSON{std::move(value)};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(construction_move_rejects_positive_infinity) {
  sourcemeta::core::Decimal value{sourcemeta::core::Decimal::infinity()};
  try {
    sourcemeta::core::JSON{std::move(value)};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(construction_move_rejects_negative_infinity) {
  sourcemeta::core::Decimal value{
      sourcemeta::core::Decimal::negative_infinity()};
  try {
    sourcemeta::core::JSON{std::move(value)};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(copy_constructor_cannot_create_invalid_json) {
  try {
    const sourcemeta::core::JSON source{sourcemeta::core::Decimal::nan()};
    const sourcemeta::core::JSON copy{source};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(move_constructor_cannot_create_invalid_json) {
  try {
    sourcemeta::core::JSON source{sourcemeta::core::Decimal::infinity()};
    const sourcemeta::core::JSON moved{std::move(source)};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(copy_assignment_cannot_create_invalid_json) {
  try {
    const sourcemeta::core::JSON source{sourcemeta::core::Decimal::nan()};
    sourcemeta::core::JSON target{sourcemeta::core::Decimal{1}};
    target = source;
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(move_assignment_cannot_create_invalid_json) {
  try {
    sourcemeta::core::JSON source{sourcemeta::core::Decimal::infinity()};
    sourcemeta::core::JSON target{sourcemeta::core::Decimal{1}};
    target = std::move(source);
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(addition_operator_cannot_create_invalid_json) {
  try {
    const sourcemeta::core::JSON left{sourcemeta::core::Decimal::infinity()};
    const sourcemeta::core::JSON right{sourcemeta::core::Decimal{1}};
    const auto result{left + right};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(subtraction_operator_cannot_create_invalid_json) {
  try {
    const sourcemeta::core::JSON left{
        sourcemeta::core::Decimal::negative_infinity()};
    const sourcemeta::core::JSON right{sourcemeta::core::Decimal{1}};
    const auto result{left - right};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(addition_assignment_operator_cannot_create_invalid_json) {
  try {
    sourcemeta::core::JSON document{sourcemeta::core::Decimal::infinity()};
    const sourcemeta::core::JSON addend{sourcemeta::core::Decimal{1}};
    document += addend;
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(subtraction_assignment_operator_cannot_create_invalid_json) {
  try {
    sourcemeta::core::JSON document{
        sourcemeta::core::Decimal::negative_infinity()};
    const sourcemeta::core::JSON subtrahend{sourcemeta::core::Decimal{1}};
    document -= subtrahend;
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(divisible_by_decimal_integer_decimal_integer_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{10}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_decimal_integer_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{11}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{5}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_decimal_real_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{6}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_decimal_real_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{7}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_decimal_integer_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"6.0"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{2}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_decimal_integer_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"6.5"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{2}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_decimal_real_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"4.5"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_decimal_real_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"4.7"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_integer_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{10}};
  const sourcemeta::core::JSON divisor{5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_integer_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{11}};
  const sourcemeta::core::JSON divisor{5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_integer_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"6.0"}};
  const sourcemeta::core::JSON divisor{2};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_integer_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"6.5"}};
  const sourcemeta::core::JSON divisor{2};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_real_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{6}};
  const sourcemeta::core::JSON divisor{1.5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_real_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{7}};
  const sourcemeta::core::JSON divisor{1.5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_real_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"4.5"}};
  const sourcemeta::core::JSON divisor{1.5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_real_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"4.7"}};
  const sourcemeta::core::JSON divisor{1.5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_decimal_integer_true) {
  const sourcemeta::core::JSON dividend{10};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_decimal_integer_false) {
  const sourcemeta::core::JSON dividend{11};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{5}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_decimal_real_true) {
  const sourcemeta::core::JSON dividend{6};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_decimal_real_false) {
  const sourcemeta::core::JSON dividend{7};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_integer_true) {
  const sourcemeta::core::JSON dividend{6.0};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{2}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_integer_false) {
  const sourcemeta::core::JSON dividend{6.5};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{2}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_real_true) {
  const sourcemeta::core::JSON dividend{4.5};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_real_false) {
  const sourcemeta::core::JSON dividend{4.7};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_zero) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{10}};
  const sourcemeta::core::JSON divisor{0};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_zero) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"3.5"}};
  const sourcemeta::core::JSON divisor{0};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_zero_decimal_integer) {
  const sourcemeta::core::JSON dividend{0};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_zero_decimal_real) {
  const sourcemeta::core::JSON dividend{0};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_zero_decimal_integer) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{0}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_zero_decimal_real) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{0}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1.5"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_integer_decimal_zero) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{10}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{0}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_zero_decimal_zero) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{0}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{0}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_very_large_decimal_by_real_not_divisible_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1e308"}};
  const sourcemeta::core::JSON divisor{0.123456789};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_very_large_decimal_by_decimal_not_divisible_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1e308"}};
  const sourcemeta::core::JSON divisor{
      sourcemeta::core::Decimal{"0.123456789"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_very_large_decimal_by_decimal_divisible_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1e308"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"1e154"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_very_large_decimal_by_integer_not_divisible_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1e308"}};
  const sourcemeta::core::JSON divisor{3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_very_large_decimal_by_integer_divisible_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1e308"}};
  const sourcemeta::core::JSON divisor{2};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_mixed_scale_decimal_real_not_divisible_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1e100"}};
  const sourcemeta::core::JSON divisor{0.3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_mixed_scale_decimal_real_divisible_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1e100"}};
  const sourcemeta::core::JSON divisor{0.5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_01_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1280.32"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_01_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1280.325"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_1_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"100.3"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.1"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_1_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"100.35"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.1"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_001_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"25.123"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.001"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_001_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"25.1235"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.001"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_0001_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"99.9999"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.0001"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_0001_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"99.99995"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.0001"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_0_01_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1280.32"}};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_0_01_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1280.325"}};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_0_1_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"100.3"}};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_0_1_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"100.35"}};
  const sourcemeta::core::JSON divisor{0.1};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_0_001_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"25.123"}};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_0_001_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"25.1235"}};
  const sourcemeta::core::JSON divisor{0.001};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_0_0001_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"99.9999"}};
  const sourcemeta::core::JSON divisor{0.0001};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_real_0_0001_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"99.99995"}};
  const sourcemeta::core::JSON divisor{0.0001};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_0_01_true) {
  const sourcemeta::core::JSON dividend{1280.32};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_0_01_false) {
  const sourcemeta::core::JSON dividend{1280.325};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_0_1_true) {
  const sourcemeta::core::JSON dividend{100.3};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.1"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_0_1_false) {
  const sourcemeta::core::JSON dividend{100.35};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.1"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_0_001_true) {
  const sourcemeta::core::JSON dividend{25.123};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.001"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_0_001_false) {
  const sourcemeta::core::JSON dividend{25.1235};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.001"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_0_0001_true) {
  const sourcemeta::core::JSON dividend{99.9999};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.0001"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_real_decimal_0_0001_false) {
  const sourcemeta::core::JSON dividend{99.99995};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.0001"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_decimal_0_01_true) {
  const sourcemeta::core::JSON dividend{100};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_decimal_0_1_true) {
  const sourcemeta::core::JSON dividend{10};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.1"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_decimal_0_001_true) {
  const sourcemeta::core::JSON dividend{1};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.001"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_integer_decimal_0_0001_true) {
  const sourcemeta::core::JSON dividend{1};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.0001"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_decimal_decimal_0_01_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"-1280.32"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_decimal_real_0_01_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"-1280.32"}};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_real_decimal_0_01_true) {
  const sourcemeta::core::JSON dividend{-1280.32};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_negative_integer_decimal_0_01_true) {
  const sourcemeta::core::JSON dividend{-100};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_3_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"0.9"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.3"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_3_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"1.0"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.3"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_7_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"2.1"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.7"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_decimal_decimal_0_7_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"2.0"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.7"}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(divisible_by_large_decimal_decimal_0_01_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"999999.99"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.01"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_large_decimal_real_0_01_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"999999.99"}};
  const sourcemeta::core::JSON divisor{0.01};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(divisible_by_large_decimal_decimal_0_001_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{"12345.678"}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{"0.001"}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(fast_hash_positive) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"3.14"}};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(fast_hash_negative) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"-3.14"}};
  EXPECT_EQ(document.fast_hash(), 5);
}

// An integral decimal hashes like the same integer so that equal numeric
// values across representations hash equally
TEST(fast_hash_zero) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{0}};
  EXPECT_EQ(document.fast_hash(), 4);
  EXPECT_EQ(document.fast_hash(),
            sourcemeta::core::JSON{static_cast<std::int64_t>(0)}.fast_hash());
}

TEST(fast_hash_large) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}};
  EXPECT_EQ(document.fast_hash(), 5);
}

// A real and a decimal of the same value compare equal, so they must hash
// equally, whether the shared value is fractional or integral.
TEST(fast_hash_matches_equal_real_fractional) {
  const sourcemeta::core::JSON as_real{0.5};
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"0.5"}};
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_EQ(as_real.fast_hash(), as_decimal.fast_hash());
}

TEST(fast_hash_matches_equal_real_integral) {
  const sourcemeta::core::JSON as_real{5.0};
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"5"}};
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_EQ(as_real.fast_hash(), as_decimal.fast_hash());
}

TEST(fast_hash_matches_equal_real_integral_with_negative_exponent) {
  const sourcemeta::core::JSON as_real{5.0};
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"5.0"}};
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_TRUE(as_decimal == as_real);
  EXPECT_EQ(as_real.fast_hash(), as_decimal.fast_hash());
}

// An integral value stored in a non-normalized form, such as a negative
// exponent that leaves trailing zeros in the coefficient, must still hash like
// the same integer, which relies on normalizing before the integer extraction
TEST(fast_hash_negative_exponent_integral_decimal) {
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"30e-1"}};
  const sourcemeta::core::JSON as_integer{static_cast<std::int64_t>(3)};
  EXPECT_TRUE(as_decimal.is_integral());
  EXPECT_TRUE(as_decimal == as_integer);
  EXPECT_EQ(as_decimal.fast_hash(), as_integer.fast_hash());
}

TEST(lexical_bignum_integer) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{
      "12345678910111213141516171819202122232425262728293031"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_integral());
  EXPECT_TRUE(document.to_decimal().is_integer());
  EXPECT_TRUE(document.to_decimal().is_integral());
}

TEST(exponent_form_integer_valued) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"1e2"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_integral());
  EXPECT_FALSE(document.to_decimal().is_integer());
  EXPECT_TRUE(document.to_decimal().is_integral());
}

TEST(exponent_zero_form) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"5e0"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_integral());
  EXPECT_FALSE(document.to_decimal().is_integer());
  EXPECT_TRUE(document.to_decimal().is_integral());
}

TEST(dot_zero_fraction_high_precision) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"1234567890123456789012.0"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_integral());
  EXPECT_FALSE(document.to_decimal().is_integer());
  EXPECT_TRUE(document.to_decimal().is_integral());
}

TEST(negative_exponent_non_integral) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"1e-2"}};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_FALSE(document.is_integer());
  EXPECT_FALSE(document.is_integral());
  EXPECT_FALSE(document.to_decimal().is_integer());
  EXPECT_FALSE(document.to_decimal().is_integral());
}

// A parse-clamped exponent must not overflow the internal 32-bit exponent
// while stripping trailing zeros during a comparison.
TEST(large_exponent_comparison_no_overflow) {
  const auto value{
      sourcemeta::core::parse_json("10000000000000000e2147483647")};
  EXPECT_TRUE(value == value);
}
