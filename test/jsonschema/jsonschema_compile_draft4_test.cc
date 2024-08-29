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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{}")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);
}

TEST(JSONSchema_compile_draft4, unknown_keyword) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "foobar": "baz"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"foo bar"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"foo bar"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/type", "#/type", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/type", "#/type", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string "
                               "but it was of type integer");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type number");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{3.14};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type number");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"3.14"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type number but it was of type string");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type string but it was of type integer");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{3};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type number, "
                               "string, or object and it was of type number");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{true};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrictAny, "/type", "#/type", "");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type number, "
                               "string, or object but it was of type boolean");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionDefines, "/required", "#/required", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionDefines, "/required", "#/required",
                              "");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to define the property \"foo\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionDefinesAll, "/required", "#/required", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionDefinesAll, "/required", "#/required",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to define properties \"baz\", and \"foo\" "
      "but did not define the property \"baz\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);
  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_PRE(1, AssertionDefinesAll, "/required", "#/required", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionDefinesAll, "/required", "#/required",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The object value was expected to define "
                               "properties \"bar\", and \"foo\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/allOf", "#/allOf", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/allOf/0/type", "#/allOf/0/type",
                     "");
  EVALUATE_TRACE_PRE(2, AssertionDefinesAll, "/allOf/1/required",
                     "#/allOf/1/required", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/allOf/0/type",
                              "#/allOf/0/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionDefinesAll, "/allOf/1/required",
                              "#/allOf/1/required", "");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/allOf", "#/allOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The object value was expected to define "
                               "properties \"bar\", and \"foo\"");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the 2 given subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"baz\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/allOf", "#/allOf", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/allOf/0/type", "#/allOf/0/type",
                     "");
  EVALUATE_TRACE_PRE(2, AssertionDefinesAll, "/allOf/1/required",
                     "#/allOf/1/required", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/allOf/0/type",
                              "#/allOf/0/type", "");
  EVALUATE_TRACE_POST_FAILURE(1, AssertionDefinesAll, "/allOf/1/required",
                              "#/allOf/1/required", "");
  EVALUATE_TRACE_POST_FAILURE(2, LogicalAnd, "/allOf", "#/allOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The object value was expected to define properties \"bar\", and \"foo\" "
      "but did not define the property \"bar\"");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the 2 given subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/allOf", "#/allOf", "");
  EVALUATE_TRACE_PRE(1, ControlLabel, "/allOf/0/$ref", "#/allOf/0/$ref", "");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/allOf/0/$ref/type",
                     "#/definitions/string/type", "");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/allOf/0/$ref/type",
                              "#/definitions/string/type", "");
  EVALUATE_TRACE_POST_FAILURE(1, ControlLabel, "/allOf/0/$ref",
                              "#/allOf/0/$ref", "");
  EVALUATE_TRACE_POST_FAILURE(2, LogicalAnd, "/allOf", "#/allOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type string but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The integer value was expected to validate "
                               "against the statically referenced schema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The integer value was expected to validate "
                               "against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/allOf", "#/allOf", "");
  EVALUATE_TRACE_PRE(1, ControlLabel, "/allOf/0/$ref", "#/allOf/0/$ref", "");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/allOf/0/$ref/type",
                     "#/definitions/string/type", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/allOf/0/$ref/type",
                              "#/definitions/string/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, ControlLabel, "/allOf/0/$ref",
                              "#/allOf/0/$ref", "");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/allOf", "#/allOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The string value was expected to validate "
                               "against the statically referenced schema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The string value was expected to validate against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": {} }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 6);

  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type",
                     "https://example.com#/type", "");
  EVALUATE_TRACE_PRE(1, LogicalAnd, "/properties",
                     "https://example.com#/properties", "");
  EVALUATE_TRACE_PRE(2, ControlLabel, "/properties/foo/$ref",
                     "https://example.com#/properties/foo/$ref", "/foo");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/properties/foo/$ref/type",
                     "https://example.com#/type", "/foo");
  EVALUATE_TRACE_PRE(4, LogicalAnd, "/properties/foo/$ref/properties",
                     "https://example.com#/properties", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(5, "/properties",
                                "https://example.com#/properties", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/type",
                              "https://example.com#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict,
                              "/properties/foo/$ref/type",
                              "https://example.com#/type", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/properties/foo/$ref/properties",
                              "https://example.com#/properties", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(3, ControlLabel, "/properties/foo/$ref",
                              "https://example.com#/properties/foo/$ref",
                              "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(4, "/properties",
                                 "https://example.com#/properties", "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(5, LogicalAnd, "/properties",
                              "https://example.com#/properties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The object value was expected to validate "
                               "against the statically referenced schema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 5,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": { \"foo\": {} } }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 10);

  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type",
                     "https://example.com#/type", "");
  EVALUATE_TRACE_PRE(1, LogicalAnd, "/properties",
                     "https://example.com#/properties", "");
  EVALUATE_TRACE_PRE(2, ControlLabel, "/properties/foo/$ref",
                     "https://example.com#/properties/foo/$ref", "/foo");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/properties/foo/$ref/type",
                     "https://example.com#/type", "/foo");
  EVALUATE_TRACE_PRE(4, LogicalAnd, "/properties/foo/$ref/properties",
                     "https://example.com#/properties", "/foo");
  EVALUATE_TRACE_PRE(5, ControlJump, "/properties/foo/$ref/properties/foo/$ref",
                     "https://example.com#/properties/foo/$ref", "/foo/foo");
  EVALUATE_TRACE_PRE(6, AssertionTypeStrict,
                     "/properties/foo/$ref/properties/foo/$ref/type",
                     "https://example.com#/type", "/foo/foo");
  EVALUATE_TRACE_PRE(7, LogicalAnd,
                     "/properties/foo/$ref/properties/foo/$ref/properties",
                     "https://example.com#/properties", "/foo/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(8, "/properties/foo/$ref/properties",
                                "https://example.com#/properties", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(9, "/properties",
                                "https://example.com#/properties", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/type",
                              "https://example.com#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict,
                              "/properties/foo/$ref/type",
                              "https://example.com#/type", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict,
                              "/properties/foo/$ref/properties/foo/$ref/type",
                              "https://example.com#/type", "/foo/foo");
  EVALUATE_TRACE_POST_SUCCESS(
      3, LogicalAnd, "/properties/foo/$ref/properties/foo/$ref/properties",
      "https://example.com#/properties", "/foo/foo");
  EVALUATE_TRACE_POST_SUCCESS(
      4, ControlJump, "/properties/foo/$ref/properties/foo/$ref",
      "https://example.com#/properties/foo/$ref", "/foo/foo");
  EVALUATE_TRACE_POST_ANNOTATION(5, "/properties/foo/$ref/properties",
                                 "https://example.com#/properties", "/foo",
                                 "foo");
  EVALUATE_TRACE_POST_SUCCESS(6, LogicalAnd, "/properties/foo/$ref/properties",
                              "https://example.com#/properties", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(7, ControlLabel, "/properties/foo/$ref",
                              "https://example.com#/properties/foo/$ref",
                              "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(8, "/properties",
                                 "https://example.com#/properties", "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(9, LogicalAnd, "/properties",
                              "https://example.com#/properties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The object value was expected to validate "
                               "against the statically referenced schema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 5,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 6,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 7,
                               "The object value was expected to validate "
                               "against the statically referenced schema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 8,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 9,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": { \"foo\": 1 } }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 7);

  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type",
                     "https://example.com#/type", "");
  EVALUATE_TRACE_PRE(1, LogicalAnd, "/properties",
                     "https://example.com#/properties", "");
  EVALUATE_TRACE_PRE(2, ControlLabel, "/properties/foo/$ref",
                     "https://example.com#/properties/foo/$ref", "/foo");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/properties/foo/$ref/type",
                     "https://example.com#/type", "/foo");
  EVALUATE_TRACE_PRE(4, LogicalAnd, "/properties/foo/$ref/properties",
                     "https://example.com#/properties", "/foo");
  EVALUATE_TRACE_PRE(5, ControlJump, "/properties/foo/$ref/properties/foo/$ref",
                     "https://example.com#/properties/foo/$ref", "/foo/foo");
  EVALUATE_TRACE_PRE(6, AssertionTypeStrict,
                     "/properties/foo/$ref/properties/foo/$ref/type",
                     "https://example.com#/type", "/foo/foo");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/type",
                              "https://example.com#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict,
                              "/properties/foo/$ref/type",
                              "https://example.com#/type", "/foo");
  EVALUATE_TRACE_POST_FAILURE(2, AssertionTypeStrict,
                              "/properties/foo/$ref/properties/foo/$ref/type",
                              "https://example.com#/type", "/foo/foo");
  EVALUATE_TRACE_POST_FAILURE(
      3, ControlJump, "/properties/foo/$ref/properties/foo/$ref",
      "https://example.com#/properties/foo/$ref", "/foo/foo");
  EVALUATE_TRACE_POST_FAILURE(4, LogicalAnd, "/properties/foo/$ref/properties",
                              "https://example.com#/properties", "/foo");
  EVALUATE_TRACE_POST_FAILURE(5, ControlLabel, "/properties/foo/$ref",
                              "https://example.com#/properties/foo/$ref",
                              "/foo");
  EVALUATE_TRACE_POST_FAILURE(6, LogicalAnd, "/properties",
                              "https://example.com#/properties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The value was expected to be of type object but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The integer value was expected to validate "
                               "against the statically referenced schema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 5,
                               "The object value was expected to validate "
                               "against the statically referenced schema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 6,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": {} }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 6);

  EVALUATE_TRACE_PRE(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_PRE(1, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(2, ControlLabel, "/properties/foo/$ref",
                     "#/properties/foo/$ref", "/foo");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/properties/foo/$ref/type",
                     "#/type", "/foo");
  EVALUATE_TRACE_PRE(4, LogicalAnd, "/properties/foo/$ref/properties",
                     "#/properties", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(5, "/properties", "#/properties", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/type", "#/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict,
                              "/properties/foo/$ref/type", "#/type", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/properties/foo/$ref/properties",
                              "#/properties", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(3, ControlLabel, "/properties/foo/$ref",
                              "#/properties/foo/$ref", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(4, "/properties", "#/properties", "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(5, LogicalAnd, "/properties", "#/properties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type object");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The object value was expected to validate "
                               "against the statically referenced schema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 5,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The object value was expected to validate "
                               "against the 6 defined properties subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/properties/bar/type",
                     "#/properties/bar/type", "/bar");
  EVALUATE_TRACE_PRE_ANNOTATION(2, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/properties/foo/type",
                     "#/properties/foo/type", "/foo");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/properties/bar/type",
                              "#/properties/bar/type", "/bar");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/properties", "#/properties", "", "bar");
  EVALUATE_TRACE_POST_FAILURE(2, AssertionTypeStrict, "/properties/foo/type",
                              "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_POST_FAILURE(3, LogicalAnd, "/properties", "#/properties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The object property \"bar\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The value was expected to be of type string but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The object value was expected to validate "
                               "against the 2 defined properties subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": \"xxx\", \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/properties/bar/type",
                     "#/properties/bar/type", "/bar");
  EVALUATE_TRACE_PRE_ANNOTATION(2, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/properties/foo/type",
                     "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(4, "/properties", "#/properties", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/properties/bar/type",
                              "#/properties/bar/type", "/bar");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/properties", "#/properties", "", "bar");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict, "/properties/foo/type",
                              "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(3, "/properties", "#/properties", "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(4, LogicalAnd, "/properties", "#/properties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The object property \"bar\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The object value was expected to validate "
                               "against the 2 defined properties subschemas");
}

TEST(JSONSchema_compile_draft4, properties_2_optimized) {
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Optimized)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": \"xxx\", \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/properties/bar/type",
                     "#/properties/bar/type", "/bar");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/properties/foo/type",
                     "#/properties/foo/type", "/foo");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/properties/bar/type",
                              "#/properties/bar/type", "/bar");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/properties/foo/type",
                              "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/properties", "#/properties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the 2 defined properties subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"baz\": [] }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The object value was expected to validate "
                               "against the 2 defined properties subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": { \"bar\": \"baz\" } }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(1, LogicalAnd, "/properties/foo/properties",
                     "#/properties/foo/properties", "/foo");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict,
                     "/properties/foo/properties/bar/type",
                     "#/properties/foo/properties/bar/type", "/foo/bar");
  EVALUATE_TRACE_PRE_ANNOTATION(3, "/properties/foo/properties",
                                "#/properties/foo/properties", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(4, "/properties", "#/properties", "");

  EVALUATE_TRACE_POST_SUCCESS(
      0, AssertionTypeStrict, "/properties/foo/properties/bar/type",
      "#/properties/foo/properties/bar/type", "/foo/bar");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/properties/foo/properties",
                                 "#/properties/foo/properties", "/foo", "bar");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/properties/foo/properties",
                              "#/properties/foo/properties", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(3, "/properties", "#/properties", "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(4, LogicalAnd, "/properties", "#/properties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The object property \"bar\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"xxx"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionRegex, "/pattern", "#/pattern", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionRegex, "/pattern", "#/pattern", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The string value \"xxx\" was expected to match "
                               "the regular expression \"^x\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"aaa"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, AssertionRegex, "/pattern", "#/pattern", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionRegex, "/pattern", "#/pattern", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The string value \"aaa\" was expected to match "
                               "the regular expression \"^x\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, LogicalAnd, "/patternProperties", "#/patternProperties",
                     "");
  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/patternProperties",
                              "#/patternProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to validate against the single defined "
      "pattern property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/patternProperties", "#/patternProperties",
                     "");
  EVALUATE_TRACE_PRE_ANNOTATION(1, "/patternProperties", "#/patternProperties",
                                "");

  EVALUATE_TRACE_POST_ANNOTATION(0, "/patternProperties", "#/patternProperties",
                                 "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(1, LogicalAnd, "/patternProperties",
                              "#/patternProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object property \"foo\" successfully validated against its pattern "
      "property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The object value was expected to validate against the single defined "
      "pattern property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/patternProperties", "#/patternProperties",
                     "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/patternProperties/^f/type",
                     // Note that the caret needs to be URI escaped
                     "#/patternProperties/%5Ef/type", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(2, "/patternProperties", "#/patternProperties",
                                "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict,
                              "/patternProperties/^f/type",
                              // Note that the caret needs to be URI escaped
                              "#/patternProperties/%5Ef/type", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/patternProperties", "#/patternProperties",
                                 "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/patternProperties",
                              "#/patternProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The object property \"foo\" successfully validated against its pattern "
      "property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The object value was expected to validate against the single defined "
      "pattern property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/patternProperties", "#/patternProperties",
                     "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/patternProperties/^f/type",
                     // Note that the caret needs to be URI escaped
                     "#/patternProperties/%5Ef/type", "/foo");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict,
                              "/patternProperties/^f/type",
                              // Note that the caret needs to be URI escaped
                              "#/patternProperties/%5Ef/type", "/foo");
  EVALUATE_TRACE_POST_FAILURE(1, LogicalAnd, "/patternProperties",
                              "#/patternProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type string but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The object value was expected to validate against the single defined "
      "pattern property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/patternProperties", "#/patternProperties",
                     "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/patternProperties/^f/type",
                     // Note that the caret needs to be URI escaped
                     "#/patternProperties/%5Ef/type", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(2, "/patternProperties", "#/patternProperties",
                                "");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/patternProperties/o$/type",
                     "#/patternProperties/o$/type", "/foo");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict,
                              "/patternProperties/^f/type",
                              // Note that the caret needs to be URI escaped
                              "#/patternProperties/%5Ef/type", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/patternProperties", "#/patternProperties",
                                 "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict,
                              "/patternProperties/o$/type",
                              "#/patternProperties/o$/type", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(3, LogicalAnd, "/patternProperties",
                              "#/patternProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The object property \"foo\" successfully validated against its pattern "
      "property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The object value was expected to validate against the 2 defined pattern "
      "properties subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": { \"bar\": 2 } }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/patternProperties", "#/patternProperties",
                     "");
  EVALUATE_TRACE_PRE(1, LogicalAnd, "/patternProperties/^f/patternProperties",
                     "#/patternProperties/%5Ef/patternProperties", "/foo");
  EVALUATE_TRACE_PRE(
      2, AssertionTypeStrict, "/patternProperties/^f/patternProperties/^b/type",
      "#/patternProperties/%5Ef/patternProperties/%5Eb/type", "/foo/bar");
  EVALUATE_TRACE_PRE_ANNOTATION(3, "/patternProperties/^f/patternProperties",
                                "#/patternProperties/%5Ef/patternProperties",
                                "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(4, "/patternProperties", "#/patternProperties",
                                "");

  EVALUATE_TRACE_POST_SUCCESS(
      0, AssertionTypeStrict, "/patternProperties/^f/patternProperties/^b/type",
      "#/patternProperties/%5Ef/patternProperties/%5Eb/type", "/foo/bar");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/patternProperties/^f/patternProperties",
                                 "#/patternProperties/%5Ef/patternProperties",
                                 "/foo", "bar");
  EVALUATE_TRACE_POST_SUCCESS(
      2, LogicalAnd, "/patternProperties/^f/patternProperties",
      "#/patternProperties/%5Ef/patternProperties", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(3, "/patternProperties", "#/patternProperties",
                                 "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(4, LogicalAnd, "/patternProperties",
                              "#/patternProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The object property \"bar\" successfully validated against its pattern "
      "property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The object value was expected to validate against the single defined "
      "pattern property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The object property \"foo\" successfully validated against its pattern "
      "property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4,
      "The object value was expected to validate against the single defined "
      "pattern property subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LoopProperties, "/additionalProperties",
                     "#/additionalProperties", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/additionalProperties/type",
                     "#/additionalProperties/type", "/bar");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/additionalProperties/type",
                     "#/additionalProperties/type", "/foo");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict,
                              "/additionalProperties/type",
                              "#/additionalProperties/type", "/bar");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict,
                              "/additionalProperties/type",
                              "#/additionalProperties/type", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(2, LoopProperties, "/additionalProperties",
                              "#/additionalProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object properties not covered by other "
                               "adjacent object keywords were "
                               "expected to validate against this subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": true, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/properties/foo/type",
                     "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(2, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(3, LoopProperties, "/additionalProperties",
                     "#/additionalProperties", "");
  EVALUATE_TRACE_PRE(4, AssertionTypeStrict, "/additionalProperties/type",
                     "#/additionalProperties/type", "/bar");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/properties/foo/type",
                              "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/properties", "#/properties", "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_POST_SUCCESS(3, AssertionTypeStrict,
                              "/additionalProperties/type",
                              "#/additionalProperties/type", "/bar");
  EVALUATE_TRACE_POST_SUCCESS(4, LoopProperties, "/additionalProperties",
                              "#/additionalProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The object properties not covered by other "
                               "adjacent object keywords were "
                               "expected to validate against this subschema")
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(1, LoopProperties, "/additionalProperties",
                     "#/additionalProperties", "");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/additionalProperties/type",
                     "#/additionalProperties/type", "/bar");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/additionalProperties/type",
                     "#/additionalProperties/type", "/foo");

  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict,
                              "/additionalProperties/type",
                              "#/additionalProperties/type", "/bar");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict,
                              "/additionalProperties/type",
                              "#/additionalProperties/type", "/foo");
  EVALUATE_TRACE_POST_SUCCESS(3, LoopProperties, "/additionalProperties",
                              "#/additionalProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The object properties not covered by other "
                               "adjacent object keywords were "
                               "expected to validate against this subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{sourcemeta::jsontoolkit::parse(
      "{ \"foo\": true, \"bar\": 2, \"baz\": \"qux\" }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 8);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/patternProperties", "#/patternProperties",
                     "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/patternProperties/^bar$/type",
                     // Note that the caret needs to be URI escaped
                     "#/patternProperties/%5Ebar$/type", "/bar");
  EVALUATE_TRACE_PRE_ANNOTATION(2, "/patternProperties", "#/patternProperties",
                                "");

  EVALUATE_TRACE_PRE(3, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(4, AssertionTypeStrict, "/properties/foo/type",
                     "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(5, "/properties", "#/properties", "");

  EVALUATE_TRACE_PRE(6, LoopProperties, "/additionalProperties",
                     "#/additionalProperties", "");
  EVALUATE_TRACE_PRE(7, AssertionTypeStrict, "/additionalProperties/type",
                     "#/additionalProperties/type", "/baz");

  // `patternProperties`
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict,
                              "/patternProperties/^bar$/type",
                              // Note that the caret needs to be URI escaped
                              "#/patternProperties/%5Ebar$/type", "/bar");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/patternProperties", "#/patternProperties",
                                 "", "bar");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/patternProperties",
                              "#/patternProperties", "");

  // `properties`
  EVALUATE_TRACE_POST_SUCCESS(3, AssertionTypeStrict, "/properties/foo/type",
                              "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(4, "/properties", "#/properties", "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(5, LogicalAnd, "/properties", "#/properties", "");

  // `additionalProperties`
  EVALUATE_TRACE_POST_SUCCESS(6, AssertionTypeStrict,
                              "/additionalProperties/type",
                              "#/additionalProperties/type", "/baz");
  EVALUATE_TRACE_POST_SUCCESS(7, LoopProperties, "/additionalProperties",
                              "#/additionalProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The object property \"bar\" successfully validated against its pattern "
      "property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The object value was expected to validate against the single defined "
      "pattern property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The value was expected to be of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 5,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 6,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 7,
                               "The object properties not covered by other "
                               "adjacent object keywords were "
                               "expected to validate against this subschema");
}

