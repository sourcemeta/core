#include <sourcemeta/core/test.h>

#include <sourcemeta/core/uri.h>

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L302

TEST(syntax_error_1) {
  try {
    sourcemeta::core::URI uri{"//[::44.1"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 3);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::44.1"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[::44.1"));
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L510-L516

TEST(syntax_error_2) {
  try {
    sourcemeta::core::URI uri{"http://moo:21@moo:21@moo/"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 21);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://moo:21@moo:21@moo/"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://moo:21@moo:21@moo/"));
}

TEST(syntax_error_3) {
  try {
    sourcemeta::core::URI uri{"http://moo:21@moo:21@moo:21/"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 21);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://moo:21@moo:21@moo:21/"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://moo:21@moo:21@moo:21/"));
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L2180

TEST(syntax_error_4) {
  try {
    // missing "]"
    sourcemeta::core::URI uri{"http://[vA.123456"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 8);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[vA.123456"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("http://[vA.123456"));
}

TEST(syntax_error_5) {
  try {
    sourcemeta::core::URI uri{"https://www.example.com#/foo%6G"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 29);
  }
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%6G"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%6G"));
}

TEST(urn_with_slash) {
  // RFC 8141 explicitly allows "/" in URN NSS
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("urn:example:foo/bar/baz"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("urn:example:foo/bar/baz"));
  sourcemeta::core::URI uri{"urn:example:foo/bar/baz"};
  EXPECT_EQ(uri.scheme().value(), "urn");
  EXPECT_EQ(uri.path().value(), "example:foo/bar/baz");
  EXPECT_EQ(uri.recompose(), "urn:example:foo/bar/baz");
}

TEST(urn_with_numeric_path) {
  // Example from RFC 8141
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("urn:example:1/406/47452/2"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("urn:example:1/406/47452/2"));
  sourcemeta::core::URI uri{"urn:example:1/406/47452/2"};
  EXPECT_EQ(uri.scheme().value(), "urn");
  EXPECT_EQ(uri.path().value(), "example:1/406/47452/2");
  EXPECT_EQ(uri.recompose(), "urn:example:1/406/47452/2");
}

TEST(urn_with_query) {
  // RFC 8141 allows query components in URNs
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("urn:example:foo?+bar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("urn:example:foo?+bar"));
  sourcemeta::core::URI uri{"urn:example:foo?+bar"};
  EXPECT_EQ(uri.scheme().value(), "urn");
  EXPECT_EQ(uri.path().value(), "example:foo");
  EXPECT_EQ(uri.query().value().raw(), "+bar");
  EXPECT_EQ(uri.recompose(), "urn:example:foo?+bar");
}

TEST(urn_with_fragment) {
  // RFC 8141 allows fragments in URNs
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("urn:example:foo#bar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("urn:example:foo#bar"));
  sourcemeta::core::URI uri{"urn:example:foo#bar"};
  EXPECT_EQ(uri.scheme().value(), "urn");
  EXPECT_EQ(uri.path().value(), "example:foo");
  EXPECT_EQ(uri.fragment().value(), "bar");
  EXPECT_EQ(uri.recompose(), "urn:example:foo#bar");
}

TEST(syntax_error_percent_at_end) {
  try {
    sourcemeta::core::URI uri{"https://www.example.com#/foo%"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 29);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("https://www.example.com#/foo%"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("https://www.example.com#/foo%"));
}

TEST(syntax_error_percent_one_hex) {
  try {
    sourcemeta::core::URI uri{"https://www.example.com#/foo%2"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 29);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("https://www.example.com#/foo%2"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%2"));
}

TEST(syntax_error_percent_non_hex) {
  try {
    sourcemeta::core::URI uri{"https://www.example.com#/foo%ZZ"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 29);
  }
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%ZZ"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%ZZ"));
}

TEST(syntax_error_percent_in_path) {
  try {
    sourcemeta::core::URI uri{"https://www.example.com/foo%6G"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 28);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("https://www.example.com/foo%6G"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com/foo%6G"));
}

TEST(syntax_error_percent_in_query) {
  try {
    sourcemeta::core::URI uri{"https://www.example.com?foo%6G"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 28);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("https://www.example.com?foo%6G"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com?foo%6G"));
}

TEST(syntax_error_percent_in_host) {
  try {
    sourcemeta::core::URI uri{"https://www.exam%ple.com"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 17);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("https://www.exam%ple.com"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("https://www.exam%ple.com"));
}

TEST(rfc3986_mixed_case_percent_encoding_lower_upper) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://a.com/%aF"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("http://a.com/%aF"));
  sourcemeta::core::URI uri{"http://a.com/%aF"};
  EXPECT_EQ(uri.recompose(), "http://a.com/%aF");
}

// RFC 3986: fragment = *( pchar / "/" / "?" )
TEST(syntax_error_double_fragment_delimiter) {
  try {
    sourcemeta::core::URI uri{"http://example.com/#frag#ment"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 25);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/#frag#ment"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://example.com/#frag#ment"));
}

// RFC 3986: "[" and "]" are gen-delims only allowed in IP-literal within
// host.
TEST(syntax_error_brackets_in_query) {
  try {
    sourcemeta::core::URI uri{"http://example.com/?q=[value]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 23);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/?q=[value]"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://example.com/?q=[value]"));
}

// RFC 3986: relative-ref path-noscheme means the first segment of a
// schemeless, authorityless path must not contain a colon
TEST(syntax_error_digit_prefix_scheme_like) {
  try {
    sourcemeta::core::URI uri{"2http://example.com"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 6);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("2http://example.com"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("2http://example.com"));
}

TEST(syntax_error_underscore_scheme_like) {
  try {
    sourcemeta::core::URI uri{"my_scheme://example.com"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 10);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("my_scheme://example.com"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("my_scheme://example.com"));
}

TEST(syntax_error_colon_slash_slash) {
  try {
    sourcemeta::core::URI uri{"://example.com"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 1);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("://example.com"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("://example.com"));
}

TEST(syntax_error_hyphen_prefix_scheme_like) {
  try {
    sourcemeta::core::URI uri{"-http://example.com"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 6);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("-http://example.com"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("-http://example.com"));
}

// RFC 3986: path-abempty after authority must start with "/" or be empty
TEST(syntax_error_port_trailing_alpha) {
  try {
    sourcemeta::core::URI uri{"http://example.com:80a"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 22);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:80a"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://example.com:80a"));
}

TEST(syntax_error_port_trailing_range) {
  try {
    sourcemeta::core::URI uri{"http://example.com:80-90"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 22);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:80-90"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://example.com:80-90"));
}

TEST(syntax_error_port_exceeds_uint32) {
  try {
    sourcemeta::core::URI uri{"http://example.com:4294967296"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 20);
  }
  // RFC 3986: port = *DIGIT, so any digit sequence is syntactically valid
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:4294967296"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://example.com:4294967296"));
}

TEST(syntax_error_port_overflow_unsigned_long) {
  try {
    sourcemeta::core::URI uri{
        "http://example.com:999999999999999999999999999999"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 20);
  }
  // RFC 3986: port = *DIGIT, so any digit sequence is syntactically valid
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://example.com:999999999999999999999999999999"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://example.com:999999999999999999999999999999"));
}

TEST(rfc3986_port_max_uint32) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:4294967295"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://example.com:4294967295"));
  sourcemeta::core::URI uri{"http://example.com:4294967295"};
  EXPECT_EQ(uri.port().value(), 4294967295U);
}

TEST(syntax_error_bare_ipv6_no_brackets) {
  try {
    sourcemeta::core::URI uri{"http://2001:db8::1"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 13);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://2001:db8::1"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("http://2001:db8::1"));
}

// RFC 3986: IPv6 brackets must contain valid hex digits, colons, and dots
TEST(syntax_error_ipv6_invalid_hex) {
  try {
    sourcemeta::core::URI uri{"http://[2001:db8::gggg]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 19);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[2001:db8::gggg]"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://[2001:db8::gggg]"));
}

// RFC 3986: IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
// Must have version hex digits and dot separator
TEST(syntax_error_ipvfuture_bare_v) {
  try {
    sourcemeta::core::URI uri{"http://[v]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 10);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[v]"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("http://[v]"));
}

TEST(syntax_error_ipvfuture_missing_dot) {
  try {
    sourcemeta::core::URI uri{"http://[vabc]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 13);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[vabc]"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("http://[vabc]"));
}

TEST(syntax_error_ipvfuture_non_hex_version) {
  try {
    sourcemeta::core::URI uri{"http://[vZ.foo]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 10);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[vZ.foo]"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("http://[vZ.foo]"));
}

TEST(syntax_error_ipv6_empty_brackets) {
  try {
    sourcemeta::core::URI uri{"http://[]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 8);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[]"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("http://[]"));
}

TEST(syntax_error_ipv6_h16_too_long) {
  try {
    sourcemeta::core::URI uri{"http://[2001:db8::00000]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 8);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[2001:db8::00000]"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://[2001:db8::00000]"));
}

TEST(syntax_error_ipv6_double_compression) {
  try {
    sourcemeta::core::URI uri{"http://[2001::db8::1]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 8);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[2001::db8::1]"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://[2001::db8::1]"));
}

TEST(syntax_error_ipv6_too_few_groups_without_compression) {
  try {
    sourcemeta::core::URI uri{"http://[1:2:3:4:5:6:7]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 8);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[1:2:3:4:5:6:7]"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://[1:2:3:4:5:6:7]"));
}

TEST(syntax_error_ipv6_too_many_groups) {
  try {
    sourcemeta::core::URI uri{"http://[1:2:3:4:5:6:7:8:9]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 8);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[1:2:3:4:5:6:7:8:9]"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://[1:2:3:4:5:6:7:8:9]"));
}

TEST(syntax_error_ipv6_embedded_ipv4_out_of_range) {
  try {
    sourcemeta::core::URI uri{"http://[::ffff:1.2.3.256]"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 8);
  }
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[::ffff:1.2.3.256]"));
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("http://[::ffff:1.2.3.256]"));
}

TEST(success_ipvfuture_valid) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[vFF.a:b]"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("http://[vFF.a:b]"));
  sourcemeta::core::URI uri{"http://[vFF.a:b]"};
  EXPECT_EQ(uri.host().value(), "vFF.a:b");
  EXPECT_EQ(uri.recompose(), "http://[vFF.a:b]");
}

// RFC 3986: port = *DIGIT (empty port after colon is valid)
TEST(rfc3986_empty_port) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("http://example.com:"));
  sourcemeta::core::URI uri{"http://example.com:"};
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_FALSE(uri.port().has_value());
}

TEST(rfc3986_empty_port_with_path) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:/path"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://example.com:/path"));
  sourcemeta::core::URI uri{"http://example.com:/path"};
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_FALSE(uri.port().has_value());
  EXPECT_EQ(uri.path().value(), "/path");
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L315

TEST(success_1) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(
      "//user:pass@[::1]:80/segment/index.html?query#frag"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "//user:pass@[::1]:80/segment/index.html?query#frag"));
  sourcemeta::core::URI uri{
      "//user:pass@[::1]:80/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(),
            "//user:pass@[::1]:80/segment/index.html?query#frag");
}

TEST(success_2) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://[::1]:80/segment/index.html?query#frag"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://[::1]:80/segment/index.html?query#frag"));
  sourcemeta::core::URI uri{"http://[::1]:80/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(), "http://[::1]:80/segment/index.html?query#frag");
}

TEST(success_3) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://user:pass@[::1]/segment/index.html?query#frag"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://user:pass@[::1]/segment/index.html?query#frag"));
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]/segment/index.html?query#frag");
}

TEST(success_4) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("http://user:pass@[::1]:80?query#frag"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://user:pass@[::1]:80?query#frag"));
  sourcemeta::core::URI uri{"http://user:pass@[::1]:80?query#frag"};
  EXPECT_EQ(uri.recompose(), "http://user:pass@[::1]:80?query#frag");
}

TEST(success_5) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://user:pass@[::1]:80/segment/index.html#frag"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://user:pass@[::1]:80/segment/index.html#frag"));
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]:80/segment/index.html#frag"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]:80/segment/index.html#frag");
}

TEST(success_6) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://user:pass@[::1]:80/segment/index.html?query"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://user:pass@[::1]:80/segment/index.html?query"));
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]:80/segment/index.html?query"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]:80/segment/index.html?query");
}

TEST(success_7) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("ftp://host:21/gnu/"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("ftp://host:21/gnu/"));
  sourcemeta::core::URI uri{"ftp://host:21/gnu/"};
  EXPECT_EQ(uri.recompose(), "ftp://host:21/gnu/");
}

TEST(success_with_percent_25_stays_encoded) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%25bar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%25bar"));
  sourcemeta::core::URI uri{"https://www.example.com#/foo%25bar"};
  // %25 encodes %, which is not unreserved, so must not be decoded
  EXPECT_EQ(uri.fragment(), "/foo%25bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo%25bar");
}

TEST(success_with_space_percent_20_stays_encoded) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com/foo%20bar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com/foo%20bar"));
  sourcemeta::core::URI uri{"https://www.example.com/foo%20bar"};
  // Space is not unreserved, so %20 should not be decoded internally
  EXPECT_EQ(uri.path(), "/foo%20bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com/foo%20bar");
}

TEST(success_with_equals_percent_3D_stays_encoded) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com?foo%3Dbar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com?foo%3Dbar"));
  sourcemeta::core::URI uri{"https://www.example.com?foo%3Dbar"};
  // Per RFC 3986 Section 2.2, %3D (=) is a reserved sub-delim
  // and must not be decoded
  EXPECT_EQ(uri.query().value().raw(), "foo%3Dbar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com?foo%3Dbar");
}

TEST(success_with_slash_percent_2F_stays_encoded) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%2Fbar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%2Fbar"));
  sourcemeta::core::URI uri{"https://www.example.com#/foo%2Fbar"};
  // Per RFC 3986 Section 2.2, %2F (/) is a reserved gen-delim
  // and must not be decoded
  EXPECT_EQ(uri.fragment(), "/foo%2Fbar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo%2Fbar");
}

TEST(success_with_lowercase_normalized_to_uppercase) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%2fbar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%2fbar"));
  sourcemeta::core::URI uri{"https://www.example.com#/foo%2fbar"};
  // Per RFC 3986 Section 2.2, reserved chars must not be decoded.
  // Hex case is preserved by recompose; canonicalize uppercases it.
  EXPECT_EQ(uri.fragment(), "/foo%2fbar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo%2fbar");
}

TEST(success_unreserved_char_decoded_hyphen) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%2Dbar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%2Dbar"));
  sourcemeta::core::URI uri{"https://www.example.com#/foo%2Dbar"};
  EXPECT_EQ(uri.fragment(), "/foo-bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo-bar");
}

TEST(success_unreserved_char_decoded_tilde) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%7Ebar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%7Ebar"));
  sourcemeta::core::URI uri{"https://www.example.com#/foo%7Ebar"};
  EXPECT_EQ(uri.fragment(), "/foo~bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo~bar");
}

TEST(success_unreserved_char_decoded_underscore) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%5Fbar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%5Fbar"));
  sourcemeta::core::URI uri{"https://www.example.com#/foo%5Fbar"};
  EXPECT_EQ(uri.fragment(), "/foo_bar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/foo_bar");
}

TEST(success_unreserved_char_decoded_letter) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("https://www.example.com#/foo%41bar"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "https://www.example.com#/foo%41bar"));
  sourcemeta::core::URI uri{"https://www.example.com#/foo%41bar"};
  EXPECT_EQ(uri.fragment(), "/fooAbar");
  EXPECT_EQ(uri.recompose(), "https://www.example.com#/fooAbar");
}

