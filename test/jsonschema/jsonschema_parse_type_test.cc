#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <set>

TEST(JSONSchema_parse_type, null) {
  std::set<sourcemeta::core::JSON::Type> types;
  sourcemeta::core::parse_schema_type(
      "null", [&types](const auto type) { types.emplace(type); });
  EXPECT_EQ(types.size(), 1);
  EXPECT_TRUE(types.contains(sourcemeta::core::JSON::Type::Null));
}

TEST(JSONSchema_parse_type, boolean) {
  std::set<sourcemeta::core::JSON::Type> types;
  sourcemeta::core::parse_schema_type(
      "boolean", [&types](const auto type) { types.emplace(type); });
  EXPECT_EQ(types.size(), 1);
  EXPECT_TRUE(types.contains(sourcemeta::core::JSON::Type::Boolean));
}

TEST(JSONSchema_parse_type, object) {
  std::set<sourcemeta::core::JSON::Type> types;
  sourcemeta::core::parse_schema_type(
      "object", [&types](const auto type) { types.emplace(type); });
  EXPECT_EQ(types.size(), 1);
  EXPECT_TRUE(types.contains(sourcemeta::core::JSON::Type::Object));
}

TEST(JSONSchema_parse_type, array) {
  std::set<sourcemeta::core::JSON::Type> types;
  sourcemeta::core::parse_schema_type(
      "array", [&types](const auto type) { types.emplace(type); });
  EXPECT_EQ(types.size(), 1);
  EXPECT_TRUE(types.contains(sourcemeta::core::JSON::Type::Array));
}

TEST(JSONSchema_parse_type, number) {
  std::set<sourcemeta::core::JSON::Type> types;
  sourcemeta::core::parse_schema_type(
      "number", [&types](const auto type) { types.emplace(type); });
  EXPECT_EQ(types.size(), 2);
  EXPECT_TRUE(types.contains(sourcemeta::core::JSON::Type::Integer));
  EXPECT_TRUE(types.contains(sourcemeta::core::JSON::Type::Real));
}

TEST(JSONSchema_parse_type, integer) {
  std::set<sourcemeta::core::JSON::Type> types;
  sourcemeta::core::parse_schema_type(
      "integer", [&types](const auto type) { types.emplace(type); });
  EXPECT_EQ(types.size(), 1);
  EXPECT_TRUE(types.contains(sourcemeta::core::JSON::Type::Integer));
}

TEST(JSONSchema_parse_type, string) {
  std::set<sourcemeta::core::JSON::Type> types;
  sourcemeta::core::parse_schema_type(
      "string", [&types](const auto type) { types.emplace(type); });
  EXPECT_EQ(types.size(), 1);
  EXPECT_TRUE(types.contains(sourcemeta::core::JSON::Type::String));
}

TEST(JSONSchema_parse_type, unknown) {
  std::set<sourcemeta::core::JSON::Type> types;
  sourcemeta::core::parse_schema_type(
      "foo", [&types](const auto type) { types.emplace(type); });
  EXPECT_EQ(types.size(), 0);
}
