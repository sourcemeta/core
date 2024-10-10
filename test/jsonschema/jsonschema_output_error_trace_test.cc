#include <gtest/gtest.h>

#include <sourcemeta/jsontoolkit/evaluator.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

#define EXPECT_OUTPUT(traces, index, expected_instance_location,               \
                      expected_evaluate_path, expected_message)                \
  EXPECT_TRUE(traces.size() > index);                                          \
  EXPECT_EQ(traces.at((index)).message, (expected_message));                   \
  EXPECT_EQ(sourcemeta::jsontoolkit::to_string(                                \
                traces.at((index)).instance_location),                         \
            expected_instance_location);                                       \
  EXPECT_EQ(                                                                   \
      sourcemeta::jsontoolkit::to_string(traces.at((index)).evaluate_path),    \
      expected_evaluate_path);

TEST(JSONSchema_output_error_trace, success_string_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string"
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};

  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_TRUE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};
  EXPECT_TRUE(traces.empty());
}

TEST(JSONSchema_output_error_trace, success_oneof_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "oneOf": [
      { "type": "string" },
      { "minLength": 3 }
    ]
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"fo"};

  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_TRUE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};
  EXPECT_TRUE(traces.empty());
}

TEST(JSONSchema_output_error_trace, fail_string) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string"
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};

  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_FALSE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};

  EXPECT_EQ(traces.size(), 1);
  EXPECT_OUTPUT(
      traces, 0, "", "/type",
      "The value was expected to be of type string but it was of type integer");
}

TEST(JSONSchema_output_error_trace, fail_string_over_ref) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/$defs/string",
    "$defs": {
      "string": {
        "type": "string"
      }
    }
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};

  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_FALSE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};

  EXPECT_EQ(traces.size(), 2);
  EXPECT_OUTPUT(
      traces, 0, "", "/$ref/type",
      "The value was expected to be of type string but it was of type integer");
  EXPECT_OUTPUT(traces, 1, "", "/$ref",
                "The integer value was expected to validate against the "
                "statically referenced schema");
}

TEST(JSONSchema_output_error_trace, fail_string_with_matching_base) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/$defs/string",
    "$defs": {
      "string": {
        "type": "string"
      }
    }
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};

  const std::string ref = "$ref";
  const sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(ref)};
  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance,
                                                                 pointer};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_FALSE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};

  EXPECT_EQ(traces.size(), 1);
  EXPECT_OUTPUT(
      traces, 0, "", "/type",
      "The value was expected to be of type string but it was of type integer");
}

TEST(JSONSchema_output_error_trace, fail_string_with_non_matching_base) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/$defs/string",
    "$defs": {
      "string": {
        "type": "string"
      }
    }
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{5};
  const std::string foo = "foo";
  const sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo)};
  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance,
                                                                 pointer};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_FALSE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};

  EXPECT_EQ(traces.size(), 2);
  EXPECT_OUTPUT(
      traces, 0, "", "/$ref/type",
      "The value was expected to be of type string but it was of type integer");
  EXPECT_OUTPUT(traces, 1, "", "/$ref",
                "The integer value was expected to validate against the "
                "statically referenced schema");
}

TEST(JSONSchema_output_error_trace, fail_oneof_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "oneOf": [
      { "type": "string" },
      { "minLength": 3 }
    ]
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};

  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_FALSE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};

  EXPECT_EQ(traces.size(), 1);
  EXPECT_OUTPUT(traces, 0, "", "/oneOf",
                "The string value was expected to validate against one and "
                "only one of the 2 given subschemas");
}

TEST(JSONSchema_output_error_trace, fail_not_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "not": {
      "type": "string"
    }
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{"foo"};

  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_FALSE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};

  EXPECT_EQ(traces.size(), 1);
  EXPECT_OUTPUT(traces, 0, "", "/not",
                "The string value was expected to not validate against the "
                "given subschema, but it did");
}

TEST(JSONSchema_output_error_trace, fail_not_not_1) {
  const sourcemeta::jsontoolkit::JSON schema{
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "not": {
      "not": {
        "type": "string"
      }
    }
  })JSON")};

  const auto schema_template{sourcemeta::jsontoolkit::compile(
      schema, sourcemeta::jsontoolkit::default_schema_walker,
      sourcemeta::jsontoolkit::official_resolver,
      sourcemeta::jsontoolkit::default_schema_compiler)};

  const sourcemeta::jsontoolkit::JSON instance{1};

  sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput output{instance};
  const auto result{sourcemeta::jsontoolkit::evaluate(schema_template, instance,
                                                      std::ref(output))};

  EXPECT_FALSE(result);
  std::vector<sourcemeta::jsontoolkit::SchemaCompilerErrorTraceOutput::Entry>
      traces{output.cbegin(), output.cend()};

  EXPECT_EQ(traces.size(), 1);
  EXPECT_OUTPUT(traces, 0, "", "/not",
                "The integer value was expected to not validate against the "
                "given subschema, but it did");
}
