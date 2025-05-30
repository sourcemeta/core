#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

#define EXPECT_FRAME_STATIC_2020_12_POINTER(                                   \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_STATIC_POINTER(frame, reference, root_id, expected_pointer,     \
                              "https://json-schema.org/draft/2020-12/schema",  \
                              "https://json-schema.org/draft/2020-12/schema",  \
                              expected_base, expected_relative_pointer,        \
                              expected_instance_locations, expected_parent);

#define EXPECT_FRAME_STATIC_2020_12_RESOURCE(                                  \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_STATIC_RESOURCE(frame, reference, root_id, expected_pointer,    \
                               "https://json-schema.org/draft/2020-12/schema", \
                               "https://json-schema.org/draft/2020-12/schema", \
                               expected_base, expected_relative_pointer,       \
                               expected_instance_locations, expected_parent);

#define EXPECT_FRAME_STATIC_2020_12_ANCHOR(                                    \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_STATIC_ANCHOR(frame, reference, root_id, expected_pointer,      \
                             "https://json-schema.org/draft/2020-12/schema",   \
                             "https://json-schema.org/draft/2020-12/schema",   \
                             expected_base, expected_relative_pointer,         \
                             expected_instance_locations, expected_parent);

#define EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(                                 \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_STATIC_SUBSCHEMA(                                               \
      frame, reference, root_id, expected_pointer,                             \
      "https://json-schema.org/draft/2020-12/schema",                          \
      "https://json-schema.org/draft/2020-12/schema", expected_base,           \
      expected_relative_pointer, expected_instance_locations,                  \
      expected_parent);

#define EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(                                   \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_DYNAMIC_ANCHOR(frame, reference, root_id, expected_pointer,     \
                              "https://json-schema.org/draft/2020-12/schema",  \
                              "https://json-schema.org/draft/2020-12/schema",  \
                              expected_base, expected_relative_pointer,        \
                              expected_instance_locations, expected_parent);

TEST(JSONSchema_frame_2020_12, anonymous_with_nested_schema_resource) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "additionalProperties": { "$id": "https://example.com" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/additionalProperties",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      {"/~?additionalProperties~/~P~"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/additionalProperties/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {},
      "/additionalProperties");

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalProperties", "/additionalProperties",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      {"/~?additionalProperties~/~P~"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalProperties/$id", "/additionalProperties/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {},
      "/additionalProperties");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame_2020_12, empty_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame_2020_12, empty_schema_trailing_hash) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema#",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 8);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/schema", "/items/type", {}, "/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type", {},
      "/properties/foo");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 14);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/test/qux",
      "https://www.sourcemeta.com/test/qux", "",
      "https://www.sourcemeta.com/test/qux", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", {"/~?items~/~I~"}, "");

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/test/qux#test",
      "https://www.sourcemeta.com/test/qux", "/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo", {"/foo"}, "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$id",
      "https://www.sourcemeta.com/test/qux", "/$id",
      "https://www.sourcemeta.com/test/qux", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test/qux#/items",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", {}, "/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties",
      "https://www.sourcemeta.com/test/qux", "/properties",
      "https://www.sourcemeta.com/test/qux", "/properties", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo",
      "https://www.sourcemeta.com/test/qux", "/properties/foo", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo/$anchor",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/$anchor", {},
      "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/properties/foo/type",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/type",
      "https://www.sourcemeta.com/test/qux", "/properties/foo/type", {},
      "/properties/foo");

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", {}, "/items");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 9);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", {"/~?items~/~I~"}, "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/$id",
      "https://www.sourcemeta.com/schema", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", {}, "/items");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 30);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/foo#test",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", {"/bar"}, "");
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/baz", "", {"/baz"}, "");
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/baz#extra",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", {"/baz/~?items~/~I~"},
      "/properties/baz");
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/qux",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", {"/foo/~?items~/~I~"},
      "/properties/foo");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", {}, "");

  // foo
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/foo", "", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/foo", "/$anchor", {}, "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/items",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", {"/foo/~?items~/~I~"},
      "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/items/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", {}, "/properties/foo/items");

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$anchor",
      "https://www.sourcemeta.com/foo", "/$anchor", {}, "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/foo#/items",
      "https://www.sourcemeta.com/schema", "/properties/foo/items",
      "https://www.sourcemeta.com/qux", "", {"/foo/~?items~/~I~"},
      "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/foo#/items/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", {}, "/properties/foo/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/qux#/$id",
      "https://www.sourcemeta.com/schema", "/properties/foo/items/$id",
      "https://www.sourcemeta.com/qux", "/$id", {}, "/properties/foo/items");

  // bar
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", {"/bar"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", {}, "/properties/bar");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", {}, "/properties/bar");

  // baz
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/baz", "", {"/baz"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/$id",
      "https://www.sourcemeta.com/schema", "/properties/baz/$id",
      "https://www.sourcemeta.com/baz", "/$id", {}, "/properties/baz");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", {"/baz/~?items~/~I~"},
      "/properties/baz");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/baz", "/items/$anchor", {},
      "/properties/baz/items");

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/baz#/$id",
      "https://www.sourcemeta.com/schema", "/properties/baz/$id",
      "https://www.sourcemeta.com/baz", "/$id", {}, "/properties/baz");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/baz#/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/baz", "/items", {"/baz/~?items~/~I~"},
      "/properties/baz");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/baz#/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$anchor",
      "https://www.sourcemeta.com/baz", "/items/$anchor", {},
      "/properties/baz/items");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame_2020_12, id_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$id": "schema" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaError);
}

