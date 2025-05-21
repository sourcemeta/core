#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/jsonschema_error.h>
#include <sourcemeta/core/jsonschema_resolver.h>

TEST(JSONSchema_SchemaFlatFileResolver, empty_no_fallback) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  EXPECT_FALSE(
      resolver("https://json-schema.org/draft/2020-12/schema").has_value());
}

TEST(JSONSchema_SchemaFlatFileResolver, empty_with_fallback) {
  sourcemeta::core::SchemaFlatFileResolver resolver{
      sourcemeta::core::schema_official_resolver};
  EXPECT_TRUE(
      resolver("https://json-schema.org/draft/2020-12/schema").has_value());
  EXPECT_EQ(resolver("https://json-schema.org/draft/2020-12/schema"),
            sourcemeta::core::schema_official_resolver(
                "https://json-schema.org/draft/2020-12/schema"));
}

TEST(JSONSchema_SchemaFlatFileResolver, single_schema) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-id.json"};
  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-id.json");
  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-id.json").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/2020-12-id.json").value(),
            sourcemeta::core::read_json(schema_path));
}

TEST(JSONSchema_SchemaFlatFileResolver, single_schema_custom_reader) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} / "2020-12-id"};
  const auto &identifier{resolver.add(
      schema_path, std::nullopt, std::nullopt, [](const auto &path) {
        return sourcemeta::core::read_json(path.string() + ".json");
      })};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-id.json");
  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-id.json").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/2020-12-id.json").value(),
            sourcemeta::core::read_json(schema_path.string() + ".json"));
}

TEST(JSONSchema_SchemaFlatFileResolver, single_schema_with_default_dialect) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} / "only-id.json"};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/only-id.json",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const auto &identifier{resolver.add(
      schema_path, "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/only-id.json");
  EXPECT_TRUE(resolver("https://www.sourcemeta.com/only-id.json").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/only-id.json").value(),
            expected);
}

TEST(JSONSchema_SchemaFlatFileResolver,
     single_schema_anonymous_with_default_id) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-anonymous.json"};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const auto &identifier{resolver.add(schema_path, std::nullopt,
                                      "https://www.sourcemeta.com/test")};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/test");
  EXPECT_TRUE(resolver("https://www.sourcemeta.com/test").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/test").value(), expected);
}

TEST(JSONSchema_SchemaFlatFileResolver, draft4_top_level_ref) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "draft4-top-level-ref.json"};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "https://www.sourcemeta.com/draft4-top-level-ref.json",
    "allOf": [ { "$ref": "#/definitions/foo" } ],
    "definitions": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/draft4-top-level-ref.json");
  EXPECT_TRUE(resolver("https://www.sourcemeta.com/draft4-top-level-ref.json")
                  .has_value());
  EXPECT_EQ(
      resolver("https://www.sourcemeta.com/draft4-top-level-ref.json").value(),
      expected);
}

TEST(JSONSchema_SchemaFlatFileResolver, single_schema_idempotent) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-id.json"};

  const auto &identifier_1{resolver.add(schema_path)};
  const auto &identifier_2{resolver.add(schema_path)};
  const auto &identifier_3{resolver.add(schema_path)};

  EXPECT_EQ(identifier_1, "https://www.sourcemeta.com/2020-12-id.json");
  EXPECT_EQ(identifier_2, "https://www.sourcemeta.com/2020-12-id.json");
  EXPECT_EQ(identifier_3, "https://www.sourcemeta.com/2020-12-id.json");

  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-id.json").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/2020-12-id.json").value(),
            sourcemeta::core::read_json(schema_path));
}

