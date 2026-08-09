#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string> // std::string

// RFC 3986 defines resolution but no way to compute a relative reference, so
// the correctness of relativization is defined by resolution undoing it:
//
//   resolve_from(relative_to(target, base), base) == target
//
// Resolution is the fixed side of that equation. Section 5.2.2 specifies it
// exactly, and every one of the 41 examples in the Section 5.4 tables is
// asserted in the resolution tests, so it can be treated as the reference
// implementation that relativization is measured against rather than as
// another moving part.
//
// Both inputs must be absolute, as Section 5.1 requires an absolute base, and
// the target path must be free of dot segments, as Section 5.2.2 removes those
// on every resolution branch and no reference can reproduce them. Every case in
// the relativization tests that meets those conditions has an entry here. A
// failure means the implementation is wrong, not the expectation.

static auto round_trip(const std::string &base_string,
                       const std::string &target_string) -> std::string {
  const sourcemeta::core::URI base{base_string};
  sourcemeta::core::URI target{target_string};
  target.relative_to(base);
  sourcemeta::core::URI resolved{target.recompose()};
  resolved.resolve_from(base);
  return resolved.recompose();
}

// The parser decodes percent-encoded unreserved characters, so comparing
// against the raw target string would fail for reasons unrelated to
// relativization
static auto normalised(const std::string &input) -> std::string {
  return sourcemeta::core::URI{input}.recompose();
}

TEST(absolute_absolute_base_true_1) {
  EXPECT_EQ(
      round_trip("https://www.example.com", "https://www.example.com/foo"),
      normalised("https://www.example.com/foo"));
}

TEST(absolute_absolute_base_true_2) {
  EXPECT_EQ(
      round_trip("https://www.example.com/foo", "https://www.example.com/foo"),
      normalised("https://www.example.com/foo"));
}

TEST(absolute_absolute_base_true_3) {
  EXPECT_EQ(round_trip("https://www.example.com/foo",
                       "https://www.example.com/foo/bar?q=1"),
            normalised("https://www.example.com/foo/bar?q=1"));
}

TEST(absolute_absolute_base_true_4) {
  EXPECT_EQ(round_trip("https://www.example.com/foo",
                       "https://www.example.com/foo/bar#baz"),
            normalised("https://www.example.com/foo/bar#baz"));
}

TEST(absolute_absolute_base_false_1) {
  EXPECT_EQ(
      round_trip("https://www.example.com/foo", "http://www.example.com/foo"),
      normalised("http://www.example.com/foo"));
}

TEST(absolute_absolute_base_false_2) {
  EXPECT_EQ(
      round_trip("https://www.example.com/foo", "https://www.example.com"),
      normalised("https://www.example.com"));
}

TEST(absolute_absolute_base_false_3) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/bar",
                       "https://www.example.com/foo"),
            normalised("https://www.example.com/foo"));
}

TEST(absolute_absolute_base_false_4) {
  EXPECT_EQ(round_trip("https://foo.com", "https://bar.com"),
            normalised("https://bar.com"));
}

TEST(absolute_absolute_base_false_different_ports) {
  EXPECT_EQ(round_trip("http://localhost:8000",
                       "http://localhost:9000/schemas/test.json"),
            normalised("http://localhost:9000/schemas/test.json"));
}

TEST(absolute_absolute_base_false_different_userinfo) {
  EXPECT_EQ(round_trip("https://alice@example.com/foo",
                       "https://bob@example.com/foo/bar"),
            normalised("https://bob@example.com/foo/bar"));
}

TEST(absolute_absolute_base_false_userinfo_vs_none) {
  EXPECT_EQ(round_trip("https://example.com/foo",
                       "https://alice@example.com/foo/bar"),
            normalised("https://alice@example.com/foo/bar"));
}

TEST(urn_1) {
  EXPECT_EQ(round_trip("schema:", "schema:myschema"),
            normalised("schema:myschema"));
}