TEST(JSONSchema_frame_2020_12, static_anchor_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo",
    "items": { "$anchor": "foo" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaError);
}

TEST(JSONSchema_frame_2020_12, explicit_argument_id_same) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "https://json-schema.org/draft/2020-12/schema",
                "https://www.sourcemeta.com/schema");

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame_2020_12, anchor_top_level) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$anchor",
      "https://www.sourcemeta.com/schema", "/$anchor",
      "https://www.sourcemeta.com/schema", "/$anchor", {}, "");

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "https://json-schema.org/draft/2020-12/schema",
                "https://www.example.com");

  EXPECT_EQ(frame.locations().size(), 38);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", {"/one"}, "");
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com", "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", {"/one"}, "");
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.test.com", "https://www.sourcemeta.com/schema",
      "/properties/two", "https://www.test.com", "", {"/two"}, "");

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.example.com#foo", "https://www.sourcemeta.com/schema",
      "/items", "https://www.example.com", "/items", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/test#bar",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", {"/one"}, "");
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.example.com/test#bar",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", {"/one"}, "");
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.test.com#baz", "https://www.sourcemeta.com/schema",
      "/properties/two", "https://www.test.com", "", {"/two"}, "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id", "https://www.example.com",
      "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.example.com", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items", "https://www.example.com",
      "/items", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/$anchor",
      "https://www.sourcemeta.com/schema", "/items/$anchor",
      "https://www.example.com", "/items/$anchor", {}, "/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.example.com", "/properties", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/one",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", {"/one"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.example.com/test", "/$id", {}, "/properties/one");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/one/$anchor",
      "https://www.example.com/test", "/$anchor", {}, "/properties/one");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/two",
      "https://www.sourcemeta.com/schema", "/properties/two",
      "https://www.test.com", "", {"/two"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$id",
      "https://www.sourcemeta.com/schema", "/properties/two/$id",
      "https://www.test.com", "/$id", {}, "/properties/two");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/two/$anchor",
      "https://www.test.com", "/$anchor", {}, "/properties/two");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test#/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.example.com/test", "/$id", {}, "/properties/one");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/test#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/one/$anchor",
      "https://www.example.com/test", "/$anchor", {}, "/properties/one");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.test.com#/$id", "https://www.sourcemeta.com/schema",
      "/properties/two/$id", "https://www.test.com", "/$id", {},
      "/properties/two");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.test.com#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/two/$anchor",
      "https://www.test.com", "/$anchor", {}, "/properties/two");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.references().size(), 7);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$dynamicRef", "",
                          std::nullopt, std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$dynamicRef",
                          "#/properties/baz", std::nullopt, "/properties/baz",
                          "#/properties/baz");
  EXPECT_STATIC_REFERENCE(frame, "/properties/qux/$dynamicRef",
                          "https://www.example.com", "https://www.example.com",
                          std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/anchor/$dynamicRef", "#baz",
                          std::nullopt, "baz", "#baz");
  EXPECT_DYNAMIC_REFERENCE(frame, "/properties/extra/$dynamicRef", "#dynamic",
                           std::nullopt, "dynamic", "#dynamic");
  EXPECT_DYNAMIC_REFERENCE(frame, "/properties/unknown/$dynamicRef",
                           "https://www.example.com/foo#xxx",
                           "https://www.example.com/foo", "xxx",
                           "https://www.example.com/foo#xxx");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");

  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$ref",
                          "https://www.sourcemeta.com/schema#foo",
                          "https://www.sourcemeta.com/schema", "foo", "#foo");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 21);

  // Dynamic anchors

  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#test",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", {"/foo"}, "");
  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/bar#test",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", {"/bar"}, "");

  // Static anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#test",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/bar#test",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", {"/bar"}, "");

  // Static identifiers

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", {"/bar"}, "");

  // Static pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/$dynamicAnchor", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/foo/$dynamicAnchor", {},
      "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/bar", "", {"/bar"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", {}, "/properties/bar");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/$dynamicAnchor",
      "https://www.sourcemeta.com/bar", "/$dynamicAnchor", {},
      "/properties/bar");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/$anchor",
      "https://www.sourcemeta.com/bar", "/$anchor", {}, "/properties/bar");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$id",
      "https://www.sourcemeta.com/schema", "/properties/bar/$id",
      "https://www.sourcemeta.com/bar", "/$id", {}, "/properties/bar");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$dynamicAnchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/$dynamicAnchor",
      "https://www.sourcemeta.com/bar", "/$dynamicAnchor", {},
      "/properties/bar");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/bar#/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/$anchor",
      "https://www.sourcemeta.com/bar", "/$anchor", {}, "/properties/bar");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  // Dynamic anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#test", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/foo"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#test", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/foo"}, "");

  // Static frames

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/foo"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$dynamicAnchor",
      "/properties/foo/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/properties/foo");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 8);

  // Dynamic anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, "");

  // Static frames

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$dynamicRef", "/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/test/$dynamicAnchor", "/$defs/test/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/test");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$dynamicRef", "#test", std::nullopt, "test",
                          "#test");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  // Dynamic anchors

  EXPECT_ANONYMOUS_FRAME_DYNAMIC_ANCHOR(
      frame, "#test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, "");

  // Static frames

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$dynamicRef", "/$dynamicRef",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/test", "/$defs/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/test/$dynamicAnchor", "/$defs/test/$dynamicAnchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/test");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/foo", "/$defs/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/foo/$ref", "/$defs/foo/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/foo");

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/$dynamicRef", "#test", std::nullopt, "test",
                           "#test");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/foo/$ref", "https://sourcemeta.com",
                          "https://sourcemeta.com", std::nullopt,
                          "https://sourcemeta.com");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaError);
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      POINTER_TEMPLATES("", "/foo"), std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/foo"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$ref", "/properties/foo/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/properties/foo");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref", "", std::nullopt,
                          std::nullopt, "#");
}

