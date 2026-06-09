#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_recompose_relative, full_url) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo/bar");
}

TEST(URI_recompose_relative, full_url_with_query) {
  const sourcemeta::core::URI uri{"https://example.com/foo?x=1"};
  EXPECT_EQ(uri.recompose_relative(), "/foo?x=1");
}

TEST(URI_recompose_relative, full_url_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo#bar");
}

TEST(URI_recompose_relative, full_url_with_query_and_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo?x=1#bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo?x=1#bar");
}

TEST(URI_recompose_relative, json_pointer_fragment_preserved) {
  const sourcemeta::core::URI uri{
      "https://example.com/meta/target#/definitions/foo"};
  EXPECT_EQ(uri.recompose_relative(), "/meta/target#/definitions/foo");
}

TEST(URI_recompose_relative, no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_EQ(uri.recompose_relative(), "");
}

TEST(URI_recompose_relative, no_path_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com#bar"};
  EXPECT_EQ(uri.recompose_relative(), "#bar");
}

TEST(URI_recompose_relative, relative_input) {
  const sourcemeta::core::URI uri{"foo/bar"};
  EXPECT_EQ(uri.recompose_relative(), "foo/bar");
}

TEST(URI_recompose_relative, relative_input_with_fragment) {
  const sourcemeta::core::URI uri{"foo/bar#baz"};
  EXPECT_EQ(uri.recompose_relative(), "foo/bar#baz");
}

TEST(URI_recompose_relative, fragment_only) {
  const sourcemeta::core::URI uri{"#bar"};
  EXPECT_EQ(uri.recompose_relative(), "#bar");
}

TEST(URI_recompose_relative, empty_uri) {
  const sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.recompose_relative(), "");
}

TEST(URI_recompose_relative, path_only_with_query) {
  const sourcemeta::core::URI uri{"https://example.com/?x=1&y=2"};
  EXPECT_EQ(uri.recompose_relative(), "/?x=1&y=2");
}

TEST(URI_recompose_relative, percent_encoded_path) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo%20bar");
}

TEST(URI_recompose_relative, query_only_no_path) {
  const sourcemeta::core::URI uri{"https://example.com?x=1"};
  EXPECT_EQ(uri.recompose_relative(), "?x=1");
}

TEST(URI_recompose_relative, query_and_fragment_no_path) {
  const sourcemeta::core::URI uri{"https://example.com?x=1#bar"};
  EXPECT_EQ(uri.recompose_relative(), "?x=1#bar");
}

TEST(URI_recompose_relative, path_noscheme_first_segment_colon_encoded) {
  const sourcemeta::core::URI uri{"urn:foo:bar"};
  EXPECT_EQ(uri.recompose_relative(), "foo%3Abar");
}

TEST(URI_recompose_relative, path_noscheme_colon_only_in_first_segment) {
  const sourcemeta::core::URI uri{"urn:foo:bar/baz:qux"};
  EXPECT_EQ(uri.recompose_relative(), "foo%3Abar/baz:qux");
}

TEST(URI_recompose_relative, path_absolute_first_segment_colon_preserved) {
  const sourcemeta::core::URI uri{"https://example.com/foo:bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo:bar");
}
