#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JSON_decimal, positive) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 42);
}

TEST(JSON_decimal, type) {
  const sourcemeta::core::Decimal value{5};
  const sourcemeta::core::JSON document{value};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Decimal);
}

TEST(JSON_decimal, negative) {
  const sourcemeta::core::Decimal value{-10};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), -10);
}

TEST(JSON_decimal, zero) {
  const sourcemeta::core::Decimal value{0};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 0);
}

TEST(JSON_decimal, copy_constructor_from_json) {
  const sourcemeta::core::Decimal value{123};
  const sourcemeta::core::JSON decimal{value};
  const sourcemeta::core::JSON document{decimal};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 123);
}

TEST(JSON_decimal, copy_constructor_from_decimal) {
  const sourcemeta::core::Decimal value{789};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 789);
}

TEST(JSON_decimal, move_constructor) {
  sourcemeta::core::Decimal value{456};
  const sourcemeta::core::JSON document{std::move(value)};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 456);
}

TEST(JSON_decimal, copy_assignment) {
  const sourcemeta::core::Decimal value{999};
  const sourcemeta::core::JSON source{value};
  sourcemeta::core::JSON document{5};
  document = source;
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 999);
}

TEST(JSON_decimal, move_assignment) {
  sourcemeta::core::Decimal value{777};
  sourcemeta::core::JSON source{value};
  sourcemeta::core::JSON document{10};
  document = std::move(source);
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 777);
}

TEST(JSON_decimal, large_positive) {
  const sourcemeta::core::Decimal value{"123456789012345678901234567890"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(),
            "123456789012345678901234567890");
}

TEST(JSON_decimal, large_negative) {
  const sourcemeta::core::Decimal value{"-987654321098765432109876543210"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(),
            "-987654321098765432109876543210");
}

TEST(JSON_decimal, fractional) {
  const sourcemeta::core::Decimal value{"3.14159265358979323846"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "3.14159265358979323846");
}

TEST(JSON_decimal, negative_fractional) {
  const sourcemeta::core::Decimal value{"-2.71828182845904523536"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "-2.71828182845904523536");
}

TEST(JSON_decimal, is_not_integer) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_integer());
}

TEST(JSON_decimal, is_not_real) {
  const sourcemeta::core::Decimal value{"3.14"};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_real());
}

TEST(JSON_decimal, is_not_string) {
  const sourcemeta::core::Decimal value{100};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_string());
}

TEST(JSON_decimal, is_not_boolean) {
  const sourcemeta::core::Decimal value{1};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_boolean());
}

TEST(JSON_decimal, is_not_null) {
  const sourcemeta::core::Decimal value{0};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_null());
}

TEST(JSON_decimal, is_not_array) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_array());
}

TEST(JSON_decimal, is_not_object) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::JSON document{value};
  EXPECT_FALSE(document.is_object());
}

TEST(JSON_decimal, to_decimal_returns_reference) {
  const sourcemeta::core::Decimal value{555};
  const sourcemeta::core::JSON document{value};
  const sourcemeta::core::Decimal &reference{document.to_decimal()};
  EXPECT_EQ(reference.to_int64(), 555);
  EXPECT_EQ(&reference, &document.to_decimal());
}

TEST(JSON_decimal, copy_preserves_precision) {
  const sourcemeta::core::Decimal value{
      "12345678901234567890.123456789012345678901234567890"};
  const sourcemeta::core::JSON original{value};
  const sourcemeta::core::JSON copy{original};
  EXPECT_EQ(copy.to_decimal().to_string(), original.to_decimal().to_string());
}

TEST(JSON_decimal, assignment_from_other_type) {
  const sourcemeta::core::Decimal value{888};
  const sourcemeta::core::JSON decimal{value};
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document = decimal;
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_int64(), 888);
}

TEST(JSON_decimal, assignment_to_other_type) {
  const sourcemeta::core::Decimal value{333};
  sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  document = sourcemeta::core::JSON{"hello"};
  EXPECT_TRUE(document.is_string());
  EXPECT_FALSE(document.is_decimal());
}