TEST(JSONSchema_frame_2020_12, ref_metaschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");

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
      sourcemeta::core::SchemaFrame::Mode::Instances};

  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaError);
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", {""}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs/string/type",
      "https://www.sourcemeta.com/schema", "/$defs/string/type",
      "https://www.sourcemeta.com/schema", "/$defs/string/type", {},
      "/$defs/string");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", "#/$defs/string");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 9);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions",
      "https://www.sourcemeta.com/schema", "/definitions",
      "https://www.sourcemeta.com/schema", "/definitions", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/definitions/middle",
      "https://www.sourcemeta.com/schema", "/definitions/middle",
      "https://www.sourcemeta.com/schema", "/definitions/middle", {""}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions/middle/$ref",
      "https://www.sourcemeta.com/schema", "/definitions/middle/$ref",
      "https://www.sourcemeta.com/schema", "/definitions/middle/$ref", {},
      "/definitions/middle");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/definitions/string",
      "https://www.sourcemeta.com/schema", "/definitions/string",
      "https://www.sourcemeta.com/schema", "/definitions/string", {""}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/definitions/string/type",
      "https://www.sourcemeta.com/schema", "/definitions/string/type",
      "https://www.sourcemeta.com/schema", "/definitions/string/type", {},
      "/definitions/string");

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
}

