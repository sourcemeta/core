#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>

#include <cstddef>  // std::size_t
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string
#include <utility>  // std::move

// A document-relative identifier that names nothing beyond the document
// stays as it is when no base settles what it points at, and it compacts to
// itself rather than through the context
TEST(empty_identifier_without_a_base) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "",
      "http://example.com/b": [ { "@value": "v" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "b": "http://example.com/b"
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "",
    "b": "v",
    "@context": { "b": "http://example.com/b" }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

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

// JSON-LD 1.1 Section 4 writes the value of `@value` as any value, and a null
// one is a datum of its own once a document already holds it, so compaction
// keeps it rather than reading it as the absence the algorithm drops
TEST(value_object_holding_null_without_a_type) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.org/a",
      "http://example.com/p": [ { "@value": null } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": "http://example.com/p"
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "http://example.org/a",
    "p": null,
    "@context": { "p": "http://example.com/p" }
  })");

  EXPECT_EQ(result, expected);
}

// A type the term is not coerced to leaves the value object standing, as the
// shape is what carries the type rather than the term
TEST(value_object_holding_null_with_an_ordinary_type) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.org/a",
      "http://example.com/p": [
        { "@value": null, "@type": "http://example.com/t" }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": "http://example.com/p"
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "http://example.org/a",
    "p": { "@value": null, "@type": "http://example.com/t" },
    "@context": { "p": "http://example.com/p" }
  })");

  EXPECT_EQ(result, expected);
}

// JSON-LD 1.1 API Section 6.1.2 writes an empty array out as it stands rather
// than dropping it, since the property is present and says it holds nothing
TEST(property_holding_an_empty_array) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "@id": "http://example.org/a", "http://example.com/p": [] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": "http://example.com/p"
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "http://example.org/a",
    "p": [],
    "@context": { "p": "http://example.com/p" }
  })");

  EXPECT_EQ(result, expected);
}

// JSON-LD 1.0 has no list of lists, which compaction of a list under a term
// that wraps its value in one has to account for
TEST(list_compacted_under_1_0) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.org/a",
      "http://example.com/p": [ { "@list": [ { "@value": "x" } ] } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@container": "@list" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", {}, sourcemeta::core::JSONLDVersion::V1_0)};

  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "http://example.org/a",
    "p": [ "x" ],
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@list" }
    }
  })");

  EXPECT_EQ(result, expected);
}

// Compaction walks the same nesting that expansion does, so it is held to the
// same bound on how deep that nesting may run
TEST(deeply_nested_input_is_rejected) {
  // Expanded form nests a node object inside the array of a property, so the
  // depth is reached by wrapping that shape around itself from the inside out
  auto input{sourcemeta::core::JSON::make_array()};
  for (std::size_t level = 0; level < 150; level++) {
    auto node{sourcemeta::core::JSON::make_object()};
    node.assign("http://example.com/p", std::move(input));
    input = sourcemeta::core::JSON::make_array();
    input.push_back(std::move(node));
  }

  const auto context{sourcemeta::core::parse_json("{}")};
  try {
    [[maybe_unused]] const auto result{
        sourcemeta::core::jsonld_compact(input, context)};
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_EQ(std::string{error.what()}, "Maximum nesting depth exceeded");
  }
}

TEST(list_of_lists_is_rejected_under_the_1_0_processing_mode) {
  // JSON-LD 1.0 holds that a term with a list container cannot carry two list
  // objects, and names the refusal "compaction to list of lists" (JSON-LD 1.0
  // API Section 8.1). JSON-LD 1.1 permits a list of lists and drops that error
  // code from the ones it names (JSON-LD 1.1 API Section 9.6.2), so only the
  // earlier processing mode refuses this
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example/list": [
        { "@list": [ { "@list": [ { "@value": "foo" } ] },
                     { "@list": [ { "@value": "bar" } ] } ] }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "list": { "@id": "http://example/list", "@container": "@list" }
  })");

  try {
    sourcemeta::core::jsonld_compact(input, context, "", nullptr,
                                     sourcemeta::core::JSONLDVersion::V1_0);
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_EQ(std::string{error.what()}, "Compaction to list of lists");
  }
}