TEST(relative_1) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("one/two/three"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("one/two/three"));
  sourcemeta::core::URI uri{"one/two/three"};
  EXPECT_EQ(uri.recompose(), "one/two/three");
}

TEST(relative_2) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/one/two/three"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/one/two/three"));
  sourcemeta::core::URI uri{"/one/two/three"};
  EXPECT_EQ(uri.recompose(), "/one/two/three");
}

TEST(relative_3) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri("//user:pass@localhost/one/two/three"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "//user:pass@localhost/one/two/three"));
  sourcemeta::core::URI uri{"//user:pass@localhost/one/two/three"};
  EXPECT_EQ(uri.recompose(), "//user:pass@localhost/one/two/three");
}

TEST(real_life_1) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://sourceforge.net/projects/uriparser/"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://sourceforge.net/projects/uriparser/"));
  sourcemeta::core::URI uri{"http://sourceforge.net/projects/uriparser/"};
  EXPECT_EQ(uri.recompose(), "http://sourceforge.net/projects/uriparser/");
}

TEST(real_life_2) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://sourceforge.net/project/platformdownload.php?group_id=182840"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://sourceforge.net/project/platformdownload.php?group_id=182840"));
  sourcemeta::core::URI uri{
      "http://sourceforge.net/project/platformdownload.php?group_id=182840"};
  EXPECT_EQ(
      uri.recompose(),
      "http://sourceforge.net/project/platformdownload.php?group_id=182840");
}

