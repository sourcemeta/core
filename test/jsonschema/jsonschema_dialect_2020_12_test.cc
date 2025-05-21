#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_dialect_2020_12, jsonschema_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/hyper-schema",
    "type": "object"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/hyper-schema");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_links) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/links"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "https://json-schema.org/draft/2020-12/links");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_output) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/output/schema"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/output/schema");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_applicator) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/applicator"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/meta/applicator");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_content) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/content"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/meta/content");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_core) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/core"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "https://json-schema.org/draft/2020-12/meta/core");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_format_annotation) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/format-annotation"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/meta/format-annotation");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_format_assertion) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/format-assertion"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/meta/format-assertion");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/hyper-schema"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/meta/hyper-schema");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_meta_data) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/meta-data"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/meta/meta-data");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_unevaluated) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/unevaluated"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/meta/unevaluated");
}

TEST(JSONSchema_dialect_2020_12, jsonschema_meta_validation) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/validation"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2020-12/meta/validation");
}
