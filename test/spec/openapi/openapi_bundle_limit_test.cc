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

static auto throwing_openapi_resolver(std::string_view)
    -> sourcemeta::core::OpenAPIResolverResult {
  throw sourcemeta::core::OpenAPIFrameLimitError{6};
}

TEST(an_openapi_limit_error_from_the_resolver_keeps_its_own_allowance) {
  auto document{DESCRIPTION};
  try {
    sourcemeta::core::openapi_bundle(document, sourcemeta::core::schema_walker,
                                     sourcemeta::core::schema_resolver,
                                     throwing_openapi_resolver);
    FAIL();
  } catch (const sourcemeta::core::OpenAPIFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 6);
  }
}

// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
static const sourcemeta::core::JSON SCHEMA_DESCRIPTION =
    sourcemeta::core::parse_json(R"JSON({
  "openapi": "3.1.1",
  "info": { "title": "Example", "version": "1.0.0" },
  "paths": {},
  "components": {
    "schemas": {
      "Pet": {
        "$schema": "https://spec.openapis.org/oas/3.1/dialect/base",
        "$ref": "https://www.sourcemeta.com/pet"
      }
    }
  }
})JSON");

static const sourcemeta::core::JSON EMBEDDING_DESCRIPTION =
    sourcemeta::core::parse_json(R"JSON({
  "openapi": "3.1.1",
  "info": { "title": "Example", "version": "1.0.0" },
  "paths": {
    "/thing": {
      "get": {
        "responses": {
          "200": { "$ref": "https://www.sourcemeta.com/api#/components/responses/Found" }
        }
      }
    }
  }
})JSON");
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

static auto throwing_schema_resolver(std::string_view)
    -> sourcemeta::core::SchemaResolverResult {
  throw sourcemeta::core::SchemaFrameLimitError{7};
}

static auto throwing_openapi_schema_resolver(std::string_view)
    -> sourcemeta::core::SchemaResolverResult {
  throw sourcemeta::core::OpenAPIFrameLimitError{8};
}

static auto nothing_resolver(std::string_view)
    -> sourcemeta::core::OpenAPIResolverResult {
  return std::nullopt;
}

static auto embedding_resolver(const std::string_view identifier)
    -> sourcemeta::core::OpenAPIResolverResult {
  if (identifier != "https://www.sourcemeta.com/api") {
    return std::nullopt;
  }

  return sourcemeta::core::parse_json(R"JSON({
    "openapi": "3.1.1",
    "info": { "title": "Other", "version": "1.0.0" },
    "components": {
      "responses": { "Found": { "description": "Found" } }
    }
  })JSON");
}

TEST(a_limit_error_from_the_schema_resolver_keeps_its_own_allowance) {
  auto document{SCHEMA_DESCRIPTION};
  try {
    sourcemeta::core::openapi_bundle(document, sourcemeta::core::schema_walker,
                                     throwing_schema_resolver,
                                     nothing_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 7);
  }
}

TEST(an_openapi_limit_error_from_the_schema_resolver_keeps_its_own_allowance) {
  auto document{SCHEMA_DESCRIPTION};
  try {
    sourcemeta::core::openapi_bundle(document, sourcemeta::core::schema_walker,
                                     throwing_openapi_schema_resolver,
                                     nothing_resolver);
    FAIL();
  } catch (const sourcemeta::core::OpenAPIFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 8);
  }
}

TEST(a_limit_error_from_the_callback_keeps_its_own_allowance) {
  auto document{EMBEDDING_DESCRIPTION};
  sourcemeta::core::OpenAPIBundleOptions options;
  options.callback = [](const sourcemeta::core::JSON::StringView,
                        const sourcemeta::core::Pointer &) -> void {
    throw sourcemeta::core::SchemaFrameLimitError{9};
  };

  try {
    sourcemeta::core::openapi_bundle(document, sourcemeta::core::schema_walker,
                                     sourcemeta::core::schema_resolver,
                                     embedding_resolver, options);
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 9);
  }
}

TEST(an_openapi_limit_error_from_the_callback_keeps_its_own_allowance) {
  auto document{EMBEDDING_DESCRIPTION};
  sourcemeta::core::OpenAPIBundleOptions options;
  options.callback = [](const sourcemeta::core::JSON::StringView,
                        const sourcemeta::core::Pointer &) -> void {
    throw sourcemeta::core::OpenAPIFrameLimitError{10};
  };

  try {
    sourcemeta::core::openapi_bundle(document, sourcemeta::core::schema_walker,
                                     sourcemeta::core::schema_resolver,
                                     embedding_resolver, options);
    FAIL();
  } catch (const sourcemeta::core::OpenAPIFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 10);
  }
}

TEST(a_limit_error_from_the_namer_keeps_its_own_allowance) {
  auto document{EMBEDDING_DESCRIPTION};
  sourcemeta::core::OpenAPIBundleOptions options;
  options.namer = [](const sourcemeta::core::JSON::StringView,
                     const sourcemeta::core::JSON::StringView)
      -> sourcemeta::core::JSON::String {
    throw sourcemeta::core::SchemaFrameLimitError{11};
  };

  try {
    sourcemeta::core::openapi_bundle(document, sourcemeta::core::schema_walker,
                                     sourcemeta::core::schema_resolver,
                                     embedding_resolver, options);
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 11);
  }
}

TEST(an_openapi_limit_error_from_the_namer_keeps_its_own_allowance) {
  auto document{EMBEDDING_DESCRIPTION};
  sourcemeta::core::OpenAPIBundleOptions options;
  options.namer = [](const sourcemeta::core::JSON::StringView,
                     const sourcemeta::core::JSON::StringView)
      -> sourcemeta::core::JSON::String {
    throw sourcemeta::core::OpenAPIFrameLimitError{12};
  };

  try {
    sourcemeta::core::openapi_bundle(document, sourcemeta::core::schema_walker,
                                     sourcemeta::core::schema_resolver,
                                     embedding_resolver, options);
    FAIL();
  } catch (const sourcemeta::core::OpenAPIFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 12);
  }
}
