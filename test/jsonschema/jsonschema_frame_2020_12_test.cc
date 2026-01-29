#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

#define EXPECT_FRAME_STATIC_2020_12_POINTER(                                   \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name,        \
    expected_orphan)                                                           \
  EXPECT_FRAME_STATIC_POINTER(frame, reference, root_id, expected_pointer,     \
                              "https://json-schema.org/draft/2020-12/schema",  \
                              JSON_Schema_2020_12, expected_base,              \
                              expected_relative_pointer, expected_parent,      \
                              expected_property_name, expected_orphan);

#define EXPECT_FRAME_STATIC_2020_12_RESOURCE(                                  \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name,        \
    expected_orphan)                                                           \
  EXPECT_FRAME_STATIC_RESOURCE(frame, reference, root_id, expected_pointer,    \
                               "https://json-schema.org/draft/2020-12/schema", \
                               JSON_Schema_2020_12, expected_base,             \
                               expected_relative_pointer, expected_parent,     \
                               expected_property_name, expected_orphan);

#define EXPECT_FRAME_STATIC_2020_12_ANCHOR(                                    \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name,        \
    expected_orphan)                                                           \
  EXPECT_FRAME_STATIC_ANCHOR(frame, reference, root_id, expected_pointer,      \
                             "https://json-schema.org/draft/2020-12/schema",   \
                             JSON_Schema_2020_12, expected_base,               \
                             expected_relative_pointer, expected_parent,       \
                             expected_property_name, expected_orphan);

#define EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(                                 \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name,        \
    expected_orphan)                                                           \
  EXPECT_FRAME_STATIC_SUBSCHEMA(                                               \
      frame, reference, root_id, expected_pointer,                             \
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,     \
      expected_base, expected_relative_pointer, expected_parent,               \
      expected_property_name, expected_orphan);

#define EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(                                   \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name,        \
    expected_orphan)                                                           \
  EXPECT_FRAME_DYNAMIC_ANCHOR(frame, reference, root_id, expected_pointer,     \
                              "https://json-schema.org/draft/2020-12/schema",  \
                              JSON_Schema_2020_12, expected_base,              \
                              expected_relative_pointer, expected_parent,      \
                              expected_property_name, expected_orphan);

TEST(JSONSchema_frame_2020_12, anonymous_with_nested_schema_resource) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "additionalProperties": { "$id": "https://example.com" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/additionalProperties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/additionalProperties/$id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/additionalProperties", false, false);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalProperties", "/additionalProperties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalProperties/$id", "/additionalProperties/$id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/additionalProperties", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/additionalProperties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/additionalProperties/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://example.com");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://example.com#/$id");
}

TEST(JSONSchema_frame_2020_12, empty_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
}

TEST(JSONSchema_frame_2020_12, empty_schema_trailing_hash) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema#",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
}

TEST(JSONSchema_frame_2020_12, one_level_applicators_without_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/schema", "/items/type", "/items", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "/properties/foo", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/items/type");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/type");
}

TEST(JSONSchema_frame_2020_12, one_level_applicators_with_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test/qux",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/test/qux",
      "https://www.sourcemeta.com/test/qux", "",
      "https://www.sourcemeta.com/test/qux", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", "", false, false);

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/test/qux#test",
      "https://www.sourcemeta.com/test/qux", "/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo", "", false,
      false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$id",
      "https://www.sourcemeta.com/test/qux", "/$id",
      "https://www.sourcemeta.com/test/qux", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test/qux#/items",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/items", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties",
      "https://www.sourcemeta.com/test/qux", "/properties",
      "https://www.sourcemeta.com/test/qux", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo", "", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo/$anchor",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/$anchor",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo/type",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/type",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/type",
      "/properties/foo", false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/items", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test/qux");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test/qux#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test/qux#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/test/qux#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test/qux#/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/test/qux#/items/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/test/qux#/items/type");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/test/qux#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/test/qux#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/test/qux#/properties/foo/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/test/qux#/properties/foo/type");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#/type");
}

TEST(JSONSchema_frame_2020_12, subschema_absolute_identifier) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", "", false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/$id",
      "https://www.sourcemeta.com/schema", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/items", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/items/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/items/type");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#/type");
}

TEST(JSONSchema_frame_2020_12, nested_schemas) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/foo#test",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/baz", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/baz#extra",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", "/properties/baz", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/qux",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", "/properties/foo", false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);

  // foo
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/properties/foo", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/foo", "/$anchor", "/properties/foo", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/items",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/items/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", "/properties/foo/items", false,
      false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/$id",
      "https://www.sourcemeta.com/foo", "/$id", "/properties/foo", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/foo", "/$anchor", "/properties/foo", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/foo#/items",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/items/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", "/properties/foo/items", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/qux#/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", "/properties/foo/items", false,
      false);

  // bar
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", "/properties/bar", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", "/properties/bar", false,
      false);

  // baz
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/baz", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/$id",
      "https://www.sourcemeta.com/schema", "/properties/baz/$id",
      "https://www.sourcemeta.com/baz", "/$id", "/properties/baz", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", "/properties/baz", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/baz", "/items/$anchor",
      "/properties/baz/items", false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/baz#/$id",
      "https://www.sourcemeta.com/schema", "/properties/baz/$id",
      "https://www.sourcemeta.com/baz", "/$id", "/properties/baz", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/baz#/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", "/properties/baz", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/baz#/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/baz", "/items/$anchor",
      "/properties/baz/items", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/baz");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/baz#extra");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/qux");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/items/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#/items");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo#/items/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/qux#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/bar#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/baz");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/baz/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/baz/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/baz/items/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/baz#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/baz#/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/baz#/items/$anchor");
}

TEST(JSONSchema_frame_2020_12, id_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$id": "schema" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_2020_12, static_anchor_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo",
    "items": { "$anchor": "foo" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_2020_12, explicit_argument_id_same) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2020-12/schema",
                "https://www.sourcemeta.com/schema");

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
}

TEST(JSONSchema_frame_2020_12, anchor_top_level) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$anchor",
      "https://www.sourcemeta.com/schema", "/$anchor",
      "https://www.sourcemeta.com/schema", "/$anchor", "", false, false);

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#foo");
}

TEST(JSONSchema_frame_2020_12, explicit_argument_id_different) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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
                "https://json-schema.org/draft/2020-12/schema",
                "https://www.example.com");

  EXPECT_EQ(frame.locations().size(), 38);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com", "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.test.com", "https://www.sourcemeta.com/schema",
      "/properties/two", "https://www.test.com", "", "", false, false);

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.example.com#foo", "https://www.sourcemeta.com/schema",
      "/items", "https://www.example.com", "/items", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/test#bar",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.example.com/test#bar",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.test.com#baz", "https://www.sourcemeta.com/schema",
      "/properties/two", "https://www.test.com", "", "", false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/$anchor",
      "https://www.sourcemeta.com/schema", "/items/$anchor",
      "https://www.sourcemeta.com/schema", "/items/$anchor", "/items", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/one",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.sourcemeta.com/test", "/$id", "/properties/one", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/one/$anchor",
      "https://www.sourcemeta.com/test", "/$anchor", "/properties/one", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/two",
      "https://www.sourcemeta.com/schema", "/properties/two",
      "https://www.test.com", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$id",
      "https://www.sourcemeta.com/schema", "/properties/two/$id",
      "https://www.test.com", "/$id", "/properties/two", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/two/$anchor",
      "https://www.test.com", "/$anchor", "/properties/two", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test#/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.sourcemeta.com/test", "/$id", "/properties/one", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/one/$anchor",
      "https://www.sourcemeta.com/test", "/$anchor", "/properties/one", false,
      false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.test.com#/$id", "https://www.sourcemeta.com/schema",
      "/properties/two/$id", "https://www.test.com", "/$id", "/properties/two",
      false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.test.com#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/two/$anchor",
      "https://www.test.com", "/$anchor", "/properties/two", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.example.com");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.test.com");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.example.com#foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test#bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com/test#bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.test.com#baz");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/items/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/one");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/one/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/one/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/two");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/two/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/two/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test#/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.test.com#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.test.com#/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/items");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/items/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/properties/one");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/one/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/one/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/properties/two");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/two/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/two/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com/test#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com/test#/$anchor");
}

