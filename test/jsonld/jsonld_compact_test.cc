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

// JSON-LD 1.1 API Section 6.2.3: the @index candidate is added only when the
// processing mode is not json-ld-1.0. This applies to the node-reference branch
// too, so under 1.0 a node reference does not select the @index-container term
// and its property stays expanded.
TEST(iri_compaction_1_0_does_not_add_index_candidate_for_a_node_reference) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/prop": [ { "@id": "http://example.com/n" } ] }
  ])");
  const auto context = sourcemeta::core::parse_json(R"([
    null,
    { "prop": { "@id": "http://example.com/prop", "@container": "@index" } }
  ])");
  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_0)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.com/prop": { "@id": "http://example.com/n" },
    "@context": [
      null,
      { "prop": { "@id": "http://example.com/prop", "@container": "@index" } }
    ]
  })");
  EXPECT_EQ(result, expected);
}

// A JSON literal holding null compacts to a bare null under a term coerced
// to @json, and the null is data rather than absence, so it survives inside
// compacted lists instead of being dropped with other null items
TEST(json_literal_null_in_list_is_kept) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.org/vocab#e": [
        {
          "@list": [
            { "@value": null, "@type": "@json" },
            { "@value": 1, "@type": "@json" }
          ]
        }
      ]
    }
  ])");
  const auto context = sourcemeta::core::parse_json(R"({
    "e": {
      "@id": "http://example.org/vocab#e",
      "@type": "@json",
      "@container": "@list"
    }
  })");
  const auto result{sourcemeta::core::jsonld_compact(input, context)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "e": [ null, 1 ],
    "@context": {
      "e": {
        "@id": "http://example.org/vocab#e",
        "@type": "@json",
        "@container": "@list"
      }
    }
  })");
  EXPECT_EQ(result, expected);
}

TEST(json_literal_null_among_property_values_is_kept) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.org/vocab#e": [
        { "@value": null, "@type": "@json" },
        { "@value": true, "@type": "@json" }
      ]
    }
  ])");
  const auto context = sourcemeta::core::parse_json(R"({
    "e": { "@id": "http://example.org/vocab#e", "@type": "@json" }
  })");
  const auto result{sourcemeta::core::jsonld_compact(input, context)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "e": [ null, true ],
    "@context": {
      "e": { "@id": "http://example.org/vocab#e", "@type": "@json" }
    }
  })");
  EXPECT_EQ(result, expected);
}

TEST(json_literal_null_without_json_term_keeps_value_object) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.org/vocab#e": [
        { "@value": null, "@type": "@json" }
      ]
    }
  ])");
  const auto context = sourcemeta::core::parse_json(R"({
    "e": { "@id": "http://example.org/vocab#e" }
  })");
  const auto result{sourcemeta::core::jsonld_compact(input, context)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "e": { "@value": null, "@type": "@json" },
    "@context": {
      "e": { "@id": "http://example.org/vocab#e" }
    }
  })");
  EXPECT_EQ(result, expected);
}

// Language tags are case insensitive (RFC 5646 Section 2.1.1), so a value
// whose language differs from the term language mapping only in casing still
// compacts to a plain string
TEST(language_match_is_case_insensitive) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.org/vocab#label": [
        { "@value": "hello", "@language": "EN-us" }
      ]
    }
  ])");
  const auto context = sourcemeta::core::parse_json(R"({
    "label": { "@id": "http://example.org/vocab#label", "@language": "en-US" }
  })");
  const auto result{sourcemeta::core::jsonld_compact(input, context)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "label": "hello",
    "@context": {
      "label": {
        "@id": "http://example.org/vocab#label",
        "@language": "en-US"
      }
    }
  })");
  EXPECT_EQ(result, expected);
}

// A genuinely different language keeps the value object regardless of casing
TEST(language_mismatch_keeps_value_object) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.org/vocab#label": [
        { "@value": "hallo", "@language": "DE-ch" }
      ]
    }
  ])");
  const auto context = sourcemeta::core::parse_json(R"({
    "label": { "@id": "http://example.org/vocab#label", "@language": "en-US" }
  })");
  const auto result{sourcemeta::core::jsonld_compact(input, context)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.org/vocab#label": {
      "@value": "hallo",
      "@language": "DE-ch"
    },
    "@context": {
      "label": {
        "@id": "http://example.org/vocab#label",
        "@language": "en-US"
      }
    }
  })");
  EXPECT_EQ(result, expected);
}

