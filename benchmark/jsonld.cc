#include <benchmark/benchmark.h>

#include <cassert>    // assert
#include <cstddef>    // std::size_t
#include <functional> // std::cref
#include <string>     // std::string, std::to_string
#include <utility>    // std::move

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

// The catalog dimensions set how many annotation entries the annotations
// carries and how many instance positions materialization visits, so they are
// the knobs to turn when stress testing this functionality
static constexpr std::size_t catalog_member_count{256};
static constexpr std::size_t authors_per_member{3};
static constexpr std::size_t keywords_per_member{3};
static constexpr std::size_t annotations_per_member{19 +
                                                    (authors_per_member * 2)};
static constexpr std::size_t total_annotation_count{
    2 + (catalog_member_count * annotations_per_member)};

static const sourcemeta::core::JSON::String members_key{"members"};
static const sourcemeta::core::JSON::String isbn_key{"isbn"};
static const sourcemeta::core::JSON::String title_key{"title"};
static const sourcemeta::core::JSON::String abstract_key{"abstract"};
static const sourcemeta::core::JSON::String date_published_key{"datePublished"};
static const sourcemeta::core::JSON::String authors_key{"authors"};
static const sourcemeta::core::JSON::String name_key{"name"};
static const sourcemeta::core::JSON::String keywords_key{"keywords"};
static const sourcemeta::core::JSON::String identifiers_key{"identifiers"};
static const sourcemeta::core::JSON::String price_key{"price"};
static const sourcemeta::core::JSON::String currency_key{"currency"};
static const sourcemeta::core::JSON::String value_key{"value"};
static const sourcemeta::core::JSON::String publisher_key{"publisher"};
static const sourcemeta::core::JSON::String url_key{"url"};
static const sourcemeta::core::JSON::String series_key{"series"};
static const sourcemeta::core::JSON::String metadata_key{"metadata"};
static const sourcemeta::core::JSON::String provenance_key{"provenance"};
static const sourcemeta::core::JSON::String generated_by_key{"generatedBy"};

static auto currency_code(const std::size_t index) -> std::string {
  return index % 3 == 0 ? "USD" : index % 3 == 1 ? "EUR" : "GBP";
}

static auto make_person(const std::size_t index) -> sourcemeta::core::JSON {
  auto person{sourcemeta::core::JSON::make_object()};
  person.assign("id",
                sourcemeta::core::JSON{"person-" + std::to_string(index)});
  person.assign("name",
                sourcemeta::core::JSON{"Person " + std::to_string(index)});
  return person;
}

static auto make_title(const std::size_t index) -> sourcemeta::core::JSON {
  auto title{sourcemeta::core::JSON::make_object()};
  title.assign("en", sourcemeta::core::JSON{"Title " + std::to_string(index)});
  title.assign("es", sourcemeta::core::JSON{"Titulo " + std::to_string(index)});
  title.assign("fr", sourcemeta::core::JSON{"Titre " + std::to_string(index)});
  return title;
}

static auto make_keywords() -> sourcemeta::core::JSON {
  auto keywords{sourcemeta::core::JSON::make_array()};
  for (std::size_t offset = 0; offset < keywords_per_member; offset += 1) {
    keywords.push_back(
        sourcemeta::core::JSON{"keyword-" + std::to_string(offset)});
  }
  return keywords;
}

static auto make_identifiers(const std::size_t index)
    -> sourcemeta::core::JSON {
  auto identifiers{sourcemeta::core::JSON::make_object()};
  identifiers.assign("isbn",
                     sourcemeta::core::JSON{"978-" + std::to_string(index)});
  identifiers.assign(
      "doi", sourcemeta::core::JSON{"10.1000/" + std::to_string(index)});
  identifiers.assign("oclc",
                     sourcemeta::core::JSON{"oclc-" + std::to_string(index)});
  return identifiers;
}

