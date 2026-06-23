#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_from_iri, parses_and_round_trips_unicode) {
  const auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  EXPECT_EQ(uri.recompose(), "https://example.com/caf\xC3\xA9");
}

TEST(URI_from_iri, accepts_ascii_like_uri) {
  const auto uri{sourcemeta::core::URI::from_iri("https://example.com/foo")};
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");
}

TEST(URI_from_iri, is_internationalized) {
  const auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  EXPECT_TRUE(uri.is_internationalized());
}

TEST(URI_from_iri, plain_uri_is_not_internationalized) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_FALSE(uri.is_internationalized());
}

TEST(URI_from_iri, internationalized_survives_copy) {
  const auto original{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  const sourcemeta::core::URI copy{original};
  EXPECT_TRUE(copy.is_internationalized());
  EXPECT_EQ(copy.recompose(), "https://example.com/caf\xC3\xA9");
}

TEST(URI_from_iri, rejects_private_use_in_path) {
  // RFC 3987 allows private-use characters only in the query
  EXPECT_THROW(
      sourcemeta::core::URI::from_iri("https://example.com/\xEE\x80\x80"),
      sourcemeta::core::URIParseError);
}

TEST(URI_from_iri, rejects_invalid_utf8) {
  EXPECT_THROW(sourcemeta::core::URI::from_iri("https://example.com/\xC3"),
               sourcemeta::core::URIParseError);
}

TEST(URI_from_iri, equals_plain_uri_with_same_components) {
  // The parse mode is not part of a URI's value, so equality and ordering
  // ignore it and stay consistent with each other
  const sourcemeta::core::URI plain{"https://example.com/foo"};
  const auto iri{sourcemeta::core::URI::from_iri("https://example.com/foo")};
  EXPECT_EQ(plain, iri);
  EXPECT_FALSE(plain < iri);
  EXPECT_FALSE(iri < plain);
}