// JSON-LD 1.1 API Section 4.3, Inverse Context Creation, builds a key from a
// term's language and direction together, so a term carrying both has to be
// told apart from one carrying either alone or neither. The uppercase language
// tag is what pins the lowercasing that algorithm asks for
TEST(compact_selects_a_term_carrying_both_language_and_direction) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/both": [
        { "@value": "x", "@language": "en", "@direction": "ltr" }
      ],
      "http://example.com/lang": [ { "@value": "x", "@language": "en" } ],
      "http://example.com/dir": [ { "@value": "x", "@direction": "ltr" } ],
      "http://example.com/neither": [ { "@value": "x" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "both": {
      "@id": "http://example.com/both",
      "@language": "EN",
      "@direction": "ltr"
    },
    "lang": { "@id": "http://example.com/lang", "@language": "en" },
    "dir": { "@id": "http://example.com/dir", "@direction": "ltr" },
    "neither": { "@id": "http://example.com/neither" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_1, true,
      true)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "both": "x",
    "lang": "x",
    "dir": "x",
    "neither": "x",
    "@context": {
      "both": {
        "@id": "http://example.com/both",
        "@language": "EN",
        "@direction": "ltr"
      },
      "lang": { "@id": "http://example.com/lang", "@language": "en" },
      "dir": { "@id": "http://example.com/dir", "@direction": "ltr" },
      "neither": { "@id": "http://example.com/neither" }
    }
  })");

  EXPECT_EQ(result, expected);
}

// JSON-LD 1.1 API Section 4.3 builds that key from whichever of the two
// mappings is not null, so a term setting one of them to null, or both, lands
// under a key of its own
TEST(compact_selects_terms_whose_language_or_direction_is_null) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/lang": [ { "@value": "x", "@language": "en" } ],
      "http://example.com/dir": [ { "@value": "x", "@direction": "ltr" } ],
      "http://example.com/none": [ { "@value": "x" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "lang": {
      "@id": "http://example.com/lang",
      "@language": "EN",
      "@direction": null
    },
    "dir": {
      "@id": "http://example.com/dir",
      "@language": null,
      "@direction": "ltr"
    },
    "none": {
      "@id": "http://example.com/none",
      "@language": null,
      "@direction": null
    }
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_1, true,
      true)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "lang": "x",
    "dir": "x",
    "none": "x",
    "@context": {
      "lang": {
        "@id": "http://example.com/lang",
        "@language": "EN",
        "@direction": null
      },
      "dir": {
        "@id": "http://example.com/dir",
        "@language": null,
        "@direction": "ltr"
      },
      "none": {
        "@id": "http://example.com/none",
        "@language": null,
        "@direction": null
      }
    }
  })");

  EXPECT_EQ(result, expected);
}

// JSON-LD 1.1 API Section 4.4, Term Selection, reaches the same key while
// choosing a term for a list, which asks for every item to carry both
TEST(compact_selects_a_list_term_carrying_both_language_and_direction) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/both": [
        {
          "@list": [
            { "@value": "x", "@language": "en", "@direction": "ltr" },
            { "@value": "y", "@language": "en", "@direction": "ltr" }
          ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "both": {
      "@id": "http://example.com/both",
      "@container": "@list",
      "@language": "EN",
      "@direction": "ltr"
    }
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_1, true,
      true)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "both": [ "x", "y" ],
    "@context": {
      "both": {
        "@id": "http://example.com/both",
        "@container": "@list",
        "@language": "EN",
        "@direction": "ltr"
      }
    }
  })");

  EXPECT_EQ(result, expected);
}

// JSON-LD 1.1 API Section 6.2 keeps a candidate that is shorter, or the same
// length and lexicographically less, so two prefixes of one length settle it
// by comparison rather than by the order they were written
TEST(compact_breaks_a_tie_between_two_prefixes_of_one_length) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/x": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "aa": "http://example.com/",
    "ab": "http://example.com/"
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_1, true,
      true)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "aa:x": "v",
    "@context": {
      "aa": "http://example.com/",
      "ab": "http://example.com/"
    }
  })");

  EXPECT_EQ(result, expected);
}

// An identifier equal to the base relativises to nothing, and with no base
// path there is no last segment to put in its place
TEST(an_identifier_equal_to_a_base_that_carries_no_path) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.org",
      "http://example.com/b": [ { "@value": "v" } ]
    }
  ])");

  const auto context =
      sourcemeta::core::parse_json(R"({ "b": "http://example.com/b" })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "",
    "b": "v",
    "@context": { "b": "http://example.com/b" }
  })");

  EXPECT_EQ(
      sourcemeta::core::jsonld_compact(input, context, "http://example.org"),
      expected);
}

