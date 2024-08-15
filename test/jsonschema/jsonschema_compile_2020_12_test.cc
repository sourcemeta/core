#include <gtest/gtest.h>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

#include "jsonschema_test_utils.h"

TEST(JSONSchema_compile_2020_12, unknown_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "fooBar": "baz"
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(0, "/fooBar", "#/fooBar", "");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(0, "/fooBar", "#/fooBar", "", "baz");

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0, "Emit an annotation");
}

TEST(JSONSchema_compile_2020_12, unknown_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "x-test": 1
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(0, "/x-test", "#/x-test", "");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(0, "/x-test", "#/x-test", "", 1);

  EVALUATE_TRACE_POST_DESCRIBE(instance, 0, "Emit an annotation");
}

TEST(JSONSchema_compile_2020_12, items_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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

TEST(JSONSchema_compile_2020_12, items_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LoopItems, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/1");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/2");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(4, "/items", "#/items", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/1");
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/2");
  EVALUATE_TRACE_POST_SUCCESS(3, LoopItems, "/items", "#/items", "");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(4, "/items", "#/items", "", true);

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3,
                               "Loop over the items of the target array");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 4, "Emit an annotation");
}

TEST(JSONSchema_compile_2020_12, items_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
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

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2,
                               "Loop over the items of the target array");
}

TEST(JSONSchema_compile_2020_12, items_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "type": "string" },
    "prefixItems": [
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
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/prefixItems/0/type",
                     "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_PRE(2, AssertionType, "/prefixItems/1/type",
                     "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(3, "/prefixItems", "#/prefixItems", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/prefixItems/0/type",
                              "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionType, "/prefixItems/1/type",
                              "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(2, "/prefixItems", "#/prefixItems", "",
                                        true);
  EVALUATE_TRACE_POST_SUCCESS(3, LogicalAnd, "/prefixItems", "#/prefixItems",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_2020_12, items_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "type": "string" },
    "prefixItems": [
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

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/prefixItems/0/type",
                     "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_PRE(2, AssertionType, "/prefixItems/1/type",
                     "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(3, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(4, LoopItems, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(5, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/2");
  EVALUATE_TRACE_PRE(6, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/3");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(7, "/items", "#/items", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/prefixItems/0/type",
                              "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionType, "/prefixItems/1/type",
                              "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(2, "/prefixItems", "#/prefixItems", "",
                                        1);
  EVALUATE_TRACE_POST_SUCCESS(3, LogicalAnd, "/prefixItems", "#/prefixItems",
                              "");
  EVALUATE_TRACE_POST_SUCCESS(4, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/2");
  EVALUATE_TRACE_POST_SUCCESS(5, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/3");
  EVALUATE_TRACE_POST_SUCCESS(6, LoopItems, "/items", "#/items", "");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(7, "/items", "#/items", "", true);

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 5, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 6,
                               "Loop over the items of the target array");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 7, "Emit an annotation");
}

TEST(JSONSchema_compile_2020_12, items_6) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "type": "string" },
    "prefixItems": [
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
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 6);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/prefixItems/0/type",
                     "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_PRE(2, AssertionType, "/prefixItems/1/type",
                     "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(3, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(4, LoopItems, "/items", "#/items", "");
  EVALUATE_TRACE_PRE(5, AssertionTypeStrict, "/items/type", "#/items/type",
                     "/2");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/prefixItems/0/type",
                              "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionType, "/prefixItems/1/type",
                              "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(2, "/prefixItems", "#/prefixItems", "",
                                        1);
  EVALUATE_TRACE_POST_SUCCESS(3, LogicalAnd, "/prefixItems", "#/prefixItems",
                              "");
  EVALUATE_TRACE_POST_FAILURE(4, AssertionTypeStrict, "/items/type",
                              "#/items/type", "/2");
  EVALUATE_TRACE_POST_FAILURE(5, LoopItems, "/items", "#/items", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The target is expected to match all of the given assertions");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 5,
                               "Loop over the items of the target array");
}

