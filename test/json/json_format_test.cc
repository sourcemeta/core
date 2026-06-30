#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>

#include <format> // std::format
#include <string> // std::string

TEST(null_value) {
  const sourcemeta::core::JSON value{nullptr};
  const std::string result{std::format("{}", value)};
  EXPECT_EQ(result, "null");
}

TEST(boolean_true) {
  const sourcemeta::core::JSON value{true};
  EXPECT_EQ(std::format("{}", value), "true");
}

TEST(boolean_false) {
  const sourcemeta::core::JSON value{false};
  EXPECT_EQ(std::format("{}", value), "false");
}

TEST(integer_positive) {
  const sourcemeta::core::JSON value{42};
  EXPECT_EQ(std::format("{}", value), "42");
}

TEST(integer_negative) {
  const sourcemeta::core::JSON value{-10};
  EXPECT_EQ(std::format("{}", value), "-10");
}

TEST(integer_zero) {
  const sourcemeta::core::JSON value{0};
  EXPECT_EQ(std::format("{}", value), "0");
}

TEST(real_value) {
  const sourcemeta::core::JSON value{3.14};
  EXPECT_EQ(std::format("{}", value), "3.14");
}

TEST(string_value) {
  const sourcemeta::core::JSON value{"hello"};
  EXPECT_EQ(std::format("{}", value), "\"hello\"");
}

TEST(string_empty) {
  const sourcemeta::core::JSON value{""};
  EXPECT_EQ(std::format("{}", value), "\"\"");
}

TEST(type_null) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Null), "null");
}

TEST(type_boolean) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Boolean),
            "boolean");
}

TEST(type_integer) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Integer),
            "integer");
}

TEST(type_real) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Real), "real");
}

TEST(type_decimal) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Decimal),
            "decimal");
}

TEST(type_string) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::String), "string");
}

TEST(type_array) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Array), "array");
}

TEST(type_object) {
  EXPECT_EQ(std::format("{}", sourcemeta::core::JSON::Type::Object), "object");
}
