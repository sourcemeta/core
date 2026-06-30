#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>

TEST(nested_anonymous_node_gets_blank_node_identifier) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [
        { "http://example.com/q": [ { "@value": "x" } ] }
      ]
    }
  ])");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "_:b0",
      "http://example.com/q": [ { "@value": "x" } ]
    },
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [ { "@id": "_:b0" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_flatten(input), expected);
}

TEST(named_graph_is_folded_into_graph_entry) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/g",
      "@graph": [
        {
          "@id": "http://example.com/a",
          "http://example.com/p": [ { "@value": "x" } ]
        }
      ]
    }
  ])");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/g",
      "@graph": [
        {
          "@id": "http://example.com/a",
          "http://example.com/p": [ { "@value": "x" } ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_flatten(input), expected);
}

TEST(reverse_property_becomes_forward_edge) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "@reverse": {
        "http://example.com/p": [ { "@id": "http://example.com/b" } ]
      }
    }
  ])");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/b",
      "http://example.com/p": [ { "@id": "http://example.com/a" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_flatten(input), expected);
}

TEST(list_value_is_preserved) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [
        { "@list": [ { "@value": "x" }, { "@value": "y" } ] }
      ]
    }
  ])");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [
        { "@list": [ { "@value": "x" }, { "@value": "y" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_flatten(input), expected);
}

TEST(set_object_items_are_unwrapped) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [
        { "@set": [ { "@value": "x" }, { "@value": "y" } ] }
      ]
    }
  ])");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [ { "@value": "x" }, { "@value": "y" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_flatten(input), expected);
}

TEST(set_object_wrapping_anonymous_node) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [
        { "@set": [ { "http://example.com/q": [ { "@value": "z" } ] } ] }
      ]
    }
  ])");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "_:b0",
      "http://example.com/q": [ { "@value": "z" } ]
    },
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [ { "@id": "_:b0" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_flatten(input), expected);
}

TEST(conflicting_indexes_are_rejected) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "@id": "http://example.com/a", "@index": "1" },
    { "@id": "http://example.com/a", "@index": "2" }
  ])");

  try {
    sourcemeta::core::jsonld_flatten(input);
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_EQ(sourcemeta::core::to_string(error.pointer()), "");
  }
}

TEST(flatten_and_compact_against_context) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/p": [
        { "http://example.com/q": [ { "@value": "x" } ] }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@vocab": "http://example.com/"
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "@graph": [
      { "@id": "_:b0", "q": "x" },
      { "@id": "http://example.com/a", "p": { "@id": "_:b0" } }
    ],
    "@context": { "@vocab": "http://example.com/" }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_flatten(input, context), expected);
}