TEST(absolute_absolute_trailing_slash) {
  EXPECT_EQ(
      round_trip("https://github.com/apis-json/api-json/blob/develop/spec",
                 "https://github.com/apis-json/api-json/blob/develop/spec/"),
      normalised("https://github.com/apis-json/api-json/blob/develop/spec/"));
}

TEST(target_is_prefix_of_base_parent) {
  EXPECT_EQ(
      round_trip("https://example.com/foo/bar/baz", "https://example.com/foo"),
      normalised("https://example.com/foo"));
}

TEST(target_is_one_level_up_at_root) {
  EXPECT_EQ(
      round_trip("https://example.com/foo/bar", "https://example.com/foo"),
      normalised("https://example.com/foo"));
}

TEST(target_is_root) {
  EXPECT_EQ(round_trip("https://example.com/foo/bar", "https://example.com/"),
            normalised("https://example.com/"));
}

TEST(base_ends_with_slash) {
  EXPECT_EQ(
      round_trip("https://example.com/foo/", "https://example.com/foo/bar"),
      normalised("https://example.com/foo/bar"));
}

TEST(base_root_only) {
  EXPECT_EQ(round_trip("https://example.com/", "https://example.com/foo"),
            normalised("https://example.com/foo"));
}

TEST(sibling_paths_same_directory) {
  EXPECT_EQ(round_trip("https://example.com/schemas/bar.json",
                       "https://example.com/schemas/foo.json"),
            normalised("https://example.com/schemas/foo.json"));
}

TEST(double_slash_with_trailing_slash) {
  EXPECT_EQ(round_trip("https://example.com/slash/",
                       "https://example.com/slash/file.json"),
            normalised("https://example.com/slash/file.json"));
}

TEST(different_directories_same_host_needs_dotdot) {
  EXPECT_EQ(round_trip("https://example.com/schemas/with-rebase-same-host.json",
                       "https://example.com/bundling/single"),
            normalised("https://example.com/bundling/single"));
}

TEST(different_directories_same_host_needs_dotdot_2) {
  EXPECT_EQ(round_trip("https://example.com/foo/bar/baz.json",
                       "https://example.com/qux/test.json"),
            normalised("https://example.com/qux/test.json"));
}

TEST(different_directories_same_host_needs_dotdot_3) {
  EXPECT_EQ(round_trip("https://example.com/a/b/c.json",
                       "https://example.com/d.json"),
            normalised("https://example.com/d.json"));
}

TEST(file_same_directory) {
  EXPECT_EQ(round_trip("file:///home/user/schemas/base.json",
                       "file:///home/user/schemas/other.json"),
            normalised("file:///home/user/schemas/other.json"));
}

TEST(file_subdirectory) {
  EXPECT_EQ(round_trip("file:///home/user/schemas",
                       "file:///home/user/schemas/sub/test.json"),
            normalised("file:///home/user/schemas/sub/test.json"));
}

TEST(file_parent_directory) {
  EXPECT_EQ(round_trip("file:///home/user/schemas/sub/base.json",
                       "file:///home/user/schemas/other.json"),
            normalised("file:///home/user/schemas/other.json"));
}

TEST(file_different_root) {
  EXPECT_EQ(round_trip("file:///home/user/schemas/base.json",
                       "file:///var/data/test.json"),
            normalised("file:///var/data/test.json"));
}

TEST(file_same_file) {
  EXPECT_EQ(round_trip("file:///home/user/schemas/base.json",
                       "file:///home/user/schemas/base.json"),
            normalised("file:///home/user/schemas/base.json"));
}

TEST(file_with_fragment) {
  EXPECT_EQ(round_trip("file:///home/user/schemas/base.json",
                       "file:///home/user/schemas/other.json#/defs/foo"),
            normalised("file:///home/user/schemas/other.json#/defs/foo"));
}

