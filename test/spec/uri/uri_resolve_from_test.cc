#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

// Relative-to-relative resolution is no part of RFC 3986, whose Section 5.1
// states that "Relative resolution requires a base URI to be absolute". This
// module extends that to a relative base as a convenience, and every test of
// it is named with an extension prefix. Section 5.2.2 reads nothing of the
// base but its authority, path and query, so the algorithm runs over a
// relative base as written. The one departure is Section 5.2.4, which
// discards the leading block of the path it is handed, as a base that is
// absolute never merges to one that has a block to discard

TEST(extension_relative_base_merges_into_its_own_directory) {
  const sourcemeta::core::URI base{"../foo"};
  sourcemeta::core::URI relative{"../baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "../../baz");
}

TEST(extension_fragment_on_relative_path) {
  const sourcemeta::core::URI base{"foo"};
  sourcemeta::core::URI relative{"#/bar"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "foo#/bar");
}

TEST(extension_base_relative_path_leading_slash) {
  const sourcemeta::core::URI base{"/foo"};
  sourcemeta::core::URI relative{"#/bar"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "/foo#/bar");
}

TEST(extension_relative_path_from_relative_path) {
  const sourcemeta::core::URI base{"foo/bar/baz"};
  sourcemeta::core::URI relative{"qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "foo/bar/qux");
}

TEST(rfc3986_normal_absolute_uri_with_scheme) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g:h"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "g:h");
}

TEST(rfc3986_normal_relative_path_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g");
}

TEST(rfc3986_normal_relative_path_dot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"./g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g");
}

TEST(rfc3986_normal_relative_path_g_slash) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g/"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g/");
}

TEST(rfc3986_normal_absolute_path) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"/g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(rfc3986_normal_network_path) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"//g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://g");
}

TEST(rfc3986_normal_query_only) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"?y"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/d;p?y");
}

TEST(rfc3986_normal_relative_with_query) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g?y"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g?y");
}

TEST(rfc3986_normal_fragment_only) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"#s"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/d;p?q#s");
}

TEST(rfc3986_normal_relative_with_fragment) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g#s"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g#s");
}

TEST(rfc3986_normal_relative_with_query_and_fragment) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g?y#s"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g?y#s");
}

TEST(rfc3986_normal_path_parameter) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{";x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/;x");
}

TEST(rfc3986_normal_relative_with_parameter) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g;x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g;x");
}

TEST(rfc3986_normal_relative_with_param_query_fragment) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g;x?y#s"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g;x?y#s");
}

TEST(rfc3986_normal_empty_reference) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{""};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/d;p?q");
}

TEST(rfc3986_normal_dot_only) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/");
}

TEST(rfc3986_normal_dot_slash) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"./"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/");
}

TEST(rfc3986_normal_dotdot_only) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{".."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/");
}

TEST(rfc3986_normal_dotdot_slash) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/");
}

TEST(rfc3986_normal_dotdot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/g");
}

TEST(rfc3986_normal_dotdot_dotdot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../.."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/");
}

TEST(rfc3986_normal_dotdot_dotdot_slash) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../../"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/");
}

TEST(rfc3986_normal_dotdot_dotdot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(rfc3986_abnormal_excessive_dotdot_1) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../../../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(rfc3986_abnormal_excessive_dotdot_2) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../../../../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(rfc3986_abnormal_absolute_with_dot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"/./g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(rfc3986_abnormal_absolute_with_dotdot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"/../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(rfc3986_abnormal_g_dot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g.");
}

TEST(rfc3986_abnormal_dot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{".g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/.g");
}

TEST(rfc3986_abnormal_g_dotdot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g.."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g..");
}

TEST(rfc3986_abnormal_dotdot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"..g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/..g");
}

TEST(rfc3986_abnormal_dot_dotdot_dot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"./../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/g");
}

TEST(rfc3986_abnormal_dot_slash_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"./g/."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g/");
}

