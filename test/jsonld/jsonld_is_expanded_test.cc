#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>

TEST(empty_array) {
  const auto document = sourcemeta::core::parse_json("[]");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(single_node_with_id) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@id": "http://example.com/a" }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(node_with_iri_property) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "bar" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(node_with_type_array) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@id": "http://example.com/a", "@type": [ "http://example.com/T" ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_with_datatype) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [
      { "@value": "v", "@type": "http://example.com/t" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_with_language) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "v", "@language": "en" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_with_language_and_direction) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [
      { "@value": "v", "@language": "ar", "@direction": "rtl" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_json_literal) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [
      { "@value": { "foo": "bar" }, "@type": "@json" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_native_number) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": 4 } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_native_boolean) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": true } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_with_index) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "v", "@index": "i" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(list_object) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@list": [ { "@value": "a" } ] } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(nested_list_object) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [
      { "@list": [ { "@list": [ { "@value": "a" } ] } ] } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(set_object) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@set": [ { "@value": "a" } ] } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(graph_object) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@graph": [ { "@id": "http://example.com/a" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(reverse_object) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@reverse": {
      "http://example.com/p": [ { "@id": "http://example.com/a" } ] } }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(included_block) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@id": "http://example.com/a",
      "@included": [ { "@id": "http://example.com/b" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(blank_node_identifier) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@id": "_:b0" }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(blank_node_property_key) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "_:b0": [ { "@value": "x" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(empty_blank_node_identifier) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@id": "_:" }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(empty_blank_node_property_key) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "_:": [ { "@value": "x" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(top_level_object) {
  const auto document = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/a"
  })");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(top_level_scalar) {
  const auto document = sourcemeta::core::parse_json(R"("foo")");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(top_level_value_object) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@value": "x" }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(top_level_list_object) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@list": [ { "@value": "x" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(top_level_set_object) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@set": [ { "@value": "x" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(context_present) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@context": {}, "@id": "http://example.com/a" }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(relative_iri_property_key) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "foo": [ { "@value": "x" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(id_not_string) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@id": [ "http://example.com/a" ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(id_null) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@id": null }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(id_relative_reference) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@id": "../document-relative" }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(type_relative_reference) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@type": [ "#document-relative" ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(type_not_array) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@type": "http://example.com/T" }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(type_entry_not_string) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@type": [ [ "http://example.com/T" ] ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(unnormalized_valid_property_key) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/vocabulary/./rel2#fragment": [
      { "@value": "x" } ] }
  ])");
  EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(invalid_iri_property_key) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/vocabulary/./rel2##fragment-works": [
      { "@value": "#fragment-works" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(relative_property_key) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "vocabulary/rel": [ { "@value": "x" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(property_value_not_array) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": { "@value": "x" } }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(property_value_bare_scalar) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ "x" ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_with_extra_property) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [
      { "@value": "x", "http://example.com/bar": [] } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_type_and_language) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [
      { "@value": "x", "@type": "http://example.com/t", "@language": "en" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_object_without_json_type) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": { "a": 1 } } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_invalid_language) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "x", "@language": "not a tag" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_object_invalid_direction) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [
      { "@value": "x", "@language": "en", "@direction": "up" } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(list_object_with_extra_property) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [
      { "@list": [], "http://example.com/bar": [] } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(value_and_list_together) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "x", "@list": [] } ] }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(unknown_keyword_form_key) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@foo": "x" }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}

TEST(reverse_value_not_array) {
  const auto document = sourcemeta::core::parse_json(R"([
    { "@reverse": { "http://example.com/p": { "@id": "http://example.com/a" } } }
  ])");
  EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(document));
}
