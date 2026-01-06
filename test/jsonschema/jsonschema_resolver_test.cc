#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uri.h>

#define EXPECT_SCHEMA(identifier)                                              \
  {                                                                            \
    const std::optional<sourcemeta::core::JSON> result{                        \
        sourcemeta::core::schema_resolver(identifier)};                        \
    EXPECT_TRUE(result.has_value());                                           \
    const sourcemeta::core::JSON &document{result.value()};                    \
    EXPECT_TRUE(sourcemeta::core::is_schema(document));                        \
    const auto id{sourcemeta::core::identify(                                  \
        document, sourcemeta::core::schema_resolver)};                         \
    EXPECT_EQ(                                                                 \
        sourcemeta::core::URI{std::string{id}}.canonicalize().recompose(),     \
        sourcemeta::core::URI{identifier}.canonicalize().recompose());         \
  }

TEST(JSONSchema_resolver, jsonschema_2020_12) {
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/applicator");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/content");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/core");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/format-annotation");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/format-assertion");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/hyper-schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/meta-data");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/unevaluated");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/meta/validation");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/links");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/output/schema");

  // Just because a lot of people get this wrong
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema#");
}

TEST(JSONSchema_resolver, jsonschema_2019_09) {
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/applicator");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/content");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/core");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/format");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/hyper-schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/meta-data");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/meta/validation");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/output/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/links");
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/output/hyper-schema");

  // Just because a lot of people get this wrong
  EXPECT_SCHEMA("https://json-schema.org/draft/2019-09/schema#");
}

TEST(JSONSchema_resolver, jsonschema_draft7) {
  EXPECT_SCHEMA("http://json-schema.org/draft-07/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/hyper-schema-output");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-07/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/links");
}

TEST(JSONSchema_resolver, jsonschema_draft6) {
  EXPECT_SCHEMA("http://json-schema.org/draft-06/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/links#");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-06/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/links");
}

TEST(JSONSchema_resolver, jsonschema_draft4) {
  EXPECT_SCHEMA("http://json-schema.org/draft-04/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/links#");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-04/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/links");
}

TEST(JSONSchema_resolver, jsonschema_draft3) {
  EXPECT_SCHEMA("http://json-schema.org/draft-03/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/json-ref#");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-03/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/links");
  EXPECT_SCHEMA("http://json-schema.org/draft-03/json-ref");
}

TEST(JSONSchema_resolver, jsonschema_draft2) {
  EXPECT_SCHEMA("http://json-schema.org/draft-02/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/json-ref#");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-02/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/links");
  EXPECT_SCHEMA("http://json-schema.org/draft-02/json-ref");
}

TEST(JSONSchema_resolver, jsonschema_draft1) {
  EXPECT_SCHEMA("http://json-schema.org/draft-01/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/json-ref#");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-01/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/links");
  EXPECT_SCHEMA("http://json-schema.org/draft-01/json-ref");
}

TEST(JSONSchema_resolver, jsonschema_draft0) {
  EXPECT_SCHEMA("http://json-schema.org/draft-00/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/json-ref#");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-00/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/links");
  EXPECT_SCHEMA("http://json-schema.org/draft-00/json-ref");
}

TEST(JSONSchema_resolver, openapi_3_1) {
  EXPECT_SCHEMA("https://spec.openapis.org/oas/3.1/dialect/base");
  EXPECT_SCHEMA("https://spec.openapis.org/oas/3.1/meta/base");
}

TEST(JSONSchema_resolver, openapi_3_2) {
  EXPECT_SCHEMA("https://spec.openapis.org/oas/3.2/dialect/2025-09-17");
  EXPECT_SCHEMA("https://spec.openapis.org/oas/3.2/meta/2025-09-17");
}

TEST(JSONSchema_resolver, idempotency) {
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_resolver, invalid) {
  const std::optional<sourcemeta::core::JSON> result{
      sourcemeta::core::schema_resolver("https://example.com/foobar")};
  EXPECT_FALSE(result.has_value());
}
