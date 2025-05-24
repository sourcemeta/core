#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/jsonschema_error.h>

#include <string>      // std::string
#include <string_view> // std::string_view

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://www.sourcemeta.com/test-1") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-02/schema#",
      "id": "https://www.sourcemeta.com/test-1",
      "type": "string"
    })JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

TEST(JSONSchema_bundle_draft2, no_references_no_id) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-02/schema#"
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::bundle(
          document, sourcemeta::core::schema_official_walker, test_resolver),
      sourcemeta::core::SchemaError);
}

TEST(JSONSchema_bundle_draft2, const_no_references_no_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-02/schema#"
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::bundle(
          document, sourcemeta::core::schema_official_walker, test_resolver),
      sourcemeta::core::SchemaError);
}

TEST(JSONSchema_bundle_draft2, simple_bundling) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-02/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-1" }
    }
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::bundle(
          document, sourcemeta::core::schema_official_walker, test_resolver),
      sourcemeta::core::SchemaError);
}
