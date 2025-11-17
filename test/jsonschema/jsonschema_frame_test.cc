#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <set>
#include <sstream>

#include "jsonschema_test_utils.h"

TEST(JSONSchema_frame, nested_schemas_mixing_dialects) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "foo": {
        "id": "foo",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "definitions": {
          "bar": {
            "id": "bar",
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

  EXPECT_EQ(frame.locations().size(), 21);

  EXPECT_FRAME_STATIC_RESOURCE(frame, "https://www.sourcemeta.com/test",
                               "https://www.sourcemeta.com/test", "",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://www.sourcemeta.com/test", "", {""},
                               std::nullopt);
  EXPECT_FRAME_STATIC_RESOURCE(frame, "https://www.sourcemeta.com/foo",
                               "https://www.sourcemeta.com/test", "/$defs/foo",
                               "http://json-schema.org/draft-04/schema#",
                               "http://json-schema.org/draft-04/schema#",
                               "https://www.sourcemeta.com/foo", "", {}, "");
  EXPECT_FRAME_STATIC_RESOURCE(
      frame, "https://www.sourcemeta.com/bar",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions/bar",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/bar", "", {}, "/$defs/foo");

  // JSON Pointers

  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.sourcemeta.com/test#/$id",
                              "https://www.sourcemeta.com/test", "/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.sourcemeta.com/test", "/$id", {},
                              "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.sourcemeta.com/test#/$schema",
                              "https://www.sourcemeta.com/test", "/$schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.sourcemeta.com/test", "/$schema", {},
                              "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.sourcemeta.com/test#/$defs",
                              "https://www.sourcemeta.com/test", "/$defs",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.sourcemeta.com/test", "/$defs", {},
                              "");
  EXPECT_FRAME_STATIC_SUBSCHEMA(frame,
                                "https://www.sourcemeta.com/test#/$defs/foo",
                                "https://www.sourcemeta.com/test", "/$defs/foo",
                                "http://json-schema.org/draft-04/schema#",
                                "http://json-schema.org/draft-04/schema#",
                                "https://www.sourcemeta.com/foo", "", {}, "");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/test#/$defs/foo/id",
      "https://www.sourcemeta.com/test", "/$defs/foo/id",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/foo", "/id", {}, "/$defs/foo");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/test#/$defs/foo/$schema",
      "https://www.sourcemeta.com/test", "/$defs/foo/$schema",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/foo", "/$schema", {}, "/$defs/foo");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/test#/$defs/foo/definitions",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/foo", "/definitions", {}, "/$defs/foo");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/test#/$defs/foo/definitions/bar",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions/bar",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/bar", "", {}, "/$defs/foo");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/test#/$defs/foo/definitions/bar/id",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions/bar/id",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/bar", "/id", {},
      "/$defs/foo/definitions/bar");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/test#/$defs/foo/definitions/bar/type",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions/bar/type",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/bar", "/type", {},
      "/$defs/foo/definitions/bar");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/foo#/id",
      "https://www.sourcemeta.com/test", "/$defs/foo/id",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/foo", "/id", {}, "/$defs/foo");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/foo#/$schema",
      "https://www.sourcemeta.com/test", "/$defs/foo/$schema",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/foo", "/$schema", {}, "/$defs/foo");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/foo#/definitions",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/foo", "/definitions", {}, "/$defs/foo");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/foo#/definitions/bar",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions/bar",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/bar", "", {}, "/$defs/foo");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/foo#/definitions/bar/id",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions/bar/id",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/bar", "/id", {},
      "/$defs/foo/definitions/bar");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/foo#/definitions/bar/type",
      "https://www.sourcemeta.com/test", "/$defs/foo/definitions/bar/type",
      "http://json-schema.org/draft-04/schema#",
      "http://json-schema.org/draft-04/schema#",
      "https://www.sourcemeta.com/bar", "/type", {},
      "/$defs/foo/definitions/bar");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.sourcemeta.com/bar#/id",
                              "https://www.sourcemeta.com/test",
                              "/$defs/foo/definitions/bar/id",
                              "http://json-schema.org/draft-04/schema#",
                              "http://json-schema.org/draft-04/schema#",
                              "https://www.sourcemeta.com/bar", "/id", {},
                              "/$defs/foo/definitions/bar");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.sourcemeta.com/bar#/type",
                              "https://www.sourcemeta.com/test",
                              "/$defs/foo/definitions/bar/type",
                              "http://json-schema.org/draft-04/schema#",
                              "http://json-schema.org/draft-04/schema#",
                              "https://www.sourcemeta.com/bar", "/type", {},
                              "/$defs/foo/definitions/bar");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/$defs/foo/$schema", "http://json-schema.org/draft-04/schema",
      "http://json-schema.org/draft-04/schema", std::nullopt,
      "http://json-schema.org/draft-04/schema#");
}