TEST(JSONSchema_SchemaFlatFileResolver, duplicate_ids) {
  sourcemeta::core::SchemaFlatFileResolver resolver;

  const auto schema_path_1{std::filesystem::path{SCHEMAS_PATH} /
                           "2020-12-id.json"};
  const auto schema_path_2{std::filesystem::path{SCHEMAS_PATH} /
                           "2020-12-id-duplicate.json"};

  const auto &identifier{resolver.add(schema_path_1)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-id.json");
  EXPECT_THROW(resolver.add(schema_path_2), sourcemeta::core::SchemaError);
}

TEST(JSONSchema_SchemaFlatFileResolver, no_embedded_resource) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-embedded.json"};

  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-embedded.json");

  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-embedded.json").has_value());
  EXPECT_EQ(
      resolver("https://www.sourcemeta.com/2020-12-embedded.json").value(),
      sourcemeta::core::read_json(schema_path));
  EXPECT_FALSE(resolver("https://www.sourcemeta.com/string").has_value());
}

TEST(JSONSchema_SchemaFlatFileResolver, metaschema_out_of_order) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-meta-1-schema.json"};
  const auto metaschema_path{std::filesystem::path{SCHEMAS_PATH} /
                             "2020-12-meta-1.json"};

  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier,
            "https://www.sourcemeta.com/2020-12-meta-1-schema.json");

  // We can't resolve it yet until eventually satisfying the metaschema
  EXPECT_THROW(
      resolver("https://www.sourcemeta.com/2020-12-meta-1-schema.json"),
      sourcemeta::core::SchemaResolutionError);

  // Note we add the metaschema AFTER the schema
  const auto &metaschema_identifier{resolver.add(metaschema_path)};
  EXPECT_EQ(metaschema_identifier,
            "https://www.sourcemeta.com/2020-12-meta-1.json");

  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-meta-1.json").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/2020-12-meta-1.json").value(),
            sourcemeta::core::read_json(metaschema_path));

  EXPECT_TRUE(resolver("https://www.sourcemeta.com/2020-12-meta-1-schema.json")
                  .has_value());
  EXPECT_EQ(
      resolver("https://www.sourcemeta.com/2020-12-meta-1-schema.json").value(),
      sourcemeta::core::read_json(schema_path));
}

TEST(JSONSchema_SchemaFlatFileResolver, iterators) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-id.json"};
  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-id.json");

  std::vector<std::string> identifiers;
  std::vector<sourcemeta::core::SchemaFlatFileResolver::Entry> entries;
  for (const auto &entry : resolver) {
    identifiers.push_back(entry.first);
    entries.push_back(entry.second);
  }

  EXPECT_EQ(identifiers.size(), 1);
  EXPECT_EQ(identifiers.at(0), "https://www.sourcemeta.com/2020-12-id.json");

  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(entries.at(0).path, schema_path);
  EXPECT_FALSE(entries.at(0).default_dialect.has_value());
}

TEST(JSONSchema_SchemaFlatFileResolver, reidentify) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-id.json"};
  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-id.json");
  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-id.json").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/2020-12-id.json").value(),
            sourcemeta::core::read_json(schema_path));

  resolver.reidentify("https://www.sourcemeta.com/2020-12-id.json",
                      "https://example.com");

  EXPECT_FALSE(
      resolver("https://www.sourcemeta.com/2020-12-id.json").has_value());
  EXPECT_TRUE(resolver("https://example.com").has_value());

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  EXPECT_EQ(resolver("https://example.com").value(), expected);
}

TEST(JSONSchema_SchemaFlatFileResolver, with_absolute_references) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-absolute-ref.json"};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/2020-12-absolute-ref.json",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "2020-12-id.json"
  })JSON");

  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-absolute-ref.json");
  EXPECT_TRUE(resolver("https://www.sourcemeta.com/2020-12-absolute-ref.json")
                  .has_value());
  EXPECT_EQ(
      resolver("https://www.sourcemeta.com/2020-12-absolute-ref.json").value(),
      expected);
}

