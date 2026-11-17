#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonschema.h>

// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
static const sourcemeta::core::SchemaVocabularies VOCABULARIES_2020_12_CORE{
    {"https://json-schema.org/draft/2020-12/vocab/core", true}};

static const sourcemeta::core::SchemaVocabularies
    VOCABULARIES_2020_12_VALIDATION{
        {"https://json-schema.org/draft/2020-12/vocab/core", true},
        {"https://json-schema.org/draft/2020-12/vocab/validation", true}};

static const sourcemeta::core::SchemaVocabularies VOCABULARIES_DRAFT4{
    {"http://json-schema.org/draft-04/schema#", true}};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

TEST(defines_keyword_2020_12_core_identifier) {
  EXPECT_TRUE(sourcemeta::core::schema_defines_keyword(
      sourcemeta::core::schema_walker, VOCABULARIES_2020_12_CORE, "$id"));
}

TEST(defines_keyword_2020_12_core_without_validation) {
  EXPECT_FALSE(sourcemeta::core::schema_defines_keyword(
      sourcemeta::core::schema_walker, VOCABULARIES_2020_12_CORE, "maxLength"));
}

TEST(defines_keyword_2020_12_validation) {
  EXPECT_TRUE(sourcemeta::core::schema_defines_keyword(
      sourcemeta::core::schema_walker, VOCABULARIES_2020_12_VALIDATION,
      "maxLength"));
}

TEST(defines_keyword_2020_12_unknown_keyword) {
  EXPECT_FALSE(sourcemeta::core::schema_defines_keyword(
      sourcemeta::core::schema_walker, VOCABULARIES_2020_12_CORE, "x-foo"));
}

TEST(defines_keyword_draft4_identifier) {
  EXPECT_TRUE(sourcemeta::core::schema_defines_keyword(
      sourcemeta::core::schema_walker, VOCABULARIES_DRAFT4, "id"));
}

// A base dialect names the keyword that introduces an identifier, which is not
// what the vocabularies in force say about it
TEST(defines_keyword_draft4_modern_identifier) {
  EXPECT_FALSE(sourcemeta::core::schema_defines_keyword(
      sourcemeta::core::schema_walker, VOCABULARIES_DRAFT4, "$id"));
}

TEST(defines_keyword_empty_vocabularies) {
  const sourcemeta::core::SchemaVocabularies vocabularies;
  EXPECT_FALSE(sourcemeta::core::schema_defines_keyword(
      sourcemeta::core::schema_walker, vocabularies, "$id"));
}
