#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

// NON-STANDARD EXTENSION: Relative-to-Relative Resolution
// The following tests exercise relative-to-relative URI resolution, which is
// NOT defined by RFC 3986. RFC 3986 Section 5.1 explicitly states:
// "Relative resolution requires a base URI to be absolute."
// This implementation extends RFC 3986 to support relative bases in specific
// cases as a convenience feature.

TEST(URI_resolve_from, extension_relative_base_unchanged) {
  const sourcemeta::core::URI base{"../foo"};
  sourcemeta::core::URI relative{"../baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "../baz");
}

TEST(URI_resolve_from, extension_fragment_on_relative_path) {
  const sourcemeta::core::URI base{"foo"};
  sourcemeta::core::URI relative{"#/bar"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "foo#/bar");
}

TEST(URI_resolve_from, extension_base_relative_path_leading_slash) {
  const sourcemeta::core::URI base{"/foo"};
  sourcemeta::core::URI relative{"#/bar"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "/foo#/bar");
}

TEST(URI_resolve_from, extension_relative_path_from_relative_path) {
  const sourcemeta::core::URI base{"foo/bar/baz"};
  sourcemeta::core::URI relative{"qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "foo/bar/qux");
}

TEST(URI_resolve_from, rfc3986_normal_absolute_uri_with_scheme) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g:h"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "g:h");
}

TEST(URI_resolve_from, rfc3986_normal_relative_path_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g");
}

TEST(URI_resolve_from, rfc3986_normal_relative_path_dot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"./g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g");
}

TEST(URI_resolve_from, rfc3986_normal_relative_path_g_slash) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g/"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g/");
}

TEST(URI_resolve_from, rfc3986_normal_absolute_path) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"/g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(URI_resolve_from, rfc3986_normal_network_path) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"//g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://g");
}

TEST(URI_resolve_from, rfc3986_normal_query_only) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"?y"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/d;p?y");
}

TEST(URI_resolve_from, rfc3986_normal_relative_with_query) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g?y"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g?y");
}

TEST(URI_resolve_from, rfc3986_normal_fragment_only) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"#s"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/d;p?q#s");
}

TEST(URI_resolve_from, rfc3986_normal_relative_with_fragment) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g#s"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g#s");
}

TEST(URI_resolve_from, rfc3986_normal_relative_with_query_and_fragment) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g?y#s"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g?y#s");
}

TEST(URI_resolve_from, rfc3986_normal_path_parameter) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{";x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/;x");
}

TEST(URI_resolve_from, rfc3986_normal_relative_with_parameter) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g;x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g;x");
}

TEST(URI_resolve_from, rfc3986_normal_relative_with_param_query_fragment) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g;x?y#s"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g;x?y#s");
}

TEST(URI_resolve_from, rfc3986_normal_empty_reference) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{""};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/d;p?q");
}

TEST(URI_resolve_from, rfc3986_normal_dot_only) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/");
}

TEST(URI_resolve_from, rfc3986_normal_dot_slash) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"./"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/");
}

TEST(URI_resolve_from, rfc3986_normal_dotdot_only) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{".."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/");
}

TEST(URI_resolve_from, rfc3986_normal_dotdot_slash) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/");
}

TEST(URI_resolve_from, rfc3986_normal_dotdot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/g");
}

TEST(URI_resolve_from, rfc3986_normal_dotdot_dotdot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../.."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/");
}

TEST(URI_resolve_from, rfc3986_normal_dotdot_dotdot_slash) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../../"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/");
}

TEST(URI_resolve_from, rfc3986_normal_dotdot_dotdot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(URI_resolve_from, rfc3986_abnormal_excessive_dotdot_1) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../../../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(URI_resolve_from, rfc3986_abnormal_excessive_dotdot_2) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"../../../../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(URI_resolve_from, rfc3986_abnormal_absolute_with_dot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"/./g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(URI_resolve_from, rfc3986_abnormal_absolute_with_dotdot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"/../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/g");
}

TEST(URI_resolve_from, rfc3986_abnormal_g_dot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g.");
}

TEST(URI_resolve_from, rfc3986_abnormal_dot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{".g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/.g");
}

TEST(URI_resolve_from, rfc3986_abnormal_g_dotdot) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g.."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g..");
}

TEST(URI_resolve_from, rfc3986_abnormal_dotdot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"..g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/..g");
}

TEST(URI_resolve_from, rfc3986_abnormal_dot_dotdot_dot_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"./../g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/g");
}

