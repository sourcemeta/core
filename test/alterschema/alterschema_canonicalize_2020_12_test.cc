#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include "alterschema_test_utils.h"

TEST(AlterSchema_canonicalize_2020_12, duplicate_allof_branches_2) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "allOf": [
      { "type": "number" },
      { "type": "string" },
      { "type": "number" }
    ]
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0,
    "allOf": [
      { "type": "number" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, duplicate_allof_branches_3) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "allOf": [
      { "type": "number" },
      { "type": "number" },
      { "type": "string" }
    ]
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0,
    "allOf": [
      { "type": "number" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, duplicate_allof_branches_4) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0,
    "allOf": [
      { "type": "number" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, duplicate_allof_branches_5) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "multipleOf": 2,
    "allOf": [
      { "type": "number", "multipleOf": 1 }
    ]
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "multipleOf": 2,
    "type": "number",
    "anyOf": [
      { "enum": [ null ] },
      { "enum": [ false, true ] },
      { "type": "object", "minProperties": 0, "properties": {} },
      { "type": "array", "minItems": 0 },
      { "type": "string", "minLength": 0 },
      { "type": "number", "multipleOf": 1 }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, dependent_required_tautology_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "required": [ "foo" ],
    "dependentRequired": {
      "foo": [ "bar" ],
      "bar": [ "baz" ]
    }
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "required": [ "foo", "bar", "baz" ],
    "dependentRequired": {},
    "minProperties": 3,
    "properties": {
      "foo": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      },
      "bar": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      },
      "baz": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_boolean_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "boolean"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "enum": [ false, true ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_boolean_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "boolean",
    "enum": [ 1, 2, 3 ]
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "boolean",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_null_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "null"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "enum": [ null ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_null_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "null",
    "enum": [ 1, 2, 3 ]
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "null",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, const_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "const": 1
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "enum": [ 1 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, exclusive_maximum_integer_to_maximum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "exclusiveMaximum": 1
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "multipleOf": 1,
    "maximum": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, exclusive_maximum_integer_to_maximum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "exclusiveMaximum": 1.2
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "multipleOf": 1,
    "maximum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, exclusive_maximum_integer_to_maximum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "number",
    "exclusiveMaximum": 1
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "number",
    "exclusiveMaximum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, exclusive_maximum_integer_to_maximum_5) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "exclusiveMaximum": 1
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "anyOf": [
      { "enum": [ null ] },
      { "enum": [ false, true ] },
      { "properties": {}, "minProperties": 0, "type": "object" },
      { "minItems": 0, "type": "array" },
      { "minLength": 0, "type": "string" },
      { "type": "number", "exclusiveMaximum": 1 }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, exclusive_minimum_integer_to_minimum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "exclusiveMinimum": 1
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "multipleOf": 1,
    "minimum": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, exclusive_minimum_integer_to_minimum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "exclusiveMinimum": 1.2
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "multipleOf": 1,
    "minimum": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, exclusive_minimum_integer_to_minimum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "number",
    "exclusiveMinimum": 1
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "number",
    "exclusiveMinimum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, exclusive_minimum_integer_to_minimum_5) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "exclusiveMinimum": 1
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "anyOf": [
      { "enum": [ null ] },
      { "enum": [ false, true ] },
      { "properties": {}, "minProperties": 0, "type": "object" },
      { "minItems": 0, "type": "array" },
      { "minLength": 0, "type": "string" },
      { "type": "number", "exclusiveMinimum": 1 }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, boolean_true_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": true
    }
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "anyOf": [
      { "enum": [ null ] },
      { "enum": [ false, true ] },
      {
        "minProperties": 0,
        "type": "object",
        "properties": {
          "foo": {
            "anyOf": [
              { "enum": [ null ] },
              { "enum": [ false, true ] },
              { "properties": {}, "minProperties": 0, "type": "object" },
              { "minItems": 0, "type": "array" },
              { "minLength": 0, "type": "string" },
              { "type": "number" }
            ]
          }
        }
      },
      { "minItems": 0, "type": "array" },
      { "minLength": 0, "type": "string" },
      { "type": "number" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_array_to_any_of_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo",
    "type": [ "integer", "number", "string" ],
    "minimum": 5
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo",
    "anyOf": [
      { "type": "integer", "minimum": 5, "multipleOf": 1 },
      { "type": "number", "minimum": 5 },
      { "type": "string", "minLength": 0 }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_array_to_any_of_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": [ "integer", "number", "string" ],
    "anyOf": [
      { "minimum": 4, "type": "integer" },
      { "maximum": 8, "type": "integer" }
    ]
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "anyOf": [
      { "type": "integer", "multipleOf": 1 },
      { "type": "number" },
      { "type": "string", "minLength": 0 }
    ],
    "allOf": [
      {
        "anyOf": [
          { "minimum": 4, "type": "integer", "multipleOf": 1 },
          { "maximum": 8, "type": "integer", "multipleOf": 1 }
        ]
      }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_array_to_any_of_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": [ "object", "array", "string" ],
    "additionalProperties": {
      "$anchor": "foo",
      "type": "string"
    }
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "anyOf": [
      {
        "type": "object",
        "properties": {},
        "minProperties": 0,
        "additionalProperties": {
          "$anchor": "foo",
          "type": "string",
          "minLength": 0
        }
      },
      {
        "type": "array",
        "minItems": 0
      },
      {
        "type": "string",
        "minLength": 0
      }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_array_to_any_of_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": [ "object", "array" ],
    "$ref": "#/$defs/foo",
    "$defs": {
      "foo": {
        "type": "string",
        "minLength": 0
      }
    }
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "anyOf": [
      {
        "type": "object",
        "properties": {},
        "minProperties": 0
      },
      {
        "type": "array",
        "minItems": 0
      }
    ],
    "$ref": "#/$defs/foo",
    "$defs": {
      "foo": {
        "type": "string",
        "minLength": 0
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, type_array_to_any_of_5) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": [ "string", "number" ],
    "anyOf": [
      { "minLength": 1 },
      { "minimum": 0 }
    ],
    "allOf": [
      { "title": "My schema" }
    ]
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "anyOf": [
      { "type": "string", "minLength": 0 },
      { "type": "number" }
    ],
    "allOf": [
      {
        "anyOf": [
          {
            "anyOf": [
              { "enum": [ null ] },
              { "enum": [ false, true ] },
              { "type": "object", "minProperties": 0, "properties": {} },
              { "type": "array", "minItems": 0 },
              { "type": "string", "minLength": 1 },
              { "type": "number" }
            ]
          },
          {
            "anyOf": [
              { "enum": [ null ] },
              { "enum": [ false, true ] },
              { "type": "object", "minProperties": 0, "properties": {} },
              { "type": "array", "minItems": 0 },
              { "type": "string", "minLength": 0 },
              { "type": "number", "minimum": 0 }
            ]
          }
        ]
      }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, max_contains_covered_by_max_items_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "array",
    "contains": { "type": "string" },
    "maxContains": 2,
    "maxItems": 1
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "array",
    "contains": {
      "type": "string",
      "minLength": 0
    },
    "maxContains": 1,
    "minItems": 0,
    "maxItems": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, min_properties_covered_by_required_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "minProperties": 1,
    "required": [ "foo", "bar" ]
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "minProperties": 2,
    "required": [ "foo", "bar" ],
    "properties": {
      "foo": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      },
      "bar": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, min_properties_implicit_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "required": [ "foo", "bar" ]
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "required": [ "foo", "bar" ],
    "minProperties": 2,
    "properties": {
      "foo": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      },
      "bar": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, min_properties_implicit_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "minProperties": 2,
    "required": [ "foo", "bar" ]
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "required": [ "foo", "bar" ],
    "minProperties": 2,
    "properties": {
      "foo": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      },
      "bar": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, min_items_given_min_contains_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "array",
    "contains": { "type": "boolean" },
    "minContains": 3
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "array",
    "contains": { "enum": [ false, true ] },
    "minContains": 3,
    "minItems": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, min_items_given_min_contains_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "array",
    "minContains": 3
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "array",
    "minItems": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12,
     exclusive_maximum_integer_to_maximum_decimal_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "exclusiveMaximum": 1.0e400
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "multipleOf": 1,
    "maximum": 10.00000000000000e+399
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12,
     exclusive_minimum_integer_to_minimum_decimal_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "exclusiveMinimum": 1.0e400
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "multipleOf": 1,
    "minimum": 1.0e+400
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_comment_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$comment": "This is a comment",
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_title_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "title": "My Schema",
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_description_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "description": "A description of the schema",
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_unknown_keyword_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foobar": "unknown keyword",
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_x_prefix_keyword_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "x-custom": "custom extension",
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_multiple_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$comment": "A comment",
    "title": "My Schema",
    "description": "A description",
    "x-custom": "extension",
    "unknown": true,
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_nested_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "title": "Root schema",
    "type": "object",
    "properties": {
      "foo": {
        "title": "Foo property",
        "$comment": "This is foo",
        "type": "string"
      }
    }
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "type": "string",
        "minLength": 0
      }
    },
    "type": "object",
    "minProperties": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_examples_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "examples": [ "foo", "bar" ],
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "default": "hello",
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_deprecated_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "deprecated": true,
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, no_metadata_readonly_writeonly_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "readOnly": true,
    "writeOnly": false,
    "type": "string"
  })JSON");

  CANONICALIZE(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, circular_ref_through_defs_1) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "a": { "$ref": "#/$defs/b" },
      "b": { "$ref": "#/$defs/a" }
    },
    "$ref": "#/$defs/a"
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "a": { "$ref": "#/$defs/b" },
      "b": { "$ref": "#/$defs/a" }
    },
    "$ref": "#/$defs/a"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, conflicting_type_and_const_1) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "const": 42
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "enum": [ 42 ],
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, conflicting_type_and_enum_1) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "enum": [ 1, 2, 3 ]
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "enum": [ 1, 2, 3 ],
    "minLength": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, allof_two_required_branches_1) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "allOf": [
      { "required": [ "a" ] },
      { "required": [ "b" ] }
    ]
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "required": [ "b" ],
    "anyOf": [
      { "enum": [ null ] },
      { "enum": [ false, true ] },
      {
        "type": "object",
        "required": [ "a" ],
        "properties": {
          "a": {
            "anyOf": [
              { "enum": [ null ] },
              { "enum": [ false, true ] },
              { "type": "object", "minProperties": 0, "properties": {} },
              { "type": "array", "minItems": 0 },
              { "type": "string", "minLength": 0 },
              { "type": "number" }
            ]
          }
        },
        "minProperties": 1
      },
      { "type": "array", "minItems": 0 },
      { "type": "string", "minLength": 0 },
      { "type": "number" }
    ],
    "minProperties": 1,
    "properties": {
      "b": {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          { "type": "object", "minProperties": 0, "properties": {} },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, allof_min_max_constraints_1) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "allOf": [
      { "type": "integer", "minimum": 0 },
      { "maximum": 100 }
    ]
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "integer",
    "maximum": 100,
    "minimum": 0,
    "multipleOf": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, allof_two_properties_branches_1) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "allOf": [
      { "properties": { "a": {} } },
      { "properties": { "b": {} } }
    ]
  })JSON");

  CANONICALIZE(document);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "anyOf": [
      { "enum": [ null ] },
      { "enum": [ false, true ] },
      {
        "type": "object",
        "properties": {
          "a": {
            "anyOf": [
              { "enum": [ null ] },
              { "enum": [ false, true ] },
              { "type": "object", "minProperties": 0, "properties": {} },
              { "type": "array", "minItems": 0 },
              { "type": "string", "minLength": 0 },
              { "type": "number" }
            ]
          }
        },
        "minProperties": 0
      },
      { "type": "array", "minItems": 0 },
      { "type": "string", "minLength": 0 },
      { "type": "number" }
    ],
    "allOf": [
      {
        "anyOf": [
          { "enum": [ null ] },
          { "enum": [ false, true ] },
          {
            "type": "object",
            "properties": {
              "b": {
                "anyOf": [
                  { "enum": [ null ] },
                  { "enum": [ false, true ] },
                  { "type": "object", "minProperties": 0, "properties": {} },
                  { "type": "array", "minItems": 0 },
                  { "type": "string", "minLength": 0 },
                  { "type": "number" }
                ]
              }
            },
            "minProperties": 0
          },
          { "type": "array", "minItems": 0 },
          { "type": "string", "minLength": 0 },
          { "type": "number" }
        ]
      }
    ],
    "minProperties": 0,
    "properties": {}
  })JSON");

  EXPECT_EQ(document, expected);
}
