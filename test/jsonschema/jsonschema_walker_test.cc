#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <vector>

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://sourcemeta.com/test-metaschema") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://sourcemeta.com/test-metaschema",
      "$vocabulary": {
        "https://json-schema.org/draft/2020-12/vocab/core": true,
        "https://sourcemeta.com/vocab/test-1": true
      }
    })JSON");
  } else if (identifier == "https://sourcemeta.com/custom-vocab") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://sourcemeta.com/custom-vocab",
      "$vocabulary": {
        "https://json-schema.org/draft/2020-12/vocab/core": true,
        "https://sourcemeta.com/vocab/test-2": true
      }
    })JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

static auto test_walker(std::string_view keyword,
                        const sourcemeta::core::Vocabularies &vocabularies)
    -> sourcemeta::core::SchemaWalkerResult {
  if (vocabularies.find("https://sourcemeta.com/vocab/test-1") !=
      vocabularies.end()) {
    if (keyword == "schema") {
      return {sourcemeta::core::SchemaKeywordType::
                  ApplicatorValueTraverseSomeProperty,
              "https://sourcemeta.com/vocab/test-1",
              {}};
    }

    if (keyword == "schemas") {
      return {
          sourcemeta::core::SchemaKeywordType::ApplicatorElementsTraverseItem,
          "https://sourcemeta.com/vocab/test-1",
          {}};
    }

    if (keyword == "schemaMap") {
      return {sourcemeta::core::SchemaKeywordType::
                  ApplicatorMembersTraversePropertyStatic,
              "https://sourcemeta.com/vocab/test-1",
              {}};
    }

    if (keyword == "schemaOrSchemas") {
      return {sourcemeta::core::SchemaKeywordType::
                  ApplicatorValueOrElementsTraverseAnyItemOrItem,
              "https://sourcemeta.com/vocab/test-1",
              {}};
    }
  }

  if (vocabularies.find("https://sourcemeta.com/vocab/test-2") !=
      vocabularies.end()) {
    if (keyword == "custom") {
      return {sourcemeta::core::SchemaKeywordType::
                  ApplicatorValueTraverseSomeProperty,
              "https://sourcemeta.com/vocab/test-2",
              {}};
    }
  }

  return {sourcemeta::core::SchemaKeywordType::Unknown, std::nullopt, {}};
}

TEST(JSONSchema_walker, true) {
  const sourcemeta::core::JSON document{true};
  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 1);
  EXPECT_EQ(subschemas.at(0).is_boolean(), true);
  EXPECT_EQ(subschemas.at(0).to_boolean(), true);
  EXPECT_EQ(entries.size(), 1);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_FALSE(entries.at(0).dialect.has_value());
  EXPECT_FALSE(entries.at(0).base_dialect.has_value());
  EXPECT_TRUE(entries.at(0).vocabularies.empty());
}

TEST(JSONSchema_walker, false) {
  const sourcemeta::core::JSON document{false};
  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 1);
  EXPECT_EQ(subschemas.at(0).is_boolean(), true);
  EXPECT_EQ(subschemas.at(0).to_boolean(), false);
  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_FALSE(entries.at(0).dialect.has_value());
  EXPECT_FALSE(entries.at(0).base_dialect.has_value());
  EXPECT_TRUE(entries.at(0).vocabularies.empty());
}

