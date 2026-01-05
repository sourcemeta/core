#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <sstream>

TEST(JSON_type, print_null) {
  std::ostringstream output;
  output << sourcemeta::core::JSON::Type::Null;
  EXPECT_EQ(output.str(), "null");
}

TEST(JSON_type, print_boolean) {
  std::ostringstream output;
  output << sourcemeta::core::JSON::Type::Boolean;
  EXPECT_EQ(output.str(), "boolean");
}

TEST(JSON_type, print_integer) {
  std::ostringstream output;
  output << sourcemeta::core::JSON::Type::Integer;
  EXPECT_EQ(output.str(), "integer");
}

TEST(JSON_type, print_real) {
  std::ostringstream output;
  output << sourcemeta::core::JSON::Type::Real;
  EXPECT_EQ(output.str(), "real");
}

TEST(JSON_type, print_string) {
  std::ostringstream output;
  output << sourcemeta::core::JSON::Type::String;
  EXPECT_EQ(output.str(), "string");
}

TEST(JSON_type, print_array) {
  std::ostringstream output;
  output << sourcemeta::core::JSON::Type::Array;
  EXPECT_EQ(output.str(), "array");
}

TEST(JSON_type, print_object) {
  std::ostringstream output;
  output << sourcemeta::core::JSON::Type::Object;
  EXPECT_EQ(output.str(), "object");
}

TEST(JSON_type, make_set_empty) {
  const auto types = sourcemeta::core::make_set({});
  EXPECT_TRUE(types.none());
}

TEST(JSON_type, make_set_single) {
  const auto types =
      sourcemeta::core::make_set({sourcemeta::core::JSON::Type::String});
  EXPECT_FALSE(types.none());
  EXPECT_TRUE(types.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::String)));
  EXPECT_FALSE(types.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::Integer)));
  EXPECT_FALSE(types.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::Object)));
}

TEST(JSON_type, make_set_multiple) {
  const auto types =
      sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Integer,
                                  sourcemeta::core::JSON::Type::Real});
  EXPECT_FALSE(types.none());
  EXPECT_TRUE(types.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::Integer)));
  EXPECT_TRUE(
      types.test(static_cast<std::size_t>(sourcemeta::core::JSON::Type::Real)));
  EXPECT_FALSE(types.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::String)));
  EXPECT_FALSE(types.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::Object)));
}

TEST(JSON_type, make_set_intersection) {
  const auto types_a =
      sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Integer,
                                  sourcemeta::core::JSON::Type::Real,
                                  sourcemeta::core::JSON::Type::String});
  const auto types_b =
      sourcemeta::core::make_set({sourcemeta::core::JSON::Type::String,
                                  sourcemeta::core::JSON::Type::Object});
  const auto intersection = types_a & types_b;
  EXPECT_FALSE(intersection.none());
  EXPECT_TRUE(intersection.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::String)));
  EXPECT_FALSE(intersection.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::Integer)));
  EXPECT_FALSE(intersection.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::Real)));
  EXPECT_FALSE(intersection.test(
      static_cast<std::size_t>(sourcemeta::core::JSON::Type::Object)));
}

TEST(JSON_type, make_set_no_intersection) {
  const auto types_a =
      sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Integer,
                                  sourcemeta::core::JSON::Type::Real});
  const auto types_b =
      sourcemeta::core::make_set({sourcemeta::core::JSON::Type::String,
                                  sourcemeta::core::JSON::Type::Object});
  const auto intersection = types_a & types_b;
  EXPECT_TRUE(intersection.none());
}
