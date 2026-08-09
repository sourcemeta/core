#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

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

// Both preconditions are asserted rather than assumed. RFC 3986 Section 5.1
// resolves against an absolute base, and a relative target is not something the
// equation can reproduce. The expected value is the target parsed and
// recomposed rather than the target string itself, because the parser decodes
// percent-encoded unreserved characters and that has nothing to do with
// relativization
#define EXPECT_ROUND_TRIP(base_string, target_string)                          \
  {                                                                            \
    const sourcemeta::core::URI base{(base_string)};                           \
    sourcemeta::core::URI target{(target_string)};                             \
    EXPECT_TRUE(base.is_absolute());                                           \
    EXPECT_TRUE(target.is_absolute());                                         \
    target.relative_to(base);                                                  \
    sourcemeta::core::URI resolved{target.recompose()};                        \
    resolved.resolve_from(base);                                               \
    EXPECT_EQ(resolved.recompose(),                                            \
              sourcemeta::core::URI{(target_string)}.recompose());             \
  }

// RFC 3986 is ASCII only, so a non-ASCII input has to travel the RFC 3987 path
// from parsing through to recomposition for the equation to mean anything
#define EXPECT_IRI_ROUND_TRIP(base_string, target_string)                      \
  {                                                                            \
    const auto base{sourcemeta::core::URI::from_iri(base_string)};             \
    auto target{sourcemeta::core::URI::from_iri(target_string)};               \
    EXPECT_TRUE(base.is_absolute());                                           \
    EXPECT_TRUE(target.is_absolute());                                         \
    target.relative_to(base);                                                  \
    auto resolved{sourcemeta::core::URI::from_iri(target.recompose())};        \
    resolved.resolve_from(base);                                               \
    EXPECT_EQ(resolved.recompose(),                                            \
              sourcemeta::core::URI::from_iri(target_string).recompose());     \
  }

TEST(absolute_absolute_base_true_1) {
  EXPECT_ROUND_TRIP("https://www.example.com", "https://www.example.com/foo");
}

TEST(absolute_absolute_base_true_2) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo",
                    "https://www.example.com/foo");
}

TEST(absolute_absolute_base_true_3) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo",
                    "https://www.example.com/foo/bar?q=1");
}

TEST(absolute_absolute_base_true_4) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo",
                    "https://www.example.com/foo/bar#baz");
}

TEST(absolute_absolute_base_false_1) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo",
                    "http://www.example.com/foo");
}

TEST(absolute_absolute_base_false_2) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo", "https://www.example.com");
}

TEST(absolute_absolute_base_false_3) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/bar",
                    "https://www.example.com/foo");
}

TEST(absolute_absolute_base_false_4) {
  EXPECT_ROUND_TRIP("https://foo.com", "https://bar.com");
}

TEST(absolute_absolute_base_false_different_ports) {
  EXPECT_ROUND_TRIP("http://localhost:8000",
                    "http://localhost:9000/schemas/test.json");
}

TEST(absolute_absolute_base_false_different_userinfo) {
  EXPECT_ROUND_TRIP("https://alice@example.com/foo",
                    "https://bob@example.com/foo/bar");
}

TEST(absolute_absolute_base_false_userinfo_vs_none) {
  EXPECT_ROUND_TRIP("https://example.com/foo",
                    "https://alice@example.com/foo/bar");
}

TEST(urn_1) { EXPECT_ROUND_TRIP("schema:", "schema:myschema"); }

TEST(absolute_absolute_trailing_slash) {
  EXPECT_ROUND_TRIP("https://github.com/apis-json/api-json/blob/develop/spec",
                    "https://github.com/apis-json/api-json/blob/develop/spec/");
}

TEST(target_is_prefix_of_base_parent) {
  EXPECT_ROUND_TRIP("https://example.com/foo/bar/baz",
                    "https://example.com/foo");
}

TEST(target_is_one_level_up_at_root) {
  EXPECT_ROUND_TRIP("https://example.com/foo/bar", "https://example.com/foo");
}

TEST(target_is_root) {
  EXPECT_ROUND_TRIP("https://example.com/foo/bar", "https://example.com/");
}

TEST(base_ends_with_slash) {
  EXPECT_ROUND_TRIP("https://example.com/foo/", "https://example.com/foo/bar");
}

TEST(base_root_only) {
  EXPECT_ROUND_TRIP("https://example.com/", "https://example.com/foo");
}

TEST(sibling_paths_same_directory) {
  EXPECT_ROUND_TRIP("https://example.com/schemas/bar.json",
                    "https://example.com/schemas/foo.json");
}

