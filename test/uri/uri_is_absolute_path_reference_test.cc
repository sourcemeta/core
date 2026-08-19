#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string> // std::string

// RFC 3986 §4.2: "A relative reference that begins with a single slash
// character is termed an absolute-path reference"
TEST(is_absolute_path_reference_root) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/"));
}

TEST(is_absolute_path_reference_single_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/schemas"));
}

TEST(is_absolute_path_reference_several_segments) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_absolute_path_reference("/schemas/example"));
}

// RFC 3986 §3.3: segment = *pchar, so a trailing empty segment is well formed
TEST(is_absolute_path_reference_trailing_slash) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/schemas/"));
}

// RFC 3986 §4.2: a reference beginning with two slashes is a network-path
// reference, which resolves against another authority
TEST(is_absolute_path_reference_network_path) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_absolute_path_reference("//example.com/evil"));
}

TEST(is_absolute_path_reference_two_slashes_alone) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("//"));
}

TEST(is_absolute_path_reference_empty) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference(""));
}

// RFC 3986 §4.2: a reference not beginning with a slash is a relative-path
// reference
TEST(is_absolute_path_reference_relative_path) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("schemas"));
}

TEST(is_absolute_path_reference_absolute_uri) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference(
      "https://example.com/evil"));
}

TEST(is_absolute_path_reference_scheme_relative_without_slash) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_absolute_path_reference("https:/evil"));
}

// RFC 3986 §3.3: the backslash is not a pchar, and a browser folds it to a
// slash, which would make this a network-path reference
TEST(is_absolute_path_reference_backslash_after_slash) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_absolute_path_reference("/\\example.com"));
}

TEST(is_absolute_path_reference_leading_backslashes) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_absolute_path_reference("\\\\example.com"));
}

TEST(is_absolute_path_reference_backslash_within_a_segment) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a\\b"));
}

// RFC 3986 §4.2: relative-ref = relative-part [ "?" query ] [ "#" fragment ]
TEST(is_absolute_path_reference_with_a_query) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_absolute_path_reference("/schemas?page=2"));
}

TEST(is_absolute_path_reference_with_a_fragment) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_absolute_path_reference("/schemas#section"));
}

TEST(is_absolute_path_reference_with_a_query_and_a_fragment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference(
      "/schemas?page=2#section"));
}

TEST(is_absolute_path_reference_with_an_empty_query) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/schemas?"));
}

TEST(is_absolute_path_reference_with_an_empty_fragment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/schemas#"));
}

// RFC 3986 §3.4: query = *( pchar / "/" / "?" )
TEST(is_absolute_path_reference_query_admits_slashes_and_questions) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/a?b=/c?d=e"));
}

// RFC 3986 §3.5: fragment = *( pchar / "/" / "?" )
TEST(is_absolute_path_reference_fragment_admits_slashes_and_questions) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/a#b/c?d"));
}

// RFC 3986 §3.5: the number sign is not admitted within a fragment, so only
// the first one delimits
TEST(is_absolute_path_reference_second_number_sign) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a#b#c"));
}

// RFC 3986 §2.1: pct-encoded = "%" HEXDIG HEXDIG
TEST(is_absolute_path_reference_percent_encoded) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/a%20b"));
}

TEST(is_absolute_path_reference_percent_encoded_lowercase) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/a%2fb"));
}

TEST(is_absolute_path_reference_bare_percent) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a%"));
}

TEST(is_absolute_path_reference_truncated_percent_encoding) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a%2"));
}

TEST(is_absolute_path_reference_non_hexadecimal_percent_encoding) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a%zz"));
}

TEST(is_absolute_path_reference_percent_encoding_in_a_query) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/a?b=%20"));
}

TEST(is_absolute_path_reference_bare_percent_in_a_fragment) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a#%"));
}

// RFC 3986 §3.3: pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
TEST(is_absolute_path_reference_unreserved_characters) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/aZ9-._~"));
}

TEST(is_absolute_path_reference_sub_delimiters) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_absolute_path_reference("/a!$&'()*+,;="));
}

TEST(is_absolute_path_reference_colon_and_at_sign) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/a:b@c"));
}

TEST(is_absolute_path_reference_space) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a b"));
}

TEST(is_absolute_path_reference_control_character) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_absolute_path_reference(std::string{"/a\x01"
                                                                    "b"}));
}

TEST(is_absolute_path_reference_carriage_return) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a\r\nb"));
}

TEST(is_absolute_path_reference_delete_character) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a\x7f"));
}

TEST(is_absolute_path_reference_non_ascii) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a\xc3\xa9"));
}

// Every one of these is outside pchar, so a faithful validator refuses them
// where a permissive one would let them reach a Location header unencoded
TEST(is_absolute_path_reference_characters_outside_pchar) {
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a<b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a>b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a\"b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a{b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a}b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a|b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a^b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a`b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a[b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_absolute_path_reference("/a]b"));
}

// The check is syntactic and says nothing about where a reference resolves
// within the origin, so a dot-segment is well formed and stays the caller's
// concern
TEST(is_absolute_path_reference_dot_segments) {
  EXPECT_TRUE(sourcemeta::core::URI::is_absolute_path_reference("/.."));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_absolute_path_reference("/../../etc/passwd"));
}
