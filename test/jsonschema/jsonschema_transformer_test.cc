#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "jsonschema_transform_rules.h"

static auto
transformer_callback_noop(const sourcemeta::core::Pointer &,
                          const std::string_view, const std::string_view,
                          const sourcemeta::core::SchemaTransformRule::Result &)
    -> void {}

using TestTransformTraces =
    std::vector<std::tuple<sourcemeta::core::Pointer, std::string, std::string,
                           sourcemeta::core::SchemaTransformRule::Result>>;

static auto transformer_callback_trace(TestTransformTraces &traces) -> auto {
  return [&traces](const auto &pointer, const auto &name, const auto &message,
                   const auto &result) {
    traces.emplace_back(pointer, name, message, result);
  };
}

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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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
                            transformer_callback_noop,
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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries),
                   "https://json-schema.org/draft/2020-12/schema");

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries),
                   "https://json-schema.org/draft/2020-12/schema");

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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
                            sourcemeta::core::schema_official_resolver,
                            transformer_callback_noop),
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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries),
                   "http://json-schema.org/draft-04/schema#");

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

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
  bundle.add<ExampleRule1WithPointer>();
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

  TestTransformTraces entries;
  const auto result =
      bundle.check(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_FALSE(result.first);
  EXPECT_EQ(result.second, 0);

  EXPECT_EQ(entries.size(), 2);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_1");
  EXPECT_EQ(std::get<2>(entries.at(0)), "Keyword foo is not permitted");
  EXPECT_EQ(std::get<3>(entries.at(0)).locations.size(), 1);
  EXPECT_EQ(
      sourcemeta::core::to_string(std::get<3>(entries.at(0)).locations.at(0)),
      "/foo");
  EXPECT_FALSE(std::get<3>(entries.at(0)).description.has_value());

  EXPECT_EQ(std::get<0>(entries.at(1)),
            sourcemeta::core::Pointer({"properties", "xxx"}));
  EXPECT_EQ(std::get<1>(entries.at(1)), "example_rule_2");
  EXPECT_EQ(std::get<2>(entries.at(1)), "Keyword bar is not permitted");
  EXPECT_EQ(std::get<3>(entries.at(1)).locations.size(), 0);
  EXPECT_FALSE(std::get<3>(entries.at(1)).description.has_value());
}

TEST(JSONSchema_transformer, check_multiple_pointers) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleWithManyPointers>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar",
    "bar": "baz"
  })JSON");

  TestTransformTraces entries;
  const auto result =
      bundle.check(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_FALSE(result.first);
  EXPECT_EQ(result.second, 0);

  EXPECT_EQ(entries.size(), 1);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_with_many_pointers");
  EXPECT_EQ(std::get<2>(entries.at(0)), "Foo Bar");
  EXPECT_EQ(std::get<3>(entries.at(0)).locations.size(), 2);
  EXPECT_EQ(
      sourcemeta::core::to_string(std::get<3>(entries.at(0)).locations.at(0)),
      "/foo");
  EXPECT_EQ(
      sourcemeta::core::to_string(std::get<3>(entries.at(0)).locations.at(1)),
      "/bar");
  EXPECT_FALSE(std::get<3>(entries.at(0)).description.has_value());
}

TEST(JSONSchema_transformer, check_with_description) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1WithDescription>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar"
  })JSON");

  TestTransformTraces entries;
  const auto result =
      bundle.check(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_FALSE(result.first);
  EXPECT_EQ(result.second, 0);

  EXPECT_EQ(entries.size(), 1);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_1");
  EXPECT_EQ(std::get<2>(entries.at(0)), "Keyword foo is not permitted");
  EXPECT_EQ(std::get<3>(entries.at(0)).locations.size(), 1);
  EXPECT_EQ(
      sourcemeta::core::to_string(std::get<3>(entries.at(0)).locations.at(0)),
      "/foo");
  EXPECT_TRUE(std::get<3>(entries.at(0)).description.has_value());
  EXPECT_EQ(std::get<3>(entries.at(0)).description.value(),
            "This is a message from the rule");
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

  TestTransformTraces entries;
  const auto result =
      bundle.check(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, 100);

  EXPECT_TRUE(entries.empty());
}