TEST(JSONSchema_frame, nested_schemas_sibling_ref_nested_2020_12_draft7) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/main",
    "$ref": "embedded",
    "$defs": {
      "embedded": {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "$id": "embedded",
        "$ref": "#/definitions/foo",
        "definitions": {
          "foo": { "type": "number" }
        }
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 19);

  // Resources

  EXPECT_FRAME_STATIC_RESOURCE(
      frame, "https://example.com/main", "https://example.com/main", "",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com/main#/$schema",
                              "https://example.com/main", "/$schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com/main", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com/main#/$id",
                              "https://example.com/main", "/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com/main", "/$id", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com/main#/$ref",
                              "https://example.com/main", "/$ref",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com/main", "/$ref", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com/main#/$defs",
                              "https://example.com/main", "/$defs",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com/main", "/$defs", {}, "");

  EXPECT_FRAME_STATIC_RESOURCE(frame, "https://example.com/embedded",
                               "https://example.com/main", "/$defs/embedded",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://example.com/embedded", "", {""}, "");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/embedded#/$schema",
      "https://example.com/main", "/$defs/embedded/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/$schema", {}, "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/embedded#/$id", "https://example.com/main",
      "/$defs/embedded/$id", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/$id", {}, "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/embedded#/$ref", "https://example.com/main",
      "/$defs/embedded/$ref", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/$ref", {}, "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/embedded#/definitions",
      "https://example.com/main", "/$defs/embedded/definitions",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/definitions", {}, "/$defs/embedded");

  // Note that this is still considered to be a subschema, but a 2020-12 one.
  // The logic is that we try to interpret it as Draft 7, given `$schema`, but
  // the `$ref` there overrides the `$id`, therefore it is not a schema
  // resource, and thus `$schema` gets ignored.

  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://example.com/embedded#/definitions/foo",
      "https://example.com/main", "/$defs/embedded/definitions/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/definitions/foo", {""},
      "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/embedded#/definitions/foo/type",
      "https://example.com/main", "/$defs/embedded/definitions/foo/type",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/definitions/foo/type", {},
      "/$defs/embedded/definitions/foo");

  // From the root
  EXPECT_FRAME_STATIC_SUBSCHEMA(frame,
                                "https://example.com/main#/$defs/embedded",
                                "https://example.com/main", "/$defs/embedded",
                                "https://json-schema.org/draft/2020-12/schema",
                                "https://json-schema.org/draft/2020-12/schema",
                                "https://example.com/embedded", "", {""}, "");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/$schema",
      "https://example.com/main", "/$defs/embedded/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/$schema", {}, "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/$id",
      "https://example.com/main", "/$defs/embedded/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/$id", {}, "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/$ref",
      "https://example.com/main", "/$defs/embedded/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/$ref", {}, "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/definitions",
      "https://example.com/main", "/$defs/embedded/definitions",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/definitions", {}, "/$defs/embedded");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://example.com/main#/$defs/embedded/definitions/foo",
      "https://example.com/main", "/$defs/embedded/definitions/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/definitions/foo", {""},
      "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/definitions/foo/type",
      "https://example.com/main", "/$defs/embedded/definitions/foo/type",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/embedded", "/definitions/foo/type", {},
      "/$defs/embedded/definitions/foo");

  // References

  EXPECT_EQ(frame.references().size(), 4);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "https://example.com/embedded",
                          "https://example.com/embedded", std::nullopt,
                          "embedded");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/embedded/$schema",
                          "http://json-schema.org/draft-07/schema",
                          "http://json-schema.org/draft-07/schema",
                          std::nullopt,
                          "http://json-schema.org/draft-07/schema#");

  // Note that the reference keep its base URI because we end up interpreting
  // the embedded schema as 2020-12
  EXPECT_STATIC_REFERENCE(frame, "/$defs/embedded/$ref",
                          "https://example.com/embedded#/definitions/foo",
                          "https://example.com/embedded", "/definitions/foo",
                          "#/definitions/foo");
}

TEST(JSONSchema_frame, nested_schemas_sibling_ref_nested_2020_12_draft4) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/main",
    "$ref": "embedded",
    "$defs": {
      "embedded": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "embedded",
        "$ref": "#/definitions/foo",
        "definitions": {
          "foo": { "type": "number" }
        }
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 12);

  // Resources
  EXPECT_FRAME_STATIC_RESOURCE(
      frame, "https://example.com/main", "https://example.com/main", "",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "", {""}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com/main#/$schema",
                              "https://example.com/main", "/$schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com/main", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com/main#/$id",
                              "https://example.com/main", "/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com/main", "/$id", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com/main#/$ref",
                              "https://example.com/main", "/$ref",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com/main", "/$ref", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com/main#/$defs",
                              "https://example.com/main", "/$defs",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com/main", "/$defs", {}, "");

  // Note that in this case, we DO NOT consider this to be a resource, as we end
  // up interpreting it as 2020-12, where `id` is not a valid keyword
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://example.com/main#/$defs/embedded",
      "https://example.com/main", "/$defs/embedded",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "/$defs/embedded", {}, "");

  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/$schema",
      "https://example.com/main", "/$defs/embedded/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "/$defs/embedded/$schema", {},
      "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/id",
      "https://example.com/main", "/$defs/embedded/id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "/$defs/embedded/id", {}, "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/$ref",
      "https://example.com/main", "/$defs/embedded/$ref",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "/$defs/embedded/$ref", {},
      "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/definitions",
      "https://example.com/main", "/$defs/embedded/definitions",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "/$defs/embedded/definitions", {},
      "/$defs/embedded");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://example.com/main#/$defs/embedded/definitions/foo",
      "https://example.com/main", "/$defs/embedded/definitions/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "/$defs/embedded/definitions/foo", {},
      "/$defs/embedded");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com/main#/$defs/embedded/definitions/foo/type",
      "https://example.com/main", "/$defs/embedded/definitions/foo/type",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/main", "/$defs/embedded/definitions/foo/type", {},
      "/$defs/embedded/definitions/foo");

  // References

  EXPECT_EQ(frame.references().size(), 4);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/$ref", "https://example.com/embedded",
                          "https://example.com/embedded", std::nullopt,
                          "embedded");
  EXPECT_STATIC_REFERENCE(frame, "/$defs/embedded/$schema",
                          "http://json-schema.org/draft-04/schema",
                          "http://json-schema.org/draft-04/schema",
                          std::nullopt,
                          "http://json-schema.org/draft-04/schema#");

  // Note that this reference is interpreted from the root, as we end up trying
  // to match `id` against 2020-12, which does not exist
  EXPECT_STATIC_REFERENCE(frame, "/$defs/embedded/$ref",
                          "https://example.com/main#/definitions/foo",
                          "https://example.com/main", "/definitions/foo",
                          "#/definitions/foo");
}

