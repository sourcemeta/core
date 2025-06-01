#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_empty, empty) {
  const sourcemeta::core::URI uri{""};
  EXPECT_TRUE(uri.empty());
}

TEST(URI_empty, hier_only) {
  const sourcemeta::core::URI uri{"example.com"};
  EXPECT_FALSE(uri.empty());
}

TEST(URI_empty, path_only) {
  const sourcemeta::core::URI uri{"/foo/bar"};
  EXPECT_FALSE(uri.empty());
}

TEST(URI_empty, query_only) {
  const sourcemeta::core::URI uri{"?foo=bar"};
  EXPECT_FALSE(uri.empty());
}

TEST(URI_empty, fragment_only) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_FALSE(uri.empty());
}

TEST(URI_empty, fragment_empty_only) {
  const sourcemeta::core::URI uri{"#"};
  EXPECT_FALSE(uri.empty());
}

TEST(URI_empty, urn_path) {
  const sourcemeta::core::URI uri{"urn:example:animal:ferret:nose"};
  EXPECT_FALSE(uri.empty());
}