TEST(file_windows_same_directory) {
  EXPECT_EQ(round_trip("file:///C:/Users/user/schemas/base.json",
                       "file:///C:/Users/user/schemas/other.json"),
            normalised("file:///C:/Users/user/schemas/other.json"));
}

TEST(file_windows_subdirectory) {
  EXPECT_EQ(round_trip("file:///C:/Users/user/schemas",
                       "file:///C:/Users/user/schemas/sub/test.json"),
            normalised("file:///C:/Users/user/schemas/sub/test.json"));
}

TEST(file_windows_parent_directory) {
  EXPECT_EQ(round_trip("file:///C:/Users/user/schemas/sub/base.json",
                       "file:///C:/Users/user/schemas/other.json"),
            normalised("file:///C:/Users/user/schemas/other.json"));
}

TEST(file_windows_different_drive) {
  EXPECT_EQ(round_trip("file:///C:/Users/user/schemas/base.json",
                       "file:///D:/Data/test.json"),
            normalised("file:///D:/Data/test.json"));
}

TEST(file_windows_same_file) {
  EXPECT_EQ(round_trip("file:///C:/Users/user/schemas/base.json",
                       "file:///C:/Users/user/schemas/base.json"),
            normalised("file:///C:/Users/user/schemas/base.json"));
}

TEST(file_windows_with_fragment) {
  EXPECT_EQ(round_trip("file:///C:/Users/user/schemas/base.json",
                       "file:///C:/Users/user/schemas/other.json#/defs/foo"),
            normalised("file:///C:/Users/user/schemas/other.json#/defs/foo"));
}

TEST(same_path_only_query_differs) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/bar",
                       "https://www.example.com/foo/bar?q=1"),
            normalised("https://www.example.com/foo/bar?q=1"));
}

TEST(same_path_only_fragment_differs) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/bar",
                       "https://www.example.com/foo/bar#baz"),
            normalised("https://www.example.com/foo/bar#baz"));
}

TEST(same_path_query_and_fragment_differ) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/bar",
                       "https://www.example.com/foo/bar?q=1#baz"),
            normalised("https://www.example.com/foo/bar?q=1#baz"));
}

TEST(same_path_base_query_target_none) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/bar?q=1",
                       "https://www.example.com/foo/bar"),
            normalised("https://www.example.com/foo/bar"));
}

TEST(same_path_base_query_target_fragment_only) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/bar?q=1",
                       "https://www.example.com/foo/bar#baz"),
            normalised("https://www.example.com/foo/bar#baz"));
}

TEST(same_path_base_query_target_other_query) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/bar?q=1",
                       "https://www.example.com/foo/bar?q=2"),
            normalised("https://www.example.com/foo/bar?q=2"));
}

TEST(same_directory_path_base_query_target_none) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/?q=1",
                       "https://www.example.com/foo/"),
            normalised("https://www.example.com/foo/"));
}

TEST(same_directory_path_base_query_target_fragment) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/?q=1",
                       "https://www.example.com/foo/#baz"),
            normalised("https://www.example.com/foo/#baz"));
}

TEST(same_directory_path_target_query) {
  EXPECT_EQ(round_trip("https://www.example.com/foo/?q=1",
                       "https://www.example.com/foo/?q=2"),
            normalised("https://www.example.com/foo/?q=2"));
}

TEST(authority_no_path_only_fragment_differs) {
  EXPECT_EQ(
      round_trip("https://www.example.com", "https://www.example.com#baz"),
      normalised("https://www.example.com#baz"));
}

TEST(authority_no_path_only_query_differs) {
  EXPECT_EQ(
      round_trip("https://www.example.com?q=1", "https://www.example.com?q=2"),
      normalised("https://www.example.com?q=2"));
}

TEST(authority_no_path_base_query_target_none) {
  EXPECT_EQ(
      round_trip("https://www.example.com?q=1", "https://www.example.com"),
      normalised("https://www.example.com"));
}

