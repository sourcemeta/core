#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

static const sourcemeta::core::Vocabularies VOCABULARIES_DRAFT2{
    {"http://json-schema.org/draft-02/schema#", true}};

static const sourcemeta::core::Vocabularies VOCABULARIES_DRAFT2_HYPERSCHEMA{
    {"http://json-schema.org/draft-02/hyper-schema#", true}};

TEST(JSONSchema_official_walker_draft2, schema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$schema", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, id) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("id", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, ref) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("$ref", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Reference);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, items) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("items", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueOrElementsTraverseAnyItemOrItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft2, properties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("properties", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyStatic);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft2, additionalProperties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("additionalProperties", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  const std::set<std::string> expected{"properties"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft2, type) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("type", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, enum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("enum", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, maximum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maximum", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft2, minimum) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minimum", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft2, maximumCanEqual) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maximumCanEqual", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft2, minimumCanEqual) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minimumCanEqual", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft2, maxLength) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maxLength", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft2, minLength) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minLength", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft2, pattern) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("pattern", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft2, maxItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maxItems", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft2, minItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minItems", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft2, uniqueItems) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("uniqueItems", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft2, requires) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("requires", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseParent);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft2, format) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("format", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Other);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft2, title) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("title", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, description) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("description", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, default) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("default", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, divisibleBy) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("divisibleBy", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Integer,
                                   sourcemeta::core::JSON::Type::Real}));
}

TEST(JSONSchema_official_walker_draft2, disallow) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("disallow", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Assertion);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, extends) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("extends", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueOrElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, contentEncoding) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("contentEncoding", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Comment);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::String}));
}

TEST(JSONSchema_official_walker_draft2, hyperschema_links) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("links", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_fragmentResolution) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("fragmentResolution",
                                           VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_root) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("root", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_readonly) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("readonly", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_pathStart) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("pathStart", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_mediaType) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("mediaType", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_alternate) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("alternate", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_href) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("href", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_rel) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("rel", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_method) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("method", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_enctype) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("enctype", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_targetSchema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueInPlaceOther);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_links_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("links", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_schema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$schema", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_id) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("id", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_ref) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("$ref", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_items) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("items", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueOrElementsTraverseAnyItemOrItem);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Array}));
}

TEST(JSONSchema_official_walker_draft2, hyperschema_properties) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("properties", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorMembersTraversePropertyStatic);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft2, hyperschema_additionalProperties) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("additionalProperties",
                                           VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type,
            SchemaKeywordType::ApplicatorValueTraverseSomeProperty);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/hyper-schema#");
  const std::set<std::string> expected{"properties"};
  EXPECT_EQ(result.dependencies, expected);
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft2, hyperschema_type) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("type", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorElementsInPlaceSome);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_enum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("enum", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_maximum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maximum", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_minimum) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minimum", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_maximumCanEqual) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("maximumCanEqual",
                                           VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_minimumCanEqual) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("minimumCanEqual",
                                           VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_maxLength) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxLength", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_minLength) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minLength", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_pattern) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("pattern", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_maxItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("maxItems", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_minItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("minItems", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_uniqueItems) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("uniqueItems", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_requires) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("requires", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueTraverseParent);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_EQ(result.instances, std::set<sourcemeta::core::JSON::Type>(
                                  {sourcemeta::core::JSON::Type::Object}));
}

TEST(JSONSchema_official_walker_draft2, hyperschema_format) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("format", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_title) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("title", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_description) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("description", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_default) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("default", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_divisibleBy) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("divisibleBy", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_disallow) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("disallow", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_extends) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("extends", VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::ApplicatorValueOrElementsInPlace);
  EXPECT_TRUE(result.vocabulary.has_value());
  EXPECT_EQ(result.vocabulary.value(),
            "http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_contentEncoding) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("contentEncoding",
                                           VOCABULARIES_DRAFT2_HYPERSCHEMA)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2,
     hyperschema_fragmentResolution_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("fragmentResolution", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_root_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("root", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2,
     hyperschema_readonly_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("readonly", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2,
     hyperschema_pathStart_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("pathStart", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2,
     hyperschema_mediaType_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("mediaType", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2,
     hyperschema_alternate_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("alternate", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_href_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("href", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, hyperschema_rel_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("rel", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2,
     hyperschema_method_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("method", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2,
     hyperschema_enctype_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{schema_official_walker("enctype", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2,
     hyperschema_targetSchema_without_hyperschema) {
  using namespace sourcemeta::core;
  const auto result{
      schema_official_walker("targetSchema", VOCABULARIES_DRAFT2)};
  EXPECT_EQ(result.type, SchemaKeywordType::Unknown);
  EXPECT_FALSE(result.vocabulary.has_value());
  EXPECT_TRUE(result.dependencies.empty());
  EXPECT_TRUE(result.instances.empty());
}

TEST(JSONSchema_official_walker_draft2, schema_keyword_priority_object) {
  const auto &vocabularies = VOCABULARIES_DRAFT2;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("properties", vocabularies, walker), 0);
  EXPECT_EQ(
      schema_keyword_priority("additionalProperties", vocabularies, walker), 1);
}

TEST(JSONSchema_official_walker_draft2, schema_keyword_priority_unknown) {
  const auto &vocabularies = VOCABULARIES_DRAFT2;
  const auto &walker = sourcemeta::core::schema_official_walker;
  using namespace sourcemeta::core;
  EXPECT_EQ(schema_keyword_priority("foobar", vocabularies, walker), 0);
}

TEST(JSONSchema_official_walker_draft2, instance_locations) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-02/schema#",
    "properties": {
      "foo": {
        "type": "string",
        "requires": { "type": "object" }
      },
      "bar": false
    },
    "additionalProperties": { "type": "number" },
    "items": { "items": [ { "type": "string" }, { "type": "integer" } ] },
    "type": [ "string", { "type": "number" }, { "type": "integer" } ],
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

  EXPECT_EQ(entries.size(), 12);

  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 0, "", std::nullopt, "", "");

  // Applicators (object)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 1, "/properties/foo", "", "/foo",
                                      "/foo");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 2, "/properties/foo/requires",
                                      "/properties/foo", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 3, "/properties/bar", "", "/bar",
                                      "/bar");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 4, "/additionalProperties", "",
                                      "/~?additionalProperties~/~P~",
                                      "/~?additionalProperties~/~P~");

  // Applicators (array)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 5, "/items", "", "/~I~", "/~I~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 6, "/items/items/0", "/items",
                                      "/~I~/0", "/0");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 7, "/items/items/1", "/items",
                                      "/~I~/1", "/1");

  // Applicators (any)
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 8, "/type/1", "",
                                      "/~?type~/~?1~", "/~?type~/~?1~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 9, "/type/2", "",
                                      "/~?type~/~?2~", "/~?type~/~?2~");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 10, "/extends", "", "", "");
  EXPECT_OFFICIAL_WALKER_ENTRY_DRAFT2(entries, 11, "/extends/extends/0",
                                      "/extends", "", "");
}
