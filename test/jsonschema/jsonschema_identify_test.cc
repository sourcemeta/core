#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_identify, boolean_no_dialect) {
  const sourcemeta::core::JSON document{true};
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, boolean_no_dialect_with_default_id) {
  const sourcemeta::core::JSON document{true};
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Strict, std::nullopt,
      "https://www.sourcemeta.com/foo")};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://www.sourcemeta.com/foo");
}

TEST(JSONSchema_identify, empty_old_no_dollar_sign_id_with_default) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Strict,
      "http://json-schema.org/draft-00/schema#",
      "https://example.com/my-schema")};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify, empty_dollar_sign_id_with_default) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Strict,
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/my-schema")};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify, boolean_unknown_dialect) {
  const sourcemeta::core::JSON document{true};
  EXPECT_THROW(sourcemeta::core::identify(
                   document, sourcemeta::core::schema_official_resolver,
                   sourcemeta::core::SchemaIdentificationStrategy::Strict,
                   "https://www.sourcemeta.com/invalid-dialect"),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_identify, empty_object_no_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, empty_object_unknown_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  EXPECT_THROW(sourcemeta::core::identify(
                   document, sourcemeta::core::schema_official_resolver,
                   sourcemeta::core::SchemaIdentificationStrategy::Strict,
                   "https://www.sourcemeta.com/invalid-dialect"),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_identify, object_with_dollar_id_with_no_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, object_with_id_with_no_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://example.com/my-schema"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, loose_boolean) {
  const sourcemeta::core::JSON document{true};
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, loose_with_valid_dollar_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify, loose_with_invalid_dollar_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": false
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, loose_with_valid_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://example.com/my-schema"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify, loose_with_invalid_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": false
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, loose_with_valid_dollar_id_and_invalid_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "id": false
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify, loose_with_valid_id_and_invalid_dollar_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://example.com/my-schema",
    "$id": false
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify, loose_with_invalid_id_and_invalid_dollar_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": 1,
    "id": false
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, loose_with_matching_id_and_dollar_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "id": "https://example.com/my-schema"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify, loose_with_non_matching_id_and_dollar_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "http://example.com/my-schema",
    "id": "https://example.com/my-schema"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, loose_with_resolvable_default_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "http://example.com/my-schema",
    "id": "https://example.com/my-schema"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose,
      "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "http://example.com/my-schema");
}

TEST(JSONSchema_identify, strict_draft4_top_level_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "http://example.com/my-schema",
    "$ref": "#/definitions/foo",
    "definitions": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Strict)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify, loose_draft4_top_level_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "http://example.com/my-schema",
    "$ref": "#/definitions/foo",
    "definitions": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "http://example.com/my-schema");
}

TEST(JSONSchema_identify, loose_with_unresolvable_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://www.sourcemeta.com/invalid-dialect"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::schema_official_resolver,
      sourcemeta::core::SchemaIdentificationStrategy::Loose)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify, anonymize_with_unknown_base_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_THROW(sourcemeta::core::anonymize(
                   document, "https://www.sourcemeta.com/invalid-base-dialect"),
               sourcemeta::core::SchemaError);
}

TEST(JSONSchema_identify, reidentify_boolean) {
  sourcemeta::core::JSON document{true};
  EXPECT_THROW(
      sourcemeta::core::reidentify(document, "https://example.com/my-new-id",
                                   sourcemeta::core::schema_official_resolver),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}
