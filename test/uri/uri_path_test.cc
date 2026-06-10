#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <string>
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

  EXPECT_THROW(uri.path("/foo%20bar?query=value#fragment"),
               sourcemeta::core::URIError);

  std::string path{"/fooz%20bar?query=value#fragment"};
  EXPECT_THROW(uri.path(std::move(path)), sourcemeta::core::URIError);
}

TEST(URI_path_setter, set_path_with_fragment) {
  sourcemeta::core::URI uri{"https://example.com"};

  EXPECT_THROW(uri.path("/foo%20bar#fragment"), sourcemeta::core::URIError);

  std::string path{"/fooz%20bar#fragment"};
  EXPECT_THROW(uri.path(std::move(path)), sourcemeta::core::URIError);
}

TEST(URI_path_setter, set_path_with_query_and_fragment) {
  sourcemeta::core::URI uri{"https://example.com/old?query=value#fragment"};

  EXPECT_THROW(uri.path("/new?query=value#fragment"),
               sourcemeta::core::URIError);

  std::string path{"/newer?query=value#fragment"};
  EXPECT_THROW(uri.path(std::move(path)), sourcemeta::core::URIError);
}

TEST(URI_path_setter_no_scheme, set_path_on_host_only) {
  sourcemeta::core::URI uri{"example.com"};

  uri.path("/foo");
  EXPECT_EQ(uri.path().value(), "/foo");
  EXPECT_EQ(uri.recompose(), "/foo");

  std::string path{"/bar"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/bar");
  EXPECT_EQ(uri.recompose(), "/bar");
}

TEST(URI_path_setter_no_scheme, replace_existing_path) {
  sourcemeta::core::URI uri{"example.com/old"};

  uri.path("/new");
  EXPECT_EQ(uri.path().value(), "/new");
  EXPECT_EQ(uri.recompose(), "/new");

  std::string path{"/newer"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/newer");
  EXPECT_EQ(uri.recompose(), "/newer");
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
  EXPECT_EQ(uri.path().value(), "/admin");
  EXPECT_EQ(uri.recompose(), "/admin");

  std::string path{"/admin2"};
  uri.path(std::move(path));
  EXPECT_EQ(uri.path().value(), "/admin2");
  EXPECT_EQ(uri.recompose(), "/admin2");
}

TEST(URI_path_setter, set_path_with_port) {
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

TEST(URI_path, append_path_rejects_scheme) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  EXPECT_THROW(uri.append_path("https://other.example.com/foo"),
               sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_authority_only) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  EXPECT_THROW(uri.append_path("//other.example.com/foo"),
               sourcemeta::core::URIError);
}

TEST(URI_path, append_path_colon_in_first_segment_accepted) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("bar:baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar:baz");
}

TEST(URI_path, append_path_urn_like_input_treated_as_path) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  uri.append_path("urn:example:foo");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/urn:example:foo");
}

TEST(URI_path, append_path_colon_in_non_first_segment_accepted) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  uri.append_path("foo/bar:baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar:baz");
}

TEST(URI_path, append_path_dot_dot_collapses) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo/"};
  uri.append_path("../bar");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/bar");
}

TEST(URI_path, append_path_dot_dot_multiple) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo/bar/"};
  uri.append_path("../../baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/baz");
}

TEST(URI_path, append_path_dot_collapses) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo/"};
  uri.append_path("./bar");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar");
}

TEST(URI_path, append_path_dot_dot_empty_base) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  uri.append_path("../meta/custom.json");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/meta/custom.json");
}

TEST(URI_path, append_path_rejects_fragment) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar#baz"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_query) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar?x=1"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_fragment_only) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("#bar"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_query_only) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("?x=1"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_double_slash_in_argument_preserved) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo/"};
  uri.append_path("..//bar");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080//bar");
}

TEST(URI_path, append_path_deep_dot_dot_chain) {
  sourcemeta::core::URI uri{"http://example.com:8080/a/b/c/"};
  uri.append_path("../../../d");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/d");
}

TEST(URI_path, append_path_dot_dot_past_root) {
  sourcemeta::core::URI uri{"http://example.com:8080/a"};
  uri.append_path("../../b");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/b");
}

TEST(URI_path, append_path_at_sign_in_segment_accepted) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("user@host");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/user@host");
}

TEST(URI_path, append_path_trailing_slash_on_segment_input) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("/");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/");
}

TEST(URI_path, append_path_rejects_space) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar baz"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_control_character) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar\nbaz"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_square_brackets) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar[1]"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_non_ascii) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar\xE2\x86\x92"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_truncated_percent) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar%"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_partial_percent) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar%2"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_rejects_invalid_percent_digits) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  EXPECT_THROW(uri.append_path("bar%GZ"), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_accepts_valid_percent_encoding) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("bar%20baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar%20baz");
}

TEST(URI_path, append_path_accepts_sub_delims) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path("bar!$&'()*+,;=baz");
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar!$&'()*+,;=baz");
}

TEST(URI_path, append_path_uri_overload_relative_reference) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  const sourcemeta::core::URI reference{"bar/baz"};
  uri.append_path(reference);
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar/baz");
}

