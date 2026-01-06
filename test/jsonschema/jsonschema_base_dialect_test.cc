#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://sourcemeta.com/metaschema_1") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/metaschema_1",
      "$schema": "https://sourcemeta.com/metaschema_1"
    })JSON");
  } else if (identifier == "https://sourcemeta.com/metaschema_2") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/metaschema_2",
      "$schema": "https://sourcemeta.com/metaschema_1"
    })JSON");
  } else if (identifier == "https://sourcemeta.com/no-schema") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/no-schema"
    })JSON");
  } else if (identifier == "https://sourcemeta.com/metaschema_3") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/metaschema_3",
      "$schema": "https://sourcemeta.com/no-schema"
    })JSON");
  } else if (identifier == "https://sourcemeta.com/metaschema_4") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/metaschema_4",
      "$schema": "https://sourcemeta.com/metaschema_4"
    })JSON");
  } else {
    return std::nullopt;
  }
}

TEST(JSONSchema_base_dialect, boolean_schema_true) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("true");
  const auto base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_FALSE(base_dialect.has_value());
}

TEST(JSONSchema_base_dialect, boolean_schema_false) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("false");
  const auto base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_FALSE(base_dialect.has_value());
}

TEST(JSONSchema_base_dialect, boolean_schema_default_dialect_official) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("true");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, sourcemeta::core::schema_resolver,
      "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_2020_12);
}

TEST(JSONSchema_base_dialect, boolean_schema_default_dialect_custom_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("true");
  EXPECT_THROW(
      sourcemeta::core::base_dialect(document, test_resolver,
                                     "https://sourcemeta.com/metaschema_1"),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_base_dialect, self_descriptive_schema_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://example.com/my-schema"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_base_dialect, non_resolvable_schema_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://example.com/does-not-exist"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_base_dialect, relative_schema_uri_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "../foo.json"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaRelativeMetaschemaResolutionError);
}

TEST(JSONSchema_base_dialect, relative_schema_uri) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "../foo.json"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaRelativeMetaschemaResolutionError);
}

TEST(JSONSchema_base_dialect, non_resolvable_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/does-not-exist"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_base_dialect, non_resolvable_default_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  EXPECT_THROW(
      sourcemeta::core::base_dialect(document, test_resolver,
                                     "https://example.com/does-not-exist"),
      sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_base_dialect, id_with_custom_metaschema_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://sourcemeta.com/metaschema_1"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_base_dialect, no_id_with_custom_metaschema_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/metaschema_1"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_base_dialect, self_descriptive_custom_metaschema_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/no-schema",
    "$schema": "https://sourcemeta.com/no-schema"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_base_dialect, metaschema_without_schema_one_hop_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/metaschema_3"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_base_dialect, id_self_descriptive_default_dialect_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/foo-bar"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver,
                                              "https://sourcemeta.com/foo-bar"),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_base_dialect, id_default_dialect_custom_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/foo-bar"
  })JSON");
  EXPECT_THROW(
      sourcemeta::core::base_dialect(document, test_resolver,
                                     "https://sourcemeta.com/metaschema_1"),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_base_dialect, default_dialect_custom_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  EXPECT_THROW(
      sourcemeta::core::base_dialect(document, test_resolver,
                                     "https://sourcemeta.com/metaschema_1"),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_base_dialect, default_dialect_precedence_custom_throws) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/metaschema_4"
  })JSON");
  EXPECT_THROW(
      sourcemeta::core::base_dialect(document, test_resolver,
                                     "https://sourcemeta.com/metaschema_1"),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_base_dialect, default_dialect_official_takes_precedence) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");
  const auto base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/metaschema_1")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_2020_12);
}

TEST(JSONSchema_base_dialect, to_string_2020_12) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_2020_12),
            "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_base_dialect, to_string_2020_12_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_2020_12_Hyper),
            "https://json-schema.org/draft/2020-12/hyper-schema");
}

TEST(JSONSchema_base_dialect, to_string_2019_09) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_2019_09),
            "https://json-schema.org/draft/2019-09/schema");
}

TEST(JSONSchema_base_dialect, to_string_2019_09_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_2019_09_Hyper),
            "https://json-schema.org/draft/2019-09/hyper-schema");
}

TEST(JSONSchema_base_dialect, to_string_draft7) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_7),
            "http://json-schema.org/draft-07/schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft7_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_7_Hyper),
            "http://json-schema.org/draft-07/hyper-schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft6) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_6),
            "http://json-schema.org/draft-06/schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft6_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_6_Hyper),
            "http://json-schema.org/draft-06/hyper-schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft4) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_4),
            "http://json-schema.org/draft-04/schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft4_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_4_Hyper),
            "http://json-schema.org/draft-04/hyper-schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft3) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_3),
            "http://json-schema.org/draft-03/schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft3_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_3_Hyper),
            "http://json-schema.org/draft-03/hyper-schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft2_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_2_Hyper),
            "http://json-schema.org/draft-02/hyper-schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft1_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_1_Hyper),
            "http://json-schema.org/draft-01/hyper-schema#");
}

TEST(JSONSchema_base_dialect, to_string_draft0_hyper) {
  EXPECT_EQ(sourcemeta::core::to_string(
                sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_0_Hyper),
            "http://json-schema.org/draft-00/hyper-schema#");
}

TEST(JSONSchema_base_dialect, to_base_dialect_2020_12) {
  const auto result{sourcemeta::core::to_base_dialect(
      "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_2020_12);
}

TEST(JSONSchema_base_dialect, to_base_dialect_2020_12_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "https://json-schema.org/draft/2020-12/hyper-schema")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_2020_12_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_2019_09) {
  const auto result{sourcemeta::core::to_base_dialect(
      "https://json-schema.org/draft/2019-09/schema")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_2019_09);
}

TEST(JSONSchema_base_dialect, to_base_dialect_2019_09_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "https://json-schema.org/draft/2019-09/hyper-schema")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_2019_09_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft7) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-07/schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_7);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft7_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-07/hyper-schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_7_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft6) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-06/schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_6);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft6_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-06/hyper-schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_6_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft4) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-04/schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_4);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft4_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-04/hyper-schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_4_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft3) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-03/schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_3);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft3_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-03/hyper-schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_3_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft2_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-02/hyper-schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_2_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft1_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-01/hyper-schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_1_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_draft0_hyper) {
  const auto result{sourcemeta::core::to_base_dialect(
      "http://json-schema.org/draft-00/hyper-schema#")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_Draft_0_Hyper);
}

TEST(JSONSchema_base_dialect, to_base_dialect_unknown) {
  const auto result{
      sourcemeta::core::to_base_dialect("https://example.com/unknown-dialect")};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONSchema_base_dialect, to_base_dialect_empty) {
  const auto result{sourcemeta::core::to_base_dialect("")};
  EXPECT_FALSE(result.has_value());
}
