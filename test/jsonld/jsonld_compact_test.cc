#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>

TEST(JSONLD_compact, compact_to_relative_true_relativises_against_base) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.org/a",
      "http://example.com/b": [ { "@id": "http://example.org/c" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "b": "http://example.com/b"
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "http://example.org/", {},
      sourcemeta::core::JSONLDVersion::V1_1, true, true)};

  EXPECT_EQ(result.at("@id").to_string(), "a");
  EXPECT_EQ(result.at("b").at("@id").to_string(), "c");
}

TEST(JSONLD_compact, compact_to_relative_false_keeps_absolute_against_base) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.org/a",
      "http://example.com/b": [ { "@id": "http://example.org/c" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "b": "http://example.com/b"
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "http://example.org/", {},
      sourcemeta::core::JSONLDVersion::V1_1, true, false)};

  EXPECT_EQ(result.at("@id").to_string(), "http://example.org/a");
  EXPECT_EQ(result.at("b").at("@id").to_string(), "http://example.org/c");
}

TEST(JSONLD_compact, id_typed_value_with_index_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/id": [
        { "@id": "http://example.org/x", "@index": "foo" }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "id": { "@id": "http://example.com/id", "@type": "@id" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};

  EXPECT_EQ(result.at("id").at("@id").to_string(), "http://example.org/x");
  EXPECT_EQ(result.at("id").at("@index").to_string(), "foo");
}

TEST(JSONLD_compact, nest_value_not_expanding_to_nest_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@vocab": "http://example.com/",
    "p": { "@nest": "other" },
    "other": "http://example.com/other"
  })");

  EXPECT_THROW(sourcemeta::core::jsonld_compact(input, context),
               sourcemeta::core::JSONLDError);
}

TEST(JSONLD_compact, nest_value_aliasing_nest_nests_the_property) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@vocab": "http://example.com/",
    "p": { "@nest": "nst" },
    "nst": "@nest"
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};

  EXPECT_EQ(result.at("nst").at("p").to_string(), "v");
}

TEST(JSONLD_compact, type_map_remaining_stays_array_without_compact_arrays) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@type": [ "http://example.com/A", "http://example.com/B" ],
          "http://example.com/v": [ { "@value": "x" } ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@vocab": "http://example.com/",
    "p": { "@id": "http://example.com/p", "@container": "@type" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_1, false)};

  const auto &node{result.at("@graph").at(0).at("p").at("A").at(0)};
  EXPECT_TRUE(node.at("@type").is_array());
  EXPECT_EQ(node.at("@type").size(), 1);
  EXPECT_EQ(node.at("@type").at(0).to_string(), "B");
}

TEST(JSONLD_compact, protected_redefinition_with_different_index_expansion) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "x" } ] }
  ])");

  // The two definitions of "t" share the same lexical @index ("idx"), but it
  // expands differently in each, so redefining the protected term is rejected.
  const auto context = sourcemeta::core::parse_json(R"([
    {
      "@version": 1.1,
      "@vocab": "http://example.com/",
      "idx": "http://example.com/a",
      "t": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "idx",
        "@protected": true
      }
    },
    {
      "idx": "http://example.com/b",
      "t": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "idx"
      }
    }
  ])");

  EXPECT_THROW(sourcemeta::core::jsonld_compact(input, context),
               sourcemeta::core::JSONLDError);
}
