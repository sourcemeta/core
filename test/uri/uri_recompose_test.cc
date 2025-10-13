#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_recompose, example_1) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");
}

TEST(URI_recompose, example_2) {
  const sourcemeta::core::URI uri{"https://example.com/foo/../bar"};
  // Without canonicalize(), path with ".." is preserved
  EXPECT_EQ(uri.recompose(), "https://example.com/foo/../bar");
}

TEST(URI_recompose, example_3) {
  const sourcemeta::core::URI uri{"https://example.com/foo/%25/bar"};
  EXPECT_EQ(uri.recompose(), "https://example.com/foo/%25/bar");
}

TEST(URI_recompose, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_EQ(uri.recompose(), "urn:example:schema");
}

TEST(URI_recompose, https_with_empty_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#"};
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#");
}

TEST(URI_recompose, http_trailing_slash) {
  const sourcemeta::core::URI uri{"http://example.com/"};
  EXPECT_EQ(uri.recompose(), "http://example.com/");
}

TEST(URI_recompose, no_scheme) {
  const sourcemeta::core::URI uri{"example.com/foo"};
  EXPECT_EQ(uri.recompose(), "example.com/foo");
}

TEST(URI_recompose, empty_fragment) {
  const sourcemeta::core::URI uri{"#"};
  EXPECT_EQ(uri.recompose(), "#");
}

TEST(URI_recompose, empty_uri_default_constructor) {
  const sourcemeta::core::URI uri;
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_recompose, empty_uri_string_constructor) {
  const sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.recompose(), "");
}