TEST(JSONSchema_frame, no_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$anchor": "foo",
        "type": "string"
      },
      "bar": {
        "$id": "https://example.com",
        "$anchor": "bar"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 14);

  // Top level

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

  // Foo

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/foo"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/$anchor", "/properties/foo/$anchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/properties/foo");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/foo/type", "/properties/foo/type",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/properties/foo");
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "#foo", "/properties/foo",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/foo"}, "");

  // Bar

  EXPECT_ANONYMOUS_FRAME_STATIC_RESOURCE(
      frame, "https://example.com", "/properties/bar",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/bar"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_ANCHOR(
      frame, "https://example.com#bar", "/properties/bar",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/bar"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$id", "/properties/bar/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/properties/bar");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "https://example.com#/$anchor", "/properties/bar/$anchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/properties/bar");

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/properties/bar", "/properties/bar",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {"/bar"}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/bar/$id", "/properties/bar/$id",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/properties/bar");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/properties/bar/$anchor", "/properties/bar/$anchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/properties/bar");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame, no_id_with_default) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "type": "string" }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver,
                "https://json-schema.org/draft/2020-12/schema",
                "https://www.sourcemeta.com/schema");

  EXPECT_EQ(frame.locations().size(), 4);
  EXPECT_FRAME_STATIC_RESOURCE(frame, "https://www.sourcemeta.com/schema",
                               "https://www.sourcemeta.com/schema", "",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://www.sourcemeta.com/schema", "", {""},
                               std::nullopt);
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/schema#/$schema",
      "https://www.sourcemeta.com/schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://www.sourcemeta.com/schema", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://www.sourcemeta.com/schema#/items",
      "https://www.sourcemeta.com/schema", "/items",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://www.sourcemeta.com/schema", "/items", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.sourcemeta.com/schema#/items/type",
      "https://www.sourcemeta.com/schema", "/items/type",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://www.sourcemeta.com/schema", "/items/type", {}, "/items");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame, id_with_default_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "additionalProperties": {
      "type": "string"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver, std::nullopt,
                "https://other.com");

  EXPECT_EQ(frame.locations().size(), 10);

  EXPECT_FRAME_STATIC_RESOURCE(frame, "https://example.com",
                               "https://example.com", "",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://example.com", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com#/$id",
                              "https://example.com", "/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com", "/$id", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://example.com#/$schema",
                              "https://example.com", "/$schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://example.com#/additionalProperties", "https://example.com",
      "/additionalProperties", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://example.com",
      "/additionalProperties", {"/~?additionalProperties~/~P~"}, "");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://example.com#/additionalProperties/type",
      "https://example.com", "/additionalProperties/type",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://example.com",
      "/additionalProperties/type", {}, "/additionalProperties");

  EXPECT_FRAME_STATIC_RESOURCE(frame, "https://other.com",
                               "https://example.com", "",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://example.com", "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_POINTER(frame, "https://other.com#/$id",
                              "https://example.com", "/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com", "/$id", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://other.com#/$schema",
                              "https://example.com", "/$schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://example.com", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://other.com#/additionalProperties", "https://example.com",
      "/additionalProperties", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://example.com",
      "/additionalProperties", {"/~?additionalProperties~/~P~"}, "");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://other.com#/additionalProperties/type",
      "https://example.com", "/additionalProperties/type",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://example.com",
      "/additionalProperties/type", {}, "/additionalProperties");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame, cross_2020_12_to_2019_09_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2019-09/schema"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  // Top level

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");

  // Subschema

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/schema", "/$defs/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/schema/$schema", "/$defs/schema/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");
}

