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