TEST(graph_container_over_a_value_that_is_not_a_graph_object) {
  // A graph container only takes hold of a graph object, so a value object
  // under the same term compacts as any other value would (JSON-LD 1.1 API
  // Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/g": [ { "@value": "plain" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "g": { "@id": "http://example.com/g", "@container": "@graph" }
  })");

  // The term is declined because its container does not match the value, so
  // the property keeps the IRI it was expanded to
  const auto result{sourcemeta::core::jsonld_compact(input, context)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.com/g": "plain",
    "@context": {
      "g": { "@id": "http://example.com/g", "@container": "@graph" }
    }
  })");

  EXPECT_EQ(result, expected);
}

TEST(graph_container_keeps_an_array_when_arrays_are_not_compacted) {
  // Holding every value in an array is what asking for arrays to stand
  // uncompacted means, a graph container included (JSON-LD 1.1 API Section
  // 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/g": [
        { "@graph": [ { "@id": "http://example.com/node" } ] }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "g": { "@id": "http://example.com/g", "@container": "@graph" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", nullptr, sourcemeta::core::JSONLDVersion::V1_1,
      false)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "@graph": [ { "g": [ { "@id": "http://example.com/node" } ] } ],
    "@context": {
      "g": { "@id": "http://example.com/g", "@container": "@graph" }
    }
  })");

  EXPECT_EQ(result, expected);
}

TEST(language_container_over_a_node_object) {
  // A language map keys value objects by their language, so a node object
  // under the same term takes the ordinary path instead (JSON-LD 1.1 API
  // Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/label": [ { "@id": "http://example.com/other" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "label": { "@id": "http://example.com/label", "@container": "@language" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(input, context)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.com/label": { "@id": "http://example.com/other" },
    "@context": {
      "label": { "@id": "http://example.com/label", "@container": "@language" }
    }
  })");

  EXPECT_EQ(result, expected);
}

TEST(reverse_property_under_the_1_0_processing_mode) {
  // The index candidates join the container preferences only past 1.0 and only
  // where the value carries no index of its own, so the earlier mode weighs a
  // reverse property without them (JSON-LD 1.1 API Section 6.2.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/parent",
      "@reverse": {
        "http://example.com/child": [ { "@id": "http://example.com/kid" } ]
      }
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "child": { "@reverse": "http://example.com/child" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", nullptr, sourcemeta::core::JSONLDVersion::V1_0)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/parent",
    "child": { "@id": "http://example.com/kid" },
    "@context": { "child": { "@reverse": "http://example.com/child" } }
  })");

  EXPECT_EQ(result, expected);
}

TEST(identifier_valued_term_prefers_vocab_when_the_target_names_a_term) {
  // Where the identifier compacts onto a term that maps back to the very same
  // IRI, the vocabulary candidate is weighed ahead of the identifier one
  // (JSON-LD 1.1 API Section 6.2.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/ref": [ { "@id": "http://example.com/target" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "ref": { "@id": "http://example.com/ref", "@type": "@id" },
    "target": "http://example.com/target"
  })");

  // The term is typed as an identifier rather than as a vocabulary entry, so
  // the value stands as an IRI against the document and keeps its full form
  // even though a term names it (JSON-LD 1.1 API Section 6.3.2)
  const auto result{sourcemeta::core::jsonld_compact(input, context)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "ref": "http://example.com/target",
    "@context": {
      "ref": { "@id": "http://example.com/ref", "@type": "@id" },
      "target": "http://example.com/target"
    }
  })");

  EXPECT_EQ(result, expected);
}

TEST(context_named_by_an_iri_is_carried_into_the_result) {
  // The compacted document carries the context it was compacted against, and a
  // context handed over as an IRI reference is carried as that reference
  // rather than as the document it loaded (JSON-LD 1.1 API Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/context") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const sourcemeta::core::JSON context{"https://example.com/context"};
  const auto result{
      sourcemeta::core::jsonld_compact(input, context, "", resolver)};
  const auto expected = sourcemeta::core::parse_json(R"({
    "p": "v",
    "@context": "https://example.com/context"
  })");

  EXPECT_EQ(result, expected);
}