TEST(JSONSchema_frame, cross_2020_12_to_draft7_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "schema": {
        "$schema": "http://json-schema.org/draft-07/schema#"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  // Top level

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");

  // Note that `$schema` without an identifier is NOT allowed
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/schema", "/$defs/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/schema/$schema", "/$defs/schema/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");
}

TEST(JSONSchema_frame, cross_2020_12_to_draft6_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "schema": {
        "$schema": "http://json-schema.org/draft-06/schema#"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  // Top level

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");

  // Note that `$schema` without an identifier is NOT allowed
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/schema", "/$defs/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/schema/$schema", "/$defs/schema/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");
}

TEST(JSONSchema_frame, cross_2020_12_to_draft4_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "schema": {
        "$schema": "http://json-schema.org/draft-04/schema#"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  // Top level

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs", "/$defs",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");

  // Note that `$schema` without an identifier is NOT allowed
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/$defs/schema", "/$defs/schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$defs/schema/$schema", "/$defs/schema/$schema",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", {}, "/$defs/schema");
}

TEST(JSONSchema_frame, cross_draft7_to_draft4_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "definitions": {
      "schema": {
        "$schema": "http://json-schema.org/draft-04/schema#"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  // Top level

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions", "/definitions",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");

  // Note that `$schema` without an identifier is NOT allowed
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/definitions/schema", "/definitions/schema",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions/schema/$schema", "/definitions/schema/$schema",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/definitions/schema");
}

TEST(JSONSchema_frame, cross_draft7_to_2020_12_without_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "definitions": {
      "schema": {
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 5);

  // Top level

  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "", "", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, std::nullopt);
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/$schema", "/$schema", "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions", "/definitions",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");

  // Subschema

  // Note that `$schema` without an identifier is NOT allowed
  EXPECT_ANONYMOUS_FRAME_STATIC_SUBSCHEMA(
      frame, "#/definitions/schema", "/definitions/schema",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "");
  EXPECT_ANONYMOUS_FRAME_STATIC_POINTER(
      frame, "#/definitions/schema/$schema", "/definitions/schema/$schema",
      "http://json-schema.org/draft-07/schema#",
      "http://json-schema.org/draft-07/schema#", {}, "/definitions/schema");
}

TEST(JSONSchema_frame, anchor_on_absolute_subid) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "$id": "https://www.example.org",
      "items": {
        "$anchor": "foo"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.locations().size(), 12);
  EXPECT_FRAME_STATIC_RESOURCE(
      frame, "https://www.example.com", "https://www.example.com", "",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.com",
      "", {""}, std::nullopt);
  EXPECT_FRAME_STATIC_RESOURCE(
      frame, "https://www.example.org", "https://www.example.com", "/items",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.org",
      "", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_ANCHOR(
      frame, "https://www.example.org#foo", "https://www.example.com",
      "/items/items", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.org",
      "/items", {"/~?items~/~I~/~?items~/~I~"}, "/items");

  // JSON Pointers

  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/$id",
                              "https://www.example.com", "/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/$id", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/$schema",
                              "https://www.example.com", "/$schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/$schema", {}, "");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://www.example.com#/items", "https://www.example.com",
      "/items", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.org",
      "", {"/~?items~/~I~"}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/items/$id",
                              "https://www.example.com", "/items/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.org", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://www.example.com#/items/items", "https://www.example.com",
      "/items/items", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.org",
      "/items", {"/~?items~/~I~/~?items~/~I~"}, "/items");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.example.com#/items/items/$anchor",
      "https://www.example.com", "/items/items/$anchor",
      "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.org",
      "/items/$anchor", {}, "/items/items");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.org#/$id",
                              "https://www.example.com", "/items/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.org", "/$id", {}, "/items");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://www.example.org#/items", "https://www.example.com",
      "/items/items", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.org",
      "/items", {"/~?items~/~I~/~?items~/~I~"}, "/items");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.org#/items/$anchor",
                              "https://www.example.com", "/items/items/$anchor",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.org", "/items/$anchor", {},
                              "/items/items");

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame, uri_iterators) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "$id": "test",
      "$anchor": "foo",
      "type": "string"
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  std::set<std::string> uris;
  for (const auto &entry : frame.locations()) {
    uris.insert(entry.first.second);
  }

  EXPECT_EQ(frame.locations().size(), 12);
  EXPECT_EQ(uris.size(), 12);

  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/schema"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/test"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/test#foo"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/schema#/$id"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/schema#/$schema"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/schema#/items"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/schema#/items/$id"));
  EXPECT_TRUE(
      uris.contains("https://www.sourcemeta.com/schema#/items/$anchor"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/schema#/items/type"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/test#/$id"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/test#/$anchor"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/test#/type"));

  // References

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame, no_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "type": "string" },
      "bar": { "type": "number" },
      "baz": { "type": "array" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame, refs_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "#" },
      "bar": { "$ref": "#/properties/baz" },
      "baz": {
        "$anchor": "baz",
        "type": "string"
      },
      "qux": {
        "$id": "test",
        "$ref": "#"
      },
      "anchor": {
        "$ref": "#baz"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.references().size(), 5);
  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/properties/foo/$ref", "https://www.sourcemeta.com/schema",
      "https://www.sourcemeta.com/schema", std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$ref",
                          "https://www.sourcemeta.com/schema#/properties/baz",
                          "https://www.sourcemeta.com/schema",
                          "/properties/baz", "#/properties/baz");
  EXPECT_STATIC_REFERENCE(frame, "/properties/qux/$ref",
                          "https://www.sourcemeta.com/test",
                          "https://www.sourcemeta.com/test", std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/anchor/$ref",
                          "https://www.sourcemeta.com/schema#baz",
                          "https://www.sourcemeta.com/schema", "baz", "#baz");
}