static auto make_price(const std::size_t index) -> sourcemeta::core::JSON {
  auto price{sourcemeta::core::JSON::make_object()};
  price.assign("currency", sourcemeta::core::JSON{currency_code(index)});
  price.assign("value",
               sourcemeta::core::JSON{static_cast<double>(index) + 0.99});
  return price;
}

static auto make_organization(const std::size_t index)
    -> sourcemeta::core::JSON {
  auto organization{sourcemeta::core::JSON::make_object()};
  organization.assign("id",
                      sourcemeta::core::JSON{"org-" + std::to_string(index)});
  organization.assign(
      "name", sourcemeta::core::JSON{"Organization " + std::to_string(index)});
  organization.assign("url", sourcemeta::core::JSON{"https://example.com/org/" +
                                                    std::to_string(index)});
  return organization;
}

static auto make_series(const std::size_t index) -> sourcemeta::core::JSON {
  auto series{sourcemeta::core::JSON::make_object()};
  series.assign("id",
                sourcemeta::core::JSON{"series-" + std::to_string(index)});
  series.assign("name",
                sourcemeta::core::JSON{"Series " + std::to_string(index)});
  return series;
}

static auto make_metadata(const std::size_t index) -> sourcemeta::core::JSON {
  auto metadata{sourcemeta::core::JSON::make_object()};
  metadata.assign("source", sourcemeta::core::JSON{"registry"});
  metadata.assign("batch",
                  sourcemeta::core::JSON{"batch-" + std::to_string(index)});
  return metadata;
}

static auto make_provenance(const std::size_t index) -> sourcemeta::core::JSON {
  auto provenance{sourcemeta::core::JSON::make_object()};
  provenance.assign("id",
                    sourcemeta::core::JSON{"prov-" + std::to_string(index)});
  provenance.assign("generatedBy", sourcemeta::core::JSON{
                                       "Importer " + std::to_string(index)});
  return provenance;
}

static auto make_member(const std::size_t index) -> sourcemeta::core::JSON {
  auto member{sourcemeta::core::JSON::make_object()};
  member.assign("isbn",
                sourcemeta::core::JSON{"978-0-" + std::to_string(index)});
  member.assign("title", make_title(index));
  member.assign("abstract",
                sourcemeta::core::JSON{"Abstract " + std::to_string(index)});
  member.assign("datePublished", sourcemeta::core::JSON{"2020-05-15"});

  auto authors{sourcemeta::core::JSON::make_array()};
  for (std::size_t offset = 0; offset < authors_per_member; offset += 1) {
    authors.push_back(make_person(index * 10 + offset));
  }
  member.assign("authors", std::move(authors));

  member.assign("keywords", make_keywords());
  member.assign("identifiers", make_identifiers(index));
  member.assign("price", make_price(index));
  member.assign("publisher", make_organization(index));
  member.assign("series", make_series(index));
  member.assign("metadata", make_metadata(index));
  member.assign("provenance", make_provenance(index));
  return member;
}

static auto make_catalog() -> sourcemeta::core::JSON {
  auto members{sourcemeta::core::JSON::make_array()};
  for (std::size_t index = 0; index < catalog_member_count; index += 1) {
    members.push_back(make_member(index));
  }

  auto catalog{sourcemeta::core::JSON::make_object()};
  catalog.assign("members", std::move(members));
  return catalog;
}

