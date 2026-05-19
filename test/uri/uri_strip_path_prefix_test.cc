#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_strip_path_prefix, exact_match) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, exact_match_root) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, suffix_single_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_strip_path_prefix, suffix_multi_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar/baz", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar/baz");
}

TEST(URI_strip_path_prefix, prefix_with_trailing_slash_matches) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/foo/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_strip_path_prefix, prefix_with_trailing_slash_against_bare_path) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo", "/foo/")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, path_with_trailing_slash) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, path_with_trailing_slash_both) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/", "/foo/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, prefix_does_not_match) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/bar/baz", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, component_boundary_required) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foobar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, component_boundary_required_multi) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foobar/baz", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, suffix_with_trailing_slash) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar/", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar/");
}

TEST(URI_strip_path_prefix, empty_prefix_returns_path_without_leading_slash) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/bar", "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo/bar");
}

TEST(URI_strip_path_prefix, empty_prefix_empty_path) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("", "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, empty_path_non_empty_prefix) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, slash_prefix_matches_anything) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo/bar");
}

TEST(URI_strip_path_prefix, slash_prefix_against_root) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, deep_nesting) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/a/b/c/d/e/f", "/a/b/c")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "d/e/f");
}

TEST(URI_strip_path_prefix, full_match_deep) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/a/b/c", "/a/b/c")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, partial_prefix_with_extra_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/a/b/cd", "/a/b/c")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, dot_segments_in_path_resolved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/./bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_strip_path_prefix, dot_dot_segments_in_path_resolved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/baz/../bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_strip_path_prefix, dot_dot_does_not_escape_prefix) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/../etc", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, dot_dot_at_root_collapses) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/..", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, many_dot_dot_at_root_collapses) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/../../../foo", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo");
}

TEST(URI_strip_path_prefix, dot_segments_in_prefix_resolved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/x/../foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_strip_path_prefix, dot_only_path) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/.", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, percent_encoded_slash_kept_as_data) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%2Fb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%2Fb");
}

TEST(URI_strip_path_prefix, encoded_dot_dot_resolves_and_escapes_prefix) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2E%2E/bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, lowercase_hex_normalised_to_uppercase) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2f", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "%2F");
}

TEST(URI_strip_path_prefix, encoded_sub_delim_uppercased_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2a", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "%2A");
}

TEST(URI_strip_path_prefix, encoded_unreserved_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%41", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "A");
}

TEST(URI_strip_path_prefix, truncated_percent_encoding_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, percent_alone_rejected) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/%", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, non_hex_percent_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%ZZ", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, empty_mid_segment_preserved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo//bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/bar");
}

TEST(URI_strip_path_prefix, space_in_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, backslash_in_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo\\bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, control_char_in_path_rejected) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo\x01"
                                                             "bar",
                                                             "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, question_mark_in_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo?bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, hash_in_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo#bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, non_absolute_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("foo/bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, non_absolute_prefix_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, sub_delims_allowed_in_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a!b$c&d'e", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a!b$c&d'e");
}

TEST(URI_strip_path_prefix, colon_and_at_allowed_in_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/user:pass@host", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "user:pass@host");
}

TEST(URI_strip_path_prefix, unreserved_allowed_in_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/abc-XYZ_123.~", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abc-XYZ_123.~");
}

TEST(URI_strip_path_prefix, encoded_unreserved_in_prefix_matches_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/%66oo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_strip_path_prefix, percent_encoded_segment_matches_byte_for_byte) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%21/bar", "/foo/%21")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_strip_path_prefix, encoded_sub_delim_not_equivalent_to_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%21/bar", "/foo/!")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, prefix_is_proper_prefix_segment_boundary) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/foo/ba")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, deeply_nested_dot_dot_resolution) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/a/b/c/../../x", "/a")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "x");
}

TEST(URI_strip_path_prefix, prefix_ending_at_dot_segment_boundary) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar/.", "/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, trailing_empty_segments_preserved) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo//", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/");
}

TEST(URI_strip_path_prefix, multiple_trailing_empty_segments_preserved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo///", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "//");
}

TEST(URI_strip_path_prefix, mid_and_trailing_empty_segments_preserved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo//bar//", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/bar//");
}

TEST(URI_strip_path_prefix, path_of_only_slashes_preserved) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("////", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "///");
}

TEST(URI_strip_path_prefix, path_abempty_double_leading_slash) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("//foo/bar", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/foo/bar");
}

TEST(URI_strip_path_prefix, lowercase_hex_in_prefix_normalised) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2F/bar", "/foo/%2f")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(URI_strip_path_prefix,
     encoded_unreserved_in_prefix_equivalent_to_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/foo/%62%61%72")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(URI_strip_path_prefix, encoded_tilde_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%7Ehome", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "~home");
}

TEST(URI_strip_path_prefix, encoded_at_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%40handle", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "%40handle");
}

TEST(URI_strip_path_prefix, encoded_colon_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%3Ab", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%3Ab");
}

TEST(URI_strip_path_prefix, encoded_slash_never_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo%2Fbar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URI_strip_path_prefix, encoded_question_mark_never_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%3Fb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%3Fb");
}

TEST(URI_strip_path_prefix, encoded_hash_never_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%23b", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%23b");
}

TEST(URI_strip_path_prefix, mixed_dot_segments_and_encoded_traversal) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/x/%2E%2E/y", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "y");
}

TEST(URI_strip_path_prefix, encoded_dot_segment_resolved_at_root_collapses) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/%2E%2E/foo", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo");
}