TEST(double_slash_with_trailing_slash) {
  EXPECT_ROUND_TRIP("https://example.com/slash/",
                    "https://example.com/slash/file.json");
}

TEST(different_directories_same_host_needs_dotdot) {
  EXPECT_ROUND_TRIP("https://example.com/schemas/with-rebase-same-host.json",
                    "https://example.com/bundling/single");
}

TEST(different_directories_same_host_needs_dotdot_2) {
  EXPECT_ROUND_TRIP("https://example.com/foo/bar/baz.json",
                    "https://example.com/qux/test.json");
}

TEST(different_directories_same_host_needs_dotdot_3) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c.json",
                    "https://example.com/d.json");
}

TEST(file_same_directory) {
  EXPECT_ROUND_TRIP("file:///home/user/schemas/base.json",
                    "file:///home/user/schemas/other.json");
}

TEST(file_subdirectory) {
  EXPECT_ROUND_TRIP("file:///home/user/schemas",
                    "file:///home/user/schemas/sub/test.json");
}

TEST(file_parent_directory) {
  EXPECT_ROUND_TRIP("file:///home/user/schemas/sub/base.json",
                    "file:///home/user/schemas/other.json");
}

TEST(file_different_root) {
  EXPECT_ROUND_TRIP("file:///home/user/schemas/base.json",
                    "file:///var/data/test.json");
}

TEST(file_same_file) {
  EXPECT_ROUND_TRIP("file:///home/user/schemas/base.json",
                    "file:///home/user/schemas/base.json");
}

TEST(file_with_fragment) {
  EXPECT_ROUND_TRIP("file:///home/user/schemas/base.json",
                    "file:///home/user/schemas/other.json#/defs/foo");
}

TEST(file_windows_same_directory) {
  EXPECT_ROUND_TRIP("file:///C:/Users/user/schemas/base.json",
                    "file:///C:/Users/user/schemas/other.json");
}

TEST(file_windows_subdirectory) {
  EXPECT_ROUND_TRIP("file:///C:/Users/user/schemas",
                    "file:///C:/Users/user/schemas/sub/test.json");
}

TEST(file_windows_parent_directory) {
  EXPECT_ROUND_TRIP("file:///C:/Users/user/schemas/sub/base.json",
                    "file:///C:/Users/user/schemas/other.json");
}

TEST(file_windows_different_drive) {
  EXPECT_ROUND_TRIP("file:///C:/Users/user/schemas/base.json",
                    "file:///D:/Data/test.json");
}

TEST(file_windows_same_file) {
  EXPECT_ROUND_TRIP("file:///C:/Users/user/schemas/base.json",
                    "file:///C:/Users/user/schemas/base.json");
}

TEST(file_windows_with_fragment) {
  EXPECT_ROUND_TRIP("file:///C:/Users/user/schemas/base.json",
                    "file:///C:/Users/user/schemas/other.json#/defs/foo");
}

TEST(same_path_only_query_differs) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/bar",
                    "https://www.example.com/foo/bar?q=1");
}

TEST(same_path_only_fragment_differs) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/bar",
                    "https://www.example.com/foo/bar#baz");
}

TEST(same_path_query_and_fragment_differ) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/bar",
                    "https://www.example.com/foo/bar?q=1#baz");
}

TEST(same_path_base_query_target_none) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/bar?q=1",
                    "https://www.example.com/foo/bar");
}

TEST(same_path_base_query_target_fragment_only) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/bar?q=1",
                    "https://www.example.com/foo/bar#baz");
}

TEST(same_path_base_query_target_other_query) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/bar?q=1",
                    "https://www.example.com/foo/bar?q=2");
}

TEST(same_directory_path_base_query_target_none) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/?q=1",
                    "https://www.example.com/foo/");
}

TEST(same_directory_path_base_query_target_fragment) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/?q=1",
                    "https://www.example.com/foo/#baz");
}

TEST(same_directory_path_target_query) {
  EXPECT_ROUND_TRIP("https://www.example.com/foo/?q=1",
                    "https://www.example.com/foo/?q=2");
}

TEST(authority_no_path_only_fragment_differs) {
  EXPECT_ROUND_TRIP("https://www.example.com", "https://www.example.com#baz");
}

TEST(authority_no_path_only_query_differs) {
  EXPECT_ROUND_TRIP("https://www.example.com?q=1",
                    "https://www.example.com?q=2");
}

TEST(authority_no_path_base_query_target_none) {
  EXPECT_ROUND_TRIP("https://www.example.com?q=1", "https://www.example.com");
}

TEST(relative_to_same_path_with_query_in_base) {
  EXPECT_ROUND_TRIP("schema:foo", "schema:foo?bar=1");
}

