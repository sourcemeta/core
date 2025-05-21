#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_dialect, dialect_true) {
  const sourcemeta::core::JSON document{true};
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_FALSE(dialect.has_value());
}

TEST(JSONSchema_dialect, dialect_false) {
  const sourcemeta::core::JSON document{false};
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_FALSE(dialect.has_value());
}

TEST(JSONSchema_dialect, dialect_empty_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_FALSE(dialect.has_value());
}

TEST(JSONSchema_dialect, dialect_empty_object_with_default) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(
      document, "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "https://json-schema.org/draft/2020-12/schema");
}
