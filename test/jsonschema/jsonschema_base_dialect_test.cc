#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/jsonschema_error.h>

#include <string> // std::string

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
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_FALSE(base_dialect.has_value());
}

TEST(JSONSchema_base_dialect, boolean_schema_false) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("false");
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_FALSE(base_dialect.has_value());
}

TEST(JSONSchema_base_dialect, boolean_schema_default_dialect_one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("true");
  const std::optional<std::string> base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/metaschema_1")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, boolean_schema_default_dialect_two_hops) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("true");
  const std::optional<std::string> base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/metaschema_2")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, self_descriptive_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://example.com/my-schema"
  })JSON");
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://example.com/my-schema");
}

TEST(JSONSchema_base_dialect, non_resolvable_schema_with_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://example.com/does-not-exist"
  })JSON");
  EXPECT_THROW(sourcemeta::core::base_dialect(document, test_resolver),
               sourcemeta::core::SchemaResolutionError);
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

TEST(JSONSchema_base_dialect, id_with_one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://sourcemeta.com/metaschema_1"
  })JSON");
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, id_with_two_hops) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/my-schema",
    "$schema": "https://sourcemeta.com/metaschema_2"
  })JSON");
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, no_id_with_one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/metaschema_1"
  })JSON");
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, no_id_with_two_hops) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/metaschema_2"
  })JSON");
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, self_descriptive_metaschema_without_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/no-schema",
    "$schema": "https://sourcemeta.com/no-schema"
  })JSON");
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/no-schema");
}

TEST(JSONSchema_base_dialect, metaschema_without_schema_one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/metaschema_3"
  })JSON");
  const std::optional<std::string> base_dialect{
      sourcemeta::core::base_dialect(document, test_resolver)};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/no-schema");
}

TEST(JSONSchema_base_dialect, id_self_descriptive_default_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/foo-bar"
  })JSON");
  const std::optional<std::string> base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/foo-bar")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/foo-bar");
}

TEST(JSONSchema_base_dialect, id_default_dialect_one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/foo-bar"
  })JSON");
  const std::optional<std::string> base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/metaschema_1")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, id_default_dialect_two_hops) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://sourcemeta.com/foo-bar"
  })JSON");
  const std::optional<std::string> base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/metaschema_2")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, default_dialect_one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  const std::optional<std::string> base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/metaschema_1")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, default_dialect_two_hops) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  const std::optional<std::string> base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/metaschema_2")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_1");
}

TEST(JSONSchema_base_dialect, default_dialect_precedence) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/metaschema_4"
  })JSON");
  const std::optional<std::string> base_dialect{sourcemeta::core::base_dialect(
      document, test_resolver, "https://sourcemeta.com/metaschema_1")};
  EXPECT_TRUE(base_dialect.has_value());
  EXPECT_EQ(base_dialect.value(), "https://sourcemeta.com/metaschema_4");
}
