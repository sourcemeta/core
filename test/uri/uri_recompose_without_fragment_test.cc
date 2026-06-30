#include <sourcemeta/core/test.h>

#include <sourcemeta/core/uri.h>

TEST(example_1) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment(), "https://example.com/foo");
}

TEST(example_2) {
  const sourcemeta::core::URI uri{"https://example.com/foo/../bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  // Without canonicalize(), path with ".." is preserved
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "https://example.com/foo/../bar");
}

TEST(urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(), "urn:example:schema");
}

TEST(https_with_empty_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "https://example.com/foo");
}

TEST(http_trailing_slash) {
  const sourcemeta::core::URI uri{"http://example.com/"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(), "http://example.com/");
}

TEST(https_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "https://example.com/foo");
}

TEST(urn_with_fragment) {
  const sourcemeta::core::URI uri{"urn:example:schema#test"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(), "urn:example:schema");
}

TEST(tag_with_fragment) {
  const sourcemeta::core::URI uri{
      "tag:example.com,2024-06-12:some-unique-id#tag"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "tag:example.com,2024-06-12:some-unique-id");
}

TEST(fragment_only) {
  const sourcemeta::core::URI uri{"#bar"};
  EXPECT_FALSE(uri.recompose_without_fragment().has_value());
}

TEST(preserves_scheme_case) {
  const sourcemeta::core::URI uri{"HtTpS://example.com/foo#bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "HtTpS://example.com/foo");
}

TEST(preserves_host_case) {
  const sourcemeta::core::URI uri{"https://ExAmPlE.CoM/foo#bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "https://ExAmPlE.CoM/foo");
}

TEST(preserves_scheme_and_host_case) {
  const sourcemeta::core::URI uri{"HtTp://ExAmPlE.CoM/foo#bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(), "HtTp://ExAmPlE.CoM/foo");
}

TEST(iri_preserves_ucschar) {
  const auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9#frag")};
  const auto result{uri.recompose_without_fragment()};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "https://example.com/caf\xC3\xA9");
}