TEST(relative_to_same_path_with_query_in_base) {
  EXPECT_EQ(round_trip("schema:foo", "schema:foo?bar=1"),
            normalised("schema:foo?bar=1"));
}

TEST(relative_to_slashless_base_path) {
  EXPECT_EQ(round_trip("schema:foo", "schema:bar"), normalised("schema:bar"));
}

TEST(target_is_directory_of_base) {
  EXPECT_EQ(round_trip("https://example.com/test/foo.json",
                       "https://example.com/test/"),
            normalised("https://example.com/test/"));
}

TEST(target_is_nested_directory_of_base) {
  EXPECT_EQ(round_trip("https://example.com/foo/bar/baz.json",
                       "https://example.com/foo/bar/"),
            normalised("https://example.com/foo/bar/"));
}

TEST(target_is_directory_of_base_with_query) {
  EXPECT_EQ(round_trip("https://example.com/test/foo.json",
                       "https://example.com/test/?q=1"),
            normalised("https://example.com/test/?q=1"));
}

TEST(target_is_directory_of_base_with_fragment) {
  EXPECT_EQ(round_trip("https://example.com/test/foo.json",
                       "https://example.com/test/#baz"),
            normalised("https://example.com/test/#baz"));
}

TEST(file_target_is_directory_of_base) {
  EXPECT_EQ(round_trip("file:///home/user/schemas/base.json",
                       "file:///home/user/schemas/"),
            normalised("file:///home/user/schemas/"));
}

TEST(target_is_root_of_base_without_path) {
  EXPECT_EQ(round_trip("https://example.com", "https://example.com/"),
            normalised("https://example.com/"));
}

TEST(target_with_empty_first_segment_of_base_without_path) {
  EXPECT_EQ(round_trip("https://example.com", "https://example.com//foo"),
            normalised("https://example.com//foo"));
}

TEST(target_with_empty_first_segment_of_root_base) {
  EXPECT_EQ(round_trip("https://example.com/foo", "https://example.com//bar"),
            normalised("https://example.com//bar"));
}

TEST(target_with_empty_segment_below_base) {
  EXPECT_EQ(
      round_trip("https://example.com/foo", "https://example.com/foo//bar"),
      normalised("https://example.com/foo//bar"));
}

TEST(target_with_empty_first_segment_from_nested_base) {
  EXPECT_EQ(round_trip("https://example.com/a/b", "https://example.com//bar"),
            normalised("https://example.com//bar"));
}

TEST(target_with_empty_segment_below_directory_base) {
  EXPECT_EQ(
      round_trip("https://example.com/foo/", "https://example.com/foo//bar"),
      normalised("https://example.com/foo//bar"));
}

TEST(target_with_empty_segment_below_nested_directory_base) {
  EXPECT_EQ(round_trip("https://example.com/a/b/c.json",
                       "https://example.com/a/b//d"),
            normalised("https://example.com/a/b//d"));
}

TEST(target_is_absolute_path_of_base_without_authority) {
  EXPECT_EQ(round_trip("schema:", "schema:/foo"), normalised("schema:/foo"));
}

TEST(sibling_with_colon_in_first_segment) {
  EXPECT_EQ(round_trip("file:///C:/Users/user/schemas/base.json",
                       "file:///C:/Users/user/schemas/D:foo.json"),
            normalised("file:///C:/Users/user/schemas/D:foo.json"));
}

TEST(descendant_of_base_names_the_base_segment) {
  EXPECT_EQ(
      round_trip("https://example.com/foo", "https://example.com/foo/bar"),
      normalised("https://example.com/foo/bar"));
}

TEST(base_with_fragment) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b#frag", "https://example.com/a/c"),
      normalised("https://example.com/a/c"));
}

TEST(base_with_query) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b?q=1", "https://example.com/a/c"),
      normalised("https://example.com/a/c"));
}

