#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <string_view>

// The fixture suite cannot express a resolver of the caller's that throws, and
// what these pin is that such an error arrives as it was thrown. This operation
// reaches for the resolver of its own accord, inside handlers that translate
// failures of its own making, so an error of the caller's that spells the same
// as one of those is the case that tells them apart
// Refusing to answer for the dialect that a remote declares is what puts the
// error inside the lookup that decides whether what was just fetched can be
// read
static auto resolution_throwing_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/remote") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://www.sourcemeta.com/custom-dialect",
      "$id": "https://www.sourcemeta.com/remote",
      "type": "string"
    })JSON");
  }

  if (identifier == "https://www.sourcemeta.com/custom-dialect") {
    throw sourcemeta::core::SchemaResolutionError(
        "The caller could not reach this one",
        "https://www.sourcemeta.com/custom-dialect");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

static auto base_dialect_throwing_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/remote") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://www.sourcemeta.com/custom-dialect",
      "$id": "https://www.sourcemeta.com/remote",
      "type": "string"
    })JSON");
  }

  if (identifier == "https://www.sourcemeta.com/custom-dialect") {
    throw sourcemeta::core::SchemaUnknownBaseDialectError();
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(a_resolution_error_of_the_callers_own_is_unchanged) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "items": { "$ref": "https://www.sourcemeta.com/remote" }
  })JSON")};

  try {
    sourcemeta::core::schema_bundle(document, sourcemeta::core::schema_walker,
                                    resolution_throwing_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(), "The caller could not reach this one");
    EXPECT_EQ(error.identifier(), "https://www.sourcemeta.com/custom-dialect");
  }
}

TEST(a_base_dialect_error_of_the_callers_own_is_unchanged) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "items": { "$ref": "https://www.sourcemeta.com/remote" }
  })JSON")};

  try {
    sourcemeta::core::schema_bundle(document, sourcemeta::core::schema_walker,
                                    base_dialect_throwing_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not determine the base dialect of the schema");
  }
}
