#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <string_view>

// Getter

TEST(URI_path_getter, https_example_url_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_FALSE(uri.path().has_value());
}

TEST(URI_path_getter, https_example_url_slash) {
  const sourcemeta::core::URI uri{"https://example.com/"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/");
}

TEST(URI_path_getter, https_example_url_single) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/foo");
}

TEST(URI_path_getter, https_example_url_multi) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/foo/bar/baz");
}

TEST(URI_path_getter, relative_multi) {
  const sourcemeta::core::URI uri{"../foo/bar"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "../foo/bar");
}

TEST(URI_path_getter, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "example:schema");
}

TEST(URI_path_getter, urn_with_fragment) {
  const sourcemeta::core::URI uri{"urn:example:schema#foo"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "example:schema");
}

TEST(URI_path_getter, tag) {
  const sourcemeta::core::URI uri{
      "tag:bowtie.report,2023-11:referencing-suite-tag-uris-id"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(),
            "bowtie.report,2023-11:referencing-suite-tag-uris-id");
}

TEST(URI_path_getter, tag_with_fragment) {
  const sourcemeta::core::URI uri{
      "tag:bowtie.report,2023-11:referencing-suite-tag-uris-id#foo"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(),
            "bowtie.report,2023-11:referencing-suite-tag-uris-id");
}

TEST(URI_path_getter, without_scheme) {
  const sourcemeta::core::URI uri{"example.com/foo"};
  EXPECT_EQ(uri.path().value(), "example.com/foo");
}

TEST(URI_path_getter, mailto) {
  const sourcemeta::core::URI uri{"mailto:jdoe@woo.com"};
  EXPECT_EQ(uri.path().value(), "jdoe@woo.com");
}

// Setter

TEST(URI_path_setter, no_path) {
  sourcemeta::core::URI uri{"https://example.com"};

  uri.path(std::string{"/foo"});
  EXPECT_EQ(uri.path().value(), "/foo");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");

  std::string path{"/bar"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/bar");
}

TEST(URI_path_setter, url_slash) {
  sourcemeta::core::URI uri{"https://example.com/"};

  uri.path(std::string{"/foo"});
  EXPECT_EQ(uri.path().value(), "/foo");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");

  std::string path{"/bar"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/bar");
}

TEST(URI_path_setter, url_path) {
  sourcemeta::core::URI uri{"https://example.com/foo"};

  uri.path(std::string{"/bar"});
  EXPECT_EQ(uri.path().value(), "/bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/bar");

  std::string path{"/baz"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/baz");
  EXPECT_EQ(uri.recompose(), "https://example.com/baz");
}

TEST(URI_path_setter, set_empty) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};

  uri.path(std::string{""});
  EXPECT_EQ(uri.path().has_value(), false);
  EXPECT_EQ(uri.recompose(), "https://example.com");

  std::string empty_path{""};
  uri.path(std::move(empty_path));
  EXPECT_EQ(uri.path().has_value(), false);
  EXPECT_EQ(uri.recompose(), "https://example.com");
}

TEST(URI_path_setter, set_path_without_leading_slash) {
  sourcemeta::core::URI uri{"https://example.com"};

  uri.path(std::string{"foo"});
  EXPECT_EQ(uri.path().value(), "/foo");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");

  std::string path{"bar"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/bar");
}

TEST(URI_path_setter, set_path_with_trailing_slash) {
  sourcemeta::core::URI uri{"https://example.com"};

  uri.path(std::string{"/foo/"});
  EXPECT_EQ(uri.path().value(), "/foo/");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo/");

  std::string path{"/foo2/"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/foo2/");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo2/");
}

TEST(URI_path_setter, set_relative_path) {
  sourcemeta::core::URI uri{"https://example.com"};

  EXPECT_THROW(uri.path("../foo"), sourcemeta::core::URIError);
  EXPECT_THROW(uri.path("./foo"), sourcemeta::core::URIError);

  auto path{"../foo"};
  EXPECT_THROW(uri.path(std::move(path)), sourcemeta::core::URIError);
  EXPECT_EQ(path, "../foo");

  path = "./foo";
  EXPECT_THROW(uri.path(std::move(path)), sourcemeta::core::URIError);
  EXPECT_EQ(path, "./foo");
}

TEST(URI_path_setter, set_path_with_query) {
  sourcemeta::core::URI uri{"https://example.com"};

  uri.path("/foo%20bar?query=value#fragment");
  EXPECT_EQ(uri.path().value(), "/foo%20bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo%20bar");

  std::string path{"/fooz%20bar?query=value#fragment"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/fooz%20bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/fooz%20bar");
}

TEST(URI_path_setter, set_path_with_fragment) {
  sourcemeta::core::URI uri{"https://example.com"};

  uri.path("/foo%20bar#fragment");
  EXPECT_EQ(uri.path().value(), "/foo%20bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo%20bar");

  std::string path{"/fooz%20bar#fragment"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/fooz%20bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/fooz%20bar");
}

TEST(URI_path_setter, set_path_with_query_and_fragment) {
  sourcemeta::core::URI uri{"https://example.com/old?query=value#fragment"};

  uri.path("/new?query=value#fragment");
  EXPECT_EQ(uri.path().value(), "/new");
  EXPECT_EQ(uri.recompose(), "https://example.com/new?query=value#fragment");

  std::string path{"/newer?query=value#fragment"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/newer");
  EXPECT_EQ(uri.recompose(), "https://example.com/newer?query=value#fragment");
}

TEST(URI_path_setter_no_scheme, set_path_on_host_only) {
  sourcemeta::core::URI uri{"example.com"};

  uri.path("/foo");
  EXPECT_EQ(uri.path().value(), "foo");
  EXPECT_EQ(uri.recompose(), "foo");

  std::string path{"/bar"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "bar");
  EXPECT_EQ(uri.recompose(), "bar");
}

TEST(URI_path_setter_no_scheme, replace_existing_path) {
  sourcemeta::core::URI uri{"example.com/old"};

  uri.path("/new");
  EXPECT_EQ(uri.path().value(), "new");
  EXPECT_EQ(uri.recompose(), "new");

  std::string path{"/newer"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "newer");
  EXPECT_EQ(uri.recompose(), "newer");
}

TEST(URI_path_setter_no_scheme, set_empty_path) {
  sourcemeta::core::URI uri{"example.com/foo"};

  uri.path("");
  EXPECT_EQ(uri.path().has_value(), false);
  EXPECT_EQ(uri.recompose(), "");

  std::string empty_path{""};
  uri.path(std::move(empty_path));
  EXPECT_EQ(uri.path().has_value(), false);
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_path_setter_no_scheme, set_path_on_ip_address) {
  sourcemeta::core::URI uri{"192.168.0.1"};

  uri.path("/admin");
  EXPECT_EQ(uri.path().value(), "admin");
  EXPECT_EQ(uri.recompose(), "admin");

  std::string path{"/admin2"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "admin2");
  EXPECT_EQ(uri.recompose(), "admin2");
}

// TODO: dig why scheme return example.com
TEST(URI_path_setter_no_scheme, set_path_with_port) {
  sourcemeta::core::URI uri{"http://example.com:8080"};

  uri.path("/test");
  EXPECT_EQ(uri.path().value(), "/test");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/test");

  std::string path{"/test2"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/test2");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/test2");
}

TEST(URI_path_setter, lowercase_relative_path) {
  sourcemeta::core::URI uri{"../SERVER.JSON"};
  EXPECT_EQ(uri.recompose(), "../SERVER.JSON");
  uri.path("../server.json");
  EXPECT_EQ(uri.recompose(), "../server.json");
  const auto const_path{"../foo.json"};
  uri.path(const_path);
  EXPECT_EQ(uri.recompose(), "../foo.json");
}

TEST(URI_path, append_path_without_path_from_root) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  uri.append_path("/test");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/test");
}

TEST(URI_path, append_path_without_path) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  uri.append_path("test/bar");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/test/bar");
}

TEST(URI_path, append_path_without_path_empty) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  uri.append_path("");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080");
}

TEST(URI_path, append_path_without_path_slash_only) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  uri.append_path("/");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/");
}

TEST(URI_path, append_path_with_path_with_slash_prefix) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("/bar");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar");
}

