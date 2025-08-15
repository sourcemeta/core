#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include "alterschema_test_utils.h"

TEST(AlterSchema_lint_draft3, enum_with_type_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "string",
    "enum": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ "foo", "bar" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_with_type_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "string",
    "enum": [ "foo", 1 ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "string",
    "enum": [ "foo", 1 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_with_type_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": [ "string", "null" ],
    "enum": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ "foo", "bar" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_with_type_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": [ "string", "null" ],
    "enum": [ "foo", "bar", "null" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ "foo", "bar", "null" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_validation_keywords_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ "foo", "bar" ],
    "minimum": 0
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ "foo", "bar" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_validation_keywords_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 1, 2, 3 ],
    "minLength": 0,
    "maxLength": 5
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_validation_keywords_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ { "a": 1 }, { "b": 2 } ],
    "minLength": 3,
    "minimum": 0
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ { "a": 1 }, { "b": 2 } ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_validation_keywords_default_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 1, "foo" ],
    "minLength": 2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 1, "foo" ],
    "minLength": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_validation_keywords_default_5) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 1, "foo" ],
    "minLength": 2,
    "minimum": 0,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 1, "foo" ],
    "minLength": 2,
    "minimum": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_validation_keywords_default_6) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ { "name": "alice" }, { "age": 25 } ],
    "properties": {
      "name": { "type": "string" },
      "age": { "type": "number" }
    },
    "minLength": 5,
    "minimum": 10
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ { "name": "alice" }, { "age": 25 } ],
    "properties": {
      "name": { "type": "string" },
      "age": { "type": "number" }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_validation_keywords_default_7) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ "small", "medium", "large" ],
    "title": "Size Options",
    "minLength": 3,
    "minItems": 2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ "small", "medium", "large" ],
    "title": "Size Options",
    "minLength": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, enum_validation_keywords_default_8) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 42, "hello", true, null, { "key": "value" }, [1, 2, 3] ],
    "minimum": 10,
    "minLength": 2,
    "minItems": 1,
    "minProperties": 1,
    "maxLength": 100,
    "maxItems": 10,
    "maxProperties": 5
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 42, "hello", true, null, { "key": "value" }, [1, 2, 3] ],
    "minimum": 10,
    "minLength": 2,
    "minItems": 1,
    "minProperties": 1,
    "maxLength": 100,
    "maxItems": 10,
    "maxProperties": 5
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, single_type_array_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": [ "string" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "string"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, additional_properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "additionalProperties": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, additional_properties_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "additionalProperties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, additional_properties_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "additionalProperties": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "additionalProperties": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, items_schema_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, items_schema_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, items_schema_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": { "type": "string" }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": { "type": "string" }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, items_array_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": []
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, duplicate_enum_values_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 1, {}, 2, 1, 1, 3, {} ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 1, {}, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, maximum_real_for_integer_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "integer",
    "maximum": 3.2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "integer",
    "maximum": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, minimum_real_for_integer_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "integer",
    "minimum": 3.2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "integer",
    "minimum": 4
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, dependent_required_tautology_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "required": [ "foo" ],
    "dependencies": {
      "foo": "bar",
      "xxx": { "type": "string" },
      "yyy": [ "extra" ]
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "required": [ "foo", "bar" ],
    "dependencies": {
      "xxx": { "type": "string" },
      "yyy": [ "extra" ]
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, dependent_required_tautology_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "required": [ "foo" ],
    "dependencies": {
      "foo": [ "bar" ],
      "bar": [ "baz" ]
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "required": [ "foo", "bar", "baz" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "properties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, pattern_properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "patternProperties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, drop_non_array_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "array",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "array",
    "minItems": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, drop_non_boolean_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "boolean",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "boolean"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, drop_non_null_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "null",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "null"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, drop_non_numeric_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "number",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "number"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, drop_non_numeric_keywords_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "integer",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "integer"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, drop_non_object_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "object",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "object",
    "additionalProperties": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, drop_non_string_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "string",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "title": "Test",
    "type": "string",
    "minLength": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, type_boolean_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "boolean"
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ false, true ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, type_boolean_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "boolean",
    "enum": [ 1, 2, 3 ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "boolean",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, type_boolean_as_enum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "boolean"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "boolean"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, type_null_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "null"
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ null ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, type_null_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "null",
    "enum": [ 1, 2, 3 ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "null",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, type_null_as_enum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "null"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "null"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, boolean_true_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "properties": {
      "foo": true
    }
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": [ "null", "boolean", "object", "array", "string", "number", "integer" ],
    "properties": {
      "foo": {
        "type": [ "null", "boolean", "object", "array", "string", "number", "integer" ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, equal_numeric_bounds_to_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "integer",
    "minimum": 3,
    "maximum": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, equal_numeric_bounds_to_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "integer",
    "minimum": 3,
    "maximum": 3
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, draft_official_dialect_without_empty_fragment_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema",
    "type": "string"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "type": "string"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, additional_items_with_schema_items_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": {
      "type": "number"
    },
    "additionalItems": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": {
      "type": "number"
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, additional_items_with_schema_items_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": {
      "type": "string"
    },
    "additionalItems": {
      "type": "boolean"
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": {
      "type": "string"
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3,
     additional_items_with_schema_items_boolean_items_true) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": true,
    "additionalItems": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3,
     additional_items_with_schema_items_boolean_items_false) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": false,
    "additionalItems": {
      "type": "string"
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, additional_items_with_schema_items_array_items) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": [
      { "type": "string" },
      { "type": "number" }
    ],
    "additionalItems": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "items": [
      { "type": "string" },
      { "type": "number" }
    ],
    "additionalItems": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, draft_ref_siblings_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "#/definitions/foo",
    "type": "string"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, draft_ref_siblings_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "#/definitions/foo",
    "type": "string",
    "minLength": 5,
    "description": "A string field"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, draft_ref_siblings_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "#/definitions/foo",
    "title": "Test Schema",
    "$comment": "This is a comment",
    "examples": [42],
    "default": null,
    "type": "object",
    "required": ["name"]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, draft_ref_siblings_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "id": "http://example.com/schema",
    "$ref": "#/definitions/foo",
    "type": "string",
    "description": "Documentation"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "id": "http://example.com/schema",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft3, non_applicable_type_specific_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ true, false ],
    "maxItems": 4,
    "maxLength": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "enum": [ true, false ]
  })JSON");

  EXPECT_EQ(document, expected);
}
