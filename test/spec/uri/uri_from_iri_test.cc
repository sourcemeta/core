#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(parses_and_round_trips_unicode) {
  const auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  EXPECT_EQ(uri.recompose(), "https://example.com/caf\xC3\xA9");
}

TEST(accepts_ascii_like_uri) {
  const auto uri{sourcemeta::core::URI::from_iri("https://example.com/foo")};
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");
}

TEST(rejects_private_use_in_path) {
  // RFC 3987 allows private-use characters only in the query
  try {
    sourcemeta::core::URI::from_iri("https://example.com/\xEE\x80\x80");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 21);
  }
}

TEST(rejects_invalid_utf8) {
  try {
    sourcemeta::core::URI::from_iri("https://example.com/\xC3");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 21);
  }
}

TEST(equals_plain_uri_with_same_components) {
  // The parse mode is not part of a URI's value, so equality and ordering
  // ignore it and stay consistent with each other
  const sourcemeta::core::URI plain{"https://example.com/foo"};
  const auto iri{sourcemeta::core::URI::from_iri("https://example.com/foo")};
  EXPECT_EQ(plain, iri);
  EXPECT_FALSE(plain < iri);
  EXPECT_FALSE(iri < plain);
}

TEST(empty_authority) {
  const auto uri{sourcemeta::core::URI::from_iri("http://")};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "");
  EXPECT_FALSE(uri.path().has_value());
}

TEST(ip_literal_host) {
  const auto uri{sourcemeta::core::URI::from_iri("http://[::1]/foo")};
  EXPECT_EQ(uri.host().value(), "::1");
  EXPECT_EQ(uri.recompose(), "http://[::1]/foo");
}

TEST(host_terminated_by_a_fragment) {
  const auto uri{sourcemeta::core::URI::from_iri("http://example.com#foo")};
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_EQ(uri.fragment().value(), "foo");
  EXPECT_FALSE(uri.path().has_value());
}

TEST(host_terminated_by_a_query) {
  const auto uri{sourcemeta::core::URI::from_iri("http://example.com?foo")};
  EXPECT_EQ(uri.host().value(), "example.com");
  EXPECT_EQ(uri.query().value().raw(), "foo");
  EXPECT_FALSE(uri.path().has_value());
}

TEST(host_terminated_by_a_userinfo) {
  const auto uri{sourcemeta::core::URI::from_iri("http://user@example.com/")};
  EXPECT_EQ(uri.userinfo().value(), "user");
  EXPECT_EQ(uri.host().value(), "example.com");
}

TEST(host_with_a_percent_escape) {
  const auto uri{sourcemeta::core::URI::from_iri("http://ex%41mple.com/")};
  EXPECT_EQ(uri.host().value(), "exAmple.com");
}

TEST(host_with_a_sub_delimiter) {
  const auto uri{sourcemeta::core::URI::from_iri("http://ex&ample.com/")};
  EXPECT_EQ(uri.host().value(), "ex&ample.com");
}

TEST(rejects_a_host_character_outside_the_grammar) {
  try {
    sourcemeta::core::URI::from_iri("http://ex<ample.com/");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 10);
  }
}

TEST(userinfo_with_a_percent_escape) {
  const auto uri{sourcemeta::core::URI::from_iri("http://us%41er@e/")};
  EXPECT_EQ(uri.userinfo().value(), "usAer");
}

TEST(userinfo_with_a_colon) {
  const auto uri{sourcemeta::core::URI::from_iri("http://user:pass@e/")};
  EXPECT_EQ(uri.userinfo().value(), "user:pass");
  EXPECT_EQ(uri.host().value(), "e");
}

// RFC 3987 leaves the private-use characters out of iuserinfo, so the reader
// gives up on a userinfo and the character is refused as part of the host
TEST(rejects_a_private_use_character_before_an_at_sign) {
  try {
    sourcemeta::core::URI::from_iri("http://u\xEE\x80\x80@e/");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 9);
  }
}

TEST(empty_port) {
  const auto uri{sourcemeta::core::URI::from_iri("http://e:/foo")};
  EXPECT_FALSE(uri.port().has_value());
  EXPECT_EQ(uri.host().value(), "e");
  EXPECT_EQ(uri.path().value(), "/foo");
}

// The port this class reads is bounded by the width it answers with, which is
// narrower than the unbounded `port = *DIGIT` of RFC 3986 Section 3.2.3
TEST(rejects_a_port_wider_than_thirty_two_bits) {
  try {
    sourcemeta::core::URI::from_iri("http://e:4294967296/foo");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 10);
  }
}

// RFC 3986 Section 3.2 writes `authority = [ userinfo "@" ] host [ ":" port
// ]`, where neither the host nor the port can carry an at sign
TEST(rejects_a_second_at_sign_in_the_authority) {
  try {
    sourcemeta::core::URI::from_iri("http://a@b@c/");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 11);
  }
}

// RFC 3986 Section 3: `hier-part = "//" authority path-abempty`, so what
// follows an authority opens a path, a query or a fragment
TEST(rejects_a_character_after_the_authority) {
  try {
    sourcemeta::core::URI::from_iri("http://e:80x/");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 12);
  }
}

TEST(query_with_a_slash) {
  const auto uri{sourcemeta::core::URI::from_iri("http://e/?a/b")};
  EXPECT_EQ(uri.query().value().raw(), "a/b");
}

TEST(query_with_a_question_mark) {
  const auto uri{sourcemeta::core::URI::from_iri("http://e/?a?b")};
  EXPECT_EQ(uri.query().value().raw(), "a?b");
}

TEST(fragment_with_a_slash_and_a_question_mark) {
  const auto uri{sourcemeta::core::URI::from_iri("http://e/#a/b?c")};
  EXPECT_EQ(uri.fragment().value(), "a/b?c");
}

TEST(fragment_with_a_percent_escape) {
  const auto uri{sourcemeta::core::URI::from_iri("http://e/#a%41b")};
  EXPECT_EQ(uri.fragment().value(), "aAb");
}

TEST(rejects_a_fragment_character_outside_the_grammar) {
  try {
    sourcemeta::core::URI::from_iri("http://e/#a<b");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 12);
  }
}

TEST(rejects_a_query_character_outside_the_grammar) {
  try {
    sourcemeta::core::URI::from_iri("http://e/?a<b");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 12);
  }
}

// RFC 3986 Section 4.2: "A path segment that contains a colon character cannot
// be used as the first segment of a relative-path reference, as it would be
// mistaken for a scheme name"
TEST(rejects_a_colon_in_the_first_segment_of_a_relative_reference) {
  try {
    sourcemeta::core::URI::from_iri("1a:b");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 3);
  }
}

TEST(a_colon_after_the_first_segment_of_a_relative_reference) {
  const auto uri{sourcemeta::core::URI::from_iri("1a/b:c")};
  EXPECT_FALSE(uri.scheme().has_value());
  EXPECT_EQ(uri.path().value(), "1a/b:c");
}

TEST(rejects_a_colon_before_the_first_slash_of_a_relative_reference) {
  try {
    sourcemeta::core::URI::from_iri("1a:b/c");
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 3);
  }
}
