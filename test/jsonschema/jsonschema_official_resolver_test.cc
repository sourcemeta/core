#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uri.h>

#define EXPECT_SCHEMA(identifier)                                              \
  {                                                                            \
    const std::optional<sourcemeta::core::JSON> result{                        \
        sourcemeta::core::schema_official_resolver(identifier)};               \
    EXPECT_TRUE(result.has_value());                                           \
    const sourcemeta::core::JSON &document{result.value()};                    \
    EXPECT_TRUE(sourcemeta::core::is_schema(document));                        \
    std::optional<std::string> id{sourcemeta::core::identify(                  \
        document, sourcemeta::core::schema_official_resolver)};                \
    EXPECT_TRUE(id.has_value());                                               \
    EXPECT_EQ(sourcemeta::core::URI{id.value()}.canonicalize().recompose(),    \
              sourcemeta::core::URI{identifier}.canonicalize().recompose());   \
  }

TEST(JSONSchema_official_resolver, jsonschema_2020_12) {
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
}

TEST(JSONSchema_official_resolver, jsonschema_2019_09) {
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
}

TEST(JSONSchema_official_resolver, jsonschema_draft7) {
  EXPECT_SCHEMA("http://json-schema.org/draft-07/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/links#");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/hyper-schema-output");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-07/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-07/links");
}

TEST(JSONSchema_official_resolver, jsonschema_draft6) {
  EXPECT_SCHEMA("http://json-schema.org/draft-06/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/links#");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-06/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-06/links");
}

TEST(JSONSchema_official_resolver, jsonschema_draft4) {
  EXPECT_SCHEMA("http://json-schema.org/draft-04/schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/hyper-schema#");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/links#");

  // Take canonicalized versions too
  EXPECT_SCHEMA("http://json-schema.org/draft-04/schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/hyper-schema");
  EXPECT_SCHEMA("http://json-schema.org/draft-04/links");
}

TEST(JSONSchema_official_resolver, jsonschema_draft3) {
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

TEST(JSONSchema_official_resolver, jsonschema_draft2) {
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

TEST(JSONSchema_official_resolver, jsonschema_draft1) {
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

TEST(JSONSchema_official_resolver, jsonschema_draft0) {
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

TEST(JSONSchema_official_resolver, idempotency) {
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
  EXPECT_SCHEMA("https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_official_resolver, invalid) {
  const std::optional<sourcemeta::core::JSON> result{
      sourcemeta::core::schema_official_resolver("https://example.com/foobar")};
  EXPECT_FALSE(result.has_value());
}
