#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(example_1) {
  sourcemeta::core::URI uri{"https://example.com/foo?bar=baz#test"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/foo?bar=baz#test");
}

// RFC 3986 Section 4.2: a recomposed path beginning with "//" and no authority
// must not re-parse as an authority
TEST(recompose_double_slash_path_stays_authority_less) {
  sourcemeta::core::URI uri{"a:/.//b"};
  uri.canonicalize();
  const auto recomposed{uri.recompose()};
  EXPECT_EQ(recomposed, "a:/.//b");
  const sourcemeta::core::URI reparsed{recomposed};
  EXPECT_FALSE(reparsed.host().has_value());
}

// The empty fragment is optional
TEST(example_2) {
  sourcemeta::core::URI uri{"http://example.com/foo#"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo");
}

// Explicit 80 port for http scheme
TEST(example_3) {
  sourcemeta::core::URI uri{"http://example.com:80/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/default/port");
}

// Explicit 443 port for https scheme
TEST(example_4) {
  sourcemeta::core::URI uri{"https://example.com:443/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/default/port");
}

TEST(example_5) {
  sourcemeta::core::URI uri{"https://example.com:80/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com:80/default/port");
}

TEST(example_6) {
  sourcemeta::core::URI uri{"http://example.com:443/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com:443/default/port");
}

// Schemes are case insensitive
TEST(example_7) {
  sourcemeta::core::URI uri{"hTtP://example.com/case-insensitive-scheme"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-insensitive-scheme");
}

// Hosts are case insensitive
TEST(example_8) {
  sourcemeta::core::URI uri{"http://exAmpLe.com/case-insensitive-host"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-insensitive-host");
}

// A percent-encoded uppercase letter in the host folds to lowercase
TEST(percent_encoded_uppercase_host_letter) {
  sourcemeta::core::URI uri{"http://%41EXAMPLE.com/path"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://aexample.com/path");
}

// Paths are case sensitive
TEST(example_9) {
  sourcemeta::core::URI uri{"hTtP://exAmpLe.com/case-SENSITIVE-path"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-SENSITIVE-path");
}

TEST(example_10) {
  sourcemeta::core::URI uri{"urn:example:schema"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "urn:example:schema");
}

TEST(example_relative_1) {
  sourcemeta::core::URI uri{"../foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../foo");
}

TEST(example_relative_2) {
  sourcemeta::core::URI uri{"./foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "foo");
}

TEST(example_relative_3) {
  sourcemeta::core::URI uri{"foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "foo");
}

TEST(example_relative_4) {
  sourcemeta::core::URI uri{"foo/bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "foo/bar");
}

TEST(example_relative_6) {
  sourcemeta::core::URI uri{"/foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "/foo");
}

TEST(example_12) {
  sourcemeta::core::URI uri{"#foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(example_13) {
  sourcemeta::core::URI uri{
      "tag:bowtie.report,2023-11:referencing-suite-tag-uris-id"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(),
            "tag:bowtie.report,2023-11:referencing-suite-tag-uris-id");
}

TEST(example_14) {
  sourcemeta::core::URI uri{"http://example.com/escapes/a%c2%b1b"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/escapes/a%C2%B1b");
}

TEST(example_15) {
  sourcemeta::core::URI uri{"http://example.com/unreserved/%7Efoo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/unreserved/~foo");
}

TEST(example_16) {
  sourcemeta::core::URI uri{"http://example.com/foo/"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/");
}

TEST(example_17) {
  sourcemeta::core::URI uri{"http://example.com/"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/");
}

TEST(empty_fragment) {
  sourcemeta::core::URI uri{"#"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(static_from_string) {
  const std::string uri{"hTtP://exAmpLe.com:80/TEST"};
  const auto result{sourcemeta::core::URI::canonicalize(uri)};
  EXPECT_EQ(result, "http://example.com/TEST");
}

TEST(path_with_dotdot_absolute) {
  sourcemeta::core::URI uri{"http://example.com/foo/bar/../baz"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/baz");
}

TEST(path_with_multiple_dotdot_absolute) {
  sourcemeta::core::URI uri{"http://example.com/foo/bar/qux/../../baz"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/baz");
}

TEST(path_with_dotdot_at_root) {
  sourcemeta::core::URI uri{"http://example.com/../foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo");
}

TEST(path_with_dot_segments) {
  sourcemeta::core::URI uri{"http://example.com/foo/./bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/bar");
}

TEST(path_with_mixed_dot_segments) {
  sourcemeta::core::URI uri{"http://example.com/foo/./bar/../baz"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo/baz");
}

TEST(relative_path_with_dotdot) {
  sourcemeta::core::URI uri{"../foo/bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../foo/bar");
}

TEST(relative_path_with_dotdot_middle) {
  sourcemeta::core::URI uri{"foo/../bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "bar");
}

TEST(relative_path_multiple_dotdot) {
  sourcemeta::core::URI uri{"../../foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../foo");
}

TEST(relative_path_only_dotdot) {
  sourcemeta::core::URI uri{".."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../");
}

TEST(relative_path_two_dotdot_no_segment) {
  sourcemeta::core::URI uri{"../.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../");
}

TEST(relative_path_three_dotdot_no_segment) {
  sourcemeta::core::URI uri{"../../.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../../");
}

TEST(absolute_path_with_dotdot) {
  sourcemeta::core::URI uri{"/foo/bar/../baz"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "/foo/baz");
}

TEST(absolute_path_excess_dotdot) {
  sourcemeta::core::URI uri{"/foo/../../bar"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "/bar");
}

TEST(empty_uri_default_constructor) {
  sourcemeta::core::URI uri;
  uri.canonicalize();
  EXPECT_TRUE(uri.empty());
  EXPECT_EQ(uri.recompose(), "");
}

TEST(empty_uri_string_constructor) {
  sourcemeta::core::URI uri{""};
  uri.canonicalize();
  EXPECT_TRUE(uri.empty());
  EXPECT_EQ(uri.recompose(), "");
}

// Inspired from https://cr.openjdk.org/~dfuchs/writeups/updating-uri/

TEST(rfc3986_1) {
  sourcemeta::core::URI uri{"s://h/a/../../b"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "s://h/b");
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/master/test/test.cpp#L1438

TEST(rfc3986_2) {
  sourcemeta::core::URI uri{"eXAMPLE://a/./b/../b/%63/%7bfoo%7d"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "example://a/b/c/%7Bfoo%7D");
}

TEST(percent_encoding) {
  sourcemeta::core::URI uri{"http://examp%4Ce.com/"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/");
}

TEST(dot_segments_percent_encoded) {
  sourcemeta::core::URI uri{"http://example.com/a/b/%2E%2E/"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/a/");
}

TEST(case_normalization) {
  sourcemeta::core::URI uri{"http://user:pass@SOMEHOST.COM:123"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://user:pass@somehost.com:123");
}

TEST(complex_case) {
  sourcemeta::core::URI uri{"HTTP://a:b@HOST:123/./1/2/../%41?abc#def"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a:b@host:123/1/A?abc#def");
}

TEST(component_aware_decode) {
  sourcemeta::core::URI uri{"http://example.com/%3a%3b%2f?foo%3dbar#baz%2fqux"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(),
            "http://example.com/%3A%3B%2F?foo%3Dbar#baz%2Fqux");
}

TEST(fragment_encoded_colon) {
  sourcemeta::core::URI uri{
      "https://www.example.com#/$defs/https%3A~1~1example.com~1schema/type"};
  uri.canonicalize();
  EXPECT_EQ(
      uri.recompose(),
      "https://www.example.com#/$defs/https%3A~1~1example.com~1schema/type");
}

TEST(relative_path_no_canonicalize) {
  const sourcemeta::core::URI uri{"../../abc"};
  EXPECT_EQ(uri.recompose(), "../../abc");
}

TEST(relative_path_dotdot_trailing) {
  sourcemeta::core::URI uri{"../../abc/.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../");
}

TEST(relative_path_dotdot_middle_with_segment) {
  sourcemeta::core::URI uri{"../../abc/../def"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../def");
}

TEST(relative_path_cancel_to_empty) {
  sourcemeta::core::URI uri{"abc/.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(relative_path_cancel_to_empty_trailing_slash) {
  sourcemeta::core::URI uri{"abc/../"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(relative_path_multiple_dotdot_with_dot) {
  sourcemeta::core::URI uri{"../../abc/./def"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "../../abc/def");
}

TEST(relative_path_leading_dot_removed) {
  sourcemeta::core::URI uri{"./def"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "def");
}

TEST(relative_path_trailing_dot) {
  sourcemeta::core::URI uri{"def/."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "def/");
}

TEST(relative_path_colon_ambiguity) {
  const sourcemeta::core::URI uri{"./abc:def"};
  EXPECT_EQ(uri.recompose(), "./abc:def");
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/master/test/test.cpp#L1531

TEST(path_multiple_dotdot_to_root) {
  sourcemeta::core::URI uri{"http://a/b/c/../../.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(path_mixed_dotdot_to_root) {
  sourcemeta::core::URI uri{"http://a/b/../c/../.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(path_single_dotdot_at_root) {
  sourcemeta::core::URI uri{"http://a/.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(path_dotdot_only_no_canonicalize) {
  const sourcemeta::core::URI uri{"/.."};
  EXPECT_EQ(uri.recompose(), "/..");
}

TEST(path_empty_segments_after_dotdot) {
  sourcemeta::core::URI uri{"http://a/..///"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a///");
}

TEST(path_empty_segments_with_trailing_dotdot) {
  sourcemeta::core::URI uri{"http://a/..///.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://a//");
}

TEST(relative_path_collapse_completely) {
  sourcemeta::core::URI uri{"a/b/c/../../.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(relative_path_nested_collapse) {
  sourcemeta::core::URI uri{"a/b/../../c/.."};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "");
}

TEST(ipv6_uppercase_to_lowercase) {
  sourcemeta::core::URI uri{"http://[1080::8:800:200C:417A]/foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://[1080::8:800:200c:417a]/foo");
}

TEST(ipv6_mixed_case_to_lowercase) {
  sourcemeta::core::URI uri{"http://[::FFFF:129.144.52.38]:80/index.html"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://[::ffff:129.144.52.38]/index.html");
}

TEST(ipv6_hex_uppercase_to_lowercase) {
  sourcemeta::core::URI uri{"http://[2010:836B:4179::836B:4179]"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://[2010:836b:4179::836b:4179]");
}

TEST(iri_preserves_literal_ucschar) {
  auto uri{sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/caf\xC3\xA9");
}

TEST(iri_decodes_percent_encoded_ucschar) {
  // RFC 3987 Section 5.3.2.3: percent-encoded octets that form characters an
  // IRI may carry unencoded are decoded to their literal form
  auto uri{sourcemeta::core::URI::from_iri("https://example.com/caf%C3%A9")};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/caf\xC3\xA9");
}

TEST(iri_removes_dot_segments_preserving_ucschar) {
  auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/a/../caf\xC3\xA9")};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/caf\xC3\xA9");
}

TEST(iri_leaves_private_use_percent_encoded) {
  // Private-use characters are not among those an IRI may carry unencoded, so
  // they must stay percent-encoded even though allowed literally in the query
  auto uri{sourcemeta::core::URI::from_iri("https://example.com/?x=%EE%80%80")};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/?x=%EE%80%80");
}

TEST(iri_removes_default_port_preserving_host_ucschar) {
  auto uri{sourcemeta::core::URI::from_iri(
      "https://\xE4\xBE\x8B\xE3\x81\x88.jp:443/")};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://\xE4\xBE\x8B\xE3\x81\x88.jp/");
}

TEST(static_rejects_non_ascii) {
  // The static entry point parses as a plain URI, so IRI input is rejected
  try {
    sourcemeta::core::URI::canonicalize("https://example.com/caf\xC3\xA9");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 24);
  }
}

TEST(iri_decodes_percent_encoded_lower_boundary) {
  // U+00A0 is the lowest non-ASCII character an IRI may carry unencoded; its
  // percent-encoded form must decode to its literal bytes
  auto uri{sourcemeta::core::URI::from_iri("https://example.com/%C2%A0")};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/\xC2\xA0");
}

TEST(iri_leaves_noncharacter_percent_encoded) {
  // U+FFFF is a valid codepoint but not one an IRI may carry unencoded, so it
  // must stay percent-encoded
  auto uri{sourcemeta::core::URI::from_iri("https://example.com/%EF%BF%BF")};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/%EF%BF%BF");
}

TEST(iri_leaves_incomplete_percent_sequence_encoded) {
  // A percent-encoded lead byte not followed by its continuation is not a valid
  // character and must remain encoded
  auto uri{sourcemeta::core::URI::from_iri("https://example.com/%C3x")};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/%C3x");
}
