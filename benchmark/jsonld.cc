#include <benchmark/benchmark.h>

#include <cassert>    // assert
#include <cstddef>    // std::size_t
#include <functional> // std::cref
#include <string>     // std::string, std::to_string
#include <utility>    // std::move

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

// The catalog dimensions set how many entries the annotation list carries
// and how many instance positions materialization visits, so they are the
// knobs to turn when stress testing this functionality
static constexpr std::size_t CATALOG_MEMBER_COUNT{256};
static constexpr std::size_t AUTHORS_PER_MEMBER{3};
static constexpr std::size_t KEYWORDS_PER_MEMBER{3};
static constexpr std::size_t ANNOTATIONS_PER_MEMBER{19 +
                                                    (AUTHORS_PER_MEMBER * 2)};
static constexpr std::size_t TOTAL_ANNOTATION_COUNT{
    2 + (CATALOG_MEMBER_COUNT * ANNOTATIONS_PER_MEMBER)};

struct CatalogKeys {
  const sourcemeta::core::JSON::String members{"members"};
  const sourcemeta::core::JSON::String isbn{"isbn"};
  const sourcemeta::core::JSON::String title{"title"};
  const sourcemeta::core::JSON::String abstract{"abstract"};
  const sourcemeta::core::JSON::String date_published{"datePublished"};
  const sourcemeta::core::JSON::String authors{"authors"};
  const sourcemeta::core::JSON::String name{"name"};
  const sourcemeta::core::JSON::String keywords{"keywords"};
  const sourcemeta::core::JSON::String identifiers{"identifiers"};
  const sourcemeta::core::JSON::String price{"price"};
  const sourcemeta::core::JSON::String currency{"currency"};
  const sourcemeta::core::JSON::String value{"value"};
  const sourcemeta::core::JSON::String publisher{"publisher"};
  const sourcemeta::core::JSON::String url{"url"};
  const sourcemeta::core::JSON::String series{"series"};
  const sourcemeta::core::JSON::String metadata{"metadata"};
  const sourcemeta::core::JSON::String provenance{"provenance"};
  const sourcemeta::core::JSON::String generated_by{"generatedBy"};
};