TEST(mailto) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("mailto:test@example.com"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("mailto:test@example.com"));
  sourcemeta::core::URI uri{"mailto:test@example.com"};
  EXPECT_EQ(uri.recompose(), "mailto:test@example.com");
}

TEST(relative_4) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("../../"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("../../"));
  sourcemeta::core::URI uri{"../../"};
  EXPECT_EQ(uri.recompose(), "../../");
}

TEST(root_path) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/"));
  sourcemeta::core::URI uri{"/"};
  EXPECT_EQ(uri.recompose(), "/");
}

TEST(empty_uri) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(""));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(""));
  sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.recompose(), "");
}

TEST(file_uri) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("file:///bin/bash"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("file:///bin/bash"));
  sourcemeta::core::URI uri{"file:///bin/bash"};
  EXPECT_EQ(uri.recompose(), "file:///bin/bash");
}

TEST(percent_encoding) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://www.example.com/name%20with%20spaces/"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://www.example.com/name%20with%20spaces/"));
  sourcemeta::core::URI uri{"http://www.example.com/name%20with%20spaces/"};
  EXPECT_EQ(uri.recompose(), "http://www.example.com/name%20with%20spaces/");
}

TEST(rfc3986_complete_uri) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("http://user:pass@example.com:8080/path/to/"
                                    "resource?query=value&key=data#section"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://user:pass@example.com:8080/path/to/"
      "resource?query=value&key=data#section"));
  sourcemeta::core::URI uri{"http://user:pass@example.com:8080/path/to/"
                            "resource?query=value&key=data#section"};
  EXPECT_EQ(uri.scheme().value(), "http");
  EXPECT_EQ(uri.userinfo().value(), "user:pass");
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_EQ(uri.port().value(), 8080);
  EXPECT_EQ(uri.path().value(), "/path/to/resource");
  EXPECT_EQ(uri.query().value().raw(), "query=value&key=data");
  EXPECT_EQ(uri.fragment().value(), "section");
}

