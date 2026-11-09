#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/openapi.h>

#include <string_view>

// The fixture suite answers from a table and reports through a callback of its
// own, so what it cannot express is a resolver or a callback of the caller's
// that runs an allowance of its own and reports running out of it in the same
// terms this walk uses

static auto throwing_schema_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/dialect.json") {
    throw sourcemeta::core::SchemaFrameLimitError{7};
  }

  return sourcemeta::core::schema_resolver(identifier);
}

static auto no_descriptions(std::string_view)
    -> sourcemeta::core::OpenAPIResolverResult {
  return std::nullopt;
}

TEST(a_limit_error_from_the_resolver_keeps_its_own_allowance) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "jsonSchemaDialect": "https://www.sourcemeta.com/dialect.json",
    "components": { "schemas": { "Pet": { "type": "object" } } }
  })JSON");

  sourcemeta::core::OpenAPIDependencyOptions options;
  options.max_locations = 100;

  try {
    [[maybe_unused]] const auto locations{
        sourcemeta::core::openapi_dependencies(
            document, sourcemeta::core::schema_walker, throwing_schema_resolver,
            no_descriptions, [](const auto &) -> void {}, options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 7);
  }
}

static auto chain_schema_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/a.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/a.json",
      "$ref": "https://www.sourcemeta.com/b.json"
    })JSON");
  }

  if (identifier == "https://www.sourcemeta.com/b.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/b.json",
      "type": "object"
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(a_limit_error_from_the_callback_keeps_its_own_allowance) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Example", "version": "1.0.0" },
    "components": { "schemas": { "Pet": {
      "$ref": "https://www.sourcemeta.com/a.json" } } }
  })JSON");

  sourcemeta::core::OpenAPIDependencyOptions options;
  options.max_locations = 1000;

  try {
    [[maybe_unused]] const auto locations{
        sourcemeta::core::openapi_dependencies(
            document, sourcemeta::core::schema_walker, chain_schema_resolver,
            no_descriptions,
            [](const auto &dependency) -> void {
              if (dependency.destination ==
                  "https://www.sourcemeta.com/b.json") {
                throw sourcemeta::core::SchemaFrameLimitError{7};
              }
            },
            options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 7);
  }
}