TEST(rfc3986_abnormal_g_slash_dot_h) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g/./h"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g/h");
}

TEST(rfc3986_abnormal_g_dotdot_h) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g/../h"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/h");
}

TEST(rfc3986_abnormal_g_semicolon_x_equal_1_dotdot_y) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g;x=1/./y"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g;x=1/y");
}

TEST(rfc3986_abnormal_g_semicolon_x_equal_1_dotdot_dotdot_y) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g;x=1/../y"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/y");
}

TEST(rfc3986_abnormal_query_dot_slash_x) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g?y/./x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g?y/./x");
}

TEST(rfc3986_abnormal_query_dotdot_x) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g?y/../x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g?y/../x");
}

TEST(rfc3986_abnormal_fragment_dot_slash_x) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g#s/./x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g#s/./x");
}

TEST(rfc3986_abnormal_fragment_dotdot_x) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g#s/../x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g#s/../x");
}

TEST(rfc3986_abnormal_http_colon_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"http:g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http:g");
}

TEST(absolute_relative_with_slash) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"/baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/baz");
}

TEST(example_1) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"../baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/baz");
}

TEST(add_fragment) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"#baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/foo/bar#baz");
}

TEST(change_fragment) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar#baz"};
  sourcemeta::core::URI relative{"#qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/foo/bar#qux");
}

TEST(rfc3986_resolve_with_relative_path) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI relative_path{"../../b"};
  relative_path.resolve_from(base);
  EXPECT_EQ(relative_path.recompose(), "s://h/b");
}

TEST(rfc3986_resolve_with_empty) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI empty{""};
  empty.resolve_from(base);
  EXPECT_EQ(empty.recompose(), "s://h/a/c");
}

TEST(rfc3986_resolve_with_query) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI query{"?x=y"};
  query.resolve_from(base);
  EXPECT_EQ(query.recompose(), "s://h/a/c?x=y");
}

TEST(rfc3986_resolve_with_fragment) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI fragment{"#x=y"};
  fragment.resolve_from(base);
  EXPECT_EQ(fragment.recompose(), "s://h/a/c#x=y");
}

TEST(rfc3986_resolve_with_absolute_path) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI absolute_path{"/././x"};
  absolute_path.resolve_from(base);
  EXPECT_EQ(absolute_path.recompose(), "s://h/x");
}

