#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>      // std::string
#include <string_view> // std::string_view

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://www.sourcemeta.com/test-1") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-01/schema#",
      "id": "https://www.sourcemeta.com/test-1",
      "type": "string"
    })JSON");
  } else {
    return sourcemeta::core::official_resolver(identifier);
  }
}

TEST(JSONSchema_bundle_draft1, no_references_no_id) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-01/schema#"
  })JSON");

  EXPECT_THROW(sourcemeta::core::bundle(document,
                                        sourcemeta::core::default_schema_walker,
                                        test_resolver),
               sourcemeta::core::SchemaError);
}

TEST(JSONSchema_bundle_draft1, const_no_references_no_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-01/schema#"
  })JSON");

  EXPECT_THROW(sourcemeta::core::bundle(document,
                                        sourcemeta::core::default_schema_walker,
                                        test_resolver),
               sourcemeta::core::SchemaError);
}

TEST(JSONSchema_bundle_draft1, simple_bundling) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-01/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-1" }
    }
  })JSON");

  EXPECT_THROW(sourcemeta::core::bundle(document,
                                        sourcemeta::core::default_schema_walker,
                                        test_resolver),
               sourcemeta::core::SchemaError);
}