TEST(JSONSchema_compile_draft4, additionalProperties_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "type": "boolean"
      }
    },
    "additionalProperties": false
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": true, \"bar\": 2 }")};

  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/properties/foo/type",
                     "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(2, "/properties", "#/properties", "");
  EVALUATE_TRACE_PRE(3, LoopProperties, "/additionalProperties",
                     "#/additionalProperties", "");
  EVALUATE_TRACE_PRE(4, AssertionFail, "/additionalProperties",
                     "#/additionalProperties", "/bar");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/properties/foo/type",
                              "#/properties/foo/type", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/properties", "#/properties", "", "foo");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/properties", "#/properties", "");
  EVALUATE_TRACE_POST_FAILURE(3, AssertionFail, "/additionalProperties",
                              "#/additionalProperties", "/bar");
  EVALUATE_TRACE_POST_FAILURE(4, LoopProperties, "/additionalProperties",
                              "#/additionalProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The object value was not expected to define the property \"bar\"");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4,
      "The object value was not expected to define additional properties")
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalNot, "/not", "#/not", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/not/type", "#/not/type", "");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/not/type", "#/not/type",
                              "");
  EVALUATE_TRACE_POST_SUCCESS(1, LogicalNot, "/not", "#/not", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type string but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The integer value was expected to not validate "
                               "against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalNot, "/not", "#/not", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/not/type", "#/not/type", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/not/type", "#/not/type",
                              "");
  EVALUATE_TRACE_POST_FAILURE(1, LogicalNot, "/not", "#/not", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The string value was expected to not validate "
                               "against the given subschema, but it did");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": true, \"bar\": false }")};

  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 6);

  EVALUATE_TRACE_PRE(0, LogicalNot, "/not", "#/not", "");
  EVALUATE_TRACE_PRE(1, LogicalAnd, "/not/properties", "#/not/properties", "");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/not/properties/foo/type",
                     "#/not/properties/foo/type", "/foo");
  EVALUATE_TRACE_PRE_ANNOTATION(3, "/not/properties", "#/not/properties", "");
  EVALUATE_TRACE_PRE(4, LoopProperties, "/not/additionalProperties",
                     "#/not/additionalProperties", "");
  EVALUATE_TRACE_PRE(5, AssertionTypeStrict, "/not/additionalProperties/type",
                     "#/not/additionalProperties/type", "/bar");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict,
                              "/not/properties/foo/type",
                              "#/not/properties/foo/type", "/foo");
  EVALUATE_TRACE_POST_ANNOTATION(1, "/not/properties", "#/not/properties", "",
                                 "foo");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/not/properties",
                              "#/not/properties", "");
  EVALUATE_TRACE_POST_FAILURE(3, AssertionTypeStrict,
                              "/not/additionalProperties/type",
                              "#/not/additionalProperties/type", "/bar");
  EVALUATE_TRACE_POST_FAILURE(4, LoopProperties, "/not/additionalProperties",
                              "#/not/additionalProperties", "");
  EVALUATE_TRACE_POST_SUCCESS(5, LogicalNot, "/not", "#/not", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The object property \"foo\" successfully "
                               "validated against its property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The object value was expected to validate "
                               "against the single defined property subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The value was expected to be of type integer "
                               "but it was of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4,
      "The object properties not covered by other adjacent object keywords "
      "were expected to validate against this subschema");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 5,
                               "The object value was expected to not validate "
                               "against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", \"bar\", \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LoopItems, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/1");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/2");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/1");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/2");
  EVALUATE_TRACE_POST_SUCCESS(3, LoopItems, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "Every item in the array value was expected to "
                               "validate against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", 5, \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LoopItems, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/1");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/0");
  EVALUATE_TRACE_POST_FAILURE(1, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/1");
  EVALUATE_TRACE_POST_FAILURE(2, LoopItems, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The value was expected to be of type string but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "Every item in the array value was expected to "
                               "validate against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The first 2 items of the array value were expected to validate against "
      "the corresponding subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/0/type", "#/items/0/type",
                     "/0");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/0/type",
                              "#/items/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The first 2 items of the array value were expected to validate against "
      "the corresponding subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5, true, \"extra\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/0/type", "#/items/0/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/1/type", "#/items/1/type",
                     "/1");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/0/type",
                              "#/items/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/items/1/type",
                              "#/items/1/type", "/1");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The first 2 items of the array value were expected to validate against "
      "the corresponding subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5, 1, \"extra\" ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/0/type", "#/items/0/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/1/type", "#/items/1/type",
                     "/1");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/0/type",
                              "#/items/0/type", "/0");
  EVALUATE_TRACE_POST_FAILURE(1, AssertionTypeStrict, "/items/1/type",
                              "#/items/1/type", "/1");
  EVALUATE_TRACE_POST_FAILURE(2, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type boolean "
                               "but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The first 2 items of the array value were expected to validate against "
      "the corresponding subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", \"bar\", \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LoopItems, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/1");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/2");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/1");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/2");
  EVALUATE_TRACE_POST_SUCCESS(3, LoopItems, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "Every item in the array value was expected to "
                               "validate against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ true, 5 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/0/type", "#/items/0/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/1/type", "#/items/1/type",
                     "/1");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/0/type",
                              "#/items/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/items/1/type",
                              "#/items/1/type", "/1");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The first 2 items of the array value were expected to validate against "
      "the corresponding subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ true, 5, \"foo\", \"bar\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 6);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/0/type", "#/items/0/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/1/type", "#/items/1/type",
                     "/1");
  EVALUATE_TRACE_PRE(3, LoopItems, "/additionalItems", "#/additionalItems", "");
  EVALUATE_TRACE_PRE(4, AssertionTypeStrict, "/additionalItems/type",
                     "#/additionalItems/type", "/2");
  EVALUATE_TRACE_PRE(5, AssertionTypeStrict, "/additionalItems/type",
                     "#/additionalItems/type", "/3");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/0/type",
                              "#/items/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/items/1/type",
                              "#/items/1/type", "/1");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_POST_SUCCESS(3, AssertionTypeStrict, "/additionalItems/type",
                              "#/additionalItems/type", "/2");
  EVALUATE_TRACE_POST_SUCCESS(4, AssertionTypeStrict, "/additionalItems/type",
                              "#/additionalItems/type", "/3");
  EVALUATE_TRACE_POST_SUCCESS(5, LoopItems, "/additionalItems",
                              "#/additionalItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The first 2 items of the array value were expected to validate against "
      "the corresponding subschemas");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 5,
      "Every item in the array value except for the first 2 was expected to "
      "validate against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ true, 5, 6, \"bar\" ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/0/type", "#/items/0/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/1/type", "#/items/1/type",
                     "/1");
  EVALUATE_TRACE_PRE(3, LoopItems, "/additionalItems", "#/additionalItems", "");
  EVALUATE_TRACE_PRE(4, AssertionTypeStrict, "/additionalItems/type",
                     "#/additionalItems/type", "/2");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/0/type",
                              "#/items/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/items/1/type",
                              "#/items/1/type", "/1");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/items", "#/items", "");
  EVALUATE_TRACE_POST_FAILURE(3, AssertionTypeStrict, "/additionalItems/type",
                              "#/additionalItems/type", "/2");
  EVALUATE_TRACE_POST_FAILURE(4, LoopItems, "/additionalItems",
                              "#/additionalItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The first 2 items of the array value were expected to validate against "
      "the corresponding subschemas");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The value was expected to be of type string but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4,
      "Every item in the array value except for the first 2 was expected to "
      "validate against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{1};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalOr, "/anyOf", "#/anyOf", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/anyOf/0/type", "#/anyOf/0/type",
                     "");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/anyOf/1/type", "#/anyOf/1/type",
                     "");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/anyOf/0/type",
                              "#/anyOf/0/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/anyOf/1/type",
                              "#/anyOf/1/type", "");
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalOr, "/anyOf", "#/anyOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type string but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The integer value was expected to validate against at least one of the "
      "3 given subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{true};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LogicalOr, "/anyOf", "#/anyOf", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/anyOf/0/type", "#/anyOf/0/type",
                     "");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/anyOf/1/type", "#/anyOf/1/type",
                     "");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrictAny, "/anyOf/2/type",
                     "#/anyOf/2/type", "");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/anyOf/0/type",
                              "#/anyOf/0/type", "");
  EVALUATE_TRACE_POST_FAILURE(1, AssertionTypeStrict, "/anyOf/1/type",
                              "#/anyOf/1/type", "");
  EVALUATE_TRACE_POST_FAILURE(2, AssertionTypeStrictAny, "/anyOf/2/type",
                              "#/anyOf/2/type", "");
  EVALUATE_TRACE_POST_FAILURE(3, LogicalOr, "/anyOf", "#/anyOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type string but it was of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type integer "
                               "but it was of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The value was expected to be of type number but it was of type boolean");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The boolean value was expected to validate against at least one of the "
      "3 given subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalXor, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/oneOf/0/type", "#/oneOf/0/type",
                     "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/oneOf/0/type",
                              "#/oneOf/0/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, LogicalXor, "/oneOf", "#/oneOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type string");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The string value was expected to validate against one and only one of "
      "the 4 given subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{3.14};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LogicalXor, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/oneOf/0/type", "#/oneOf/0/type",
                     "");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/oneOf/1/type", "#/oneOf/1/type",
                     "");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrictAny, "/oneOf/2/type",
                     "#/oneOf/2/type", "");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/oneOf/0/type",
                              "#/oneOf/0/type", "");
  EVALUATE_TRACE_POST_FAILURE(1, AssertionTypeStrict, "/oneOf/1/type",
                              "#/oneOf/1/type", "");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrictAny, "/oneOf/2/type",
                              "#/oneOf/2/type", "");
  EVALUATE_TRACE_POST_SUCCESS(3, LogicalXor, "/oneOf", "#/oneOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type string but it was of type number");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The value was expected to be an integer but it was a real number");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The value was expected to be of type number");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The number value was expected to validate against one and only one of "
      "the 4 given subschemas");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LogicalXor, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/oneOf/0/type", "#/oneOf/0/type",
                     "");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/oneOf/1/type", "#/oneOf/1/type",
                     "");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrictAny, "/oneOf/2/type",
                     "#/oneOf/2/type", "");
  EVALUATE_TRACE_PRE(4, AssertionTypeStrict, "/oneOf/3/type", "#/oneOf/3/type",
                     "");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/oneOf/0/type",
                              "#/oneOf/0/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/oneOf/1/type",
                              "#/oneOf/1/type", "");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrictAny, "/oneOf/2/type",
                              "#/oneOf/2/type", "");
  EVALUATE_TRACE_POST_FAILURE(3, AssertionTypeStrict, "/oneOf/3/type",
                              "#/oneOf/3/type", "");
  EVALUATE_TRACE_POST_FAILURE(4, LogicalXor, "/oneOf", "#/oneOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The value was expected to be of type string but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1,
                               "The value was expected to be of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "The value was expected to be of type number");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "The value was expected to be of type boolean "
                               "but it was of type integer");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4,
      "The integer value was expected to validate against one and only one of "
      "the 4 given subschemas");
}

