#include <gtest/gtest.h>
#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

static const std::map<std::string, bool> VOCABULARIES_DRAFT7{
    {"http://json-schema.org/draft-07/schema#", true}};

static const std::map<std::string, bool> VOCABULARIES_DRAFT7_HYPERSCHEMA{
    {"http://json-schema.org/draft-07/hyper-schema#", true}};

TEST(JSONSchema_official_walker_draft7, schema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$schema", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, id) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$id", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, ref) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$ref", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Reference);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_draft7, definitions) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("definitions", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::LocationMembers);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, comment) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$comment", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, allOf) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("allOf", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, anyOf) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("anyOf", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, oneOf) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("oneOf", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, not) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("not", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceNegate);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, if) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("if", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceMaybe);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, then) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("then", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceMaybe);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"if"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, else) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("else", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceMaybe);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"if"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, items) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("items", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueOrElementsTraverseAnyItemOrItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, additionalItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("additionalItems", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseSomeItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"items"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, contains) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("contains", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, properties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("properties", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyStatic);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref", "required"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, patternProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("patternProperties", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyRegex);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, dependencies) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("dependencies", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorMembersInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, additionalProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("additionalProperties", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"properties", "patternProperties"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, propertyNames) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("propertyNames", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseAnyPropertyKey);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, type) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("type", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"properties"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, enum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("enum", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, const) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("const", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, multipleOf) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("multipleOf", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, maximum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maximum", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, minimum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minimum", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, exclusiveMaximum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMaximum", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, exclusiveMinimum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMinimum", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, maxLength) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maxLength", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, minLength) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minLength", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, pattern) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("pattern", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, maxItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maxItems", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, minItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minItems", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, uniqueItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("uniqueItems", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, maxProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxProperties", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, minProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minProperties", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, required) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("required", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, format) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("format", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, contentEncoding) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentEncoding", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, contentMediaType) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentMediaType", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, title) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("title", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, description) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("description", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, default) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("default", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, readOnly) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("readOnly", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, writeOnly) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("writeOnly", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, examples) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("examples", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_links) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("links", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_base) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("base", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_anchor) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchor", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_anchorPointer) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchorPointer", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_rel) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("rel", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_href) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("href", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_templatePointers) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("templatePointers",
                                           VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_templateRequired) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("templateRequired",
                                           VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_targetMediaType) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("targetMediaType",
                                           VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_targetHints) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetHints", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_submissionMediaType) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("submissionMediaType",
                                           VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_hrefSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("hrefSchema", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_draft7, hyperschema_targetSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_draft7, hyperschema_headerSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("headerSchema", VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_draft7, hyperschema_submissionSchema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("submissionSchema",
                                           VOCABULARIES_DRAFT7_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-07/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
}

TEST(JSONSchema_official_walker_draft7, hyperschema_links_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("links", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_base_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("base", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_anchor_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("anchor", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_anchorPointer_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("anchorPointer", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_rel_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("rel", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, hyperschema_href_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("href", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_templatePointers_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("templatePointers", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_templateRequired_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("templateRequired", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_targetMediaType_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetMediaType", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_targetHints_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("targetHints", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_submissionMediaType_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("submissionMediaType", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_hrefSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("hrefSchema", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_targetSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_headerSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("headerSchema", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7,
     hyperschema_submissionSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("submissionSchema", VOCABULARIES_DRAFT7)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
}

TEST(JSONSchema_official_walker_draft7, schema_keyword_priority_array) {
  const auto &vocabularies = VOCABULARIES_DRAFT7;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("items", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("additionalItems", vocabularies, walker),
            2);
}

TEST(JSONSchema_official_walker_draft7, schema_keyword_priority_object) {
  const auto &vocabularies = VOCABULARIES_DRAFT7;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("properties", vocabularies, walker), 2);
  EXPECT_EQ(schema_keyword_priority("patternProperties", vocabularies, walker),
            1);
  EXPECT_EQ(
      schema_keyword_priority("additionalProperties", vocabularies, walker), 3);
}

TEST(JSONSchema_official_walker_draft7, schema_keyword_priority_other) {
  const auto &vocabularies = VOCABULARIES_DRAFT7;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("if", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("then", vocabularies, walker), 2);
  EXPECT_EQ(schema_keyword_priority("else", vocabularies, walker), 2);
}

TEST(JSONSchema_official_walker_draft7, schema_keyword_priority_unknown) {
  const auto &vocabularies = VOCABULARIES_DRAFT7;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("foobar", vocabularies, walker), 1);
}

TEST(JSONSchema_official_walker_draft7, instance_locations) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
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
    "dependencies": {
      "foo": [ "bar" ],
      "baz": { "type": "string" }
    },
    "propertyNames": { "minLength": 1 },
    "contains": { "type": "number" },
    "additionalItems": { "minimum": 2 },
    "items": { "items": [ true, false ] },
    "definitions": { "foo": true }
  })JSON");

  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIterator(
           document, sourcemeta::core::schema_official_walker,
           sourcemeta::core::schema_official_resolver)) {
    entries.push_back(entry);
  }

  EXPECT_EQ(entries.size(), 22);

  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 0, "", std::nullopt, "", "");

  // Applicators (any)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 1, "/allOf/0", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 2, "/allOf/1", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 3, "/anyOf/0", "",
                                      "/~?anyOf~/~?0~", "/~?anyOf~/~?0~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 4, "/oneOf/0", "",
                                      "/~?oneOf~/~?0~", "/~?oneOf~/~?0~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 5, "/if", "", "/~?if~",
                                      "/~?if~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 6, "/then", "", "/~?then~",
                                      "/~?then~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 7, "/else", "", "/~?else~",
                                      "/~?else~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 8, "/not", "", "/~!~", "/~!~");

  // Applicators (object)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 9, "/properties/foo", "", "/foo",
                                      "/foo");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 10, "/properties/bar", "",
                                      "/bar", "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 11, "/additionalProperties", "",
                                      "/~?additionalProperties~/~P~",
                                      "/~?additionalProperties~/~P~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 12, "/patternProperties/^f", "",
                                      "/~R^f~", "/~R^f~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 13, "/patternProperties/x$", "",
                                      "/~Rx$~", "/~Rx$~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 14, "/dependencies/baz", "",
                                      "/~?dependencies~/~?baz~",
                                      "/~?dependencies~/~?baz~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 15, "/propertyNames", "", "/~K~",
                                      "/~K~");

  // Applicators (array)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 16, "/contains", "", "/~I~",
                                      "/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 17, "/additionalItems", "",
                                      "/~?additionalItems~/~I~",
                                      "/~?additionalItems~/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 18, "/items", "", "/~I~",
                                      "/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 19, "/items/items/0", "/items",
                                      "/~I~/0", "/0");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 20, "/items/items/1", "/items",
                                      "/~I~/1", "/1");

  // Core
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7_ORPHAN(entries, 21, "/definitions/foo",
                                             "", "", "");
}

TEST(JSONSchema_official_walker_draft7, definitions_subschemas) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "definitions": {
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

  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7(entries, 0, "", std::nullopt, "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7_ORPHAN(entries, 1, "/definitions/foo", "",
                                             "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7_ORPHAN(
      entries, 2, "/definitions/foo/properties/bar", "/definitions/foo", "/bar",
      "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT7_ORPHAN(
      entries, 3, "/definitions/foo/properties/bar/additionalProperties",
      "/definitions/foo/properties/bar", "/bar/~?additionalProperties~/~P~",
      "/~?additionalProperties~/~P~");
}