TEST(JSONSchema_frame_2020_12, dynamic_refs_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$dynamicRef": "#" },
      "bar": { "$dynamicRef": "#/properties/baz" },
      "baz": {
        "$anchor": "baz",
        "type": "string"
      },
      "qux": {
        "$id": "test",
        "$dynamicRef": "#"
      },
      "anchor": {
        "$dynamicRef": "#baz"
      },
      "dynamic": {
        "$dynamicAnchor": "dynamic"
      },
      "extra": {
        "$dynamicRef": "#dynamic"
      },
      "unknown": {
        "$dynamicRef": "foo#xxx"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 28);

  // Resources

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/test",
      "https://www.sourcemeta.com/schema", "/properties/qux",
      "https://www.sourcemeta.com/test", "", "", false, false);

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#dynamic",
      "https://www.sourcemeta.com/schema", "/properties/dynamic",
      "https://www.sourcemeta.com/schema", "/properties/dynamic", "", false,
      false);
  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#dynamic",
      "https://www.sourcemeta.com/schema", "/properties/dynamic",
      "https://www.sourcemeta.com/schema", "/properties/dynamic", "", false,
      false);

  // Subschemas

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/qux",
      "https://www.sourcemeta.com/schema", "/properties/qux",
      "https://www.sourcemeta.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/anchor",
      "https://www.sourcemeta.com/schema", "/properties/anchor",
      "https://www.sourcemeta.com/schema", "/properties/anchor", "", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/dynamic",
      "https://www.sourcemeta.com/schema", "/properties/dynamic",
      "https://www.sourcemeta.com/schema", "/properties/dynamic", "", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/extra",
      "https://www.sourcemeta.com/schema", "/properties/extra",
      "https://www.sourcemeta.com/schema", "/properties/extra", "", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/unknown",
      "https://www.sourcemeta.com/schema", "/properties/unknown",
      "https://www.sourcemeta.com/schema", "/properties/unknown", "", false,
      false);

  // Pointers (from root base URI)

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicRef",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/bar/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/bar/$dynamicRef",
      "/properties/bar", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/$anchor",
      "/properties/baz", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/type",
      "https://www.sourcemeta.com/schema", "/properties/baz/type",
      "https://www.sourcemeta.com/schema", "/properties/baz/type",
      "/properties/baz", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/qux/$id",
      "https://www.sourcemeta.com/schema", "/properties/qux/$id",
      "https://www.sourcemeta.com/test", "/$id", "/properties/qux", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/qux/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/qux/$dynamicRef",
      "https://www.sourcemeta.com/test", "/$dynamicRef", "/properties/qux",
      false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/anchor/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/anchor/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/anchor/$dynamicRef",
      "/properties/anchor", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame,
      "https://www.sourcemeta.com/schema#/properties/dynamic/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/dynamic/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/dynamic/$dynamicAnchor",
      "/properties/dynamic", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/extra/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/extra/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/extra/$dynamicRef",
      "/properties/extra", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame,
      "https://www.sourcemeta.com/schema#/properties/unknown/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/unknown/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/unknown/$dynamicRef",
      "/properties/unknown", false, false);

  // Pointers (from nested base URI)

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test#/$id",
      "https://www.sourcemeta.com/schema", "/properties/qux/$id",
      "https://www.sourcemeta.com/test", "/$id", "/properties/qux", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test#/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/qux/$dynamicRef",
      "https://www.sourcemeta.com/test", "/$dynamicRef", "/properties/qux",
      false, false);

  // References

  EXPECT_EQ(frame.references().size(), 7);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/properties/foo/$dynamicRef", "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$dynamicRef",
                          "https://www.sourcemeta.com/schema#/properties/baz",
                          "https://www.sourcemeta.com/schema",
                          "/properties/baz", "#/properties/baz");
  EXPECT_STATIC_REFERENCE(frame, "/properties/qux/$dynamicRef",
                          "https://www.sourcemeta.com/test",
                          "https://www.sourcemeta.com/test", std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/anchor/$dynamicRef",
                          "https://www.sourcemeta.com/schema#baz",
                          "https://www.sourcemeta.com/schema", "baz", "#baz");
  EXPECT_DYNAMIC_REFERENCE(frame, "/properties/extra/$dynamicRef",
                           "https://www.sourcemeta.com/schema#dynamic",
                           "https://www.sourcemeta.com/schema", "dynamic",
                           "#dynamic");
  EXPECT_DYNAMIC_REFERENCE(frame, "/properties/unknown/$dynamicRef",
                           "https://www.sourcemeta.com/foo#xxx",
                           "https://www.sourcemeta.com/foo", "xxx", "foo#xxx");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#baz");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#dynamic");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic,
                                  "https://www.sourcemeta.com/schema#dynamic");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/baz");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/qux");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/dynamic");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/extra");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/unknown");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/bar/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/baz/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/baz/type");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/qux/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/qux/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/anchor/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/dynamic/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/extra/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/unknown/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/test#/$dynamicRef");
}

