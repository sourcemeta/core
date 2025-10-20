#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

#define EXPECT_FRAME_STATIC_DRAFT7_POINTER(                                    \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_STATIC_POINTER(frame, reference, root_id, expected_pointer,     \
                              "http://json-schema.org/draft-07/schema#",       \
                              "http://json-schema.org/draft-07/schema#",       \
                              expected_base, expected_relative_pointer,        \
                              expected_instance_locations, expected_parent);

#define EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(                                   \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_STATIC_RESOURCE(frame, reference, root_id, expected_pointer,    \
                               "http://json-schema.org/draft-07/schema#",      \
                               "http://json-schema.org/draft-07/schema#",      \
                               expected_base, expected_relative_pointer,       \
                               expected_instance_locations, expected_parent);

#define EXPECT_FRAME_STATIC_DRAFT7_ANCHOR(                                     \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_STATIC_ANCHOR(frame, reference, root_id, expected_pointer,      \
                             "http://json-schema.org/draft-07/schema#",        \
                             "http://json-schema.org/draft-07/schema#",        \
                             expected_base, expected_relative_pointer,         \
                             expected_instance_locations, expected_parent);

#define EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(                                  \
    frame, reference, root_id, expected_pointer, expected_base,                \
    expected_relative_pointer, expected_instance_locations, expected_parent)   \
  EXPECT_FRAME_STATIC_SUBSCHEMA(frame, reference, root_id, expected_pointer,   \
                                "http://json-schema.org/draft-07/schema#",     \
                                "http://json-schema.org/draft-07/schema#",     \
                                expected_base, expected_relative_pointer,      \
                                expected_instance_locations, expected_parent);

TEST(JSONSchema_frame_draft7, anonymous_with_nested_schema_resource) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "additionalProperties": { "$id": "https://example.com" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 6);

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/additionalProperties",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#",
      {"/~?additionalProperties~/~P~"}, "");

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/additionalProperties/$id",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/additionalProperties");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/additionalProperties", "/additionalProperties",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#",
      {"/~?additionalProperties~/~P~"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/additionalProperties/$id", "/additionalProperties/$id",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/additionalProperties");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, empty_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-07/schema#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, empty_schema_trailing_hash) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema#",
    "$schema": "http://json-schema.org/draft-07/schema#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, one_level_applicators_without_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-07/schema#",
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
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/schema", "/items", {"/~I~"}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/schema", "/items/type", {}, "/items");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo",
      "https://www.sourcemeta.com/schema", "/properties/foo", {"/foo"}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type",
      "https://www.sourcemeta.com/schema", "/properties/foo/type", {},
      "/properties/foo");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, one_level_applicators_with_identifiers) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test/qux",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "items": { "$id": "../foo", "type": "string" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 9);
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/test/qux",
      "https://www.sourcemeta.com/test/qux", "",
      "https://www.sourcemeta.com/test/qux", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", {"/~I~"}, "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$id",
      "https://www.sourcemeta.com/test/qux", "/$id",
      "https://www.sourcemeta.com/test/qux", "/$id", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema",
      "https://www.sourcemeta.com/test/qux", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test/qux#/items",
      "https://www.sourcemeta.com/test/qux", "/items",
      "https://www.sourcemeta.com/foo", "", {"/~I~"}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/test/qux#/items/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", {}, "/items");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/test/qux", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/foo#/type",
      "https://www.sourcemeta.com/test/qux", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", {}, "/items");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, subschema_absolute_identifier) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-07/schema#",
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
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/foo",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", {"/~I~"}, "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://www.sourcemeta.com/foo", "", {"/~I~"}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/$id",
      "https://www.sourcemeta.com/schema", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", {}, "/items");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$id",
      "https://www.sourcemeta.com/schema", "/items/$id",
      "https://www.sourcemeta.com/foo", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/foo#/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://www.sourcemeta.com/foo", "/type", {}, "/items");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, id_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "items": { "$id": "schema" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaError);
}