TEST(JSONSchema_compile_draft4, oneOf_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "oneOf": [
      { "type": "number" }
    ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{3.14};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalXor, "/oneOf", "#/oneOf", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrictAny, "/oneOf/0/type",
                     "#/oneOf/0/type", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrictAny, "/oneOf/0/type",
                              "#/oneOf/0/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, LogicalXor, "/oneOf", "#/oneOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The value was expected to be of type number");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "The number value was expected to validate against the given subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2, \"baz\": 3 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/dependencies", "#/dependencies",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "Because the object value defined the property \"foo\", it was also "
      "expected to define the properties \"bar\", and \"baz\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"baz\": 3 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_POST_FAILURE(0, LogicalAnd, "/dependencies", "#/dependencies",
                              "");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "Because the object value defined the property \"foo\", it was also "
      "expected to define the property \"bar\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"qux\": 1, \"extra\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_PRE(1, AssertionDefines, "/dependencies/qux/required",
                     "#/dependencies/qux/required", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionDefines, "/dependencies/qux/required",
                              "#/dependencies/qux/required", "");
  EVALUATE_TRACE_POST_SUCCESS(1, LogicalAnd, "/dependencies", "#/dependencies",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to define the property \"extra\"");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "Because the object value defined the property \"qux\", it was also "
      "expected to successfully validate against the corresponding \"qux\" "
      "subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"qux\": 1 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_PRE(1, AssertionDefines, "/dependencies/qux/required",
                     "#/dependencies/qux/required", "");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionDefines, "/dependencies/qux/required",
                              "#/dependencies/qux/required", "");
  EVALUATE_TRACE_POST_FAILURE(1, LogicalAnd, "/dependencies", "#/dependencies",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to define the property \"extra\"");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1,
      "Because the object value defined the property \"qux\", it was also "
      "expected to successfully validate against the corresponding \"qux\" "
      "subschema");
}

