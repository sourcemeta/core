#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <sstream>

TEST(JSON_parse_roundtrip, decimal_simple) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"123.45678901234567890"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
  EXPECT_EQ(parsed.to_decimal().to_string(), "123.45678901234567890");
}

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

TEST(JSON_parse_roundtrip, decimal_very_small) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"0.00000000000000000123456789"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_negative) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"-987.654321987654321"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
  EXPECT_EQ(parsed.to_decimal().to_string(), "-987.654321987654321");
}

TEST(JSON_parse_roundtrip, decimal_exponential_positive) {
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{
      "1.234567890123456789012345678901234567890e10"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_negative) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"4.567890123456789012345678901234567890e-20"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_large_exponent) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"9.876543219876543210987654321098765432e100"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_small_exponent) {
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{
      "1.234567890123456789012345678901234567890e-100"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_zero_exponent) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"5.567890123456789012345678901234567890e0"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_negative_mantissa) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"-2.567890123456789012345678901234567890e15"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_exponential_negative_mantissa_negative_exp) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"-3.756789012345678901234567890123456789e-25"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_zero) {
  const sourcemeta::core::JSON original{
      sourcemeta::core::Decimal{"0.000000000000000000000000000001"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_in_array) {
  auto array{sourcemeta::core::JSON::make_array()};
  array.push_back(sourcemeta::core::JSON{sourcemeta::core::Decimal{
      "1.234567890123456789012345678901234567890e45"}});
  array.push_back(sourcemeta::core::JSON{sourcemeta::core::Decimal{
      "9.876543210987654321098765432109876543210e-50"}});
  array.push_back(sourcemeta::core::JSON{
      sourcemeta::core::Decimal{"123456789012345678901234567890"}});
  const sourcemeta::core::JSON original{std::move(array)};

  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_array());
  EXPECT_EQ(parsed.size(), 3);
  EXPECT_TRUE(parsed.at(0).is_decimal());
  EXPECT_TRUE(parsed.at(1).is_decimal());
  EXPECT_TRUE(parsed.at(2).is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_in_object) {
  auto object{sourcemeta::core::JSON::make_object()};
  object.assign("large", sourcemeta::core::JSON{sourcemeta::core::Decimal{
                             "1.234567890123456789012345678901234567890e200"}});
  object.assign("small",
                sourcemeta::core::JSON{sourcemeta::core::Decimal{
                    "1.234567890123456789012345678901234567890e-200"}});
  object.assign("precise", sourcemeta::core::JSON{sourcemeta::core::Decimal{
                               "3.141592653589793238462643383279"}});
  const sourcemeta::core::JSON original{std::move(object)};

  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_object());
  EXPECT_TRUE(parsed.defines("large"));
  EXPECT_TRUE(parsed.defines("small"));
  EXPECT_TRUE(parsed.defines("precise"));
  EXPECT_TRUE(parsed.at("large").is_decimal());
  EXPECT_TRUE(parsed.at("small").is_decimal());
  EXPECT_TRUE(parsed.at("precise").is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_high_precision) {
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{
      "3.14159265358979323846264338327950288419716939937510"}};
  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}

TEST(JSON_parse_roundtrip, decimal_scientific_from_to_string) {
  const sourcemeta::core::Decimal decimal{"123456789012345678901234567890"};
  const std::string scientific{decimal.to_scientific_string()};
  const sourcemeta::core::JSON original{sourcemeta::core::Decimal{scientific}};

  std::ostringstream stream;
  sourcemeta::core::stringify(original, stream);
  const auto parsed{sourcemeta::core::parse_json(stream.str())};
  EXPECT_EQ(original, parsed);
  EXPECT_TRUE(parsed.is_decimal());
}
