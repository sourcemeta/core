#include <gtest/gtest.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_anchor_2020_12, boolean_schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse("true");
  const auto anchors{sourcemeta::core::anchors(
      document, sourcemeta::core::official_resolver,
      "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_TRUE(anchors.empty());
}

TEST(JSONSchema_anchor_2020_12, top_level_no_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const auto anchors{
      sourcemeta::core::anchors(document, sourcemeta::core::official_resolver)};
  EXPECT_TRUE(anchors.empty());
}

TEST(JSONSchema_anchor_2020_12, top_level_static_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo"
  })JSON");

  const auto anchors{
      sourcemeta::core::anchors(document, sourcemeta::core::official_resolver)};
  EXPECT_EQ(anchors.size(), 1);
  EXPECT_TRUE(anchors.contains("foo"));
  EXPECT_EQ(anchors.at("foo"), sourcemeta::core::AnchorType::Static);
}

TEST(JSONSchema_anchor_2020_12, top_level_dynamic_anchor) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$dynamicAnchor": "foo"
  })JSON");

  const auto anchors{
      sourcemeta::core::anchors(document, sourcemeta::core::official_resolver)};
  EXPECT_EQ(anchors.size(), 1);
  EXPECT_TRUE(anchors.contains("foo"));
  EXPECT_EQ(anchors.at("foo"), sourcemeta::core::AnchorType::Dynamic);
}

TEST(JSONSchema_anchor_2020_12, top_level_static_and_dynamic_anchor_different) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "bar",
    "$dynamicAnchor": "foo"
  })JSON");

  const auto anchors{
      sourcemeta::core::anchors(document, sourcemeta::core::official_resolver)};
  EXPECT_EQ(anchors.size(), 2);
  EXPECT_TRUE(anchors.contains("foo"));
  EXPECT_TRUE(anchors.contains("bar"));
  EXPECT_EQ(anchors.at("foo"), sourcemeta::core::AnchorType::Dynamic);
  EXPECT_EQ(anchors.at("bar"), sourcemeta::core::AnchorType::Static);
}

TEST(JSONSchema_anchor_2020_12, top_level_static_and_dynamic_anchor_same) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo",
    "$dynamicAnchor": "foo"
  })JSON");

  const auto anchors{
      sourcemeta::core::anchors(document, sourcemeta::core::official_resolver)};
  EXPECT_EQ(anchors.size(), 1);
  EXPECT_TRUE(anchors.contains("foo"));
  EXPECT_EQ(anchors.at("foo"), sourcemeta::core::AnchorType::All);
}

TEST(JSONSchema_anchor_2020_12, nested_static_with_default_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$anchor": "foo"
  })JSON");

  const auto anchors{sourcemeta::core::anchors(
      document, sourcemeta::core::official_resolver,
      "https://json-schema.org/draft/2020-12/schema")};
  EXPECT_EQ(anchors.size(), 1);
  EXPECT_TRUE(anchors.contains("foo"));
  EXPECT_EQ(anchors.at("foo"), sourcemeta::core::AnchorType::Static);
}

TEST(JSONSchema_anchor_2020_12, vocabularies_shortcut) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo",
    "$dynamicAnchor": "bar"
  })JSON");

  const std::map<std::string, bool> vocabularies{sourcemeta::core::vocabularies(
      document, sourcemeta::core::official_resolver)};

  const auto anchors{sourcemeta::core::anchors(document, vocabularies)};
  EXPECT_EQ(anchors.size(), 2);
  EXPECT_TRUE(anchors.contains("foo"));
  EXPECT_TRUE(anchors.contains("bar"));
  EXPECT_EQ(anchors.at("foo"), sourcemeta::core::AnchorType::Static);
  EXPECT_EQ(anchors.at("bar"), sourcemeta::core::AnchorType::Dynamic);
}

TEST(JSONSchema_anchor_2020_12, old_id_anchor_not_recognized) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "#foo"
  })JSON");

  const auto anchors{
      sourcemeta::core::anchors(document, sourcemeta::core::official_resolver)};
  EXPECT_TRUE(anchors.empty());
}