TEST(JSONSchema_frame_2020_12, dynamic_refs_with_no_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$dynamicRef": "#" },
      "bar": { "$dynamicRef": "#/properties/baz" },
      "baz": {
        "$anchor": "baz",
        "type": "string"
      },
      "qux": {
        "$id": "https://www.example.com",
        "$dynamicRef": "#"
      },
      "anchor": {
        "$dynamicRef": "#baz"
      },
      "dynamic": {
        "$dynamicAnchor": "dynamic"
      },
      "extra": {
        "$dynamicRef": "#dynamic"
      },
      "unknown": {
        "$dynamicRef": "https://www.example.com/foo#xxx"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 27);

  // Resources

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com", "", "/properties/qux",
      "https://www.example.com", "", "", false, false);

  // Anchors

  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#baz", "/properties/baz",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#dynamic", "/properties/dynamic",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#dynamic", "/properties/dynamic",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/bar", "/properties/bar",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/baz", "/properties/baz",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/qux", "/properties/qux",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/anchor", "/properties/anchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/dynamic", "/properties/dynamic",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/extra", "/properties/extra",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/unknown", "/properties/unknown",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);

  // Pointers (anonymous)

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$dynamicRef", "/properties/foo/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/bar/$dynamicRef", "/properties/bar/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/bar", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/baz/$anchor", "/properties/baz/$anchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/baz", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/baz/type", "/properties/baz/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/baz", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/qux/$id", "/properties/qux/$id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/qux", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/qux/$dynamicRef", "/properties/qux/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/qux", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/anchor/$dynamicRef",
      "/properties/anchor/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/anchor", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/dynamic/$dynamicAnchor",
      "/properties/dynamic/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/dynamic", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/extra/$dynamicRef", "/properties/extra/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/extra", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/unknown/$dynamicRef",
      "/properties/unknown/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/unknown", false, false);

  // Pointers (from nested $id base URI)

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/$id", "", "/properties/qux/$id",
      "https://www.example.com", "/$id", "/properties/qux", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/$dynamicRef", "",
      "/properties/qux/$dynamicRef", "https://www.example.com", "/$dynamicRef",
      "/properties/qux", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 7);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$dynamicRef", "", "",
                          std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$dynamicRef",
                          "#/properties/baz", "", "/properties/baz",
                          "#/properties/baz");
  EXPECT_STATIC_REFERENCE(frame, "/properties/qux/$dynamicRef",
                          "https://www.example.com", "https://www.example.com",
                          std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/anchor/$dynamicRef", "#baz", "",
                          "baz", "#baz");
  EXPECT_DYNAMIC_REFERENCE(frame, "/properties/extra/$dynamicRef", "#dynamic",
                           "", "dynamic", "#dynamic");
  EXPECT_DYNAMIC_REFERENCE(frame, "/properties/unknown/$dynamicRef",
                           "https://www.example.com/foo#xxx",
                           "https://www.example.com/foo", "xxx",
                           "https://www.example.com/foo#xxx");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.example.com");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#baz");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#dynamic");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic, "#dynamic");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/baz");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/qux");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/dynamic");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/extra");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/unknown");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/foo/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/bar/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/baz/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/baz/type");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/qux/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/qux/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/anchor/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/dynamic/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/extra/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/unknown/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/$dynamicRef");
}

TEST(JSONSchema_frame_2020_12, ref_to_dynamic_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$anchor": "foo"
      },
      "bar": {
        "$ref": "#foo"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);

  // Resources

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // Subschemas

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar", "", false, false);

  // Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref",
      "/properties/bar", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$ref",
                          "https://www.sourcemeta.com/schema#foo",
                          "https://www.sourcemeta.com/schema", "foo", "#foo");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar/$ref");
}

TEST(JSONSchema_frame_2020_12, different_dynamic_and_refs_in_same_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#/properties/bar",
        "$dynamicRef": "#"
      },
      "bar": {
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  // Resources

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // Subschemas

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar", "", false, false);

  // Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicRef",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/type",
      "https://www.sourcemeta.com/schema", "/properties/bar/type",
      "https://www.sourcemeta.com/schema", "/properties/bar/type",
      "/properties/bar", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref",
                          "https://www.sourcemeta.com/schema#/properties/bar",
                          "https://www.sourcemeta.com/schema",
                          "/properties/bar", "#/properties/bar");
  EXPECT_STATIC_REFERENCE(
      frame, "/properties/foo/$dynamicRef", "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", std::nullopt, "#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar/type");
}

TEST(JSONSchema_frame_2020_12, same_dynamic_and_refs_in_same_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#/properties/bar",
        "$dynamicRef": "#/properties/bar"
      },
      "bar": {
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  // Resources

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // Subschemas

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar", "", false, false);

  // Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicRef",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicRef",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/type",
      "https://www.sourcemeta.com/schema", "/properties/bar/type",
      "https://www.sourcemeta.com/schema", "/properties/bar/type",
      "/properties/bar", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref",
                          "https://www.sourcemeta.com/schema#/properties/bar",
                          "https://www.sourcemeta.com/schema",
                          "/properties/bar", "#/properties/bar");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$dynamicRef",
                          "https://www.sourcemeta.com/schema#/properties/bar",
                          "https://www.sourcemeta.com/schema",
                          "/properties/bar", "#/properties/bar");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar/type");
}

TEST(JSONSchema_frame_2020_12, dynamic_anchor_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicAnchor": "foo",
    "properties": {
      "foo": {
        "$dynamicAnchor": "test"
      },
      "bar": {
        "$id": "bar",
        "$dynamicAnchor": "test",
        "$anchor": "test"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 21);

  // Dynamic anchors

  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#test",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/bar#test",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", "", false, false);

  // Static anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#test",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/bar#test",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", "", false, false);

  // Static identifiers

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", "", false, false);

  // Static pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/$dynamicAnchor", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicAnchor",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", "/properties/bar", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/$dynamicAnchor",
      "https://www.sourcemeta.com/bar", "/$dynamicAnchor", "/properties/bar",
      false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/$anchor",
      "https://www.sourcemeta.com/bar", "/$anchor", "/properties/bar", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", "/properties/bar", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/$dynamicAnchor",
      "https://www.sourcemeta.com/bar", "/$dynamicAnchor", "/properties/bar",
      false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/$anchor",
      "https://www.sourcemeta.com/bar", "/$anchor", "/properties/bar", false,
      false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic,
                                  "https://www.sourcemeta.com/schema#foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic,
                                  "https://www.sourcemeta.com/schema#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic,
                                  "https://www.sourcemeta.com/bar#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/bar#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/bar/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/bar/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/bar#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/bar#/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/bar#/$anchor");
}

TEST(JSONSchema_frame_2020_12, dynamic_anchor_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$dynamicAnchor": "test"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  // Dynamic anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#test", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#test", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);

  // Static frames

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$dynamicAnchor",
      "/properties/foo/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic, "#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/foo/$dynamicAnchor");
}

TEST(JSONSchema_frame_2020_12,
     dynamic_ref_to_single_dynamic_anchor_standalone) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicRef": "#test",
    "$defs": {
      "test": {
        "$dynamicAnchor": "test"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 8);

  // Dynamic anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // Static frames

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$dynamicRef", "/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/test/$dynamicAnchor", "/$defs/test/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/test", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$dynamicRef", "#test", "", "test", "#test");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic, "#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/test/$dynamicAnchor");
}

TEST(JSONSchema_frame_2020_12, dynamic_ref_to_single_dynamic_anchor_external) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicRef": "#test",
    "$defs": {
      "test": {
        "$dynamicAnchor": "test"
      },
      "foo": {
        "$ref": "https://sourcemeta.com"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  // Dynamic anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // Static frames

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$dynamicRef", "/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/test/$dynamicAnchor", "/$defs/test/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/test", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/foo", "/$defs/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/foo/$ref", "/$defs/foo/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/foo", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/$dynamicRef", "#test", "", "test", "#test");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/foo/$ref", "https://sourcemeta.com",
                          "https://sourcemeta.com", std::nullopt,
                          "https://sourcemeta.com");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic, "#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/test/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/foo");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/foo/$ref");
}

TEST(JSONSchema_frame_2020_12, dynamic_anchor_same_on_schema_resource) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicAnchor": "foo",
    "items": {
      "$dynamicAnchor": "foo"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_2020_12, no_id_recursive_empty_pointer) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$ref", "/properties/foo/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref", "", "", std::nullopt,
                          "#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo/$ref");
}

TEST(JSONSchema_frame_2020_12, ref_metaschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$ref");
}

TEST(JSONSchema_frame_2020_12, location_independent_identifier_anonymous) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameError &error) {
    EXPECT_EQ(error.identifier(), "#foo");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, ref_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", "", false, true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs/string/type",
      "https://www.sourcemeta.com/schema", "/$defs/string/type",
      "https://www.sourcemeta.com/schema", "/$defs/string/type",
      "/$defs/string", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", "#/$defs/string");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/string");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/string/type");
}

