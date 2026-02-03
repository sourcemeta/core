#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

#define EXPECT_FRAME_STATIC_DRAFT3_POINTER(                                    \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name,        \
    expected_orphan)                                                           \
  EXPECT_FRAME_STATIC_POINTER(frame, reference, root_id, expected_pointer,     \
                              "http://json-schema.org/draft-03/schema#",       \
                              JSON_Schema_Draft_3, expected_base,              \
                              expected_relative_pointer, expected_parent,      \
                              expected_property_name, expected_orphan);

#define EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(                                   \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name,        \
    expected_orphan)                                                           \
  EXPECT_FRAME_STATIC_RESOURCE(frame, reference, root_id, expected_pointer,    \
                               "http://json-schema.org/draft-03/schema#",      \
                               JSON_Schema_Draft_3, expected_base,             \
                               expected_relative_pointer, expected_parent,     \
                               expected_property_name, expected_orphan);

#define EXPECT_FRAME_STATIC_DRAFT3_SUBSCHEMA(                                  \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_parent, expected_property_name,        \
    expected_orphan)                                                           \
  EXPECT_FRAME_STATIC_SUBSCHEMA(frame, reference, root_id, expected_pointer,   \
                                "http://json-schema.org/draft-03/schema#",     \
                                JSON_Schema_Draft_3, expected_base,            \
                                expected_relative_pointer, expected_parent,    \
                                expected_property_name, expected_orphan);

TEST(JSONSchema_frame_draft3, anonymous_with_nested_schema_resource) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "additionalProperties": { "id": "https://example.com" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/additionalProperties",
      "http://json-schema.org/draft-03/schema#", JSON_Schema_Draft_3, "", false,
      false);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/id", "/additionalProperties/id",
      "http://json-schema.org/draft-03/schema#", JSON_Schema_Draft_3,
      "/additionalProperties", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalProperties", "/additionalProperties",
      "http://json-schema.org/draft-03/schema#", JSON_Schema_Draft_3, "", false,
      false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalProperties/id", "/additionalProperties/id",
      "http://json-schema.org/draft-03/schema#", JSON_Schema_Draft_3,
      "/additionalProperties", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://example.com",
                                  frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "#/additionalProperties",
                                  frame.root());

  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static, "", "https://example.com");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://example.com",
                                  "https://example.com");
}

TEST(JSONSchema_frame_draft3, empty_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/id",
      "https://www.sourcemeta.com/schema", "/id",
      "https://www.sourcemeta.com/schema", "/id", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema", frame.root());
}

TEST(JSONSchema_frame_draft3, empty_schema_trailing_slash) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/schema#",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/id",
      "https://www.sourcemeta.com/schema", "/id",
      "https://www.sourcemeta.com/schema", "/id", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema", frame.root());
}

TEST(JSONSchema_frame_draft3, one_level_applicators_without_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-03/schema#",
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
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/id",
      "https://www.sourcemeta.com/schema", "/id",
      "https://www.sourcemeta.com/schema", "/id", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/schema", "/items/type", "/items", false,
      false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "/properties/foo", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/items", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo",
      frame.root());

  EXPECT_FRAME_LOCATION_NON_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema#/items");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/schema#/items",
                                  "https://www.sourcemeta.com/schema#/items");

  EXPECT_FRAME_LOCATION_NON_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema#/properties/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema#/properties/foo");
}

TEST(JSONSchema_frame_draft3, one_level_applicators_with_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/test/qux",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": { "id": "../foo", "type": "string" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/test/qux",
      "https://www.sourcemeta.com/test/qux", "",
      "https://www.sourcemeta.com/test/qux", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", "", false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/id",
      "https://www.sourcemeta.com/test/qux", "/id",
      "https://www.sourcemeta.com/test/qux", "/id", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test/qux#/items",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/id",
      "https://www.sourcemeta.com/test/qux", "/items/id",
      "https://www.sourcemeta.com/foo", "/id", "/items", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/foo#/id",
      "https://www.sourcemeta.com/test/qux", "/items/id",
      "https://www.sourcemeta.com/foo", "/id", "/items", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/foo#/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/test/qux", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/foo", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test/qux#/items",
                                  frame.root());

  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static,
                                      "https://www.sourcemeta.com/test/qux",
                                      "https://www.sourcemeta.com/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo",
                                  "https://www.sourcemeta.com/foo");
}

TEST(JSONSchema_frame_draft3, subschema_absolute_identifier) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": {
      "id": "https://www.sourcemeta.com/foo",
      "type": "string"
     }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 9);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", "", false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/id",
      "https://www.sourcemeta.com/schema", "/id",
      "https://www.sourcemeta.com/schema", "/id", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/id",
      "https://www.sourcemeta.com/schema", "/items/id",
      "https://www.sourcemeta.com/foo", "/id", "/items", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/foo#/id",
      "https://www.sourcemeta.com/schema", "/items/id",
      "https://www.sourcemeta.com/foo", "/id", "/items", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/foo#/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", "/items", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/foo", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/items", frame.root());

  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static,
                                      "https://www.sourcemeta.com/schema",
                                      "https://www.sourcemeta.com/foo");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/foo",
                                  "https://www.sourcemeta.com/foo");
}

TEST(JSONSchema_frame_draft3, id_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": { "id": "schema" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_walker,
                             sourcemeta::core::schema_resolver),
               sourcemeta::core::SchemaFrameError);
}

TEST(JSONSchema_frame_draft3, explicit_argument_id_same) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "http://json-schema.org/draft-03/schema#",
                "https://www.sourcemeta.com/schema");

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/id",
      "https://www.sourcemeta.com/schema", "/id",
      "https://www.sourcemeta.com/schema", "/id", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema", frame.root());
}

