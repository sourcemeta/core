#include <sourcemeta/core/test.h>

#include <sourcemeta/core/uri.h>

TEST(full_url) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo/bar");
}

TEST(full_url_with_query) {
  const sourcemeta::core::URI uri{"https://example.com/foo?x=1"};
  EXPECT_EQ(uri.recompose_relative(), "/foo?x=1");
}

TEST(full_url_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo#bar");
}

TEST(full_url_with_query_and_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo?x=1#bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo?x=1#bar");
}

TEST(json_pointer_fragment_preserved) {
  const sourcemeta::core::URI uri{
      "https://example.com/meta/target#/definitions/foo"};
  EXPECT_EQ(uri.recompose_relative(), "/meta/target#/definitions/foo");
}

TEST(no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_EQ(uri.recompose_relative(), "");
}

TEST(no_path_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com#bar"};
  EXPECT_EQ(uri.recompose_relative(), "#bar");
}

TEST(relative_input) {
  const sourcemeta::core::URI uri{"foo/bar"};
  EXPECT_EQ(uri.recompose_relative(), "foo/bar");
}

TEST(relative_input_with_fragment) {
  const sourcemeta::core::URI uri{"foo/bar#baz"};
  EXPECT_EQ(uri.recompose_relative(), "foo/bar#baz");
}

TEST(fragment_only) {
  const sourcemeta::core::URI uri{"#bar"};
  EXPECT_EQ(uri.recompose_relative(), "#bar");
}

TEST(empty_uri) {
  const sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.recompose_relative(), "");
}

TEST(path_only_with_query) {
  const sourcemeta::core::URI uri{"https://example.com/?x=1&y=2"};
  EXPECT_EQ(uri.recompose_relative(), "/?x=1&y=2");
}

TEST(percent_encoded_path) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo%20bar");
}

TEST(query_only_no_path) {
  const sourcemeta::core::URI uri{"https://example.com?x=1"};
  EXPECT_EQ(uri.recompose_relative(), "?x=1");
}

TEST(query_and_fragment_no_path) {
  const sourcemeta::core::URI uri{"https://example.com?x=1#bar"};
  EXPECT_EQ(uri.recompose_relative(), "?x=1#bar");
}

TEST(path_noscheme_first_segment_colon_encoded) {
  const sourcemeta::core::URI uri{"urn:foo:bar"};
  EXPECT_EQ(uri.recompose_relative(), "foo%3Abar");
}

TEST(path_noscheme_colon_only_in_first_segment) {
  const sourcemeta::core::URI uri{"urn:foo:bar/baz:qux"};
  EXPECT_EQ(uri.recompose_relative(), "foo%3Abar/baz:qux");
}

TEST(path_absolute_first_segment_colon_preserved) {
  const sourcemeta::core::URI uri{"https://example.com/foo:bar"};
  EXPECT_EQ(uri.recompose_relative(), "/foo:bar");
}

TEST(iri_preserves_ucschar) {
  const auto uri{
      sourcemeta::core::URI::from_iri("caf\xC3\xA9/page?q=\xCE\xB1")};
  EXPECT_EQ(uri.recompose_relative(), "caf\xC3\xA9/page?q=\xCE\xB1");
}