TEST(JSONSchema_frame, refs_with_no_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "#" },
      "bar": { "$ref": "#/properties/baz" },
      "baz": {
        "$anchor": "baz",
        "type": "string"
      },
      "qux": {
        "$id": "https://www.example.com",
        "$ref": "#"
      },
      "anchor": {
        "$ref": "#baz"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.references().size(), 5);
  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref", "", std::nullopt,
                          std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$ref", "#/properties/baz",
                          std::nullopt, "/properties/baz", "#/properties/baz");
  EXPECT_STATIC_REFERENCE(frame, "/properties/qux/$ref",
                          "https://www.example.com", "https://www.example.com",
                          std::nullopt, "#");
  EXPECT_STATIC_REFERENCE(frame, "/properties/anchor/$ref", "#baz",
                          std::nullopt, "baz", "#baz");
}

TEST(JSONSchema_frame, no_dynamic_ref_on_old_drafts) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": { "$dynamicRef": "#" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.references().size(), 1);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2019-09/schema",
      "https://json-schema.org/draft/2019-09/schema", std::nullopt,
      "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_frame, remote_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "https://www.example.com" },
      "bar": { "$ref": "https://www.example.com/test#foo" },
      "baz": { "$ref": "https://www.example.com/x/y#/foo/bar" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.references().size(), 4);
  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(frame, "/properties/foo/$ref",
                          "https://www.example.com", "https://www.example.com",
                          std::nullopt, "https://www.example.com");
  EXPECT_STATIC_REFERENCE(frame, "/properties/bar/$ref",
                          "https://www.example.com/test#foo",
                          "https://www.example.com/test", "foo",
                          "https://www.example.com/test#foo");
  EXPECT_STATIC_REFERENCE(frame, "/properties/baz/$ref",
                          "https://www.example.com/x/y#/foo/bar",
                          "https://www.example.com/x/y", "/foo/bar",
                          "https://www.example.com/x/y#/foo/bar");
}

TEST(JSONSchema_frame, standalone_with_external_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "https://www.example.com" },
      "bar": { "$ref": "#/properties/foo" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_FALSE(frame.standalone());
}

TEST(JSONSchema_frame, standalone_with_embedded_external_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "https://www.example.com" },
      "bar": { "$ref": "#/properties/foo" }
    },
    "$defs": {
      "https://www.example.com": {
        "$id": "https://www.example.com"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(frame.standalone());
}

TEST(JSONSchema_frame, standalone_with_internal_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "type": "string" },
      "bar": { "$ref": "#/properties/foo" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(frame.standalone());
}

TEST(JSONSchema_frame, standalone_without_refs) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "type": "string" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(frame.standalone());
}

TEST(JSONSchema_frame, standalone_with_draft3_external_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-1" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_FALSE(frame.standalone());
}

TEST(JSONSchema_frame, no_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "type": "string"
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  EXPECT_THROW(frame.analyse(document, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_frame, mode_references) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "$anchor": "helper",
      "$ref": "#/$defs/helper"
    },
    "$defs": {
      "helper": true
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.mode(), sourcemeta::core::SchemaFrame::Mode::References);

  EXPECT_EQ(frame.locations().size(), 9);
  EXPECT_FRAME_STATIC_RESOURCE(frame, "https://www.example.com",
                               "https://www.example.com", "",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://www.example.com", "", {}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/$id",
                              "https://www.example.com", "/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/$id", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/$schema",
                              "https://www.example.com", "/$schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/$schema", {}, "");

  EXPECT_FRAME_STATIC_SUBSCHEMA(frame, "https://www.example.com#/items",
                                "https://www.example.com", "/items",
                                "https://json-schema.org/draft/2020-12/schema",
                                "https://json-schema.org/draft/2020-12/schema",
                                "https://www.example.com", "/items", {}, "");
  EXPECT_FRAME_STATIC_ANCHOR(frame, "https://www.example.com#helper",
                             "https://www.example.com", "/items",
                             "https://json-schema.org/draft/2020-12/schema",
                             "https://json-schema.org/draft/2020-12/schema",
                             "https://www.example.com", "/items", {}, "");

  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/items/$anchor",
                              "https://www.example.com", "/items/$anchor",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/items/$anchor", {},
                              "/items");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.example.com#/items/$ref", "https://www.example.com",
      "/items/$ref", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.com",
      "/items/$ref", {}, "/items");

  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/$defs",
                              "https://www.example.com", "/$defs",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/$defs", {}, "");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://www.example.com#/$defs/helper", "https://www.example.com",
      "/$defs/helper", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.com",
      "/$defs/helper", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 2);

  EXPECT_STATIC_REFERENCE(
      frame, "/$schema", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      "https://json-schema.org/draft/2020-12/schema");
  EXPECT_STATIC_REFERENCE(
      frame, "/items/$ref", "https://www.example.com#/$defs/helper",
      "https://www.example.com", "/$defs/helper", "#/$defs/helper");
}

