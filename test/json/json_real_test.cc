#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <cmath>
#include <limits>

TEST(JSON_real, positive) {
  const sourcemeta::core::JSON document{10.2};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 10.2);
}

TEST(JSON_real, negative) {
  const sourcemeta::core::JSON document{-10.02};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -10.02);
}

TEST(JSON_real, zero) {
  const sourcemeta::core::JSON document{0.0};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.0);
}

TEST(JSON_real, type) {
  const sourcemeta::core::JSON document{1.23};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Real);
}

TEST(JSON_real, long_double_nan) {
  const double value{static_cast<double>(NAN)};
  EXPECT_THROW(sourcemeta::core::JSON{value}, std::invalid_argument);
}

TEST(JSON_real, double_nan) {
  const double value{static_cast<double>(NAN)};
  EXPECT_THROW(sourcemeta::core::JSON{value}, std::invalid_argument);
}

TEST(JSON_real, float_nan) {
  const float value{NAN};
  EXPECT_THROW(sourcemeta::core::JSON{value}, std::invalid_argument);
}

TEST(JSON_real, long_double_infinity) {
  const double value{std::numeric_limits<double>::infinity()};
  EXPECT_THROW(sourcemeta::core::JSON{value}, std::invalid_argument);
}

TEST(JSON_real, double_infinity) {
  const double value{std::numeric_limits<double>::infinity()};
  EXPECT_THROW(sourcemeta::core::JSON{value}, std::invalid_argument);
}

TEST(JSON_real, float_infinity) {
  const float value{std::numeric_limits<float>::infinity()};
  EXPECT_THROW(sourcemeta::core::JSON{value}, std::invalid_argument);
}

TEST(JSON_real, copy_constructor_cannot_create_invalid_json) {
  EXPECT_THROW(
      {
        const sourcemeta::core::JSON source{
            std::numeric_limits<double>::quiet_NaN()};
        const sourcemeta::core::JSON copy{source};
      },
      std::invalid_argument);
}

TEST(JSON_real, move_constructor_cannot_create_invalid_json) {
  EXPECT_THROW(
      {
        sourcemeta::core::JSON source{std::numeric_limits<double>::infinity()};
        const sourcemeta::core::JSON moved{std::move(source)};
      },
      std::invalid_argument);
}

TEST(JSON_real, copy_assignment_cannot_create_invalid_json) {
  EXPECT_THROW(
      {
        const sourcemeta::core::JSON source{
            std::numeric_limits<double>::quiet_NaN()};
        sourcemeta::core::JSON target{1.0};
        target = source;
      },
      std::invalid_argument);
}

TEST(JSON_real, move_assignment_cannot_create_invalid_json) {
  EXPECT_THROW(
      {
        sourcemeta::core::JSON source{std::numeric_limits<double>::infinity()};
        sourcemeta::core::JSON target{1.0};
        target = std::move(source);
      },
      std::invalid_argument);
}

TEST(JSON_real, addition_operator_cannot_create_invalid_json) {
  EXPECT_THROW(
      {
        const sourcemeta::core::JSON left{
            std::numeric_limits<double>::infinity()};
        const sourcemeta::core::JSON right{1.0};
        const auto result{left + right};
      },
      std::invalid_argument);
}

TEST(JSON_real, subtraction_operator_cannot_create_invalid_json) {
  EXPECT_THROW(
      {
        const sourcemeta::core::JSON left{
            -std::numeric_limits<double>::infinity()};
        const sourcemeta::core::JSON right{1.0};
        const auto result{left - right};
      },
      std::invalid_argument);
}

TEST(JSON_real, addition_assignment_operator_cannot_create_invalid_json) {
  EXPECT_THROW(
      {
        sourcemeta::core::JSON document{
            std::numeric_limits<double>::infinity()};
        const sourcemeta::core::JSON addend{1.0};
        document += addend;
      },
      std::invalid_argument);
}

TEST(JSON_real, subtraction_assignment_operator_cannot_create_invalid_json) {
  EXPECT_THROW(
      {
        sourcemeta::core::JSON document{
            -std::numeric_limits<double>::infinity()};
        const sourcemeta::core::JSON subtrahend{1.0};
        document -= subtrahend;
      },
      std::invalid_argument);
}

TEST(JSON_real, estimated_byte_size_3_14) {
  const sourcemeta::core::JSON document{3.14};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_real, estimated_byte_size_minus_3_14) {
  const sourcemeta::core::JSON document{-3.14};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_real, estimated_byte_size_minus_5_0) {
  const sourcemeta::core::JSON document{5.0};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_real, estimated_byte_size_minus_0_0) {
  const sourcemeta::core::JSON document{0.0};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_real, fast_hash_3_14) {
  const sourcemeta::core::JSON document{3.14};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(JSON_real, fast_hash_minus_3_14) {
  const sourcemeta::core::JSON document{-3.14};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(JSON_real, fast_hash_minus_5_0) {
  const sourcemeta::core::JSON document{5.0};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(JSON_real, fast_hash_minus_0_0) {
  const sourcemeta::core::JSON document{0.0};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(JSON_real, less_than_real_decimal) {
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(real < decimal);
  EXPECT_FALSE(decimal < real);
}

TEST(JSON_real, less_than_or_equal_real_decimal) {
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"3.5"}};
  EXPECT_TRUE(real <= decimal);
  EXPECT_FALSE(decimal <= real);
  EXPECT_TRUE(real <= equal);
}

TEST(JSON_real, greater_than_real_decimal) {
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  EXPECT_TRUE(real > decimal);
  EXPECT_FALSE(decimal > real);
}

TEST(JSON_real, greater_than_or_equal_real_decimal) {
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"5.5"}};
  EXPECT_TRUE(real >= decimal);
  EXPECT_FALSE(decimal >= real);
  EXPECT_TRUE(real >= equal);
}

TEST(JSON_real, addition_real_decimal) {
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON result{real + decimal};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "9.2");
}

TEST(JSON_real, subtraction_real_decimal) {
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON result{real - decimal};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "2.3");
}

TEST(JSON_real, addition_assignment_real_decimal) {
  sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  real += decimal;
  EXPECT_TRUE(real.is_decimal());
  EXPECT_EQ(real.to_decimal().to_string(), "9.2");
}

TEST(JSON_real, subtraction_assignment_real_decimal) {
  sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  real -= decimal;
  EXPECT_TRUE(real.is_decimal());
  EXPECT_EQ(real.to_decimal().to_string(), "2.3");
}