TEST(relative_to_slashless_base_path) {
  EXPECT_ROUND_TRIP("schema:foo", "schema:bar");
}

TEST(target_is_directory_of_base) {
  EXPECT_ROUND_TRIP("https://example.com/test/foo.json",
                    "https://example.com/test/");
}

TEST(target_is_nested_directory_of_base) {
  EXPECT_ROUND_TRIP("https://example.com/foo/bar/baz.json",
                    "https://example.com/foo/bar/");
}

TEST(target_is_directory_of_base_with_query) {
  EXPECT_ROUND_TRIP("https://example.com/test/foo.json",
                    "https://example.com/test/?q=1");
}

TEST(target_is_directory_of_base_with_fragment) {
  EXPECT_ROUND_TRIP("https://example.com/test/foo.json",
                    "https://example.com/test/#baz");
}

TEST(file_target_is_directory_of_base) {
  EXPECT_ROUND_TRIP("file:///home/user/schemas/base.json",
                    "file:///home/user/schemas/");
}

TEST(target_is_root_of_base_without_path) {
  EXPECT_ROUND_TRIP("https://example.com", "https://example.com/");
}

TEST(target_with_empty_first_segment_of_base_without_path) {
  EXPECT_ROUND_TRIP("https://example.com", "https://example.com//foo");
}

TEST(target_with_empty_first_segment_of_root_base) {
  EXPECT_ROUND_TRIP("https://example.com/foo", "https://example.com//bar");
}

TEST(target_with_empty_segment_below_base) {
  EXPECT_ROUND_TRIP("https://example.com/foo", "https://example.com/foo//bar");
}

TEST(target_with_empty_first_segment_from_nested_base) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com//bar");
}

TEST(target_with_empty_segment_below_directory_base) {
  EXPECT_ROUND_TRIP("https://example.com/foo/", "https://example.com/foo//bar");
}

TEST(target_with_empty_segment_below_nested_directory_base) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c.json",
                    "https://example.com/a/b//d");
}

TEST(target_is_absolute_path_of_base_without_authority) {
  EXPECT_ROUND_TRIP("schema:", "schema:/foo");
}

TEST(sibling_with_colon_in_first_segment) {
  EXPECT_ROUND_TRIP("file:///C:/Users/user/schemas/base.json",
                    "file:///C:/Users/user/schemas/D:foo.json");
}

TEST(descendant_of_base_names_the_base_segment) {
  EXPECT_ROUND_TRIP("https://example.com/foo", "https://example.com/foo/bar");
}

TEST(base_with_fragment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b#frag", "https://example.com/a/c");
}

TEST(base_with_query) {
  EXPECT_ROUND_TRIP("https://example.com/a/b?q=1", "https://example.com/a/c");
}

TEST(userinfo) {
  EXPECT_ROUND_TRIP("https://user:pass@example.com/a/b",
                    "https://user:pass@example.com/a/c");
}

TEST(non_default_port) {
  EXPECT_ROUND_TRIP("https://example.com:8443/a/b",
                    "https://example.com:8443/a/c");
}

TEST(percent_encoded_space) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/c%20d");
}

TEST(percent_encoded_unreserved) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/c%41d");
}

TEST(percent_encoded_slash) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/c%2Fd");
}

TEST(literal_dotdot_prefix_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/..foo");
}

TEST(literal_dotdot_suffix_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/foo..");
}

TEST(literal_dot_prefix_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/.foo");
}

TEST(target_without_path) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com");
}

TEST(identical_without_path) {
  EXPECT_ROUND_TRIP("https://example.com", "https://example.com");
}

TEST(foreign_authority) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://other.com/x");
}

TEST(slashless_base) { EXPECT_ROUND_TRIP("schema:foo", "schema:bar"); }

TEST(deep_target_from_root_base) {
  EXPECT_ROUND_TRIP("https://example.com/", "https://example.com/a/b/c");
}

TEST(parent_of_base_directory) {
  EXPECT_ROUND_TRIP("https://example.com/a/", "https://example.com/");
}

TEST(query_and_fragment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/b?q=1#f");
}

TEST(windows_drive_sibling) {
  EXPECT_ROUND_TRIP("file:///C:/x/y.json", "file:///C:/x/D:foo.json");
}

TEST(target_with_trailing_slash_at_depth) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c", "https://example.com/a/b/d/");
}

TEST(target_is_base_with_trailing_slash) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/b/");
}

TEST(deep_descendant) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/b/c/d");
}

TEST(descendant_with_query_and_fragment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b",
                    "https://example.com/a/b/c?q=1#f");
}