TEST(JSONSchema_frame_2020_12, ref_from_definitions) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions",
      "https://www.sourcemeta.com/schema", "/definitions",
      "https://www.sourcemeta.com/schema", "/definitions", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/definitions/middle",
      "https://www.sourcemeta.com/schema", "/definitions/middle",
      "https://www.sourcemeta.com/schema", "/definitions/middle", "", false,
      true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions/middle/$ref",
      "https://www.sourcemeta.com/schema", "/definitions/middle/$ref",
      "https://www.sourcemeta.com/schema", "/definitions/middle/$ref",
      "/definitions/middle", false, true);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/definitions/string",
      "https://www.sourcemeta.com/schema", "/definitions/string",
      "https://www.sourcemeta.com/schema", "/definitions/string", "", false,
      true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions/string/type",
      "https://www.sourcemeta.com/schema", "/definitions/string/type",
      "https://www.sourcemeta.com/schema", "/definitions/string/type",
      "/definitions/string", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/definitions/middle",
      "https://www.sourcemeta.com/schema", "/definitions/middle",
      "#/definitions/middle");
  EXPECT_STATIC_REFERENCE(
      frame, "/definitions/middle/$ref",
      "https://www.sourcemeta.com/schema#/definitions/string",
      "https://www.sourcemeta.com/schema", "/definitions/string",
      "#/definitions/string");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/definitions");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/definitions/middle");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/definitions/middle/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/definitions/string");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/definitions/string/type");
}

TEST(JSONSchema_frame_2020_12, relative_base_uri_without_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "common"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(frame, "common", "common", "", "common",
                                       "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$schema", "common",
                                      "/$schema", "common", "/$schema", "",
                                      false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$id", "common", "/$id",
                                      "common", "/$id", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/$id");
}

TEST(JSONSchema_frame_2020_12, relative_base_uri_with_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(frame, "common", "common", "", "common",
                                       "", std::nullopt, false, false);

  // Anchors
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(frame, "common#foo", "common",
                                     "/$defs/foo", "common", "/$defs/foo", "",
                                     false, true);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$schema", "common",
                                      "/$schema", "common", "/$schema", "",
                                      false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$id", "common", "/$id",
                                      "common", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/allOf", "common",
                                      "/allOf", "common", "/allOf", "", false,
                                      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(frame, "common#/allOf/0", "common",
                                        "/allOf/0", "common", "/allOf/0", "",
                                        false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "common#/allOf/0/$ref", "common", "/allOf/0/$ref", "common",
      "/allOf/0/$ref", "/allOf/0", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$defs", "common",
                                      "/$defs", "common", "/$defs", "", false,
                                      false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(frame, "common#/$defs/foo", "common",
                                        "/$defs/foo", "common", "/$defs/foo",
                                        "", false, true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "common#/$defs/foo/$anchor", "common", "/$defs/foo/$anchor",
      "common", "/$defs/foo/$anchor", "/$defs/foo", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/allOf/0/$ref", "common#foo", "common", "foo",
                          "#foo");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/allOf");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/allOf/0");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/allOf/0/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/$defs/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "common#/$defs/foo/$anchor");
}

TEST(JSONSchema_frame_2020_12, relative_base_with_relative_path_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "foo/bar/baz",
    "$ref": "qux"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 4);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(frame, "foo/bar/baz", "foo/bar/baz", "",
                                       "foo/bar/baz", "", std::nullopt, false,
                                       false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "foo/bar/baz#/$schema",
                                      "foo/bar/baz", "/$schema", "foo/bar/baz",
                                      "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "foo/bar/baz#/$id", "foo/bar/baz",
                                      "/$id", "foo/bar/baz", "/$id", "", false,
                                      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "foo/bar/baz#/$ref", "foo/bar/baz",
                                      "/$ref", "foo/bar/baz", "/$ref", "",
                                      false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "foo/bar/qux", "foo/bar/qux",
                          std::nullopt, "qux");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "foo/bar/baz");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "foo/bar/baz#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "foo/bar/baz#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "foo/bar/baz#/$ref");
}

TEST(JSONSchema_frame_2020_12, idempotent_with_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/$defs/string",
    "$defs": {
      "string": {}
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", "", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", "#/$defs/string");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/string");
}

TEST(JSONSchema_frame_2020_12, allof_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "allOf": [
      { "$ref": "https://json-schema.org/draft/2020-12/schema" },
      { "$ref": "https://json-schema.org/draft/2020-12/meta/hyper-schema" }
    ]
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/allOf/0", "/allOf/0",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/allOf/1", "/allOf/1",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);

  // Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/allOf", "/allOf",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/allOf/0/$ref", "/allOf/0/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/allOf/0", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/allOf/1/$ref", "/allOf/1/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/allOf/1", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/allOf/0/$ref", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/allOf/1/$ref",
      "https://json-schema.org/draft/2020-12/meta/hyper-schema",
      "https://json-schema.org/draft/2020-12/meta/hyper-schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/meta/hyper-schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/allOf");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/allOf/0");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/allOf/0/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/allOf/1");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/allOf/1/$ref");
}

TEST(JSONSchema_frame_2020_12, properties_with_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "type": "string"
      },
      "bar": {
        "$ref": "#/properties/foo"
      },
      "baz": {
        "items": {
          "$ref": "#/properties/bar"
        }
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 11);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "/properties/foo", false, false);

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref",
      "/properties/bar", false, false);

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "/properties/baz", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items/$ref",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$ref",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$ref",
      "/properties/baz/items", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$ref",
                          "https://www.sourcemeta.com/schema#/properties/foo",
                          "https://www.sourcemeta.com/schema",
                          "/properties/foo", "#/properties/foo");
  EXPECT_STATIC_REFERENCE(frame, "/properties/baz/items/$ref",
                          "https://www.sourcemeta.com/schema#/properties/bar",
                          "https://www.sourcemeta.com/schema",
                          "/properties/bar", "#/properties/bar");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/type");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/baz");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/baz/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/baz/items/$ref");
}

TEST(JSONSchema_frame_2020_12, property_ref_defs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#/$defs/helper"
      },
      "bar": {
        "$ref": "#/$defs/helper/items"
      }
    },
    "$defs": {
      "helper": {
        "items": {
          "additionalProperties": {
            "type": "string"
          }
        }
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 13);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref",
      "/properties/bar", false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/helper",
      "https://www.sourcemeta.com/schema", "/$defs/helper",
      "https://www.sourcemeta.com/schema", "/$defs/helper", "", false, true);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/helper/items",
      "https://www.sourcemeta.com/schema", "/$defs/helper/items",
      "https://www.sourcemeta.com/schema", "/$defs/helper/items",
      "/$defs/helper", false, true);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame,
      "https://www.sourcemeta.com/schema#/$defs/helper/items/"
      "additionalProperties",
      "https://www.sourcemeta.com/schema",
      "/$defs/helper/items/additionalProperties",
      "https://www.sourcemeta.com/schema",
      "/$defs/helper/items/additionalProperties", "/$defs/helper/items", false,
      true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame,
      "https://www.sourcemeta.com/schema#/$defs/helper/items/"
      "additionalProperties/type",
      "https://www.sourcemeta.com/schema",
      "/$defs/helper/items/additionalProperties/type",
      "https://www.sourcemeta.com/schema",
      "/$defs/helper/items/additionalProperties/type",
      "/$defs/helper/items/additionalProperties", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref",
                          "https://www.sourcemeta.com/schema#/$defs/helper",
                          "https://www.sourcemeta.com/schema", "/$defs/helper",
                          "#/$defs/helper");
  EXPECT_STATIC_REFERENCE(
      frame, "/properties/bar/$ref",
      "https://www.sourcemeta.com/schema#/$defs/helper/items",
      "https://www.sourcemeta.com/schema", "/$defs/helper/items",
      "#/$defs/helper/items");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/helper");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/helper/items");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs/"
                                  "helper/items/additionalProperties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs/"
                                  "helper/items/additionalProperties/"
                                  "type");
}

