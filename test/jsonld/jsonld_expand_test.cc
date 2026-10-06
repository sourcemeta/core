#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>

#include <optional> // std::optional, std::nullopt
#include <string>   // std::string

TEST(deeply_nested_input_is_rejected) {
  const auto input{sourcemeta::core::parse_json(std::string(1500, '[') +
                                                std::string(1500, ']'))};
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_EQ(std::string{error.what()}, "Maximum nesting depth exceeded");
  }
}

TEST(empty_object) {
  const auto input = sourcemeta::core::parse_json("{}");
  const auto expected = sourcemeta::core::parse_json("[]");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(absolute_iri_property_with_string_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": "bar"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [ { "@value": "bar" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(node_with_id_and_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/a",
    "http://example.com/foo": "bar"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/foo": [ { "@value": "bar" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_is_made_an_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/a",
    "@type": "http://example.com/T",
    "http://example.com/foo": "bar"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "@type": [ "http://example.com/T" ],
      "http://example.com/foo": [ { "@value": "bar" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(multiple_values_preserve_order) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": [ "a", "b" ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [ { "@value": "a" }, { "@value": "b" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(numeric_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": 1
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [ { "@value": 1 } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(boolean_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": true
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [ { "@value": true } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(undefined_term_without_context_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "foo": "bar"
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_map_null_value_contributes_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "p": { "@id": "http://example.com/p", "@container": "@type" }
    },
    "p": { "http://example.com/T": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(graph_map_null_value_contributes_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "p": {
        "@id": "http://example.com/p",
        "@container": [ "@graph", "@index" ]
      }
    },
    "p": { "idx": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(graph_container_null_value_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "p": { "@id": "http://example.com/p", "@container": "@graph" }
    },
    "p": null
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(graph_container_value_expanding_to_null_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "p": { "@id": "http://example.com/p", "@container": "@graph" }
    },
    "p": { "@value": null }
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(graph_container_empty_array_contributes_an_empty_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "p": { "@id": "http://example.com/p", "@container": "@graph" }
    },
    "p": [ null ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(reordered_container_does_not_redefine_a_protected_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@version": 1.1, "@protected": true,
        "t": { "@id": "http://example.com/t",
               "@container": [ "@set", "@index" ] } },
      { "@version": 1.1,
        "t": { "@id": "http://example.com/t",
               "@container": [ "@index", "@set" ] } }
    ],
    "t": { "k": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/t": [ { "@value": "v", "@index": "k" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(index_map_null_value_contributes_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@index" }
    },
    "p": { "idx": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(empty_index_map_contributes_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@index" }
    },
    "p": { }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(index_map_null_value_among_others_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@index" }
    },
    "p": { "a": null, "b": "value" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "value", "@index": "b" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(id_map_null_value_contributes_nothing) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@id" }
    },
    "p": { "http://example.com/a": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(free_floating_list_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@list": [ "foo" ], "@id": "http://example.com/bar"
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(free_floating_list_in_graph_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@graph": { "@list": [ "foo" ], "@id": "http://example.com/bar" }
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(term_maps_to_iri) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "name": "http://example.com/name" },
    "name": "John"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/name": [ { "@value": "John" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(vocabulary_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "http://example.com/" },
    "name": "John"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/name": [ { "@value": "John" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(compact_iri_via_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "ex": "http://example.com/" },
    "ex:name": "John"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/name": [ { "@value": "John" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_coercion_to_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "knows": { "@id": "http://example.com/knows", "@type": "@id" }
    },
    "knows": "http://example.com/jane"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/knows": [ { "@id": "http://example.com/jane" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_coercion_to_datatype) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "born": {
        "@id": "http://example.com/born",
        "@type": "http://www.w3.org/2001/XMLSchema#date"
      }
    },
    "born": "1990-01-01"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/born": [
        {
          "@value": "1990-01-01",
          "@type": "http://www.w3.org/2001/XMLSchema#date"
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(relative_typed_value_datatype_resolves_against_the_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@type": "relative" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "x", "@type": "https://example.com/relative" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://example.com/"),
            expected);
}

TEST(fragment_typed_value_datatype) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": {
      "@value": "x",
      "@type": "http://example.com/t#dt"
    }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "x", "@type": "http://example.com/t#dt" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(exponent_version_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1e0, "p": "http://example.com/p" },
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(default_language) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@language": "en", "name": "http://example.com/name" },
    "name": "John"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/name": [ { "@value": "John", "@language": "en" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(list_keyword) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": { "@list": [ "a", "b" ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [
        { "@list": [ { "@value": "a" }, { "@value": "b" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(direction_dropped_in_json_ld_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@value": "v", "@direction": "rtl" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(included_dropped_in_json_ld_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": "v",
    "@included": { "@id": "http://example.com/other" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(nest_term_whose_scoped_context_redefines_itself) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "nest": {
        "@id": "@nest",
        "@context": { "nest": { "@id": "http://example.com/nest" } }
      }
    },
    "nest": { "http://example.com/foo": "bar" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "bar" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(json_typed_value_in_list_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "e": {
        "@id": "http://example.com/e",
        "@type": "@json",
        "@container": "@list"
      }
    },
    "e": 42
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/e": [
        { "@list": [ { "@value": 42, "@type": "@json" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(id_typed_keyword_form_value_expands_to_null) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "http://example.com/p", "@type": "@id" } },
    "p": "@foo"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@id": null } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(non_expandable_type_value_is_omitted) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/n",
    "@type": "@foo",
    "http://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/n",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(graph_value_expanding_to_null_yields_no_element) {
  const auto input = sourcemeta::core::parse_json(R"({ "@graph": "scalar" })");
  const auto expected = sourcemeta::core::parse_json("[]");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(base_in_remote_context_is_ignored) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/remote-base") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "@base": "http://remote.example/" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/remote-base",
    "@id": "relative-node",
    "http://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://doc.example/relative-node",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(
      sourcemeta::core::jsonld_expand(input, "http://doc.example/", resolver),
      expected);
}

TEST(language_map_direction_uses_the_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@direction": "rtl"
      }
    },
    "p": { "en": "hello" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "hello", "@language": "en", "@direction": "rtl" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(reverse_term_with_set_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "rev": { "@reverse": "http://example.com/x", "@container": "@set" }
    },
    "@id": "http://example.com/subject",
    "rev": { "@id": "http://example.com/object" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/subject",
      "@reverse": {
        "http://example.com/x": [ { "@id": "http://example.com/object" } ]
      }
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(reverse_term_with_null_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "rev": { "@reverse": "http://example.com/x", "@container": null }
    },
    "@id": "http://example.com/subject",
    "rev": { "@id": "http://example.com/object" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/subject",
      "@reverse": {
        "http://example.com/x": [ { "@id": "http://example.com/object" } ]
      }
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(reverse_term_ignores_prefix_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "rev": { "@reverse": "http://example.com/x", "@prefix": "not-a-boolean" }
    },
    "@id": "http://example.com/subject",
    "rev": { "@id": "http://example.com/object" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/subject",
      "@reverse": {
        "http://example.com/x": [ { "@id": "http://example.com/object" } ]
      }
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_redefinition_with_set_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@container": "@set" } },
    "@type": "http://example.com/Type"
  })");

  const auto result{sourcemeta::core::jsonld_expand(input)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).defines("@type"));
  EXPECT_TRUE(result.at(0).at("@type").is_array());
  EXPECT_EQ(result.at(0).at("@type").size(), 1);
  EXPECT_EQ(result.at(0).at("@type").at(0).to_string(),
            "http://example.com/Type");
}

TEST(term_with_slash_expands_against_vocabulary) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "http://vocab.example/",
      "a/b": { "@type": "@id" }
    },
    "a/b": "http://example.com/x"
  })");

  const auto result{sourcemeta::core::jsonld_expand(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON([
    {
      "http://vocab.example/a/b": [
        { "@id": "http://example.com/x" }
      ]
    }
  ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(compact_iri_term_with_unresolvable_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "ex:suffix": { "@type": "@id" }
    },
    "ex:suffix": "http://example.com/x"
  })");

  const auto result{sourcemeta::core::jsonld_expand(input)};
  const auto expected{sourcemeta::core::parse_json(R"JSON([
    {
      "ex:suffix": [
        { "@id": "http://example.com/x" }
      ]
    }
  ])JSON")};
  EXPECT_EQ(result, expected);
}

TEST(type_redefinition_protected_with_matching_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@type": { "@container": "@set", "@protected": true } },
      { "@type": { "@container": "@set" } }
    ],
    "@type": "http://example.com/T"
  })");

  const auto result{sourcemeta::core::jsonld_expand(input)};
  const auto expected{sourcemeta::core::parse_json(R"([
    { "@type": [ "http://example.com/T" ] }
  ])")};
  EXPECT_EQ(result, expected);
}

TEST(simple_term_with_keyword_form_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "http://example.com/", "ignored": "@nest" },
    "ignored": { "http://example.com/foo": "bar" }
  })");

  const auto result{sourcemeta::core::jsonld_expand(input)};
  const auto expected{sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "bar" } ] }
  ])")};
  EXPECT_EQ(result, expected);
}

TEST(self_referential_compact_term_via_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "ex": "http://example.com/",
      "ex:name": "ex:name"
    },
    "ex:name": "value"
  })");

  const auto result{sourcemeta::core::jsonld_expand(input)};
  const auto expected{sourcemeta::core::parse_json(R"([
    { "http://example.com/name": [ { "@value": "value" } ] }
  ])")};
  EXPECT_EQ(result, expected);
}