TEST(file_1) {
  const sourcemeta::core::URI base{"file:///foo/bar/baz"};
  sourcemeta::core::URI relative{"./qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "file:///foo/bar/qux");
}

TEST(file_from_path) {
  const std::filesystem::path base_path{"/foo/bar/baz"};
  const auto base{sourcemeta::core::URI::from_path(base_path)};
  sourcemeta::core::URI relative{"./qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "file:///foo/bar/qux");
}

TEST(network_path_with_port) {
  const sourcemeta::core::URI base{"http://example.com/path"};
  sourcemeta::core::URI reference{"//other.com:8080/newpath"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://other.com:8080/newpath");
}

TEST(base_with_no_path) {
  const sourcemeta::core::URI base{"http://example.com"};
  sourcemeta::core::URI reference{"path"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://example.com/path");
}

TEST(base_with_trailing_slash) {
  const sourcemeta::core::URI base{"http://example.com/dir/"};
  sourcemeta::core::URI reference{"file"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://example.com/dir/file");
}

TEST(query_replaces_base_query) {
  const sourcemeta::core::URI base{"http://example.com/path?oldquery"};
  sourcemeta::core::URI reference{"?newquery"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://example.com/path?newquery");
}

TEST(iri_relative_reference_with_ucschar) {
  const auto base{sourcemeta::core::URI::from_iri("https://example.com/dir/")};
  auto relative{sourcemeta::core::URI::from_iri("caf\xC3\xA9")};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://example.com/dir/caf\xC3\xA9");
}

TEST(iri_preserves_base_ucschar) {
  const auto base{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9/page")};
  auto relative{sourcemeta::core::URI::from_iri("other")};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://example.com/caf\xC3\xA9/other");
}

TEST(iri_flag_propagates_from_base) {
  const auto base{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9/page")};
  sourcemeta::core::URI relative{"other"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://example.com/caf\xC3\xA9/other");
  EXPECT_TRUE(relative.is_internationalized());
}

TEST(iri_rfc3986_normal_example_path) {
  // RFC 3986 Section 5.4.1 "Normal Examples", confirmed to resolve identically
  // when base and reference are parsed as IRIs.
  // https://www.rfc-editor.org/rfc/rfc3986#section-5.4.1
  const auto base{sourcemeta::core::URI::from_iri("http://a/b/c/d;p?q")};
  auto reference{sourcemeta::core::URI::from_iri("g")};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g");
}

TEST(iri_rfc3986_normal_example_query) {
  // RFC 3986 Section 5.4.1 "Normal Examples", confirmed to resolve identically
  // when base and reference are parsed as IRIs.
  // https://www.rfc-editor.org/rfc/rfc3986#section-5.4.1
  const auto base{sourcemeta::core::URI::from_iri("http://a/b/c/d;p?q")};
  auto reference{sourcemeta::core::URI::from_iri("?y")};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/d;p?y");
}

TEST(iri_reference_against_plain_uri_base) {
  // An IRI reference resolved against a plain URI base keeps its non-ASCII
  // characters literal
  const sourcemeta::core::URI base{"https://example.com/dir/"};
  auto reference{sourcemeta::core::URI::from_iri("caf\xC3\xA9")};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "https://example.com/dir/caf\xC3\xA9");
  EXPECT_TRUE(reference.is_internationalized());
}

TEST(ipv6_base_authority_is_inherited) {
  const sourcemeta::core::URI base{"https://[::1]/a/b"};
  sourcemeta::core::URI reference{"c"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "https://[::1]/a/c");
}

TEST(ipv6_base_authority_is_inherited_by_a_fragment) {
  const sourcemeta::core::URI base{"https://[2001:db8::1]:8443/a"};
  sourcemeta::core::URI reference{"#frag"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "https://[2001:db8::1]:8443/a#frag");
}

// An empty path segment carries meaning and RFC 3986 Section 5.2.4 never
// removes one, so these survive resolution. Some widely used implementations
// collapse them, which is a deviation rather than a licence to follow
TEST(empty_segment_in_the_reference_is_preserved) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g//h"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g//h");
}

TEST(dot_segment_before_an_empty_segment_is_preserved) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{".//g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c//g");
}

TEST(empty_segment_in_the_base_is_preserved) {
  const sourcemeta::core::URI base{"http://a//b/c"};
  sourcemeta::core::URI reference{"g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a//b/g");
}

TEST(empty_segment_in_the_base_survives_a_parent_step) {
  const sourcemeta::core::URI base{"http://a//b/c"};
  sourcemeta::core::URI reference{".."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a//");
}

TEST(network_path_reference_with_an_empty_authority) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"//"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://");
}

TEST(a_dot_reference_resolves_to_the_base_directory) {
  const sourcemeta::core::URI base{"https://example.com/a/b"};
  sourcemeta::core::URI reference{"./c"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "https://example.com/a/c");
}

TEST(a_dotdot_reference_steps_out_of_the_base_directory) {
  const sourcemeta::core::URI base{"https://example.com/a/b"};
  sourcemeta::core::URI reference{"../c"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "https://example.com/c");
}

// RFC 3986 Section 5.2.2 takes the base path for a reference that carries
// none. Section 5.2.1 asks the base to be absolute, so resolving against a
// relative one is this implementation going further than the RFC
TEST(fragment_only_against_a_relative_base) {
  sourcemeta::core::URI uri{"#frag"};
  uri.resolve_from(sourcemeta::core::URI{"foo/bar"});
  EXPECT_EQ(uri.recompose(), "foo/bar#frag");
}

// A reference whose path is already rooted takes no part of the base path
TEST(absolute_path_against_a_relative_base) {
  sourcemeta::core::URI uri{"/abs"};
  uri.resolve_from(sourcemeta::core::URI{"foo/bar"});
  EXPECT_EQ(uri.recompose(), "/abs");
}

TEST(query_only_against_an_absolute_base) {
  sourcemeta::core::URI uri{"?q=1"};
  uri.resolve_from(sourcemeta::core::URI{"http://example.com/a"});
  EXPECT_EQ(uri.recompose(), "http://example.com/a?q=1");
}

// Section 5.2.3 excludes the whole base path where it carries no "/", so what
// gets merged here comes out relative, and Section 5.2.4 still applies to it
// "whether or not the path was relative"
TEST(parent_segment_against_a_base_without_a_path_separator) {
  sourcemeta::core::URI uri{"../target"};
  uri.resolve_from(sourcemeta::core::URI{"urn:example:entry"});
  EXPECT_EQ(uri.recompose(), "urn:target");
}

TEST(lone_parent_segment_against_a_base_without_a_path_separator) {
  sourcemeta::core::URI uri{".."};
  uri.resolve_from(sourcemeta::core::URI{"urn:example:entry"});
  EXPECT_EQ(uri.recompose(), "urn:");
}

TEST(parent_segment_against_a_mailto_base) {
  sourcemeta::core::URI uri{"../target"};
  uri.resolve_from(sourcemeta::core::URI{"mailto:user@example.com"});
  EXPECT_EQ(uri.recompose(), "mailto:target");
}

TEST(current_segment_against_a_base_without_a_path_separator) {
  sourcemeta::core::URI uri{"./target"};
  uri.resolve_from(sourcemeta::core::URI{"urn:example:entry"});
  EXPECT_EQ(uri.recompose(), "urn:target");
}

// What a relative base still needs is the block that RFC resolution would
// drop, so the extension keeps it where the specification algorithm does not
TEST(extension_relative_base_keeps_what_merging_produced) {
  sourcemeta::core::URI uri{"bar"};
  uri.resolve_from(sourcemeta::core::URI{"../foo"});
  EXPECT_EQ(uri.recompose(), "../bar");
}

// Section 5.2.4 step C replaces the prefix with "/" and takes a segment off
// the output, so the slash it leaves behind is moved into the result by step
// E. Step A drops a leading block without touching the output, which is why
// the two do not agree on a leading slash
TEST(interior_parent_segment_against_a_base_without_a_path_separator) {
  sourcemeta::core::URI uri{"../a/../target"};
  uri.resolve_from(sourcemeta::core::URI{"urn:example:entry"});
  EXPECT_EQ(uri.recompose(), "urn:/target");
}

TEST(interior_parent_segment_exhausting_the_output) {
  sourcemeta::core::URI uri{"../a/.."};
  uri.resolve_from(sourcemeta::core::URI{"urn:example:entry"});
  EXPECT_EQ(uri.recompose(), "urn:/");
}

// RFC 3986 Section 5.2.2 takes the authority from the reference whenever the
// reference states one, and Section 3.2 lets a userinfo state one on its own
TEST(reference_naming_only_a_userinfo_keeps_it) {
  sourcemeta::core::URI uri;
  uri.userinfo("alice");
  uri.resolve_from(sourcemeta::core::URI{"https://example.com/a/b"});
  EXPECT_EQ(uri.recompose(), "https://alice@");
}

// A path that holds nothing once the dot segments go is no path at all, which
// is the spelling the rest of this class answers with
TEST(reference_with_a_scheme_and_a_lone_dot_path) {
  sourcemeta::core::URI uri{"foo:."};
  uri.resolve_from(sourcemeta::core::URI{"https://example.com/a/b"});
  EXPECT_FALSE(uri.path().has_value());
  EXPECT_EQ(uri.recompose(), "foo:");
}

TEST(a_lone_dot_against_a_relative_base_without_a_path) {
  sourcemeta::core::URI uri{"."};
  uri.resolve_from(sourcemeta::core::URI{""});
  EXPECT_FALSE(uri.path().has_value());
  EXPECT_EQ(uri.recompose(), "");
}

// RFC 3986 Section 5.2.3 merges against the base path with everything after
// its right-most slash excluded, which a rooted base path answers for just as
// a rootless one does
TEST(relative_base_with_an_absolute_path_merges) {
  sourcemeta::core::URI uri{"c"};
  uri.resolve_from(sourcemeta::core::URI{"/a/b"});
  EXPECT_EQ(uri.recompose(), "/a/c");
}

TEST(relative_base_with_an_absolute_path_merges_a_reference_with_segments) {
  sourcemeta::core::URI uri{"c/d"};
  uri.resolve_from(sourcemeta::core::URI{"/a/b"});
  EXPECT_EQ(uri.recompose(), "/a/c/d");
}

TEST(rootless_relative_base_merges_a_reference_with_segments) {
  sourcemeta::core::URI uri{"c/d"};
  uri.resolve_from(sourcemeta::core::URI{"a/b"});
  EXPECT_EQ(uri.recompose(), "a/c/d");
}

// RFC 3986 Section 5.2.2: "if defined(R.query) then T.query = R.query" and
// "T.path = Base.path" where the reference path is empty, so a reference that
// states nothing but a query names the base document
TEST(reference_without_a_path_against_a_relative_base) {
  sourcemeta::core::URI uri{"?q"};
  uri.resolve_from(sourcemeta::core::URI{"a/b"});
  EXPECT_EQ(uri.recompose(), "a/b?q");
}

TEST(reference_without_a_path_takes_the_query_of_a_relative_base) {
  sourcemeta::core::URI uri{"#f"};
  uri.resolve_from(sourcemeta::core::URI{"a/b?q"});
  EXPECT_EQ(uri.recompose(), "a/b?q#f");
}

// RFC 3986 Section 5.2.2 reads the base no further than its authority once the
// reference path begins with a slash, so a relative base leaves the reference
// to answer for itself with its dot segments gone
TEST(reference_with_a_rooted_path_against_a_relative_base) {
  sourcemeta::core::URI uri{"/c/./d/../e"};
  uri.resolve_from(sourcemeta::core::URI{"a/b"});
  EXPECT_EQ(uri.recompose(), "/c/e");
}

// RFC 3986 Section 5.2.2: "if defined(R.authority) then T.authority =
// R.authority", which says nothing of the base at all
TEST(reference_with_an_authority_against_a_relative_base) {
  sourcemeta::core::URI uri{"//example.com/c/../d"};
  uri.resolve_from(sourcemeta::core::URI{"a/b"});
  EXPECT_EQ(uri.recompose(), "//example.com/d");
}

// RFC 3986 Section 5.2.3 supplies the separator from a base that has a defined
// authority and no path, and Section 3.2 lets a userinfo define one, so
// Section 3.3 still requires the merged path to begin with a slash
TEST(base_naming_only_a_userinfo_supplies_the_separator) {
  sourcemeta::core::URI base{"https:"};
  base.userinfo("alice");
  sourcemeta::core::URI uri{"x"};
  uri.resolve_from(base);
  EXPECT_EQ(uri.recompose(), "https://alice@/x");
}

// RFC 3986 Section 5.2.3 merges into the directory of the base, so a reference
// that runs above that directory names a level above the base's own leading
// block rather than cancelling it
TEST(reference_running_above_the_directory_of_a_relative_base) {
  sourcemeta::core::URI uri{"a/../../c"};
  uri.resolve_from(sourcemeta::core::URI{"../b"});
  EXPECT_EQ(uri.recompose(), "../../c");
}
