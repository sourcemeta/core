#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <functional> // std::cref

TEST(node_with_literal_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "name": "Sourcemeta" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/org"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"name"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/org",
      "https://schema.org/name": [ { "@value": "Sourcemeta" } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(literal_with_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "isbn": "978-0131103627" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "urn:isbn:978-0131103627"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"isbn"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(literal_with_language_and_direction) {
  const auto instance = sourcemeta::core::parse_json(R"({ "title": "مرحبا" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"title"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(reference_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "currency": "USD" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "urn:isbn:978-0131103627"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"currency"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(ordered_collection_becomes_list) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "authors": [ "Ada", "Alan" ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#string"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors", 1},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(unordered_collection_spreads_into_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "tags": [ "a", "b" ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"tags"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/keywords", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"tags", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"tags", 1},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/keywords": [ { "@value": "a" }, { "@value": "b" } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(field_with_multiple_predicates) {
  const auto instance = sourcemeta::core::parse_json(R"({ "label": "Hi" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(reverse_edge) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "series": { "name": "Trilogy" } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"series"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/hasPart", true}},
                               .value = sourcemeta::core::JSONLDNode{}});
  annotations.emplace_back(sourcemeta::core::Pointer{"series", "name"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(shared_blank_node_identifier_co_refers) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "billing": {}, "shipping": {} })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/order"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"billing"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/billing", false}},
          .value = sourcemeta::core::JSONLDNode{.id = "_:address"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"shipping"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(json_literal_preserved_verbatim) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "payload": { "a": [ 1, 2 ] } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"payload"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(nested_ordered_collection_is_list_of_lists) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "matrix": [ [ 5 ] ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"matrix"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/rows", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"matrix", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"matrix", 0, 0},
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
                  "@value": "5",
                  "@type": "http://www.w3.org/2001/XMLSchema#integer"
                }
              ]
            }
          ]
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(undescribed_object_with_described_child) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "name": "Doc", "meta": { "title": "T" } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"name"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(sourcemeta::core::Pointer{"meta", "title"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(described_node_without_edge_is_standalone) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "extra": { "x": "v" } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"extra"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/other"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"extra", "x"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(null_value_produces_no_triple) {
  const auto instance = sourcemeta::core::parse_json(R"({ "maybe": null })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"maybe"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/doc" }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(named_graph) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "prov": { "who": "X" } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"prov"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/statedIn", false}},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/graph", .graph = true}});
  annotations.emplace_back(sourcemeta::core::Pointer{"prov", "who"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(scalar_root_reference) {
  const auto instance = sourcemeta::core::parse_json(R"("USD")");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(empty_map_yields_empty_array) {
  const auto instance = sourcemeta::core::parse_json(R"({ "a": 1 })");
  const sourcemeta::core::JSONLDAnnotationList annotations;

  const auto expected = sourcemeta::core::parse_json("[]");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(node_with_type) {
  const auto instance = sourcemeta::core::parse_json(R"({ "name": "Ada" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/ada",
                                   .types = {"https://schema.org/Person"}}});
  annotations.emplace_back(sourcemeta::core::Pointer{"name"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(node_with_multiple_types) {
  const auto instance = sourcemeta::core::parse_json(R"({})");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(reference_without_types) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "currency": "USD" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"currency"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(literal_with_language_only) {
  const auto instance = sourcemeta::core::parse_json(R"({ "title": "Hello" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"title"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDLiteral{.language = "en"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/name": [ { "@value": "Hello", "@language": "en" } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(literal_with_ltr_direction) {
  const auto instance = sourcemeta::core::parse_json(R"({ "title": "Hello" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"title"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(boolean_literal) {
  const auto instance = sourcemeta::core::parse_json(R"({ "active": true })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"active"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://example.com/active", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/active": [ { "@value": true } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(integer_literal_without_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "count": 42 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"count"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://example.com/count", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/count": [ { "@value": 42 } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(ordered_collection_of_nodes) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "authors": [ { "name": "Ada" } ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDNode{}});
  annotations.emplace_back(sourcemeta::core::Pointer{"authors", 0, "name"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(unordered_collection_of_nodes) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "items": [ { "sku": "1" }, { "sku": "2" } ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/order"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"items"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/item", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"items", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDNode{}});
  annotations.emplace_back(sourcemeta::core::Pointer{"items", 0, "sku"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://example.com/sku", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"items", 1},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDNode{}});
  annotations.emplace_back(sourcemeta::core::Pointer{"items", 1, "sku"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(empty_ordered_collection_is_rdf_nil) {
  const auto instance = sourcemeta::core::parse_json(R"({ "authors": [] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/author": [ { "@list": [] } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(empty_unordered_collection_adds_no_property) {
  const auto instance = sourcemeta::core::parse_json(R"({ "tags": [] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"tags"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/keywords", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/doc" }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(collection_defaults_undescribed_scalar_elements) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "authors": [ "Ada", "Unknown", "Alan" ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"authors", 2},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/author": [
        { "@list": [
          { "@value": "Ada" }, { "@value": "Unknown" }, { "@value": "Alan" }
        ] }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(unordered_collection_defaults_all_undescribed_elements) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "keywords": [ "reference", "standards", true, 5 ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"keywords"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/keywords", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/keywords": [
        { "@value": "reference" },
        { "@value": "standards" },
        { "@value": true },
        { "@value": 5 }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(index_collection_defaults_undescribed_members) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "identifiers": { "doi": "10.1000/1", "isbn": "978-0" } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"identifiers"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/identifier", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Index}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/identifier": [
        { "@value": "10.1000/1" },
        { "@value": "978-0" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(collection_defaults_nested_undescribed_array_as_set) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "groups": [ [ "a", "b" ], "c" ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/member", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/member": [
        { "@value": "a" }, { "@value": "b" }, { "@value": "c" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(collection_undescribed_object_element_is_still_dropped) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "keywords": [ "reference", { "label": "extra" } ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"keywords"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/keywords", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/keywords": [ { "@value": "reference" } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(collection_null_element_is_still_dropped) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "keywords": [ "reference", null, "standards" ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"keywords"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/keywords", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/keywords": [
        { "@value": "reference" }, { "@value": "standards" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(nested_unordered_collection_flattens) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "groups": [ [ "a", "b" ], [ "c" ] ] })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/member", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups", 1},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups", 0, 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups", 0, 1},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups", 1, 0},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(array_root_spreads_into_default_graph) {
  const auto instance =
      sourcemeta::core::parse_json(R"([ { "name": "A" }, { "name": "B" } ])");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{0},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/a"}});
  annotations.emplace_back(sourcemeta::core::Pointer{0, "name"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{1},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/b"}});
  annotations.emplace_back(sourcemeta::core::Pointer{1, "name"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(root_collection_of_literals_is_dropped) {
  const auto instance = sourcemeta::core::parse_json(R"([ "a", "b" ])");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{1},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json("[]");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(root_collection_keeps_only_node_elements) {
  const auto instance =
      sourcemeta::core::parse_json(R"([ { "name": "A" }, "loose" ])");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{0},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/a"}});
  annotations.emplace_back(sourcemeta::core::Pointer{0, "name"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{1},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/a",
      "https://schema.org/name": [ { "@value": "A" } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(scalar_root_literal_is_dropped) {
  const auto instance = sourcemeta::core::parse_json(R"("hello")");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json("[]");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(collection_descriptor_on_non_array_is_dropped) {
  const auto instance = sourcemeta::core::parse_json(R"({ "x": "scalar" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"x"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/x", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/doc" }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(output_is_a_fixed_point_of_expansion) {
  const auto instance = sourcemeta::core::parse_json(R"({
    "name": "Ada",
    "homepage": "https://ada.example",
    "books": [ "Notes" ]
  })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/ada",
                                   .types = {"https://schema.org/Person"}}});
  annotations.emplace_back(sourcemeta::core::Pointer{"name"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(sourcemeta::core::Pointer{"homepage"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/url", false}},
                               .value = sourcemeta::core::JSONLDReference{
                                   .id = "https://ada.example"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"books"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"books", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#string"}});

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));

  // Expanded form is a fixed point of expansion, so re-expanding the output of
  // materialization must leave it unchanged.
  EXPECT_EQ(sourcemeta::core::jsonld_expand(result), result);
}

TEST(node_descriptor_on_scalar_ignores_value) {
  const auto instance = sourcemeta::core::parse_json(R"({ "x": "scalar" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"x"},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(weak_node_with_literal_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "name": "Sourcemeta" })");

  const sourcemeta::core::JSON::String name_key{"name"};

  sourcemeta::core::JSONLDWeakAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/org"}});
  annotations.emplace_back(sourcemeta::core::WeakPointer{std::cref(name_key)},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/org",
      "https://schema.org/name": [ { "@value": "Sourcemeta" } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(weak_reference_property) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "currency": "USD" })");

  const sourcemeta::core::JSON::String currency_key{"currency"};

  sourcemeta::core::JSONLDWeakAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "urn:isbn:978-0131103627"}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{std::cref(currency_key)},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(weak_ordered_collection_becomes_list) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "authors": [ "Ada", "Alan" ] })");

  const sourcemeta::core::JSON::String authors_key{"authors"};

  sourcemeta::core::JSONLDWeakAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::WeakPointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{std::cref(authors_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{std::cref(authors_key), 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#string"}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{std::cref(authors_key), 1},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(weak_reverse_edge) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "series": { "name": "Trilogy" } })");

  const sourcemeta::core::JSON::String series_key{"series"};
  const sourcemeta::core::JSON::String name_key{"name"};

  sourcemeta::core::JSONLDWeakAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::WeakPointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(sourcemeta::core::WeakPointer{std::cref(series_key)},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/hasPart", true}},
                               .value = sourcemeta::core::JSONLDNode{}});
  annotations.emplace_back(
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(weak_array_root_spreads_into_default_graph) {
  const auto instance =
      sourcemeta::core::parse_json(R"([ { "name": "A" }, { "name": "B" } ])");

  const sourcemeta::core::JSON::String name_key{"name"};

  sourcemeta::core::JSONLDWeakAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{0},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/a"}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{0, std::cref(name_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{1},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/b"}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{1, std::cref(name_key)},
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

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(language_container) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "label": { "en": "Colour", "fr": [ "Couleur", "Teinte" ], "@none": "Color" } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/thing"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"label"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Language}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/thing",
      "https://schema.org/name": [
        { "@value": "Color" },
        { "@value": "Colour", "@language": "en" },
        { "@value": "Couleur", "@language": "fr" },
        { "@value": "Teinte", "@language": "fr" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(language_container_empty) {
  const auto instance = sourcemeta::core::parse_json(R"({ "label": {} })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/thing"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"label"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Language}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/thing" }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(language_container_null_value) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "label": { "en": null, "fr": "Bonjour" } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/thing"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"label"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Language}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/thing",
      "https://schema.org/name": [
        { "@value": "Bonjour", "@language": "fr" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(language_container_null_array_element) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "label": { "en": [ "Hi", null, "Hello" ] } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/thing"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"label"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Language}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/thing",
      "https://schema.org/name": [
        { "@value": "Hi", "@language": "en" },
        { "@value": "Hello", "@language": "en" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(language_container_null_none) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "label": { "@none": null, "en": "Colour" } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/thing"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"label"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Language}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/thing",
      "https://schema.org/name": [
        { "@value": "Colour", "@language": "en" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(language_container_all_null) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "label": { "en": null, "fr": [ null ] } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/thing"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"label"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Language}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/thing" }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(index_container_of_nodes) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "posts": { "2023": { "title": "A" }, "2024": { "title": "B" } } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/blog"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"posts"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/blogPost", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Index}});
  annotations.emplace_back(sourcemeta::core::Pointer{"posts", "2023"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/post/2023"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"posts", "2023", "title"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/headline", false}},
          .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(sourcemeta::core::Pointer{"posts", "2024"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/post/2024"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"posts", "2024", "title"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/headline", false}},
          .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/blog",
      "https://schema.org/blogPost": [
        {
          "@id": "https://example.com/post/2023",
          "https://schema.org/headline": [ { "@value": "A" } ]
        },
        {
          "@id": "https://example.com/post/2024",
          "https://schema.org/headline": [ { "@value": "B" } ]
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(index_container_of_literals) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "scores": { "math": 90, "art": 80 } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/report"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"scores"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/value", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Index}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"scores", "math"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"scores", "art"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/report",
      "https://schema.org/value": [ { "@value": 80 }, { "@value": 90 } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(index_container_with_nested_set) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "groups": { "a": [ "x", "y" ] } })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/item", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Index}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups", "a"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups", "a", 0},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"groups", "a", 1},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {}, .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://schema.org/item": [ { "@value": "x" }, { "@value": "y" } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(index_container_empty) {
  const auto instance = sourcemeta::core::parse_json(R"({ "posts": {} })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/blog"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"posts"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/blogPost", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Index}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@id": "https://example.com/blog" }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(weak_collection_defaults_undescribed_scalar_elements) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "keywords": [ "reference", "standards" ] })");

  const sourcemeta::core::JSON::String keywords_key{"keywords"};

  sourcemeta::core::JSONLDWeakAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::WeakPointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/book"}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{std::cref(keywords_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/keywords", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/book",
      "https://schema.org/keywords": [
        { "@value": "reference" }, { "@value": "standards" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(duplicate_annotations_first_one_wins) {
  const auto instance = sourcemeta::core::parse_json(R"({ "name": "Ada" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/person"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"name"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(sourcemeta::core::Pointer{},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {},
                               .value = sourcemeta::core::JSONLDNode{
                                   .id = "https://example.com/ignored"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"name"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/ignored", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "https://example.com/ignored"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/person",
      "https://schema.org/name": [ { "@value": "Ada" } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// RDF conversion turns a native number with a non-zero fractional part into
// the "canonical lexical form of an xsd:double" no matter which datatype was
// requested (JSON-LD 1.1 API Section 8.2.2), and xsd:decimal admits no
// exponent (XSD 1.1 Part 2 Section 3.3.3), so the value must materialize as a
// string to survive as a well-typed literal
TEST(fractional_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.5",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(precise_fractional_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 1.85 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1.85",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// An integral value happens to survive native RDF conversion as a valid
// decimal, yet it is still serialised as a string so that every explicitly
// typed literal takes the same form
TEST(integer_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 42 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "42",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// An integral value drops the decimal point, matching the canonical mapping
// of xsd:decimal (XSD 1.1 Part 2 Section 3.3.3)
TEST(integral_real_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 2.0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "2",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(zero_real_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(negative_fractional_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": -0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "-0.5",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(negative_integer_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": -42 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "-42",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// The shortest round-trip spelling of this magnitude is scientific notation,
// which xsd:decimal does not admit, so the fixed notation expansion is
// required
TEST(small_fractional_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.00001 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.00001",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// Native RDF conversion switches to double form at "an absolute value greater
// or equal to 10^21" (JSON-LD 1.1 API Section 8.2.2), so this magnitude only
// stays a valid decimal through the string form
TEST(large_real_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 1e21 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1000000000000000000000",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// Source notation does not survive parsing, so serialisation depends only on
// the parsed value
TEST(exponent_notation_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 1e2 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "100",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// A fraction beyond 15 significant digits is kept as an exact decimal by the
// parser, so the spelling survives untouched
TEST(long_fractional_literal_with_decimal_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": 0.3333333333333333 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.3333333333333333",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// A mismatched datatype has no valid lexical form to offer, so the output is
// the deterministic exponent-free spelling of the value
TEST(fractional_literal_with_integer_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#integer"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.5",
          "@type": "http://www.w3.org/2001/XMLSchema#integer"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(integer_literal_with_integer_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 42 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#integer"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "42",
          "@type": "http://www.w3.org/2001/XMLSchema#integer"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(large_integer_literal_with_integer_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": 9007199254740993 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#integer"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "9007199254740993",
          "@type": "http://www.w3.org/2001/XMLSchema#integer"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// A quantity that no double can hold exactly stays exact all the way into the
// literal, unlike processors whose numbers bottom out at IEEE doubles
TEST(high_precision_fractional_literal_with_decimal_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": 1.8500000000000000001 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1.8500000000000000001",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(huge_integer_literal_with_integer_datatype) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "value": 123456789012345678901234567890 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#integer"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "123456789012345678901234567890",
          "@type": "http://www.w3.org/2001/XMLSchema#integer"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// An explicit xsd:double reproduces the exact canonical form that conforming
// RDF conversion derives from a native number (JSON-LD 1.1 API Section 8.6),
// keeping the resulting literal terms byte-identical
TEST(fractional_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "5.0E-1",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(precise_fractional_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 1.85 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1.85E0",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(integer_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 42 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "4.2E1",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(integral_real_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 2.0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "2.0E0",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// "The canonical representation for zero is 0.0E0" (JSON-LD 1.1 API Section
// 8.6)
TEST(zero_real_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.0E0",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// Zero has a single canonical representation regardless of sign
TEST(negative_zero_real_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": -0.0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.0E0",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(negative_fractional_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": -1.85 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "-1.85E0",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(small_fractional_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.1 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1.0E-1",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(large_real_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 1e21 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1.0E21",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// The value space of xsd:double is the IEEE 64-bit floating point type, so an
// integer above 2^53 rounds to the nearest representable value (JSON-LD 1.1
// API Section 8.6)
TEST(large_integer_literal_with_double_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": 9007199254740993 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "9.007199254740992E15",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// The mantissa is "rounded to 15 digits after the decimal point" (JSON-LD 1.1
// API Section 8.6)
TEST(tiny_real_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 5e-324 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "4.940656458412465E-324",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// xsd:float shares the scientific canonical form of xsd:double (XSD 1.1
// Part 2 Section 3.3.4)
TEST(fractional_literal_with_float_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#float"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "5.0E-1",
          "@type": "http://www.w3.org/2001/XMLSchema#float"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// Booleans become "the string true or false which is the canonical lexical
// form" (JSON-LD 1.1 API Section 8.2.2)
TEST(true_literal_with_boolean_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": true })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#boolean"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "true",
          "@type": "http://www.w3.org/2001/XMLSchema#boolean"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(false_literal_with_boolean_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": false })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#boolean"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "false",
          "@type": "http://www.w3.org/2001/XMLSchema#boolean"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// RDF conversion handles booleans before numbers, so a boolean keeps its
// boolean lexical form under any datatype (JSON-LD 1.1 API Section 8.2.2)
TEST(true_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": true })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "true",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(true_literal_with_custom_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": true })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"value"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://example.com/value", false}},
                               .value = sourcemeta::core::JSONLDLiteral{
                                   .datatype = "https://example.com/flag"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "true",
          "@type": "https://example.com/flag"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(fractional_literal_with_custom_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "https://example.com/quantity"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.5",
          "@type": "https://example.com/quantity"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(integer_literal_with_custom_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 42 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "https://example.com/quantity"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "42",
          "@type": "https://example.com/quantity"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// Without an explicit datatype a native number is exactly what the default
// xsd:integer and xsd:double typing is for (JSON-LD 1.1 API Section 8.6), so
// it is preserved verbatim
TEST(fractional_literal_without_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"value"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://example.com/value", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [ { "@value": 0.5 } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(boolean_literal_without_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": true })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(sourcemeta::core::Pointer{"value"},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://example.com/value", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [ { "@value": true } ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(fractional_json_literal_preserved_verbatim) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{.json = true}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        { "@value": 0.5, "@type": "@json" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// A string value passes through RDF conversion verbatim as the lexical form,
// so it is never rewritten
TEST(string_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": "0.5" })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.5",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(minimum_integer_literal_with_integer_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": -9223372036854775808 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#integer"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "-9223372036854775808",
          "@type": "http://www.w3.org/2001/XMLSchema#integer"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(maximum_integer_literal_with_integer_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": 9223372036854775807 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#integer"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "9223372036854775807",
          "@type": "http://www.w3.org/2001/XMLSchema#integer"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(zero_integer_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(zero_integer_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.0E0",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(minimum_integer_literal_with_double_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": -9223372036854775808 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "-9.223372036854776E18",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(integer_literal_with_float_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 42 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#float"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "4.2E1",
          "@type": "http://www.w3.org/2001/XMLSchema#float"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// The largest finite double expands to 309 digits, close to the
// serialisation buffer bound. The digits are the exact expansion of the
// stored value, which ties in length with the zero padded seventeen digit
// spelling and denotes the value precisely
TEST(maximum_real_literal_with_decimal_datatype) {
  auto instance{sourcemeta::core::JSON::make_object()};
  instance.assign("value", sourcemeta::core::JSON{1.7976931348623157e308});

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// The smallest subnormal double expands to the deepest possible
// fraction
TEST(subnormal_real_literal_with_decimal_datatype) {
  auto instance{sourcemeta::core::JSON::make_object()};
  instance.assign("value", sourcemeta::core::JSON{5e-324});

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(negative_large_decimal_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": -1e21 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "-1000000000000000000000",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(negative_fraction_decimal_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": -1e-20 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "-0.00000000000000000001",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(interior_point_decimal_literal_with_decimal_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": 123.4567890123456789 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "123.4567890123456789",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// Trailing zeros normalize away so that the same value always produces
// the same literal regardless of the written quantum
TEST(trailing_zeros_decimal_literal_with_decimal_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": 1.8500000000000000000 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1.85",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(trailing_zeros_integer_decimal_literal_with_integer_datatype) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "value": 12300000000000000000000000 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#integer"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "12300000000000000000000000",
          "@type": "http://www.w3.org/2001/XMLSchema#integer"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(zero_decimal_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0e0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(zero_decimal_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0e0 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.0E0",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// A magnitude beyond the double range cannot borrow the scientific
// canonical form, so the exact expansion is the deterministic fallback
TEST(overflowing_decimal_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 1e309 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(underflowing_decimal_literal_with_double_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 1e-400 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(high_precision_fraction_decimal_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(
      R"({ "value": 0.1234567890123456789012345678901234567890123456789 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "0.1234567890123456789012345678901234567890123456789",
          "@type": "http://www.w3.org/2001/XMLSchema#decimal"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

TEST(maximum_double_decimal_literal_with_double_datatype) {
  const auto instance =
      sourcemeta::core::parse_json(R"({ "value": 1.7976931348623157e308 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#double"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        {
          "@value": "1.797693134862316E308",
          "@type": "http://www.w3.org/2001/XMLSchema#double"
        }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// An opaque JSON literal takes precedence, so an accompanying datatype is
// ignored and the value stays native
TEST(json_literal_ignores_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": 0.5 })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal",
              .json = true}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc",
      "https://example.com/value": [
        { "@value": 0.5, "@type": "@json" }
      ]
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}

// A null value asserts nothing, so no literal is produced at all
TEST(null_literal_with_decimal_datatype) {
  const auto instance = sourcemeta::core::parse_json(R"({ "value": null })");

  sourcemeta::core::JSONLDAnnotationList annotations;
  annotations.emplace_back(
      sourcemeta::core::Pointer{},
      sourcemeta::core::JSONLDDescriptor{.edges = {},
                                         .value = sourcemeta::core::JSONLDNode{
                                             .id = "https://example.com/doc"}});
  annotations.emplace_back(
      sourcemeta::core::Pointer{"value"},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://example.com/value", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/doc"
    }
  ])");

  const auto result{
      sourcemeta::core::jsonld_materialize(instance, annotations)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(result));
}
