#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>

#include <cstddef>  // std::size_t
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

TEST(remote_context_dereferenced_once) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(
        R"({ "@context": { "name": "https://example.com/name" } })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/context.jsonld",
      "https://example.com/context.jsonld"
    ],
    "name": "Juan"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/name": [ { "@value": "Juan" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  EXPECT_EQ(invocations, 1);
}

TEST(remote_context_reuse_is_scoped_to_one_expansion) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    if (invocations == 1) {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "name": "https://example.com/first" } })");
    }

    return sourcemeta::core::parse_json(
        R"({ "@context": { "name": "https://example.com/second" } })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/context.jsonld",
    "name": "Juan"
  })");

  const auto first = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/first": [ { "@value": "Juan" } ]
    }
  ])");

  const auto second = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/second": [ { "@value": "Juan" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), first);
  EXPECT_EQ(invocations, 1);
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), second);
  EXPECT_EQ(invocations, 2);
}

TEST(index_map_none_alias_suppresses_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "none": "@none",
      "p": { "@id": "https://example.com/p", "@container": "@index" }
    },
    "p": { "none": { "@id": "https://example.com/item" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [ { "@id": "https://example.com/item" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(property_valued_index_map_none_alias_suppresses_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "none": "@none",
      "label": "https://example.com/label",
      "p": {
        "@id": "https://example.com/p",
        "@container": "@index",
        "@index": "label"
      }
    },
    "p": { "none": { "@id": "https://example.com/item" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [ { "@id": "https://example.com/item" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_container_defaults_to_identifier_coercion) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@type" }
    },
    "p": "https://example.com/node"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [ { "@id": "https://example.com/node" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_container_keeps_an_explicit_vocab_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "https://example.com/",
      "p": {
        "@id": "https://example.com/p",
        "@container": "@type",
        "@type": "@vocab"
      }
    },
    "p": "node"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [ { "@id": "https://example.com/node" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(protected_only_type_keyword_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@protected": true } },
    "@type": "https://example.com/Type",
    "https://example.com/p": "value"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "https://example.com/Type" ],
      "https://example.com/p": [ { "@value": "value" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(remote_context_reused_across_a_scoped_reference) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/shared") {
      invocations += 1;
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "https://example.com/p" } })");
    }

    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/shared",
      { "@vocab": "https://example.com/vocab/" },
      "https://example.com/shared"
    ],
    "p": { "@context": "https://example.com/shared", "p": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "https://example.com/p": [ { "@value": "v" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  EXPECT_EQ(invocations, 1);
}

TEST(protected_type_container_redefinition_with_explicit_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      {
        "@protected": true,
        "p": { "@id": "https://example.com/p", "@container": "@type" }
      },
      {
        "p": {
          "@id": "https://example.com/p",
          "@container": "@type",
          "@type": "@id"
        }
      }
    ],
    "p": { "https://example.com/T": "https://example.com/node" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        {
          "@id": "https://example.com/node",
          "@type": [ "https://example.com/T" ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(nested_raw_array_in_explicit_list_flattens) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": { "@list": [ [ 1 ] ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [ { "@list": [ { "@value": 1 } ] } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(nested_list_object_in_explicit_list_is_preserved) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": { "@list": [ { "@list": [ 1 ] } ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@list": [ { "@list": [ { "@value": 1 } ] } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(nested_array_under_list_container_is_wrapped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@list" }
    },
    "p": [ [ 1 ] ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@list": [ { "@list": [ { "@value": 1 } ] } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_keyword_definition_explicit_false_overrides_context_protected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@protected": true, "@type": { "@protected": false } },
      null
    ],
    "https://example.com/p": "value"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/p": [ { "@value": "value" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(included_identifier_only_node) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v",
    "@included": { "@id": "http://example.com/n" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "v" } ],
      "@included": [ { "@id": "http://example.com/n" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(included_identifier_only_node_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v",
    "@included": [ { "@id": "http://example.com/n" } ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "v" } ],
      "@included": [ { "@id": "http://example.com/n" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(decimal_version_value) {
  auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": "v"
  })");
  input.at("@context")
      .assign("@version",
              sourcemeta::core::JSON{sourcemeta::core::Decimal{"1.1"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_id_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@id" }
    },
    "p": {
      "https://example.com/a": null,
      "https://example.com/b": {}
    }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@id": "https://example.com/b" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_type_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@type" }
    },
    "p": { "http://example.com/T": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_graph_index_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": [ "@graph", "@index" ]
      }
    },
    "p": { "i": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(protected_redefinition_with_reordered_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      {
        "@protected": true,
        "p": {
          "@id": "http://example.com/p",
          "@container": [ "@set", "@index" ]
        }
      },
      {
        "p": {
          "@id": "http://example.com/p",
          "@container": [ "@index", "@set" ]
        }
      }
    ],
    "p": { "a": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "v", "@index": "a" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_index_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@index" }
    },
    "p": { "a": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(relative_context_resolved_against_base) {
  sourcemeta::core::JSON::String resolved_identifier;
  const sourcemeta::core::JSONLDResolver resolver =
      [&resolved_identifier](
          const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    resolved_identifier = identifier;
    if (identifier == "https://example.com/dir/context.jsonld") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "context.jsonld",
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "https://example.com/dir/document.jsonld", resolver),
            expected);
  EXPECT_EQ(resolved_identifier, "https://example.com/dir/context.jsonld");
}

TEST(none_alias_in_language_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "none": "@none",
      "@direction": "rtl",
      "p": { "@id": "http://example.com/p", "@container": "@language" }
    },
    "p": { "en": "y", "none": "x" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "y", "@language": "en", "@direction": "rtl" },
        { "@value": "x", "@direction": "rtl" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(list_object_with_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@index": "i" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@list": [ { "@value": "a" } ], "@index": "i" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(unicode_iri_term_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "café": "https://example.com/café" },
    "café": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/café": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(unicode_relative_id_resolved_against_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "café",
    "http://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/dir/café",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://example.com/dir/"),
            expected);
}

TEST(unicode_remote_context_reference) {
  sourcemeta::core::JSON::String resolved_identifier;
  const sourcemeta::core::JSONLDResolver resolver =
      [&resolved_identifier](
          const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    resolved_identifier = identifier;
    if (identifier == "https://example.com/ctx-café.jsonld") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "ctx-café.jsonld",
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(
      sourcemeta::core::jsonld_expand(input, "https://example.com/", resolver),
      expected);
  EXPECT_EQ(resolved_identifier, "https://example.com/ctx-café.jsonld");
}

TEST(local_scoped_context_base_applies) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": { "@base": "https://scoped.example/" }
      }
    },
    "p": { "@id": "child" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@id": "https://scoped.example/child" }
      ]
    }
  ])");

  EXPECT_EQ(
      sourcemeta::core::jsonld_expand(input, "https://local.example/dir/"),
      expected);
}

TEST(repeated_sibling_remote_references) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/sibling") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/sibling",
      "https://example.com/sibling"
    ],
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
}

TEST(local_base_after_remote_context) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/sibling") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/sibling",
      { "@base": "https://local.example/" }
    ],
    "@id": "relative-node",
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://local.example/relative-node",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
}

TEST(language_map_direction_uses_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@direction": "ltr",
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

TEST(scoped_alias_invisible_to_language_map_keys) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@context": { "none": "@none" }
      }
    },
    "p": { "none": "x" }
  })");

  // Language map keys read the element's own active context, so an alias
  // defined only in the property-scoped context does not strip @language
  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "x", "@language": "none" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(language_map_term_direction_wins_over_scoped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@direction": "rtl",
        "@context": { "@direction": "ltr" }
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

TEST(reverse_term_null_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@reverse": "http://example.org/p", "@container": null }
    },
    "p": { "@id": "http://example.org/o" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@reverse": {
        "http://example.org/p": [ { "@id": "http://example.org/o" } ]
      }
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(reverse_term_null_container_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@reverse": "http://example.org/p", "@container": null }
    },
    "p": { "@id": "http://example.org/o" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@reverse": {
        "http://example.org/p": [ { "@id": "http://example.org/o" } ]
      }
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(expansion_context_base_applies) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "child",
    "http://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://ctx.example/dir/child",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  const auto bare = sourcemeta::core::parse_json(
      R"({ "@base": "https://ctx.example/dir/" })");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, bare), expected);

  const auto wrapped = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": "https://ctx.example/dir/" } })");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, wrapped), expected);

  const auto array = sourcemeta::core::parse_json(
      R"([ { "@base": "https://ctx.example/dir/" } ])");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, array), expected);
}

TEST(document_context_overrides_expansion_context_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@base": "https://doc.example/" },
    "@id": "child",
    "http://example.com/p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(
      R"({ "@base": "https://ctx.example/dir/" })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://doc.example/child",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, context), expected);
}

TEST(object_definition_is_not_a_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "https://example.com/ns/" } },
    "p:x": "v",
    "https://example.com/q": { "@id": "p:y" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "p:x": [ { "@value": "v" } ],
      "https://example.com/q": [ { "@id": "p:y" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(object_definition_is_not_a_prefix_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "https://example.com/ns/" } },
    "p:x": "v",
    "https://example.com/q": { "@id": "p:y" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "p:x": [ { "@value": "v" } ],
      "https://example.com/q": [ { "@id": "p:y" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(simple_definition_is_a_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "https://example.com/ns/" },
    "p:x": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/ns/x": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(empty_type_array_preserved) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": [],
    "https://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [],
      "https://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input,
                                            sourcemeta::core::parse_json("{}")),
            expected);
}

TEST(empty_type_array_preserved_through_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "type": "@type" },
    "type": [],
    "https://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [],
      "https://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(remote_context_loaded_once) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/shared") {
      invocations += 1;
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/shared",
      { "@vocab": "http://example.com/vocab/" },
      "https://example.com/shared"
    ],
    "p": { "@context": "https://example.com/shared", "p": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "http://example.com/p": [ { "@value": "v" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  // A previously dereferenced context is never dereferenced again within one
  // expansion
  EXPECT_EQ(invocations, 1);
}

TEST(remote_context_reuse_keeps_first_document) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    if (invocations == 1) {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/first" } })");
    }
    return sourcemeta::core::parse_json(
        R"({ "@context": { "p": "http://example.com/second" } })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/stateful",
      {},
      "https://example.com/stateful"
    ],
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/first": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  EXPECT_EQ(invocations, 1);

  // A later expansion call starts fresh and observes the new document
  const auto second_expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/second": [ { "@value": "v" } ] }
  ])");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver),
            second_expected);
}