TEST(JSONSchema_frame_2020_12, property_cross_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#/properties/bar"
      },
      "bar": {
        "items": {
          "$anchor": "foo",
          "additionalProperties": true
        }
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 11);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/items",
      "https://www.sourcemeta.com/schema", "/properties/bar/items",
      "https://www.sourcemeta.com/schema", "/properties/bar/items",
      "/properties/bar", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/items/$anchor",
      "/properties/bar/items", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame,
      "https://www.sourcemeta.com/schema#/properties/bar/items/"
      "additionalProperties",
      "https://www.sourcemeta.com/schema",
      "/properties/bar/items/additionalProperties",
      "https://www.sourcemeta.com/schema",
      "/properties/bar/items/additionalProperties", "/properties/bar/items",
      false, false);

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "/properties/bar/items",
      "https://www.sourcemeta.com/schema", "/properties/bar/items",
      "/properties/bar", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref",
                          "https://www.sourcemeta.com/schema#/properties/bar",
                          "https://www.sourcemeta.com/schema",
                          "/properties/bar", "#/properties/bar");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/bar/items");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/bar/items/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/bar/items/"
      "additionalProperties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#foo");
}

TEST(JSONSchema_frame_2020_12, dynamic_ref_multiple_targets) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicAnchor": "test",
    "properties": {
      "foo": {
        "$id": "foo",
        "$dynamicAnchor": "test"
      },
      "bar": {
        "$dynamicRef": "#test"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 17);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com", "https://www.example.com", "",
      "https://www.example.com", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com/foo", "https://www.example.com",
      "/properties/foo", "https://www.example.com/foo", "", "", false, false);

  // Subschemas

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.example.com#/properties/foo",
      "https://www.example.com", "/properties/foo",
      "https://www.example.com/foo", "", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.example.com#/properties/bar",
      "https://www.example.com", "/properties/bar", "https://www.example.com",
      "/properties/bar", "", false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/$id", "https://www.example.com", "/$id",
      "https://www.example.com", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/$schema", "https://www.example.com",
      "/$schema", "https://www.example.com", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/$dynamicAnchor",
      "https://www.example.com", "/$dynamicAnchor", "https://www.example.com",
      "/$dynamicAnchor", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/properties", "https://www.example.com",
      "/properties", "https://www.example.com", "/properties", "", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/properties/foo/$id",
      "https://www.example.com", "/properties/foo/$id",
      "https://www.example.com/foo", "/$id", "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/properties/foo/$dynamicAnchor",
      "https://www.example.com", "/properties/foo/$dynamicAnchor",
      "https://www.example.com/foo", "/$dynamicAnchor", "/properties/foo",
      false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/properties/bar/$dynamicRef",
      "https://www.example.com", "/properties/bar/$dynamicRef",
      "https://www.example.com", "/properties/bar/$dynamicRef",
      "/properties/bar", false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com/foo#/$id", "https://www.example.com",
      "/properties/foo/$id", "https://www.example.com/foo", "/$id",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com/foo#/$dynamicAnchor",
      "https://www.example.com", "/properties/foo/$dynamicAnchor",
      "https://www.example.com/foo", "/$dynamicAnchor", "/properties/foo",
      false, false);

  // Anchors

  // Note that instance locations here are weird, given that instance locations
  // operate solely on pointers, independently on the location type. So these
  // locations have those instance locations, even though they don't originate
  // from static anchors
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.example.com#test", "https://www.example.com", "",
      "https://www.example.com", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.example.com/foo#test", "https://www.example.com",
      "/properties/foo", "https://www.example.com/foo", "", "", false, false);

  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.example.com#test", "https://www.example.com", "",
      "https://www.example.com", "", std::nullopt, false, false);
  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.example.com/foo#test", "https://www.example.com",
      "/properties/foo", "https://www.example.com/foo", "", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/properties/bar/$dynamicRef",
                           "https://www.example.com#test",
                           "https://www.example.com", "test", "#test");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.example.com");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.example.com/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/foo/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/foo/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/bar/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com/foo#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com/foo#/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.example.com/foo#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic,
                                  "https://www.example.com#test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic,
                                  "https://www.example.com/foo#test");
}

TEST(JSONSchema_frame_2020_12, cross_id_anonymous_nested) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/$defs/schema/items",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://example.com",
        "items": true
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 12);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, "", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);

  // From the top

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/schema", "/$defs/schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/schema/$schema", "/$defs/schema/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/schema", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/schema/$id", "/$defs/schema/$id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/schema", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/schema/items", "/$defs/schema/items",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/schema", false, true);

  // From within

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/$defs/schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$schema", "/$defs/schema/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/schema", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/$defs/schema/$id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/schema", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "https://example.com#/items", "/$defs/schema/items",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/schema", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "#/$defs/schema/items", "",
                          "/$defs/schema/items", "#/$defs/schema/items");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/schema/$schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          std::nullopt,
                          "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/schema");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/schema/$schema");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/schema/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/schema/items");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "https://example.com");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static,
                                      "https://example.com#/$schema");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static,
                                      "https://example.com#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://example.com#/items");
}

TEST(JSONSchema_frame_2020_12, relative_id_with_absolute_default_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "relative",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2020-12/schema",
                "https://example.com/relative");

  EXPECT_EQ(frame.locations().size(), 6);

  // With current identifier
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(frame, "relative", "relative", "",
                                       "relative", "", std::nullopt, false,
                                       false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "relative#/$id", "relative",
                                      "/$id", "relative", "/$id", "", false,
                                      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "relative#/$schema", "relative",
                                      "/$schema", "relative", "/$schema", "",
                                      false, false);

  // With default identifier
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(frame, "https://example.com/relative",
                                       "relative", "", "relative", "",
                                       std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://example.com/relative#/$id", "relative", "/$id",
      "relative", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://example.com/relative#/$schema", "relative", "/$schema",
      "relative", "/$schema", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "relative");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "relative#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "relative#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://example.com/relative");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://example.com/relative#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://example.com/relative#/$schema");
}

TEST(JSONSchema_frame_2020_12, zero_paths) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver, "", "", {});

  EXPECT_EQ(frame.locations().size(), 0);
  EXPECT_EQ(frame.references().size(), 0);
}

TEST(JSONSchema_frame_2020_12, single_nested_path_recursive_with_identifier) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$id": "https://www.sourcemeta.com/schema",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$ref": "#"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer wrapper_path{"wrapper"};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver, "", "",
                {sourcemeta::core::to_weak_pointer(wrapper_path)});

  EXPECT_EQ(frame.locations().size(), 8);

  // From the nested identifier

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema", "", "/wrapper",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id", "", "/wrapper/$id",
      "https://www.sourcemeta.com/schema", "/$id", "/wrapper", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema", "",
      "/wrapper/$schema", "https://www.sourcemeta.com/schema", "/$schema",
      "/wrapper", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref", "", "/wrapper/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", "/wrapper", false, false);

  // From the root

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$id", "/wrapper/$id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$schema", "/wrapper/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/wrapper/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Because the schema has `$id`, `#` does not escape to the root
  EXPECT_STATIC_REFERENCE(
      frame, "/wrapper/$ref", "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", std::nullopt, "#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/$ref");
}

