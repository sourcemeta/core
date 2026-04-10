#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_is_in_place_applicator, unknown) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::Unknown));
}

TEST(JSONSchema_is_in_place_applicator, assertion) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::Assertion));
}

TEST(JSONSchema_is_in_place_applicator, annotation) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::Annotation));
}

TEST(JSONSchema_is_in_place_applicator, reference) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::Reference));
}

TEST(JSONSchema_is_in_place_applicator, other) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::Other));
}

TEST(JSONSchema_is_in_place_applicator, comment) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::Comment));
}

TEST(JSONSchema_is_in_place_applicator, location_members) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::LocationMembers));
}

TEST(JSONSchema_is_in_place_applicator,
     applicator_members_traverse_property_static) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::
          ApplicatorMembersTraversePropertyStatic));
}

TEST(JSONSchema_is_in_place_applicator,
     applicator_members_traverse_property_regex) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::
          ApplicatorMembersTraversePropertyRegex));
}

TEST(JSONSchema_is_in_place_applicator,
     applicator_value_traverse_some_property) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::
          ApplicatorValueTraverseSomeProperty));
}

TEST(JSONSchema_is_in_place_applicator,
     applicator_value_traverse_any_property_key) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::
          ApplicatorValueTraverseAnyPropertyKey));
}

TEST(JSONSchema_is_in_place_applicator, applicator_value_traverse_any_item) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorValueTraverseAnyItem));
}

TEST(JSONSchema_is_in_place_applicator, applicator_value_traverse_some_item) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorValueTraverseSomeItem));
}

TEST(JSONSchema_is_in_place_applicator, applicator_value_traverse_parent) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorValueTraverseParent));
}

TEST(JSONSchema_is_in_place_applicator, applicator_elements_traverse_item) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorElementsTraverseItem));
}

TEST(JSONSchema_is_in_place_applicator,
     applicator_value_or_elements_traverse_any_item_or_item) {
  EXPECT_FALSE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::
          ApplicatorValueOrElementsTraverseAnyItemOrItem));
}

TEST(JSONSchema_is_in_place_applicator, applicator_value_or_elements_in_place) {
  EXPECT_TRUE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorValueOrElementsInPlace));
}

TEST(JSONSchema_is_in_place_applicator, applicator_members_in_place_some) {
  EXPECT_TRUE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorMembersInPlaceSome));
}

TEST(JSONSchema_is_in_place_applicator, applicator_elements_in_place) {
  EXPECT_TRUE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorElementsInPlace));
}

TEST(JSONSchema_is_in_place_applicator, applicator_elements_in_place_some) {
  EXPECT_TRUE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorElementsInPlaceSome));
}

TEST(JSONSchema_is_in_place_applicator,
     applicator_elements_in_place_some_negate) {
  EXPECT_TRUE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::
          ApplicatorElementsInPlaceSomeNegate));
}

TEST(JSONSchema_is_in_place_applicator, applicator_value_in_place_maybe) {
  EXPECT_TRUE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorValueInPlaceMaybe));
}

TEST(JSONSchema_is_in_place_applicator, applicator_value_in_place_other) {
  EXPECT_TRUE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorValueInPlaceOther));
}

TEST(JSONSchema_is_in_place_applicator, applicator_value_in_place_negate) {
  EXPECT_TRUE(sourcemeta::core::is_in_place_applicator(
      sourcemeta::core::SchemaKeywordType::ApplicatorValueInPlaceNegate));
}
