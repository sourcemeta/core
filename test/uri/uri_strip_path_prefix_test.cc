#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(exact_match) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(exact_match_root) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(suffix_single_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(suffix_multi_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar/baz", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar/baz");
}

TEST(prefix_with_trailing_slash_matches) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/foo/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(prefix_with_trailing_slash_against_bare_path) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo", "/foo/")};
  EXPECT_FALSE(result.has_value());
}

TEST(path_with_trailing_slash) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(path_with_trailing_slash_both) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/", "/foo/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(prefix_does_not_match) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/bar/baz", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(component_boundary_required) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foobar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(component_boundary_required_multi) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foobar/baz", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(suffix_with_trailing_slash) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar/", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar/");
}

TEST(empty_prefix_returns_path_without_leading_slash) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/bar", "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo/bar");
}

TEST(empty_prefix_empty_path) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("", "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(empty_path_non_empty_prefix) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(slash_prefix_matches_anything) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo/bar");
}

TEST(slash_prefix_against_root) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(deep_nesting) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/a/b/c/d/e/f", "/a/b/c")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "d/e/f");
}

TEST(full_match_deep) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/a/b/c", "/a/b/c")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(partial_prefix_with_extra_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/a/b/cd", "/a/b/c")};
  EXPECT_FALSE(result.has_value());
}

TEST(dot_segments_in_path_resolved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/./bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(dot_dot_segments_in_path_resolved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/baz/../bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(dot_dot_does_not_escape_prefix) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/../etc", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(dot_dot_at_root_collapses) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/..", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(many_dot_dot_at_root_collapses) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/../../../foo", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo");
}

TEST(dot_segments_in_prefix_resolved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/x/../foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(dot_only_path) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/.", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(percent_encoded_slash_kept_as_data) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%2Fb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%2Fb");
}

TEST(encoded_dot_dot_resolves_and_escapes_prefix) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2E%2E/bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(lowercase_hex_normalised_to_uppercase) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2f", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "%2F");
}

TEST(encoded_sub_delim_uppercased_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2a", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "%2A");
}

TEST(encoded_unreserved_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%41", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "A");
}

TEST(truncated_percent_encoding_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(percent_alone_rejected) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo/%", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(non_hex_percent_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%ZZ", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(empty_mid_segment_preserved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo//bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/bar");
}

TEST(space_in_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(backslash_in_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo\\bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(control_char_in_path_rejected) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo\x01"
                                                             "bar",
                                                             "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(question_mark_in_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo?bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(hash_in_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo#bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(non_absolute_path_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("foo/bar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(non_absolute_prefix_rejected) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(sub_delims_allowed_in_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a!b$c&d'e", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a!b$c&d'e");
}

TEST(colon_and_at_allowed_in_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/user:pass@host", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "user:pass@host");
}

TEST(unreserved_allowed_in_segment) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/abc-XYZ_123.~", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abc-XYZ_123.~");
}

TEST(encoded_unreserved_in_prefix_matches_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/%66oo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(percent_encoded_segment_matches_byte_for_byte) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%21/bar", "/foo/%21")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(encoded_sub_delim_not_equivalent_to_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%21/bar", "/foo/!")};
  EXPECT_FALSE(result.has_value());
}

TEST(prefix_is_proper_prefix_segment_boundary) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/foo/ba")};
  EXPECT_FALSE(result.has_value());
}

TEST(deeply_nested_dot_dot_resolution) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/a/b/c/../../x", "/a")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "x");
}

TEST(prefix_ending_at_dot_segment_boundary) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar/.", "/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(trailing_empty_segments_preserved) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("/foo//", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/");
}

TEST(multiple_trailing_empty_segments_preserved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo///", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "//");
}

TEST(mid_and_trailing_empty_segments_preserved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo//bar//", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/bar//");
}

TEST(path_of_only_slashes_preserved) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("////", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "///");
}

TEST(path_abempty_double_leading_slash) {
  const auto result{sourcemeta::core::URI::strip_path_prefix("//foo/bar", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "/foo/bar");
}

TEST(encoded_hyphen_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%2Db", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a-b");
}

TEST(encoded_underscore_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%5Fb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a_b");
}

TEST(encoded_period_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%2Eb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a.b");
}

TEST(encoded_digit_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%30", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "0");
}

TEST(encoded_plus_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%2Bb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%2Bb");
}

TEST(encoded_semicolon_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%3Bb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%3Bb");
}

TEST(encoded_equals_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%3Db", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%3Db");
}

TEST(encoded_open_bracket_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%5Bb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%5Bb");
}

TEST(encoded_null_byte_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%00", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "%00");
}

TEST(high_byte_percent_encoded_preserved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%FF", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "%FF");
}

TEST(segment_named_three_dots_passes_through) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/.../bar", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), ".../bar");
}

TEST(encoded_three_dots_segment_passes_through) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2E%2E%2E", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "...");
}

TEST(lowercase_encoded_dot_dot_resolves_after_normalisation) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2e%2e/bar", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(mixed_encoded_and_literal_unreserved) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%62c%64", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "abcd");
}

TEST(lowercase_hex_in_prefix_normalised) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%2F/bar", "/foo/%2f")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "bar");
}

TEST(encoded_unreserved_in_prefix_equivalent_to_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/bar", "/foo/%62%61%72")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(encoded_tilde_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%7Ehome", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "~home");
}

TEST(encoded_at_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/%40handle", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "%40handle");
}

TEST(encoded_colon_not_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%3Ab", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%3Ab");
}

TEST(encoded_slash_never_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo%2Fbar", "/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(encoded_question_mark_never_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%3Fb", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%3Fb");
}

TEST(encoded_hash_never_decoded) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/a%23b", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a%23b");
}

TEST(mixed_dot_segments_and_encoded_traversal) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/foo/x/%2E%2E/y", "/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "y");
}

TEST(encoded_dot_segment_resolved_at_root_collapses) {
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/%2E%2E/foo", "/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo");
}

TEST(iri_unicode_segments) {
  const auto result{sourcemeta::core::URI::strip_path_prefix(
      "/caf\xC3\xA9/page", "/caf\xC3\xA9")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "page");
}

TEST(iri_rejects_invalid_utf8) {
  // An invalid UTF-8 byte sequence in the path is not a valid path
  const auto result{
      sourcemeta::core::URI::strip_path_prefix("/caf\xC3/page", "/")};
  EXPECT_FALSE(result.has_value());
}