TEST(JSONSchema_walker, value) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": {
      "schema": {
        "foo": 1
      }
    }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 3);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": {
      "schema": {
        "foo": 1
      }
    }
  })JSON"));
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "schema": {
      "foo": 1
    }
  })JSON"));
  EXPECT_EQ(subschemas.at(2), sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON"));

  EXPECT_EQ(entries.size(), 3);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer, sourcemeta::core::Pointer{"schema"});
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(2).pointer,
            sourcemeta::core::Pointer({"schema", "schema"}));
  EXPECT_EQ(entries.at(2).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(2).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, value_invalid) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": [ { "foo": 1 } ]
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 1);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": [ { "foo": 1 } ]
  })JSON"));

  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, elements) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemas": [ { "foo": 1 }, { "schema": { "bar": 2 } } ]
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 4);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemas": [ { "foo": 1 }, { "schema": { "bar": 2 } } ]
  })JSON"));
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON"));
  EXPECT_EQ(subschemas.at(2), sourcemeta::core::parse_json(R"JSON({
    "schema": { "bar": 2 }
  })JSON"));
  EXPECT_EQ(subschemas.at(3), sourcemeta::core::parse_json(R"JSON({
    "bar": 2
  })JSON"));

  EXPECT_EQ(entries.size(), 4);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer, sourcemeta::core::Pointer({"schemas", 0}));
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(2).pointer, sourcemeta::core::Pointer({"schemas", 1}));
  EXPECT_EQ(entries.at(2).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(2).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(3).pointer,
            sourcemeta::core::Pointer({"schemas", 1, "schema"}));
  EXPECT_EQ(entries.at(3).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(3).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, elements_invalid) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemas": { "foo": 1 }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 1);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemas": { "foo": 1 }
  })JSON"));

  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, members) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemaMap": {
      "foo": {
        "schema": {
          "bar": 1
        }
      }
    }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 3);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemaMap": {
      "foo": {
        "schema": {
          "bar": 1
        }
      }
    }
  })JSON"));
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "schema": { "bar": 1 }
  })JSON"));
  EXPECT_EQ(subschemas.at(2), sourcemeta::core::parse_json(R"JSON({
    "bar": 1
  })JSON"));

  EXPECT_EQ(entries.size(), 3);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer,
            sourcemeta::core::Pointer({"schemaMap", "foo"}));
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(2).pointer,
            sourcemeta::core::Pointer({"schemaMap", "foo", "schema"}));
  EXPECT_EQ(entries.at(2).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(2).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, members_invalid) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemaMap": [ { "foo": 1 } ]
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 1);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemaMap": [ { "foo": 1 } ]
  })JSON"));

  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, value_or_elements) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemaOrSchemas": {
      "schemaOrSchemas": [
        { "foo": 1 }
      ]
    }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 3);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemaOrSchemas": {
      "schemaOrSchemas": [
        { "foo": 1 }
      ]
    }
  })JSON"));
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "schemaOrSchemas": [ { "foo": 1 } ]
  })JSON"));

  EXPECT_EQ(subschemas.at(2), sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON"));

  EXPECT_EQ(entries.size(), 3);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer,
            sourcemeta::core::Pointer{"schemaOrSchemas"});
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(
      entries.at(2).pointer,
      sourcemeta::core::Pointer({"schemaOrSchemas", "schemaOrSchemas", 0}));
  EXPECT_EQ(entries.at(2).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(2).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, no_metaschema_and_no_default) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "schema": { "foo": 1 }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 1);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "schema": { "foo": 1 }
  })JSON"));

  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_FALSE(entries.at(0).dialect.has_value());
  EXPECT_FALSE(entries.at(0).base_dialect.has_value());
}

TEST(JSONSchema_walker, no_metaschema_with_default) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "schema": {
      "schema": {
        "foo": 1
      }
    }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIterator(
           document, test_walker, test_resolver,
           "https://sourcemeta.com/test-metaschema")) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 3);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "schema": {
      "schema": {
        "foo": 1
      }
    }
  })JSON"));
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "schema": {
      "foo": 1
    }
  })JSON"));
  EXPECT_EQ(subschemas.at(2), sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON"));

  EXPECT_EQ(entries.size(), 3);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer, sourcemeta::core::Pointer{"schema"});
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(2).pointer,
            sourcemeta::core::Pointer({"schema", "schema"}));
  EXPECT_EQ(entries.at(2).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(2).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, unknown_keyword_from_other_vocab) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": {
      "custom": { "foo": 1 }
    }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 2);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": {
      "custom": { "foo": 1 }
    }
  })JSON"));
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "custom": { "foo": 1 }
  })JSON"));

  EXPECT_EQ(entries.size(), 2);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer, sourcemeta::core::Pointer{"schema"});
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, multi_metaschemas) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": {
      "$schema": "https://sourcemeta.com/custom-vocab",
      "custom": { "foo": 1 }
    }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 3);
  EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": {
      "$schema": "https://sourcemeta.com/custom-vocab",
      "custom": { "foo": 1 }
    }
  })JSON"));
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/custom-vocab",
    "custom": { "foo": 1 }
  })JSON"));
  EXPECT_EQ(subschemas.at(2), sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON"));

  EXPECT_EQ(entries.size(), 3);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer, sourcemeta::core::Pointer{"schema"});
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/custom-vocab");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(2).pointer,
            sourcemeta::core::Pointer({"schema", "custom"}));
  EXPECT_EQ(entries.at(2).dialect, "https://sourcemeta.com/custom-vocab");
  EXPECT_EQ(entries.at(2).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, flat) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": {
      "schema": { "foo": 1 }
    },
    "schemaMap": {
      "foo": {
        "schema": { "foo": 2 }
      }
    }
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIteratorFlat(
           document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 2);
  EXPECT_EQ(entries.size(), 2);

  // We don't guarantee any specific ordering
  if (subschemas.at(0).at("schema").at("foo").to_integer() == 1) {
    EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
      "schema": { "foo": 1 }
    })JSON"));
    EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
      "schema": { "foo": 2 }
    })JSON"));

    EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{"schema"});
    EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
    EXPECT_EQ(entries.at(0).base_dialect,
              "https://json-schema.org/draft/2020-12/schema");

    EXPECT_EQ(entries.at(1).pointer,
              sourcemeta::core::Pointer({"schemaMap", "foo"}));
    EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
    EXPECT_EQ(entries.at(1).base_dialect,
              "https://json-schema.org/draft/2020-12/schema");
  } else {
    EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
      "schema": { "foo": 1 }
    })JSON"));
    EXPECT_EQ(subschemas.at(0), sourcemeta::core::parse_json(R"JSON({
      "schema": { "foo": 2 }
    })JSON"));

    EXPECT_EQ(entries.at(1).pointer, sourcemeta::core::Pointer{"schema"});
    EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
    EXPECT_EQ(entries.at(1).base_dialect,
              "https://json-schema.org/draft/2020-12/schema");

    EXPECT_EQ(entries.at(0).pointer,
              sourcemeta::core::Pointer({"schemaMap", "foo"}));
    EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
    EXPECT_EQ(entries.at(0).base_dialect,
              "https://json-schema.org/draft/2020-12/schema");
  }
}