TEST(JSON_decimal, very_small_fractional) {
  const sourcemeta::core::Decimal value{"0.000000000000000000000000000001"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "1e-30");
}

TEST(JSON_decimal, scientific_notation) {
  const sourcemeta::core::Decimal value{"1.23e10"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "12.3e+9");
}

TEST(JSON_decimal, negative_scientific_notation) {
  const sourcemeta::core::Decimal value{"-4.56e-5"};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_string(), "-0.0000456");
}

TEST(JSON_decimal, unsigned_integer_construction) {
  const sourcemeta::core::Decimal value{
      static_cast<std::uint64_t>(18446744073709551615ULL)};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_decimal());
  EXPECT_EQ(document.to_decimal().to_uint64(), 18446744073709550000ULL);
}

TEST(JSON_decimal, multiple_copies) {
  const sourcemeta::core::Decimal value{111};
  const sourcemeta::core::JSON first{value};
  const sourcemeta::core::JSON second{first};
  const sourcemeta::core::JSON third{second};
  EXPECT_TRUE(third.is_decimal());
  EXPECT_EQ(third.to_decimal().to_int64(), 111);
}

TEST(JSON_decimal, negative_in_object) {
  const sourcemeta::core::Decimal value{"-67.89"};
  const sourcemeta::core::JSON document{
      {"test", sourcemeta::core::JSON{value}}};
  EXPECT_TRUE(document.at("test").is_decimal());
  EXPECT_EQ(document.at("test").to_decimal().to_string(), "-67.89");
}

TEST(JSON_decimal, negative_copy_into_array) {
  const sourcemeta::core::Decimal value{"-67.89"};
  const sourcemeta::core::JSON decimal_json{value};
  sourcemeta::core::JSON document{sourcemeta::core::JSON::make_array()};
  document.push_back(decimal_json);
  EXPECT_TRUE(document.at(0).is_decimal());
  EXPECT_EQ(document.at(0).to_decimal().to_string(), "-67.89");
}

TEST(JSON_decimal, less_than_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(value1 < value2);
  EXPECT_FALSE(value2 < value1);
  EXPECT_FALSE(value1 < value1);
}

TEST(JSON_decimal, less_than_or_equal_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value3{sourcemeta::core::Decimal{"3.2"}};
  EXPECT_TRUE(value1 <= value2);
  EXPECT_FALSE(value2 <= value1);
  EXPECT_TRUE(value1 <= value3);
}

TEST(JSON_decimal, greater_than_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"3.2"}};
  EXPECT_TRUE(value1 > value2);
  EXPECT_FALSE(value2 > value1);
  EXPECT_FALSE(value1 > value1);
}

TEST(JSON_decimal, greater_than_or_equal_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value3{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(value1 >= value2);
  EXPECT_FALSE(value2 >= value1);
  EXPECT_TRUE(value1 >= value3);
}

TEST(JSON_decimal, less_than_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON integer{5};
  EXPECT_TRUE(decimal < integer);
  EXPECT_FALSE(integer < decimal);
}

TEST(JSON_decimal, less_than_or_equal_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"5"}};
  EXPECT_TRUE(decimal <= integer);
  EXPECT_FALSE(integer <= decimal);
  EXPECT_TRUE(equal <= integer);
}

TEST(JSON_decimal, greater_than_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON integer{3};
  EXPECT_TRUE(decimal > integer);
  EXPECT_FALSE(integer > decimal);
}

TEST(JSON_decimal, greater_than_or_equal_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"3"}};
  EXPECT_TRUE(decimal >= integer);
  EXPECT_FALSE(integer >= decimal);
  EXPECT_TRUE(equal >= integer);
}

TEST(JSON_decimal, less_than_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON real{5.5};
  EXPECT_TRUE(decimal < real);
  EXPECT_FALSE(real < decimal);
}

TEST(JSON_decimal, less_than_or_equal_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"5.5"}};
  EXPECT_TRUE(decimal <= real);
  EXPECT_FALSE(real <= decimal);
  EXPECT_TRUE(equal <= real);
}

