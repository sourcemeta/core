#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <string_view>

// The fixture suite pins what the walk's own allowance reports. What it cannot
// express is a resolver or a callback of the caller's that throws, which is
// what these cover
// Framing asks for the dialect a document declares, so throwing for that is
// what puts the error inside the construction this operation bounds
static auto throwing_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/dialect") {
    throw sourcemeta::core::SchemaFrameLimitError{5};
  }

  return sourcemeta::core::schema_resolver(identifier);
}

static auto leaf_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/leaf") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/leaf",
      "type": "string"
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(a_limit_error_from_the_resolver_keeps_its_own_allowance) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://www.sourcemeta.com/dialect",
    "type": "string"
  })JSON");

  try {
    sourcemeta::core::schema_dependencies(
        document, sourcemeta::core::schema_walker, throwing_resolver,
        [](const auto &) -> void {});
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 5);
  }
}

TEST(a_limit_error_from_the_callback_keeps_its_own_allowance) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/leaf" }
  })JSON");

  try {
    sourcemeta::core::schema_dependencies(
        document, sourcemeta::core::schema_walker, leaf_resolver,
        [](const auto &) -> void {
          throw sourcemeta::core::SchemaFrameLimitError{7};
        });
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 7);
  }
}