static auto
populate_member(sourcemeta::core::JSONLDWeakAnnotationList &annotations,
                const std::size_t index) -> void {
  using sourcemeta::core::WeakPointer;
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDNode{
              .id = "urn:isbn:978-0-" + std::to_string(index),
              .types = {"https://schema.org/Book"}}});
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(isbn_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/isbn", false}},
          .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(title_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/name", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Language}});
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(abstract_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/abstract", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .language = "en",
              .direction = sourcemeta::core::JSONLDDirection::LTR}});
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(date_published_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/datePublished", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#date"}});

  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(authors_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/author", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  for (std::size_t offset = 0; offset < authors_per_member; offset += 1) {
    annotations.emplace_back(
        WeakPointer{std::cref(members_key), index, std::cref(authors_key),
                    offset},
        sourcemeta::core::JSONLDDescriptor{
            .edges = {},
            .value = sourcemeta::core::JSONLDNode{
                .id = "https://example.com/people/person-" +
                      std::to_string(index * 10 + offset),
                .types = {"https://schema.org/Person"}}});
    annotations.emplace_back(WeakPointer{std::cref(members_key), index,
                                         std::cref(authors_key), offset,
                                         std::cref(name_key)},
                             sourcemeta::core::JSONLDDescriptor{
                                 .edges = {{"https://schema.org/name", false}},
                                 .value = sourcemeta::core::JSONLDLiteral{}});
  }

  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(keywords_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/keywords", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});

  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(identifiers_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/identifier", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Index}});

  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(price_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/offers", false}},
          .value = sourcemeta::core::JSONLDNode{}});
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(price_key),
                  std::cref(currency_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/priceCurrency", false}},
          .value = sourcemeta::core::JSONLDReference{
              .id = "https://www.iso.org/iso-4217/" + currency_code(index),
              .types = {"https://schema.org/Currency"}}});
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(price_key),
                  std::cref(value_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/price", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(publisher_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/publisher", false}},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/org/" + std::to_string(index),
              .types = {"https://schema.org/Organization"}}});
  annotations.emplace_back(WeakPointer{std::cref(members_key), index,
                                       std::cref(publisher_key),
                                       std::cref(name_key)},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(publisher_key),
                  std::cref(url_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/url", false}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#anyURI"}});

  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(series_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/hasPart", true}},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/series/" + std::to_string(index),
              .types = {"https://schema.org/CreativeWorkSeries"}}});
  annotations.emplace_back(WeakPointer{std::cref(members_key), index,
                                       std::cref(series_key),
                                       std::cref(name_key)},
                           sourcemeta::core::JSONLDDescriptor{
                               .edges = {{"https://schema.org/name", false}},
                               .value = sourcemeta::core::JSONLDLiteral{}});

  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(metadata_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/additionalProperty", false}},
          .value = sourcemeta::core::JSONLDLiteral{.json = true}});

  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(provenance_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://www.w3.org/ns/prov#has_provenance", false}},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/provenance/" + std::to_string(index),
              .graph = true}});
  annotations.emplace_back(
      WeakPointer{std::cref(members_key), index, std::cref(provenance_key),
                  std::cref(generated_by_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://www.w3.org/ns/prov#wasGeneratedBy", false}},
          .value = sourcemeta::core::JSONLDLiteral{}});
}

static auto populate_annotation_list(
    sourcemeta::core::JSONLDWeakAnnotationList &annotations) -> void {
  annotations.reserve(total_annotation_count);
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/catalog",
              .types = {"https://schema.org/DataCatalog"}}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{std::cref(members_key)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{"https://schema.org/dataset", false}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  for (std::size_t index = 0; index < catalog_member_count; index += 1) {
    populate_member(annotations, index);
  }
}

static void JSONLD_Catalog_Annotation_List_Populate(benchmark::State &state) {
  for (auto _ : state) {
    sourcemeta::core::JSONLDWeakAnnotationList annotations;
    populate_annotation_list(annotations);
    assert(annotations.size() == total_annotation_count);
    benchmark::DoNotOptimize(annotations);
  }
}

static void JSONLD_Catalog_Materialize(benchmark::State &state) {
  const auto instance{make_catalog()};
  sourcemeta::core::JSONLDWeakAnnotationList annotations;
  populate_annotation_list(annotations);
  assert(annotations.size() == total_annotation_count);

  for (auto _ : state) {
    auto result{sourcemeta::core::jsonld_materialize(instance, annotations)};
    assert(result.is_array());
    assert(!result.empty());
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(JSONLD_Catalog_Annotation_List_Populate);
BENCHMARK(JSONLD_Catalog_Materialize);