TEST(JSONSchema_frame_2020_12,
     single_nested_path_recursive_without_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/wrapper"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer wrapper_path{"wrapper"};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2020-12/schema", "",
                {sourcemeta::core::to_weak_pointer(wrapper_path)});

  EXPECT_EQ(frame.locations().size(), 2);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  // Without an identifier, the reference goes from the root
  EXPECT_STATIC_REFERENCE(frame, "/wrapper/$ref", "#/wrapper", "", "/wrapper",
                          "#/wrapper");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/$ref");
}

TEST(JSONSchema_frame_2020_12, single_nested_anonymous_with_nested_resource) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "items": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$anchor": "test"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer wrapper_path{"wrapper"};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2020-12/schema", "",
                {sourcemeta::core::to_weak_pointer(wrapper_path)});

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper/items", "/wrapper/items",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/items/$id", "/wrapper/items/$id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper/items", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/items/$schema", "/wrapper/items/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper/items", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/items/$anchor", "/wrapper/items/$anchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper/items", false, false);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema", "", "/wrapper/items",
      "https://www.sourcemeta.com/schema", "", "/wrapper", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id", "", "/wrapper/items/$id",
      "https://www.sourcemeta.com/schema", "/$id", "/wrapper/items", false,
      false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema", "",
      "/wrapper/items/$schema", "https://www.sourcemeta.com/schema", "/$schema",
      "/wrapper/items", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$anchor", "",
      "/wrapper/items/$anchor", "https://www.sourcemeta.com/schema", "/$anchor",
      "/wrapper/items", false, false);

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#test", "", "/wrapper/items",
      "https://www.sourcemeta.com/schema", "", "/wrapper", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(frame, "/wrapper/items/$schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          std::nullopt,
                          "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/items");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/items/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/items/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/items/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#test");
}

TEST(JSONSchema_frame_2020_12, multiple_nested_cross_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/common/test"
    },
    "common": {
      "test": {
        "$anchor": "foo",
        "$ref": "#/common/with-id"
      },
      "with-id": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$anchor": "bar",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer path1{"wrapper"};
  const sourcemeta::core::Pointer path2{"common", "test"};
  const sourcemeta::core::Pointer path3{"common", "with-id"};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2020-12/schema", "",
                {sourcemeta::core::to_weak_pointer(path1),
                 sourcemeta::core::to_weak_pointer(path2),
                 sourcemeta::core::to_weak_pointer(path3)});

  EXPECT_EQ(frame.locations().size(), 17);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper", false, false);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/common/test", "/common/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/test/$anchor", "/common/test/$anchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/common/test", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/test/$ref", "/common/test/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/common/test", false, false);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/common/with-id", "/common/with-id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/with-id/$id", "/common/with-id/$id",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/common/with-id", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/with-id/$schema", "/common/with-id/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/common/with-id", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/with-id/$anchor", "/common/with-id/$anchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/common/with-id", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/with-id/type", "/common/with-id/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/common/with-id", false, false);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema", "", "/common/with-id",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id", "",
      "/common/with-id/$id", "https://www.sourcemeta.com/schema", "/$id",
      "/common/with-id", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema", "",
      "/common/with-id/$schema", "https://www.sourcemeta.com/schema",
      "/$schema", "/common/with-id", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$anchor", "",
      "/common/with-id/$anchor", "https://www.sourcemeta.com/schema",
      "/$anchor", "/common/with-id", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/type", "",
      "/common/with-id/type", "https://www.sourcemeta.com/schema", "/type",
      "/common/with-id", false, false);

  // Anchors

  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#foo", "/common/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#bar", "", "/common/with-id",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(frame, "/wrapper/$ref", "#/common/test", "",
                          "/common/test", "#/common/test");
  EXPECT_STATIC_REFERENCE(frame, "/common/test/$ref", "#/common/with-id", "",
                          "/common/with-id", "#/common/with-id");
  EXPECT_STATIC_REFERENCE(frame, "/common/with-id/$schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          std::nullopt,
                          "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/test/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/test/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/with-id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/with-id/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/with-id/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/with-id/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/with-id/type");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/type");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#bar");
}

TEST(JSONSchema_frame_2020_12, multiple_nested_cross_ref_missing_target) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/common/test"
    },
    "common": {
      "test": {
        "$ref": "#/common/with-id"
      },
      "with-id": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer path1{"wrapper"};
  const sourcemeta::core::Pointer path2{"common", "test"};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2020-12/schema", "",
                {sourcemeta::core::to_weak_pointer(path1),
                 sourcemeta::core::to_weak_pointer(path2)});

  EXPECT_EQ(frame.locations().size(), 4);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper", false, false);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/common/test", "/common/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/test/$ref", "/common/test/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/common/test", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(frame, "/wrapper/$ref", "#/common/test", "",
                          "/common/test", "#/common/test");
  EXPECT_STATIC_REFERENCE(frame, "/common/test/$ref", "#/common/with-id", "",
                          "/common/with-id", "#/common/with-id");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/test/$ref");
}

TEST(JSONSchema_frame_2020_12, multiple_nested_no_base_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/common/test"
    },
    "common": {
      "test": {
        "$ref": "#/common/with-id"
      },
      "with-id": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer path1{"wrapper"};
  const sourcemeta::core::Pointer path2{"common", "test"};
  const sourcemeta::core::Pointer path3{"common", "with-id"};

  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver, "", "",
                             {sourcemeta::core::to_weak_pointer(path1),
                              sourcemeta::core::to_weak_pointer(path2),
                              sourcemeta::core::to_weak_pointer(path3)}),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_frame_2020_12, multiple_nested_same_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "common": {
      "foo": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      },
      "bar": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer path1{"common", "foo"};
  const sourcemeta::core::Pointer path2{"common", "bar"};

  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver, "", "",
                             {sourcemeta::core::to_weak_pointer(path1),
                              sourcemeta::core::to_weak_pointer(path2)}),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_2020_12, multiple_nested_same_anonymous_anchors) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "common": {
      "foo": {
        "$anchor": "test"
      },
      "bar": {
        "$anchor": "test"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer path1{"common", "foo"};
  const sourcemeta::core::Pointer path2{"common", "bar"};

  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver,
                             "https://json-schema.org/draft/2020-12/schema", "",
                             {sourcemeta::core::to_weak_pointer(path1),
                              sourcemeta::core::to_weak_pointer(path2)}),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_2020_12, multiple_nested_with_default_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/common/test"
    },
    "common": {
      "test": {
        "$ref": "#/common/with-id"
      },
      "with-id": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  const sourcemeta::core::Pointer path1{"wrapper"};
  const sourcemeta::core::Pointer path2{"common", "test"};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "https://json-schema.org/draft/2020-12/schema",
                // The default id should be getting ignored on nested schemas
                // as it only makes sense for top-level framing
                "https://www.example.com",
                {sourcemeta::core::to_weak_pointer(path1),
                 sourcemeta::core::to_weak_pointer(path2)});

  EXPECT_EQ(frame.locations().size(), 4);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/wrapper", false, false);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/common/test", "/common/test",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/test/$ref", "/common/test/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/common/test", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(frame, "/wrapper/$ref", "#/common/test", "",
                          "/common/test", "#/common/test");
  EXPECT_STATIC_REFERENCE(frame, "/common/test/$ref", "#/common/with-id", "",
                          "/common/with-id", "#/common/with-id");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/wrapper/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/common/test/$ref");
}

