#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>

#include <cmath>
#include <limits>

TEST(positive) {
  const sourcemeta::core::JSON document{10.2};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 10.2);
}

TEST(negative) {
  const sourcemeta::core::JSON document{-10.02};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -10.02);
}

TEST(zero) {
  const sourcemeta::core::JSON document{0.0};
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), 0.0);
}

TEST(type) {
  const sourcemeta::core::JSON document{1.23};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Real);
}

TEST(long_double_nan) {
  const double value{static_cast<double>(NAN)};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(double_nan) {
  const double value{static_cast<double>(NAN)};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(float_nan) {
  const float value{NAN};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(long_double_infinity) {
  const double value{std::numeric_limits<double>::infinity()};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(double_infinity) {
  const double value{std::numeric_limits<double>::infinity()};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(float_infinity) {
  const float value{std::numeric_limits<float>::infinity()};
  try {
    sourcemeta::core::JSON{value};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(copy_constructor_cannot_create_invalid_json) {
  try {
    const sourcemeta::core::JSON source{
        std::numeric_limits<double>::quiet_NaN()};
    const sourcemeta::core::JSON copy{source};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(move_constructor_cannot_create_invalid_json) {
  try {
    sourcemeta::core::JSON source{std::numeric_limits<double>::infinity()};
    const sourcemeta::core::JSON moved{std::move(source)};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(copy_assignment_cannot_create_invalid_json) {
  try {
    const sourcemeta::core::JSON source{
        std::numeric_limits<double>::quiet_NaN()};
    sourcemeta::core::JSON target{1.0};
    target = source;
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(move_assignment_cannot_create_invalid_json) {
  try {
    sourcemeta::core::JSON source{std::numeric_limits<double>::infinity()};
    sourcemeta::core::JSON target{1.0};
    target = std::move(source);
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(addition_operator_cannot_create_invalid_json) {
  try {
    const sourcemeta::core::JSON left{std::numeric_limits<double>::infinity()};
    const sourcemeta::core::JSON right{1.0};
    const auto result{left + right};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(subtraction_operator_cannot_create_invalid_json) {
  try {
    const sourcemeta::core::JSON left{-std::numeric_limits<double>::infinity()};
    const sourcemeta::core::JSON right{1.0};
    const auto result{left - right};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(addition_assignment_operator_cannot_create_invalid_json) {
  try {
    sourcemeta::core::JSON document{std::numeric_limits<double>::infinity()};
    const sourcemeta::core::JSON addend{1.0};
    document += addend;
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(subtraction_assignment_operator_cannot_create_invalid_json) {
  try {
    sourcemeta::core::JSON document{-std::numeric_limits<double>::infinity()};
    const sourcemeta::core::JSON subtrahend{1.0};
    document -= subtrahend;
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}

TEST(estimated_byte_size_3_14) {
  const sourcemeta::core::JSON document{3.14};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(estimated_byte_size_minus_3_14) {
  const sourcemeta::core::JSON document{-3.14};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(estimated_byte_size_minus_5_0) {
  const sourcemeta::core::JSON document{5.0};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(estimated_byte_size_minus_0_0) {
  const sourcemeta::core::JSON document{0.0};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(fast_hash_3_14) {
  const sourcemeta::core::JSON document{3.14};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(fast_hash_minus_3_14) {
  const sourcemeta::core::JSON document{-3.14};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(fast_hash_minus_5_0) {
  const sourcemeta::core::JSON document{5.0};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(fast_hash_minus_0_0) {
  const sourcemeta::core::JSON document{0.0};
  EXPECT_EQ(document.fast_hash(), 5);
}

TEST(less_than_real_decimal) {
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(real < decimal);
  EXPECT_FALSE(decimal < real);
}

TEST(less_than_or_equal_real_decimal) {
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"3.5"}};
  EXPECT_TRUE(real <= decimal);
  EXPECT_FALSE(decimal <= real);
  EXPECT_TRUE(real <= equal);
}

TEST(greater_than_real_decimal) {
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  EXPECT_TRUE(real > decimal);
  EXPECT_FALSE(decimal > real);
}

TEST(greater_than_or_equal_real_decimal) {
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"5.5"}};
  EXPECT_TRUE(real >= decimal);
  EXPECT_FALSE(decimal >= real);
  EXPECT_TRUE(real >= equal);
}

TEST(addition_real_decimal) {
  const sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON result{real + decimal};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "9.2");
}

TEST(subtraction_real_decimal) {
  const sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON result{real - decimal};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "2.3");
}

TEST(addition_assignment_real_decimal) {
  sourcemeta::core::JSON real{3.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  real += decimal;
  EXPECT_TRUE(real.is_decimal());
  EXPECT_EQ(real.to_decimal().to_string(), "9.2");
}

TEST(subtraction_assignment_real_decimal) {
  sourcemeta::core::JSON real{5.5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  real -= decimal;
  EXPECT_TRUE(real.is_decimal());
  EXPECT_EQ(real.to_decimal().to_string(), "2.3");
}
