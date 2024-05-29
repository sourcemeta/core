#include <gtest/gtest.h>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

#include "jsonschema_test_utils.h"

TEST(JSONSchema_compile_draft4, metaschema) {
  const auto metaschema{sourcemeta::jsontoolkit::official_resolver(
                            "http://json-schema.org/draft-04/schema#")
                            .get()};
  EXPECT_TRUE(metaschema.has_value());

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      metaschema.value(), sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{}")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);
}

TEST(JSONSchema_compile_draft4, type_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "string"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo bar"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
}

TEST(JSONSchema_compile_draft4, type_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "string"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionType, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
}

TEST(JSONSchema_compile_draft4, type_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionTypeAny, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of one of the given types");
}

TEST(JSONSchema_compile_draft4, type_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{3.14};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionTypeAny, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of one of the given types");
}

TEST(JSONSchema_compile_draft4, type_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"3.14"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionTypeAny, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of one of the given types");
}

TEST(JSONSchema_compile_draft4, type_6) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": [ "string" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
}

TEST(JSONSchema_compile_draft4, type_7) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": [ "string" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionType, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
}

TEST(JSONSchema_compile_draft4, type_8) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": [ "string", "number", "object" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{3};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionTypeAny, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of one of the given types");
}

TEST(JSONSchema_compile_draft4, type_9) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": [ "string", "number", "object" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{true};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionTypeAny, "/type", "#/type", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of one of the given types");
}

TEST(JSONSchema_compile_draft4, required_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo bar"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, required_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionDefines, "/required", "#/required", "");
  EVALUATE_TRACE_DESCRIBE(
      0, "The target object is expected to define the given property");
}

TEST(JSONSchema_compile_draft4, required_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo", "baz" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);
  EVALUATE_TRACE_SUCCESS(0, AssertionDefines, "/required", "#/required", "");
  EVALUATE_TRACE_FAILURE(1, AssertionDefines, "/required", "#/required", "");
  EVALUATE_TRACE_FAILURE(2, LogicalAnd, "/required", "#/required", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target object is expected to define the given property");

  // TODO: Remove this step
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, required_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "required": [ "foo", "bar" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/type", "#/type", "");
  EVALUATE_TRACE_SUCCESS(1, AssertionDefines, "/required", "#/required", "");
  EVALUATE_TRACE_SUCCESS(2, AssertionDefines, "/required", "#/required", "");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/required", "#/required", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target object is expected to define the given property");

  // TODO: Remove this step
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, allOf_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
      { "type": "object" },
      { "required": [ "foo", "bar" ] }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/allOf/0/type", "#/allOf/0/type",
                         "");
  EVALUATE_TRACE_SUCCESS(1, AssertionDefines, "/allOf/1/required",
                         "#/allOf/1/required", "");
  EVALUATE_TRACE_SUCCESS(2, AssertionDefines, "/allOf/1/required",
                         "#/allOf/1/required", "");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/allOf/1/required",
                         "#/allOf/1/required", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target object is expected to define the given property");

  // TODO: Remove this step
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, allOf_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
      { "type": "object" },
      { "required": [ "foo", "bar" ] }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"baz\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 4);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/allOf/0/type", "#/allOf/0/type",
                         "");
  EVALUATE_TRACE_SUCCESS(1, AssertionDefines, "/allOf/1/required",
                         "#/allOf/1/required", "");
  EVALUATE_TRACE_FAILURE(2, AssertionDefines, "/allOf/1/required",
                         "#/allOf/1/required", "");
  EVALUATE_TRACE_FAILURE(3, LogicalAnd, "/allOf/1/required",
                         "#/allOf/1/required", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target object is expected to define the given property");

  // TODO: Remove this step
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, ref_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [ { "$ref": "#/definitions/string" } ],
    "definitions": {
      "string": { "type": "string" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionType, "/allOf/0/$ref/type",
                         "#/definitions/string/type", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
}

TEST(JSONSchema_compile_draft4, ref_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [ { "$ref": "#/definitions/string" } ],
    "definitions": {
      "string": { "type": "string" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/allOf/0/$ref/type",
                         "#/definitions/string/type", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
}

TEST(JSONSchema_compile_draft4, ref_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
      "foo": {
        "$ref": "https://example.com"
      }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": {} }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/type", "https://example.com#/type",
                         "");
  EVALUATE_TRACE_SUCCESS(1, AssertionType, "/properties/foo/$ref/type",
                         "https://example.com#/type", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(
      2, "/properties", "https://example.com#/properties", "", "foo");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(2, "Emit an internal annotation");
}

TEST(JSONSchema_compile_draft4, ref_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
      "foo": {
        "$ref": "https://example.com"
      }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": { \"foo\": {} } }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);

  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/type", "https://example.com#/type",
                         "");
  EVALUATE_TRACE_SUCCESS(1, AssertionType, "/properties/foo/$ref/type",
                         "https://example.com#/type", "/foo");
  EVALUATE_TRACE_SUCCESS(2, AssertionType,
                         "/properties/foo/$ref/properties/foo/$ref/type",
                         "https://example.com#/type", "/foo/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(3, "/properties/foo/$ref/properties",
                                    "https://example.com#/properties", "/foo",
                                    "foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(
      4, "/properties", "https://example.com#/properties", "", "foo");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(3, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(4, "Emit an internal annotation");
}

TEST(JSONSchema_compile_draft4, ref_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
      "foo": {
        "$ref": "https://example.com"
      }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": { \"foo\": 1 } }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);

  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/type", "https://example.com#/type",
                         "");
  EVALUATE_TRACE_SUCCESS(1, AssertionType, "/properties/foo/$ref/type",
                         "https://example.com#/type", "/foo");
  EVALUATE_TRACE_FAILURE(2, AssertionType,
                         "/properties/foo/$ref/properties/foo/$ref/type",
                         "https://example.com#/type", "/foo/foo");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
}

TEST(JSONSchema_compile_draft4, ref_6) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
      "foo": {
        "$ref": "#"
      }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": {} }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/type", "#/type", "");
  EVALUATE_TRACE_SUCCESS(1, AssertionType, "/properties/foo/$ref/type",
                         "#/type", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(2, "/properties", "#/properties", "",
                                    "foo");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(2, "Emit an internal annotation");
}

TEST(JSONSchema_compile_draft4, ref_7) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "a": { "$ref": "#" },
      "b": { "$ref": "#" },
      "c": { "$ref": "#" },
      "d": { "$ref": "#" },
      "e": { "$ref": "#" },
      "f": { "$ref": "#" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, properties_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "type": "string" },
      "bar": { "type": "integer" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/properties/bar/type",
                         "#/properties/bar/type", "/bar");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(1, "/properties", "#/properties", "",
                                    "bar");
  EVALUATE_TRACE_FAILURE(2, AssertionType, "/properties/foo/type",
                         "#/properties/foo/type", "/foo");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
}

TEST(JSONSchema_compile_draft4, properties_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "type": "string" },
      "bar": { "type": "integer" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": \"xxx\", \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/properties/bar/type",
                         "#/properties/bar/type", "/bar");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(1, "/properties", "#/properties", "",
                                    "bar");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/properties/foo/type",
                         "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(3, "/properties", "#/properties", "",
                                    "foo");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(3, "Emit an internal annotation");
}

