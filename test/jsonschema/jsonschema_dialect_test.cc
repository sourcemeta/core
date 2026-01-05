#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_dialect, dialect_true) {
  const sourcemeta::core::JSON document{true};
  const auto dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.empty());
}

TEST(JSONSchema_dialect, dialect_false) {
  const sourcemeta::core::JSON document{false};
  const auto dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.empty());
}

TEST(JSONSchema_dialect, dialect_empty_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  const auto dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.empty());
}

TEST(JSONSchema_dialect, dialect_empty_object_with_default) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  const auto dialect{sourcemeta::core::dialect(
      document, "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_EQ(dialect, "https://json-schema.org/draft/2020-12/schema");
}