TEST(userinfo) {
  EXPECT_EQ(round_trip("https://user:pass@example.com/a/b",
                       "https://user:pass@example.com/a/c"),
            normalised("https://user:pass@example.com/a/c"));
}

TEST(non_default_port) {
  EXPECT_EQ(round_trip("https://example.com:8443/a/b",
                       "https://example.com:8443/a/c"),
            normalised("https://example.com:8443/a/c"));
}

TEST(percent_encoded_space) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b", "https://example.com/a/c%20d"),
      normalised("https://example.com/a/c%20d"));
}

TEST(percent_encoded_unreserved) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b", "https://example.com/a/c%41d"),
      normalised("https://example.com/a/c%41d"));
}

TEST(percent_encoded_slash) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b", "https://example.com/a/c%2Fd"),
      normalised("https://example.com/a/c%2Fd"));
}

TEST(literal_dotdot_prefix_segment) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b", "https://example.com/a/..foo"),
      normalised("https://example.com/a/..foo"));
}

TEST(literal_dotdot_suffix_segment) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b", "https://example.com/a/foo.."),
      normalised("https://example.com/a/foo.."));
}

TEST(literal_dot_prefix_segment) {
  EXPECT_EQ(round_trip("https://example.com/a/b", "https://example.com/a/.foo"),
            normalised("https://example.com/a/.foo"));
}

TEST(target_without_path) {
  EXPECT_EQ(round_trip("https://example.com/a/b", "https://example.com"),
            normalised("https://example.com"));
}

TEST(identical_without_path) {
  EXPECT_EQ(round_trip("https://example.com", "https://example.com"),
            normalised("https://example.com"));
}

TEST(foreign_authority) {
  EXPECT_EQ(round_trip("https://example.com/a/b", "https://other.com/x"),
            normalised("https://other.com/x"));
}

TEST(slashless_base) {
  EXPECT_EQ(round_trip("schema:foo", "schema:bar"), normalised("schema:bar"));
}

TEST(deep_target_from_root_base) {
  EXPECT_EQ(round_trip("https://example.com/", "https://example.com/a/b/c"),
            normalised("https://example.com/a/b/c"));
}

TEST(parent_of_base_directory) {
  EXPECT_EQ(round_trip("https://example.com/a/", "https://example.com/"),
            normalised("https://example.com/"));
}

TEST(query_and_fragment) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b", "https://example.com/a/b?q=1#f"),
      normalised("https://example.com/a/b?q=1#f"));
}

TEST(windows_drive_sibling) {
  EXPECT_EQ(round_trip("file:///C:/x/y.json", "file:///C:/x/D:foo.json"),
            normalised("file:///C:/x/D:foo.json"));
}

TEST(target_with_trailing_slash_at_depth) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b/c", "https://example.com/a/b/d/"),
      normalised("https://example.com/a/b/d/"));
}

TEST(target_is_base_with_trailing_slash) {
  EXPECT_EQ(round_trip("https://example.com/a/b", "https://example.com/a/b/"),
            normalised("https://example.com/a/b/"));
}

TEST(deep_descendant) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b", "https://example.com/a/b/c/d"),
      normalised("https://example.com/a/b/c/d"));
}

TEST(descendant_with_query_and_fragment) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b", "https://example.com/a/b/c?q=1#f"),
      normalised("https://example.com/a/b/c?q=1#f"));
}

TEST(base_last_segment_has_literal_dots) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b..", "https://example.com/a/b../c"),
      normalised("https://example.com/a/b../c"));
}

TEST(windows_descendant) {
  EXPECT_EQ(round_trip("file:///C:/x", "file:///C:/x/y.json"),
            normalised("file:///C:/x/y.json"));
}

TEST(colon_in_base_last_segment_descendant) {
  EXPECT_EQ(
      round_trip("https://example.com/D:foo", "https://example.com/D:foo/bar"),
      normalised("https://example.com/D:foo/bar"));
}