TEST(JSONSchema_SchemaFlatFileResolver, case_insensitive_lookup) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-id.json"};
  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-id.json");
  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-Id.json").has_value());
  EXPECT_EQ(resolver("https://www.sourcemeta.com/2020-12-Id.json").value(),
            sourcemeta::core::read_json(schema_path));
  EXPECT_TRUE(
      resolver("https://WwW.SOURCEmeta.com/2020-12-Id.json").has_value());
  EXPECT_EQ(resolver("https://WwW.SOURCEmeta.com/2020-12-Id.json").value(),
            sourcemeta::core::read_json(schema_path));
}

TEST(JSONSchema_SchemaFlatFileResolver, case_insensitive_insert) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-id-casing.json"};
  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-id.json");
  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-id.json").has_value());
  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-ID.json").has_value());
  EXPECT_TRUE(
      resolver("https://www.sourcemeta.com/2020-12-iD.json").has_value());
  EXPECT_TRUE(
      resolver("https://www.SOURCEMETA.com/2020-12-id.json").has_value());
}

TEST(JSONSchema_SchemaFlatFileResolver, case_insensitive_reidentify) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-id-casing.json"};
  const auto &identifier{resolver.add(schema_path)};
  resolver.reidentify(identifier, "https://EXAMPLE.com");
  EXPECT_TRUE(resolver("https://EXAMPLE.com").has_value());
  EXPECT_TRUE(resolver("https://example.com").has_value());
}

TEST(JSONSchema_SchemaFlatFileResolver, with_recursive_ref) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2019-09-recursive-ref.json"};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$id": "https://www.sourcemeta.com/2019-09-recursive-ref.json",
    "$recursiveAnchor": true,
    "additionalProperties": {
      "$recursiveRef": "#"
    }
  })JSON");

  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier,
            "https://www.sourcemeta.com/2019-09-recursive-ref.json");
  EXPECT_TRUE(resolver("https://www.sourcemeta.com/2019-09-recursive-ref.json")
                  .has_value());
  EXPECT_EQ(
      resolver("https://www.sourcemeta.com/2019-09-recursive-ref.json").value(),
      expected);
}

TEST(JSONSchema_SchemaFlatFileResolver, with_absolute_references_reidentify) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-absolute-ref.json"};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com/2020-12-absolute-ref.json",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "2020-12-id.json"
  })JSON");

  const auto &identifier{resolver.add(schema_path)};
  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-absolute-ref.json");

  resolver.reidentify("https://www.sourcemeta.com/2020-12-absolute-ref.json",
                      "https://www.example.com/2020-12-absolute-ref.json");

  EXPECT_FALSE(resolver("https://www.sourcemeta.com/2020-12-absolute-ref.json")
                   .has_value());
  EXPECT_TRUE(resolver("https://www.example.com/2020-12-absolute-ref.json")
                  .has_value());
  EXPECT_EQ(
      resolver("https://www.example.com/2020-12-absolute-ref.json").value(),
      expected);
}

TEST(JSONSchema_SchemaFlatFileResolver, custom_reference_visitor) {
  sourcemeta::core::SchemaFlatFileResolver resolver;
  const auto schema_path{std::filesystem::path{SCHEMAS_PATH} /
                         "2020-12-absolute-ref.json"};

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/2020-12-absolute-ref.json",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "MUTATED"
  })JSON");

  const auto &identifier{resolver.add(
      schema_path, std::nullopt, std::nullopt,
      [](const std::filesystem::path &path) {
        return sourcemeta::core::read_json(path);
      },
      [](sourcemeta::core::JSON &schema, const sourcemeta::core::URI &,
         const sourcemeta::core::JSON::String &,
         const sourcemeta::core::JSON::String &keyword,
         sourcemeta::core::URI &) {
        schema.assign(keyword, sourcemeta::core::JSON{"MUTATED"});
      })};

  EXPECT_EQ(identifier, "https://www.sourcemeta.com/2020-12-absolute-ref.json");
  EXPECT_TRUE(resolver("https://www.sourcemeta.com/2020-12-absolute-ref.json")
                  .has_value());
  EXPECT_EQ(
      resolver("https://www.sourcemeta.com/2020-12-absolute-ref.json").value(),
      expected);
}