TEST(list_member_value_object_keeps_its_language_and_direction) {
  // A list member is compacted in its own right, so a value object standing
  // there has its own keywords compacted alongside its value (JSON-LD 1.1 API
  // Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@list": [
            { "@value": "v", "@language": "en", "@direction": "ltr" }
          ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@container": "@list" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": [ { "@value": "v", "@language": "en", "@direction": "ltr" } ],
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@list" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(list_member_value_object_keeps_its_index) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@list": [ { "@value": "v", "@index": "one" } ] }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@container": "@list" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": [ { "@value": "v", "@index": "one" } ],
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@list" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(type_map_container_keys_an_untyped_node_under_none) {
  // A type map keys its members by their first type, so a member carrying no
  // type at all stands under the none label (JSON-LD 1.1 API Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@id": "http://example.com/x" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@container": "@type" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@none": "http://example.com/x" },
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@type" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(type_map_container_keys_a_node_with_an_empty_type_under_none) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@id": "http://example.com/x", "@type": [] }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@container": "@type" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@none": "http://example.com/x" },
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@type" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(value_object_with_a_direction_and_an_index) {
  // A direction joins the language in what the term preferences are weighed
  // against, but only where the value carries no index of its own
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "v", "@direction": "ltr", "@index": "one" }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@value": "v", "@direction": "ltr", "@index": "one" },
    "@context": { "p": { "@id": "http://example.com/p" } }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(an_iri_whose_scheme_names_a_term_that_is_no_prefix_stands) {
  // An IRI is refused where its scheme names a term usable as a prefix, since
  // the result would read as a compact IRI instead. A term that came from an
  // expanded definition is no prefix, so nothing is confused here (JSON-LD
  // 1.1 API Section 6.2.2)
  const auto input = sourcemeta::core::parse_json(R"([
    { "foo:bar": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "foo": { "@id": "http://example.com/foo" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "foo:bar": "v",
    "@context": { "foo": { "@id": "http://example.com/foo" } }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_context_handed_over_wrapped_in_a_context_entry) {
  // A context may arrive as the document that carries it, in which case the
  // entry inside is what the result is compacted against
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" }
  })");

  // The result carries the context that was inside the wrapper rather than
  // the wrapper itself
  const auto expected = sourcemeta::core::parse_json(R"({
    "p": "v",
    "@context": { "p": "http://example.com/p" }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_context_handed_over_as_an_array) {
  // A context may arrive as a list of contexts, which are layered in order
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"([
    { "q": "http://example.com/q" },
    { "p": "http://example.com/p" }
  ])");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": "v",
    "@context": [
      { "q": "http://example.com/q" },
      { "p": "http://example.com/p" }
    ]
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_term_that_declares_a_language_takes_it_off_the_value) {
  // Where the term already supplies the language, the value carries none of
  // its own (JSON-LD 1.1 API Section 6.3.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "v", "@language": "en" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@language": "en" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": "v",
    "@context": {
      "p": { "@id": "http://example.com/p", "@language": "en" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_term_that_declares_a_direction_takes_it_off_the_value) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "v", "@direction": "ltr" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@direction": "ltr" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": "v",
    "@context": {
      "p": { "@id": "http://example.com/p", "@direction": "ltr" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_context_that_is_null_is_not_carried_into_the_result) {
  // The result carries the context it was compacted against only where that
  // context holds something, so one that names nothing leaves no entry behind
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json("null");

  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v"
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(nest_naming_a_retired_term_is_refused) {
  // A nest value that is not the keyword itself has to name a term expanding
  // onto it, and a term that was retired maps to nothing at all
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "n": null,
    "p": { "@id": "http://example.com/p", "@nest": "n" }
  })");

  try {
    sourcemeta::core::jsonld_compact(input, context);
    FAIL();
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid @nest value");
  }
}

TEST(a_list_object_under_a_term_without_a_list_container) {
  // A list object compacts to a bare array only where the term coerces a list,
  // so one under a plain term keeps the list entry (JSON-LD 1.1 API Section
  // 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@list": [ { "@value": "x" } ] } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@list": [ "x" ] },
    "@context": { "p": { "@id": "http://example.com/p" } }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_list_container_over_a_plain_value_under_the_1_0_processing_mode) {
  // The refusal of a list inside a list weighs the value the term carries, and
  // a term coercing a list may still be handed a value that is no list at all
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "x" } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@container": "@list" }
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "", nullptr, sourcemeta::core::JSONLDVersion::V1_0)};
  // The term is declined because its container does not match the value, so
  // the property keeps the IRI it was expanded to and the list refusal is
  // never weighed against it
  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "x",
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@list" }
    }
  })");

  EXPECT_EQ(result, expected);
}

