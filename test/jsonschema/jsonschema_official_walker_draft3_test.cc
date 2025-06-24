#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

static const sourcemeta::core::Vocabularies VOCABULARIES_DRAFT3{
    {"http://json-schema.org/draft-03/schema#", true}};

static const sourcemeta::core::Vocabularies VOCABULARIES_DRAFT3_HYPERSCHEMA{
    {"http://json-schema.org/draft-03/hyper-schema#", true}};

TEST(JSONSchema_official_walker_draft3, schema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$schema", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, id) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("id", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, ref) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$ref", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Reference);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, items) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("items", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueOrElementsTraverseAnyItemOrItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft3, additionalItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("additionalItems", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseSomeItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"items"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft3, properties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("properties", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyStatic);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft3, patternProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("patternProperties", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyRegex);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft3, dependencies) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("dependencies", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorMembersInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft3, additionalProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("additionalProperties", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"properties", "patternProperties"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft3, type) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("type", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, enum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("enum", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, maximum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maximum", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft3, minimum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minimum", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft3, exclusiveMaximum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMaximum", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft3, exclusiveMinimum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("exclusiveMinimum", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft3, maxLength) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maxLength", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft3, minLength) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minLength", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft3, pattern) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("pattern", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft3, maxItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maxItems", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft3, minItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minItems", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft3, uniqueItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("uniqueItems", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft3, required) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("required", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft3, format) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("format", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft3, title) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("title", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, description) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("description", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, default) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("default", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, divisibleBy) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("divisibleBy", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft3, disallow) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("disallow", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorElementsInPlaceSomeNegate);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, extends) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("extends", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueOrElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/schema#");
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_links) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("links", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_fragmentResolution) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("fragmentResolution",
                                           VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_root) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("root", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_readonly) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("readonly", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_contentEncoding) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("contentEncoding",
                                           VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_pathStart) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("pathStart", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_mediaType) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("mediaType", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_alternate) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("alternate", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_href) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("href", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_rel) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("rel", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_method) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("method", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_enctype) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("enctype", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_targetSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_DRAFT3_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-03/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_links_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("links", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_fragmentResolution_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("fragmentResolution", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_root_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("root", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_readonly_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("readonly", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_contentEncoding_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentEncoding", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_pathStart_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("pathStart", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_mediaType_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("mediaType", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_alternate_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("alternate", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_href_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("href", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, hyperschema_rel_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("rel", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_method_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("method", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_enctype_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("enctype", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3,
     hyperschema_targetSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_DRAFT3)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  const std::set<std::string> expected{"$ref"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft3, schema_keyword_priority_array) {
  const auto &vocabularies = VOCABULARIES_DRAFT3;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("items", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("additionalItems", vocabularies, walker),
            2);
}

TEST(JSONSchema_official_walker_draft3, schema_keyword_priority_object) {
  const auto &vocabularies = VOCABULARIES_DRAFT3;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("properties", vocabularies, walker), 1);
  EXPECT_EQ(schema_keyword_priority("patternProperties", vocabularies, walker),
            1);
  EXPECT_EQ(
      schema_keyword_priority("additionalProperties", vocabularies, walker), 2);
}

TEST(JSONSchema_official_walker_draft3, schema_keyword_priority_unknown) {
  const auto &vocabularies = VOCABULARIES_DRAFT3;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("foobar", vocabularies, walker), 1);
}

TEST(JSONSchema_official_walker_draft3, instance_locations) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-03/schema#",
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
    "additionalItems": { "minimum": 2 },
    "items": { "items": [ true, false ] },
    "type": [ "string", { "type": "number" }, { "type": "integer" } ],
    "disallow": [ "boolean", { "type": "string" }, { "type": "integer" } ],
    "extends": {
      "extends": [ { "type": "string" } ]
    }
  })JSON");

  std::vector<sourcemeta::core::SchemaIteratorEntry> entries;
  for (const auto &entry : sourcemeta::core::SchemaIterator(
           document, sourcemeta::core::schema_official_walker,
           sourcemeta::core::schema_official_resolver)) {
    entries.push_back(entry);
  }

  EXPECT_EQ(entries.size(), 17);

  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 0, "", std::nullopt, "", "");

  // Applicators (object)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 1, "/properties/foo", "", "/foo",
                                      "/foo");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 2, "/properties/bar", "", "/bar",
                                      "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 3, "/additionalProperties", "",
                                      "/~?additionalProperties~/~P~",
                                      "/~?additionalProperties~/~P~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 4, "/patternProperties/^f", "",
                                      "/~R^f~", "/~R^f~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 5, "/patternProperties/x$", "",
                                      "/~Rx$~", "/~Rx$~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 6, "/dependencies/baz", "",
                                      "/~?dependencies~/~?baz~",
                                      "/~?dependencies~/~?baz~");

  // Applicators (array)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 7, "/additionalItems", "",
                                      "/~?additionalItems~/~I~",
                                      "/~?additionalItems~/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 8, "/items", "", "/~I~", "/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 9, "/items/items/0", "/items",
                                      "/~I~/0", "/0");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 10, "/items/items/1", "/items",
                                      "/~I~/1", "/1");

  // Applicators (any)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 11, "/type/1", "",
                                      "/~?type~/~?1~", "/~?type~/~?1~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 12, "/type/2", "",
                                      "/~?type~/~?2~", "/~?type~/~?2~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 13, "/disallow/1", "",
                                      "/~?disallow~/~?1~/~!~",
                                      "/~?disallow~/~?1~/~!~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 14, "/disallow/2", "",
                                      "/~?disallow~/~?2~/~!~",
                                      "/~?disallow~/~?2~/~!~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 15, "/extends", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT3(entries, 16, "/extends/extends/0",
                                      "/extends", "", "");
}
