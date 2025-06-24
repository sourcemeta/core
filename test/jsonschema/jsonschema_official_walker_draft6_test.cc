#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

static const sourcemeta::core::Vocabularies VOCABULARIES_DRAFT6{
    {"http://json-schema.org/draft-06/schema#", true}};

static const sourcemeta::core::Vocabularies VOCABULARIES_DRAFT6_HYPERSCHEMA{
    {"http://json-schema.org/draft-06/hyper-schema#", true}};

TEST(JSONSchema_official_walker_draft6, schema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$schema", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, id) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$id", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, ref) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$ref", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Reference);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, definitions) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("definitions", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::LocationMembers);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, allOf) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("allOf", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, anyOf) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("anyOf", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, oneOf) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("oneOf", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, not) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("not", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceNegate);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, items) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("items", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueOrElementsTraverseAnyItemOrItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Array});
}

TEST(JSONSchema_official_walker_draft6, additionalItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("additionalItems", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseSomeItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"items"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Array});
}

TEST(JSONSchema_official_walker_draft6, contains) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("contains", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseAnyItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Array});
}

TEST(JSONSchema_official_walker_draft6, properties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("properties", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyStatic);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref", "required"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Object});
}

TEST(JSONSchema_official_walker_draft6, patternProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("patternProperties", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyRegex);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Object});
}

TEST(JSONSchema_official_walker_draft6, dependencies) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("dependencies", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorMembersInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Object});
}

TEST(JSONSchema_official_walker_draft6, additionalProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("additionalProperties", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"properties", "patternProperties"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Object});
}

TEST(JSONSchema_official_walker_draft6, propertyNames) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("propertyNames", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseAnyPropertyKey);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Object});
}

TEST(JSONSchema_official_walker_draft6, type) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("type", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"properties"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, enum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("enum", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, const) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("const", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, multipleOf) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("multipleOf", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  const std::set<sourcemeta::core::JSON::Type> instances{
      sourcemeta::core::JSON::Type::Integer,
      sourcemeta::core::JSON::Type::Real};
  EXPECT_EQ(result.instances, instances);
}

TEST(JSONSchema_official_walker_draft6, maximum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maximum", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  const std::set<sourcemeta::core::JSON::Type> instances{
      sourcemeta::core::JSON::Type::Integer,
      sourcemeta::core::JSON::Type::Real};
  EXPECT_EQ(result.instances, instances);
}

TEST(JSONSchema_official_walker_draft6, minimum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minimum", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  const std::set<sourcemeta::core::JSON::Type> instances{
      sourcemeta::core::JSON::Type::Integer,
      sourcemeta::core::JSON::Type::Real};
  EXPECT_EQ(result.instances, instances);
}

TEST(JSONSchema_official_walker_draft6, exclusiveMaximum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMaximum", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  const std::set<sourcemeta::core::JSON::Type> instances{
      sourcemeta::core::JSON::Type::Integer,
      sourcemeta::core::JSON::Type::Real};
  EXPECT_EQ(result.instances, instances);
}

TEST(JSONSchema_official_walker_draft6, exclusiveMinimum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMinimum", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  const std::set<sourcemeta::core::JSON::Type> instances{
      sourcemeta::core::JSON::Type::Integer,
      sourcemeta::core::JSON::Type::Real};
  EXPECT_EQ(result.instances, instances);
}

TEST(JSONSchema_official_walker_draft6, maxLength) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maxLength", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::String});
}

TEST(JSONSchema_official_walker_draft6, minLength) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minLength", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::String});
}

TEST(JSONSchema_official_walker_draft6, pattern) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("pattern", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::String});
}

TEST(JSONSchema_official_walker_draft6, maxItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maxItems", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Array});
}

TEST(JSONSchema_official_walker_draft6, minItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minItems", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Array});
}

TEST(JSONSchema_official_walker_draft6, uniqueItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("uniqueItems", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Array});
}

TEST(JSONSchema_official_walker_draft6, maxProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxProperties", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Object});
}

TEST(JSONSchema_official_walker_draft6, minProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minProperties", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Object});
}

TEST(JSONSchema_official_walker_draft6, required) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("required", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::Object});
}

TEST(JSONSchema_official_walker_draft6, format) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("format", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>{
                                  sourcemeta::core::JSON::Type::String});
}