TEST(JSONSchema_walker, flat_non_modify) {
  const std::string json{R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": {
      "schema": { "foo": 1 }
    },
    "schemaMap": {
      "foo": {
        "schema": { "foo": 2 }
      }
    }
  })JSON"};

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(json);
  for (const auto &entry : sourcemeta::core::SchemaIteratorFlat(
           document, test_walker, test_resolver)) {
    sourcemeta::core::set(document, entry.pointer,
                          sourcemeta::core::JSON{true});
  }

  EXPECT_EQ(document, sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schema": true,
    "schemaMap": {
      "foo": true
    }
  })JSON"));
}

TEST(JSONSchema_walker, flat_no_metaschema) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json(R"JSON({ "foo": 1 })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIteratorFlat(
           document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 0);
  EXPECT_EQ(entries.size(), 0);
}

TEST(JSONSchema_walker, members_with_array) {
  const std::string json{R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemaMap": {
      "foo": { "test": 1 },
      "bar": [ "baz" ]
    }
  })JSON"};

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(json);
  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 2);
  EXPECT_EQ(subschemas.at(0), document);
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "test": 1
  })JSON"));

  EXPECT_EQ(entries.size(), 2);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer,
            sourcemeta::core::Pointer({"schemaMap", "foo"}));
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, elements_with_string_items) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemas": [ { "foo": 1 }, "foo" ]
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 2);
  EXPECT_EQ(subschemas.at(0), document);
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON"));

  EXPECT_EQ(entries.size(), 2);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer, sourcemeta::core::Pointer({"schemas", 0}));
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_walker, value_or_elements_with_string_items) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/test-metaschema",
    "schemaOrSchemas": [ { "foo": 1 }, "foo" ]
  })JSON");

  std::vector<sourcemeta::core::JSON> subschemas;
  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry :
       sourcemeta::core::SchemaIterator(document, test_walker, test_resolver)) {
    subschemas.push_back(sourcemeta::core::get(document, entry.pointer));
    entries.push_back(entry);
  }

  EXPECT_EQ(subschemas.size(), 2);
  EXPECT_EQ(subschemas.at(0), document);
  EXPECT_EQ(subschemas.at(1), sourcemeta::core::parse_json(R"JSON({
    "foo": 1
  })JSON"));

  EXPECT_EQ(entries.size(), 2);

  EXPECT_EQ(entries.at(0).pointer, sourcemeta::core::Pointer{});
  EXPECT_EQ(entries.at(0).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(0).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(entries.at(1).pointer,
            sourcemeta::core::Pointer({"schemaOrSchemas", 0}));
  EXPECT_EQ(entries.at(1).dialect, "https://sourcemeta.com/test-metaschema");
  EXPECT_EQ(entries.at(1).base_dialect,
            "https://json-schema.org/draft/2020-12/schema");
}
