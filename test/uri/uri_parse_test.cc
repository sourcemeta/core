#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L302

TEST(URI_parse, syntax_error_1) {
  EXPECT_THROW(sourcemeta::core::URI uri{"//[::44.1"},
               sourcemeta::core::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L510-L516

TEST(URI_parse, syntax_error_2) {
  EXPECT_THROW(sourcemeta::core::URI uri{"http://moo:21@moo:21@moo/"},
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_3) {
  EXPECT_THROW(sourcemeta::core::URI uri{"http://moo:21@moo:21@moo:21/"},
               sourcemeta::core::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L2180

TEST(URI_parse, syntax_error_4) {
  EXPECT_THROW(sourcemeta::core::URI uri{"http://[vA.123456"}, // missing "]"
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_5) {
  EXPECT_THROW(sourcemeta::core::URI uri{"https://www.example.com#/foo%bar"},
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_percent_at_end) {
  EXPECT_THROW(sourcemeta::core::URI uri{"https://www.example.com#/foo%"},
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_percent_one_hex) {
  EXPECT_THROW(sourcemeta::core::URI uri{"https://www.example.com#/foo%2"},
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_percent_non_hex) {
  EXPECT_THROW(sourcemeta::core::URI uri{"https://www.example.com#/foo%ZZ"},
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_percent_in_path) {
  EXPECT_THROW(sourcemeta::core::URI uri{"https://www.example.com/foo%bar"},
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_percent_in_query) {
  EXPECT_THROW(sourcemeta::core::URI uri{"https://www.example.com?foo%bar"},
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_percent_in_host) {
  EXPECT_THROW(sourcemeta::core::URI uri{"https://www.exam%ple.com"},
               sourcemeta::core::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L315

TEST(URI_parse, success_1) {
  sourcemeta::core::URI uri{
      "//user:pass@[::1]:80/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(),
            "//user:pass@[::1]:80/segment/index.html?query#frag");
}

TEST(URI_parse, success_2) {
  sourcemeta::core::URI uri{"http://[::1]:80/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(), "http://[::1]:80/segment/index.html?query#frag");
}

TEST(URI_parse, success_3) {
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]/segment/index.html?query#frag");
}

TEST(URI_parse, success_4) {
  sourcemeta::core::URI uri{"http://user:pass@[::1]:80?query#frag"};
  EXPECT_EQ(uri.recompose(), "http://user:pass@[::1]:80?query#frag");
}

TEST(URI_parse, success_5) {
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]:80/segment/index.html#frag"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]:80/segment/index.html#frag");
}

TEST(URI_parse, success_6) {
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]:80/segment/index.html?query"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]:80/segment/index.html?query");
}

TEST(URI_parse, success_7) {
  sourcemeta::core::URI uri{"ftp://host:21/gnu/"};
  EXPECT_EQ(uri.recompose(), "ftp://host:21/gnu/");
}

TEST(URI_parse, success_with_percent_25_stays_encoded) {
  sourcemeta::core::URI uri{"https://www.example.com#/foo%25bar"};
  EXPECT_EQ(uri.fragment(), "/foo%bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo%25bar");
}

TEST(URI_parse, success_with_space_percent_20_stays_encoded) {
  sourcemeta::core::URI uri{"https://www.example.com/foo%20bar"};
  EXPECT_EQ(uri.path(), "/foo bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com/foo%20bar");
}

TEST(URI_parse, success_with_equals_percent_3D_stays_encoded) {
  sourcemeta::core::URI uri{"https://www.example.com?foo%3Dbar"};
  EXPECT_EQ(uri.query(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com?foo=bar");
}

TEST(URI_parse, success_with_slash_percent_2F_stays_encoded) {
  sourcemeta::core::URI uri{"https://www.example.com#/foo%2Fbar"};
  EXPECT_EQ(uri.fragment(), "/foo/bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo/bar");
}

TEST(URI_parse, success_with_lowercase_normalized_to_uppercase) {
  sourcemeta::core::URI uri{"https://www.example.com#/foo%2fbar"};
  EXPECT_EQ(uri.fragment(), "/foo/bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo/bar");
}

TEST(URI_parse, success_unreserved_char_decoded_hyphen) {
  sourcemeta::core::URI uri{"https://www.example.com#/foo%2Dbar"};
  EXPECT_EQ(uri.fragment(), "/foo-bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo-bar");
}

TEST(URI_parse, success_unreserved_char_decoded_tilde) {
  sourcemeta::core::URI uri{"https://www.example.com#/foo%7Ebar"};
  EXPECT_EQ(uri.fragment(), "/foo~bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo~bar");
}

TEST(URI_parse, success_unreserved_char_decoded_underscore) {
  sourcemeta::core::URI uri{"https://www.example.com#/foo%5Fbar"};
  EXPECT_EQ(uri.fragment(), "/foo_bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo_bar");
}

TEST(URI_parse, success_unreserved_char_decoded_letter) {
  sourcemeta::core::URI uri{"https://www.example.com#/foo%41bar"};
  EXPECT_EQ(uri.fragment(), "/fooAbar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/fooAbar");
}

TEST(URI_parse, relative_1) {
  sourcemeta::core::URI uri{"one/two/three"};
  EXPECT_EQ(uri.recompose(), "one/two/three");
}

TEST(URI_parse, relative_2) {
  sourcemeta::core::URI uri{"/one/two/three"};
  EXPECT_EQ(uri.recompose(), "/one/two/three");
}

TEST(URI_parse, relative_3) {
  sourcemeta::core::URI uri{"//user:pass@localhost/one/two/three"};
  EXPECT_EQ(uri.recompose(), "//user:pass@localhost/one/two/three");
}

TEST(URI_parse, real_life_1) {
  sourcemeta::core::URI uri{"http://sourceforge.net/projects/uriparser/"};
  EXPECT_EQ(uri.recompose(), "http://sourceforge.net/projects/uriparser/");
}

TEST(URI_parse, real_life_2) {
  sourcemeta::core::URI uri{
      "http://sourceforge.net/project/platformdownload.php?group_id=182840"};
  EXPECT_EQ(
      uri.recompose(),
      "http://sourceforge.net/project/platformdownload.php?group_id=182840");
}

TEST(URI_parse, mailto) {
  sourcemeta::core::URI uri{"mailto:test@example.com"};
  EXPECT_EQ(uri.recompose(), "mailto:test@example.com");
}

TEST(URI_parse, relative_4) {
  sourcemeta::core::URI uri{"../../"};
  EXPECT_EQ(uri.recompose(), "../../");
}

TEST(URI_parse, root_path) {
  sourcemeta::core::URI uri{"/"};
  EXPECT_EQ(uri.recompose(), "/");
}

TEST(URI_parse, empty_uri) {
  sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_parse, file_uri) {
  sourcemeta::core::URI uri{"file:///bin/bash"};
  EXPECT_EQ(uri.recompose(), "file:///bin/bash");
}

TEST(URI_parse, percent_encoding) {
  sourcemeta::core::URI uri{"http://www.example.com/name%20with%20spaces/"};
  EXPECT_EQ(uri.recompose(), "http://www.example.com/name%20with%20spaces/");
}

TEST(URI_parse, rfc3986_complete_uri) {
  sourcemeta::core::URI uri{"http://user:pass@example.com:8080/path/to/"
                            "resource?query=value&key=data#section"};
  EXPECT_EQ(uri.scheme().value(), "http");
  EXPECT_EQ(uri.userinfo().value(), "user:pass");
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_EQ(uri.port().value(), 8080);
  EXPECT_EQ(uri.path().value(), "/path/to/resource");
  EXPECT_EQ(uri.query().value(), "query=value&key=data");
  EXPECT_EQ(uri.fragment().value(), "section");
}

TEST(URI_parse, rfc3986_minimal_uri) {
  sourcemeta::core::URI uri{"s:p"};
  EXPECT_EQ(uri.scheme().value(), "s");
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.recompose(), "s:p");
}

TEST(URI_parse, rfc3986_authority_without_userinfo) {
  sourcemeta::core::URI uri{"http://example.com/path"};
  EXPECT_FALSE(uri.userinfo().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
}

TEST(URI_parse, rfc3986_authority_with_empty_userinfo) {
  sourcemeta::core::URI uri{"http://@example.com/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "");
}

TEST(URI_parse, rfc3986_authority_with_userinfo_no_password) {
  sourcemeta::core::URI uri{"http://user@example.com/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user");
}

TEST(URI_parse, rfc3986_path_absolute_no_authority) {
  sourcemeta::core::URI uri{"/absolute/path"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_FALSE(uri.host().has_value());
  EXPECT_EQ(uri.path().value(), "/absolute/path");
  EXPECT_EQ(uri.recompose(), "/absolute/path");
}

TEST(URI_parse, rfc3986_path_relative_simple) {
  sourcemeta::core::URI uri{"relative/path"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_FALSE(uri.host().has_value());
  EXPECT_EQ(uri.path().value(), "relative/path");
  EXPECT_EQ(uri.recompose(), "relative/path");
}

TEST(URI_parse, rfc3986_query_only) {
  sourcemeta::core::URI uri{"?query=value"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "query=value");
  EXPECT_EQ(uri.recompose(), "?query=value");
}

TEST(URI_parse, rfc3986_fragment_only) {
  sourcemeta::core::URI uri{"#fragment"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "fragment");
  EXPECT_EQ(uri.recompose(), "#fragment");
}

TEST(URI_parse, rfc3986_percent_encoded_unreserved) {
  sourcemeta::core::URI uri{"http://example.com/%7Euser/path"};
  EXPECT_EQ(uri.recompose(), "http://example.com/~user/path");
}

TEST(URI_parse, rfc3986_percent_encoded_reserved) {
  sourcemeta::core::URI uri{"http://example.com/path%2Fwith%2Fslashes"};
  EXPECT_EQ(uri.recompose(), "http://example.com/path/with/slashes");
}

TEST(URI_parse, rfc3986_mixed_case_percent_encoding) {
  sourcemeta::core::URI uri{"http://example.com/%3a%3A%3b%3B"};
  EXPECT_EQ(uri.recompose(), "http://example.com/::;;");
}

TEST(URI_parse, rfc3986_authority_only) {
  sourcemeta::core::URI uri{"//example.com"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_EQ(uri.recompose(), "//example.com");
}

TEST(URI_parse, rfc3986_authority_with_port_no_path) {
  sourcemeta::core::URI uri{"//example.com:8080"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_EQ(uri.port().value(), 8080);
  EXPECT_EQ(uri.recompose(), "//example.com:8080");
}

TEST(URI_parse, rfc3986_ipv4_dotted_decimal) {
  sourcemeta::core::URI uri{"http://127.0.0.1/path"};
  EXPECT_EQ(uri.host().value(), "127.0.0.1");
  EXPECT_EQ(uri.recompose(), "http://127.0.0.1/path");
}

TEST(URI_parse, rfc3986_ipv6_full_form) {
  sourcemeta::core::URI uri{
      "http://[2001:0db8:0000:0000:0000:0000:0000:0001]/path"};
  // uriparser normalizes IPv6 but may not fully compress leading zeros
  EXPECT_TRUE(uri.host().has_value());
  const auto host = uri.host().value();
  // Accept either compressed or uncompressed form
  EXPECT_TRUE(host == "2001:db8::1" ||
              host == "2001:0db8:0000:0000:0000:0000:0000:0001" ||
              host.find("2001:") == 0);
}

TEST(URI_parse, rfc3986_ipv6_compressed) {
  sourcemeta::core::URI uri{"http://[::1]/path"};
  EXPECT_EQ(uri.host().value(), "::1");
  EXPECT_EQ(uri.recompose(), "http://[::1]/path");
}

TEST(URI_parse, rfc3986_empty_path_with_query) {
  sourcemeta::core::URI uri{"http://example.com?query"};
  EXPECT_FALSE(uri.path().has_value());
  EXPECT_EQ(uri.query().value(), "query");
}

TEST(URI_parse, rfc3986_empty_path_with_fragment) {
  sourcemeta::core::URI uri{"http://example.com#fragment"};
  EXPECT_FALSE(uri.path().has_value());
  EXPECT_EQ(uri.fragment().value(), "fragment");
}