TEST(JSONSchema_compile_draft4, properties_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "type": "string" },
      "bar": { "type": "integer" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"baz\": [] }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, properties_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "properties": {
          "bar": {
            "type": "string"
          }
        }
      }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": { \"bar\": \"baz\" } }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);
  EVALUATE_TRACE_SUCCESS(0, AssertionType,
                         "/properties/foo/properties/bar/type",
                         "#/properties/foo/properties/bar/type", "/foo/bar");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(1, "/properties/foo/properties",
                                    "#/properties/foo/properties", "/foo",
                                    "bar");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(2, "/properties", "#/properties", "",
                                    "foo");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(2, "Emit an internal annotation");
}

TEST(JSONSchema_compile_draft4, pattern_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "pattern": "^x"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"xxx"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionRegex, "/pattern", "#/pattern", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target string is expected to match the given regular expression");
}

TEST(JSONSchema_compile_draft4, pattern_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "pattern": "^x"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"aaa"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionRegex, "/pattern", "#/pattern", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target string is expected to match the given regular expression");
}

TEST(JSONSchema_compile_draft4, pattern_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "pattern": "^x"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, patternProperties_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "patternProperties": {}
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, patternProperties_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "patternProperties": {
      "^v": true
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);
  EVALUATE_TRACE_SUCCESS(0, LoopProperties, "/patternProperties",
                         "#/patternProperties", "");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "");

  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(0, "Loop over the properties of the target object");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, patternProperties_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "patternProperties": {
      "^f": true
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);
  EVALUATE_TRACE_ANNOTATION_PRIVATE(0, "/patternProperties",
                                    "#/patternProperties", "", "foo");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "/foo");
  EVALUATE_TRACE_SUCCESS(2, LoopProperties, "/patternProperties",
                         "#/patternProperties", "");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "");

  EVALUATE_TRACE_DESCRIBE(0, "Emit an internal annotation");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(2, "Loop over the properties of the target object");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, patternProperties_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "patternProperties": {
      "^f": { "type": "integer" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/patternProperties/^f/type",
                         // Note that the caret needs to be URI escaped
                         "#/patternProperties/%5Ef/type", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(1, "/patternProperties",
                                    "#/patternProperties", "", "foo");
  EVALUATE_TRACE_SUCCESS(2, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "/foo");
  EVALUATE_TRACE_SUCCESS(3, LoopProperties, "/patternProperties",
                         "#/patternProperties", "");
  EVALUATE_TRACE_SUCCESS(4, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(3, "Loop over the properties of the target object");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      4, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, patternProperties_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "patternProperties": {
      "^f": { "type": "string" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 4);
  EVALUATE_TRACE_FAILURE(0, AssertionType, "/patternProperties/^f/type",
                         // Note that the caret needs to be URI escaped
                         "#/patternProperties/%5Ef/type", "/foo");
  EVALUATE_TRACE_FAILURE(1, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "/foo");
  EVALUATE_TRACE_FAILURE(2, LoopProperties, "/patternProperties",
                         "#/patternProperties", "");
  EVALUATE_TRACE_FAILURE(3, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(2, "Loop over the properties of the target object");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, patternProperties_6) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "patternProperties": {
      "^f": { "type": "integer" },
      "o$": { "type": "integer" }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 8);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/patternProperties/^f/type",
                         // Note that the caret needs to be URI escaped
                         "#/patternProperties/%5Ef/type", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(1, "/patternProperties",
                                    "#/patternProperties", "", "foo");
  EVALUATE_TRACE_SUCCESS(2, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "/foo");
  EVALUATE_TRACE_SUCCESS(3, LoopProperties, "/patternProperties",
                         "#/patternProperties", "");
  EVALUATE_TRACE_SUCCESS(4, AssertionType, "/patternProperties/o$/type",
                         "#/patternProperties/o$/type", "/foo");
  EVALUATE_TRACE_SUCCESS(5, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "/foo");
  EVALUATE_TRACE_SUCCESS(6, LoopProperties, "/patternProperties",
                         "#/patternProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(3, "Loop over the properties of the target object");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target document is expected to be of the given type");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(
      5, "The target is expected to match all of the given assertions");
  // TODO: Remove this
  EVALUATE_TRACE_DESCRIBE(6, "Loop over the properties of the target object");
}

TEST(JSONSchema_compile_draft4, patternProperties_7) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "patternProperties": {
      "^f": {
        "patternProperties": {
          "^b": { "type": "integer" }
        }
      }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": { \"bar\": 2 } }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 9);
  EVALUATE_TRACE_SUCCESS(
      0, AssertionType, "/patternProperties/^f/patternProperties/^b/type",
      "#/patternProperties/%5Ef/patternProperties/%5Eb/type", "/foo/bar");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(
      1, "/patternProperties/^f/patternProperties",
      "#/patternProperties/%5Ef/patternProperties", "/foo", "bar");
  EVALUATE_TRACE_SUCCESS(
      2, LogicalAnd, "/patternProperties/^f/patternProperties",
      "#/patternProperties/%5Ef/patternProperties", "/foo/bar");
  EVALUATE_TRACE_SUCCESS(3, LoopProperties,
                         "/patternProperties/^f/patternProperties",
                         "#/patternProperties/%5Ef/patternProperties", "/foo");
  EVALUATE_TRACE_SUCCESS(4, LogicalAnd,
                         "/patternProperties/^f/patternProperties",
                         "#/patternProperties/%5Ef/patternProperties", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(5, "/patternProperties",
                                    "#/patternProperties", "", "foo");
  EVALUATE_TRACE_SUCCESS(6, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "/foo");
  EVALUATE_TRACE_SUCCESS(7, LoopProperties, "/patternProperties",
                         "#/patternProperties", "");
  EVALUATE_TRACE_SUCCESS(8, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(3, "Loop over the properties of the target object");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(5, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(
      6, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(7, "Loop over the properties of the target object");
  EVALUATE_TRACE_DESCRIBE(
      8, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, additionalProperties_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalProperties": {
      "type": "integer"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/additionalProperties/type",
                         "#/additionalProperties/type", "/bar");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/additionalProperties",
                         "#/additionalProperties", "/bar");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/additionalProperties/type",
                         "#/additionalProperties/type", "/foo");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/additionalProperties",
                         "#/additionalProperties", "/foo");
  EVALUATE_TRACE_SUCCESS(4, LoopProperties, "/additionalProperties",
                         "#/additionalProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(4, "Loop over the properties of the target object");
}

TEST(JSONSchema_compile_draft4, additionalProperties_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "type": "boolean"
      }
    },
    "additionalProperties": {
      "type": "integer"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": true, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/properties/foo/type",
                         "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(1, "/properties", "#/properties", "",
                                    "foo");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/additionalProperties/type",
                         "#/additionalProperties/type", "/bar");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/additionalProperties",
                         "#/additionalProperties", "/bar");
  EVALUATE_TRACE_SUCCESS(4, LoopProperties, "/additionalProperties",
                         "#/additionalProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(4, "Loop over the properties of the target object");
}

TEST(JSONSchema_compile_draft4, additionalProperties_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "baz": {
        "type": "integer"
      }
    },
    "additionalProperties": {
      "type": "integer"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/additionalProperties/type",
                         "#/additionalProperties/type", "/bar");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/additionalProperties",
                         "#/additionalProperties", "/bar");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/additionalProperties/type",
                         "#/additionalProperties/type", "/foo");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/additionalProperties",
                         "#/additionalProperties", "/foo");
  EVALUATE_TRACE_SUCCESS(4, LoopProperties, "/additionalProperties",
                         "#/additionalProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(4, "Loop over the properties of the target object");
}