// The pointers these benchmarks build borrow their tokens rather than own
// them, so the keys have to outlive every list that refers back to them
static auto catalog_keys() -> const CatalogKeys & {
  static const CatalogKeys INSTANCE;
  return INSTANCE;
}

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
  for (std::size_t offset = 0; offset < KEYWORDS_PER_MEMBER; offset += 1) {
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
  for (std::size_t offset = 0; offset < AUTHORS_PER_MEMBER; offset += 1) {
    authors.push_back(make_person((index * 10) + offset));
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
  for (std::size_t index = 0; index < CATALOG_MEMBER_COUNT; index += 1) {
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
  const auto &keys{catalog_keys()};
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDNode{
              .id = "urn:isbn:978-0-" + std::to_string(index),
              .types = {"https://schema.org/Book"}}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.isbn)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/isbn"}},
          .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.title)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/name"}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Language}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.abstract)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/abstract"}},
          .value = sourcemeta::core::JSONLDLiteral{
              .language = "en",
              .direction = sourcemeta::core::JSONLDDirection::LTR}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index,
                  std::cref(keys.date_published)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/datePublished"}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#date"}});

  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.authors)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/author"}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::List}});
  for (std::size_t offset = 0; offset < AUTHORS_PER_MEMBER; offset += 1) {
    annotations.emplace_back(
        WeakPointer{std::cref(keys.members), index, std::cref(keys.authors),
                    offset},
        sourcemeta::core::JSONLDDescriptor{
            .edges = {},
            .value = sourcemeta::core::JSONLDNode{
                .id = "https://example.com/people/person-" +
                      std::to_string((index * 10) + offset),
                .types = {"https://schema.org/Person"}}});
    annotations.emplace_back(
        WeakPointer{std::cref(keys.members), index, std::cref(keys.authors),
                    offset, std::cref(keys.name)},
        sourcemeta::core::JSONLDDescriptor{
            .edges = {{.predicate = "https://schema.org/name"}},
            .value = sourcemeta::core::JSONLDLiteral{}});
  }

  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.keywords)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/keywords"}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});

  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.identifiers)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/identifier"}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Index}});

  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.price)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/offers"}},
          .value = sourcemeta::core::JSONLDNode{}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.price),
                  std::cref(keys.currency)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/priceCurrency"}},
          .value = sourcemeta::core::JSONLDReference{
              .id = "https://www.iso.org/iso-4217/" + currency_code(index),
              .types = {"https://schema.org/Currency"}}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.price),
                  std::cref(keys.value)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/price"}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#decimal"}});

  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.publisher)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/publisher"}},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/org/" + std::to_string(index),
              .types = {"https://schema.org/Organization"}}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.publisher),
                  std::cref(keys.name)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/name"}},
          .value = sourcemeta::core::JSONLDLiteral{}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.publisher),
                  std::cref(keys.url)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/url"}},
          .value = sourcemeta::core::JSONLDLiteral{
              .datatype = "http://www.w3.org/2001/XMLSchema#anyURI"}});

  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.series)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/hasPart",
                     .reverse = true}},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/series/" + std::to_string(index),
              .types = {"https://schema.org/CreativeWorkSeries"}}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.series),
                  std::cref(keys.name)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/name"}},
          .value = sourcemeta::core::JSONLDLiteral{}});

  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.metadata)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/additionalProperty"}},
          .value = sourcemeta::core::JSONLDLiteral{.json = true}});

  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.provenance)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://www.w3.org/ns/prov#has_provenance"}},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/provenance/" + std::to_string(index),
              .graph = true}});
  annotations.emplace_back(
      WeakPointer{std::cref(keys.members), index, std::cref(keys.provenance),
                  std::cref(keys.generated_by)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://www.w3.org/ns/prov#wasGeneratedBy"}},
          .value = sourcemeta::core::JSONLDLiteral{}});
}

static auto populate_annotation_list(
    sourcemeta::core::JSONLDWeakAnnotationList &annotations) -> void {
  const auto &keys{catalog_keys()};
  annotations.reserve(TOTAL_ANNOTATION_COUNT);
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {},
          .value = sourcemeta::core::JSONLDNode{
              .id = "https://example.com/catalog",
              .types = {"https://schema.org/DataCatalog"}}});
  annotations.emplace_back(
      sourcemeta::core::WeakPointer{std::cref(keys.members)},
      sourcemeta::core::JSONLDDescriptor{
          .edges = {{.predicate = "https://schema.org/dataset"}},
          .value = sourcemeta::core::JSONLDCollection{
              .container = sourcemeta::core::JSONLDContainer::Set}});
  for (std::size_t index = 0; index < CATALOG_MEMBER_COUNT; index += 1) {
    populate_member(annotations, index);
  }
}

// GoogleBenchmark reports the name of each of these functions as the label
// of its result, and the tooling that tracks those results over time keys
// its history on that label, so they do not follow the usual convention
// NOLINTNEXTLINE(readability-identifier-naming)
static void JSONLD_Catalog_Annotation_List_Populate(benchmark::State &state) {
  for (auto iteration : state) {
    sourcemeta::core::JSONLDWeakAnnotationList annotations;
    populate_annotation_list(annotations);
    assert(annotations.size() == TOTAL_ANNOTATION_COUNT);
    benchmark::DoNotOptimize(annotations);
  }
}

// NOLINTNEXTLINE(readability-identifier-naming)
static void JSONLD_Catalog_Materialize(benchmark::State &state) {
  const auto instance{make_catalog()};
  sourcemeta::core::JSONLDWeakAnnotationList annotations;
  populate_annotation_list(annotations);
  assert(annotations.size() == TOTAL_ANNOTATION_COUNT);

  for (auto iteration : state) {
    auto result{sourcemeta::core::jsonld_materialize(instance, annotations)};
    assert(result.is_array());
    assert(!result.empty());
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(JSONLD_Catalog_Annotation_List_Populate);
BENCHMARK(JSONLD_Catalog_Materialize);
