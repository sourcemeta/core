#include <gtest/gtest.h>
#include <sourcemeta/core/jsonschema.h>

#include <algorithm>

#include "jsonschema_test_utils.h"

static const std::map<std::string, bool> VOCABULARIES_2020_12_CORE{
    {"https://json-schema.org/draft/2020-12/vocab/core", true}};

static const std::map<std::string, bool> VOCABULARIES_2020_12_APPLICATOR{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2020-12/vocab/applicator", true}};

static const std::map<std::string, bool> VOCABULARIES_2020_12_UNEVALUATED{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2020-12/vocab/unevaluated", true}};

static const std::map<std::string, bool> VOCABULARIES_2020_12_VALIDATION{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2020-12/vocab/validation", true}};

static const std::map<std::string, bool> VOCABULARIES_2020_12_FORMAT_ANNOTATION{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2020-12/vocab/format-annotation", true}};

static const std::map<std::string, bool> VOCABULARIES_2020_12_FORMAT_ASSERTION{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2020-12/vocab/format-assertion", true}};

static const std::map<std::string, bool> VOCABULARIES_2020_12_CONTENT{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2020-12/vocab/content", true}};

static const std::map<std::string, bool> VOCABULARIES_2020_12_METADATA{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2020-12/vocab/meta-data", true}};

static const std::map<std::string, bool> VOCABULARIES_2020_12_HYPERSCHEMA{
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2019-09/vocab/hyper-schema", true}};

TEST(JSONSchema_official_walker_2020_12, core_schema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$schema", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, core_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$vocabulary", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, core_id) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$id", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, core_anchor) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$anchor", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, core_dynamicAnchor) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$dynamicAnchor", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, core_ref) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$ref", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Reference);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, core_dynamicRef) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$dynamicRef", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Reference);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, core_defs) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$defs", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::LocationMembers);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, core_comment) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$comment", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_allOf) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("allOf", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_anyOf) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anyOf", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_oneOf) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("oneOf", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_not) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("not", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceNegate);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_if) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("if", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceMaybe);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_then) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("then", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceMaybe);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  const std::set<std::string> expected{"if"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12, applicator_else) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("else", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceMaybe);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  const std::set<std::string> expected{"if"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12, applicator_dependentSchemas) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("dependentSchemas",
                                           VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorMembersInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_prefixItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("prefixItems", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsTraverseItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_items) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("items", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseSomeItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  const std::set<std::string> expected{"prefixItems"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12, applicator_contains_only) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contains", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_contains_with_validation) {
  using namespace sourcemeta::core;
  std::map<std::string, bool> vocabularies;
  std::copy(VOCABULARIES_2020_12_APPLICATOR.cbegin(),
            VOCABULARIES_2020_12_APPLICATOR.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  std::copy(VOCABULARIES_2020_12_VALIDATION.cbegin(),
            VOCABULARIES_2020_12_VALIDATION.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  const auto result{schema_official_walker("contains", vocabularies)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  const std::set<std::string> expected{"minContains", "maxContains"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12, applicator_properties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("properties", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyStatic);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_patternProperties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("patternProperties",
                                           VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyRegex);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_additionalProperties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("additionalProperties",
                                           VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  const std::set<std::string> expected{"properties", "patternProperties"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12, applicator_propertyNames) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("propertyNames", VOCABULARIES_2020_12_APPLICATOR)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseAnyPropertyKey);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, unevaluated_unevaluatedItems_only) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("unevaluatedItems",
                                           VOCABULARIES_2020_12_UNEVALUATED)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseSomeItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/unevaluated");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     unevaluated_unevaluatedItems_with_applicator) {
  using namespace sourcemeta::core;
  std::map<std::string, bool> vocabularies;
  std::copy(VOCABULARIES_2020_12_UNEVALUATED.cbegin(),
            VOCABULARIES_2020_12_UNEVALUATED.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  std::copy(VOCABULARIES_2020_12_APPLICATOR.cbegin(),
            VOCABULARIES_2020_12_APPLICATOR.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  const auto result{schema_official_walker("unevaluatedItems", vocabularies)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseSomeItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/unevaluated");
  const std::set<std::string> expected{"prefixItems", "items", "contains"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12,
     unevaluated_unevaluatedProperties_only) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("unevaluatedProperties",
                                           VOCABULARIES_2020_12_UNEVALUATED)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/unevaluated");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     unevaluated_unevaluatedProperties_with_applicator) {
  using namespace sourcemeta::core;
  std::map<std::string, bool> vocabularies;
  std::copy(VOCABULARIES_2020_12_UNEVALUATED.cbegin(),
            VOCABULARIES_2020_12_UNEVALUATED.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  std::copy(VOCABULARIES_2020_12_APPLICATOR.cbegin(),
            VOCABULARIES_2020_12_APPLICATOR.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  const auto result{
      schema_official_walker("unevaluatedProperties", vocabularies)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/unevaluated");
  const std::set<std::string> expected{"properties", "patternProperties",
                                       "additionalProperties"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12, validation_type) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("type", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_enum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("enum", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_const) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("const", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_multipleOf) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("multipleOf", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_maximum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maximum", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  const std::set<std::string> expected{"type"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12, validation_minimum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minimum", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  const std::set<std::string> expected{"type"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_2020_12, validation_exclusiveMaximum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("exclusiveMaximum",
                                           VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_exclusiveMinimum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("exclusiveMinimum",
                                           VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_maxLength) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxLength", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_minLength) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minLength", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_pattern) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("pattern", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_maxItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxItems", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_minItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minItems", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_uniqueItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("uniqueItems", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_maxContains) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxContains", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_minContains) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minContains", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_maxProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxProperties", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_minProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minProperties", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_required) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("required", VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_dependentRequired) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("dependentRequired",
                                           VOCABULARIES_2020_12_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, format_annotation_format) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("format", VOCABULARIES_2020_12_FORMAT_ANNOTATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/format-annotation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, format_assertion_format) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("format", VOCABULARIES_2020_12_FORMAT_ASSERTION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/format-assertion");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, content_contentEncoding) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentEncoding", VOCABULARIES_2020_12_CONTENT)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/content");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, content_contentMediaType) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentMediaType", VOCABULARIES_2020_12_CONTENT)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/content");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, content_contentSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentSchema", VOCABULARIES_2020_12_CONTENT)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/content");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_title) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("title", VOCABULARIES_2020_12_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_description) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("description", VOCABULARIES_2020_12_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_default) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("default", VOCABULARIES_2020_12_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_deprecated) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("deprecated", VOCABULARIES_2020_12_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_readOnly) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("readOnly", VOCABULARIES_2020_12_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_writeOnly) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("writeOnly", VOCABULARIES_2020_12_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_examples) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("examples", VOCABULARIES_2020_12_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_base) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("base", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_links) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("links", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_ref) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("ref", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_href) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("href", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_anchor) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchor", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_anchorPointer) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("anchorPointer",
                                           VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_rel) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("rel", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_templatePointers) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("templatePointers",
                                           VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_templateRequired) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("templateRequired",
                                           VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_targetMediaType) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("targetMediaType",
                                           VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_targetHints) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetHints", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_submissionMediaType) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("submissionMediaType",
                                           VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_hrefSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("hrefSchema", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/hyper-schema");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_targetSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/hyper-schema");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_headerSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("headerSchema", VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/hyper-schema");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_submissionSchema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("submissionSchema",
                                           VOCABULARIES_2020_12_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/hyper-schema");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_allOf_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("allOf", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_anyOf_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("anyOf", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_oneOf_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("oneOf", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_not_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("not", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_if_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("if", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_then_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("then", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_else_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("else", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     applicator_dependentSchemas_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("dependentSchemas", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     applicator_prefixItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("prefixItems", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, applicator_items_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("items", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     applicator_contains_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contains", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     applicator_properties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("properties", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     applicator_patternProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("patternProperties", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     applicator_additionalProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("additionalProperties",
                                           VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     applicator_propertyNames_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("propertyNames", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     unevaluated_unevaluatedItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("unevaluatedItems", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     unevaluated_unevaluatedProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("unevaluatedProperties",
                                           VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_type_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("type", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_enum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("enum", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, validation_const_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("const", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_multipleOf_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("multipleOf", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_maximum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maximum", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_minimum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minimum", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_exclusiveMaximum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMaximum", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_exclusiveMinimum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMinimum", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_maxLength_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxLength", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_minLength_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minLength", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_pattern_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("pattern", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_maxItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxItems", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_minItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minItems", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_uniqueItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("uniqueItems", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_maxContains_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxContains", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_minContains_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minContains", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_maxProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxProperties", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_minProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minProperties", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_required_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("required", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     validation_dependentRequired_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("dependentRequired", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     format_annotation_format_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("format", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     format_assertion_format_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("format", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     content_contentEncoding_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentEncoding", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     content_contentMediaType_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentMediaType", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     content_contentSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentSchema", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_title_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("title", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     metadata_description_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("description", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_default_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("default", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     metadata_deprecated_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("deprecated", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_readOnly_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("readOnly", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     metadata_writeOnly_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("writeOnly", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, metadata_examples_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("examples", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_base_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("base", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_links_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("links", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_ref_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("ref", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_href_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("href", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_anchor_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchor", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_anchorPointer_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchorPointer", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, hyperschema_rel_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("rel", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_templatePointers_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("templatePointers", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_templateRequired_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("templateRequired", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_targetMediaType_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetMediaType", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_targetHints_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetHints", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_submissionMediaType_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("submissionMediaType", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_hrefSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("hrefSchema", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_targetSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_headerSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("headerSchema", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12,
     hyperschema_submissionSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("submissionSchema", VOCABULARIES_2020_12_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_2020_12, schema_keyword_priority_array) {
  std::map<std::string, bool> vocabularies;
  std::copy(VOCABULARIES_2020_12_APPLICATOR.cbegin(),
            VOCABULARIES_2020_12_APPLICATOR.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  std::copy(VOCABULARIES_2020_12_UNEVALUATED.cbegin(),
            VOCABULARIES_2020_12_UNEVALUATED.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  std::copy(VOCABULARIES_2020_12_VALIDATION.cbegin(),
            VOCABULARIES_2020_12_VALIDATION.cend(),
            std::inserter(vocabularies, vocabularies.end()));

  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("prefixItems", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("items", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("minContains", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("maxContains", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("contains", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("unevaluatedItems", vocabularies, walker),
            2);
}

TEST(JSONSchema_official_walker_2020_12, schema_keyword_priority_object) {
  std::map<std::string, bool> vocabularies;
  std::copy(VOCABULARIES_2020_12_APPLICATOR.cbegin(),
            VOCABULARIES_2020_12_APPLICATOR.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  std::copy(VOCABULARIES_2020_12_UNEVALUATED.cbegin(),
            VOCABULARIES_2020_12_UNEVALUATED.cend(),
            std::inserter(vocabularies, vocabularies.end()));

  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("properties", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("patternProperties", vocabularies, walker),
            0);
  EXPECT_EQ(
      schema_keyword_priority("additionalProperties", vocabularies, walker), 1);
  EXPECT_EQ(
      schema_keyword_priority("unevaluatedProperties", vocabularies, walker),
      2);
}

TEST(JSONSchema_official_walker_2020_12, schema_keyword_priority_other) {
  const auto &vocabularies = VOCABULARIES_2020_12_APPLICATOR;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("if", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("then", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("else", vocabularies, walker), 1);
}

TEST(JSONSchema_official_walker_2020_12, schema_keyword_priority_unknown) {
  const auto &vocabularies = VOCABULARIES_2020_12_CORE;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("foobar", vocabularies, walker), 0);
}

TEST(JSONSchema_official_walker_2020_12, instance_locations) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "allOf": [ { "type": "string" }, { "minLength": 3 } ],
    "anyOf": [ { "type": "string" } ],
    "oneOf": [ { "type": "string" } ],
    "if": { "const": "foo" },
    "then": true,
    "else": { "maxLength": 10 },
    "not": { "const": "bar" },
    "properties": {
      "foo": { "type": "string" },
      "bar": false
    },
    "additionalProperties": { "type": "number" },
    "patternProperties": {
      "^f": { "type": "integer" },
      "x$": true
    },
    "dependentSchemas": {
      "foo": { "minProperties": 1 },
      "bar": { "maxProperties": 10 }
    },
    "propertyNames": { "minLength": 1 },
    "contains": { "type": "number" },
    "items": { "minimum": 2 },
    "prefixItems": [ true, false ],
    "unevaluatedProperties": { "type": "number" },
    "unevaluatedItems": { "minimum": 2 },
    "contentSchema": { "type": "string" },
    "$defs": {
      "foo": {
        "properties": {
          "bar": { "type": "string" }
        }
      }
    },
    "definitions": { "foo": true }
  })JSON");

  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIterator(
           document, sourcemeta::core::schema_official_walker,
           sourcemeta::core::schema_official_resolver)) {
    entries.push_back(entry);
  }

  EXPECT_EQ(entries.size(), 27);

  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 0, "", std::nullopt, "", "");

  // Applicator vocabulary (any)
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 1, "/allOf/0", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 2, "/allOf/1", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 3, "/anyOf/0", "", "/~?anyOf~",
                                       "/~?anyOf~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 4, "/oneOf/0", "", "/~?oneOf~",
                                       "/~?oneOf~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 5, "/if", "", "/~?if~",
                                       "/~?if~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 6, "/then", "", "/~?then~",
                                       "/~?then~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 7, "/else", "", "/~?else~",
                                       "/~?else~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 8, "/not", "", "/~!~", "/~!~");

  // Applicator vocabulary (object)
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 9, "/properties/foo", "",
                                       "/foo", "/foo");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 10, "/properties/bar", "",
                                       "/bar", "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 11, "/additionalProperties", "",
                                       "/~?additionalProperties~/~P~",
                                       "/~?additionalProperties~/~P~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 12, "/patternProperties/^f", "",
                                       "/~R^f~", "/~R^f~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 13, "/patternProperties/x$", "",
                                       "/~Rx$~", "/~Rx$~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 14, "/dependentSchemas/foo", "",
                                       "/~?dependentSchemas~/~?foo~",
                                       "/~?dependentSchemas~/~?foo~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 15, "/dependentSchemas/bar", "",
                                       "/~?dependentSchemas~/~?bar~",
                                       "/~?dependentSchemas~/~?bar~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 16, "/propertyNames", "",
                                       "/~K~", "/~K~");

  // Applicator vocabulary (array)
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 17, "/contains", "", "/~I~",
                                       "/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 18, "/items", "",
                                       "/~?items~/~I~", "/~?items~/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 19, "/prefixItems/0", "", "/0",
                                       "/0");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 20, "/prefixItems/1", "", "/1",
                                       "/1");

  // Unevaluated vocabulary
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 21, "/unevaluatedProperties",
                                       "", "/~?unevaluatedProperties~/~P~",
                                       "/~?unevaluatedProperties~/~P~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 22, "/unevaluatedItems", "",
                                       "/~?unevaluatedItems~/~I~",
                                       "/~?unevaluatedItems~/~I~");

  // Content vocabulary
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 23, "/contentSchema", "", "",
                                       "");

  // Core vocabulary
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12_ORPHAN(entries, 24, "/$defs/foo", "", "",
                                              "");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12_ORPHAN(
      entries, 25, "/$defs/foo/properties/bar", "/$defs/foo", "/bar", "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12_ORPHAN(entries, 26, "/definitions/foo",
                                              "", "", "");
}

TEST(JSONSchema_official_walker_2020_12, instance_locations_nested) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "additionalProperties": {
      "properties": {
        "foo": {
          "allOf": [
            { "type": "string" }
          ]
        }
      }
    }
  })JSON");

  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIterator(
           document, sourcemeta::core::schema_official_walker,
           sourcemeta::core::schema_official_resolver)) {
    entries.push_back(entry);
  }

  EXPECT_EQ(entries.size(), 4);

  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 0, "", std::nullopt, "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 1, "/additionalProperties", "",
                                       "/~?additionalProperties~/~P~",
                                       "/~?additionalProperties~/~P~");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(
      entries, 2, "/additionalProperties/properties/foo",
      "/additionalProperties", "/~?additionalProperties~/~P~/foo", "/foo");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(
      entries, 3, "/additionalProperties/properties/foo/allOf/0",
      "/additionalProperties/properties/foo",
      "/~?additionalProperties~/~P~/foo", "");
}

TEST(JSONSchema_official_walker_2020_12, instance_locations_defs_with_ref) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "common",
      "allOf": [ { "$ref": "#foo" } ],
      "$defs": {
        "foo": {
          "$anchor": "foo"
        }
      }
  })JSON");

  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIterator(
           document, sourcemeta::core::schema_official_walker,
           sourcemeta::core::schema_official_resolver)) {
    entries.push_back(entry);
  }

  EXPECT_EQ(entries.size(), 3);

  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 0, "", std::nullopt, "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 1, "/allOf/0", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12_ORPHAN(entries, 2, "/$defs/foo", "", "",
                                              "");
}

TEST(JSONSchema_official_walker_2020_12, definitions_subschemas) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "foo": {
        "properties": {
          "bar": {
            "additionalProperties": {
              "type": "string"
            }
          }
        }
      }
    }
  })JSON");

  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIterator(
           document, sourcemeta::core::schema_official_walker,
           sourcemeta::core::schema_official_resolver)) {
    entries.push_back(entry);
  }

  EXPECT_EQ(entries.size(), 4);

  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12(entries, 0, "", std::nullopt, "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12_ORPHAN(entries, 1, "/$defs/foo", "", "",
                                              "");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12_ORPHAN(
      entries, 2, "/$defs/foo/properties/bar", "/$defs/foo", "/bar", "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_2020_12_ORPHAN(
      entries, 3, "/$defs/foo/properties/bar/additionalProperties",
      "/$defs/foo/properties/bar", "/bar/~?additionalProperties~/~P~",
      "/~?additionalProperties~/~P~");
}
