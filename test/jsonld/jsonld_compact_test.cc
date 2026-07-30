#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>

TEST(compact_to_relative_true_relativises_against_base) {
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

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "a",
    "b": { "@id": "c" },
    "@context": { "b": "http://example.com/b" }
  })");

  EXPECT_EQ(result, expected);
}

TEST(compact_to_relative_false_keeps_absolute_against_base) {
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

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "http://example.org/a",
    "b": { "@id": "http://example.org/c" },
    "@context": { "b": "http://example.com/b" }
  })");

  EXPECT_EQ(result, expected);
}

TEST(id_typed_value_with_index_keeps_index) {
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

  const auto expected = sourcemeta::core::parse_json(R"({
    "id": { "@id": "http://example.org/x", "@index": "foo" },
    "@context": { "id": { "@id": "http://example.com/id", "@type": "@id" } }
  })");

  EXPECT_EQ(result, expected);
}

TEST(nest_value_not_expanding_to_nest_is_rejected) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@vocab": "http://example.com/",
    "p": { "@nest": "other" },
    "other": "http://example.com/other"
  })");

  try {
    sourcemeta::core::jsonld_compact(input, context);
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_EQ(sourcemeta::core::to_string(error.pointer()), "");
  }
}

TEST(nest_value_aliasing_nest_nests_the_property) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@vocab": "http://example.com/",
    "p": { "@nest": "nst" },
    "nst": "@nest"
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "nst": { "p": "v" },
    "@context": {
      "@vocab": "http://example.com/",
      "p": { "@nest": "nst" },
      "nst": "@nest"
    }
  })");

  EXPECT_EQ(result, expected);
}

TEST(type_map_remaining_stays_array_without_compact_arrays) {
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

  const auto expected = sourcemeta::core::parse_json(R"({
    "@graph": [
      { "p": { "A": [ { "@type": [ "B" ], "v": [ "x" ] } ] } }
    ],
    "@context": {
      "@vocab": "http://example.com/",
      "p": { "@id": "http://example.com/p", "@container": "@type" }
    }
  })");

  EXPECT_EQ(result, expected);
}

TEST(protected_redefinition_with_different_index_expansion) {
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

  try {
    sourcemeta::core::jsonld_compact(input, context);
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_EQ(sourcemeta::core::to_string(error.pointer()), "/1/t");
  }
}

TEST(list_object_stays_array_without_compact_arrays) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@list": [ { "@value": "a" } ] } ] }
  ])");

  const auto context = sourcemeta::core::parse_json("{}");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_1, false)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "@graph": [
      { "http://example.com/p": [ { "@list": [ "a" ] } ] }
    ]
  })");

  EXPECT_EQ(result, expected);
}

TEST(iri_compaction_prefers_no_container_type_over_index) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "@type": [ "http://example.com/T" ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "t": "http://example.com/T",
    "idx": { "@id": "http://example.com/T", "@container": "@index" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "@type": "t",
    "@context": {
      "t": "http://example.com/T",
      "idx": { "@id": "http://example.com/T", "@container": "@index" }
    }
  })");

  EXPECT_EQ(result, expected);
}

TEST(iri_compaction_prefers_plain_graph_over_graph_id) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/g": [
        { "@graph": [ { "@id": "http://example.com/node" } ] }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "g": { "@id": "http://example.com/g", "@container": "@graph" },
    "gid": {
      "@id": "http://example.com/g",
      "@container": [ "@graph", "@id" ]
    }
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "g": { "@id": "http://example.com/node" },
    "@context": {
      "g": { "@id": "http://example.com/g", "@container": "@graph" },
      "gid": {
        "@id": "http://example.com/g",
        "@container": [ "@graph", "@id" ]
      }
    }
  })");

  EXPECT_EQ(result, expected);
}

// JSON-LD 1.1 API Section 6.2.3: an @index container is added as a candidate
// only when the processing mode is not json-ld-1.0. Nullifying the compaction
// context must not discard that processing mode, otherwise a 1.0 compaction
// would wrongly select the @index-container term. Under 1.0 the term is not a
// candidate, so the property stays expanded.
TEST(iri_compaction_1_0_preserves_processing_mode_across_a_null_context) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/prop": [ { "@value": "x" } ] }
  ])");
  const auto context = sourcemeta::core::parse_json(R"([
    null,
    { "prop": { "@id": "http://example.com/prop", "@container": "@index" } }
  ])");
  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_0)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.com/prop": "x",
    "@context": [
      null,
      { "prop": { "@id": "http://example.com/prop", "@container": "@index" } }
    ]
  })");
  EXPECT_EQ(result, expected);
}
