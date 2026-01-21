#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

#define EXPECT_FRAME_STATIC_2019_09_POINTER(                                   \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name)        \
  EXPECT_FRAME_STATIC_POINTER(frame, reference, root_id, expected_pointer,     \
                              "https://json-schema.org/draft/2019-09/schema",  \
                              JSON_Schema_2019_09, expected_base,              \
                              expected_relative_pointer, expected_parent,      \
                              expected_property_name);

#define EXPECT_FRAME_STATIC_2019_09_RESOURCE(                                  \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name)        \
  EXPECT_FRAME_STATIC_RESOURCE(frame, reference, root_id, expected_pointer,    \
                               "https://json-schema.org/draft/2019-09/schema", \
                               JSON_Schema_2019_09, expected_base,             \
                               expected_relative_pointer, expected_parent,     \
                               expected_property_name);

#define EXPECT_FRAME_STATIC_2019_09_ANCHOR(                                    \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name)        \
  EXPECT_FRAME_STATIC_ANCHOR(frame, reference, root_id, expected_pointer,      \
                             "https://json-schema.org/draft/2019-09/schema",   \
                             JSON_Schema_2019_09, expected_base,               \
                             expected_relative_pointer, expected_parent,       \
                             expected_property_name);

#define EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(                                 \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name)        \
  EXPECT_FRAME_STATIC_SUBSCHEMA(                                               \
      frame, reference, root_id, expected_pointer,                             \
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,     \
      expected_base, expected_relative_pointer, expected_parent,               \
      expected_property_name);

#define EXPECT_FRAME_DYNAMIC_2019_09_ANCHOR(                                   \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name)        \
  EXPECT_FRAME_DYNAMIC_ANCHOR(frame, reference, root_id, expected_pointer,     \
                              "https://json-schema.org/draft/2019-09/schema",  \
                              JSON_Schema_2019_09, expected_base,              \
                              expected_relative_pointer, expected_parent,      \
                              expected_property_name);

