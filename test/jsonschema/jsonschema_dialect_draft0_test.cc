#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_dialect_draft0, jsonschema_draft_hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-00/hyper-schema#",
    "type": "object"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "http://json-schema.org/draft-00/hyper-schema#");
}

TEST(JSONSchema_dialect_draft0, jsonschema_draft_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-00/schema#",
    "type": "object"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "http://json-schema.org/draft-00/schema#");
}

TEST(JSONSchema_dialect_draft0, jsonschema_draft_jsonref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-00/json-ref#"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "http://json-schema.org/draft-00/json-ref#");
}

TEST(JSONSchema_dialect_draft0, jsonschema_draft_links) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-00/links#"
  })JSON");
  const std::optional<std::string> dialect{sourcemeta::core::dialect(document)};
  EXPECT_TRUE(dialect.has_value());
  EXPECT_EQ(dialect.value(), "http://json-schema.org/draft-00/links#");
}