TEST(JSONSchema_frame_2020_12, relative_base_uri_without_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "common"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(frame, "common", "common", "", "common",
                                       "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$schema", "common",
                                      "/$schema", "common", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$id", "common", "/$id",
                                      "common", "/$id", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(frame, "common", "common", "", "common",
                                       "", {""}, std::nullopt);

  // Anchors
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(frame, "common#foo", "common",
                                     "/$defs/foo", "common", "/$defs/foo", {""},
                                     "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$schema", "common",
                                      "/$schema", "common", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$id", "common", "/$id",
                                      "common", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/allOf", "common",
                                      "/allOf", "common", "/allOf", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(frame, "common#/allOf/0", "common",
                                        "/allOf/0", "common", "/allOf/0", {""},
                                        "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/allOf/0/$ref", "common",
                                      "/allOf/0/$ref", "common",
                                      "/allOf/0/$ref", {}, "/allOf/0");
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$defs", "common",
                                      "/$defs", "common", "/$defs", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(frame, "common#/$defs/foo", "common",
                                        "/$defs/foo", "common", "/$defs/foo",
                                        {""}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "common#/$defs/foo/$anchor",
                                      "common", "/$defs/foo/$anchor", "common",
                                      "/$defs/foo/$anchor", {}, "/$defs/foo");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/allOf/0/$ref", "common#foo", "common", "foo",
                          "#foo");
}

TEST(JSONSchema_frame_2020_12, relative_base_with_relative_path_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "foo/bar/baz",
    "$ref": "qux"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 4);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(frame, "foo/bar/baz", "foo/bar/baz", "",
                                       "foo/bar/baz", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "foo/bar/baz#/$schema",
                                      "foo/bar/baz", "/$schema", "foo/bar/baz",
                                      "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "foo/bar/baz#/$id", "foo/bar/baz",
                                      "/$id", "foo/bar/baz", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(frame, "foo/bar/baz#/$ref", "foo/bar/baz",
                                      "/$ref", "foo/bar/baz", "/$ref", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "foo/bar/qux", "foo/bar/qux",
                          std::nullopt, "qux");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};

  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref",
      "https://www.sourcemeta.com/schema", "/$ref",
      "https://www.sourcemeta.com/schema", "/$ref", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", {""}, "");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "https://www.sourcemeta.com/schema#/$defs/string",
      "https://www.sourcemeta.com/schema", "/$defs/string", "#/$defs/string");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 11);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      POINTER_TEMPLATES("/foo", "/bar", "/baz/~?items~/~I~"), "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type", {},
      "/properties/foo");

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      POINTER_TEMPLATES("/bar", "/baz/~?items~/~I~"), "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref", {},
      "/properties/bar");

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz",
      "https://www.sourcemeta.com/schema", "/properties/baz", {"/baz"}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      "https://www.sourcemeta.com/schema", "/properties/baz/items",
      {"/baz/~?items~/~I~"}, "/properties/baz");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/baz/items/$ref",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$ref",
      "https://www.sourcemeta.com/schema", "/properties/baz/items/$ref", {},
      "/properties/baz/items");

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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 13);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref", {},
      "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar", {"/bar"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref",
      "https://www.sourcemeta.com/schema", "/properties/bar/$ref", {},
      "/properties/bar");

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$defs",
      "https://www.sourcemeta.com/schema", "/$defs",
      "https://www.sourcemeta.com/schema", "/$defs", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/helper",
      "https://www.sourcemeta.com/schema", "/$defs/helper",
      "https://www.sourcemeta.com/schema", "/$defs/helper", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/$defs/helper/items",
      "https://www.sourcemeta.com/schema", "/$defs/helper/items",
      "https://www.sourcemeta.com/schema", "/$defs/helper/items",
      POINTER_TEMPLATES("/bar", "/foo/~?items~/~I~"), "/$defs/helper");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame,
      "https://www.sourcemeta.com/schema#/$defs/helper/items/"
      "additionalProperties",
      "https://www.sourcemeta.com/schema",
      "/$defs/helper/items/additionalProperties",
      "https://www.sourcemeta.com/schema",
      "/$defs/helper/items/additionalProperties",
      POINTER_TEMPLATES("/bar/~?additionalProperties~/~P~",
                        "/foo/~?items~/~I~/~?additionalProperties~/~P~"),
      "/$defs/helper/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame,
      "https://www.sourcemeta.com/schema#/$defs/helper/items/"
      "additionalProperties/type",
      "https://www.sourcemeta.com/schema",
      "/$defs/helper/items/additionalProperties/type",
      "https://www.sourcemeta.com/schema",
      "/$defs/helper/items/additionalProperties/type", {},
      "/$defs/helper/items/additionalProperties");

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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 11);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", {}, "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", {"/foo"}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref",
      "https://www.sourcemeta.com/schema", "/properties/foo/$ref", {},
      "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      "https://www.sourcemeta.com/schema", "/properties/bar",
      POINTER_TEMPLATES("/bar", "/foo"), "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/items",
      "https://www.sourcemeta.com/schema", "/properties/bar/items",
      "https://www.sourcemeta.com/schema", "/properties/bar/items",
      POINTER_TEMPLATES("/bar/~?items~/~I~", "/foo/~?items~/~I~"),
      "/properties/bar");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/bar/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/items/$anchor",
      "https://www.sourcemeta.com/schema", "/properties/bar/items/$anchor", {},
      "/properties/bar/items");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame,
      "https://www.sourcemeta.com/schema#/properties/bar/items/"
      "additionalProperties",
      "https://www.sourcemeta.com/schema",
      "/properties/bar/items/additionalProperties",
      "https://www.sourcemeta.com/schema",
      "/properties/bar/items/additionalProperties",
      POINTER_TEMPLATES("/bar/~?items~/~I~/~?additionalProperties~/~P~",
                        "/foo/~?items~/~I~/~?additionalProperties~/~P~"),
      "/properties/bar/items");

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#foo",
      "https://www.sourcemeta.com/schema", "/properties/bar/items",
      "https://www.sourcemeta.com/schema", "/properties/bar/items",
      POINTER_TEMPLATES("/bar/~?items~/~I~", "/foo/~?items~/~I~"),
      "/properties/bar");

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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 17);

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com", "https://www.example.com", "",
      "https://www.example.com", "", POINTER_TEMPLATES("", "/bar"),
      std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.example.com/foo", "https://www.example.com",
      "/properties/foo", "https://www.example.com/foo", "",
      POINTER_TEMPLATES("/foo", "/bar"), "");

  // Subschemas

  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.example.com#/properties/foo",
      "https://www.example.com", "/properties/foo",
      "https://www.example.com/foo", "", POINTER_TEMPLATES("/foo", "/bar"), "");
  EXPECT_FRAME_STATIC_2020_12_SUBSCHEMA(
      frame, "https://www.example.com#/properties/bar",
      "https://www.example.com", "/properties/bar", "https://www.example.com",
      "/properties/bar", {"/bar"}, "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/$id", "https://www.example.com", "/$id",
      "https://www.example.com", "/$id", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/$schema", "https://www.example.com",
      "/$schema", "https://www.example.com", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/$dynamicAnchor",
      "https://www.example.com", "/$dynamicAnchor", "https://www.example.com",
      "/$dynamicAnchor", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/properties", "https://www.example.com",
      "/properties", "https://www.example.com", "/properties", {}, "");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/properties/foo/$id",
      "https://www.example.com", "/properties/foo/$id",
      "https://www.example.com/foo", "/$id", {}, "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/properties/foo/$dynamicAnchor",
      "https://www.example.com", "/properties/foo/$dynamicAnchor",
      "https://www.example.com/foo", "/$dynamicAnchor", {}, "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com#/properties/bar/$dynamicRef",
      "https://www.example.com", "/properties/bar/$dynamicRef",
      "https://www.example.com", "/properties/bar/$dynamicRef", {},
      "/properties/bar");

  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com/foo#/$id", "https://www.example.com",
      "/properties/foo/$id", "https://www.example.com/foo", "/$id", {},
      "/properties/foo");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.example.com/foo#/$dynamicAnchor",
      "https://www.example.com", "/properties/foo/$dynamicAnchor",
      "https://www.example.com/foo", "/$dynamicAnchor", {}, "/properties/foo");

  // Anchors

  // Note that instance locations here are weird, given that instance locations
  // operate solely on pointers, independently on the location type. So these
  // locations have those instance locations, even though they don't originate
  // from static anchors
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.example.com#test", "https://www.example.com", "",
      "https://www.example.com", "", POINTER_TEMPLATES("", "/bar"),
      std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.example.com/foo#test", "https://www.example.com",
      "/properties/foo", "https://www.example.com/foo", "",
      POINTER_TEMPLATES("/foo", "/bar"), "");

  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.example.com#test", "https://www.example.com", "",
      "https://www.example.com", "", POINTER_TEMPLATES("", "/bar"),
      std::nullopt);
  EXPECT_FRAME_DYNAMIC_2020_12_ANCHOR(
      frame, "https://www.example.com/foo#test", "https://www.example.com",
      "/properties/foo", "https://www.example.com/foo", "",
      POINTER_TEMPLATES("/foo", "/bar"), "");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DYNAMIC_REFERENCE(frame, "/properties/bar/$dynamicRef",
                           "https://www.example.com#test",
                           "https://www.example.com", "test", "#test");
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
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 12);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, std::nullopt);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");

  // From the top

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/schema", "/$defs/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/schema/$schema", "/$defs/schema/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/schema/$id", "/$defs/schema/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/schema/items", "/$defs/schema/items",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");

  // From within

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/$defs/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$schema", "/$defs/schema/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/$defs/schema/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "https://example.com#/items", "/$defs/schema/items",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "#/$defs/schema/items", std::nullopt,
                          "/$defs/schema/items", "#/$defs/schema/items");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/schema/$schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          std::nullopt,
                          "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame_2020_12, zero_paths) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver, std::nullopt,
                std::nullopt, {});

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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver, std::nullopt,
                std::nullopt, {sourcemeta::core::Pointer{"wrapper"}});

  EXPECT_EQ(frame.locations().size(), 8);

  // From the nested identifier

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema", std::nullopt, "/wrapper",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id", std::nullopt,
      "/wrapper/$id", "https://www.sourcemeta.com/schema", "/$id", {},
      "/wrapper");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema", std::nullopt,
      "/wrapper/$schema", "https://www.sourcemeta.com/schema", "/$schema", {},
      "/wrapper");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$ref", std::nullopt,
      "/wrapper/$ref", "https://www.sourcemeta.com/schema", "/$ref", {},
      "/wrapper");

  // From the root

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$id", "/wrapper/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$schema", "/wrapper/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper");

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
}

