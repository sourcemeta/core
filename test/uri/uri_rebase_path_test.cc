#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_rebase_path, single_segment_suffix) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/bar", "/foo", "/qux")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/qux/bar");
}

TEST(URI_rebase_path, multi_segment_suffix) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/bar/baz", "/foo", "/qux")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/qux/bar/baz");
}

TEST(URI_rebase_path, new_prefix_with_trailing_slash_no_duplicate) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/bar/baz", "/foo", "/qux/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/qux/bar/baz");
}

TEST(URI_rebase_path, old_prefix_with_trailing_slash) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/bar", "/foo/", "/qux")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/qux/bar");
}

TEST(URI_rebase_path, exact_match_empty_suffix) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo", "/foo", "/qux")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/qux");
}

TEST(URI_rebase_path, exact_match_new_prefix_with_trailing_slash) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo", "/foo", "/qux/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/qux/");
}

TEST(URI_rebase_path, prefix_not_matched) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/bar/baz", "/foo", "/qux")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_rebase_path, component_boundary_mismatch_returns_nullopt) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foobar", "/foo", "/qux")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_rebase_path, new_prefix_is_full_url) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo/bar", "/foo",
                                                       "https://example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "https://example.com/bar");
}

TEST(URI_rebase_path, new_prefix_is_full_url_with_trailing_slash) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo/bar", "/foo",
                                                       "https://example.com/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "https://example.com/bar");
}

TEST(URI_rebase_path, new_prefix_is_full_url_with_path) {
  const auto result{sourcemeta::core::URI::rebase_path(
      "/foo/bar", "/foo", "https://example.com/api")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "https://example.com/api/bar");
}

TEST(URI_rebase_path, new_prefix_empty_with_suffix) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo/bar", "/foo", "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_rebase_path, new_prefix_empty_empty_suffix) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo", "/foo", "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_rebase_path, old_prefix_empty_strips_leading_slash) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo/bar", "", "/x")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/x/foo/bar");
}

TEST(URI_rebase_path, old_prefix_empty_new_prefix_empty) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo/bar", "", "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo/bar");
}

TEST(URI_rebase_path, root_old_prefix) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo/bar", "/", "/x")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/x/foo/bar");
}

TEST(URI_rebase_path, dot_segments_in_path_resolved) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/./bar", "/foo", "/x")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/x/bar");
}

TEST(URI_rebase_path, dot_dot_resolution_does_not_escape_prefix) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/../etc", "/foo", "/x")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_rebase_path, dot_dot_collapses_within_path) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/a/../bar", "/foo", "/x")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/x/bar");
}

TEST(URI_rebase_path, percent_encoded_data_preserved) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/a%2Fb", "/foo", "/x")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/x/a%2Fb");
}

TEST(URI_rebase_path, malformed_percent_returns_nullopt) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/%2", "/foo", "/x")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_rebase_path, lowercase_hex_normalised) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/%2f", "/foo", "/x")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/x/%2F");
}

TEST(URI_rebase_path, non_absolute_path_returns_nullopt) {
  const auto result{
      sourcemeta::core::URI::rebase_path("foo/bar", "/foo", "/x")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_rebase_path, non_absolute_old_prefix_returns_nullopt) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/bar", "foo", "/x")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_rebase_path, trailing_slash_in_path_preserved) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/bar/", "/foo", "/x")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/x/bar/");
}

TEST(URI_rebase_path, new_prefix_is_filesystem_path) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/bar", "/foo", "/var/lib/data")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/var/lib/data/bar");
}

TEST(URI_rebase_path, deep_nesting) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/a/b/c/d/e", "/a/b/c", "/x/y")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/x/y/d/e");
}

TEST(URI_rebase_path, empty_inputs_all_empty) {
  const auto result{sourcemeta::core::URI::rebase_path("", "", "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_rebase_path, empty_path_with_non_empty_prefix_returns_nullopt) {
  const auto result{sourcemeta::core::URI::rebase_path("", "/foo", "/x")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_rebase_path, new_prefix_with_only_slash) {
  const auto result{
      sourcemeta::core::URI::rebase_path("/foo/bar", "/foo", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/bar");
}

TEST(URI_rebase_path, new_prefix_with_only_slash_empty_suffix) {
  const auto result{sourcemeta::core::URI::rebase_path("/foo", "/foo", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/");
}

TEST(URI_rebase_path, iri_unicode_segments) {
  const auto result{sourcemeta::core::URI::rebase_path(
      "/caf\xC3\xA9/page", "/caf\xC3\xA9", "/menu")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/menu/page");
}

TEST(URI_rebase_path, iri_rejects_invalid_utf8) {
  // An invalid UTF-8 byte sequence in the path is not a valid path
  const auto result{sourcemeta::core::URI::rebase_path(
      "/caf\xC3/page", "/caf\xC3/page", "/menu")};
  EXPECT_FALSE(result.has_value());
}
