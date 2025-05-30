#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_wrap, identifier_without_fragment) {
  const auto identifier{"https://www.example.com"};
  const auto result{sourcemeta::core::wrap(identifier)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://www.example.com"
  })JSON")};

  EXPECT_EQ(result, expected);
}

// Should still work, as people might be relying on relative URIs
// as identifiers though still correctly resolving those
TEST(JSONSchema_wrap, identifier_relative) {
  const auto identifier{"/foo/bar"};
  const auto result{sourcemeta::core::wrap(identifier)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "/foo/bar"
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, identifier_with_fragment) {
  const auto identifier{"https://www.example.com#/foo/bar"};
  const auto result{sourcemeta::core::wrap(identifier)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://www.example.com#/foo/bar"
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_without_identifier) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "__sourcemeta-core-wrap__#/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "__sourcemeta-core-wrap__",
        "items": {
          "type": "string"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_without_identifier_and_relative_uri) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "$ref": "relative"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver)};

  // We don't want the relative reference to be resolved against
  // an absolute base
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "__sourcemeta-core-wrap__#/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "__sourcemeta-core-wrap__",
        "items": {
          "$ref": "relative"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_without_identifier_with_default_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver,
      "https://json-schema.org/draft/2020-12/schema")};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "__sourcemeta-core-wrap__#/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "__sourcemeta-core-wrap__",
        "items": {
          "type": "string"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap,
     schema_without_identifier_with_different_default_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver,
      "https://json-schema.org/draft/2019-09/schema")};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "__sourcemeta-core-wrap__#/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "__sourcemeta-core-wrap__",
        "items": {
          "type": "string"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_without_identifier_empty_pointer) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {}, sourcemeta::core::schema_official_resolver)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "type": "string"
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_without_identifier_without_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "items": {
      "type": "string"
    }
  })JSON")};

  EXPECT_THROW(
      sourcemeta::core::wrap(schema, {"items"},
                             sourcemeta::core::schema_official_resolver),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_wrap, schema_with_identifier) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://sourcemeta.com/1",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://sourcemeta.com/1#/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://sourcemeta.com/1",
        "items": {
          "type": "string"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_with_identifier_trailing_empty_fragment) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://sourcemeta.com/1#",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://sourcemeta.com/1#/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://sourcemeta.com/1#",
        "items": {
          "type": "string"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_with_identifier_different_default_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://sourcemeta.com/1",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver,
      "https://json-schema.org/draft/2019-09/schema")};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://sourcemeta.com/1#/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://sourcemeta.com/1",
        "items": {
          "type": "string"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_with_identifier_default_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/1",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver,
      "https://json-schema.org/draft/2019-09/schema")};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://sourcemeta.com/1#/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "$id": "https://sourcemeta.com/1",
        "items": {
          "type": "string"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_with_identifier_empty_pointer) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/1",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {}, sourcemeta::core::schema_official_resolver)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/1",
    "items": {
      "type": "string"
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, schema_with_identifier_no_dialect) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/1",
    "items": {
      "type": "string"
    }
  })JSON")};

  EXPECT_THROW(
      sourcemeta::core::wrap(schema, {"items"},
                             sourcemeta::core::schema_official_resolver),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_wrap, schema_with_identifier_with_fragment) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "https://sourcemeta.com/1#foo",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto result{sourcemeta::core::wrap(
      schema, {"items"}, sourcemeta::core::schema_official_resolver)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/$defs/schema/items",
    "$defs": {
      "schema": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://sourcemeta.com/1#foo",
        "items": {
          "type": "string"
        }
      }
    }
  })JSON")};

  EXPECT_EQ(result, expected);
}