TEST(JSONSchema_frame, mode_locations) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "$anchor": "helper",
      "$ref": "#/$defs/helper"
    },
    "$defs": {
      "helper": true
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  EXPECT_EQ(frame.mode(), sourcemeta::core::SchemaFrame::Mode::Locations);
  EXPECT_EQ(frame.locations().size(), 9);
  EXPECT_FRAME_STATIC_RESOURCE(frame, "https://www.example.com",
                               "https://www.example.com", "",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://json-schema.org/draft/2020-12/schema",
                               "https://www.example.com", "", {}, std::nullopt);

  // JSON Pointers

  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/$id",
                              "https://www.example.com", "/$id",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/$id", {}, "");
  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/$schema",
                              "https://www.example.com", "/$schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/$schema", {}, "");

  EXPECT_FRAME_STATIC_SUBSCHEMA(frame, "https://www.example.com#/items",
                                "https://www.example.com", "/items",
                                "https://json-schema.org/draft/2020-12/schema",
                                "https://json-schema.org/draft/2020-12/schema",
                                "https://www.example.com", "/items", {}, "");
  EXPECT_FRAME_STATIC_ANCHOR(frame, "https://www.example.com#helper",
                             "https://www.example.com", "/items",
                             "https://json-schema.org/draft/2020-12/schema",
                             "https://json-schema.org/draft/2020-12/schema",
                             "https://www.example.com", "/items", {}, "");

  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/items/$anchor",
                              "https://www.example.com", "/items/$anchor",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/items/$anchor", {},
                              "/items");
  EXPECT_FRAME_STATIC_POINTER(
      frame, "https://www.example.com#/items/$ref", "https://www.example.com",
      "/items/$ref", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.com",
      "/items/$ref", {}, "/items");

  EXPECT_FRAME_STATIC_POINTER(frame, "https://www.example.com#/$defs",
                              "https://www.example.com", "/$defs",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://json-schema.org/draft/2020-12/schema",
                              "https://www.example.com", "/$defs", {}, "");
  EXPECT_FRAME_STATIC_SUBSCHEMA(
      frame, "https://www.example.com#/$defs/helper", "https://www.example.com",
      "/$defs/helper", "https://json-schema.org/draft/2020-12/schema",
      "https://json-schema.org/draft/2020-12/schema", "https://www.example.com",
      "/$defs/helper", {}, "");

  // References

  EXPECT_EQ(frame.references().size(), 0);
}