TEST(JSONSchema_compile_draft4, additionalProperties_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "type": "boolean" }
    },
    "patternProperties": {
      "^bar$": { "type": "integer" }
    },
    "additionalProperties": { "type": "string" }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{sourcemeta::jsontoolkit::parse(
      "{ \"foo\": true, \"bar\": 2, \"baz\": \"qux\" }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 10);

  // `patternProperties`
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/patternProperties/^bar$/type",
                         // Note that the caret needs to be URI escaped
                         "#/patternProperties/%5Ebar$/type", "/bar");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(1, "/patternProperties",
                                    "#/patternProperties", "", "bar");
  EVALUATE_TRACE_SUCCESS(2, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "/bar");
  EVALUATE_TRACE_SUCCESS(3, LoopProperties, "/patternProperties",
                         "#/patternProperties", "");
  EVALUATE_TRACE_SUCCESS(4, LogicalAnd, "/patternProperties",
                         "#/patternProperties", "");

  // `properties`
  EVALUATE_TRACE_SUCCESS(5, AssertionType, "/properties/foo/type",
                         "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(6, "/properties", "#/properties", "",
                                    "foo");

  // `additionalProperties`
  EVALUATE_TRACE_SUCCESS(7, AssertionType, "/additionalProperties/type",
                         "#/additionalProperties/type", "/baz");
  EVALUATE_TRACE_SUCCESS(8, LogicalAnd, "/additionalProperties",
                         "#/additionalProperties", "/baz");
  EVALUATE_TRACE_SUCCESS(9, LoopProperties, "/additionalProperties",
                         "#/additionalProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(3, "Loop over the properties of the target object");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      5, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(6, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(
      7, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      8, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(9, "Loop over the properties of the target object");
}

TEST(JSONSchema_compile_draft4, not_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "not": {
      "type": "string"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);
  EVALUATE_TRACE_FAILURE(0, AssertionType, "/not/type", "#/not/type", "");
  EVALUATE_TRACE_SUCCESS(1, LogicalNot, "/not", "#/not", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The given schema is expected to not validate successfully");
}

TEST(JSONSchema_compile_draft4, not_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "not": {
      "type": "string"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 2);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/not/type", "#/not/type", "");
  EVALUATE_TRACE_FAILURE(1, LogicalNot, "/not", "#/not", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The given schema is expected to not validate successfully");
}

TEST(JSONSchema_compile_draft4, not_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "not": {
      "properties": {
        "foo": {
          "type": "boolean"
        }
      },
      "additionalProperties": {
        "type": "integer"
      }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": true, \"bar\": false }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 6);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/not/properties/foo/type",
                         "#/not/properties/foo/type", "/foo");
  EVALUATE_TRACE_ANNOTATION_PRIVATE(1, "/not/properties", "#/not/properties",
                                    "", "foo");
  EVALUATE_TRACE_FAILURE(2, AssertionType, "/not/additionalProperties/type",
                         "#/not/additionalProperties/type", "/bar");
  EVALUATE_TRACE_FAILURE(3, LogicalAnd, "/not/additionalProperties",
                         "#/not/additionalProperties", "/bar");
  EVALUATE_TRACE_FAILURE(4, LoopProperties, "/not/additionalProperties",
                         "#/not/additionalProperties", "");
  EVALUATE_TRACE_SUCCESS(5, LogicalNot, "/not", "#/not", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(1, "Emit an internal annotation");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(4, "Loop over the properties of the target object");
  EVALUATE_TRACE_DESCRIBE(
      5, "The given schema is expected to not validate successfully");
}

TEST(JSONSchema_compile_draft4, items_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, items_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", \"bar\", \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/type", "#/items/type", "/0");
  EVALUATE_TRACE_SUCCESS(1, AssertionType, "/items/type", "#/items/type", "/1");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/items/type", "#/items/type", "/2");
  EVALUATE_TRACE_SUCCESS(3, LoopItems, "/items", "#/items", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(3, "Loop over the items of the target array");
}

TEST(JSONSchema_compile_draft4, items_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": {
      "type": "string"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", 5, \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/type", "#/items/type", "/0");
  EVALUATE_TRACE_FAILURE(1, AssertionType, "/items/type", "#/items/type", "/1");
  EVALUATE_TRACE_FAILURE(2, LoopItems, "/items", "#/items", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(2, "Loop over the items of the target array");
}

TEST(JSONSchema_compile_draft4, items_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": [ { "type": "string" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, items_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, items_6) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/0/type", "#/items/0/type",
                         "/0");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(2, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");

  // TODO: Remove these
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, items_7) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5, true, \"extra\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/0/type", "#/items/0/type",
                         "/0");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/items/1/type", "#/items/1/type",
                         "/1");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(4, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, items_8) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5, 1, \"extra\" ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 5);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/0/type", "#/items/0/type",
                         "/0");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_FAILURE(2, AssertionType, "/items/1/type", "#/items/1/type",
                         "/1");
  EVALUATE_TRACE_FAILURE(3, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_FAILURE(4, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, additionalItems_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalItems": {
      "type": "string"
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", \"bar\", \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, additionalItems_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalItems": { "type": "integer" },
    "items": { "type": "string" }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", \"bar\", \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/type", "#/items/type", "/0");
  EVALUATE_TRACE_SUCCESS(1, AssertionType, "/items/type", "#/items/type", "/1");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/items/type", "#/items/type", "/2");
  EVALUATE_TRACE_SUCCESS(3, LoopItems, "/items", "#/items", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(3, "Loop over the items of the target array");
}

TEST(JSONSchema_compile_draft4, additionalItems_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalItems": { "type": "string" },
    "items": [
      { "type": "boolean" },
      { "type": "integer" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ true, 5 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 6);

  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/0/type", "#/items/0/type",
                         "/0");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/items/1/type", "#/items/1/type",
                         "/1");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(4, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(5, LoopItems, "/additionalItems", "#/additionalItems",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(5, "Loop over the items of the target array");
}

TEST(JSONSchema_compile_draft4, additionalItems_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalItems": { "type": "string" },
    "items": [
      { "type": "boolean" },
      { "type": "integer" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ true, 5, \"foo\", \"bar\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 8);

  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/0/type", "#/items/0/type",
                         "/0");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/items/1/type", "#/items/1/type",
                         "/1");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(4, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(5, AssertionType, "/additionalItems/type",
                         "#/additionalItems/type", "/2");
  EVALUATE_TRACE_SUCCESS(6, AssertionType, "/additionalItems/type",
                         "#/additionalItems/type", "/3");
  EVALUATE_TRACE_SUCCESS(7, LoopItems, "/additionalItems", "#/additionalItems",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      5, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      6, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(7, "Loop over the items of the target array");
}

TEST(JSONSchema_compile_draft4, additionalItems_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalItems": { "type": "string" },
    "items": [
      { "type": "boolean" },
      { "type": "integer" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ true, 5, 6, \"bar\" ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 7);

  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/items/0/type", "#/items/0/type",
                         "/0");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/items/1/type", "#/items/1/type",
                         "/1");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_SUCCESS(4, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_FAILURE(5, AssertionType, "/additionalItems/type",
                         "#/additionalItems/type", "/2");
  EVALUATE_TRACE_FAILURE(6, LoopItems, "/additionalItems", "#/additionalItems",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      5, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(6, "Loop over the items of the target array");
}

TEST(JSONSchema_compile_draft4, anyOf_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "anyOf": [
      { "type": "string" },
      { "type": "integer" },
      { "type": "number" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{1};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);

  EVALUATE_TRACE_FAILURE(0, AssertionType, "/anyOf/0/type", "#/anyOf/0/type",
                         "");
  EVALUATE_TRACE_FAILURE(1, LogicalAnd, "/anyOf", "#/anyOf", "");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/anyOf/1/type", "#/anyOf/1/type",
                         "");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/anyOf", "#/anyOf", "");
  EVALUATE_TRACE_SUCCESS(4, LogicalOr, "/anyOf", "#/anyOf", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4,
      "The target is expected to match at least one of the given assertions");
}

TEST(JSONSchema_compile_draft4, anyOf_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "anyOf": [
      { "type": "string" },
      { "type": "integer" },
      { "type": "number" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{true};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 7);

  EVALUATE_TRACE_FAILURE(0, AssertionType, "/anyOf/0/type", "#/anyOf/0/type",
                         "");
  EVALUATE_TRACE_FAILURE(1, LogicalAnd, "/anyOf", "#/anyOf", "");
  EVALUATE_TRACE_FAILURE(2, AssertionType, "/anyOf/1/type", "#/anyOf/1/type",
                         "");
  EVALUATE_TRACE_FAILURE(3, LogicalAnd, "/anyOf", "#/anyOf", "");
  EVALUATE_TRACE_FAILURE(4, AssertionTypeAny, "/anyOf/2/type", "#/anyOf/2/type",
                         "");
  EVALUATE_TRACE_FAILURE(5, LogicalAnd, "/anyOf", "#/anyOf", "");
  EVALUATE_TRACE_FAILURE(6, LogicalOr, "/anyOf", "#/anyOf", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target document is expected to be of one of the given types");
  EVALUATE_TRACE_DESCRIBE(
      5, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      6,
      "The target is expected to match at least one of the given assertions");
}

TEST(JSONSchema_compile_draft4, oneOf_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "oneOf": [
      { "type": "string" },
      { "type": "integer" },
      { "type": "number" },
      { "type": "boolean" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);
  EVALUATE_TRACE_SUCCESS(0, AssertionType, "/oneOf/0/type", "#/oneOf/0/type",
                         "");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_SUCCESS(2, LogicalXor, "/oneOf", "#/oneOf", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(2, "The target is expected to match one and only one "
                             "of the given assertions");
}

TEST(JSONSchema_compile_draft4, oneOf_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "oneOf": [
      { "type": "string" },
      { "type": "integer" },
      { "type": "number" },
      { "type": "boolean" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{3.14};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 7);

  EVALUATE_TRACE_FAILURE(0, AssertionType, "/oneOf/0/type", "#/oneOf/0/type",
                         "");
  EVALUATE_TRACE_FAILURE(1, LogicalAnd, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_FAILURE(2, AssertionType, "/oneOf/1/type", "#/oneOf/1/type",
                         "");
  EVALUATE_TRACE_FAILURE(3, LogicalAnd, "/oneOf", "#/oneOf", "");

  EVALUATE_TRACE_SUCCESS(4, AssertionTypeAny, "/oneOf/2/type", "#/oneOf/2/type",
                         "");
  EVALUATE_TRACE_SUCCESS(5, LogicalAnd, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_SUCCESS(6, LogicalXor, "/oneOf", "#/oneOf", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target document is expected to be of one of the given types");
  EVALUATE_TRACE_DESCRIBE(
      5, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(6, "The target is expected to match one and only one "
                             "of the given assertions");
}

TEST(JSONSchema_compile_draft4, oneOf_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "oneOf": [
      { "type": "string" },
      { "type": "integer" },
      { "type": "number" },
      { "type": "boolean" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 9);

  EVALUATE_TRACE_FAILURE(0, AssertionType, "/oneOf/0/type", "#/oneOf/0/type",
                         "");
  EVALUATE_TRACE_FAILURE(1, LogicalAnd, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_SUCCESS(2, AssertionType, "/oneOf/1/type", "#/oneOf/1/type",
                         "");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_SUCCESS(4, AssertionTypeAny, "/oneOf/2/type", "#/oneOf/2/type",
                         "");
  EVALUATE_TRACE_SUCCESS(5, LogicalAnd, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_FAILURE(6, AssertionType, "/oneOf/3/type", "#/oneOf/3/type",
                         "");
  EVALUATE_TRACE_FAILURE(7, LogicalAnd, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_FAILURE(8, LogicalXor, "/oneOf", "#/oneOf", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      4, "The target document is expected to be of one of the given types");
  EVALUATE_TRACE_DESCRIBE(
      5, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      6, "The target document is expected to be of the given type");
  EVALUATE_TRACE_DESCRIBE(
      7, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(8, "The target is expected to match one and only one "
                             "of the given assertions");
}

TEST(JSONSchema_compile_draft4, dependencies_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "dependencies": {
      "foo": [ "bar", "baz" ],
      "qux": { "required": [ "extra" ] }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, dependencies_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "dependencies": {
      "foo": [ "bar", "baz" ],
      "qux": { "required": [ "extra" ] }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2, \"baz\": 3 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_SUCCESS(0, AssertionDefines, "/dependencies", "#/dependencies",
                         "");
  EVALUATE_TRACE_SUCCESS(1, AssertionDefines, "/dependencies", "#/dependencies",
                         "");
  EVALUATE_TRACE_SUCCESS(2, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_SUCCESS(3, LogicalAnd, "/dependencies", "#/dependencies", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      3, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, dependencies_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "dependencies": {
      "foo": [ "bar", "baz" ],
      "qux": { "required": [ "extra" ] }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"baz\": 3 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);

  EVALUATE_TRACE_FAILURE(0, AssertionDefines, "/dependencies", "#/dependencies",
                         "");
  EVALUATE_TRACE_FAILURE(1, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_FAILURE(2, LogicalAnd, "/dependencies", "#/dependencies", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, dependencies_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "dependencies": {
      "foo": [ "bar", "baz" ],
      "qux": { "required": [ "extra" ] }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"qux\": 1, \"extra\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_SUCCESS(0, AssertionDefines, "/dependencies/qux/required",
                         "#/dependencies/qux/required", "");
  EVALUATE_TRACE_SUCCESS(1, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_SUCCESS(2, LogicalAnd, "/dependencies", "#/dependencies", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, dependencies_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "dependencies": {
      "foo": [ "bar", "baz" ],
      "qux": { "required": [ "extra" ] }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"qux\": 1 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);

  EVALUATE_TRACE_FAILURE(0, AssertionDefines, "/dependencies/qux/required",
                         "#/dependencies/qux/required", "");
  EVALUATE_TRACE_FAILURE(1, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_FAILURE(2, LogicalAnd, "/dependencies", "#/dependencies", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target object is expected to define the given property");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_draft4, enum_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 1, {}, "foo" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{}")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);
  EVALUATE_TRACE_FAILURE(0, AssertionEqual, "/enum", "#/enum", "");
  EVALUATE_TRACE_SUCCESS(1, AssertionEqual, "/enum", "#/enum", "");
  EVALUATE_TRACE_SUCCESS(2, LogicalOr, "/enum", "#/enum", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be equal to the given number");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target size is expected to be equal to the given number");
  EVALUATE_TRACE_DESCRIBE(
      2,
      "The target is expected to match at least one of the given assertions");
}

TEST(JSONSchema_compile_draft4, enum_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 1, {}, "foo" ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 4);
  EVALUATE_TRACE_FAILURE(0, AssertionEqual, "/enum", "#/enum", "");
  EVALUATE_TRACE_FAILURE(1, AssertionEqual, "/enum", "#/enum", "");
  EVALUATE_TRACE_FAILURE(2, AssertionEqual, "/enum", "#/enum", "");
  EVALUATE_TRACE_FAILURE(3, LogicalOr, "/enum", "#/enum", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be equal to the given number");
  EVALUATE_TRACE_DESCRIBE(
      1, "The target size is expected to be equal to the given number");
  EVALUATE_TRACE_DESCRIBE(
      2, "The target size is expected to be equal to the given number");
  EVALUATE_TRACE_DESCRIBE(
      3,
      "The target is expected to match at least one of the given assertions");
}

TEST(JSONSchema_compile_draft4, enum_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 1 ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{1};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionEqual, "/enum", "#/enum", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be equal to the given number");
}

TEST(JSONSchema_compile_draft4, uniqueItems_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "uniqueItems": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionUnique, "/uniqueItems", "#/uniqueItems",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target array is expected to not contain duplicates");
}

TEST(JSONSchema_compile_draft4, uniqueItems_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "uniqueItems": false
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, uniqueItems_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "uniqueItems": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2, 3 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionUnique, "/uniqueItems", "#/uniqueItems",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target array is expected to not contain duplicates");
}

TEST(JSONSchema_compile_draft4, uniqueItems_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "uniqueItems": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 2, 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionUnique, "/uniqueItems", "#/uniqueItems",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target array is expected to not contain duplicates");
}

TEST(JSONSchema_compile_draft4, uniqueItems_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "uniqueItems": false
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 2, 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, minLength_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minLength": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, minLength_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minLength": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"xx"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionSizeGreater, "/minLength", "#/minLength",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be greater than the given number");
}

TEST(JSONSchema_compile_draft4, minLength_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minLength": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"x"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionSizeGreater, "/minLength", "#/minLength",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be greater than the given number");
}

TEST(JSONSchema_compile_draft4, maxLength_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxLength": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, maxLength_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxLength": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"xx"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionSizeLess, "/maxLength", "#/maxLength", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be less than the given number");
}

TEST(JSONSchema_compile_draft4, maxLength_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxLength": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"xxx"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionSizeLess, "/maxLength", "#/maxLength", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be less than the given number");
}

TEST(JSONSchema_compile_draft4, minItems_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minItems": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, minItems_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minItems": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionSizeGreater, "/minItems", "#/minItems",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be greater than the given number");
}

TEST(JSONSchema_compile_draft4, minItems_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minItems": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionSizeGreater, "/minItems", "#/minItems",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be greater than the given number");
}

TEST(JSONSchema_compile_draft4, maxItems_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxItems": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, maxItems_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxItems": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionSizeLess, "/maxItems", "#/maxItems", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be less than the given number");
}

TEST(JSONSchema_compile_draft4, maxItems_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxItems": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2, 3 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionSizeLess, "/maxItems", "#/maxItems", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be less than the given number");
}

TEST(JSONSchema_compile_draft4, minProperties_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minProperties": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, minProperties_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minProperties": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionSizeGreater, "/minProperties",
                         "#/minProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be greater than the given number");
}

TEST(JSONSchema_compile_draft4, minProperties_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minProperties": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionSizeGreater, "/minProperties",
                         "#/minProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be greater than the given number");
}

TEST(JSONSchema_compile_draft4, maxProperties_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxProperties": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, maxProperties_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxProperties": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionSizeLess, "/maxProperties",
                         "#/maxProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be less than the given number");
}

TEST(JSONSchema_compile_draft4, maxProperties_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maxProperties": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2, \"baz\": 3 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionSizeLess, "/maxProperties",
                         "#/maxProperties", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target size is expected to be less than the given number");
}

TEST(JSONSchema_compile_draft4, minimum_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minimum": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, minimum_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minimum": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2.1};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionGreaterEqual, "/minimum", "#/minimum", "");

  EVALUATE_TRACE_DESCRIBE(0, "The target number is expected to be greater than "
                             "or equal to the given number");
}

TEST(JSONSchema_compile_draft4, minimum_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minimum": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionGreaterEqual, "/minimum", "#/minimum", "");

  EVALUATE_TRACE_DESCRIBE(0, "The target number is expected to be greater than "
                             "or equal to the given number");
}

TEST(JSONSchema_compile_draft4, minimum_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minimum": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{1.8};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionGreaterEqual, "/minimum", "#/minimum", "");

  EVALUATE_TRACE_DESCRIBE(0, "The target number is expected to be greater than "
                             "or equal to the given number");
}

TEST(JSONSchema_compile_draft4, maximum_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maximum": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, maximum_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maximum": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{1.9};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionLessEqual, "/maximum", "#/maximum", "");

  EVALUATE_TRACE_DESCRIBE(0, "The target number is expected to be less than or "
                             "equal to the given number");
}

TEST(JSONSchema_compile_draft4, maximum_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maximum": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionLessEqual, "/maximum", "#/maximum", "");

  EVALUATE_TRACE_DESCRIBE(0, "The target number is expected to be less than or "
                             "equal to the given number");
}

TEST(JSONSchema_compile_draft4, maximum_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maximum": 2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2.1};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionLessEqual, "/maximum", "#/maximum", "");

  EVALUATE_TRACE_DESCRIBE(0, "The target number is expected to be less than or "
                             "equal to the given number");
}

TEST(JSONSchema_compile_draft4, exclusiveMinimum_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minimum": 2,
    "exclusiveMinimum": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2.1};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionGreater, "/minimum", "#/minimum", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target number is expected to be greater than the given number");
}

TEST(JSONSchema_compile_draft4, exclusiveMinimum_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minimum": 2,
    "exclusiveMinimum": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionGreater, "/minimum", "#/minimum", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target number is expected to be greater than the given number");
}

TEST(JSONSchema_compile_draft4, exclusiveMinimum_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minimum": 2,
    "exclusiveMinimum": false
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionGreaterEqual, "/minimum", "#/minimum", "");

  EVALUATE_TRACE_DESCRIBE(0, "The target number is expected to be greater than "
                             "or equal to the given number");
}

TEST(JSONSchema_compile_draft4, exclusiveMaximum_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maximum": 2,
    "exclusiveMaximum": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{1.9};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionLess, "/maximum", "#/maximum", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target number is expected to be less than the given number");
}

TEST(JSONSchema_compile_draft4, exclusiveMaximum_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maximum": 2,
    "exclusiveMaximum": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionLess, "/maximum", "#/maximum", "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target number is expected to be less than the given number");
}

TEST(JSONSchema_compile_draft4, exclusiveMaximum_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "maximum": 2,
    "exclusiveMaximum": false
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionLessEqual, "/maximum", "#/maximum", "");

  EVALUATE_TRACE_DESCRIBE(0, "The target number is expected to be less than or "
                             "equal to the given number");
}

TEST(JSONSchema_compile_draft4, multipleOf_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "multipleOf": 3
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, multipleOf_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "multipleOf": 3
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{6};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionDivisible, "/multipleOf", "#/multipleOf",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target number is expected to be divisible by the given number");
}

TEST(JSONSchema_compile_draft4, multipleOf_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "multipleOf": 3
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{7};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionDivisible, "/multipleOf", "#/multipleOf",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target number is expected to be divisible by the given number");
}

TEST(JSONSchema_compile_draft4, multipleOf_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "multipleOf": 3.2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{6.4};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_SUCCESS(0, AssertionDivisible, "/multipleOf", "#/multipleOf",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target number is expected to be divisible by the given number");
}

TEST(JSONSchema_compile_draft4, multipleOf_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "multipleOf": 3.2
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{6.0};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_FAILURE(0, AssertionDivisible, "/multipleOf", "#/multipleOf",
                         "");

  EVALUATE_TRACE_DESCRIBE(
      0, "The target number is expected to be divisible by the given number");
}