// A base path with no slash is its own last segment
TEST(an_identifier_equal_to_a_base_whose_path_holds_no_slash) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "urn:example",
      "http://example.com/b": [ { "@value": "v" } ]
    }
  ])");

  const auto context =
      sourcemeta::core::parse_json(R"({ "b": "http://example.com/b" })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "example",
    "b": "v",
    "@context": { "b": "http://example.com/b" }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context, "urn:example"),
            expected);
}

// JSON-LD 1.1 API Section 5.2.3 joins the default language and direction
// with an underscore when a direction is set, which is the key a value
// carrying both is looked up under
TEST(a_context_carrying_both_a_language_and_a_direction) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/b": [
        { "@value": "v", "@language": "en", "@direction": "ltr" }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "@language": "en",
    "@direction": "ltr",
    "b": "http://example.com/b"
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "b": "v",
    "@context": {
      "@version": 1.1,
      "@language": "en",
      "@direction": "ltr",
      "b": "http://example.com/b"
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context, ""), expected);
}

// The index candidates are offered only to a value that does not already
// carry an index of its own
TEST(a_value_that_already_carries_an_index) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/b": [ { "@value": "v", "@index": "i" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "b": { "@id": "http://example.com/b", "@container": "@index" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "b": { "i": "v" },
    "@context": {
      "@version": 1.1,
      "b": { "@id": "http://example.com/b", "@container": "@index" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context, ""), expected);
}

// The compacted document carries the context it was compacted against, and an
// empty one says nothing worth carrying
TEST(compact_against_an_empty_array_context_carries_no_context) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "@id": "http://example.org/a" }
  ])");

  const auto context = sourcemeta::core::parse_json("[]");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "http://example.org/", {},
      sourcemeta::core::JSONLDVersion::V1_1, true, false)};

  EXPECT_FALSE(result.defines("@context"));
  EXPECT_EQ(result.at("@id").to_string(), "http://example.org/a");
}

// JSON-LD 1.1 API Section 6.1 keeps the value object whole when its index
// cannot be carried by the term, even though the type mapping does match
TEST(a_typed_value_that_already_carries_an_index) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/b": [
        { "@value": "x", "@type": "http://example.com/t", "@index": "i" }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "b": { "@id": "http://example.com/b", "@type": "http://example.com/t" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "b": {
      "@value": "x",
      "@type": "http://example.com/t",
      "@index": "i"
    },
    "@context": {
      "b": { "@id": "http://example.com/b", "@type": "http://example.com/t" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context, ""), expected);
}

// A value whose direction disagrees with the one the context sets by default
// has to keep that direction, so it cannot compact to a bare string
TEST(a_value_whose_direction_disagrees_with_the_default) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/b": [ { "@value": "x", "@direction": "rtl" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "@direction": "ltr",
    "b": "http://example.com/b"
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "b": { "@value": "x", "@direction": "rtl" },
    "@context": {
      "@version": 1.1,
      "@direction": "ltr",
      "b": "http://example.com/b"
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context, ""), expected);
}

// JSON-LD 1.1 API Section 5.2.2 refuses to compact a list of lists under the
// json-ld-1.0 processing mode, as that revision has no spelling for one
TEST(compaction_in_1_0_refuses_a_list_of_lists) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@list": [ { "@list": [ { "@value": "x" } ] } ] }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": "http://example.com/p"
  })");

  try {
    sourcemeta::core::jsonld_compact(input, context, "", {},
                                     sourcemeta::core::JSONLDVersion::V1_0);
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_EQ(sourcemeta::core::to_string(error.pointer()), "");
  }
}

// A list whose members are not themselves lists is what that revision does
// have a spelling for
TEST(compaction_in_1_0_keeps_a_plain_list) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@list": [ { "@value": "x" }, { "@value": "y" } ] }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": "http://example.com/p"
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_0)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@list": [ "x", "y" ] },
    "@context": { "p": "http://example.com/p" }
  })");

  EXPECT_EQ(result, expected);
}

// JSON-LD 1.1 API Section 6.1.2 takes the index key of a property-valued index
// container from the compacted item's own property, which the container then
// drops. A term keeping that property as an array is what shows the key being
// lifted out of one
TEST(compaction_takes_a_property_index_key_from_a_single_valued_array) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/a",
          "http://example.com/key": [ { "@value": "one" } ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "p": {
      "@id": "http://example.com/p",
      "@container": "@index",
      "@index": "http://example.com/key"
    },
    "key": { "@id": "http://example.com/key", "@container": "@set" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context, "")};

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "one": { "@id": "http://example.com/a" } },
    "@context": {
      "@version": 1.1,
      "p": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "http://example.com/key"
      },
      "key": { "@id": "http://example.com/key", "@container": "@set" }
    }
  })");

  EXPECT_EQ(result, expected);
}