TEST(JSONSchema_frame_2019_09, anonymous_with_nested_schema_resource) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalProperties": { "$id": "https://example.com" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/additionalProperties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/additionalProperties/$id",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalProperties", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalProperties", "/additionalProperties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalProperties/$id", "/additionalProperties/$id",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalProperties", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, empty_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, empty_schema_trailing_hash) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema#",
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, one_level_applicators_without_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": { "type": "string" },
    "properties": {
      "foo": { "type": "number" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 8);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/schema", "/items/type", "/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "/properties/foo", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, one_level_applicators_with_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test/qux",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": { "$id": "../foo", "type": "string" },
    "properties": {
      "foo": { "$anchor": "test", "type": "number" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 14);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/test/qux",
      "https://www.sourcemeta.com/test/qux", "",
      "https://www.sourcemeta.com/test/qux", "", std::nullopt, false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", "", false);

  // Anchors

  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/test/qux#test",
      "https://www.sourcemeta.com/test/qux", "/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo", "", false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$id",
      "https://www.sourcemeta.com/test/qux", "/$id",
      "https://www.sourcemeta.com/test/qux", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test/qux#/items",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties",
      "https://www.sourcemeta.com/test/qux", "/properties",
      "https://www.sourcemeta.com/test/qux", "/properties", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo/$anchor",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/$anchor",
      "/properties/foo", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo/type",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/type",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/type",
      "/properties/foo", false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/foo#/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, subschema_absolute_identifier) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": {
      "$id": "https://www.sourcemeta.com/foo",
      "type": "string"
     }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", "", false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/$id",
      "https://www.sourcemeta.com/schema", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/schema", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/foo#/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, nested_schemas) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$id": "https://www.sourcemeta.com/foo",
        "$anchor": "test",
        "items": {
          "$id": "qux"
        }
      },
      "bar": {
        "$id": "https://www.sourcemeta.com/bar"
      },
      "baz": {
        "$id": "baz",
        "items": {
          "$anchor": "extra"
        }
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 30);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/foo#test",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/baz", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/baz#extra",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", "/properties/baz", false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/qux",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", "/properties/foo", false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false);

  // foo
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/properties/foo", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/foo", "/$anchor", "/properties/foo", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/items",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", "/properties/foo", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/items/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", "/properties/foo/items", false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/properties/foo", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/foo", "/$anchor", "/properties/foo", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/foo#/items",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", "/properties/foo", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/foo#/items/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", "/properties/foo/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/qux#/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", "/properties/foo/items", false);

  // bar
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", "/properties/bar", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", "/properties/bar", false);

  // baz
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/baz", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/$id",
      "https://www.sourcemeta.com/schema", "/properties/baz/$id",
      "https://www.sourcemeta.com/baz", "/$id", "/properties/baz", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", "/properties/baz", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/baz", "/items/$anchor",
      "/properties/baz/items", false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/baz#/$id",
      "https://www.sourcemeta.com/schema", "/properties/baz/$id",
      "https://www.sourcemeta.com/baz", "/$id", "/properties/baz", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/baz#/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", "/properties/baz", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/baz#/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/baz", "/items/$anchor",
      "/properties/baz/items", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, id_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": { "$id": "schema" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_2019_09, static_anchor_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$anchor": "foo",
    "items": { "$anchor": "foo" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_2019_09, explicit_argument_id_same) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2019-09/schema",
                "https://www.sourcemeta.com/schema");

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, anchor_top_level) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$anchor": "foo"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$anchor",
      "https://www.sourcemeta.com/schema", "/$anchor",
      "https://www.sourcemeta.com/schema", "/$anchor", "", false);

  // Anchors

  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, explicit_argument_id_different) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": {
      "$anchor": "foo"
    },
    "properties": {
      "one": {
        "$id": "test",
        "$anchor": "bar"
      },
      "two": {
        "$id": "https://www.test.com",
        "$anchor": "baz"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2019-09/schema",
                "https://www.example.com");

  EXPECT_EQ(frame.locations().size(), 38);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.example.com", "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.example.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.test.com", "https://www.sourcemeta.com/schema",
      "/properties/two", "https://www.test.com", "", "", false);

  // Anchors

  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", "", false);
  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.example.com#foo", "https://www.sourcemeta.com/schema",
      "/items", "https://www.example.com", "/items", "", false);
  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/test#bar",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.example.com/test#bar",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_ANCHOR(
      frame, "https://www.test.com#baz", "https://www.sourcemeta.com/schema",
      "/properties/two", "https://www.test.com", "", "", false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/$anchor",
      "https://www.sourcemeta.com/schema", "/items/$anchor",
      "https://www.sourcemeta.com/schema", "/items/$anchor", "/items", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/one",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.sourcemeta.com/test", "/$id", "/properties/one", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/one/$anchor",
      "https://www.sourcemeta.com/test", "/$anchor", "/properties/one", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/two",
      "https://www.sourcemeta.com/schema", "/properties/two",
      "https://www.test.com", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$id",
      "https://www.sourcemeta.com/schema", "/properties/two/$id",
      "https://www.test.com", "/$id", "/properties/two", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/two/$anchor",
      "https://www.test.com", "/$anchor", "/properties/two", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test#/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.sourcemeta.com/test", "/$id", "/properties/one", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/test#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/one/$anchor",
      "https://www.sourcemeta.com/test", "/$anchor", "/properties/one", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.test.com#/$id", "https://www.sourcemeta.com/schema",
      "/properties/two/$id", "https://www.test.com", "/$id", "/properties/two",
      false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.test.com#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/two/$anchor",
      "https://www.test.com", "/$anchor", "/properties/two", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, ref_metaschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, "", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, location_independent_identifier_anonymous) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "foo": {
        "$id": "#foo"
      },
      "bar": {
        "$ref": "#foo"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver),
               sourcemeta::core::SchemaError);
}

