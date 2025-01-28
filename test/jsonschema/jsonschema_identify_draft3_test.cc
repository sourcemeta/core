#include <gtest/gtest.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://sourcemeta.com/metaschema") {
    return sourcemeta::core::parse(R"JSON({
      "id": "https://sourcemeta.com/metaschema",
      "$schema": "http://json-schema.org/draft-03/schema#"
    })JSON");
  } else {
    return sourcemeta::core::official_resolver(identifier);
  }
}

TEST(JSONSchema_identify_draft3, valid_one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema",
    "$schema": "https://sourcemeta.com/metaschema"
  })JSON");
  std::optional<std::string> id{
      sourcemeta::core::identify(document, test_resolver)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify_draft3, new_one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://sourcemeta.com/metaschema"
  })JSON");
  std::optional<std::string> id{
      sourcemeta::core::identify(document, test_resolver)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify_draft3, id_boolean_default_dialect) {
  const sourcemeta::core::JSON document{true};
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::official_resolver,
      sourcemeta::core::IdentificationStrategy::Strict,
      "http://json-schema.org/draft-03/schema#")};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify_draft3, empty_object_default_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse("{}");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::official_resolver,
      sourcemeta::core::IdentificationStrategy::Strict,
      "http://json-schema.org/draft-03/schema#")};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify_draft3, valid_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::official_resolver)};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify_draft3, new_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::official_resolver)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify_draft3, default_dialect_precedence) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");
  std::optional<std::string> id{sourcemeta::core::identify(
      document, sourcemeta::core::official_resolver,
      sourcemeta::core::IdentificationStrategy::Strict,
      "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify_draft3, base_dialect_shortcut) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");
  const std::optional<std::string> id{sourcemeta::core::identify(
      document, "http://json-schema.org/draft-03/schema#")};
  EXPECT_TRUE(id.has_value());
  EXPECT_EQ(id.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_identify_draft3, anonymize_with_base_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::official_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  sourcemeta::core::anonymize(document, base_dialect.value());

  const sourcemeta::core::JSON expected = sourcemeta::core::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_identify_draft3, anonymize_with_base_dialect_no_id) {
  sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::official_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  sourcemeta::core::anonymize(document, base_dialect.value());

  const sourcemeta::core::JSON expected = sourcemeta::core::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_identify_draft3, sibling_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "#"
  })JSON");
  std::optional<std::string> id{
      sourcemeta::core::identify(document, test_resolver)};
  EXPECT_FALSE(id.has_value());
}

TEST(JSONSchema_identify_draft3, reidentify_replace) {
  sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  sourcemeta::core::reidentify(document, "https://example.com/my-new-id",
                               sourcemeta::core::official_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-new-id",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_identify_draft3, reidentify_set) {
  sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  sourcemeta::core::reidentify(document, "https://example.com/my-new-id",
                               sourcemeta::core::official_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-new-id",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_identify_draft3, reidentify_replace_default_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema"
  })JSON");

  sourcemeta::core::reidentify(document, "https://example.com/my-new-id",
                               sourcemeta::core::official_resolver,
                               "http://json-schema.org/draft-03/schema#");

  const sourcemeta::core::JSON expected = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-new-id"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_identify_draft3, reidentify_replace_base_dialect_shortcut) {
  sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-schema",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::official_resolver)};
  EXPECT_TRUE(base_dialect.has_value());

  sourcemeta::core::reidentify(document, "https://example.com/my-new-id",
                               base_dialect.value());

  const sourcemeta::core::JSON expected = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-new-id",
    "$schema": "http://json-schema.org/draft-03/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_identify_draft3, reidentify_set_with_top_level_ref) {
  sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "https://example.com/schema"
  })JSON");

  sourcemeta::core::reidentify(document, "https://example.com/my-new-id",
                               sourcemeta::core::official_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-new-id",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "extends": { "$ref": "https://example.com/schema" }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_identify_draft3,
     reidentify_set_with_top_level_ref_and_extends) {
  sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
    "$ref": "https://example.com/schema",
    "extends": { "type": "string" }
  })JSON");

  sourcemeta::core::reidentify(document, "https://example.com/my-new-id",
                               sourcemeta::core::official_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse(R"JSON({
    "id": "https://example.com/my-new-id",
    "$schema": "http://json-schema.org/draft-03/schema#",
    "extends": { "$ref": "https://example.com/schema" }
  })JSON");

  EXPECT_EQ(document, expected);
}
