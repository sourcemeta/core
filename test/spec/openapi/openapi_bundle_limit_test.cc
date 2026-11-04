#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/openapi.h>

#include <string_view>

// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
static const sourcemeta::core::JSON DESCRIPTION =
    sourcemeta::core::parse_json(R"JSON({
  "openapi": "3.1.1",
  "info": { "title": "Example", "version": "1.0.0" },
  "paths": {
    "/thing": {
      "get": {
        "responses": {
          "200": { "$ref": "https://www.sourcemeta.com/response" }
        }
      }
    }
  }
})JSON");
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

static auto throwing_resolver(std::string_view)
    -> sourcemeta::core::OpenAPIResolverResult {
  throw sourcemeta::core::SchemaFrameLimitError{5};
}

TEST(a_limit_error_from_the_resolver_keeps_its_own_allowance) {
  auto document{DESCRIPTION};
  try {
    sourcemeta::core::openapi_bundle(document, sourcemeta::core::schema_walker,
                                     sourcemeta::core::schema_resolver,
                                     throwing_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 5);
  }
}