TEST(JSONSchema_frame_draft7, explicit_argument_id_same) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-07/schema#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "http://json-schema.org/draft-07/schema#",
                "https://www.sourcemeta.com/schema");

  EXPECT_EQ(frame.locations().size(), 3);
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, explicit_argument_id_different) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "properties": {
      "one": {
        "$id": "test"
      },
      "two": {
        "$id": "https://www.test.com"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "http://json-schema.org/draft-07/schema#",
                "https://www.example.com");

  EXPECT_EQ(frame.locations().size(), 22);

  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.sourcemeta.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", {"/one"}, "");
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.example.com", "https://www.sourcemeta.com/schema", "",
      "https://www.sourcemeta.com/schema", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.example.com/test",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.example.com/test", "", {"/one"}, "");
  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(
      frame, "https://www.test.com", "https://www.sourcemeta.com/schema",
      "/properties/two", "https://www.test.com", "", {"/two"}, "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$id",
      "https://www.sourcemeta.com/schema", "/$id",
      "https://www.sourcemeta.com/schema", "/$id", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties",
      "https://www.sourcemeta.com/schema", "/properties",
      "https://www.sourcemeta.com/schema", "/properties", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/one",
      "https://www.sourcemeta.com/schema", "/properties/one",
      "https://www.sourcemeta.com/test", "", {"/one"}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/one/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.sourcemeta.com/test", "/$id", {}, "/properties/one");
  EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/properties/two",
      "https://www.sourcemeta.com/schema", "/properties/two",
      "https://www.test.com", "", {"/two"}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/schema#/properties/two/$id",
      "https://www.sourcemeta.com/schema", "/properties/two/$id",
      "https://www.test.com", "/$id", {}, "/properties/two");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.sourcemeta.com/test#/$id",
      "https://www.sourcemeta.com/schema", "/properties/one/$id",
      "https://www.sourcemeta.com/test", "/$id", {}, "/properties/one");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "https://www.test.com#/$id", "https://www.sourcemeta.com/schema",
      "/properties/two/$id", "https://www.test.com", "/$id", {},
      "/properties/two");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, ref_metaschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "$ref": "http://json-schema.org/draft-07/schema#"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
  EXPECT_STATIC_REFERENCE(
      frame, "/$ref", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, location_independent_identifier_anonymous) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "definitions": {
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
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 8);

  // Pointers
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions", "/definitions",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");

  // Foo
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/definitions/foo", "/definitions/foo",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions/foo/$id", "/definitions/foo/$id",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/definitions/foo");

  // Bar
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/definitions/bar", "/definitions/bar",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions/bar/$ref", "/definitions/bar/$ref",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/definitions/bar");

  // Anchors
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#foo", "/definitions/foo",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
  EXPECT_STATIC_REFERENCE(frame, "/definitions/bar/$ref", "#foo", std::nullopt,
                          "foo", "#foo");
}

TEST(JSONSchema_frame_draft7, ref_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "$ref": "#/definitions/string",
    "definitions": {
      "string": { "type": "string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 7);

  // JSON Pointers

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$id", "/$id", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$ref", "/$ref", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions", "/definitions",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/definitions/string", "/definitions/string",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {""}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions/string/type", "/definitions/string/type",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/definitions/string");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "#/definitions/string", std::nullopt,
                          "/definitions/string", "#/definitions/string");
}

TEST(JSONSchema_frame_draft7, relative_base_uri_without_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "$id": "common"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 3);

  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(frame, "common", "common", "", "common",
                                      "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(frame, "common#/$schema", "common",
                                     "/$schema", "common", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(frame, "common#/$id", "common", "/$id",
                                     "common", "/$id", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_frame_draft7, relative_base_uri_with_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "$id": "common",
    "allOf": [ { "$ref": "#foo" } ],
    "definitions": {
      "foo": {
        "$id": "#foo"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_FRAME_STATIC_DRAFT7_RESOURCE(frame, "common", "common", "", "common",
                                      "", {""}, std::nullopt);

  // Anchors
  EXPECT_FRAME_STATIC_DRAFT7_ANCHOR(frame, "common#foo", "common",
                                    "/definitions/foo", "common",
                                    "/definitions/foo", {""}, "");

  // JSON Pointers

  EXPECT_FRAME_STATIC_DRAFT7_POINTER(frame, "common#/$schema", "common",
                                     "/$schema", "common", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(frame, "common#/$id", "common", "/$id",
                                     "common", "/$id", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(frame, "common#/allOf", "common", "/allOf",
                                     "common", "/allOf", {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(frame, "common#/allOf/0", "common",
                                       "/allOf/0", "common", "/allOf/0", {""},
                                       "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(frame, "common#/allOf/0/$ref", "common",
                                     "/allOf/0/$ref", "common", "/allOf/0/$ref",
                                     {}, "/allOf/0");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(frame, "common#/definitions", "common",
                                     "/definitions", "common", "/definitions",
                                     {}, "");
  EXPECT_FRAME_STATIC_DRAFT7_SUBSCHEMA(frame, "common#/definitions/foo",
                                       "common", "/definitions/foo", "common",
                                       "/definitions/foo", {""}, "");
  EXPECT_FRAME_STATIC_DRAFT7_POINTER(
      frame, "common#/definitions/foo/$id", "common", "/definitions/foo/$id",
      "common", "/definitions/foo/$id", {}, "/definitions/foo");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
  EXPECT_STATIC_REFERENCE(frame, "/allOf/0/$ref", "common#foo", "common", "foo",
                          "#foo");
}

TEST(JSONSchema_frame_draft7, ref_invalidates_sibling_subschemas_and_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
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
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {""}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties", "/properties",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {"/foo"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$ref", "/properties/foo/$ref",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/properties/foo");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties", "/properties/foo/properties",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/properties/foo");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties/bar",
      "/properties/foo/properties/bar",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/properties/foo");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties/bar/$ref",
      "/properties/foo/properties/bar/$ref",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/properties/foo");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties/bar/additionalProperties",
      "/properties/foo/properties/bar/additionalProperties",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/properties/foo");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/properties/bar/additionalProperties/$ref",
      "/properties/foo/properties/bar/additionalProperties/$ref",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/properties/foo");

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "http://json-schema.org/draft-07/schema",
      "http://json-schema.org/draft-07/schema", std::nullopt,
      "http://json-schema.org/draft-07/schema#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref",
                          "#/definitions/enabled", std::nullopt,
                          "/definitions/enabled", "#/definitions/enabled");
}