TEST(base_last_segment_has_literal_dots) {
  EXPECT_ROUND_TRIP("https://example.com/a/b..", "https://example.com/a/b../c");
}

TEST(windows_descendant) {
  EXPECT_ROUND_TRIP("file:///C:/x", "file:///C:/x/y.json");
}

TEST(colon_in_base_last_segment_descendant) {
  EXPECT_ROUND_TRIP("https://example.com/D:foo",
                    "https://example.com/D:foo/bar");
}

TEST(descendant_with_empty_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/b//c");
}

TEST(descendant_of_single_segment_base) {
  EXPECT_ROUND_TRIP("https://example.com/a", "https://example.com/a/b");
}

TEST(iri_preserves_ucschar) {
  EXPECT_IRI_ROUND_TRIP("https://example.com/dir/",
                        "https://example.com/dir/caf\xC3\xA9");
}

TEST(slashless_base_with_descendant_target) {
  EXPECT_ROUND_TRIP("schema:foo", "schema:foo/bar");
}

TEST(ipv6_host) { EXPECT_ROUND_TRIP("https://[::1]/a/b", "https://[::1]/a/c"); }

TEST(parent_of_nested_directory_base) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/", "https://example.com/a/");
}

TEST(ancestor_two_levels_up) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c", "https://example.com/a");
}

TEST(directory_base_with_target_of_the_same_name) {
  EXPECT_ROUND_TRIP("https://example.com/a/", "https://example.com/a");
}

TEST(file_base_with_directory_target_of_the_same_name) {
  EXPECT_ROUND_TRIP("https://example.com/a", "https://example.com/a/");
}

TEST(windows_drive_root_from_nested_base) {
  EXPECT_ROUND_TRIP("file:///C:/x/y/", "file:///C:/");
}

TEST(identical_without_path_but_with_query) {
  EXPECT_ROUND_TRIP("https://example.com?q=1", "https://example.com?q=1");
}

TEST(ancestor_three_levels_up) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c/d", "https://example.com/a");
}

TEST(mailto_slashless_base) { EXPECT_ROUND_TRIP("mailto:foo", "mailto:bar"); }

TEST(target_with_empty_fragment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/b#");
}

TEST(target_with_empty_query) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/b?");
}

TEST(sibling_carrying_the_same_query) {
  EXPECT_ROUND_TRIP("https://example.com/a/b?x=1",
                    "https://example.com/a/c?x=1");
}

TEST(root_base_with_target_without_path) {
  EXPECT_ROUND_TRIP("https://example.com/", "https://example.com");
}

TEST(descendant_of_a_base_with_an_empty_first_segment) {
  EXPECT_ROUND_TRIP("https://example.com//a", "https://example.com//a/b");
}

TEST(target_gains_an_empty_first_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a", "https://example.com//a");
}

TEST(base_with_dot_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a/./b", "https://example.com/a/c");
}

TEST(base_with_dotdot_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a/../b", "https://example.com/a/c");
}

TEST(uppercase_scheme_in_base) {
  EXPECT_ROUND_TRIP("HTTPS://example.com/a/b", "https://example.com/a/c");
}

TEST(uppercase_host_in_base) {
  EXPECT_ROUND_TRIP("https://EXAMPLE.COM/a/b", "https://example.com/a/c");
}

TEST(explicit_default_port) {
  EXPECT_ROUND_TRIP("https://example.com:443/a/b",
                    "https://example.com:443/a/c");
}

TEST(base_with_port_and_target_without) {
  EXPECT_ROUND_TRIP("https://example.com:443/a/b", "https://example.com/a/c");
}

TEST(deep_ancestor_traversal) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c/d/e/f", "https://example.com/x");
}

TEST(repeated_empty_segments) {
  EXPECT_ROUND_TRIP("https://example.com/a//b//c",
                    "https://example.com/a//b//d");
}

TEST(file_with_localhost_authority) {
  EXPECT_ROUND_TRIP("file://localhost/a/b", "file://localhost/a/c");
}

TEST(file_with_empty_authority) {
  EXPECT_ROUND_TRIP("file:///a/b", "file:///a/c");
}

TEST(scheme_with_plus_hyphen_and_dot) {
  EXPECT_ROUND_TRIP("a+b-c.d:/x/y", "a+b-c.d:/x/z");
}

TEST(query_containing_slashes) {
  EXPECT_ROUND_TRIP("https://example.com/a/b",
                    "https://example.com/a/c?x=/y/z");
}

TEST(fragment_containing_slashes) {
  EXPECT_ROUND_TRIP("https://example.com/a/b",
                    "https://example.com/a/c#/defs/x");
}