TEST(JSONSchema_compile_draft4, dependencies_6) {
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"none\": 1 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/dependencies", "#/dependencies",
                              "");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value did not define the properties \"foo\", or \"qux\"");
}

TEST(JSONSchema_compile_draft4, dependencies_7) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "dependencies": {
      "foo": [ "bar", "baz" ]
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"none\": 1 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/dependencies", "#/dependencies",
                              "");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The object value did not define the property \"foo\"");
}

TEST(JSONSchema_compile_draft4, dependencies_8) {
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"qux\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);
  EVALUATE_TRACE_PRE(0, LogicalAnd, "/dependencies", "#/dependencies", "");
  EVALUATE_TRACE_POST_FAILURE(0, LogicalAnd, "/dependencies", "#/dependencies",
                              "");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "Because the object value defined the properties \"foo\", and \"qux\", "
      "it was also expected to define the properties \"bar\", and \"baz\", and "
      "it was also expected to successfully validate against the corresponding "
      "\"qux\" subschema");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{}")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionEqualsAny, "/enum", "#/enum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionEqualsAny, "/enum", "#/enum", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value {} was expected to equal one of the 3 declared values");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionEqualsAny, "/enum", "#/enum", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionEqualsAny, "/enum", "#/enum", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value {\"foo\":1} was expected to equal one of the following "
      "values: 1, \"foo\", and {}");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{1};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionEqual, "/enum", "#/enum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionEqual, "/enum", "#/enum", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The integer value 1 was expected to equal the integer constant 1");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionUnique, "/uniqueItems", "#/uniqueItems", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionUnique, "/uniqueItems",
                              "#/uniqueItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The array value was expected to not contain duplicate items");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2, 3 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionUnique, "/uniqueItems", "#/uniqueItems", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionUnique, "/uniqueItems",
                              "#/uniqueItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The array value was expected to not contain duplicate items");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 2, 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionUnique, "/uniqueItems", "#/uniqueItems", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionUnique, "/uniqueItems",
                              "#/uniqueItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The array value contained the following duplicate item: 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 2, 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, uniqueItems_6) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "uniqueItems": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 2, 1, 2, 3, 2, 2, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionUnique, "/uniqueItems", "#/uniqueItems", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionUnique, "/uniqueItems",
                              "#/uniqueItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The array value contained the following duplicate item: 2");
}

