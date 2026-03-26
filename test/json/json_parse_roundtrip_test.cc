#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <sstream>

TEST(JSON_parse_roundtrip, decimal_large_integer) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
  EXPECT_EQ(parsed.to_decimal().to_string(), "123456789012345678901234567890");
}

TEST(JSON_parse_roundtrip, decimal_exponential_overflow_positive) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"1.23456789012345678901234567890e309"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_overflow_large) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"9.87654321098765432109876543210e320"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_very_large_exponent) {
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{"1.5e400"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_negative_overflow) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"-2.718281828459045235360287471352e310"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip,
     decimal_exponential_small_coefficient_large_exponent) {
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{"3.14e350"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_small_1) {
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{"1e-05"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_small_2) {
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{"1e-06"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_small_3) {
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{"5e-05"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_small_4) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"6.10352e-05"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, real_number_precision_1) {
  const auto original{sourcemeta::core::parse_json("10.213854")};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_real());
}

TEST(JSON_parse_roundtrip, real_number_precision_2) {
  const auto original{sourcemeta::core::parse_json("52.273577")};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_real());
}

TEST(JSON_parse_roundtrip, real_number_precision_3) {
  const auto original{sourcemeta::core::parse_json("10.107058")};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_real());
}

TEST(JSON_parse_roundtrip, real_number_precision_4) {
  const auto original{sourcemeta::core::parse_json("52.148044")};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_real());
}

TEST(JSON_parse_roundtrip, real_number_precision_negative) {
  const auto original{sourcemeta::core::parse_json("-52.273577")};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_real());
}

TEST(JSON_parse_roundtrip, real_number_precision_small_fractional) {
  const auto original{sourcemeta::core::parse_json("0.00123456789")};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_real());
}

TEST(JSON_parse_roundtrip, real_number_precision_15_significant_digits) {
  const auto original{sourcemeta::core::parse_json("1.23456789012345")};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_real());
}
