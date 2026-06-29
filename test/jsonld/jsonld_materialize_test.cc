#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

TEST(JSONLD_materialize, node_with_literal_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "name": "Sourcemeta" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/org"}});
  map.emplace(sourcemeta::core::Pointer{"name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/org",
      "https://schema.org/name": [ { "@value": "Sourcemeta" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, literal_with_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "isbn": "978-0131103627" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "urn:isbn:978-0131103627"}});
  map.emplace(sourcemeta::core::Pointer{"isbn"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/isbn", false}},
                  .value = sourcemeta::core::JSONLDLiteral{
                      .datatype = "http://www.w3.org/2001/XMLSchema#string"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "urn:isbn:978-0131103627",
      "https://schema.org/isbn": [
        {
          "@value": "978-0131103627",
          "@type": "http://www.w3.org/2001/XMLSchema#string"
        }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, literal_with_language_and_direction) {
  const auto instance = sourcemeta::core::parse_json(R"({ "title": "مرحبا" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"title"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{
                      .language = "ar",
                      .direction = sourcemeta::core::JSONLDDirection::RTL}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/name": [
        { "@value": "مرحبا", "@language": "ar", "@direction": "rtl" }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, reference_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "currency": "USD" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "urn:isbn:978-0131103627"}});
  map.emplace(sourcemeta::core::Pointer{"currency"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/priceCurrency", false}},
                  .value = sourcemeta::core::JSONLDReference{
                      .id = "https://www.iso.org/iso-4217/USD",
                      .types = {"https://schema.org/Currency"}}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "urn:isbn:978-0131103627",
      "https://schema.org/priceCurrency": [
        {
          "@id": "https://www.iso.org/iso-4217/USD",
          "@type": [ "https://schema.org/Currency" ]
        }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, ordered_collection_becomes_list) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "authors": [ "Ada", "Alan" ] })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/book"}});
  map.emplace(
      sourcemeta::core::Pointer{"authors"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{.ordered = true}});
  map.emplace(sourcemeta::core::Pointer{"authors", 0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDLiteral{
                      .datatype = "http://www.w3.org/2001/XMLSchema#string"}});
  map.emplace(sourcemeta::core::Pointer{"authors", 1},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDLiteral{
                      .datatype = "http://www.w3.org/2001/XMLSchema#string"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/author": [
        {
          "@list": [
            { "@value": "Ada", "@type": "http://www.w3.org/2001/XMLSchema#string" },
            { "@value": "Alan", "@type": "http://www.w3.org/2001/XMLSchema#string" }
          ]
        }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, unordered_collection_spreads_into_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "tags": [ "a", "b" ] })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(
      sourcemeta::core::Pointer{"tags"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/keywords", false}},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(sourcemeta::core::Pointer{"tags", 0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{"tags", 1},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/keywords": [ { "@value": "a" }, { "@value": "b" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, field_with_multiple_predicates) {
  const auto instance = sourcemeta::core::parse_json(R"({ "label": "Hi" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(
      sourcemeta::core::Pointer{"label"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false},
                    {"http://www.w3.org/2000/01/rdf-schema#label", false}},
          .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/name": [ { "@value": "Hi" } ],
      "http://www.w3.org/2000/01/rdf-schema#label": [ { "@value": "Hi" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, reverse_edge) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "series": { "name": "Trilogy" } })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/book"}});
  map.emplace(sourcemeta::core::Pointer{"series"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/hasPart", true}},
                  .value = sourcemeta::core::JSONLDNode{}});
  map.emplace(sourcemeta::core::Pointer{"series", "name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "@reverse": {
        "https://schema.org/hasPart": [
          { "https://schema.org/name": [ { "@value": "Trilogy" } ] }
        ]
      }
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, shared_blank_node_identifier_co_refers) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "billing": {}, "shipping": {} })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/order"}});
  map.emplace(sourcemeta::core::Pointer{"billing"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/billing", false}},
                  .value = sourcemeta::core::JSONLDNode{.id = "_:address"}});
  map.emplace(sourcemeta::core::Pointer{"shipping"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/shipping", false}},
                  .value = sourcemeta::core::JSONLDNode{.id = "_:address"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/order",
      "https://example.com/billing": [ { "@id": "_:address" } ],
      "https://example.com/shipping": [ { "@id": "_:address" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, json_literal_preserved_verbatim) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "payload": { "a": [ 1, 2 ] } })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"payload"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/raw", false}},
                  .value = sourcemeta::core::JSONLDLiteral{.json = true}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/raw": [
        { "@value": { "a": [ 1, 2 ] }, "@type": "@json" }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, nested_ordered_collection_is_list_of_lists) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "matrix": [ [ 5 ] ] })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(
      sourcemeta::core::Pointer{"matrix"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/rows", false}},
          .value = sourcemeta::core::JSONLDCollection{.ordered = true}});
  map.emplace(
      sourcemeta::core::Pointer{"matrix", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{.ordered = true}});
  map.emplace(sourcemeta::core::Pointer{"matrix", 0, 0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDLiteral{
                      .datatype = "http://www.w3.org/2001/XMLSchema#integer"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/rows": [
        {
          "@list": [
            {
              "@list": [
                {
                  "@value": 5,
                  "@type": "http://www.w3.org/2001/XMLSchema#integer"
                }
              ]
            }
          ]
        }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, undescribed_object_with_described_child) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "name": "Doc", "meta": { "title": "T" } })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{"meta", "title"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/title", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/name": [ { "@value": "Doc" } ]
    },
    { "https://example.com/title": [ { "@value": "T" } ] }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, described_node_without_edge_is_standalone) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "extra": { "x": "v" } })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"extra"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/other"}});
  map.emplace(sourcemeta::core::Pointer{"extra", "x"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/x", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/doc" },
    {
      "@id": "https://example.com/other",
      "https://example.com/x": [ { "@value": "v" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, null_value_produces_no_triple) {
  const auto instance = sourcemeta::core::parse_json(R"({ "maybe": null })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"maybe"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/doc" }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, named_graph) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "prov": { "who": "X" } })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"prov"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/statedIn", false}},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/graph", .graph = true}});
  map.emplace(sourcemeta::core::Pointer{"prov", "who"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/statedIn": [
        {
          "@id": "https://example.com/graph",
          "@graph": [
            {
              "@id": "https://example.com/graph",
              "https://schema.org/name": [ { "@value": "X" } ]
            }
          ]
        }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, scalar_root_reference) {
  const auto instance = sourcemeta::core::parse_json(R"("USD")");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDReference{
                      .id = "https://www.iso.org/iso-4217/USD",
                      .types = {"https://schema.org/Currency"}}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://www.iso.org/iso-4217/USD",
      "@type": [ "https://schema.org/Currency" ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(JSONLD_materialize, empty_map_yields_empty_array) {
  const auto instance = sourcemeta::core::parse_json(R"({ "a": 1 })");
  const sourcemeta::core::JSONLDAnnotationMap map;

  const auto expected = sourcemeta::core::parse_json("[]");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}