TEST(JSONSchema_frame_2020_12, anchor_with_invalid_type) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": 123
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$anchor", "/$anchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);

  EXPECT_EQ(frame.references().size(), 1);
  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$anchor");
}

TEST(JSONSchema_frame_2020_12, dynamic_anchor_with_invalid_type) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicAnchor": 123
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$dynamicAnchor", "/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);

  EXPECT_EQ(frame.references().size(), 1);
  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$dynamicAnchor");
}

TEST(JSONSchema_frame_2020_12, dynamic_ref_with_invalid_type) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicRef": 123
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$dynamicRef");
    EXPECT_EQ(error.value(), "123");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12,
     ref_does_not_invalidate_sibling_subschemas_and_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$ref", "/properties/foo/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties", "/properties/foo/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo/properties/bar",
      "/properties/foo/properties/bar",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties/bar/$ref",
      "/properties/foo/properties/bar/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo/properties/bar", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo/properties/bar/additionalProperties",
      "/properties/foo/properties/bar/additionalProperties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo/properties/bar", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties/bar/additionalProperties/$ref",
      "/properties/foo/properties/bar/additionalProperties/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo/properties/bar/additionalProperties", false, false);

  EXPECT_EQ(frame.references().size(), 4);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/foo/properties/bar");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/foo/properties/bar/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "#/properties/foo/properties/bar/additionalProperties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static,
      "#/properties/foo/properties/bar/additionalProperties/$ref");
}

TEST(JSONSchema_frame_2020_12, ref_to_ref_chain) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/$defs/parent",
    "$defs": {
      "parent": {
        "$ref": "#/$defs/child"
      },
      "child": {
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/parent",
      "https://www.sourcemeta.com/schema", "/$defs/parent",
      "https://www.sourcemeta.com/schema", "/$defs/parent", "", false, true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs/parent/$ref",
      "https://www.sourcemeta.com/schema", "/$defs/parent/$ref",
      "https://www.sourcemeta.com/schema", "/$defs/parent/$ref",
      "/$defs/parent", false, true);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/child",
      "https://www.sourcemeta.com/schema", "/$defs/child",
      "https://www.sourcemeta.com/schema", "/$defs/child", "", false, true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs/child/type",
      "https://www.sourcemeta.com/schema", "/$defs/child/type",
      "https://www.sourcemeta.com/schema", "/$defs/child/type", "/$defs/child",
      false, true);

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/$defs/parent",
      "https://www.sourcemeta.com/schema", "/$defs/parent", "#/$defs/parent");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/parent/$ref",
                          "https://www.sourcemeta.com/schema#/$defs/child",
                          "https://www.sourcemeta.com/schema", "/$defs/child",
                          "#/$defs/child");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/parent");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/parent/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/child");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/child/type");
}

TEST(JSONSchema_frame_2020_12, nested_defs_unreferenced) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$defs": {
          "unused": { "type": "string" }
        },
        "type": "object"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$defs",
      "https://www.sourcemeta.com/schema", "/properties/foo/$defs",
      "https://www.sourcemeta.com/schema", "/properties/foo/$defs",
      "/properties/foo", false, false);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$defs/unused",
      "https://www.sourcemeta.com/schema", "/properties/foo/$defs/unused",
      "https://www.sourcemeta.com/schema", "/properties/foo/$defs/unused",
      "/properties/foo", false, true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame,
      "https://www.sourcemeta.com/schema#/properties/foo/$defs/unused/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/$defs/unused/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/$defs/unused/type",
      "/properties/foo/$defs/unused", false, true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "/properties/foo", false, false);

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/$defs");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/$defs/unused");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(
      frame, Static,
      "https://www.sourcemeta.com/schema#/properties/foo/$defs/unused/type");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo/type");
}

TEST(JSONSchema_frame_2020_12, circular_ref_through_defs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "a": { "$ref": "#/$defs/b" },
      "b": { "$ref": "#/$defs/a" }
    },
    "$ref": "#/$defs/a"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/a",
      "https://www.sourcemeta.com/schema", "/$defs/a",
      "https://www.sourcemeta.com/schema", "/$defs/a", "", false, true);
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/b",
      "https://www.sourcemeta.com/schema", "/$defs/b",
      "https://www.sourcemeta.com/schema", "/$defs/b", "", false, true);

  // JSON Pointers
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", "", false, false);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs/a/$ref",
      "https://www.sourcemeta.com/schema", "/$defs/a/$ref",
      "https://www.sourcemeta.com/schema", "/$defs/a/$ref", "/$defs/a", false,
      true);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs/b/$ref",
      "https://www.sourcemeta.com/schema", "/$defs/b/$ref",
      "https://www.sourcemeta.com/schema", "/$defs/b/$ref", "/$defs/b", false,
      true);

  // References

  EXPECT_EQ(frame.references().size(), 4);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/$defs/a",
      "https://www.sourcemeta.com/schema", "/$defs/a", "#/$defs/a");
  EXPECT_STATIC_REFERENCE(
      frame, "/$defs/a/$ref", "https://www.sourcemeta.com/schema#/$defs/b",
      "https://www.sourcemeta.com/schema", "/$defs/b", "#/$defs/b");
  EXPECT_STATIC_REFERENCE(
      frame, "/$defs/b/$ref", "https://www.sourcemeta.com/schema#/$defs/a",
      "https://www.sourcemeta.com/schema", "/$defs/a", "#/$defs/a");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$id");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs/a");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/$defs/b");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/a/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/$defs/b/$ref");
}

TEST(JSONSchema_frame_2020_12, nested_invalid_schema_keyword) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "nested": {
        "$schema": 123
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$schema");
    EXPECT_EQ(error.value(), "123");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, invalid_id_not_string) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": 123
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$id");
    EXPECT_EQ(error.value(), "123");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, invalid_id_not_uri) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "not a valid uri"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$id");
    EXPECT_EQ(error.value(), "not a valid uri");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, nested_invalid_schema_not_uri) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "nested": {
        "$schema": "not a valid uri"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$schema");
    EXPECT_EQ(error.value(), "not a valid uri");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, invalid_ref_not_string) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": 123
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$ref");
    EXPECT_EQ(error.value(), "123");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, invalid_ref_not_uri) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "not a valid uri"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$ref");
    EXPECT_EQ(error.value(), "not a valid uri");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, invalid_dynamic_ref_not_string) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicRef": 123
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$dynamicRef");
    EXPECT_EQ(error.value(), "123");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, invalid_dynamic_ref_not_uri) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicRef": "not a valid uri"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};

  try {
    frame.analyse(document, sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_EQ(error.keyword(), "$dynamicRef");
    EXPECT_EQ(error.value(), "not a valid uri");
  } catch (...) {
    FAIL();
  }
}