TEST(JSONSchema_frame_2020_12,
     single_nested_path_recursive_without_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/wrapper"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "https://json-schema.org/draft/2020-12/schema", std::nullopt,
                {sourcemeta::core::Pointer{"wrapper"}});

  EXPECT_EQ(frame.locations().size(), 2);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  // Without an identifier, the reference goes from the root
  EXPECT_STATIC_REFERENCE(frame, "/wrapper/$ref", "#/wrapper", std::nullopt,
                          "/wrapper", "#/wrapper");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "https://json-schema.org/draft/2020-12/schema", std::nullopt,
                {sourcemeta::core::Pointer{"wrapper"}});

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper/items", "/wrapper/items",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/~?items~/~I~"},
      "/wrapper");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/items/$id", "/wrapper/items/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper/items");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/items/$schema", "/wrapper/items/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper/items");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/items/$anchor", "/wrapper/items/$anchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper/items");

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema", std::nullopt,
      "/wrapper/items", "https://www.sourcemeta.com/schema", "",
      {"/~?items~/~I~"}, "/wrapper");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id", std::nullopt,
      "/wrapper/items/$id", "https://www.sourcemeta.com/schema", "/$id", {},
      "/wrapper/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema", std::nullopt,
      "/wrapper/items/$schema", "https://www.sourcemeta.com/schema", "/$schema",
      {}, "/wrapper/items");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$anchor", std::nullopt,
      "/wrapper/items/$anchor", "https://www.sourcemeta.com/schema", "/$anchor",
      {}, "/wrapper/items");

  // Anchors

  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#test", std::nullopt,
      "/wrapper/items", "https://www.sourcemeta.com/schema", "",
      {"/~?items~/~I~"}, "/wrapper");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(frame, "/wrapper/items/$schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          std::nullopt,
                          "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "https://json-schema.org/draft/2020-12/schema", std::nullopt,
                {sourcemeta::core::Pointer{"wrapper"},
                 sourcemeta::core::Pointer{"common", "test"},
                 sourcemeta::core::Pointer{"common", "with-id"}});

  EXPECT_EQ(frame.locations().size(), 17);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper");

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/common/test", "/common/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/test/$anchor", "/common/test/$anchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/common/test");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/test/$ref", "/common/test/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/common/test");

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/common/with-id", "/common/with-id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/with-id/$id", "/common/with-id/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/common/with-id");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/with-id/$schema", "/common/with-id/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/common/with-id");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/with-id/$anchor", "/common/with-id/$anchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/common/with-id");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/with-id/type", "/common/with-id/type",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/common/with-id");

  EXPECT_FRAME_STATIC_2020_12_RESOURCE(
      frame, "https://www.sourcemeta.com/schema", std::nullopt,
      "/common/with-id", "https://www.sourcemeta.com/schema", "", {""},
      std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id", std::nullopt,
      "/common/with-id/$id", "https://www.sourcemeta.com/schema", "/$id", {},
      "/common/with-id");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema", std::nullopt,
      "/common/with-id/$schema", "https://www.sourcemeta.com/schema",
      "/$schema", {}, "/common/with-id");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$anchor", std::nullopt,
      "/common/with-id/$anchor", "https://www.sourcemeta.com/schema",
      "/$anchor", {}, "/common/with-id");
  EXPECT_FRAME_STATIC_2020_12_POINTER(
      frame, "https://www.sourcemeta.com/schema#/type", std::nullopt,
      "/common/with-id/type", "https://www.sourcemeta.com/schema", "/type", {},
      "/common/with-id");

  // Anchors

  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#foo", "/common/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_2020_12_ANCHOR(
      frame, "https://www.sourcemeta.com/schema#bar", std::nullopt,
      "/common/with-id", "https://www.sourcemeta.com/schema", "", {""},
      std::nullopt);

  // References

  EXPECT_EQ(frame.references().size(), 3);

  EXPECT_STATIC_REFERENCE(frame, "/wrapper/$ref", "#/common/test", std::nullopt,
                          "/common/test", "#/common/test");
  EXPECT_STATIC_REFERENCE(frame, "/common/test/$ref", "#/common/with-id",
                          std::nullopt, "/common/with-id", "#/common/with-id");
  EXPECT_STATIC_REFERENCE(frame, "/common/with-id/$schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          "https://json-schema.org/draft/2020-12/schema",
                          std::nullopt,
                          "https://json-schema.org/draft/2020-12/schema");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "https://json-schema.org/draft/2020-12/schema", std::nullopt,
                {sourcemeta::core::Pointer{"wrapper"},
                 sourcemeta::core::Pointer{"common", "test"}});

  EXPECT_EQ(frame.locations().size(), 4);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper");

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/common/test", "/common/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/test/$ref", "/common/test/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/common/test");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(frame, "/wrapper/$ref", "#/common/test", std::nullopt,
                          "/common/test", "#/common/test");
  EXPECT_STATIC_REFERENCE(frame, "/common/test/$ref", "#/common/with-id",
                          std::nullopt, "/common/with-id", "#/common/with-id");
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
      sourcemeta::core::SchemaFrame::Mode::Instances};

  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver,
                             std::nullopt, std::nullopt,
                             {sourcemeta::core::Pointer{"wrapper"},
                              sourcemeta::core::Pointer{"common", "test"},
                              sourcemeta::core::Pointer{"common", "with-id"}}),
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
      sourcemeta::core::SchemaFrame::Mode::Instances};

  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver,
                             std::nullopt, std::nullopt,
                             {sourcemeta::core::Pointer{"common", "foo"},
                              sourcemeta::core::Pointer{"common", "bar"}}),
               sourcemeta::core::SchemaError);
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
      sourcemeta::core::SchemaFrame::Mode::Instances};

  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver,
                             "https://json-schema.org/draft/2020-12/schema",
                             std::nullopt,
                             {sourcemeta::core::Pointer{"common", "foo"},
                              sourcemeta::core::Pointer{"common", "bar"}}),
               sourcemeta::core::SchemaError);
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "https://json-schema.org/draft/2020-12/schema",
                // The default id should be getting ignored on nested schemas
                // as it only makes sense for top-level framing
                "https://www.example.com",
                {sourcemeta::core::Pointer{"wrapper"},
                 sourcemeta::core::Pointer{"common", "test"}});

  EXPECT_EQ(frame.locations().size(), 4);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/wrapper", "/wrapper",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/wrapper/$ref", "/wrapper/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/wrapper");

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/common/test", "/common/test",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/common/test/$ref", "/common/test/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/common/test");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(frame, "/wrapper/$ref", "#/common/test", std::nullopt,
                          "/common/test", "#/common/test");
  EXPECT_STATIC_REFERENCE(frame, "/common/test/$ref", "#/common/with-id",
                          std::nullopt, "/common/with-id", "#/common/with-id");
}
