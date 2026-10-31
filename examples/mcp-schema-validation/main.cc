#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/test.h>

#include <functional>  // std::function
#include <stdexcept>   // std::invalid_argument
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {
// This application chooses one dialect explicitly. It neither relabels an old
// schema nor silently guesses a dialect. Schema authoring and approval are
// trusted setup steps; compilation is not a schema security sandbox.
auto compile(const sourcemeta::core::JSON &schema)
    -> sourcemeta::blaze::Template {
  if (!schema.is_object() || !schema.defines("$schema") ||
      schema.at("$schema") !=
          sourcemeta::core::JSON{
              "https://json-schema.org/draft/2020-12/schema"}) {
    throw std::invalid_argument{"This example requires explicit Draft 2020-12"};
  }
  return sourcemeta::blaze::compile(
      schema, sourcemeta::core::schema_walker,
      [](const std::string_view identifier)
          -> sourcemeta::core::SchemaResolverResult {
        if (identifier == "https://example.com/quantity") {
          return sourcemeta::core::parse_json(R"JSON({
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$id": "https://example.com/quantity",
            "type": "integer",
            "minimum": 1
          })JSON");
        }
        // Core resolves built-in schemas only. An unknown identifier stays
        // unresolved: do not add HTTP requests or filesystem fallback here.
        return sourcemeta::core::schema_resolver(identifier);
      },
      sourcemeta::blaze::default_schema_compiler,
      sourcemeta::blaze::Mode::FastValidation);
}

// Application-owned dispatch after transport validation and authorization.
// Invalid input never reaches the callback. Invalid output is not published as
// success, but this cannot undo an action the callback already performed.
// A null output schema means this tool declares no output contract, not that
// its result has been independently established correct.
auto call(
    const sourcemeta::blaze::Template &input_schema,
    const sourcemeta::blaze::Template *output_schema,
    const sourcemeta::core::JSON &arguments,
    const std::function<sourcemeta::core::JSON(const sourcemeta::core::JSON &)>
        &execute) -> sourcemeta::core::JSON {
  constexpr auto VERSION{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  const sourcemeta::core::JSON identifier{1};
  sourcemeta::blaze::Evaluator evaluator;
  if (!evaluator.validate(input_schema, arguments)) {
    return sourcemeta::core::mcp_make_tool_error(
        VERSION, identifier, "Input schema rejected arguments");
  }
  auto output{execute(arguments)};
  if (output_schema != nullptr && !evaluator.validate(*output_schema, output)) {
    return sourcemeta::core::mcp_make_tool_error(
        VERSION, identifier, "Output schema rejected result");
  }
  return sourcemeta::core::mcp_make_tool_success(VERSION, identifier,
                                                 std::move(output));
}
} // namespace

TEST(valid_arguments_execute_once_and_validate_structured_output) {
  const auto input{compile(sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "properties": { "quantity": { "$ref": "https://example.com/quantity" } },
    "required": [ "quantity" ],
    "additionalProperties": false
  })JSON"))};
  const auto output{compile(sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "properties": { "accepted": { "type": "integer" } },
    "required": [ "accepted" ],
    "additionalProperties": false
  })JSON"))};
  int executions{0};
  const auto result{
      call(input, &output,
           sourcemeta::core::parse_json(R"JSON({ "quantity": 2 })JSON"),
           [&executions](const sourcemeta::core::JSON &arguments) {
             ++executions;
             auto response{sourcemeta::core::JSON::make_object()};
             response.assign("accepted", arguments.at("quantity"));
             return response;
           })};
  EXPECT_EQ(executions, 1);
  EXPECT_FALSE(result.at("result").at("isError").to_boolean());
  EXPECT_EQ(result.at("result").at("structuredContent").at("accepted"),
            sourcemeta::core::JSON{2});
}

TEST(referenced_input_constraint_rejects_before_execution) {
  const auto input{compile(sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "properties": { "quantity": { "$ref": "https://example.com/quantity" } },
    "required": [ "quantity" ],
    "additionalProperties": false
  })JSON"))};
  int executions{0};
  const auto result{
      call(input, nullptr,
           sourcemeta::core::parse_json(R"JSON({ "quantity": 0 })JSON"),
           [&executions](const sourcemeta::core::JSON &) {
             ++executions;
             return sourcemeta::core::JSON::make_object();
           })};
  EXPECT_EQ(executions, 0);
  EXPECT_TRUE(result.at("result").at("isError").to_boolean());
}

TEST(invalid_output_is_not_published_as_success) {
  const auto input{compile(sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object"
  })JSON"))};
  const auto output{compile(sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "properties": { "accepted": { "type": "integer" } },
    "required": [ "accepted" ]
  })JSON"))};
  int executions{0};
  const auto result{call(input, &output, sourcemeta::core::JSON::make_object(),
                         [&executions](const sourcemeta::core::JSON &) {
                           ++executions;
                           return sourcemeta::core::parse_json(R"JSON({
                             "accepted": "wrong type"
                           })JSON");
                         })};
  EXPECT_EQ(executions, 1);
  EXPECT_TRUE(result.at("result").at("isError").to_boolean());
  EXPECT_FALSE(result.at("result").defines("structuredContent"));
}

TEST(no_declared_output_schema_still_checks_input) {
  const auto input{compile(sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object"
  })JSON"))};
  int executions{0};
  const auto result{call(input, nullptr, sourcemeta::core::JSON::make_object(),
                         [&executions](const sourcemeta::core::JSON &) {
                           ++executions;
                           return sourcemeta::core::JSON::make_object();
                         })};
  EXPECT_EQ(executions, 1);
  EXPECT_FALSE(result.at("result").at("isError").to_boolean());
}

TEST(envelope_builder_does_not_evaluate_an_application_schema) {
  const auto result{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "accepted": "wrong type" })JSON"))};
  EXPECT_FALSE(result.at("result").at("isError").to_boolean());
}

TEST(unsupported_dialect_is_not_silently_relabelled) {
  try {
    [[maybe_unused]] const auto compiled{
        compile(sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-07/schema#",
      "type": "object"
    })JSON"))};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "This example requires explicit Draft 2020-12");
  }
}

TEST(missing_dialect_is_not_silently_guessed) {
  try {
    [[maybe_unused]] const auto compiled{
        compile(sourcemeta::core::parse_json(R"JSON({
      "type": "object"
    })JSON"))};
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "This example requires explicit Draft 2020-12");
  }
}

TEST(unlisted_reference_fails_without_network_or_file_access) {
  try {
    [[maybe_unused]] const auto compiled{
        compile(sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "type": "object",
      "properties": { "quantity": { "$ref": "https://unlisted.invalid/schema" } }
    })JSON"))};
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://unlisted.invalid/schema");
  }
}
