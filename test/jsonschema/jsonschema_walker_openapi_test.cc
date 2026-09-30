#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonschema.h>

#include <algorithm>   // std::ranges::equal
#include <array>       // std::to_array
#include <string_view> // std::string_view
#include <variant>     // std::holds_alternative, std::get

#define EXPECT_VOCABULARY_KNOWN(vocabulary_value, expected_known)              \
  EXPECT_TRUE(                                                                 \
      std::holds_alternative<sourcemeta::core::SchemaVocabularies::Known>(     \
          (vocabulary_value)));                                                \
  EXPECT_EQ(std::get<sourcemeta::core::SchemaVocabularies::Known>(             \
                (vocabulary_value)),                                           \
            sourcemeta::core::SchemaVocabularies::Known::expected_known)

// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
static const sourcemeta::core::SchemaVocabularies VOCABULARIES_OPENAPI_3_1{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://spec.openapis.org/oas/3.1/vocab/base", true}};

static const sourcemeta::core::SchemaVocabularies VOCABULARIES_OPENAPI_3_2{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://spec.openapis.org/oas/3.2/vocab/base", true}};

static const sourcemeta::core::SchemaVocabularies VOCABULARIES_OPENAPI_3_0{
    {"tag:spec.openapis.org,2017:oas/3.0/vocab/base", true}};

static const sourcemeta::core::SchemaVocabularies VOCABULARIES_2020_12_CORE{
    {"https://json-schema.org/draft/2020-12/vocab/core", true}};

// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)
TEST(openapi_3_1_discriminator) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("discriminator", VOCABULARIES_OPENAPI_3_1)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_1_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_1_discriminator_without_vocabulary) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("discriminator", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_1_xml) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("xml", VOCABULARIES_OPENAPI_3_1)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_1_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_1_xml_without_vocabulary) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("xml", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_1_externalDocs) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("externalDocs", VOCABULARIES_OPENAPI_3_1)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_1_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_1_externalDocs_without_vocabulary) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("externalDocs", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_1_example) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("example", VOCABULARIES_OPENAPI_3_1)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_1_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_1_example_without_vocabulary) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("example", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_2_discriminator) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("discriminator", VOCABULARIES_OPENAPI_3_2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_2_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_2_discriminator_without_vocabulary) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("discriminator", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_2_xml) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("xml", VOCABULARIES_OPENAPI_3_2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_2_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_2_xml_without_vocabulary) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("xml", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_2_externalDocs) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("externalDocs", VOCABULARIES_OPENAPI_3_2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_2_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_2_externalDocs_without_vocabulary) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("externalDocs", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_2_example) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("example", VOCABULARIES_OPENAPI_3_2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_2_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_2_example_without_vocabulary) {
  using namespace sourcemeta::core;
  using namespace sourcemeta::core;
  const auto &result{schema_walker("example", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_title) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("title", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_multipleOf) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("multipleOf", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Integer,
                                        sourcemeta::core::JSON::Type::Real}));
}

TEST(openapi_3_0_maximum) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("maximum", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Integer,
                                        sourcemeta::core::JSON::Type::Real}));
}

TEST(openapi_3_0_exclusiveMaximum) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("exclusiveMaximum", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Integer,
                                        sourcemeta::core::JSON::Type::Real}));
}

TEST(openapi_3_0_minimum) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("minimum", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Integer,
                                        sourcemeta::core::JSON::Type::Real}));
}

TEST(openapi_3_0_exclusiveMinimum) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("exclusiveMinimum", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Integer,
                                        sourcemeta::core::JSON::Type::Real}));
}

TEST(openapi_3_0_maxLength) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("maxLength", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::String}));
}

TEST(openapi_3_0_minLength) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("minLength", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::String}));
}

TEST(openapi_3_0_pattern) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("pattern", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::String}));
}

TEST(openapi_3_0_maxItems) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("maxItems", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Array}));
}

TEST(openapi_3_0_minItems) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("minItems", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Array}));
}

TEST(openapi_3_0_uniqueItems) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("uniqueItems", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Array}));
}

TEST(openapi_3_0_maxProperties) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("maxProperties", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Object}));
}

TEST(openapi_3_0_minProperties) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("minProperties", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Object}));
}

TEST(openapi_3_0_required) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("required", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Object}));
}

TEST(openapi_3_0_enum) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("enum", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_not) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("not", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceNegate);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_allOf) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("allOf", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_oneOf) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("oneOf", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_anyOf) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("anyOf", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_items) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("items", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Array}));
}

TEST(openapi_3_0_description) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("description", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_format) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("format", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::String}));
}

TEST(openapi_3_0_default) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("default", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_nullable) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("nullable", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_discriminator) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("discriminator", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_readOnly) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("readOnly", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_writeOnly) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("writeOnly", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_example) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("example", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_externalDocs) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("externalDocs", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_deprecated) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("deprecated", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_xml) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("xml", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_x_definitions) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("x-definitions", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::LocationMembers);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_type) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("type", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(
      std::ranges::equal(result.order_dependencies,
                         std::to_array<std::string_view>({"properties"})));
  EXPECT_TRUE(result.instances.none());
}

TEST(openapi_3_0_properties) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("properties", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyStatic);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(
      std::ranges::equal(result.order_dependencies,
                         std::to_array<std::string_view>({"required"})));
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Object}));
}

TEST(openapi_3_0_additionalProperties_defers_to_properties_only) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("additionalProperties", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_VOCABULARY_KNOWN(result.vocabulary.value(), OPENAPI_3_0_BASE);
  EXPECT_TRUE(std::ranges::equal(
      result.dependencies, std::to_array<std::string_view>({"properties"})));
  EXPECT_TRUE(result.order_dependencies.empty());
  EXPECT_EQ(result.instances,
            sourcemeta::core::make_set({sourcemeta::core::JSON::Type::Object}));
}

TEST(openapi_3_0_nullable_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("nullable", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_discriminator_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("discriminator", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_readOnly_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("readOnly", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_writeOnly_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("writeOnly", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_example_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("example", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_externalDocs_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("externalDocs", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_deprecated_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("deprecated", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_xml_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("xml", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_x_definitions_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("x-definitions", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
}

TEST(openapi_3_0_rejects_patternProperties) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("patternProperties", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dependencies) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("dependencies", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_additionalItems) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("additionalItems", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_definitions) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("definitions", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_id) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("id", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_schema) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$schema", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_ref) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$ref", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_defs) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$defs", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_id) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$id", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_anchor) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$anchor", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_comment) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$comment", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_const) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("const", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_contains) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("contains", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_if) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("if", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_then) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("then", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_else) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("else", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_propertyNames) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("propertyNames", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_unevaluatedProperties) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("unevaluatedProperties", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_unevaluatedItems) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("unevaluatedItems", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_examples) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("examples", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_contentEncoding) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("contentEncoding", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_contentMediaType) {
  using namespace sourcemeta::core;
  const auto &result{
      schema_walker("contentMediaType", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_contentSchema) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("contentSchema", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_prefixItems) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("prefixItems", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_maxContains) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("maxContains", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_minContains) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("minContains", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_divisibleBy) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("divisibleBy", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_extends) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("extends", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_disallow) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("disallow", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_links) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("links", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_media) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("media", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_pathStart) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("pathStart", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_vocabulary) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$vocabulary", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_dynamicRef) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$dynamicRef", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}

TEST(openapi_3_0_rejects_dollar_dynamicAnchor) {
  using namespace sourcemeta::core;
  const auto &result{schema_walker("$dynamicAnchor", VOCABULARIES_OPENAPI_3_0)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.order_dependencies.empty());
}