TEST(index_map_none_alias_suppresses_metadata) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "none": "@none",
      "p": { "@id": "https://example.com/p", "@container": "@index" }
    },
    "p": { "none": "v", "other": "w" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@value": "v" },
        { "@value": "w", "@index": "other" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(property_valued_index_map_none_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "none": "@none",
      "p": {
        "@id": "https://example.com/p",
        "@container": "@index",
        "@index": "https://example.com/prop"
      }
    },
    "p": { "none": { "@id": "https://example.com/node" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@id": "https://example.com/node" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_container_explicit_vocab_mapping_is_kept) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "https://example.com/vocab/",
      "p": {
        "@id": "https://example.com/p",
        "@container": "@type",
        "@type": "@vocab"
      }
    },
    "p": "node"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@id": "https://example.com/vocab/node" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(scalar_scoped_context_overrides_protected_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "x": { "@id": "urn:x", "@protected": true } },
      { "p": { "@id": "urn:p", "@context": { "x": "urn:y" } } }
    ],
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);

  const auto array_input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "x": { "@id": "urn:x", "@protected": true } },
      { "p": { "@id": "urn:p", "@context": { "x": "urn:y" } } }
    ],
    "p": [ "v" ]
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(array_input), expected);
}

TEST(default_framing_keyword_type_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": "@default",
    "https://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(default_framing_keyword_as_coerced_identifier) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@type": "@id" }
    },
    "p": "@default"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/p": [ { "@id": null } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(graph_container_with_single_map_kind_and_set) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "https://example.com/p",
        "@container": [ "@graph", "@index", "@set" ]
      }
    },
    "p": { "i": { "https://example.com/q": "v" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        {
          "@graph": [ { "https://example.com/q": [ { "@value": "v" } ] } ],
          "@index": "i"
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(unprotected_context_nullification_mid_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "a": "urn:a" }, null ],
    "urn:p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_scoped_precedence_follows_input_keys) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "A": { "@id": "urn:A", "@context": { "p": "urn:fromA" } },
      "Z": { "@id": "urn:Z", "@context": { "p": "urn:fromZ" } },
      "t": "@type"
    },
    "@type": "Z",
    "t": "A",
    "p": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "urn:Z", "urn:A" ],
      "urn:fromA": [ { "@value": "x" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(type_scoped_precedence_sorts_values_within_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "A": { "@id": "urn:A", "@context": { "p": "urn:fromA" } },
      "Z": { "@id": "urn:Z", "@context": { "p": "urn:fromZ" } }
    },
    "@type": [ "Z", "A" ],
    "p": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "urn:Z", "urn:A" ],
      "urn:fromZ": [ { "@value": "x" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(term_direction_accepted_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@direction": "rtl" } },
    "p": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "x", "@direction": "rtl" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(explicit_unprotected_type_definition_is_redefinable) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@protected": true, "@type": { "@protected": false } },
      { "@type": { "@container": "@set" } }
    ],
    "@type": "urn:T"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@type": [ "urn:T" ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_set_drops_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@set": null },
    "urn:q": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:q": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(empty_set_keeps_empty_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@set": [] },
    "urn:q": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:p": [],
      "urn:q": [ { "@value": "x" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(error_message_survives_buffer_destruction_and_copy) {
  std::optional<sourcemeta::core::JSONLDError> copy;

  {
    std::string code{
        "A custom error code longer than small string optimization"};
    const sourcemeta::core::JSONLDError error{
        code.c_str(), sourcemeta::core::Pointer{"where"}};
    copy.emplace(error);
  }

  EXPECT_STREQ(copy->what(),
               "A custom error code longer than small string optimization");
  EXPECT_EQ(sourcemeta::core::to_string(copy->pointer()), "/where");
}

TEST(free_floating_json_literal_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": "@json",
    "@value": 42
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(free_floating_json_literal_in_graph_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@graph": [ { "@type": "@json", "@value": 42 } ]
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(json_literal_under_property_is_preserved) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@type": "@json", "@value": [ 2, 1 ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:p": [ { "@type": "@json", "@value": [ 2, 1 ] } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_mapped_type_map_key_preserves_node) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@type" },
      "NoType": null
    },
    "p": { "NoType": { "@id": "urn:n" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@id": "urn:n" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(included_nested_array_member_expands) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": [ [ { "urn:p": "v" } ] ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@included": [ { "urn:p": [ { "@value": "v" } ] } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(included_set_member_expands) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": { "@set": [ { "urn:p": "v" } ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@included": [ { "urn:p": [ { "@value": "v" } ] } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(query_distinct_contexts_resolve_separately) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    if (identifier == "https://example.com/ctx?v=1") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "urn:first" } })");
    }
    if (identifier == "https://example.com/ctx?v=2") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "urn:second" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/ctx?v=1",
      "https://example.com/ctx?v=2"
    ],
    "p": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:second": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  EXPECT_EQ(invocations, 2);
}

TEST(top_level_graph_object_unwraps) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@graph": [ { "urn:p": "x" } ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(top_level_array_preserves_graph_object) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "@graph": [ { "urn:p": "x" } ] }
  ])");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@graph": [ { "urn:p": [ { "@value": "x" } ] } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(included_named_graph_member_expands) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": { "@id": "urn:g", "@graph": [ { "urn:p": "v" } ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@included": [
        {
          "@id": "urn:g",
          "@graph": [ { "urn:p": [ { "@value": "v" } ] } ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(resolved_property_valued_index_control) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "urn:",
      "p": { "@id": "urn:p", "@container": "@index", "@index": "relative" }
    },
    "p": { "x": { "@id": "urn:n" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:p": [
        { "@id": "urn:n", "urn:relative": [ { "@value": "x" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(nest_expands_in_1_1) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:q": "y",
    "@nest": { "urn:p": "x" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:q": [ { "@value": "y" } ],
      "urn:p": [ { "@value": "x" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(included_empty_set_member) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": { "@set": [] },
    "urn:p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@included": [],
      "urn:p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(included_set_alias_valid_members) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": {
      "@context": { "s": "@set" },
      "s": [ { "@id": "urn:n" } ]
    },
    "urn:p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@included": [ { "@id": "urn:n" } ],
      "urn:p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(inputs_are_unchanged) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@container": "@list" } },
    "p": [ "a", [ "b" ] ],
    "urn:q": { "@value": 1 }
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "@vocab": "urn:"
  })");
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  const auto input_copy{input};
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  const auto context_copy{context};

  sourcemeta::core::jsonld_expand(input, context);

  EXPECT_EQ(input, input_copy);
  EXPECT_EQ(context, context_copy);
}

TEST(explicit_nested_list_accepted_in_1_1) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@list": [ { "@set": [ { "@list": [ "x" ] } ] } ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:p": [
        { "@list": [ { "@list": [ { "@value": "x" } ] } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(external_base_keeps_document_context_resolution) {
  std::vector<sourcemeta::core::JSON::String> requests;
  const sourcemeta::core::JSONLDResolver resolver =
      [&requests](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    requests.emplace_back(identifier);
    if (identifier == "https://document.example/dir/ctx.jsonld") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "name": "urn:name" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "ctx.jsonld",
    "@id": "node",
    "name": "Alice"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "@base": "https://ids.example/"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://ids.example/node",
      "urn:name": [ { "@value": "Alice" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, context, "https://document.example/dir/input.jsonld",
                resolver),
            expected);
  EXPECT_EQ(requests.size(), 1);
  EXPECT_EQ(requests.front(), "https://document.example/dir/ctx.jsonld");
}

TEST(unknown_only_type_array_retained) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": [ "@foo" ],
    "urn:p": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [],
      "urn:p": [ { "@value": "x" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(unknown_scalar_type_omitted) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": "@foo",
    "urn:p": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(set_container_control) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@container": [ "@set" ] } },
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(keyword_form_id_with_valid_type_ignored) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "@foo", "@type": "@id" } },
    "urn:q": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:q": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_redefined_index_term_drops_metadata) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      {
        "i": "urn:i",
        "p": { "@id": "urn:p", "@container": "@index", "@index": "i" }
      },
      { "i": null }
    ],
    "p": { "x": { "@id": "urn:n" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@id": "urn:n" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(custom_index_term_control) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "i": "urn:i",
      "p": { "@id": "urn:p", "@container": "@index", "@index": "i" }
    },
    "p": { "x": { "@id": "urn:n" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:p": [
        { "@id": "urn:n", "urn:i": [ { "@value": "x" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(null_redefined_index_term_graph_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      {
        "i": "urn:i",
        "p": {
          "@id": "urn:p",
          "@container": [ "@graph", "@index" ],
          "@index": "i"
        }
      },
      { "i": null }
    ],
    "p": { "x": { "@id": "urn:n" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:p": [
        { "@graph": [ { "@id": "urn:n" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(malformed_identifier_preserved_with_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "bad%zz",
    "urn:p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "bad%zz",
      "urn:p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://example.com/", {}),
            expected);
}

TEST(malformed_coerced_identifier_preserved_with_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@type": "@id" } },
    "p": "bad%zz"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@id": "bad%zz" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://example.com/", {}),
            expected);
}

TEST(indexed_set_accepts_index_metadata) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@set": [ "x" ], "@index": "i" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(indexed_set_accepts_index_metadata_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@set": [ "x" ], "@index": "i" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(aliased_indexed_set_accepts_index_metadata) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "s": "@set", "i": "@index" },
    "urn:p": { "s": [ "x" ], "i": "i" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(included_null_array_member_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": "v",
    "@included": [ null ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:p": [ { "@value": "v" } ],
      "@included": []
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

// JSON-LD 1.1 API Section 4.2 steps 16.1 to 16.3 resolve the prefix of a
// compact IRI by defining that prefix's own term first, so a context that
// spells the dependent term before its prefix gives the same result as one that
// spells them the other way round
TEST(context_defines_a_compact_iri_term_before_its_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "ex:foo": { "@container": "@set" },
      "ex": "http://example.com/"
    },
    "ex:foo": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(context_defines_a_compact_iri_term_after_its_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "ex": "http://example.com/",
      "ex:foo": { "@container": "@set" }
    },
    "ex:foo": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

// The same dependency, with the dependent term given as a string rather than as
// a definition object
TEST(context_defines_a_compact_iri_string_term_before_its_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "bar": "ex:foo",
      "ex": "http://example.com/"
    },
    "bar": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

// JSON-LD 1.1 API Section 6.3 step 6 resolves a compact IRI through a term for
// its prefix, and with no such term the value is handed back as it was
// written. The underscore keeps it from reading as a scheme RFC 3987 defines,
// so nothing else could have resolved it either
TEST(context_defines_a_compact_iri_term_whose_prefix_has_no_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "bar": "ex_:foo" },
    "bar": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "ex_:foo": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

// A term that maps to itself and carries a slash is not a prefix of itself, so
// resolving it must stop rather than recur
TEST(context_defines_a_self_referential_term_holding_a_slash) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "foo/bar": "foo/bar" },
    "foo/bar": "x"
  })");

  const auto result{sourcemeta::core::jsonld_expand(input)};
  const auto expected = sourcemeta::core::parse_json("[]");
  EXPECT_EQ(result, expected);
}
