#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <format> // std::format
#include <string> // std::string

TEST(JSON_format, null_value) {
  const sourcemeta::core::JSON value{nullptr};
  const std::string result{std::format("{}", value)};
  EXPECT_EQ(result, "null");
}

TEST(JSON_format, boolean_true) {
  const sourcemeta::core::JSON value{true};
  EXPECT_EQ(std::format("{}", value), "true");
}

TEST(JSON_format, boolean_false) {
  const sourcemeta::core::JSON value{false};
  EXPECT_EQ(std::format("{}", value), "false");
}

TEST(JSON_format, integer_positive) {
  const sourcemeta::core::JSON value{42};
  EXPECT_EQ(std::format("{}", value), "42");
}

TEST(JSON_format, integer_negative) {
  const sourcemeta::core::JSON value{-10};
  EXPECT_EQ(std::format("{}", value), "-10");
}

TEST(JSON_format, integer_zero) {
  const sourcemeta::core::JSON value{0};
  EXPECT_EQ(std::format("{}", value), "0");
}

TEST(JSON_format, real_value) {
  const sourcemeta::core::JSON value{3.14};
  EXPECT_EQ(std::format("{}", value), "3.14");
}

TEST(JSON_format, string_value) {
  const sourcemeta::core::JSON value{"hello"};
  EXPECT_EQ(std::format("{}", value), "\"hello\"");
}

TEST(JSON_format, string_empty) {
  const sourcemeta::core::JSON value{""};
  EXPECT_EQ(std::format("{}", value), "\"\"");
}

TEST(JSON_format, type_null) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Null), "null");
}

TEST(JSON_format, type_boolean) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Boolean),
            "boolean");
}

TEST(JSON_format, type_integer) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Integer),
            "integer");
}

TEST(JSON_format, type_real) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Real), "real");
}

TEST(JSON_format, type_decimal) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Decimal),
            "decimal");
}

TEST(JSON_format, type_string) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::String), "string");
}

TEST(JSON_format, type_array) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Array), "array");
}

TEST(JSON_format, type_object) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Object), "object");
}
