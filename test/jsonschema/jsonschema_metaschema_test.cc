#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_metaschema, example_2020_12) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string"
  })JSON")};

  const auto metaschema{sourcemeta::core::metaschema(
      schema, sourcemeta::core::schema_official_resolver)};
  EXPECT_TRUE(metaschema.is_object());
  EXPECT_TRUE(sourcemeta::core::schema_official_resolver(
                  "https://json-schema.org/draft/2020-12/schema")
                  .has_value());
  EXPECT_EQ(metaschema, sourcemeta::core::schema_official_resolver(
                            "https://json-schema.org/draft/2020-12/schema")
                            .value());
}

TEST(JSONSchema_metaschema, with_default_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "type": "string"
  })JSON")};

  const auto metaschema{sourcemeta::core::metaschema(
      schema, sourcemeta::core::schema_official_resolver,
      "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_TRUE(metaschema.is_object());
  EXPECT_TRUE(sourcemeta::core::schema_official_resolver(
                  "https://json-schema.org/draft/2020-12/schema")
                  .has_value());
  EXPECT_EQ(metaschema, sourcemeta::core::schema_official_resolver(
                            "https://json-schema.org/draft/2020-12/schema")
                            .value());
}

TEST(JSONSchema_metaschema, no_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "type": "string"
  })JSON")};

  EXPECT_THROW(sourcemeta::core::metaschema(
                   schema, sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaUnknownDialectError);
}

TEST(JSONSchema_metaschema, unknown_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com",
    "type": "string"
  })JSON")};

  EXPECT_THROW(sourcemeta::core::metaschema(
                   schema, sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaResolutionError);
}