TEST(URI_path, append_path_uri_overload_with_leading_slash) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  const sourcemeta::core::URI reference{"/bar"};
  uri.append_path(reference);
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar");
}

TEST(URI_path, append_path_uri_overload_dot_dot_collapses) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo/"};
  const sourcemeta::core::URI reference{"../bar"};
  uri.append_path(reference);
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/bar");
}

TEST(URI_path, append_path_uri_overload_rejects_scheme) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  const sourcemeta::core::URI reference{"https://other.example.com/foo"};
  EXPECT_THROW(uri.append_path(reference), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_uri_overload_rejects_urn) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  const sourcemeta::core::URI reference{"urn:example:foo"};
  EXPECT_THROW(uri.append_path(reference), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_uri_overload_rejects_query) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  const sourcemeta::core::URI reference{"bar?x=1"};
  EXPECT_THROW(uri.append_path(reference), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_uri_overload_rejects_fragment) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  const sourcemeta::core::URI reference{"bar#baz"};
  EXPECT_THROW(uri.append_path(reference), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_uri_overload_rejects_authority) {
  sourcemeta::core::URI uri{"http://example.com:8080"};
  const sourcemeta::core::URI reference{"//other.example.com/foo"};
  EXPECT_THROW(uri.append_path(reference), sourcemeta::core::URIError);
}

TEST(URI_path, append_path_uri_overload_chain) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  const sourcemeta::core::URI bar{"bar"};
  const sourcemeta::core::URI baz{"baz"};
  uri.append_path(bar).append_path(baz);
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar/baz");
}

TEST(URI_path, append_path_uri_overload_move) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  sourcemeta::core::URI reference{"bar/baz"};
  uri.append_path(std::move(reference));
  EXPECT_EQ(uri.recompose(), "http://example.com:8080/foo/bar/baz");
}

TEST(URI_path, append_path_uri_overload_move_temporary) {
  sourcemeta::core::URI uri{"http://example.com:8080/foo"};
  uri.append_path(sourcemeta::core::URI{"bar/baz"});
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
  // Per RFC 3986 Section 2.2, %2F is semantically distinct from literal '/'
  EXPECT_EQ(uri.path().value(), "/path%2Fwith%2Fencoded%2Fslashes");
}

TEST(URI_path_getter, encoded_question_mark_not_query_delimiter) {
  const sourcemeta::core::URI uri{"http://example.com/foo%3Fbar"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/foo%3Fbar");
  EXPECT_FALSE(uri.query().has_value());
}

TEST(URI_path_getter, encoded_hash_not_fragment_delimiter) {
  const sourcemeta::core::URI uri{"http://example.com/foo%23bar"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/foo%23bar");
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_path_getter, mixed_encoded_and_literal_slashes) {
  const sourcemeta::core::URI uri{"http://example.com/a/b%2Fc/d"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/a/b%2Fc/d");
}

TEST(URI_path_getter, encoded_slash_vs_literal_slash_are_distinct) {
  const sourcemeta::core::URI encoded{"http://example.com/v1%2F2.json"};
  const sourcemeta::core::URI literal{"http://example.com/v1/2.json"};
  EXPECT_NE(encoded.path().value(), literal.path().value());
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

TEST(URI_path_setter, getter_setter_invariant_relative_with_leading_slash) {
  sourcemeta::core::URI uri{"/test/no-serve/schema"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/test/no-serve/schema");
  EXPECT_EQ(uri.recompose(), "/test/no-serve/schema");

  const std::string original_path{uri.path().value()};
  const auto original_recompose{uri.recompose()};
  uri.path(original_path);

  EXPECT_EQ(uri.path().value(), original_path);
  EXPECT_EQ(uri.recompose(), original_recompose);
}

TEST(URI_path_setter, getter_setter_invariant_relative_without_leading_slash) {
  sourcemeta::core::URI uri{"test/no-serve/schema"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "test/no-serve/schema");
  EXPECT_EQ(uri.recompose(), "test/no-serve/schema");

  const std::string original_path{uri.path().value()};
  const auto original_recompose{uri.recompose()};
  uri.path(original_path);

  EXPECT_EQ(uri.path().value(), original_path);
  EXPECT_EQ(uri.recompose(), original_recompose);
}

TEST(URI_path_setter, getter_setter_invariant_absolute_uri) {
  sourcemeta::core::URI uri{"http://example.com/foo/bar"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "/foo/bar");
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/bar");

  const std::string original_path{uri.path().value()};
  const auto original_recompose{uri.recompose()};
  uri.path(original_path);

  EXPECT_EQ(uri.path().value(), original_path);
  EXPECT_EQ(uri.recompose(), original_recompose);
}

TEST(URI_path_setter, getter_setter_invariant_relative_dotdot) {
  sourcemeta::core::URI uri{"../foo/bar"};
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.path().value(), "../foo/bar");
  EXPECT_EQ(uri.recompose(), "../foo/bar");

  const std::string original_path{uri.path().value()};
  const auto original_recompose{uri.recompose()};
  uri.path(original_path);

  EXPECT_EQ(uri.path().value(), original_path);
  EXPECT_EQ(uri.recompose(), original_recompose);
}
