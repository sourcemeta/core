#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_recompose_without_fragment, example_1) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment(), "https://example.com/foo");
}

TEST(URI_recompose_without_fragment, example_2) {
  const sourcemeta::core::URI uri{"https://example.com/foo/../bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  // Without canonicalize(), path with ".." is preserved
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "https://example.com/foo/../bar");
}

TEST(URI_recompose_without_fragment, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(), "urn:example:schema");
}

TEST(URI_recompose_without_fragment, https_with_empty_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "https://example.com/foo");
}

TEST(URI_recompose_without_fragment, http_trailing_slash) {
  const sourcemeta::core::URI uri{"http://example.com/"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(), "http://example.com/");
}

TEST(URI_recompose_without_fragment, https_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "https://example.com/foo");
}

TEST(URI_recompose_without_fragment, urn_with_fragment) {
  const sourcemeta::core::URI uri{"urn:example:schema#test"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(), "urn:example:schema");
}

TEST(URI_recompose_without_fragment, tag_with_fragment) {
  const sourcemeta::core::URI uri{
      "tag:example.com,2024-06-12:some-unique-id#tag"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "tag:example.com,2024-06-12:some-unique-id");
}

TEST(URI_recompose_without_fragment, fragment_only) {
  const sourcemeta::core::URI uri{"#bar"};
  EXPECT_FALSE(uri.recompose_without_fragment().has_value());
}

TEST(URI_recompose_without_fragment, preserves_scheme_case) {
  const sourcemeta::core::URI uri{"HtTpS://example.com/foo#bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "HtTpS://example.com/foo");
}

TEST(URI_recompose_without_fragment, preserves_host_case) {
  const sourcemeta::core::URI uri{"https://ExAmPlE.CoM/foo#bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(),
            "https://ExAmPlE.CoM/foo");
}

TEST(URI_recompose_without_fragment, preserves_scheme_and_host_case) {
  const sourcemeta::core::URI uri{"HtTp://ExAmPlE.CoM/foo#bar"};
  EXPECT_TRUE(uri.recompose_without_fragment().has_value());
  EXPECT_EQ(uri.recompose_without_fragment().value(), "HtTp://ExAmPlE.CoM/foo");
}