TEST(JSONSchema_compile_2020_12, prefixItems_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "prefixItems": [ { "type": "string" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_2020_12, prefixItems_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "prefixItems": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 1);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_POST_SUCCESS(0, LogicalAnd, "/prefixItems", "#/prefixItems",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0,
      "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_2020_12, prefixItems_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "prefixItems": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(1, AssertionType, "/prefixItems/0/type",
                     "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(2, "/prefixItems", "#/prefixItems", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionType, "/prefixItems/0/type",
                              "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(1, "/prefixItems", "#/prefixItems", "",
                                        0);
  EVALUATE_TRACE_POST_SUCCESS(2, LogicalAnd, "/prefixItems", "#/prefixItems",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_2020_12, prefixItems_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "prefixItems": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5, true, \"extra\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(1, AssertionType, "/prefixItems/0/type",
                     "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/prefixItems/1/type",
                     "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(3, "/prefixItems", "#/prefixItems", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionType, "/prefixItems/0/type",
                              "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/prefixItems/1/type",
                              "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(2, "/prefixItems", "#/prefixItems", "",
                                        1);
  EVALUATE_TRACE_POST_SUCCESS(3, LogicalAnd, "/prefixItems", "#/prefixItems",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_2020_12, prefixItems_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "prefixItems": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5, 1, \"extra\" ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 3);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(1, AssertionType, "/prefixItems/0/type",
                     "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/prefixItems/1/type",
                     "#/prefixItems/1/type", "/1");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionType, "/prefixItems/0/type",
                              "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_POST_FAILURE(1, AssertionTypeStrict, "/prefixItems/1/type",
                              "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_POST_FAILURE(2, LogicalAnd, "/prefixItems", "#/prefixItems",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2,
      "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_2020_12, prefixItems_6) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "prefixItems": [ { "type": "integer" }, { "type": "boolean" } ]
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 5, true ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LogicalAnd, "/prefixItems", "#/prefixItems", "");
  EVALUATE_TRACE_PRE(1, AssertionType, "/prefixItems/0/type",
                     "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/prefixItems/1/type",
                     "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(3, "/prefixItems", "#/prefixItems", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionType, "/prefixItems/0/type",
                              "#/prefixItems/0/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/prefixItems/1/type",
                              "#/prefixItems/1/type", "/1");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(2, "/prefixItems", "#/prefixItems", "",
                                        true);
  EVALUATE_TRACE_POST_SUCCESS(3, LogicalAnd, "/prefixItems", "#/prefixItems",
                              "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "The target is expected to match all of the given assertions");
}

TEST(JSONSchema_compile_2020_12, contains_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "contains": { "type": "string" }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{2};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 0);
}