TEST(JSONSchema_frame_2020_12, ref_to_orphan_chain) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "#/$defs/a" }
    },
    "$defs": {
      "a": { "$ref": "#/$defs/b" },
      "b": { "$ref": "#/$defs/c" },
      "c": { "type": "string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 12);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/a", "/$defs/a",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/b", "/$defs/b",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/c", "/$defs/c",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$ref", "/properties/foo/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/a/$ref", "/$defs/a/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/a", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/b/$ref", "/$defs/b/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/b", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/c/type", "/$defs/c/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/c", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 4);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref", "#/$defs/a", "",
                          "/$defs/a", "#/$defs/a");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/a/$ref", "#/$defs/b", "", "/$defs/b",
                          "#/$defs/b");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/b/$ref", "#/$defs/c", "", "/$defs/c",
                          "#/$defs/c");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/a");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/a/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/b");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/b/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/c");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/c/type");
}

TEST(JSONSchema_frame_2020_12, orphan_chain_unreferenced) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "a": { "$ref": "#/$defs/b" },
      "b": { "$ref": "#/$defs/c" },
      "c": { "type": "string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/a", "/$defs/a",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/b", "/$defs/b",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/c", "/$defs/c",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/a/$ref", "/$defs/a/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/a", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/b/$ref", "/$defs/b/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/b", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/c/type", "/$defs/c/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/c", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/a/$ref", "#/$defs/b", "", "/$defs/b",
                          "#/$defs/b");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/b/$ref", "#/$defs/c", "", "/$defs/c",
                          "#/$defs/c");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/a");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/a/$ref");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/b");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/b/$ref");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/c");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/c/type");
}

TEST(JSONSchema_frame_2020_12, circular_orphan_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "a": { "$ref": "#/$defs/b" },
      "b": { "$ref": "#/$defs/a" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/a", "/$defs/a",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/b", "/$defs/b",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/a/$ref", "/$defs/a/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/a", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/b/$ref", "/$defs/b/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/b", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/a/$ref", "#/$defs/b", "", "/$defs/b",
                          "#/$defs/b");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/b/$ref", "#/$defs/a", "", "/$defs/a",
                          "#/$defs/a");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/a");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/a/$ref");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/b");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/b/$ref");
}

TEST(JSONSchema_frame_2020_12, ref_from_deep_nesting) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "x": {
        "properties": {
          "y": {
            "properties": {
              "z": { "$ref": "#/$defs/string" }
            }
          }
        }
      }
    },
    "$defs": {
      "string": { "type": "string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 12);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/x", "/properties/x",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/x/properties/y", "/properties/x/properties/y",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/x", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/x/properties/y/properties/z",
      "/properties/x/properties/y/properties/z",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/x/properties/y", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/x/properties", "/properties/x/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/x", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/x/properties/y/properties",
      "/properties/x/properties/y/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/x/properties/y", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/x/properties/y/properties/z/$ref",
      "/properties/x/properties/y/properties/z/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/x/properties/y/properties/z", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/type", "/$defs/string/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/x/properties/y/properties/z/$ref",
                          "#/$defs/string", "", "/$defs/string",
                          "#/$defs/string");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/x");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/x/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/x/properties/y");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/x/properties/y/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/x/properties/y/properties/z");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "#/properties/x/properties/y/properties/z/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string/type");
}

TEST(JSONSchema_frame_2020_12, orphan_refs_reachable_target) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "#/$defs/string" }
    },
    "$defs": {
      "string": { "type": "string" },
      "orphan": { "$ref": "#/$defs/string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/orphan", "/$defs/orphan",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$ref", "/properties/foo/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/type", "/$defs/string/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/orphan/$ref", "/$defs/orphan/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/orphan", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref", "#/$defs/string", "",
                          "/$defs/string", "#/$defs/string");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/orphan/$ref", "#/$defs/string", "",
                          "/$defs/string", "#/$defs/string");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string/type");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/orphan");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/orphan/$ref");
}

TEST(JSONSchema_frame_2020_12, root_dynamic_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicAnchor": "root",
    "type": "string"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#root", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#root", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$dynamicAnchor", "/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/type", "/type", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#root");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic, "#root");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/type");
}

TEST(JSONSchema_frame_2020_12, dynamic_ref_to_static_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$dynamicRef": "#target" }
    },
    "$defs": {
      "string": {
        "$anchor": "target",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#target", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$dynamicRef", "/properties/foo/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/$anchor", "/$defs/string/$anchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/type", "/$defs/string/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$dynamicRef", "#target", "",
                          "target", "#target");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/foo/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#target");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string/$anchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string/type");
}

TEST(JSONSchema_frame_2020_12, dynamic_ref_to_pointer) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$dynamicRef": "#/$defs/string" }
    },
    "$defs": {
      "string": { "type": "string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 8);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$dynamicRef", "/properties/foo/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/type", "/$defs/string/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$dynamicRef",
                          "#/$defs/string", "", "/$defs/string",
                          "#/$defs/string");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/foo/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string/type");
}

TEST(JSONSchema_frame_2020_12, dynamic_ref_to_dynamic_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$dynamicRef": "#target" }
    },
    "$defs": {
      "string": {
        "$dynamicAnchor": "target",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 11);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#target", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#target", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$dynamicRef", "/properties/foo/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/$dynamicAnchor", "/$defs/string/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/type", "/$defs/string/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$dynamicRef", "#target", "",
                          "target", "#target");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/properties/foo/$dynamicRef");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#target");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic, "#target");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/$defs/string/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string/type");
}

TEST(JSONSchema_frame_2020_12, static_ref_to_dynamic_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "#target" }
    },
    "$defs": {
      "string": {
        "$dynamicAnchor": "target",
        "additionalProperties": {
          "type": "string",
          "$defs": {
            "nested": {
              "type": "string"
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

  EXPECT_EQ(frame.locations().size(), 15);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#target", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#target", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string/additionalProperties",
      "/$defs/string/additionalProperties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string/additionalProperties/$defs/nested",
      "/$defs/string/additionalProperties/$defs/nested",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string/additionalProperties", false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$ref", "/properties/foo/$ref",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/properties/foo", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/$dynamicAnchor", "/$defs/string/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/additionalProperties/type",
      "/$defs/string/additionalProperties/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string/additionalProperties", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/additionalProperties/$defs",
      "/$defs/string/additionalProperties/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string/additionalProperties", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/additionalProperties/$defs/nested/type",
      "/$defs/string/additionalProperties/$defs/nested/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string/additionalProperties/$defs/nested", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref", "#target", "",
                          "target", "#target");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/properties/foo/$ref");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#target");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Dynamic, "#target");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs/string");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/$defs/string/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/$defs/string/additionalProperties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "#/$defs/string/additionalProperties/type");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(
      frame, Static, "#/$defs/string/additionalProperties/$defs");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(
      frame, Static, "#/$defs/string/additionalProperties/$defs/nested");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(
      frame, Static, "#/$defs/string/additionalProperties/$defs/nested/type");
}

TEST(JSONSchema_frame_2020_12, unreferenced_dynamic_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "string": {
        "$dynamicAnchor": "unused",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 8);

  // Subschemas

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      JSON_Schema_2020_12, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/string", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#unused", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);
  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#unused", "/$defs/string",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, true);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12, "",
      false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/$dynamicAnchor", "/$defs/string/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/string/type", "/$defs/string/type",
      "https://json-schema.org/draft/2020-12/schema", JSON_Schema_2020_12,
      "/$defs/string", false, true);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$schema");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/$defs");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#unused");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Dynamic, "#unused");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/string");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static,
                                      "#/$defs/string/$dynamicAnchor");
  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "#/$defs/string/type");
}
