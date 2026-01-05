#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_canonicalize, example_1) {
  sourcemeta::core::URI uri{"https://example.com/foo?bar=baz#test"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/foo?bar=baz#test");
}

// The empty fragment is optional
TEST(URI_canonicalize, example_2) {
  sourcemeta::core::URI uri{"http://example.com/foo#"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo");
}

// Explicit 80 port for http scheme
TEST(URI_canonicalize, example_3) {
  sourcemeta::core::URI uri{"http://example.com:80/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/default/port");
}

// Explicit 443 port for https scheme
TEST(URI_canonicalize, example_4) {
  sourcemeta::core::URI uri{"https://example.com:443/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/default/port");
}

TEST(URI_canonicalize, example_5) {
  sourcemeta::core::URI uri{"https://example.com:80/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com:80/default/port");
}

TEST(URI_canonicalize, example_6) {
  sourcemeta::core::URI uri{"http://example.com:443/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com:443/default/port");
}

// Schemes are case insensitive
TEST(URI_canonicalize, example_7) {
  sourcemeta::core::URI uri{"hTtP://example.com/case-insensitive-scheme"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-insensitive-scheme");
}

// Hosts are case insensitive
TEST(URI_canonicalize, example_8) {
  sourcemeta::core::URI uri{"http://exAmpLe.com/case-insensitive-host"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-insensitive-host");
}

// Paths are case sensitive
TEST(URI_canonicalize, example_9) {
  sourcemeta::core::URI uri{"hTtP://exAmpLe.com/case-SENSITIVE-path"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-SENSITIVE-path");
}

TEST(URI_canonicalize, example_10) {
  sourcemeta::core::URI uri{"urn:example:schema"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "urn:example:schema");
}

TEST(URI_canonicalize, example_relative_1) {
  sourcemeta::core::URI uri{"../foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../foo");
}

TEST(URI_canonicalize, example_relative_2) {
  sourcemeta::core::URI uri{"./foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "foo");
}

TEST(URI_canonicalize, example_relative_3) {
  sourcemeta::core::URI uri{"foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "foo");
}

TEST(URI_canonicalize, example_relative_4) {
  sourcemeta::core::URI uri{"foo/bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "foo/bar");
}

TEST(URI_canonicalize, example_relative_6) {
  sourcemeta::core::URI uri{"/foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "/foo");
}

TEST(URI_canonicalize, example_12) {
  sourcemeta::core::URI uri{"#foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(URI_canonicalize, example_13) {
  sourcemeta::core::URI uri{
      "tag:bowtie.report,2023-11:referencing-suite-tag-uris-id"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(),
            "tag:bowtie.report,2023-11:referencing-suite-tag-uris-id");
}

TEST(URI_canonicalize, example_14) {
  sourcemeta::core::URI uri{"http://example.com/escapes/a%c2%b1b"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/escapes/a%C2%B1b");
}

TEST(URI_canonicalize, example_15) {
  sourcemeta::core::URI uri{"http://example.com/unreserved/%7Efoo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/unreserved/~foo");
}

TEST(URI_canonicalize, example_16) {
  sourcemeta::core::URI uri{"http://example.com/foo/"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/");
}

TEST(URI_canonicalize, example_17) {
  sourcemeta::core::URI uri{"http://example.com/"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/");
}

TEST(URI_canonicalize, empty_fragment) {
  sourcemeta::core::URI uri{"#"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_canonicalize, static_from_string) {
  const std::string uri{"hTtP://exAmpLe.com:80/TEST"};
  const auto result{sourcemeta::core::URI::canonicalize(uri)};
  EXPECT_EQ(result, "http://example.com/TEST");
}

TEST(URI_canonicalize, path_with_dotdot_absolute) {
  sourcemeta::core::URI uri{"http://example.com/foo/bar/../baz"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/baz");
}

TEST(URI_canonicalize, path_with_multiple_dotdot_absolute) {
  sourcemeta::core::URI uri{"http://example.com/foo/bar/qux/../../baz"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/baz");
}

TEST(URI_canonicalize, path_with_dotdot_at_root) {
  sourcemeta::core::URI uri{"http://example.com/../foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo");
}

TEST(URI_canonicalize, path_with_dot_segments) {
  sourcemeta::core::URI uri{"http://example.com/foo/./bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/bar");
}

TEST(URI_canonicalize, path_with_mixed_dot_segments) {
  sourcemeta::core::URI uri{"http://example.com/foo/./bar/../baz"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/baz");
}

TEST(URI_canonicalize, relative_path_with_dotdot) {
  sourcemeta::core::URI uri{"../foo/bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../foo/bar");
}

TEST(URI_canonicalize, relative_path_with_dotdot_middle) {
  sourcemeta::core::URI uri{"foo/../bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "bar");
}

TEST(URI_canonicalize, relative_path_multiple_dotdot) {
  sourcemeta::core::URI uri{"../../foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../foo");
}

TEST(URI_canonicalize, absolute_path_with_dotdot) {
  sourcemeta::core::URI uri{"/foo/bar/../baz"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "/foo/baz");
}

TEST(URI_canonicalize, absolute_path_excess_dotdot) {
  sourcemeta::core::URI uri{"/foo/../../bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "/bar");
}

TEST(URI_canonicalize, empty_uri_default_constructor) {
  sourcemeta::core::URI uri;
  uri.canonicalize();
  EXPECT_TRUE(uri.empty());
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_canonicalize, empty_uri_string_constructor) {
  sourcemeta::core::URI uri{""};
  uri.canonicalize();
  EXPECT_TRUE(uri.empty());
  EXPECT_EQ(uri.recompose(), "");
}

// Inspired from https://cr.openjdk.org/~dfuchs/writeups/updating-uri/

TEST(URI_canonicalize, rfc3986_1) {
  sourcemeta::core::URI uri{"s://h/a/../../b"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "s://h/b");
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/master/test/test.cpp#L1438

TEST(URI_canonicalize, rfc3986_2) {
  sourcemeta::core::URI uri{"eXAMPLE://a/./b/../b/%63/%7bfoo%7d"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "example://a/b/c/%7Bfoo%7D");
}

TEST(URI_canonicalize, percent_encoding) {
  sourcemeta::core::URI uri{"http://examp%4Ce.com/"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/");
}

TEST(URI_canonicalize, dot_segments_percent_encoded) {
  sourcemeta::core::URI uri{"http://example.com/a/b/%2E%2E/"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/a/");
}

TEST(URI_canonicalize, case_normalization) {
  sourcemeta::core::URI uri{"http://user:pass@SOMEHOST.COM:123"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://user:pass@somehost.com:123");
}

TEST(URI_canonicalize, complex_case) {
  sourcemeta::core::URI uri{"HTTP://a:b@HOST:123/./1/2/../%41?abc#def"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a:b@host:123/1/A?abc#def");
}

TEST(URI_canonicalize, relative_path_no_canonicalize) {
  const sourcemeta::core::URI uri{"../../abc"};
  EXPECT_EQ(uri.recompose(), "../../abc");
}

TEST(URI_canonicalize, relative_path_dotdot_trailing) {
  sourcemeta::core::URI uri{"../../abc/.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../");
}

TEST(URI_canonicalize, relative_path_dotdot_middle_with_segment) {
  sourcemeta::core::URI uri{"../../abc/../def"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../def");
}

TEST(URI_canonicalize, relative_path_cancel_to_empty) {
  sourcemeta::core::URI uri{"abc/.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_canonicalize, relative_path_cancel_to_empty_trailing_slash) {
  sourcemeta::core::URI uri{"abc/../"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_canonicalize, relative_path_multiple_dotdot_with_dot) {
  sourcemeta::core::URI uri{"../../abc/./def"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../abc/def");
}

TEST(URI_canonicalize, relative_path_leading_dot_removed) {
  sourcemeta::core::URI uri{"./def"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "def");
}

TEST(URI_canonicalize, relative_path_trailing_dot) {
  sourcemeta::core::URI uri{"def/."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "def/");
}

TEST(URI_canonicalize, relative_path_colon_ambiguity) {
  const sourcemeta::core::URI uri{"./abc:def"};
  EXPECT_EQ(uri.recompose(), "./abc:def");
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/master/test/test.cpp#L1531

TEST(URI_canonicalize, path_multiple_dotdot_to_root) {
  sourcemeta::core::URI uri{"http://a/b/c/../../.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(URI_canonicalize, path_mixed_dotdot_to_root) {
  sourcemeta::core::URI uri{"http://a/b/../c/../.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(URI_canonicalize, path_single_dotdot_at_root) {
  sourcemeta::core::URI uri{"http://a/.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(URI_canonicalize, path_dotdot_only_no_canonicalize) {
  const sourcemeta::core::URI uri{"/.."};
  EXPECT_EQ(uri.recompose(), "/..");
}

TEST(URI_canonicalize, path_empty_segments_after_dotdot) {
  sourcemeta::core::URI uri{"http://a/..///"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a///");
}

TEST(URI_canonicalize, path_empty_segments_with_trailing_dotdot) {
  sourcemeta::core::URI uri{"http://a/..///.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a//");
}

TEST(URI_canonicalize, relative_path_collapse_completely) {
  sourcemeta::core::URI uri{"a/b/c/../../.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_canonicalize, relative_path_nested_collapse) {
  sourcemeta::core::URI uri{"a/b/../../c/.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_canonicalize, ipv6_uppercase_to_lowercase) {
  sourcemeta::core::URI uri{"http://[1080::8:800:200C:417A]/foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://[1080::8:800:200c:417a]/foo");
}

TEST(URI_canonicalize, ipv6_mixed_case_to_lowercase) {
  sourcemeta::core::URI uri{"http://[::FFFF:129.144.52.38]:80/index.html"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://[::ffff:129.144.52.38]/index.html");
}

TEST(URI_canonicalize, ipv6_hex_uppercase_to_lowercase) {
  sourcemeta::core::URI uri{"http://[2010:836B:4179::836B:4179]"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://[2010:836b:4179::836b:4179]");
}
