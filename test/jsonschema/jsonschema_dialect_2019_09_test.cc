#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_dialect_2019_09, jsonschema_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "object"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/hyper-schema",
    "type": "object"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/hyper-schema");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_links) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/links"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "https://json-schema.org/draft/2019-09/links");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_output) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/output/schema"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/output/schema");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_output_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/output/hyper-schema"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/output/hyper-schema");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_meta_applicator) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/meta/applicator"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/meta/applicator");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_meta_content) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/meta/content"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/meta/content");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_meta_core) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/meta/core"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "https://json-schema.org/draft/2019-09/meta/core");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_meta_format) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/meta/format"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/meta/format");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_meta_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/meta/hyper-schema"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/meta/hyper-schema");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_meta_meta_data) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/meta/meta-data"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/meta/meta-data");
}

TEST(JSONSchema_dialect_2019_09, jsonschema_meta_validation) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/meta/validation"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(),
            "https://json-schema.org/draft/2019-09/meta/validation");
}