TEST(JSONSchema_compile_draft4, uniqueItems_7) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "uniqueItems": true
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 2, 1, 2, 3, 2, 2, 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionUnique, "/uniqueItems", "#/uniqueItems", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionUnique, "/uniqueItems",
                              "#/uniqueItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The array value contained the following duplicate items: 1, and 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"xx"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeGreater, "/minLength", "#/minLength", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionSizeGreater, "/minLength",
                              "#/minLength", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The string value \"xx\" was expected to consist of at least 2 "
      "characters and it consisted of 2 characters");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"x"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeGreater, "/minLength", "#/minLength", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionSizeGreater, "/minLength",
                              "#/minLength", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The string value \"x\" was expected to consist of at least 2 characters "
      "but it consisted of 1 character");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"xx"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeLess, "/maxLength", "#/maxLength", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionSizeLess, "/maxLength", "#/maxLength",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The string value \"xx\" was expected to consist of at most 2 characters "
      "and it consisted of 2 characters");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"xxx"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeLess, "/maxLength", "#/maxLength", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionSizeLess, "/maxLength", "#/maxLength",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The string value \"xxx\" was expected to consist of at most 2 "
      "characters but it consisted of 3 characters");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeGreater, "/minItems", "#/minItems", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionSizeGreater, "/minItems",
                              "#/minItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The array value was expected to contain at "
                               "least 2 items and it contained 2 items");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeGreater, "/minItems", "#/minItems", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionSizeGreater, "/minItems",
                              "#/minItems", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The array value was expected to contain at "
                               "least 2 items but it contained 1 item");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeLess, "/maxItems", "#/maxItems", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionSizeLess, "/maxItems", "#/maxItems",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The array value was expected to contain at "
                               "most 2 items and it contained 2 items");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2, 3 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeLess, "/maxItems", "#/maxItems", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionSizeLess, "/maxItems", "#/maxItems",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The array value was expected to contain at "
                               "most 2 items but it contained 3 items");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeGreater, "/minProperties",
                     "#/minProperties", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionSizeGreater, "/minProperties",
                              "#/minProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to contain at least 2 properties and it "
      "contained 2 properties: \"bar\", and \"foo\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeGreater, "/minProperties",
                     "#/minProperties", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionSizeGreater, "/minProperties",
                              "#/minProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to contain at least 2 properties but it "
      "contained 1 property: \"foo\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2 }")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeLess, "/maxProperties", "#/maxProperties",
                     "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionSizeLess, "/maxProperties",
                              "#/maxProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to contain at most 2 properties and it "
      "contained 2 properties: \"bar\", and \"foo\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("{ \"foo\": 1, \"bar\": 2, \"baz\": 3 }")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionSizeLess, "/maxProperties", "#/maxProperties",
                     "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionSizeLess, "/maxProperties",
                              "#/maxProperties", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The object value was expected to contain at most 2 properties but it "
      "contained 3 properties: \"bar\", \"baz\", and \"foo\"");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2.1};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionGreaterEqual, "/minimum", "#/minimum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionGreaterEqual, "/minimum", "#/minimum",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The number value 2.1 was expected to be "
                               "greater than or equal to the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionGreaterEqual, "/minimum", "#/minimum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionGreaterEqual, "/minimum", "#/minimum",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The integer value 2 was expected to be greater "
                               "than or equal to the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{1.8};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionGreaterEqual, "/minimum", "#/minimum", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionGreaterEqual, "/minimum", "#/minimum",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The number value 1.8 was expected to be "
                               "greater than or equal to the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{1.9};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionLessEqual, "/maximum", "#/maximum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionLessEqual, "/maximum", "#/maximum",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The number value 1.9 was expected to be less "
                               "than or equal to the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionLessEqual, "/maximum", "#/maximum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionLessEqual, "/maximum", "#/maximum",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The integer value 2 was expected to be less "
                               "than or equal to the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2.1};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionLessEqual, "/maximum", "#/maximum", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionLessEqual, "/maximum", "#/maximum",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The number value 2.1 was expected to be less "
                               "than or equal to the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2.1};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionGreater, "/minimum", "#/minimum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionGreater, "/minimum", "#/minimum", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The number value 2.1 was expected to be greater than the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionGreater, "/minimum", "#/minimum", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionGreater, "/minimum", "#/minimum", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The integer value 2 was expected to be greater "
                               "than the integer 2, but they were equal");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionGreaterEqual, "/minimum", "#/minimum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionGreaterEqual, "/minimum", "#/minimum",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The integer value 2 was expected to be greater "
                               "than or equal to the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{1.9};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionLess, "/maximum", "#/maximum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionLess, "/maximum", "#/maximum", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The number value 1.9 was expected to be less than the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionLess, "/maximum", "#/maximum", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionLess, "/maximum", "#/maximum", "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The integer value 2 was expected to be less "
                               "than the integer 2, but they were equal");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionLessEqual, "/maximum", "#/maximum", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionLessEqual, "/maximum", "#/maximum",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The integer value 2 was expected to be less "
                               "than or equal to the integer 2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{6};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionDivisible, "/multipleOf", "#/multipleOf", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionDivisible, "/multipleOf",
                              "#/multipleOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The integer value 6 was expected to be divisible by the integer 3");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{7};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionDivisible, "/multipleOf", "#/multipleOf", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionDivisible, "/multipleOf",
                              "#/multipleOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The integer value 7 was expected to be divisible by the integer 3");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{6.4};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionDivisible, "/multipleOf", "#/multipleOf", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionDivisible, "/multipleOf",
                              "#/multipleOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The number value 6.4 was expected to be divisible by the number 3.2");
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
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{6.0};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionDivisible, "/multipleOf", "#/multipleOf", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionDivisible, "/multipleOf",
                              "#/multipleOf", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The number value 6 was expected to be divisible by the number 3.2");
}