TEST(JSONSchema_frame_draft3, explicit_argument_id_different) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "properties": {
      "one": {
        "id": "test"
      },
      "two": {
        "id": "https://www.test.com"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver,
                "http://json-schema.org/draft-03/schema#",
                "https://www.example.com");

  EXPECT_EQ(frame.locations().size(), 22);

  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.sourcemeta.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.example.com", "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.example.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://www.test.com", "https://www.sourcemeta.com/schema",
      "/properties/two", "https://www.test.com", "", "", false, false);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/id",
      "https://www.sourcemeta.com/schema", "/id",
      "https://www.sourcemeta.com/schema", "/id", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/one",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/one/id",
      "https://www.sourcemeta.com/schema", "/properties/one/id",
      "https://www.sourcemeta.com/test", "/id", "/properties/one", false,
      false);
  EXPECT_FRAME_STATIC_DRAFT3_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/two",
      "https://www.sourcemeta.com/schema", "/properties/two",
      "https://www.test.com", "", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/two/id",
      "https://www.sourcemeta.com/schema", "/properties/two/id",
      "https://www.test.com", "/id", "/properties/two", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.sourcemeta.com/test#/id",
      "https://www.sourcemeta.com/schema", "/properties/one/id",
      "https://www.sourcemeta.com/test", "/id", "/properties/one", false,
      false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://www.test.com#/id", "https://www.sourcemeta.com/schema",
      "/properties/two/id", "https://www.test.com", "/id", "/properties/two",
      false, false);

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/test", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.example.com",
                                  frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.example.com/test",
                                  frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.test.com",
                                  frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/one",
      frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.sourcemeta.com/schema#/properties/two",
      frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/one", frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://www.example.com#/properties/two", frame.root());

  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static,
                                      "https://www.sourcemeta.com/schema",
                                      "https://www.sourcemeta.com/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://www.sourcemeta.com/test",
                                  "https://www.sourcemeta.com/test");

  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static,
                                      "https://www.sourcemeta.com/schema",
                                      "https://www.example.com/test");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.example.com/test",
                                  "https://www.example.com/test");

  EXPECT_FRAME_LOCATION_NON_REACHABLE(frame, Static,
                                      "https://www.sourcemeta.com/schema",
                                      "https://www.test.com");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://www.test.com",
                                  "https://www.test.com");
}

TEST(JSONSchema_frame_draft3, ref_metaschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "http://json-schema.org/draft-03/schema#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "", frame.root());
}

TEST(JSONSchema_frame_draft3, ref_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "#/definitions/string"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 4);

  // Note that `$ref` MUST override EVERY sibling keyword, so the `id`
  // here is not considered at all
  //
  // However, note that we DO respect `$schema` at the top. This is because
  // we START by checking `$schema` to figure out that `$ref` overrides,
  // so we do know about the dialect anyway
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, std::nullopt, false, false);

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/id", "/id", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "#/definitions/string", "",
                          "/definitions/string", "#/definitions/string");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "", frame.root());
}

TEST(JSONSchema_frame_draft3, top_level_relative_ref_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "id": "https://example.com/foo",
    "$ref": "bar"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 4);

  // Note that `id` is IGNORED given the sibling `$ref`
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, std::nullopt, false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/id", "/id", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "http://json-schema.org/draft-03/schema#",
      JSON_Schema_Draft_3, "", false, false);

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "bar", "bar", std::nullopt, "bar");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "", frame.root());
}

TEST(JSONSchema_frame_draft3, nested_relative_ref_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "id": "https://example.com",
    "additionalProperties": {
      "id": "https://nested.com",
      "$ref": "bar"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_walker,
                sourcemeta::core::schema_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_FRAME_STATIC_DRAFT3_RESOURCE(
      frame, "https://example.com", "https://example.com", "",
      "https://example.com", "", std::nullopt, false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://example.com#/id", "https://example.com", "/id",
      "https://example.com", "/id", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://example.com#/$schema", "https://example.com", "/$schema",
      "https://example.com", "/$schema", "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_SUBSCHEMA(
      frame, "https://example.com#/additionalProperties", "https://example.com",
      "/additionalProperties", "https://example.com", "/additionalProperties",
      "", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://example.com#/additionalProperties/id",
      "https://example.com", "/additionalProperties/id", "https://example.com",
      "/additionalProperties/id", "/additionalProperties", false, false);
  EXPECT_FRAME_STATIC_DRAFT3_POINTER(
      frame, "https://example.com#/additionalProperties/$ref",
      "https://example.com", "/additionalProperties/$ref",
      "https://example.com", "/additionalProperties/$ref",
      "/additionalProperties", false, false);

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-03/schema",
      "http://json-schema.org/draft-03/schema", std::nullopt,
      "http://json-schema.org/draft-03/schema#");
  EXPECT_STATIC_REFERENCE(frame, "/additionalProperties/$ref",
                          "https://example.com/bar", "https://example.com/bar",
                          std::nullopt, "bar");

  // Reachability

  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static, "https://example.com",
                                  frame.root());
  EXPECT_FRAME_LOCATION_REACHABLE(
      frame, Static, "https://example.com#/additionalProperties", frame.root());

  EXPECT_FRAME_LOCATION_NON_REACHABLE(
      frame, Static, "https://example.com",
      "https://example.com#/additionalProperties");
  EXPECT_FRAME_LOCATION_REACHABLE(frame, Static,
                                  "https://example.com#/additionalProperties",
                                  "https://example.com#/additionalProperties");
}