// Only the first value becomes the key, so the rest of the property stays on
// the item
TEST(compaction_takes_a_property_index_key_from_the_first_of_several) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/a",
          "http://example.com/key": [
            { "@value": "one" },
            { "@value": "two" },
            { "@value": "three" }
          ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "p": {
      "@id": "http://example.com/p",
      "@container": "@index",
      "@index": "http://example.com/key"
    },
    "key": "http://example.com/key"
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context, "")};

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": {
      "one": {
        "@id": "http://example.com/a",
        "key": [ "two", "three" ]
      }
    },
    "@context": {
      "@version": 1.1,
      "p": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "http://example.com/key"
      },
      "key": "http://example.com/key"
    }
  })");

  EXPECT_EQ(result, expected);
}

// A lone remaining value collapses to a scalar only when arrays are being
// compacted, so leaving them alone keeps it an array of one
TEST(compaction_keeps_a_lone_remaining_property_index_value_as_an_array) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/a",
          "http://example.com/key": [ { "@value": "one" }, { "@value": "two" } ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "p": {
      "@id": "http://example.com/p",
      "@container": "@index",
      "@index": "http://example.com/key"
    },
    "key": "http://example.com/key"
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_1, false,
      true)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "@graph": [
      {
        "p": {
          "one": [
            {
              "@id": "http://example.com/a",
              "key": [ "two" ]
            }
          ]
        }
      }
    ],
    "@context": {
      "@version": 1.1,
      "p": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "http://example.com/key"
      },
      "key": "http://example.com/key"
    }
  })");

  EXPECT_EQ(result, expected);
}

// The key has to be a string, so a property holding anything else names no
// index and the item falls to the default entry
TEST(compaction_falls_back_when_a_property_index_value_is_no_string) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/a",
          "http://example.com/key": [ { "@value": 5 } ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "p": {
      "@id": "http://example.com/p",
      "@container": "@index",
      "@index": "http://example.com/key"
    },
    "key": { "@id": "http://example.com/key", "@container": "@set" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context, "")};

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": {
      "@none": {
        "@id": "http://example.com/a",
        "key": [ 5 ]
      }
    },
    "@context": {
      "@version": 1.1,
      "p": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "http://example.com/key"
      },
      "key": { "@id": "http://example.com/key", "@container": "@set" }
    }
  })");

  EXPECT_EQ(result, expected);
}

// An item that holds no such property at all names no index either
TEST(compaction_falls_back_when_an_item_lacks_the_index_property) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@id": "http://example.com/a" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "p": {
      "@id": "http://example.com/p",
      "@container": "@index",
      "@index": "http://example.com/key"
    }
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context, "")};

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@none": { "@id": "http://example.com/a" } },
    "@context": {
      "@version": 1.1,
      "p": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "http://example.com/key"
      }
    }
  })");

  EXPECT_EQ(result, expected);
}

// JSON-LD 1.1 API Section 6.1.2 indexes a type map by the item's first type and
// drops that type from the item, which leaves a node holding nothing but an
// identifier as the identifier alone. Leaving arrays alone is what keeps the
// single type an array long enough to be dropped as one
TEST(compaction_indexes_a_type_map_by_a_lone_type) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/a",
          "@type": [ "http://example.com/T" ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "p": { "@id": "http://example.com/p", "@container": "@type" },
    "T": "http://example.com/T"
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_1, false,
      true)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "@graph": [
      { "p": { "T": [ "http://example.com/a" ] } }
    ],
    "@context": {
      "@version": 1.1,
      "p": { "@id": "http://example.com/p", "@container": "@type" },
      "T": "http://example.com/T"
    }
  })");

  EXPECT_EQ(result, expected);
}

// Only the first type is dropped, so the types beyond the second stay an array
TEST(compaction_indexes_a_type_map_and_keeps_the_remaining_types) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/a",
          "@type": [
            "http://example.com/T",
            "http://example.com/U",
            "http://example.com/V"
          ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@version": 1.1,
    "p": { "@id": "http://example.com/p", "@container": "@type" },
    "T": "http://example.com/T",
    "U": "http://example.com/U",
    "V": "http://example.com/V"
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context, "")};

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": {
      "T": {
        "@id": "http://example.com/a",
        "@type": [ "U", "V" ]
      }
    },
    "@context": {
      "@version": 1.1,
      "p": { "@id": "http://example.com/p", "@container": "@type" },
      "T": "http://example.com/T",
      "U": "http://example.com/U",
      "V": "http://example.com/V"
    }
  })");

  EXPECT_EQ(result, expected);
}