TEST(JSON_decimal, greater_than_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON real{3.5};
  EXPECT_TRUE(decimal > real);
  EXPECT_FALSE(real > decimal);
}

TEST(JSON_decimal, greater_than_or_equal_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"3.5"}};
  EXPECT_TRUE(decimal >= real);
  EXPECT_FALSE(real >= decimal);
  EXPECT_TRUE(equal >= real);
}

TEST(JSON_decimal, addition_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON result{value1 + value2};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "8.9");
}

TEST(JSON_decimal, subtraction_decimal_decimal) {
  const sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON result{value1 - value2};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "2.5");
}

TEST(JSON_decimal, addition_assignment_decimal_decimal) {
  sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"5.7"}};
  value1 += value2;
  EXPECT_TRUE(value1.is_decimal());
  EXPECT_EQ(value1.to_decimal().to_string(), "8.9");
}

TEST(JSON_decimal, subtraction_assignment_decimal_decimal) {
  sourcemeta::core::JSON value1{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON value2{sourcemeta::core::Decimal{"3.2"}};
  value1 -= value2;
  EXPECT_TRUE(value1.is_decimal());
  EXPECT_EQ(value1.to_decimal().to_string(), "2.5");
}

TEST(JSON_decimal, addition_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON result{decimal + integer};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "8.2");
}

TEST(JSON_decimal, subtraction_decimal_integer) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON result{decimal - integer};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "2.7");
}

TEST(JSON_decimal, addition_assignment_decimal_integer) {
  sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON integer{5};
  decimal += integer;
  EXPECT_TRUE(decimal.is_decimal());
  EXPECT_EQ(decimal.to_decimal().to_string(), "8.2");
}

TEST(JSON_decimal, subtraction_assignment_decimal_integer) {
  sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON integer{3};
  decimal -= integer;
  EXPECT_TRUE(decimal.is_decimal());
  EXPECT_EQ(decimal.to_decimal().to_string(), "2.7");
}

TEST(JSON_decimal, addition_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON result{decimal + real};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "8.7");
}

TEST(JSON_decimal, subtraction_decimal_real) {
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON result{decimal - real};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "2.2");
}

TEST(JSON_decimal, addition_assignment_decimal_real) {
  sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON real{5.5};
  decimal += real;
  EXPECT_TRUE(decimal.is_decimal());
  EXPECT_EQ(decimal.to_decimal().to_string(), "8.7");
}

TEST(JSON_decimal, subtraction_assignment_decimal_real) {
  sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON real{3.5};
  decimal -= real;
  EXPECT_TRUE(decimal.is_decimal());
  EXPECT_EQ(decimal.to_decimal().to_string(), "2.2");
}

TEST(JSON_decimal, is_number_positive) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(document.is_number());
}

TEST(JSON_decimal, is_number_negative) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"-3.2"}};
  EXPECT_TRUE(document.is_number());
}

TEST(JSON_decimal, is_number_zero) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"0"}};
  EXPECT_TRUE(document.is_number());
}

TEST(JSON_decimal, is_number_very_large) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"123456789012345678901234567890.5"}};
  EXPECT_TRUE(document.is_number());
}

TEST(JSON_decimal, is_positive_positive) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(document.is_positive());
}

TEST(JSON_decimal, is_positive_negative) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"-3.2"}};
  EXPECT_FALSE(document.is_positive());
}

TEST(JSON_decimal, is_positive_zero) {
  const sourcemeta::core::JSON document{sourcemeta::core::Decimal{"0"}};
  EXPECT_TRUE(document.is_positive());
}

TEST(JSON_decimal, is_positive_very_small_positive) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"0.0000000001"}};
  EXPECT_TRUE(document.is_positive());
}

TEST(JSON_decimal, is_positive_very_small_negative) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::Decimal{"-0.0000000001"}};
  EXPECT_FALSE(document.is_positive());
}