TEST(property_index_keys_by_an_identifier_typed_property) {
  // An index map may key by a property of the item rather than by its index,
  // and the key is taken from that property only where it reads as a string
  // (JSON-LD 1.1 API Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/x",
          "http://example.com/label": [ { "@id": "http://example.com/y" } ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": {
      "@id": "http://example.com/p",
      "@container": "@index",
      "@index": "http://example.com/label"
    },
    "label": { "@id": "http://example.com/label", "@type": "@id" }
  })");

  // The label term coerces an identifier, so the node reference compacts to a
  // bare IRI and reads as the key
  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "http://example.com/y": { "@id": "http://example.com/x" } },
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "http://example.com/label"
      },
      "label": { "@id": "http://example.com/label", "@type": "@id" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(property_index_keys_under_none_where_the_property_holds_an_object) {
  // Where the property names no string, the item stands under the none label
  // rather than under a key of its own (JSON-LD 1.1 API Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/x",
          "http://example.com/label": [ { "@id": "http://example.com/y" } ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": {
      "@id": "http://example.com/p",
      "@container": "@index",
      "@index": "http://example.com/label"
    },
    "label": { "@id": "http://example.com/label" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": {
      "@none": {
        "@id": "http://example.com/x",
        "label": { "@id": "http://example.com/y" }
      }
    },
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "http://example.com/label"
      },
      "label": { "@id": "http://example.com/label" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_set_container_term_is_declined_for_a_list_object) {
  // Term selection asks for a term whose container suits the value, and a list
  // object asks for a list container, so a term offering a set container is no
  // candidate and the property stands as the IRI it names (Section 4.4.2)
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@list": [ { "@value": "a" } ] } ] }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@container": "@set" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ] },
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@set" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(property_index_over_an_empty_array_keys_under_none) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@id": "http://example.com/x", "http://example.com/label": [] }
      ]
    }
  ])");
  const auto context = sourcemeta::core::parse_json(R"({
    "p": {
      "@id": "http://example.com/p",
      "@container": "@index",
      "@index": "http://example.com/label"
    },
    "label": { "@id": "http://example.com/label" }
  })");
  // The key comes from the first value the indexing property holds, and an
  // empty array holds none, so the member stands under the none label with
  // that property left as it was (Section 6.1.2)
  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@none": { "@id": "http://example.com/x", "label": [] } },
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@index",
        "@index": "http://example.com/label"
      },
      "label": { "@id": "http://example.com/label" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(identifier_map_keys_under_none_where_the_member_coerces_to_a_scalar) {
  const auto input = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@id": "http://example.com/x" } ] }
  ])");
  const auto context = sourcemeta::core::parse_json(R"({
    "idmap": {
      "@id": "http://example.com/p",
      "@container": "@id",
      "@type": "@id"
    }
  })");
  // The identifier the key is read from is the one the compacted member
  // carries, and a member that coerced to a bare identifier is no map and so
  // carries none, which leaves it under the none label rather than keyed by an
  // identifier it also still holds (Section 6.1.2)
  const auto expected = sourcemeta::core::parse_json(R"({
    "idmap": { "@none": "http://example.com/x" },
    "@context": {
      "idmap": {
        "@id": "http://example.com/p",
        "@container": "@id",
        "@type": "@id"
      }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_bare_graph_object_holding_one_node_collapses) {
  // A graph object carrying no name of its own lets its lone node stand on its
  // own, which is what the official suite requires of it (suite entries
  // #t0090, #t0092 and #t0094)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@graph": [
            {
              "@id": "http://example.com/n",
              "http://example.com/q": [ { "@value": "v" } ]
            }
          ]
        }
      ]
    }
  ])");
  const auto context = sourcemeta::core::parse_json(R"({
    "p": "http://example.com/p",
    "q": "http://example.com/q"
  })");
  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@graph": { "@id": "http://example.com/n", "q": "v" } },
    "@context": {
      "p": "http://example.com/p",
      "q": "http://example.com/q"
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_named_graph_holding_one_node_keeps_its_array) {
  // A graph object that carries a name keeps its nodes as an array, so the
  // name reads as naming a graph rather than as another property of it
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "http://example.com/g",
          "@graph": [
            {
              "@id": "http://example.com/n",
              "http://example.com/q": [ { "@value": "v" } ]
            }
          ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": "http://example.com/p",
    "q": "http://example.com/q"
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": {
      "@id": "http://example.com/g",
      "@graph": [ { "@id": "http://example.com/n", "q": "v" } ]
    },
    "@context": {
      "p": "http://example.com/p",
      "q": "http://example.com/q"
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_typed_value_matching_the_term_type_drops_its_type) {
  // A term coercing a type carries it, so a value object stating that very
  // type need not state it again (Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "v", "@type": "http://example.com/T" }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@type": "http://example.com/T" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": "v",
    "@context": {
      "p": { "@id": "http://example.com/p", "@type": "http://example.com/T" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_typed_value_parting_from_the_term_type_keeps_its_type) {
  // Where the type stated is not the one the term coerces, term selection
  // declines that term, so the property stands as the IRI it names and the
  // value object states its type in full (Section 4.4.2, Section 6.1.2)
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "v", "@type": "http://example.com/U" }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "http://example.com/p", "@type": "http://example.com/T" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "http://example.com/p": {
      "@value": "v",
      "@type": "http://example.com/U"
    },
    "@context": {
      "p": { "@id": "http://example.com/p", "@type": "http://example.com/T" }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_list_of_lists_under_a_plain_term_keeps_its_nesting) {
  // Compacting the members of a list hands an inner list to the same walk
  // under a term that coerces no list, and the inner list keeps its own
  // boundary rather than being unwrapped (Section 6.1.2 step 3.3)
  const auto input = sourcemeta::core::parse_json(
      R"([ { "http://e/p": [ { "@list": [ { "@list": [] } ] } ] } ])");

  const auto context = sourcemeta::core::parse_json(R"({ "p": "http://e/p" })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": { "@list": [ { "@list": [] } ] },
    "@context": { "p": "http://e/p" }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_named_graph_holding_several_nodes_keeps_its_array) {
  // A graph naming itself holds its nodes as an array whatever their number,
  // so one holding several is carried over as it stands
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://e/p": [
        {
          "@id": "http://e/g",
          "@graph": [ { "@id": "http://e/n1" }, { "@id": "http://e/n2" } ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({ "p": "http://e/p" })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": {
      "@id": "http://e/g",
      "@graph": [ { "@id": "http://e/n1" }, { "@id": "http://e/n2" } ]
    },
    "@context": { "p": "http://e/p" }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_term_named_by_the_iri_it_maps_is_found_whatever_the_value_holds) {
  // A term whose name is the very IRI it maps is settled on without weighing
  // the value, so a container it carries says nothing about the shape in hand
  const auto input = sourcemeta::core::parse_json(
      R"([ { "http://e/p": [ { "@value": "v" } ] } ])");

  const auto context = sourcemeta::core::parse_json(
      R"({ "http://e/p": { "@container": "@graph" } })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "http://e/p": "v",
    "@context": { "http://e/p": { "@container": "@graph" } }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_list_mixing_a_language_tagged_value_with_a_node_reference) {
  // A node reference carries no language of its own, so it parts ways with the
  // language the values agreed on without being a value itself
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "http://e/p": [
        {
          "@list": [
            { "@value": "x", "@language": "en" },
            { "@id": "http://e/n" }
          ]
        }
      ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(
      R"({ "p": { "@id": "http://e/p", "@container": "@list" } })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "p": [ { "@value": "x", "@language": "en" }, { "@id": "http://e/n" } ],
    "@context": { "p": { "@id": "http://e/p", "@container": "@list" } }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(a_compact_iri_candidate_whose_term_gave_up_its_mapping_is_declined) {
  // The name a prefix would build is already a term, and that term maps
  // nothing, so the name is not free to take and the IRI stands (Section 6.2.2)
  const auto input =
      sourcemeta::core::parse_json(R"([ { "@type": [ "http://e/x" ] } ])");

  const auto context =
      sourcemeta::core::parse_json(R"({ "ex": "http://e/", "ex:x": null })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "@type": "http://e/x",
    "@context": { "ex": "http://e/", "ex:x": null }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}

TEST(of_two_candidates_the_same_length_the_one_sorting_first_wins) {
  // Two prefixes can build names of equal length, and the order the terms are
  // read in is not the order those names sort in, so the comparison settles it
  // (Section 6.2.2)
  const auto input =
      sourcemeta::core::parse_json(R"([ { "@type": [ "http://e/xyz" ] } ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "a": "http://e/",
    "a-": { "@id": "http://e/x", "@prefix": true }
  })");

  const auto expected = sourcemeta::core::parse_json(R"({
    "@type": "a-:yz",
    "@context": {
      "a": "http://e/",
      "a-": { "@id": "http://e/x", "@prefix": true }
    }
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_compact(input, context), expected);
}