TEST(JSONSchema_official_walker_draft6, title) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("title", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, description) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("description", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, default) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("default", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, examples) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("examples", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_links) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("links", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_href) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("href", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_rel) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("rel", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_mediaType) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("mediaType", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_submissionEncType) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("submissionEncType",
                                           VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_base) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("base", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_media) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("media", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_readOnly) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("readOnly", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/hyper-schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_hrefSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("hrefSchema", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_targetSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_submissionSchema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("submissionSchema",
                                           VOCABULARIES_DRAFT6_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_links_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("links", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_href_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("href", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_rel_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("rel", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6,
     hyperschema_mediaType_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("mediaType", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6,
     hyperschema_submissionEncType_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("submissionEncType", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_base_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("base", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, hyperschema_media_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("media", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6,
     hyperschema_readOnly_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("readOnly", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-06/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6,
     hyperschema_hrefSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("hrefSchema", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6,
     hyperschema_targetSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6,
     hyperschema_submissionSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("submissionSchema", VOCABULARIES_DRAFT6)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft6, schema_keyword_priority_array) {
  const auto &vocabularies = VOCABULARIES_DRAFT6;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("items", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("additionalItems", vocabularies, walker),
            2);
}

TEST(JSONSchema_official_walker_draft6, schema_keyword_priority_object) {
  const auto &vocabularies = VOCABULARIES_DRAFT6;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("properties", vocabularies, walker), 2);
  EXPECT_EQ(schema_keyword_priority("patternProperties", vocabularies, walker),
            1);
  EXPECT_EQ(
      schema_keyword_priority("additionalProperties", vocabularies, walker), 3);
}

TEST(JSONSchema_official_walker_draft6, schema_keyword_priority_unknown) {
  const auto &vocabularies = VOCABULARIES_DRAFT6;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("foobar", vocabularies, walker), 1);
}

TEST(JSONSchema_official_walker_draft6, instance_locations) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "allOf": [ { "type": "string" }, { "minLength": 3 } ],
    "anyOf": [ { "type": "string" } ],
    "oneOf": [ { "type": "string" } ],
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

  EXPECT_EQ(entries.size(), 19);

  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 0, "", std::nullopt, "", "");

  // Applicators (any)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 1, "/allOf/0", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 2, "/allOf/1", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 3, "/anyOf/0", "",
                                      "/~?anyOf~/~?0~", "/~?anyOf~/~?0~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 4, "/oneOf/0", "",
                                      "/~?oneOf~/~?0~", "/~?oneOf~/~?0~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 5, "/not", "", "/~!~", "/~!~");

  // Applicators (object)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 6, "/properties/foo", "", "/foo",
                                      "/foo");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 7, "/properties/bar", "", "/bar",
                                      "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 8, "/additionalProperties", "",
                                      "/~?additionalProperties~/~P~",
                                      "/~?additionalProperties~/~P~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 9, "/patternProperties/^f", "",
                                      "/~R^f~", "/~R^f~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 10, "/patternProperties/x$", "",
                                      "/~Rx$~", "/~Rx$~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 11, "/dependencies/baz", "",
                                      "/~?dependencies~/~?baz~",
                                      "/~?dependencies~/~?baz~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 12, "/propertyNames", "", "/~K~",
                                      "/~K~");

  // Applicators (array)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 13, "/contains", "", "/~I~",
                                      "/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 14, "/additionalItems", "",
                                      "/~?additionalItems~/~I~",
                                      "/~?additionalItems~/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 15, "/items", "", "/~I~",
                                      "/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 16, "/items/items/0", "/items",
                                      "/~I~/0", "/0");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 17, "/items/items/1", "/items",
                                      "/~I~/1", "/1");

  // Core
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6_ORPHAN(entries, 18, "/definitions/foo",
                                             "", "", "");
}

TEST(JSONSchema_official_walker_draft6, definitions_subschemas) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
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

  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6(entries, 0, "", std::nullopt, "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6_ORPHAN(entries, 1, "/definitions/foo", "",
                                             "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6_ORPHAN(
      entries, 2, "/definitions/foo/properties/bar", "/definitions/foo", "/bar",
      "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT6_ORPHAN(
      entries, 3, "/definitions/foo/properties/bar/additionalProperties",
      "/definitions/foo/properties/bar", "/bar/~?additionalProperties~/~P~",
      "/~?additionalProperties~/~P~");
}