TEST(JSONSchema_frame_2019_09, recursive_anchor_true_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": true
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  // Dynamic anchors

  EXPECT_FRAME_DYNAMIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // Static identifiers

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // Static pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor", "", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, recursive_anchor_false_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": false
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 4);

  // Static identifiers

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // Static pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor", "", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, recursive_anchor_true_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$recursiveAnchor": true
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  // Dynamic anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "", "/properties/foo",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);

  // Static frames

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$recursiveAnchor",
      "/properties/foo/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/properties/foo", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, recursive_anchor_false_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$recursiveAnchor": false
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  // Static frames

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$recursiveAnchor",
      "/properties/foo/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/properties/foo", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, recursive_ref_no_recursive_anchor_anonymous) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalItems": {
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 4);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalItems", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$recursiveRef",
      "/additionalItems/$recursiveRef",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(frame, "/additionalItems/$recursiveRef", "", "",
                          std::nullopt, "#");
}

TEST(JSONSchema_frame_2019_09, recursive_ref_no_recursive_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalItems": {
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "/additionalItems", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(frame, "/additionalItems/$recursiveRef",
                          "https://www.sourcemeta.com/schema",
                          "https://www.sourcemeta.com/schema", std::nullopt,
                          "#");
}

TEST(JSONSchema_frame_2019_09, recursive_ref_recursive_anchor_false_anonymous) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": false,
    "additionalItems": {
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$recursiveAnchor", "/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalItems", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$recursiveRef",
      "/additionalItems/$recursiveRef",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(frame, "/additionalItems/$recursiveRef", "", "",
                          std::nullopt, "#");
}

TEST(JSONSchema_frame_2019_09, recursive_ref_recursive_anchor_false) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": false,
    "additionalItems": {
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "/additionalItems", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(frame, "/additionalItems/$recursiveRef",
                          "https://www.sourcemeta.com/schema",
                          "https://www.sourcemeta.com/schema", std::nullopt,
                          "#");
}

TEST(JSONSchema_frame_2019_09, recursive_ref_recursive_anchor_true_anonymous) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": true,
    "additionalItems": {
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$recursiveAnchor", "/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalItems", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$recursiveRef",
      "/additionalItems/$recursiveRef",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);

  // Anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/additionalItems/$recursiveRef", "", "",
                           std::nullopt, "#");
}

TEST(JSONSchema_frame_2019_09, recursive_ref_recursive_anchor_true) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": true,
    "additionalItems": {
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "/additionalItems", false);

  // Anchors

  EXPECT_FRAME_DYNAMIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/additionalItems/$recursiveRef",
                           "https://www.sourcemeta.com/schema",
                           "https://www.sourcemeta.com/schema", std::nullopt,
                           "#");
}

TEST(JSONSchema_frame_2019_09,
     recursive_ref_recursive_anchor_false_anonymous_nested) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalItems": {
      "$id": "https://example.com",
      "$recursiveAnchor": false,
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalItems", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$id", "/additionalItems/$id",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$recursiveAnchor",
      "/additionalItems/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$recursiveRef",
      "/additionalItems/$recursiveRef",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/additionalItems/$id",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$recursiveAnchor",
      "/additionalItems/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$recursiveRef",
      "/additionalItems/$recursiveRef",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(frame, "/additionalItems/$recursiveRef",
                          "https://example.com", "https://example.com",
                          std::nullopt, "#");
}

TEST(JSONSchema_frame_2019_09,
     recursive_ref_recursive_anchor_true_anonymous_nested) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalItems": {
      "$id": "https://example.com",
      "$recursiveAnchor": true,
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 11);

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalItems", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$id", "/additionalItems/$id",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$recursiveAnchor",
      "/additionalItems/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$recursiveRef",
      "/additionalItems/$recursiveRef",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/additionalItems/$id",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$recursiveAnchor",
      "/additionalItems/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$recursiveRef",
      "/additionalItems/$recursiveRef",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);

  // Anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "https://example.com", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/additionalItems/$recursiveRef",
                           "https://example.com", "https://example.com",
                           std::nullopt, "#");
}