TEST(JSONSchema_frame, references_to_1) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$id": "foo",
        "$dynamicAnchor": "test"
      },
      "bar": {
        "$id": "bar",
        "$dynamicAnchor": "test"
      },
      "baz": {
        "$id": "baz",
        "$anchor": "test"
      }
    },
    "$defs": {
      "bookending": {
        "$dynamicAnchor": "test"
      },
      "static": {
        "$ref": "#test"
      },
      "dynamic": {
        "$dynamicRef": "#test"
      },
      "dynamic-non-anchor": {
        "$dynamicRef": "baz"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  const auto foo{frame.references_to({"properties", "foo"})};
  EXPECT_EQ(foo.size(), 1);
  EXPECT_REFERENCE_TO(foo, 0, Dynamic, "/$defs/dynamic/$dynamicRef");

  const auto bar{frame.references_to({"properties", "bar"})};
  EXPECT_EQ(bar.size(), 1);
  EXPECT_REFERENCE_TO(bar, 0, Dynamic, "/$defs/dynamic/$dynamicRef");

  const auto baz{frame.references_to({"properties", "baz"})};
  EXPECT_EQ(baz.size(), 1);
  EXPECT_REFERENCE_TO(baz, 0, Static, "/$defs/dynamic-non-anchor/$dynamicRef");

  const auto bookending{frame.references_to({"$defs", "bookending"})};
  EXPECT_EQ(bookending.size(), 2);
  EXPECT_REFERENCE_TO(bookending, 0, Static, "/$defs/static/$ref");
  EXPECT_REFERENCE_TO(bookending, 1, Dynamic, "/$defs/dynamic/$dynamicRef");
}

TEST(JSONSchema_frame, references_to_2) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$id": "foo",
        "$recursiveAnchor": true
      },
      "bar": {
        "$id": "bar",
        "$recursiveAnchor": true
      },
      "baz": {
        "$id": "baz",
        "$anchor": "test"
      },
      "qux": {
        "$id": "qux",
        "$recursiveAnchor": false
      }
    },
    "$defs": {
      "bookending": {
        "$recursiveAnchor": true
      },
      "static": {
        "$ref": "#test"
      },
      "dynamic": {
        "$recursiveRef": "#"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  const auto foo{frame.references_to({"properties", "foo"})};
  EXPECT_EQ(foo.size(), 1);
  EXPECT_REFERENCE_TO(foo, 0, Dynamic, "/$defs/dynamic/$recursiveRef");

  const auto bar{frame.references_to({"properties", "bar"})};
  EXPECT_EQ(bar.size(), 1);
  EXPECT_REFERENCE_TO(bar, 0, Dynamic, "/$defs/dynamic/$recursiveRef");

  const auto baz{frame.references_to({"properties", "baz"})};
  EXPECT_EQ(baz.size(), 0);

  const auto qux{frame.references_to({"properties", "qux"})};
  EXPECT_EQ(qux.size(), 0);

  const auto bookending{frame.references_to({"$defs", "bookending"})};
  EXPECT_EQ(bookending.size(), 1);
  EXPECT_REFERENCE_TO(bookending, 0, Dynamic, "/$defs/dynamic/$recursiveRef");
}

TEST(JSONSchema_frame, to_json_empty) {
  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  const auto result{frame.to_json()};
  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "locations": {
      "static": {},
      "dynamic": {}
    },
    "references": [],
    "instances": {}
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_frame, to_json_mode_instances) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "bar"
      },
      "bar": {
        "id": "bar",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Instances};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  const auto result{frame.to_json()};

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "locations": {
      "static": {
        "https://www.sourcemeta.com/bar": {
          "parent": "",
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": null,
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": null,
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": null,
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test": {
          "parent": null,
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "",
          "position": null,
          "relativePointer": "",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$id": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$id",
          "position": null,
          "relativePointer": "/$id",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$schema": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties",
          "position": null,
          "relativePointer": "/properties",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/bar": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": null,
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": null,
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": null,
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/foo": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo",
          "position": null,
          "relativePointer": "/properties/foo",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/foo/$ref": {
          "parent": "/properties/foo",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo/$ref",
          "position": null,
          "relativePointer": "/properties/foo/$ref",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        }
      },
      "dynamic": {}
    },
    "references": [
      {
        "type": "static",
        "origin": "/$schema",
        "position": null,
        "destination": "https://json-schema.org/draft/2020-12/schema",
        "base": "https://json-schema.org/draft/2020-12/schema",
        "fragment": null
      },
      {
        "type": "static",
        "origin": "/properties/bar/$schema",
        "position": null,
        "destination": "http://json-schema.org/draft-04/schema",
        "base": "http://json-schema.org/draft-04/schema",
        "fragment": null
      },
      {
        "type": "static",
        "origin": "/properties/foo/$ref",
        "position": null,
        "destination": "https://www.sourcemeta.com/bar",
        "base": "https://www.sourcemeta.com/bar",
        "fragment": null
      }
    ],
    "instances": {
      "": [
        ""
      ],
      "/properties/bar": [
        "/bar",
        "/foo"
      ],
      "/properties/foo": [
        "/foo"
      ]
    }
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_frame, to_json_mode_references) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "bar"
      },
      "bar": {
        "id": "bar",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  const auto result{frame.to_json()};

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "locations": {
      "static": {
        "https://www.sourcemeta.com/bar": {
          "parent": "",
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": null,
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": null,
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": null,
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test": {
          "parent": null,
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "",
          "position": null,
          "relativePointer": "",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$id": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$id",
          "position": null,
          "relativePointer": "/$id",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$schema": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties",
          "position": null,
          "relativePointer": "/properties",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/bar": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": null,
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": null,
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": null,
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/foo": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo",
          "position": null,
          "relativePointer": "/properties/foo",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/foo/$ref": {
          "parent": "/properties/foo",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo/$ref",
          "position": null,
          "relativePointer": "/properties/foo/$ref",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        }
      },
      "dynamic": {}
    },
    "references": [
      {
        "type": "static",
        "origin": "/$schema",
        "position": null,
        "destination": "https://json-schema.org/draft/2020-12/schema",
        "base": "https://json-schema.org/draft/2020-12/schema",
        "fragment": null
      },
      {
        "type": "static",
        "origin": "/properties/bar/$schema",
        "position": null,
        "destination": "http://json-schema.org/draft-04/schema",
        "base": "http://json-schema.org/draft-04/schema",
        "fragment": null
      },
      {
        "type": "static",
        "origin": "/properties/foo/$ref",
        "position": null,
        "destination": "https://www.sourcemeta.com/bar",
        "base": "https://www.sourcemeta.com/bar",
        "fragment": null
      }
    ],
    "instances": {}
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_frame, to_json_mode_locations) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "bar"
      },
      "bar": {
        "id": "bar",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  const auto result{frame.to_json()};

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "locations": {
      "static": {
        "https://www.sourcemeta.com/bar": {
          "parent": "",
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": null,
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": null,
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": null,
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test": {
          "parent": null,
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "",
          "position": null,
          "relativePointer": "",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$id": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$id",
          "position": null,
          "relativePointer": "/$id",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$schema": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties",
          "position": null,
          "relativePointer": "/properties",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/bar": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": null,
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": null,
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": null,
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/foo": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo",
          "position": null,
          "relativePointer": "/properties/foo",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/foo/$ref": {
          "parent": "/properties/foo",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo/$ref",
          "position": null,
          "relativePointer": "/properties/foo/$ref",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        }
      },
      "dynamic": {}
    },
    "references": [],
    "instances": {}
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_frame, to_json_mode_references_with_tracking) {
  sourcemeta::core::PointerPositionTracker tracker;
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "bar"
      },
      "bar": {
        "id": "bar",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "type": "string"
      }
    }
  })JSON",
                                   std::ref(tracker));

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  const auto result{frame.to_json(tracker)};

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "locations": {
      "static": {
        "https://www.sourcemeta.com/bar": {
          "parent": "",
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": [ 8, 7, 12, 7 ],
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": [ 10, 9, 10, 60 ],
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": [ 9, 9, 9, 19 ],
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": [ 11, 9, 11, 24 ],
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test": {
          "parent": null,
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "",
          "position": [ 1, 1, 14, 3 ],
          "relativePointer": "",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$id": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$id",
          "position": [ 2, 5, 2, 44 ],
          "relativePointer": "/$id",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$schema": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$schema",
          "position": [ 3, 5, 3, 61 ],
          "relativePointer": "/$schema",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties",
          "position": [ 4, 5, 13, 5 ],
          "relativePointer": "/properties",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/bar": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": [ 8, 7, 12, 7 ],
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": [ 10, 9, 10, 60 ],
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": [ 9, 9, 9, 19 ],
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": [ 11, 9, 11, 24 ],
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/foo": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo",
          "position": [ 5, 7, 7, 7 ],
          "relativePointer": "/properties/foo",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/foo/$ref": {
          "parent": "/properties/foo",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo/$ref",
          "position": [ 6, 9, 6, 21 ],
          "relativePointer": "/properties/foo/$ref",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        }
      },
      "dynamic": {}
    },
    "references": [
      {
        "type": "static",
        "origin": "/$schema",
        "position": [ 3, 5, 3, 61 ],
        "destination": "https://json-schema.org/draft/2020-12/schema",
        "base": "https://json-schema.org/draft/2020-12/schema",
        "fragment": null
      },
      {
        "type": "static",
        "origin": "/properties/bar/$schema",
        "position": [ 10, 9, 10, 60 ],
        "destination": "http://json-schema.org/draft-04/schema",
        "base": "http://json-schema.org/draft-04/schema",
        "fragment": null
      },
      {
        "type": "static",
        "origin": "/properties/foo/$ref",
        "position": [ 6, 9, 6, 21 ],
        "destination": "https://www.sourcemeta.com/bar",
        "base": "https://www.sourcemeta.com/bar",
        "fragment": null
      }
    ],
    "instances": {}
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_frame, to_json_mode_references_with_tracking_empty) {
  sourcemeta::core::PointerPositionTracker tracker;

  // Note we purposely don't pass the tracker to the document
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "bar"
      },
      "bar": {
        "id": "bar",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  const auto result{frame.to_json(tracker)};

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "locations": {
      "static": {
        "https://www.sourcemeta.com/bar": {
          "parent": "",
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": null,
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": null,
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/bar#/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": null,
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test": {
          "parent": null,
          "type": "resource",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "",
          "position": null,
          "relativePointer": "",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$id": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$id",
          "position": null,
          "relativePointer": "/$id",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/$schema": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties": {
          "parent": "",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties",
          "position": null,
          "relativePointer": "/properties",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/bar": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar",
          "position": null,
          "relativePointer": "",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/$schema": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/$schema",
          "position": null,
          "relativePointer": "/$schema",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/id": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/id",
          "position": null,
          "relativePointer": "/id",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/bar/type": {
          "parent": "/properties/bar",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/bar",
          "pointer": "/properties/bar/type",
          "position": null,
          "relativePointer": "/type",
          "dialect": "http://json-schema.org/draft-04/schema#",
          "baseDialect": "http://json-schema.org/draft-04/schema#"
        },
        "https://www.sourcemeta.com/test#/properties/foo": {
          "parent": "",
          "type": "subschema",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo",
          "position": null,
          "relativePointer": "/properties/foo",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        },
        "https://www.sourcemeta.com/test#/properties/foo/$ref": {
          "parent": "/properties/foo",
          "type": "pointer",
          "root": "https://www.sourcemeta.com/test",
          "base": "https://www.sourcemeta.com/test",
          "pointer": "/properties/foo/$ref",
          "position": null,
          "relativePointer": "/properties/foo/$ref",
          "dialect": "https://json-schema.org/draft/2020-12/schema",
          "baseDialect": "https://json-schema.org/draft/2020-12/schema"
        }
      },
      "dynamic": {}
    },
    "references": [
      {
        "type": "static",
        "origin": "/$schema",
        "position": null,
        "destination": "https://json-schema.org/draft/2020-12/schema",
        "base": "https://json-schema.org/draft/2020-12/schema",
        "fragment": null
      },
      {
        "type": "static",
        "origin": "/properties/bar/$schema",
        "position": null,
        "destination": "http://json-schema.org/draft-04/schema",
        "base": "http://json-schema.org/draft-04/schema",
        "fragment": null
      },
      {
        "type": "static",
        "origin": "/properties/foo/$ref",
        "position": null,
        "destination": "https://www.sourcemeta.com/bar",
        "base": "https://www.sourcemeta.com/bar",
        "fragment": null
      }
    ],
    "instances": {}
  })JSON");

  EXPECT_EQ(result, expected);
}