TEST(descendant_with_empty_segment) {
  EXPECT_EQ(round_trip("https://example.com/a/b", "https://example.com/a/b//c"),
            normalised("https://example.com/a/b//c"));
}

TEST(descendant_of_single_segment_base) {
  EXPECT_EQ(round_trip("https://example.com/a", "https://example.com/a/b"),
            normalised("https://example.com/a/b"));
}

TEST(iri_preserves_ucschar) {
  const auto base{sourcemeta::core::URI::from_iri("https://example.com/dir/")};
  auto target{
      sourcemeta::core::URI::from_iri("https://example.com/dir/caf\xC3\xA9")};
  target.relative_to(base);
  auto resolved{sourcemeta::core::URI::from_iri(target.recompose())};
  resolved.resolve_from(base);
  EXPECT_EQ(resolved.recompose(), "https://example.com/dir/caf\xC3\xA9");
}

TEST(slashless_base_with_descendant_target) {
  EXPECT_EQ(round_trip("schema:foo", "schema:foo/bar"),
            normalised("schema:foo/bar"));
}

TEST(ipv6_host) {
  EXPECT_EQ(round_trip("https://[::1]/a/b", "https://[::1]/a/c"),
            normalised("https://[::1]/a/c"));
}

TEST(parent_of_nested_directory_base) {
  EXPECT_EQ(round_trip("https://example.com/a/b/", "https://example.com/a/"),
            normalised("https://example.com/a/"));
}

TEST(ancestor_two_levels_up) {
  EXPECT_EQ(round_trip("https://example.com/a/b/c", "https://example.com/a"),
            normalised("https://example.com/a"));
}

TEST(directory_base_with_target_of_the_same_name) {
  EXPECT_EQ(round_trip("https://example.com/a/", "https://example.com/a"),
            normalised("https://example.com/a"));
}

TEST(file_base_with_directory_target_of_the_same_name) {
  EXPECT_EQ(round_trip("https://example.com/a", "https://example.com/a/"),
            normalised("https://example.com/a/"));
}

TEST(windows_drive_root_from_nested_base) {
  EXPECT_EQ(round_trip("file:///C:/x/y/", "file:///C:/"),
            normalised("file:///C:/"));
}

TEST(identical_without_path_but_with_query) {
  EXPECT_EQ(round_trip("https://example.com?q=1", "https://example.com?q=1"),
            normalised("https://example.com?q=1"));
}

TEST(ancestor_three_levels_up) {
  EXPECT_EQ(round_trip("https://example.com/a/b/c/d", "https://example.com/a"),
            normalised("https://example.com/a"));
}

TEST(mailto_slashless_base) {
  EXPECT_EQ(round_trip("mailto:foo", "mailto:bar"), normalised("mailto:bar"));
}

TEST(target_with_empty_fragment) {
  EXPECT_EQ(round_trip("https://example.com/a/b", "https://example.com/a/b#"),
            normalised("https://example.com/a/b#"));
}

TEST(target_with_empty_query) {
  EXPECT_EQ(round_trip("https://example.com/a/b", "https://example.com/a/b?"),
            normalised("https://example.com/a/b?"));
}

TEST(sibling_carrying_the_same_query) {
  EXPECT_EQ(
      round_trip("https://example.com/a/b?x=1", "https://example.com/a/c?x=1"),
      normalised("https://example.com/a/c?x=1"));
}

TEST(root_base_with_target_without_path) {
  EXPECT_EQ(round_trip("https://example.com/", "https://example.com"),
            normalised("https://example.com"));
}

TEST(descendant_of_a_base_with_an_empty_first_segment) {
  EXPECT_EQ(round_trip("https://example.com//a", "https://example.com//a/b"),
            normalised("https://example.com//a/b"));
}

TEST(target_gains_an_empty_first_segment) {
  EXPECT_EQ(round_trip("https://example.com/a", "https://example.com//a"),
            normalised("https://example.com//a"));
}