TEST(JSONSchema_transformer, check_partial_match) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "type": "string"
      },
      "bar": {
        "type": "string",
        "foo": 1
      },
      "baz": {
        "type": "string"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const auto result =
      bundle.check(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_FALSE(result.first);
  EXPECT_EQ(result.second, 75);

  EXPECT_EQ(entries.size(), 1);

  EXPECT_EQ(std::get<0>(entries.at(0)),
            sourcemeta::core::Pointer({"properties", "bar"}));
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_1");
  EXPECT_EQ(std::get<2>(entries.at(0)), "Keyword foo is not permitted");
  EXPECT_EQ(std::get<3>(entries.at(0)).locations.size(), 0);
  EXPECT_FALSE(std::get<3>(entries.at(0)).description.has_value());
}

TEST(JSONSchema_transformer, check_empty) {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar"
  })JSON");

  TestTransformTraces entries;
  const auto result =
      bundle.check(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, 100);

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

  EXPECT_THROW((void)bundle.check(document,
                                  sourcemeta::core::schema_official_walker,
                                  sourcemeta::core::schema_official_resolver,
                                  nullptr, "https://example.com/invalid"),
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

  TestTransformTraces entries;
  const auto result =
      bundle.check(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries),
                   "https://json-schema.org/draft/2020-12/schema");

  EXPECT_FALSE(result.first);
  EXPECT_EQ(result.second, 0);

  EXPECT_EQ(entries.size(), 2);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_1");
  EXPECT_EQ(std::get<2>(entries.at(0)), "Keyword foo is not permitted");
  EXPECT_EQ(std::get<3>(entries.at(0)).locations.size(), 0);
  EXPECT_FALSE(std::get<3>(entries.at(0)).description.has_value());

  EXPECT_EQ(std::get<0>(entries.at(1)),
            sourcemeta::core::Pointer({"properties", "xxx"}));
  EXPECT_EQ(std::get<1>(entries.at(1)), "example_rule_2");
  EXPECT_EQ(std::get<2>(entries.at(1)), "Keyword bar is not permitted");
  EXPECT_EQ(std::get<3>(entries.at(1)).locations.size(), 0);
  EXPECT_FALSE(std::get<3>(entries.at(1)).description.has_value());
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

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, unfixable_apply) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleUnfixable1>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_FALSE(result);

  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_unfixable_1");
  EXPECT_EQ(std::get<2>(entries.at(0)), "An example rule that cannot be fixed");
  EXPECT_EQ(std::get<3>(entries.at(0)).locations.size(), 0);
  EXPECT_FALSE(std::get<3>(entries.at(0)).description.has_value());

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, unfixable_check) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleUnfixable1>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "foo": "bar",
    "bar": "baz",
    "qux": "xxx"
  })JSON");

  TestTransformTraces entries;
  const auto result =
      bundle.check(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_FALSE(result.first);
  EXPECT_EQ(entries.size(), 1);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "example_rule_unfixable_1");
  EXPECT_EQ(std::get<2>(entries.at(0)), "An example rule that cannot be fixed");
  EXPECT_EQ(std::get<3>(entries.at(0)).locations.size(), 0);
  EXPECT_FALSE(std::get<3>(entries.at(0)).description.has_value());
}