TEST(JSONSchema_frame_2019_09, recursive_ref_nested_recursive_anchor_true) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalItems": {
      "$recursiveAnchor": true,
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame,
      "https://www.sourcemeta.com/schema#/additionalItems/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveAnchor",
      "/additionalItems", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "/additionalItems", false);

  // Anchors

  EXPECT_FRAME_DYNAMIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems", "", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/additionalItems/$recursiveRef",
                           "https://www.sourcemeta.com/schema",
                           "https://www.sourcemeta.com/schema", std::nullopt,
                           "#");
}

TEST(JSONSchema_frame_2019_09, recursive_ref_multiple_recursive_anchor_true) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": true,
    "additionalItems": {
      "$id": "nested",
      "$recursiveAnchor": true,
      "$recursiveRef": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 14);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/nested",
      "https://www.sourcemeta.com/schema", "/additionalItems",
      "https://www.sourcemeta.com/nested", "", "", false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/$recursiveAnchor", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/additionalItems",
      "https://www.sourcemeta.com/schema", "/additionalItems",
      "https://www.sourcemeta.com/nested", "", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/additionalItems/$id",
      "https://www.sourcemeta.com/schema", "/additionalItems/$id",
      "https://www.sourcemeta.com/nested", "/$id", "/additionalItems", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame,
      "https://www.sourcemeta.com/schema#/additionalItems/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveAnchor",
      "https://www.sourcemeta.com/nested", "/$recursiveAnchor",
      "/additionalItems", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/nested", "/$recursiveRef", "/additionalItems",
      false);

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/nested#/$id",
      "https://www.sourcemeta.com/schema", "/additionalItems/$id",
      "https://www.sourcemeta.com/nested", "/$id", "/additionalItems", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/nested#/$recursiveAnchor",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveAnchor",
      "https://www.sourcemeta.com/nested", "/$recursiveAnchor",
      "/additionalItems", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/nested#/$recursiveRef",
      "https://www.sourcemeta.com/schema", "/additionalItems/$recursiveRef",
      "https://www.sourcemeta.com/nested", "/$recursiveRef", "/additionalItems",
      false);

  // Anchors

  EXPECT_FRAME_DYNAMIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);
  EXPECT_FRAME_DYNAMIC_2019_09_ANCHOR(
      frame, "https://www.sourcemeta.com/nested",
      "https://www.sourcemeta.com/schema", "/additionalItems",
      "https://www.sourcemeta.com/nested", "", "", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/additionalItems/$recursiveRef",
                           "https://www.sourcemeta.com/nested",
                           "https://www.sourcemeta.com/nested", std::nullopt,
                           "#");
}

