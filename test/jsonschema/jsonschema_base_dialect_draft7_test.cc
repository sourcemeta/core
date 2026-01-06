#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_base_dialect_draft7, jsonschema_draft_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/hyper-schema#",
    "type": "object"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "http://json-schema.org/draft-07/hyper-schema#");
}

TEST(JSONSchema_base_dialect_draft7, jsonschema_draft_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "type": "object"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_base_dialect_draft7, jsonschema_draft_links) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/links#"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "http://json-schema.org/draft-07/hyper-schema#");
}

TEST(JSONSchema_base_dialect_draft7, jsonschema_draft_hyperschema_output) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/hyper-schema-output"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(base_dialect, "http://json-schema.org/draft-07/schema#");
}
