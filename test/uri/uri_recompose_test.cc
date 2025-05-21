#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_recompose, example_1) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");
}

TEST(URI_recompose, example_2) {
  const sourcemeta::core::URI uri{"https://example.com/foo/../bar"};
  EXPECT_EQ(uri.recompose(), "https://example.com/bar");
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
