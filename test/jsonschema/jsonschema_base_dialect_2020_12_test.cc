#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_base_dialect_2020_12, jsonschema_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/hyper-schema",
    "type": "object"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/hyper-schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_links) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/links"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_output) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/output/schema"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_applicator) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/applicator"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_content) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/content"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_core) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/core"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_format_annotation) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/format-annotation"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_format_assertion) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/format-assertion"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/hyper-schema"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/hyper-schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_meta_data) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/meta-data"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_unevaluated) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/unevaluated"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect_2020_12, jsonschema_meta_validation) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/meta/validation"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "https://json-schema.org/draft/2020-12/schema");
}
