#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>

#include <optional> // std::optional, std::nullopt

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

TEST(language_map_direction_uses_property_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@context": { "@direction": "rtl" }
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