TEST(JSONSchema_transformer, rereference_not_hit) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsNoRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "definitions": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_not_fixed_ref) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsNoRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/definitions/foo",
    "definitions": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  TestTransformTraces entries;

  try {
    bundle.apply(document, sourcemeta::core::schema_official_walker,
                 sourcemeta::core::schema_official_resolver,
                 transformer_callback_trace(entries));
    FAIL() << "The transformation was expected to throw";
  } catch (const sourcemeta::core::SchemaBrokenReferenceError &error) {
    EXPECT_EQ(error.id(), "#/definitions/foo");
    EXPECT_EQ(sourcemeta::core::to_string(error.location()), "/$ref");
    SUCCEED();
  }

  EXPECT_EQ(entries.size(), 0);

  // Note we still transform the schema to leave it in its broken state for
  // logging, etc
  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/definitions/foo",
    "$defs": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_not_fixed_id) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleRemoveIdentifiers>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com",
    "$ref": "helper",
    "$defs": {
      "helper": {
        "$id": "helper"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  // The reference is now just treated as an external one

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "helper",
    "$defs": {
      "helper": {}
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_not_fixed_anchor) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleRemoveIdentifiers>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#helper",
    "$defs": {
      "helper": {
        "$anchor": "helper"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  // The reference is now just treated as an external one

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#helper",
    "$defs": {
      "helper": {}
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_fixed_1) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsWithRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/definitions/foo",
    "definitions": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "#/$defs/foo",
    "$defs": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_fixed_2) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsWithRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com",
    "$ref": "https://example.com#/definitions/foo",
    "definitions": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com",
    "$ref": "https://example.com#/$defs/foo",
    "$defs": {
      "foo": {
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_fixed_3) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsWithRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com",
    "$ref": "foo",
    "definitions": {
      "foo": {
        "$id": "foo",
        "type": "string"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com",
    "$ref": "foo",
    "$defs": {
      "foo": {
        "$id": "foo",
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_fixed_4) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsWithRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com",
    "$ref": "#foo",
    "definitions": {
      "foo": {
        "$anchor": "foo",
        "type": "string"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com",
    "$ref": "#foo",
    "$defs": {
      "foo": {
        "$anchor": "foo",
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_fixed_5) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsWithRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#/properties/definitions/definitions/helper"
      },
      "definitions": {
        "definitions": {
          "helper": {
            "type": "string"
          }
        }
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#/properties/definitions/$defs/helper"
      },
      "definitions": {
        "$defs": {
          "helper": {
            "type": "string"
          }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_fixed_6) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsWithRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/here",
    "properties": {
      "foo": {
        "$id": "nested",
        "$ref": "here#/properties/bar/definitions/helper"
      },
      "bar": {
        "definitions": {
          "helper": {
            "type": "string"
          }
        }
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/here",
    "properties": {
      "foo": {
        "$id": "nested",
        "$ref": "here#/properties/bar/$defs/helper"
      },
      "bar": {
        "$defs": {
          "helper": {
            "type": "string"
          }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, rereference_fixed_7) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleDefinitionsToDefsWithRereference>();

  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/here",
    "allOf": [
      { "$ref": "#/definitions/outer" },
      {
        "properties": {
          "foo": {
            "$id": "nested",
            "$ref": "#/definitions/helper",
            "definitions": {
              "inner": {
                "type": "string"
              }
            },
            "$defs": {}
          }
        }
      }
    ],
    "definitions": {
      "outer": {
        "type": "string"
      }
    }
  })JSON");

  TestTransformTraces entries;
  const bool result =
      bundle.apply(document, sourcemeta::core::schema_official_walker,
                   sourcemeta::core::schema_official_resolver,
                   transformer_callback_trace(entries));

  EXPECT_TRUE(result);
  EXPECT_EQ(entries.size(), 0);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/here",
    "allOf": [
      { "$ref": "#/$defs/outer" },
      {
        "properties": {
          "foo": {
            "$id": "nested",
            "$ref": "#/definitions/helper",
            "definitions": {
              "inner": {
                "type": "string"
              }
            },
            "$defs": {}
          }
        }
      }
    ],
    "$defs": {
      "outer": {
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_transformer, iterators) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();
  bundle.add<ExampleRule3>();

  std::set<std::string> rules;
  for (const auto &entry : bundle) {
    rules.insert(entry.first);
  }

  EXPECT_EQ(rules.size(), 3);
  EXPECT_TRUE(rules.contains("example_rule_1"));
  EXPECT_TRUE(rules.contains("example_rule_2"));
  EXPECT_TRUE(rules.contains("example_rule_3"));
}