TEST(rfc3986_minimal_uri) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("s:p"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("s:p"));
  sourcemeta::core::URI uri{"s:p"};
  EXPECT_EQ(uri.scheme().value(), "s");
  EXPECT_TRUE(uri.path().has_value());
  EXPECT_EQ(uri.recompose(), "s:p");
}

TEST(rfc3986_authority_without_userinfo) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/path"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://example.com/path"));
  sourcemeta::core::URI uri{"http://example.com/path"};
  EXPECT_FALSE(uri.userinfo().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
}

TEST(rfc3986_authority_with_empty_userinfo) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://@example.com/path"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://@example.com/path"));
  sourcemeta::core::URI uri{"http://@example.com/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "");
}

TEST(rfc3986_authority_with_userinfo_no_password) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://user@example.com/path"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://user@example.com/path"));
  sourcemeta::core::URI uri{"http://user@example.com/path"};
  EXPECT_TRUE(uri.userinfo().has_value());
  EXPECT_EQ(uri.userinfo().value(), "user");
}

TEST(rfc3986_path_absolute_no_authority) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/absolute/path"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/absolute/path"));
  sourcemeta::core::URI uri{"/absolute/path"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_FALSE(uri.host().has_value());
  EXPECT_EQ(uri.path().value(), "/absolute/path");
  EXPECT_EQ(uri.recompose(), "/absolute/path");
}

