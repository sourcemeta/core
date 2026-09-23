#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonschema.h>

TEST(identifier_keyword_2020_12) {
  EXPECT_EQ(sourcemeta::core::schema_identifier_keyword(
                sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12),
            "$id");
}

TEST(identifier_keyword_2019_09) {
  EXPECT_EQ(sourcemeta::core::schema_identifier_keyword(
                sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2019_09),
            "$id");
}

TEST(identifier_keyword_draft7) {
  EXPECT_EQ(sourcemeta::core::schema_identifier_keyword(
                sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_7),
            "$id");
}

TEST(identifier_keyword_draft6) {
  EXPECT_EQ(sourcemeta::core::schema_identifier_keyword(
                sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_6),
            "$id");
}

TEST(identifier_keyword_draft4) {
  EXPECT_EQ(sourcemeta::core::schema_identifier_keyword(
                sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_4),
            "id");
}

TEST(identifier_keyword_draft3) {
  EXPECT_EQ(sourcemeta::core::schema_identifier_keyword(
                sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_3),
            "id");
}