TEST(JSONSchema_compile_draft4, invalid_ref_top_level) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/i-dont-exist"
  })JSON")};

  try {
    sourcemeta::jsontoolkit::compile(
        schema, sourcemeta::jsontoolkit::default_schema_walker,
        sourcemeta::jsontoolkit::official_resolver,
        sourcemeta::jsontoolkit::default_schema_compiler,
        sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full);
  } catch (const sourcemeta::jsontoolkit::SchemaReferenceError &error) {
    EXPECT_EQ(error.location(), sourcemeta::jsontoolkit::Pointer({"$ref"}));
    SUCCEED();
  } catch (...) {
    throw;
  }
}

TEST(JSONSchema_compile_draft4, invalid_ref_nested) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "$ref": "#/definitions/i-dont-exist"
      }
    }
  })JSON")};

  try {
    sourcemeta::jsontoolkit::compile(
        schema, sourcemeta::jsontoolkit::default_schema_walker,
        sourcemeta::jsontoolkit::official_resolver,
        sourcemeta::jsontoolkit::default_schema_compiler,
        sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full);
  } catch (const sourcemeta::jsontoolkit::SchemaReferenceError &error) {
    EXPECT_EQ(error.location(),
              sourcemeta::jsontoolkit::Pointer({"properties", "foo", "$ref"}));
  } catch (...) {
    throw;
  }
}

