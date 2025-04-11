#include <gtest/gtest.h>

#include <string>
#include <tuple>
#include <vector>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer, flat_document_no_applicators) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "qux": "xxx"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, throw_if_no_dialect_invalid_default) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  EXPECT_THROW(bundle.apply(document, sourcemeta::core::schema_official_walker,
                            sourcemeta::core::schema_official_resolver,
                            "https://example.com/invalid"),
               sourcemeta::core::SchemaResolutionError);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  // Nothing should have been modified
  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, with_default_dialect) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   "https://json-schema.org/draft/2020-12/schema");

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "qux": "xxx"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, with_explicit_default_dialect_same) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   "https://json-schema.org/draft/2020-12/schema");

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "qux": "xxx"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, throw_on_rules_called_twice) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRuleConflictsWith1>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar"
  })JSON");

  EXPECT_THROW(bundle.apply(document, sourcemeta::core::schema_official_walker,
                            sourcemeta::core::schema_official_resolver),
               std::runtime_error);
}

TEST(JSONSchema_transformer, top_level_rule) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule3>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "type": "string" }
    }
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "top": true,
    "properties": {
      "foo": { "type": "string" }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, walker_2020_12) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule4>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "type": "string" },
      "bar": {
        "items": {
          "type": "number"
        }
      }
    }
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "here": true,
    "properties": {
      "foo": {
        "type": "string",
        "here": true
      },
      "bar": {
        "here": true,
        "items": {
          "here": true,
          "type": "number"
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, mismatch_default_dialect) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule4>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "type": "string" },
      "bar": {
        "unevaluatedItems": {
          "type": "number"
        }
      }
    }
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   "http://json-schema.org/draft-04/schema#");

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "here": true,
    "properties": {
      "foo": {
        "type": "string",
        "here": true
      },
      "bar": {
        "here": true,
        "unevaluatedItems": {
          "here": true,
          "type": "number"
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rule_pointers) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule5>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "type": "string" },
      "bar": { "type": "string" },
      "baz": { "type": "string" }
    }
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "type": "string" },
      "bar": { "type": "string" },
      "baz": { "type": "string", "baz": true }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, multi_dialect_rules) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule4>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedItems": { "type": "string" },
    "$defs": {
      "foo": {
        "$schema": "http://json-schema.org/draft-03/schema#",
        "additionalItems": {
          "type": "string"
        }
      }
    }
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "here": true,
    "unevaluatedItems": {
      "here": true,
      "type": "string"
    },
    "$defs": {
      "foo": {
        "$schema": "http://json-schema.org/draft-03/schema#",
        "here": true,
        "additionalItems": {
          "here": true,
          "type": "string"
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, dialect_specific_rules) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule6>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "foo": {
        "id": "https://www.example.com/foo",
        "$schema": "http://json-schema.org/draft-03/schema#"
      },
      "bar": {
        "id": "https://www.example.com/bar",
        "$schema": "http://json-schema.org/draft-02/schema#"
      },
      "baz": {
        "id": "https://www.example.com/baz",
        "$schema": "http://json-schema.org/draft-01/schema#"
      }
    }
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "foo": {
        "id": "https://www.example.com/foo",
        "$schema": "http://json-schema.org/draft-03/schema#",
        "draft": 3
      },
      "bar": {
        "id": "https://www.example.com/bar",
        "$schema": "http://json-schema.org/draft-02/schema#"
      },
      "baz": {
        "id": "https://www.example.com/baz",
        "$schema": "http://json-schema.org/draft-01/schema#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, dialect_specific_rules_without_ids) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule6>();

  // Note that `$schema` is only valid on subschemas that represent
  // schema resources. Here we test that if not, `$schema` is ignored
  // and the dialects never change

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "foo": { "$schema": "http://json-schema.org/draft-03/schema#" },
      "bar": { "$schema": "http://json-schema.org/draft-02/schema#" },
      "baz": { "$schema": "http://json-schema.org/draft-01/schema#" }
    }
  })JSON");

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "foo": { "$schema": "http://json-schema.org/draft-03/schema#" },
      "bar": { "$schema": "http://json-schema.org/draft-02/schema#" },
      "baz": { "$schema": "http://json-schema.org/draft-01/schema#" }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, check_top_level) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar",
    "properties": {
      "xxx": {
        "bar": "baz"
      }
    }
  })JSON");

  std::vector<std::tuple<sourcemeta::core::Pointer, std::string, std::string>>
      entries;
  const bool result = bundle.check(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [&entries](const auto &pointer, const auto &name, const auto &message) {
        entries.emplace_back(pointer, name, message);
      });

  EXPECT_FALSE(result);
  EXPECT_EQ(entries.size(), 2);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_1");
  EXPECT_EQ(std::get<2>(entries.at(0)), "Keyword foo is not permitted");

  EXPECT_EQ(std::get<0>(entries.at(1)),
            sourcemeta::core::Pointer({"properties", "xxx"}));
  EXPECT_EQ(std::get<1>(entries.at(1)), "example_rule_2");
  EXPECT_EQ(std::get<2>(entries.at(1)), "Keyword bar is not permitted");
}

TEST(JSONSchema_transformer, check_no_match) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "xxx": {
        "type": "string"
      }
    }
  })JSON");

  std::vector<std::tuple<sourcemeta::core::Pointer, std::string, std::string>>
      entries;
  const bool result = bundle.check(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [&entries](const auto &pointer, const auto &name, const auto &message) {
        entries.emplace_back(pointer, name, message);
      });

  EXPECT_TRUE(result);
  EXPECT_TRUE(entries.empty());
}

TEST(JSONSchema_transformer, check_empty) {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar"
  })JSON");

  std::vector<std::tuple<sourcemeta::core::Pointer, std::string, std::string>>
      entries;
  const bool result = bundle.check(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [&entries](const auto &pointer, const auto &name, const auto &message) {
        entries.emplace_back(pointer, name, message);
      });

  EXPECT_TRUE(result);
  EXPECT_TRUE(entries.empty());
}

TEST(JSONSchema_transformer, check_throw_if_no_dialect_invalid_default) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  EXPECT_THROW(bundle.check(document, sourcemeta::core::schema_official_walker,
                            sourcemeta::core::schema_official_resolver, nullptr,
                            "https://example.com/invalid"),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_transformer, check_with_default_dialect) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "foo": "bar",
    "properties": {
      "xxx": {
        "bar": "baz"
      }
    }
  })JSON");

  std::vector<std::tuple<sourcemeta::core::Pointer, std::string, std::string>>
      entries;
  const bool result = bundle.check(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [&entries](const auto &pointer, const auto &name, const auto &message) {
        entries.emplace_back(pointer, name, message);
      },
      "https://json-schema.org/draft/2020-12/schema");

  EXPECT_FALSE(result);
  EXPECT_EQ(entries.size(), 2);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_1");
  EXPECT_EQ(std::get<2>(entries.at(0)), "Keyword foo is not permitted");

  EXPECT_EQ(std::get<0>(entries.at(1)),
            sourcemeta::core::Pointer({"properties", "xxx"}));
  EXPECT_EQ(std::get<1>(entries.at(1)), "example_rule_2");
  EXPECT_EQ(std::get<2>(entries.at(1)), "Keyword bar is not permitted");
}

TEST(JSONSchema_transformer, remove_rule_by_name) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  EXPECT_TRUE(bundle.remove("example_rule_2"));
  EXPECT_FALSE(bundle.remove("example_rule_2"));
  EXPECT_FALSE(bundle.remove("i_dont_exist"));

  const auto result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver);

  EXPECT_TRUE(result);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  EXPECT_EQ(document, expected);
}