TEST(URI_path, append_path_with_path_with_slash_only) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("/");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/");
}

TEST(URI_path, append_path_with_path_trailing_slash_clash) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo/"};
  uri.append_path("/bar");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar");
}

TEST(URI_path, append_path_with_path_trailing_slash_in_argument) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo/"};
  uri.append_path("/bar//baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar//baz");
}

TEST(URI_path, append_path_with_path_trailing_slash) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo/"};
  uri.append_path("bar/baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar/baz");
}

TEST(URI_path, append_path_with_path_no_slash_delimiters) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("bar/baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar/baz");
}

TEST(URI_path, append_path_chain) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("bar").append_path("baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar/baz");
}

TEST(URI_path_getter, rfc3986_path_with_colon) {
  const sourcemeta::core::URI uri{"http://example.com/path:with:colons"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path:with:colons");
}

TEST(URI_path_getter, rfc3986_path_with_at_sign) {
  const sourcemeta::core::URI uri{"http://example.com/path@with@at"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path@with@at");
}

TEST(URI_path_getter, rfc3986_path_with_equals) {
  const sourcemeta::core::URI uri{"http://example.com/path=with=equals"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path=with=equals");
}

TEST(URI_path_getter, rfc3986_path_with_subdelims) {
  const sourcemeta::core::URI uri{"http://example.com/path!$&'()*+,;="};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path!$&'()*+,;=");
}

TEST(URI_path_getter, rfc3986_empty_path_segments) {
  const sourcemeta::core::URI uri{"http://example.com/a//b///c"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/a//b///c");
}

TEST(URI_path_getter, rfc3986_path_only_slashes) {
  const sourcemeta::core::URI uri{"http://example.com////"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "////");
}

TEST(URI_path_getter, rfc3986_percent_encoded_path) {
  const sourcemeta::core::URI uri{"http://example.com/path%20with%20spaces"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path%20with%20spaces");
}

TEST(URI_path_getter, rfc3986_percent_encoded_slash) {
  const sourcemeta::core::URI uri{
      "http://example.com/path%2Fwith%2Fencoded%2Fslashes"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path%2Fwith%2Fencoded%2Fslashes");
}

TEST(URI_path_getter, rfc3986_path_with_unreserved) {
  const sourcemeta::core::URI uri{
      "http://example.com/path-with_unreserved.chars~123"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path-with_unreserved.chars~123");
}

TEST(URI_path_getter, rfc3986_relative_path_no_slash) {
  const sourcemeta::core::URI uri{"path/to/resource"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "path/to/resource");
}

TEST(URI_path_getter, rfc3986_relative_path_with_slash) {
  const sourcemeta::core::URI uri{"/path/to/resource"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path/to/resource");
}

TEST(URI_path_getter, rfc3986_path_single_segment) {
  const sourcemeta::core::URI uri{"http://example.com/segment"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/segment");
}

TEST(URI_path_getter, rfc3986_path_with_query_separator) {
  const sourcemeta::core::URI uri{"http://example.com/path?query=value"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path");
}

TEST(URI_path_getter, rfc3986_path_with_fragment_separator) {
  const sourcemeta::core::URI uri{"http://example.com/path#fragment"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/path");
}
