#include <gtest/gtest.h>
#include <sourcemeta/core/jsonschema.h>

#include <algorithm>

static const std::map<std::string, bool> VOCABULARIES_2019_09_CORE{
    {"https://json-schema.org/draft/2019-09/vocab/core", true}};

static const std::map<std::string, bool> VOCABULARIES_2019_09_APPLICATOR{
    {"https://json-schema.org/draft/2019-09/vocab/core", true},
    {"https://json-schema.org/draft/2019-09/vocab/applicator", true}};

static const std::map<std::string, bool> VOCABULARIES_2019_09_VALIDATION{
    {"https://json-schema.org/draft/2019-09/vocab/core", true},
    {"https://json-schema.org/draft/2019-09/vocab/validation", true}};

static const std::map<std::string, bool> VOCABULARIES_2019_09_FORMAT{
    {"https://json-schema.org/draft/2019-09/vocab/core", true},
    {"https://json-schema.org/draft/2019-09/vocab/format", true}};

static const std::map<std::string, bool> VOCABULARIES_2019_09_CONTENT{
    {"https://json-schema.org/draft/2019-09/vocab/core", true},
    {"https://json-schema.org/draft/2019-09/vocab/content", true}};

static const std::map<std::string, bool> VOCABULARIES_2019_09_METADATA{
    {"https://json-schema.org/draft/2019-09/vocab/core", true},
    {"https://json-schema.org/draft/2019-09/vocab/meta-data", true}};

static const std::map<std::string, bool> VOCABULARIES_2019_09_HYPERSCHEMA{
    {"https://json-schema.org/draft/2019-09/vocab/core", true},
    {"https://json-schema.org/draft/2019-09/vocab/hyper-schema", true}};

