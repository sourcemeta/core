#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <functional> // std::cref

TEST(node_with_literal_property) {
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

TEST(literal_with_datatype) {
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

TEST(literal_with_language_and_direction) {
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

TEST(reference_property) {
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

TEST(ordered_collection_becomes_list) {
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

TEST(unordered_collection_spreads_into_property) {
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

TEST(field_with_multiple_predicates) {
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

TEST(reverse_edge) {
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

TEST(shared_blank_node_identifier_co_refers) {
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

TEST(json_literal_preserved_verbatim) {
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

TEST(nested_ordered_collection_is_list_of_lists) {
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

TEST(undescribed_object_with_described_child) {
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

TEST(described_node_without_edge_is_standalone) {
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

TEST(null_value_produces_no_triple) {
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

TEST(named_graph) {
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

TEST(scalar_root_reference) {
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

TEST(empty_map_yields_empty_array) {
  const auto instance = sourcemeta::core::parse_json(R"({ "a": 1 })");
  const sourcemeta::core::JSONLDAnnotationMap map;

  const auto expected = sourcemeta::core::parse_json("[]");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(node_with_type) {
  const auto instance = sourcemeta::core::parse_json(R"({ "name": "Ada" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/ada",
                      .types = {"https://schema.org/Person"}}});
  map.emplace(sourcemeta::core::Pointer{"name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/ada",
      "@type": [ "https://schema.org/Person" ],
      "https://schema.org/name": [ { "@value": "Ada" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(node_with_multiple_types) {
  const auto instance = sourcemeta::core::parse_json(R"({})");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/thing",
                      .types = {"https://schema.org/Person",
                                "https://schema.org/Author"}}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/thing",
      "@type": [ "https://schema.org/Person", "https://schema.org/Author" ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(reference_without_types) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "currency": "USD" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/book"}});
  map.emplace(sourcemeta::core::Pointer{"currency"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/priceCurrency", false}},
                  .value = sourcemeta::core::JSONLDReference{
                      .id = "https://www.iso.org/iso-4217/USD"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/priceCurrency": [
        { "@id": "https://www.iso.org/iso-4217/USD" }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(literal_with_language_only) {
  const auto instance = sourcemeta::core::parse_json(R"({ "title": "Hello" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"title"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{.language = "en"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/name": [ { "@value": "Hello", "@language": "en" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(literal_with_ltr_direction) {
  const auto instance = sourcemeta::core::parse_json(R"({ "title": "Hello" })");

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
                      .language = "en",
                      .direction = sourcemeta::core::JSONLDDirection::LTR}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/name": [
        { "@value": "Hello", "@language": "en", "@direction": "ltr" }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(boolean_literal) {
  const auto instance = sourcemeta::core::parse_json(R"({ "active": true })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"active"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/active", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/active": [ { "@value": true } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(integer_literal_without_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "count": 42 })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"count"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/count", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/count": [ { "@value": 42 } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(ordered_collection_of_nodes) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "authors": [ { "name": "Ada" } ] })");

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
                  .edges = {}, .value = sourcemeta::core::JSONLDNode{}});
  map.emplace(sourcemeta::core::Pointer{"authors", 0, "name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/author": [
        {
          "@list": [
            { "https://schema.org/name": [ { "@value": "Ada" } ] }
          ]
        }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(unordered_collection_of_nodes) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "items": [ { "sku": "1" }, { "sku": "2" } ] })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/order"}});
  map.emplace(
      sourcemeta::core::Pointer{"items"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/item", false}},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(sourcemeta::core::Pointer{"items", 0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDNode{}});
  map.emplace(sourcemeta::core::Pointer{"items", 0, "sku"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/sku", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{"items", 1},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDNode{}});
  map.emplace(sourcemeta::core::Pointer{"items", 1, "sku"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/sku", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/order",
      "https://example.com/item": [
        { "https://example.com/sku": [ { "@value": "1" } ] },
        { "https://example.com/sku": [ { "@value": "2" } ] }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(empty_ordered_collection_is_rdf_nil) {
  const auto instance = sourcemeta::core::parse_json(R"({ "authors": [] })");

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

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/author": [ { "@list": [] } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(empty_unordered_collection_adds_no_property) {
  const auto instance = sourcemeta::core::parse_json(R"({ "tags": [] })");

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

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/doc" }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(collection_with_undescribed_elements_skips_them) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "authors": [ "Ada", "Unknown", "Alan" ] })");

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
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{"authors", 2},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/author": [
        { "@list": [ { "@value": "Ada" }, { "@value": "Alan" } ] }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(nested_unordered_collection_flattens) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "groups": [ [ "a", "b" ], [ "c" ] ] })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(
      sourcemeta::core::Pointer{"groups"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/member", false}},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(
      sourcemeta::core::Pointer{"groups", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(
      sourcemeta::core::Pointer{"groups", 1},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(sourcemeta::core::Pointer{"groups", 0, 0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{"groups", 0, 1},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{"groups", 1, 0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/member": [
        { "@value": "a" }, { "@value": "b" }, { "@value": "c" }
      ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(array_root_spreads_into_default_graph) {
  const auto instance =
      sourcemeta::core::parse_json(R"([ { "name": "A" }, { "name": "B" } ])");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(
      sourcemeta::core::Pointer{0},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/a"}});
  map.emplace(sourcemeta::core::Pointer{0, "name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(
      sourcemeta::core::Pointer{1},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/b"}});
  map.emplace(sourcemeta::core::Pointer{1, "name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/a",
      "https://schema.org/name": [ { "@value": "A" } ]
    },
    {
      "@id": "https://example.com/b",
      "https://schema.org/name": [ { "@value": "B" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(root_collection_of_literals_is_dropped) {
  const auto instance = sourcemeta::core::parse_json(R"([ "a", "b" ])");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(sourcemeta::core::Pointer{0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{1},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json("[]");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(root_collection_keeps_only_node_elements) {
  const auto instance =
      sourcemeta::core::parse_json(R"([ { "name": "A" }, "loose" ])");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(
      sourcemeta::core::Pointer{0},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/a"}});
  map.emplace(sourcemeta::core::Pointer{0, "name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{1},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/a",
      "https://schema.org/name": [ { "@value": "A" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(scalar_root_literal_is_dropped) {
  const auto instance = sourcemeta::core::parse_json(R"("hello")");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json("[]");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(collection_descriptor_on_non_array_is_dropped) {
  const auto instance = sourcemeta::core::parse_json(R"({ "x": "scalar" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(
      sourcemeta::core::Pointer{"x"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/x", false}},
          .value = sourcemeta::core::JSONLDCollection{.ordered = true}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/doc" }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(output_is_a_fixed_point_of_expansion) {
  const auto instance = sourcemeta::core::parse_json(R"({
    "name": "Ada",
    "homepage": "https://ada.example",
    "books": [ "Notes" ]
  })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(sourcemeta::core::Pointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/ada",
                      .types = {"https://schema.org/Person"}}});
  map.emplace(sourcemeta::core::Pointer{"name"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(sourcemeta::core::Pointer{"homepage"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/url", false}},
                  .value = sourcemeta::core::JSONLDReference{
                      .id = "https://ada.example"}});
  map.emplace(
      sourcemeta::core::Pointer{"books"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{.ordered = true}});
  map.emplace(sourcemeta::core::Pointer{"books", 0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDLiteral{
                      .datatype = "http://www.w3.org/2001/XMLSchema#string"}});

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));

  // Expanded form is a fixed point of expansion, so re-expanding the output of
  // materialization must leave it unchanged.
  EXPECT_EQ(sourcemeta::core::jsonld_expand(result), result);
}

TEST(node_descriptor_on_scalar_ignores_value) {
  const auto instance = sourcemeta::core::parse_json(R"({ "x": "scalar" })");

  sourcemeta::core::JSONLDAnnotationMap map;
  map.emplace(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  map.emplace(sourcemeta::core::Pointer{"x"},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://example.com/x", false}},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/other"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/x": [ { "@id": "https://example.com/other" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(weak_node_with_literal_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "name": "Sourcemeta" })");

  const sourcemeta::core::JSON::String name_key{"name"};

  sourcemeta::core::JSONLDWeakAnnotationMap map;
  map.emplace(
      sourcemeta::core::WeakPointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/org"}});
  map.emplace(sourcemeta::core::WeakPointer{std::cref(name_key)},
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

TEST(weak_reference_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "currency": "USD" })");

  const sourcemeta::core::JSON::String currency_key{"currency"};

  sourcemeta::core::JSONLDWeakAnnotationMap map;
  map.emplace(
      sourcemeta::core::WeakPointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "urn:isbn:978-0131103627"}});
  map.emplace(sourcemeta::core::WeakPointer{std::cref(currency_key)},
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

TEST(weak_ordered_collection_becomes_list) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "authors": [ "Ada", "Alan" ] })");

  const sourcemeta::core::JSON::String authors_key{"authors"};

  sourcemeta::core::JSONLDWeakAnnotationMap map;
  map.emplace(sourcemeta::core::WeakPointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/book"}});
  map.emplace(
      sourcemeta::core::WeakPointer{std::cref(authors_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{.ordered = true}});
  map.emplace(sourcemeta::core::WeakPointer{std::cref(authors_key), 0},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDLiteral{
                      .datatype = "http://www.w3.org/2001/XMLSchema#string"}});
  map.emplace(sourcemeta::core::WeakPointer{std::cref(authors_key), 1},
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

TEST(weak_reverse_edge) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "series": { "name": "Trilogy" } })");

  const sourcemeta::core::JSON::String series_key{"series"};
  const sourcemeta::core::JSON::String name_key{"name"};

  sourcemeta::core::JSONLDWeakAnnotationMap map;
  map.emplace(sourcemeta::core::WeakPointer{},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {},
                  .value = sourcemeta::core::JSONLDNode{
                      .id = "https://example.com/book"}});
  map.emplace(sourcemeta::core::WeakPointer{std::cref(series_key)},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/hasPart", true}},
                  .value = sourcemeta::core::JSONLDNode{}});
  map.emplace(
      sourcemeta::core::WeakPointer{std::cref(series_key), std::cref(name_key)},
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

TEST(weak_array_root_spreads_into_default_graph) {
  const auto instance =
      sourcemeta::core::parse_json(R"([ { "name": "A" }, { "name": "B" } ])");

  const sourcemeta::core::JSON::String name_key{"name"};

  sourcemeta::core::JSONLDWeakAnnotationMap map;
  map.emplace(
      sourcemeta::core::WeakPointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{.ordered = false}});
  map.emplace(
      sourcemeta::core::WeakPointer{0},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/a"}});
  map.emplace(sourcemeta::core::WeakPointer{0, std::cref(name_key)},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});
  map.emplace(
      sourcemeta::core::WeakPointer{1},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/b"}});
  map.emplace(sourcemeta::core::WeakPointer{1, std::cref(name_key)},
              sourcemeta::core::JSONLDDescriptor{
                  .edges = {{"https://schema.org/name", false}},
                  .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/a",
      "https://schema.org/name": [ { "@value": "A" } ]
    },
    {
      "@id": "https://example.com/b",
      "https://schema.org/name": [ { "@value": "B" } ]
    }
  ])");

  const auto result{sourcemeta::core::jsonld_materialize(instance, map)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}
