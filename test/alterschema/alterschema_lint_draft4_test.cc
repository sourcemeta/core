#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include "alterschema_test_utils.h"

TEST(AlterSchema_lint_draft4, enum_with_type_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "string",
    "enum": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ "foo", "bar" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, enum_validation_keywords_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ "foo", "bar" ],
    "minimum": 0
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ "foo", "bar" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, enum_validation_keywords_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 1, 2, 3 ],
    "minLength": 0,
    "maxLength": 5
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, enum_validation_keywords_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ { "a": 1 }, { "b": 2 } ],
    "minLength": 3,
    "minimum": 0
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ { "a": 1 }, { "b": 2 } ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, single_type_array_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": [ "string" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "string"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, additional_properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalProperties": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, additional_properties_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalProperties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, additional_properties_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalProperties": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalProperties": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, items_schema_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, items_schema_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, items_schema_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": { "type": "string" }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": { "type": "string" }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, items_array_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": []
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, duplicate_enum_values_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 1, {}, 2, 1, 1, 3, {} ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 1, {}, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, duplicate_required_values_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo", "bar", "baz", "foo" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "bar", "baz", "foo" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, duplicate_allof_branches_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [ { "type": "string" }, { "type": "integer" }, { "type": "string" } ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "allOf": [ { "type": "string" } ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, duplicate_allof_branches_2) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
      { "type": "number" },
      { "type": "string" },
      { "type": "number" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": 1,
    "allOf": [
      { "type": "string", "minLength": 0 }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, duplicate_allof_branches_3) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
      { "type": "number" },
      { "type": "number" },
      { "type": "string" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": 1,
    "allOf": [
      { "type": "string", "minLength": 0 }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, duplicate_allof_branches_4) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
      { "type": "number" },
      { "type": "string" },
      { "type": "number" },
      { "type": "number" },
      { "type": "number" },
      { "type": "string" },
      { "type": "string" },
      { "type": "string" },
      { "type": "number" },
      { "type": "number" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": 1,
    "allOf": [
      { "type": "string", "minLength": 0 }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, duplicate_anyof_branches_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "anyOf": [ { "type": "string" }, { "type": "integer" }, { "type": "string" } ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "anyOf": [ { "type": "integer" }, { "type": "string" } ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, maximum_real_for_integer_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "maximum": 3.2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "maximum": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, minimum_real_for_integer_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "minimum": 3.2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "minimum": 4
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, dependent_required_tautology_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo" ],
    "dependencies": {
      "foo": [ "bar" ],
      "xxx": { "type": "string" },
      "yyy": [ "extra" ]
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo", "bar" ],
    "dependencies": {
      "xxx": { "type": "string" },
      "yyy": [ "extra" ]
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, dependent_required_tautology_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo" ],
    "dependencies": {
      "foo": [ "bar" ],
      "bar": [ "baz" ]
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo", "bar", "baz" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, pattern_properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "patternProperties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, unsatisfiable_min_properties_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo", "bar", "bar", "baz" ],
    "minProperties": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "bar", "baz", "foo" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, unsatisfiable_min_properties_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo", "bar", "bar", "baz" ],
    "minProperties": 4
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "bar", "baz", "foo" ],
    "minProperties": 4
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, unsatisfiable_min_properties_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "foo", "bar", "bar", "baz" ],
    "minProperties": 2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "required": [ "bar", "baz", "foo" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, drop_non_array_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "array",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "array",
    "minItems": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, drop_non_boolean_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "boolean",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "boolean"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, drop_non_null_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "null",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "null"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, drop_non_numeric_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "number",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "number"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, drop_non_numeric_keywords_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "integer",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "integer"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, drop_non_object_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "object",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "object",
    "additionalProperties": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, drop_non_string_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "string",
    "additionalProperties": false,
    "minLength": 2,
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "title": "Test",
    "type": "string",
    "minLength": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, type_boolean_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "boolean"
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ false, true ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, type_boolean_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "boolean",
    "enum": [ 1, 2, 3 ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "boolean",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, type_boolean_as_enum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "boolean"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "boolean"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, type_null_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "null"
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ null ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, type_null_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "null",
    "enum": [ 1, 2, 3 ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "null",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, type_null_as_enum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "null"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "null"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, boolean_true_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": true
    }
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": [ "null", "boolean", "object", "array", "string", "number", "integer" ],
    "properties": {
      "foo": {
        "type": [ "null", "boolean", "object", "array", "string", "number", "integer" ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, min_properties_covered_by_required_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "minProperties": 1,
    "required": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "minProperties": 2,
    "properties": {},
    "required": [ "foo", "bar" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, min_properties_implicit_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "required": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "required": [ "foo", "bar" ],
    "properties": {},
    "minProperties": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, min_properties_implicit_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "minProperties": 2,
    "required": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {},
    "required": [ "foo", "bar" ],
    "minProperties": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, equal_numeric_bounds_to_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "minimum": 3,
    "maximum": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, equal_numeric_bounds_to_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "minimum": 3,
    "maximum": 3
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, unnecessary_allof_wrapper_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
      { "$ref": "https://example.com" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "allOf": [
      { "$ref": "https://example.com" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, multiple_of_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "multipleOf": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, multiple_of_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": 1.0
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, multiple_of_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "multipleOf": 1,
    "minimum": 0
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "minimum": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, multiple_of_default_no_change_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "multipleOf": 2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "integer",
    "multipleOf": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, multiple_of_default_no_change_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": 0.5
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": 0.5
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, multiple_of_default_no_change_zero) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": 0
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, multiple_of_default_no_change_negative) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": -1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "number",
    "multipleOf": -1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, unnecessary_allof_wrapper_properties_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "type": "string" }
    },
    "allOf": [
      {
        "$ref": "https://example.com",
        "properties": {
          "bar": { "pattern": "^f" }
        }
      }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": { "type": "string" }
    },
    "allOf": [
      {
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, draft_official_dialect_without_empty_fragment_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema",
    "type": "string"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "string"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, additional_items_with_schema_items_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": {
      "type": "number"
    },
    "additionalItems": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": {
      "type": "number"
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, additional_items_with_schema_items_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "array",
    "items": {
      "type": "string"
    },
    "additionalItems": {
      "type": "boolean"
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "array",
    "items": {
      "type": "string"
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4,
     additional_items_with_schema_items_no_change_array_items) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": [
      { "type": "string" },
      { "type": "number" }
    ],
    "additionalItems": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": [
      { "type": "string" },
      { "type": "number" }
    ],
    "additionalItems": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4,
     additional_items_with_schema_items_no_change_no_items) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalItems": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "additionalItems": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4,
     additional_items_with_schema_items_boolean_items_true) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": true,
    "additionalItems": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4,
     additional_items_with_schema_items_boolean_items_false) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": false,
    "additionalItems": {
      "type": "string"
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "items": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, draft_ref_siblings_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/foo",
    "type": "string"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, draft_ref_siblings_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/foo",
    "type": "string",
    "minLength": 5,
    "description": "A string field"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, draft_ref_siblings_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "http://example.com/schema",
    "$ref": "#/definitions/foo",
    "type": "string",
    "minLength": 5,
    "description": "Documentation"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "http://example.com/schema",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, draft_ref_siblings_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/foo",
    "description": "Documentation only"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, draft_ref_siblings_5) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/foo"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$ref": "#/definitions/foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, draft_ref_siblings_6) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
      "nested": {
        "$ref": "#/definitions/foo",
        "type": "string",
        "description": "ignored sibling"
      }
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object",
    "properties": {
      "nested": {
        "$ref": "#/definitions/foo"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_draft4, non_applicable_type_specific_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ true, false ],
    "maxItems": 4,
    "maxLength": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "enum": [ true, false ]
  })JSON");

  EXPECT_EQ(document, expected);
}