TEST(JSONSchema_compile_2020_12, contains_2) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "contains": { "type": "string" }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, \"bar\", 3 ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LoopContains, "/contains", "#/contains", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/1");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(3, "/contains", "#/contains", "");
  EVALUATE_TRACE_PRE(4, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/2");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/0");
  EVALUATE_TRACE_POST_SUCCESS(1, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/1");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(2, "/contains", "#/contains", "", 1);
  EVALUATE_TRACE_POST_FAILURE(3, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/2");
  EVALUATE_TRACE_POST_SUCCESS(4, LoopContains, "/contains", "#/contains", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 2, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4,
      "A certain number of array items must satisfy the given constraints");
}

TEST(JSONSchema_compile_2020_12, contains_3) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "contains": { "type": "string" }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ 1, 2, 3 ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 4);

  EVALUATE_TRACE_PRE(0, LoopContains, "/contains", "#/contains", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/0");
  EVALUATE_TRACE_PRE(2, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/1");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/2");

  EVALUATE_TRACE_POST_FAILURE(0, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/0");
  EVALUATE_TRACE_POST_FAILURE(1, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/1");
  EVALUATE_TRACE_POST_FAILURE(2, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/2");
  EVALUATE_TRACE_POST_FAILURE(3, LoopContains, "/contains", "#/contains", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 1, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 3,
      "A certain number of array items must satisfy the given constraints");
}

TEST(JSONSchema_compile_2020_12, contains_4) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "contains": { "type": "string" }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", \"bar\", \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 7);

  EVALUATE_TRACE_PRE(0, LoopContains, "/contains", "#/contains", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/0");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(2, "/contains", "#/contains", "");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/1");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(4, "/contains", "#/contains", "");
  EVALUATE_TRACE_PRE(5, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/2");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(6, "/contains", "#/contains", "");

  // TODO: We should be emitting "true" by the end of this as all items matched

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/0");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(1, "/contains", "#/contains", "", 0);
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/1");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(3, "/contains", "#/contains", "", 1);
  EVALUATE_TRACE_POST_SUCCESS(4, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/2");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(5, "/contains", "#/contains", "", 2);
  EVALUATE_TRACE_POST_SUCCESS(6, LoopContains, "/contains", "#/contains", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 5, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 6,
      "A certain number of array items must satisfy the given constraints");
}

TEST(JSONSchema_compile_2020_12, contains_5) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "maxContains": 1,
    "contains": { "type": "string" }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{
      sourcemeta::jsontoolkit::parse("[ \"foo\", \"bar\", \"baz\" ]")};
  EVALUATE_WITH_TRACE_FAST_FAILURE(compiled_schema, instance, 5);

  EVALUATE_TRACE_PRE(0, LoopContains, "/contains", "#/contains", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/0");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(2, "/contains", "#/contains", "");
  EVALUATE_TRACE_PRE(3, AssertionTypeStrict, "/contains/type",
                     "#/contains/type", "/1");
  EVALUATE_TRACE_PRE_ANNOTATION_PUBLIC(4, "/contains", "#/contains", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/0");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(1, "/contains", "#/contains", "", 0);
  EVALUATE_TRACE_POST_SUCCESS(2, AssertionTypeStrict, "/contains/type",
                              "#/contains/type", "/1");
  EVALUATE_TRACE_POST_ANNOTATION_PUBLIC(3, "/contains", "#/contains", "", 1);
  EVALUATE_TRACE_POST_FAILURE(4, LoopContains, "/contains", "#/contains", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 2, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 3, "Emit an annotation");
  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 4,
      "A certain number of array items must satisfy the given constraints");
}

TEST(JSONSchema_compile_2020_12, reference_from_unknown_keyword) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "#/definitions/bar" },
      "baz": { "type": "string" }
    },
    "definitions": {
      "bar": {
        "$ref": "#/properties/baz"
      }
    }
  })JSON")};

  try {
    sourcemeta::jsontoolkit::compile(
        schema, sourcemeta::jsontoolkit::default_schema_walker,
        sourcemeta::jsontoolkit::official_resolver,
        sourcemeta::jsontoolkit::default_schema_compiler);
  } catch (const sourcemeta::jsontoolkit::SchemaReferenceError &error) {
    EXPECT_EQ(error.id(), "#/properties/baz");
    EXPECT_EQ(error.location(),
              sourcemeta::jsontoolkit::Pointer({"definitions", "bar", "$ref"}));
  } catch (...) {
    throw;
  }
}

TEST(JSONSchema_compile_2020_12, dynamicRef_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicRef": "#foo",
    "$defs": {
      "string": {
        "$dynamicAnchor": "foo",
        "type": "string"
      }
    }
  })JSON")};

  const auto compiled_schema{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};
  EVALUATE_WITH_TRACE_FAST_SUCCESS(compiled_schema, instance, 2);

  EVALUATE_TRACE_PRE(0, ControlDynamicAnchorJump, "/$dynamicRef",
                     "#/$dynamicRef", "");
  EVALUATE_TRACE_PRE(1, AssertionTypeStrict, "/$dynamicRef/type",
                     "#/$defs/string/type", "");

  EVALUATE_TRACE_POST_SUCCESS(0, AssertionTypeStrict, "/$dynamicRef/type",
                              "#/$defs/string/type", "");
  EVALUATE_TRACE_POST_SUCCESS(1, ControlDynamicAnchorJump, "/$dynamicRef",
                              "#/$dynamicRef", "");

  EVALUATE_TRACE_POST_DESCRIBE(
      instance, 0, "The target document is expected to be of the given type");
  EVALUATE_TRACE_POST_DESCRIBE(instance, 1, "Jump to a dynamic anchor");
}
