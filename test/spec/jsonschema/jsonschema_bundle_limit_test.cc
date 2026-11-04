#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <string_view>

static auto throwing_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/test-1") {
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
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/test-1" }
  })JSON");

  try {
    sourcemeta::core::schema_bundle(document, sourcemeta::core::schema_walker,
                                    throwing_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 5);
  }
}

TEST(a_limit_error_from_the_callback_keeps_its_own_allowance) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/leaf" }
  })JSON");

  sourcemeta::core::SchemaBundleOptions options;
  options.callback = [](std::string_view,
                        const sourcemeta::core::WeakPointer &) -> void {
    throw sourcemeta::core::SchemaFrameLimitError{7};
  };

  try {
    sourcemeta::core::schema_bundle(document, sourcemeta::core::schema_walker,
                                    leaf_resolver, "", "", options);
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 7);
  }
}