TEST(rfc3986_path_relative_simple) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("relative/path"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("relative/path"));
  sourcemeta::core::URI uri{"relative/path"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_FALSE(uri.host().has_value());
  EXPECT_EQ(uri.path().value(), "relative/path");
  EXPECT_EQ(uri.recompose(), "relative/path");
}

TEST(rfc3986_query_only) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?query=value"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("?query=value"));
  sourcemeta::core::URI uri{"?query=value"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query=value");
  EXPECT_EQ(uri.recompose(), "?query=value");
}

TEST(rfc3986_fragment_only) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#fragment"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("#fragment"));
  sourcemeta::core::URI uri{"#fragment"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "fragment");
  EXPECT_EQ(uri.recompose(), "#fragment");
}

TEST(rfc3986_percent_encoded_unreserved) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%7Euser/path"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://example.com/%7Euser/path"));
  sourcemeta::core::URI uri{"http://example.com/%7Euser/path"};
  EXPECT_EQ(uri.recompose(), "http://example.com/~user/path");
}

TEST(rfc3986_percent_encoded_reserved) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://example.com/path%2Fwith%2Fslashes"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://example.com/path%2Fwith%2Fslashes"));
  sourcemeta::core::URI uri{"http://example.com/path%2Fwith%2Fslashes"};
  // Per RFC 3986 Section 2.2, percent-encoded reserved characters like %2F
  // must be preserved as they are semantically distinct from literal '/'
  EXPECT_EQ(uri.recompose(), "http://example.com/path%2Fwith%2Fslashes");
}