TEST(fragment_containing_question_mark) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/c#x?y");
}

TEST(userinfo_with_colon) {
  EXPECT_ROUND_TRIP("https://u:p@example.com/a/b",
                    "https://u:p@example.com/a/b/c");
}

TEST(percent_encoded_userinfo) {
  EXPECT_ROUND_TRIP("https://u%40x@example.com/a/b",
                    "https://u%40x@example.com/a/c");
}

TEST(path_with_semicolon_parameters) {
  EXPECT_ROUND_TRIP("https://example.com/a/b;x=1",
                    "https://example.com/a/c;y=2");
}

TEST(path_segment_with_at_sign) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com/a/@c");
}

TEST(deeply_nested_descendant) {
  EXPECT_ROUND_TRIP("https://example.com/a",
                    "https://example.com/a/b/c/d/e/f/g");
}

TEST(shared_prefix_without_a_segment_boundary) {
  EXPECT_ROUND_TRIP("https://example.com/foo",
                    "https://example.com/foobar/baz");
}

TEST(target_of_only_slashes) {
  EXPECT_ROUND_TRIP("https://example.com/a", "https://example.com///");
}

TEST(base_of_only_slashes) {
  EXPECT_ROUND_TRIP("https://example.com///", "https://example.com/a");
}

TEST(root_base_with_empty_first_segment_target) {
  EXPECT_ROUND_TRIP("https://example.com/", "https://example.com//x");
}

TEST(urn_siblings) { EXPECT_ROUND_TRIP("urn:example:a", "urn:example:b"); }

TEST(tag_uri_siblings) {
  EXPECT_ROUND_TRIP("tag:example.com,2024:a", "tag:example.com,2024:b");
}

TEST(long_ancestor_chain) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c/d/e/f/g/h/i/j/k",
                    "https://example.com/z");
}

TEST(long_descendant_chain) {
  EXPECT_ROUND_TRIP("https://example.com/a",
                    "https://example.com/a/b/c/d/e/f/g/h/i/j/k");
}

TEST(base_with_dotdot_as_its_last_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/..", "https://example.com/a/c");
}

TEST(base_with_dot_as_its_last_segment) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/.", "https://example.com/a/b/c");
}

TEST(base_with_dotdot_beyond_the_root) {
  EXPECT_ROUND_TRIP("https://example.com/../../a", "https://example.com/b");
}

TEST(target_is_an_empty_segment_at_the_root) {
  EXPECT_ROUND_TRIP("https://example.com/a/b", "https://example.com//");
}

TEST(target_is_the_base_parent_exactly) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c", "https://example.com/a/b");
}

TEST(single_character_segments) {
  EXPECT_ROUND_TRIP("https://example.com/a/b/c", "https://example.com/a/b/d");
}

TEST(iri_base_and_target) {
  EXPECT_IRI_ROUND_TRIP("https://example.com/caf\xC3\xA9/a",
                        "https://example.com/caf\xC3\xA9/b");
}

TEST(iri_query) {
  EXPECT_IRI_ROUND_TRIP("https://example.com/a/b",
                        "https://example.com/a/c?q=caf\xC3\xA9");
}

TEST(iri_fragment) {
  EXPECT_IRI_ROUND_TRIP("https://example.com/a/b",
                        "https://example.com/a/c#caf\xC3\xA9");
}

TEST(iri_descendant) {
  EXPECT_IRI_ROUND_TRIP("https://example.com/caf\xC3\xA9",
                        "https://example.com/caf\xC3\xA9/x");
}

TEST(iri_ancestor) {
  EXPECT_IRI_ROUND_TRIP("https://example.com/caf\xC3\xA9/x/y",
                        "https://example.com/caf\xC3\xA9");
}

TEST(iri_host) {
  EXPECT_IRI_ROUND_TRIP("https://caf\xC3\xA9.example/a/b",
                        "https://caf\xC3\xA9.example/a/c");
}

TEST(identical_uris_with_a_fragment) {
  EXPECT_ROUND_TRIP("https://example.com/foo#bar",
                    "https://example.com/foo#bar");
}

TEST(identical_uris_with_a_query_and_fragment) {
  EXPECT_ROUND_TRIP("https://example.com/foo?q=1#bar",
                    "https://example.com/foo?q=1#bar");
}

TEST(identical_authority_less_uris_with_a_fragment) {
  EXPECT_ROUND_TRIP("schema:foo#bar", "schema:foo#bar");
}

TEST(identical_uris_without_a_path_but_with_a_fragment) {
  EXPECT_ROUND_TRIP("https://example.com#bar", "https://example.com#bar");
}