TEST(JSONSchema_compile_draft4, invalid_ref_embedded) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "id": "https://example.com",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalProperties": {
      "$ref": "#/definitions/bar"
    },
    "definitions": {
      "bar": {
        "id": "https://example.com/nested",
        "additionalProperties": {
          "$ref": "#/definitions/baz"
        }
      },
      "baz": {}
    }
  })JSON")};

  try {
    sourcemeta::jsontoolkit::compile(
        schema, sourcemeta::jsontoolkit::default_schema_walker,
        sourcemeta::jsontoolkit::official_resolver,
        sourcemeta::jsontoolkit::default_schema_compiler,
        sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full);
  } catch (const sourcemeta::jsontoolkit::SchemaReferenceError &error) {
    EXPECT_EQ(error.location(),
              sourcemeta::jsontoolkit::Pointer(
                  {"definitions", "bar", "additionalProperties", "$ref"}));
  } catch (...) {
    throw;
  }
}

TEST(JSONSchema_compile_draft4, metadata) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "My title",
    "description": "My description",
    "default": 1
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, unknown_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "fooBar": "baz"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, unknown_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "x-test": 1
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, reference_from_unknown_keyword) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "$ref": "#/$defs/bar" },
      "baz": { "type": "string" }
    },
    "$defs": {
      "bar": {
        "$ref": "#/properties/baz"
      }
    }
  })JSON")};

  try {
    sourcemeta::jsontoolkit::compile(
        schema, sourcemeta::jsontoolkit::default_schema_walker,
        sourcemeta::jsontoolkit::official_resolver,
        sourcemeta::jsontoolkit::default_schema_compiler,
        sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full);
  } catch (const sourcemeta::jsontoolkit::SchemaReferenceError &error) {
    EXPECT_EQ(error.id(), "#/properties/baz");
    EXPECT_EQ(error.location(),
              sourcemeta::jsontoolkit::Pointer({"$defs", "bar", "$ref"}));
  } catch (...) {
    throw;
  }
}

TEST(JSONSchema_compile_draft4, format_uri_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "format": "uri"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{6};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_draft4, format_uri_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "format": "uri"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"https://www.example.com"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionStringType, "/format", "#/format", "");
  EVALUATE_TRACE_POST_SUCCESS(0, AssertionStringType, "/format", "#/format",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0,
                               "The string value \"https://www.example.com\" "
                               "was expected to represent a valid URI");
}

TEST(JSONSchema_compile_draft4, format_uri_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "format": "uri"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler,
      sourcemeta::jsontoolkit::SchemaCompilerCompilationMode::Full)};

  const sourcemeta::jsontoolkit::JSON instance{"!!!x::://"};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, AssertionStringType, "/format", "#/format", "");
  EVALUATE_TRACE_POST_FAILURE(0, AssertionStringType, "/format", "#/format",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The string value \"!!!x::://\" was expected to represent a valid URI");
}
