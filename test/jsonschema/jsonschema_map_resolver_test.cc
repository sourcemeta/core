#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>

#include <unordered_set>

TEST(JSONSchema_SchemaMapResolver, empty_no_fallback) {
  sourcemeta::core::SchemaMapResolver resolver;
  EXPECT_FALSE(
      resolver("https://json-schema.org/draft/2020-12/schema").has_value());
}

TEST(JSONSchema_SchemaMapResolver, empty_with_fallback) {
  sourcemeta::core::SchemaMapResolver resolver{
      sourcemeta::core::schema_official_resolver};
  EXPECT_TRUE(
      resolver("https://json-schema.org/draft/2020-12/schema").has_value());
  EXPECT_EQ(resolver("https://json-schema.org/draft/2020-12/schema"),
            sourcemeta::core::schema_official_resolver(
                "https://json-schema.org/draft/2020-12/schema"));
}

TEST(JSONSchema_SchemaMapResolver, single_schema) {
  sourcemeta::core::SchemaMapResolver resolver;

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const auto result{resolver.add(document)};
  EXPECT_TRUE(result);

  EXPECT_TRUE(resolver("https://www.sourcemeta.com/test").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/test").value(), document);
}

TEST(JSONSchema_SchemaMapResolver, single_schema_with_default_dialect) {
  sourcemeta::core::SchemaMapResolver resolver;

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test"
  })JSON");

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const auto result{
      resolver.add(document, "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_TRUE(result);

  EXPECT_TRUE(resolver("https://www.sourcemeta.com/test").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/test").value(), expected);
}

TEST(JSONSchema_SchemaMapResolver, single_schema_anonymous_with_default) {
  sourcemeta::core::SchemaMapResolver resolver;

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const auto result{
      resolver.add(document, std::nullopt, "https://www.sourcemeta.com/test")};
  EXPECT_TRUE(result);

  EXPECT_TRUE(resolver("https://www.sourcemeta.com/test").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/test").value(), expected);
}

TEST(JSONSchema_SchemaMapResolver, single_schema_idempotent) {
  sourcemeta::core::SchemaMapResolver resolver;

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const auto result_1{resolver.add(document)};
  const auto result_2{resolver.add(document)};
  const auto result_3{resolver.add(document)};

  EXPECT_TRUE(result_1);
  EXPECT_TRUE(result_2);
  EXPECT_TRUE(result_3);

  EXPECT_TRUE(resolver("https://www.sourcemeta.com/test").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/test").value(), document);
}

TEST(JSONSchema_SchemaMapResolver, duplicate_ids) {
  sourcemeta::core::SchemaMapResolver resolver;

  const sourcemeta::core::JSON document_1 =
      sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const sourcemeta::core::JSON document_2 =
      sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string"
  })JSON");

  const auto result{resolver.add(document_1)};
  EXPECT_TRUE(result);
  EXPECT_THROW(resolver.add(document_2),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_SchemaMapResolver, embedded_resource) {
  sourcemeta::core::SchemaMapResolver resolver;

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "string": {
        "$id": "string",
        "type": "string"
      }
    }
  })JSON");

  const auto result{resolver.add(document)};
  EXPECT_TRUE(result);

  const sourcemeta::core::JSON embedded = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/string",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string"
  })JSON");

  EXPECT_TRUE(resolver("https://www.sourcemeta.com/test").has_value());
  EXPECT_TRUE(resolver("https://www.sourcemeta.com/string").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/test").value(), document);
  EXPECT_EQ(resolver("https://www.sourcemeta.com/string").value(), embedded);
}

TEST(JSONSchema_SchemaMapResolver, embedded_resource_with_callback) {
  sourcemeta::core::SchemaMapResolver resolver;
  std::unordered_set<sourcemeta::core::JSON::String> identifiers;

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "string": {
        "$id": "string",
        "type": "string"
      }
    }
  })JSON");

  const auto result{resolver.add(document, std::nullopt, std::nullopt,
                                 [&identifiers](const auto &identifier) {
                                   identifiers.insert(identifier);
                                 })};
  EXPECT_TRUE(result);

  const sourcemeta::core::JSON embedded = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/string",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string"
  })JSON");

  EXPECT_TRUE(resolver("https://www.sourcemeta.com/test").has_value());
  EXPECT_TRUE(resolver("https://www.sourcemeta.com/string").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/test").value(), document);
  EXPECT_EQ(resolver("https://www.sourcemeta.com/string").value(), embedded);

  EXPECT_EQ(identifiers.size(), 2);
  EXPECT_TRUE(identifiers.contains("https://www.sourcemeta.com/test"));
  EXPECT_TRUE(identifiers.contains("https://www.sourcemeta.com/string"));
}