TEST(JSONSchema_default_walker_2019_09, core_schema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$schema", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, core_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$vocabulary", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, core_id) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$id", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, core_anchor) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$anchor", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, core_recursiveAnchor) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$recursiveAnchor", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, core_ref) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$ref", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Reference);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, core_recursiveRef) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$recursiveRef", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Reference);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, core_defs) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$defs", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::LocationMembers);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, core_comment) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$comment", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_allOf) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("allOf", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInline);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_anyOf) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anyOf", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_oneOf) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("oneOf", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_not) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("not", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_if) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("if", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_then) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("then", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  const std::set<std::string> expected{"if"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, applicator_else) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("else", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  const std::set<std::string> expected{"if"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, applicator_dependentSchemas) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("dependentSchemas",
                                           VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorMembersInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_items) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("items", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueOrElements);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_additionalItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("additionalItems",
                                           VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValue);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  const std::set<std::string> expected{"items"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, applicator_contains_only) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contains", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_contains_with_validation) {
  using namespace sourcemeta::core;
  std::map<std::string, bool> vocabularies;
  std::copy(VOCABULARIES_2019_09_APPLICATOR.cbegin(),
            VOCABULARIES_2019_09_APPLICATOR.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  std::copy(VOCABULARIES_2019_09_VALIDATION.cbegin(),
            VOCABULARIES_2019_09_VALIDATION.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  const auto result{schema_official_walker("contains", vocabularies)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  const std::set<std::string> expected{"minContains", "maxContains"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, applicator_properties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("properties", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyStatic);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_patternProperties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("patternProperties",
                                           VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyRegex);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_additionalProperties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("additionalProperties",
                                           VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  const std::set<std::string> expected{"properties", "patternProperties"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, applicator_propertyNames) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("propertyNames", VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseAnyPropertyKey);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_unevaluatedItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("unevaluatedItems",
                                           VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  const std::set<std::string> expected{"items", "additionalItems"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, applicator_unevaluatedProperties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("unevaluatedProperties",
                                           VOCABULARIES_2019_09_APPLICATOR)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  const std::set<std::string> expected{"properties", "patternProperties",
                                       "additionalProperties"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, validation_type) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("type", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_enum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("enum", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_const) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("const", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_multipleOf) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("multipleOf", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_maximum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maximum", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  const std::set<std::string> expected{"type"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, validation_minimum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minimum", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  const std::set<std::string> expected{"type"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_default_walker_2019_09, validation_exclusiveMaximum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("exclusiveMaximum",
                                           VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_exclusiveMinimum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("exclusiveMinimum",
                                           VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_maxLength) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxLength", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_minLength) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minLength", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_pattern) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("pattern", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_maxItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxItems", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_minItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minItems", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_uniqueItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("uniqueItems", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_maxContains) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxContains", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_minContains) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minContains", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_maxProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxProperties", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_minProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minProperties", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_required) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("required", VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_dependentRequired) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("dependentRequired",
                                           VOCABULARIES_2019_09_VALIDATION)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, format_format) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("format", VOCABULARIES_2019_09_FORMAT)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/format");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, content_contentEncoding) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentEncoding", VOCABULARIES_2019_09_CONTENT)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/content");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, content_contentMediaType) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentMediaType", VOCABULARIES_2019_09_CONTENT)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/content");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, content_contentSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentSchema", VOCABULARIES_2019_09_CONTENT)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/content");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_title) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("title", VOCABULARIES_2019_09_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_description) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("description", VOCABULARIES_2019_09_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_default) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("default", VOCABULARIES_2019_09_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_deprecated) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("deprecated", VOCABULARIES_2019_09_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_readOnly) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("readOnly", VOCABULARIES_2019_09_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_writeOnly) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("writeOnly", VOCABULARIES_2019_09_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_examples) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("examples", VOCABULARIES_2019_09_METADATA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Annotation);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_base) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("base", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_links) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("links", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_ref) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("ref", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_href) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("href", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_anchor) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchor", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_anchorPointer) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("anchorPointer",
                                           VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_rel) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("rel", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_templatePointers) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("templatePointers",
                                           VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_templateRequired) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("templateRequired",
                                           VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_targetMediaType) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("targetMediaType",
                                           VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_targetHints) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetHints", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_submissionMediaType) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("submissionMediaType",
                                           VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_hrefSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("hrefSchema", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValue);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/hyper-schema");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_targetSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValue);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/hyper-schema");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_headerSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("headerSchema", VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValue);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/hyper-schema");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_submissionSchema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("submissionSchema",
                                           VOCABULARIES_2019_09_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValue);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "https://json-schema.org/draft/2019-09/vocab/hyper-schema");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_allOf_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("allOf", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_anyOf_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("anyOf", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_oneOf_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("oneOf", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_not_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("not", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_if_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("if", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_then_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("then", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_else_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("else", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_dependentSchemas_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("dependentSchemas", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_additionalItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("additionalItems", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, applicator_items_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("items", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_contains_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contains", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_properties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("properties", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_patternProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("patternProperties", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_additionalProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("additionalProperties",
                                           VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_propertyNames_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("propertyNames", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_unevaluatedItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("unevaluatedItems", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     applicator_unevaluatedProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("unevaluatedProperties",
                                           VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_type_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("type", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_enum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("enum", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_const_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("const", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_multipleOf_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("multipleOf", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_maximum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maximum", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_minimum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minimum", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_exclusiveMaximum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMaximum", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_exclusiveMinimum_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMinimum", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_maxLength_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxLength", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_minLength_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minLength", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, validation_pattern_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("pattern", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_maxItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxItems", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_minItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minItems", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_uniqueItems_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("uniqueItems", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_maxContains_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxContains", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_minContains_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minContains", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_maxProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxProperties", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_minProperties_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minProperties", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_required_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("required", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     validation_dependentRequired_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("dependentRequired", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, format_format_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("format", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     content_contentEncoding_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentEncoding", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     content_contentMediaType_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentMediaType", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     content_contentSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentSchema", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_title_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("title", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     metadata_description_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("description", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_default_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("default", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     metadata_deprecated_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("deprecated", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_readOnly_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("readOnly", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_writeOnly_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("writeOnly", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, metadata_examples_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("examples", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_base_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("base", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_links_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("links", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_ref_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("ref", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_href_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("href", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_anchor_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchor", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_anchorPointer_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchorPointer", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, hyperschema_rel_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("rel", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_templatePointers_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("templatePointers", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_templateRequired_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("templateRequired", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_targetMediaType_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetMediaType", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_targetHints_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetHints", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_submissionMediaType_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("submissionMediaType", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_hrefSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("hrefSchema", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_targetSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_headerSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("headerSchema", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09,
     hyperschema_submissionSchema_without_vocabulary) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("submissionSchema", VOCABULARIES_2019_09_CORE)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_default_walker_2019_09, schema_keyword_priority_array) {
  std::map<std::string, bool> vocabularies;
  std::copy(VOCABULARIES_2019_09_APPLICATOR.cbegin(),
            VOCABULARIES_2019_09_APPLICATOR.cend(),
            std::inserter(vocabularies, vocabularies.end()));
  std::copy(VOCABULARIES_2019_09_VALIDATION.cbegin(),
            VOCABULARIES_2019_09_VALIDATION.cend(),
            std::inserter(vocabularies, vocabularies.end()));

  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("items", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("additionalItems", vocabularies, walker),
            1);
  EXPECT_EQ(schema_keyword_priority("minContains", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("maxContains", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("contains", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("unevaluatedItems", vocabularies, walker),
            2);
}

TEST(JSONSchema_default_walker_2019_09, schema_keyword_priority_object) {
  const auto &vocabularies = VOCABULARIES_2019_09_APPLICATOR;
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

TEST(JSONSchema_default_walker_2019_09, schema_keyword_priority_other) {
  const auto &vocabularies = VOCABULARIES_2019_09_APPLICATOR;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("if", vocabularies, walker), 0);
  EXPECT_EQ(schema_keyword_priority("then", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("else", vocabularies, walker), 1);
}

TEST(JSONSchema_default_walker_2019_09, schema_keyword_priority_unknown) {
  const auto &vocabularies = VOCABULARIES_2019_09_CORE;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("foobar", vocabularies, walker), 0);
}