TEST(URI_resolve_from, rfc3986_abnormal_dot_slash_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"./g/."};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g/");
}

TEST(URI_resolve_from, rfc3986_abnormal_g_slash_dot_h) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g/./h"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g/h");
}

TEST(URI_resolve_from, rfc3986_abnormal_g_dotdot_h) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g/../h"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/h");
}

TEST(URI_resolve_from, rfc3986_abnormal_g_semicolon_x_equal_1_dotdot_y) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g;x=1/./y"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g;x=1/y");
}

TEST(URI_resolve_from, rfc3986_abnormal_g_semicolon_x_equal_1_dotdot_dotdot_y) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g;x=1/../y"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/y");
}

TEST(URI_resolve_from, rfc3986_abnormal_query_dot_slash_x) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g?y/./x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g?y/./x");
}

TEST(URI_resolve_from, rfc3986_abnormal_query_dotdot_x) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g?y/../x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g?y/../x");
}

TEST(URI_resolve_from, rfc3986_abnormal_fragment_dot_slash_x) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g#s/./x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g#s/./x");
}

TEST(URI_resolve_from, rfc3986_abnormal_fragment_dotdot_x) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"g#s/../x"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://a/b/c/g#s/../x");
}

TEST(URI_resolve_from, rfc3986_abnormal_http_colon_g) {
  const sourcemeta::core::URI base{"http://a/b/c/d;p?q"};
  sourcemeta::core::URI reference{"http:g"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http:g");
}

TEST(URI_resolve_from, absolute_relative_with_slash) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"/baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/baz");
}

TEST(URI_resolve_from, example_1) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"../baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/baz");
}

TEST(URI_resolve_from, add_fragment) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"#baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/foo/bar#baz");
}

TEST(URI_resolve_from, change_fragment) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar#baz"};
  sourcemeta::core::URI relative{"#qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/foo/bar#qux");
}

TEST(URI_resolve_from, rfc3986_resolve_with_relative_path) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI relative_path{"../../b"};
  relative_path.resolve_from(base);
  EXPECT_EQ(relative_path.recompose(), "s://h/b");
}

TEST(URI_resolve_from, rfc3986_resolve_with_empty) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI empty{""};
  empty.resolve_from(base);
  EXPECT_EQ(empty.recompose(), "s://h/a/c");
}

TEST(URI_resolve_from, rfc3986_resolve_with_query) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI query{"?x=y"};
  query.resolve_from(base);
  EXPECT_EQ(query.recompose(), "s://h/a/c?x=y");
}

TEST(URI_resolve_from, rfc3986_resolve_with_fragment) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI fragment{"#x=y"};
  fragment.resolve_from(base);
  EXPECT_EQ(fragment.recompose(), "s://h/a/c#x=y");
}

TEST(URI_resolve_from, rfc3986_resolve_with_absolute_path) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI absolute_path{"/././x"};
  absolute_path.resolve_from(base);
  EXPECT_EQ(absolute_path.recompose(), "s://h/x");
}

TEST(URI_resolve_from, file_1) {
  const sourcemeta::core::URI base{"file:///foo/bar/baz"};
  sourcemeta::core::URI relative{"./qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "file:///foo/bar/qux");
}

TEST(URI_resolve_from, file_from_path) {
  const std::filesystem::path base_path{"/foo/bar/baz"};
  const auto base{sourcemeta::core::URI::from_path(base_path)};
  sourcemeta::core::URI relative{"./qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "file:///foo/bar/qux");
}

TEST(URI_resolve_from, network_path_with_port) {
  const sourcemeta::core::URI base{"http://example.com/path"};
  sourcemeta::core::URI reference{"//other.com:8080/newpath"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://other.com:8080/newpath");
}

TEST(URI_resolve_from, base_with_no_path) {
  const sourcemeta::core::URI base{"http://example.com"};
  sourcemeta::core::URI reference{"path"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://example.com/path");
}

TEST(URI_resolve_from, base_with_trailing_slash) {
  const sourcemeta::core::URI base{"http://example.com/dir/"};
  sourcemeta::core::URI reference{"file"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://example.com/dir/file");
}

TEST(URI_resolve_from, query_replaces_base_query) {
  const sourcemeta::core::URI base{"http://example.com/path?oldquery"};
  sourcemeta::core::URI reference{"?newquery"};
  reference.resolve_from(base);
  EXPECT_EQ(reference.recompose(), "http://example.com/path?newquery");
}
