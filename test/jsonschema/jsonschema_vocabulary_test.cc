#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_vocabulary, core_vocabularies_boolean_without_default) {
  const sourcemeta::core::JSON document{true};
  EXPECT_THROW(sourcemeta::core::vocabularies(
                   document, sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_vocabulary, unresolvable_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://non-existent.com/dialect"
  })JSON");
  EXPECT_THROW(sourcemeta::core::vocabularies(
                   document, sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaResolutionError);
}