TEST(type_array_with_non_string_item_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"({ "@type": [ 123 ] })");
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid type value");
  }
}

TEST(top_level_non_string_direction_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"({ "@direction": 123 })");
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid base direction");
  }
}

TEST(nest_array_item_not_an_object_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "http://example.com/" },
    "@nest": [ "x" ]
  })");
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid @nest value");
  }
}

TEST(nest_array_item_value_object_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "http://example.com/" },
    "@nest": [ { "@value": "x" } ]
  })");
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid @nest value");
  }
}

TEST(term_container_array_with_non_string_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "t": { "@id": "http://ex/t", "@container": [ 123 ] } },
    "t": "x"
  })");
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid container mapping");
  }
}

TEST(term_container_array_with_invalid_container_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "t": { "@id": "http://ex/t", "@container": [ "@bogus" ] } },
    "t": "x"
  })");
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid container mapping");
  }
}

TEST(term_direction_non_string_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "t": { "@id": "http://ex/t", "@direction": 123 } },
    "t": "x"
  })");
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid base direction");
  }
}

TEST(term_nest_non_string_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "t": { "@id": "http://ex/t", "@nest": 123 } },
    "t": "x"
  })");
  try {
    const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid @nest value");
  }
}

// JSON-LD 1.1 Expansion calls a reverse property inside a reverse property map
// "properties that are reversed twice", and merges those entries forward into
// the result rather than leaving them reversed
TEST(reverse_term_inside_a_reverse_map_is_reversed_twice) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "rev": { "@reverse": "http://example.com/x" }
    },
    "@id": "http://example.com/subject",
    "@reverse": { "rev": { "@id": "http://example.com/object" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/subject",
      "http://example.com/x": [ { "@id": "http://example.com/object" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

// The only keyword a reverse property map may carry is `@context`, which is
// read as a scoped context rather than rejected
TEST(context_inside_a_reverse_map_is_allowed) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/subject",
    "@reverse": {
      "@context": { "p": "http://example.com/p" },
      "p": { "@id": "http://example.com/object" }
    }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/subject",
      "@reverse": {
        "http://example.com/p": [ { "@id": "http://example.com/object" } ]
      }
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(self_referential_scoped_context_is_valid) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/self-scoped") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "p": {
            "@id": "http://example.com/p",
            "@context": "https://example.com/self-scoped"
          }
        }
      })");
    }

    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/self-scoped",
    "p": { "http://example.com/q": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "http://example.com/q": [ { "@value": "v" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
}

TEST(null_id_term_drops_the_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": null } },
    "a": "x",
    "https://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(scoped_context_is_invisible_to_language_map_keys) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@context": { "none": "@none", "@direction": "rtl" }
      }
    },
    "p": { "en": "y" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "y", "@language": "en" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_type_scoped_context_restores_the_outer_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "https://example.com/",
      "T": { "@id": "https://example.com/T", "@context": null }
    },
    "@type": "T",
    "q": "drop",
    "https://example.com/child": { "p": "keep" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "https://example.com/T" ],
      "https://example.com/child": [
        { "https://example.com/p": [ { "@value": "keep" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(nonpropagating_scoped_context_with_a_local_override) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": { "a": "http://example.com/scoped-a", "@propagate": false }
      }
    },
    "p": { "@context": { "a": "http://example.com/local-a" }, "a": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "http://example.com/local-a": [ { "@value": "v" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(nonpropagating_scoped_context_over_array_values) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "http://example.com/",
      "p": {
        "@id": "http://example.com/p",
        "@context": { "q": "http://example.com/scoped-q", "@propagate": false }
      }
    },
    "p": [ { "q": { "q": "deep" } } ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "http://example.com/scoped-q": [
            { "http://example.com/q": [ { "@value": "deep" } ] }
          ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_map_string_value_uses_the_property_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "@vocab": "http://example.org/ns/",
      "@base": "http://example.org/base/",
      "foo": {
        "@container": "@type",
        "@context": { "@base": "http://scoped.example/" }
      }
    },
    "foo": { "bar": "baz" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.org/ns/foo": [
        {
          "@id": "http://scoped.example/baz",
          "@type": [ "http://example.org/ns/bar" ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(index_map_value_keeps_the_type_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "T": {
        "@id": "https://example.com/T",
        "@context": { "a": "https://example.com/type-a" }
      },
      "p": { "@id": "https://example.com/p", "@container": "@index" }
    },
    "@type": "T",
    "p": { "i": { "a": "v" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "https://example.com/T" ],
      "https://example.com/p": [
        {
          "https://example.com/type-a": [ { "@value": "v" } ],
          "@index": "i"
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_map_string_value_uses_a_scoped_vocabulary_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "@vocab": "https://example.com/",
      "p": {
        "@container": "@type",
        "@type": "@vocab",
        "@context": { "x": "https://example.com/scoped" }
      }
    },
    "p": { "https://example.com/T": "x" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        {
          "@id": "https://example.com/scoped",
          "@type": [ "https://example.com/T" ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(index_map_array_value_keeps_the_type_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "T": {
        "@id": "https://example.com/T",
        "@context": { "a": "https://example.com/type-a" }
      },
      "p": { "@id": "https://example.com/p", "@container": "@index" }
    },
    "@type": "T",
    "p": { "i": [ { "a": "v" } ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "https://example.com/T" ],
      "https://example.com/p": [
        {
          "https://example.com/type-a": [ { "@value": "v" } ],
          "@index": "i"
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(context_direction_applies_to_a_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@direction": "rtl", "p": "https://example.com/p" },
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/p": [ { "@value": "v", "@direction": "rtl" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(reverse_term_with_a_keyword_value_is_ignored) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "@id" } },
    "http://example.com/q": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/q": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(reverse_term_with_a_keyword_form_value_is_ignored) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "@foo" } },
    "http://example.com/q": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/q": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}
