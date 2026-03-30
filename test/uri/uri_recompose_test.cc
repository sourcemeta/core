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

TEST(URI_recompose, preserves_scheme_case) {
  const sourcemeta::core::URI uri{"HtTpS://example.com/foo"};
  EXPECT_EQ(uri.recompose(), "HtTpS://example.com/foo");
}

TEST(URI_recompose, preserves_host_case) {
  const sourcemeta::core::URI uri{"https://ExAmPlE.CoM/foo"};
  EXPECT_EQ(uri.recompose(), "https://ExAmPlE.CoM/foo");
}

TEST(URI_recompose, preserves_scheme_and_host_case) {
  const sourcemeta::core::URI uri{"HtTp://ExAmPlE.CoM/foo"};
  EXPECT_EQ(uri.recompose(), "HtTp://ExAmPlE.CoM/foo");
}

// Per RFC 3986 Section 2.2, percent-encoded reserved characters must be
// preserved during recomposition. They are semantically distinct from
// their literal counterparts.

TEST(URI_recompose, encoded_slash_in_path) {
  const sourcemeta::core::URI uri{"http://example.com/v1%2F2.json"};
  EXPECT_EQ(uri.recompose(), "http://example.com/v1%2F2.json");
}

TEST(URI_recompose, encoded_slash_vs_literal_slash_in_path) {
  const sourcemeta::core::URI encoded{"http://example.com/v1%2F2.json"};
  const sourcemeta::core::URI literal{"http://example.com/v1/2.json"};
  EXPECT_NE(encoded.recompose(), literal.recompose());
}

TEST(URI_recompose, encoded_question_mark_in_path) {
  const sourcemeta::core::URI uri{"http://example.com/foo%3Fbar"};
  EXPECT_EQ(uri.recompose(), "http://example.com/foo%3Fbar");
  EXPECT_FALSE(uri.query().has_value());
}

TEST(URI_recompose, encoded_hash_in_path) {
  const sourcemeta::core::URI uri{"http://example.com/foo%23bar"};
  EXPECT_EQ(uri.recompose(), "http://example.com/foo%23bar");
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_recompose, encoded_ampersand_in_query) {
  const sourcemeta::core::URI uri{"http://example.com/path?foo%26bar"};
  EXPECT_EQ(uri.recompose(), "http://example.com/path?foo%26bar");
}

TEST(URI_recompose, encoded_hash_in_query) {
  const sourcemeta::core::URI uri{"http://example.com/path?foo%23bar"};
  EXPECT_EQ(uri.recompose(), "http://example.com/path?foo%23bar");
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_recompose, encoded_slash_in_fragment) {
  const sourcemeta::core::URI uri{"http://example.com/path#/foo%2Fbar"};
  EXPECT_EQ(uri.recompose(), "http://example.com/path#/foo%2Fbar");
}

TEST(URI_recompose, encoded_colon_in_path) {
  const sourcemeta::core::URI uri{"http://example.com/foo%3Abar"};
  EXPECT_EQ(uri.recompose(), "http://example.com/foo%3Abar");
}

TEST(URI_recompose, encoded_at_in_path) {
  const sourcemeta::core::URI uri{"http://example.com/foo%40bar"};
  EXPECT_EQ(uri.recompose(), "http://example.com/foo%40bar");
}

TEST(URI_recompose, multiple_encoded_reserved_in_path) {
  const sourcemeta::core::URI uri{"http://example.com/a%2Fb%3Fc%23d%40e"};
  EXPECT_EQ(uri.recompose(), "http://example.com/a%2Fb%3Fc%23d%40e");
}

TEST(URI_recompose, mixed_encoded_and_literal_reserved_in_path) {
  const sourcemeta::core::URI uri{"http://example.com/a/b%2Fc/d"};
  EXPECT_EQ(uri.recompose(), "http://example.com/a/b%2Fc/d");
}

TEST(URI_recompose, encoded_low_byte_zero_padded) {
  const sourcemeta::core::URI uri{"http://example.com/foo%0Abar"};
  EXPECT_EQ(uri.recompose(), "http://example.com/foo%0Abar");
}

// RFC 3986 Section 5.3: path-absolute without authority must preserve
// the leading slash verbatim during recomposition
TEST(URI_recompose, path_absolute_without_authority_file) {
  const sourcemeta::core::URI uri{"file:/path"};
  EXPECT_EQ(uri.recompose(), "file:/path");
}

TEST(URI_recompose, path_absolute_without_authority_multi_segment) {
  const sourcemeta::core::URI uri{"x:/a/b/c"};
  EXPECT_EQ(uri.recompose(), "x:/a/b/c");
}

TEST(URI_recompose, path_empty_with_scheme) {
  const sourcemeta::core::URI uri{"a:"};
  EXPECT_EQ(uri.recompose(), "a:");
}

TEST(URI_recompose, path_absolute_without_authority_roundtrip) {
  const sourcemeta::core::URI original{"file:/path"};
  const sourcemeta::core::URI roundtrip{original.recompose()};
  EXPECT_EQ(roundtrip.recompose(), "file:/path");
}
