#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint> // std::int64_t
#include <string>  // std::string

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