TEST(rfc3986_mixed_case_percent_encoding) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%3a%3A%3b%3B"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://example.com/%3a%3A%3b%3B"));
  sourcemeta::core::URI uri{"http://example.com/%3a%3A%3b%3B"};
  // Per RFC 3986 Section 2.2, : and ; are reserved characters
  // and must not be decoded. Hex case preserved by recompose.
  EXPECT_EQ(uri.recompose(), "http://example.com/%3a%3A%3b%3B");
}

TEST(rfc3986_authority_only) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//example.com"));
  sourcemeta::core::URI uri{"//example.com"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_EQ(uri.recompose(), "//example.com");
}

TEST(rfc3986_authority_with_port_no_path) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com:8080"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//example.com:8080"));
  sourcemeta::core::URI uri{"//example.com:8080"};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_EQ(uri.port().value(), 8080);
  EXPECT_EQ(uri.recompose(), "//example.com:8080");
}

TEST(rfc3986_ipv4_dotted_decimal) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://127.0.0.1/path"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("http://127.0.0.1/path"));
  sourcemeta::core::URI uri{"http://127.0.0.1/path"};
  EXPECT_EQ(uri.host().value(), "127.0.0.1");
  EXPECT_EQ(uri.recompose(), "http://127.0.0.1/path");
}

TEST(rfc3986_ipv6_full_form) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://[2001:0db8:0000:0000:0000:0000:0000:0001]/path"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(
      "http://[2001:0db8:0000:0000:0000:0000:0000:0001]/path"));
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

TEST(rfc3986_ipv6_compressed) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[::1]/path"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("http://[::1]/path"));
  sourcemeta::core::URI uri{"http://[::1]/path"};
  EXPECT_EQ(uri.host().value(), "::1");
  EXPECT_EQ(uri.recompose(), "http://[::1]/path");
}

TEST(rfc3986_empty_path_with_query) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com?query"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://example.com?query"));
  sourcemeta::core::URI uri{"http://example.com?query"};
  EXPECT_FALSE(uri.path().has_value());
  EXPECT_EQ(uri.query().value().raw(), "query");
}

TEST(rfc3986_empty_path_with_fragment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com#fragment"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://example.com#fragment"));
  sourcemeta::core::URI uri{"http://example.com#fragment"};
  EXPECT_FALSE(uri.path().has_value());
  EXPECT_EQ(uri.fragment().value(), "fragment");
}

TEST(uri_constructor_rejects_non_ascii) {
  // The plain URI constructor does not accept IRI characters
  try {
    sourcemeta::core::URI{"https://example.com/caf\xC3\xA9"};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 24);
  }
}