TEST(JSONSchema_frame_2019_09, recursive_anchor_conflict) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": true,
    "items": {
      "$recursiveAnchor": true
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_2019_09, invalid_recursive_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveRef": "nested#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaReferenceError &error) {
    EXPECT_EQ(error.identifier(), "https://www.sourcemeta.com/schema");
    EXPECT_EQ(sourcemeta::core::to_string(error.location()), "/$recursiveRef");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2019_09, recursive_anchor_on_relative_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$recursiveAnchor": true,
    "additionalItems": {
      "$id": "middle",
      "$recursiveAnchor": true
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 11);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "middle", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$recursiveAnchor", "/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalItems", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$id", "/additionalItems/$id",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalItems/$recursiveAnchor",
      "/additionalItems/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "middle#/$recursiveAnchor", "/additionalItems/$recursiveAnchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/additionalItems", false);

  // Anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "middle", "/additionalItems",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, ref_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "#/$defs/string",
    "$defs": {
      "string": { "type": "string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs/string/type",
      "https://www.sourcemeta.com/schema", "/$defs/string/type",
      "https://www.sourcemeta.com/schema", "/$defs/string/type",
      "/$defs/string", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", "#/$defs/string");
}

TEST(JSONSchema_frame_2019_09, ref_from_definitions) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "#/definitions/middle",
    "definitions": {
      "middle": { "$ref": "#/definitions/string" },
      "string": { "type": "string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions",
      "https://www.sourcemeta.com/schema", "/definitions",
      "https://www.sourcemeta.com/schema", "/definitions", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/definitions/middle",
      "https://www.sourcemeta.com/schema", "/definitions/middle",
      "https://www.sourcemeta.com/schema", "/definitions/middle", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions/middle/$ref",
      "https://www.sourcemeta.com/schema", "/definitions/middle/$ref",
      "https://www.sourcemeta.com/schema", "/definitions/middle/$ref",
      "/definitions/middle", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/definitions/string",
      "https://www.sourcemeta.com/schema", "/definitions/string",
      "https://www.sourcemeta.com/schema", "/definitions/string", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions/string/type",
      "https://www.sourcemeta.com/schema", "/definitions/string/type",
      "https://www.sourcemeta.com/schema", "/definitions/string/type",
      "/definitions/string", false);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/definitions/middle",
      "https://www.sourcemeta.com/schema", "/definitions/middle",
      "#/definitions/middle");
  EXPECT_STATIC_REFERENCE(
      frame, "/definitions/middle/$ref",
      "https://www.sourcemeta.com/schema#/definitions/string",
      "https://www.sourcemeta.com/schema", "/definitions/string",
      "#/definitions/string");
}

TEST(JSONSchema_frame_2019_09, relative_base_uri_without_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$id": "common"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(frame, "common", "common", "", "common",
                                       "", std::nullopt, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "common#/$schema", "common",
                                      "/$schema", "common", "/$schema", "",
                                      false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "common#/$id", "common", "/$id",
                                      "common", "/$id", "", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09, relative_base_uri_with_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$id": "common",
    "allOf": [ { "$ref": "#foo" } ],
    "$defs": {
      "foo": {
        "$anchor": "foo"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_FRAME_STATIC_2019_09_RESOURCE(frame, "common", "common", "", "common",
                                       "", std::nullopt, false);

  // Anchors
  EXPECT_FRAME_STATIC_2019_09_ANCHOR(frame, "common#foo", "common",
                                     "/$defs/foo", "common", "/$defs/foo", "",
                                     false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "common#/$schema", "common",
                                      "/$schema", "common", "/$schema", "",
                                      false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "common#/$id", "common", "/$id",
                                      "common", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "common#/allOf", "common",
                                      "/allOf", "common", "/allOf", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(frame, "common#/allOf/0", "common",
                                        "/allOf/0", "common", "/allOf/0", "",
                                        false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "common#/allOf/0/$ref", "common",
                                      "/allOf/0/$ref", "common",
                                      "/allOf/0/$ref", "/allOf/0", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "common#/$defs", "common",
                                      "/$defs", "common", "/$defs", "", false);
  EXPECT_FRAME_STATIC_2019_09_SUBSCHEMA(frame, "common#/$defs/foo", "common",
                                        "/$defs/foo", "common", "/$defs/foo",
                                        "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(
      frame, "common#/$defs/foo/$anchor", "common", "/$defs/foo/$anchor",
      "common", "/$defs/foo/$anchor", "/$defs/foo", false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(frame, "/allOf/0/$ref", "common#foo", "common", "foo",
                          "#foo");
}

TEST(JSONSchema_frame_2019_09, relative_id_leading_slash) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "/base",
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2019_09_RESOURCE(frame, "/base", "/base", "", "/base", "",
                                       std::nullopt, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "/base#/$id", "/base", "/$id",
                                      "/base", "/$id", "", false);
  EXPECT_FRAME_STATIC_2019_09_POINTER(frame, "/base#/$schema", "/base",
                                      "/$schema", "/base", "/$schema", "",
                                      false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame_2019_09,
     ref_does_not_invalidate_sibling_subschemas_and_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$ref": "#/definitions/enabled",
        "properties": {
          "bar": {
            "$ref": "#/definitions/config",
            "additionalProperties": {
              "$ref": "#/definitions/threshold"
            }
          }
        }
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$ref", "/properties/foo/$ref",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/properties/foo", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties", "/properties/foo/properties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/properties/foo", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo/properties/bar",
      "/properties/foo/properties/bar",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/properties/foo", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties/bar/$ref",
      "/properties/foo/properties/bar/$ref",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/properties/foo/properties/bar", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo/properties/bar/additionalProperties",
      "/properties/foo/properties/bar/additionalProperties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/properties/foo/properties/bar", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties/bar/additionalProperties/$ref",
      "/properties/foo/properties/bar/additionalProperties/$ref",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/properties/foo/properties/bar/additionalProperties", false);

  EXPECT_EQ(frame.references().size(), 4);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref",
                          "#/definitions/enabled", "", "/definitions/enabled",
                          "#/definitions/enabled");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/properties/bar/$ref",
                          "#/definitions/config", "", "/definitions/config",
                          "#/definitions/config");
  EXPECT_STATIC_REFERENCE(
      frame, "/properties/foo/properties/bar/additionalProperties/$ref",
      "#/definitions/threshold", "", "/definitions/threshold",
      "#/definitions/threshold");
}

TEST(JSONSchema_frame_2019_09, propertyNames_with_nested_applicators) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$anchor": "outer",
    "propertyNames": {
      "$anchor": "inner",
      "anyOf": [
        { "minLength": 1 },
        { "maxLength": 10 },
        { "$anchor": "nested" }
      ],
      "additionalProperties": {
        "$anchor": "other",
        "type": "boolean"
      },
      "$defs": {
        "test": {
          "type": "string"
        }
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 22);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/propertyNames", "/propertyNames",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/propertyNames/anyOf/0", "/propertyNames/anyOf/0",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/propertyNames/anyOf/1", "/propertyNames/anyOf/1",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/propertyNames/anyOf/2", "/propertyNames/anyOf/2",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/propertyNames/additionalProperties",
      "/propertyNames/additionalProperties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/propertyNames/$defs/test", "/propertyNames/$defs/test",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", false);

  // Anchors

  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#outer", "", "https://json-schema.org/draft/2019-09/schema",
      JSON_Schema_2019_09, std::nullopt, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#inner", "/propertyNames",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      true);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#nested", "/propertyNames/anyOf/2",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#other", "/propertyNames/additionalProperties",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", false);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$anchor", "/$anchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09, "",
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/$anchor", "/propertyNames/$anchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/anyOf", "/propertyNames/anyOf",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/anyOf/0/minLength",
      "/propertyNames/anyOf/0/minLength",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames/anyOf/0", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/anyOf/1/maxLength",
      "/propertyNames/anyOf/1/maxLength",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames/anyOf/1", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/anyOf/2/$anchor",
      "/propertyNames/anyOf/2/$anchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames/anyOf/2", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/additionalProperties/$anchor",
      "/propertyNames/additionalProperties/$anchor",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames/additionalProperties", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/additionalProperties/type",
      "/propertyNames/additionalProperties/type",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames/additionalProperties", false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/$defs", "/propertyNames/$defs",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames", true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/propertyNames/$defs/test/type",
      "/propertyNames/$defs/test/type",
      "https://json-schema.org/draft/2019-09/schema", JSON_Schema_2019_09,
      "/propertyNames/$defs/test", false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}
