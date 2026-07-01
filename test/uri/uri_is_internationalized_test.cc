#include <sourcemeta/core/test.h>

#include <sourcemeta/core/uri.h>

#include <sstream> // std::istringstream

TEST(from_iri_non_ascii) {
  const auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  EXPECT_TRUE(uri.is_internationalized());
}

TEST(from_iri_ascii) {
  const auto uri{sourcemeta::core::URI::from_iri("https://example.com/foo")};
  EXPECT_TRUE(uri.is_internationalized());
}

TEST(default_constructor) {
  const sourcemeta::core::URI uri;
  EXPECT_FALSE(uri.is_internationalized());
}

TEST(string_constructor) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_FALSE(uri.is_internationalized());
}

TEST(empty_string_constructor) {
  const sourcemeta::core::URI uri{""};
  EXPECT_FALSE(uri.is_internationalized());
}

TEST(stream_constructor) {
  std::istringstream input{"https://example.com/foo"};
  const sourcemeta::core::URI uri{input};
  EXPECT_FALSE(uri.is_internationalized());
}

TEST(from_fragment) {
  const auto uri{sourcemeta::core::URI::from_fragment("foo")};
  EXPECT_FALSE(uri.is_internationalized());
}

TEST(from_path) {
  const auto uri{sourcemeta::core::URI::from_path("/foo/bar")};
  EXPECT_FALSE(uri.is_internationalized());
}

TEST(survives_copy) {
  const auto original{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  const sourcemeta::core::URI copy{original};
  EXPECT_TRUE(copy.is_internationalized());
}

TEST(plain_uri_survives_copy) {
  const sourcemeta::core::URI original{"https://example.com/foo"};
  const sourcemeta::core::URI copy{original};
  EXPECT_FALSE(copy.is_internationalized());
}

TEST(plain_uri_stays_false_after_canonicalize) {
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.canonicalize();
  EXPECT_FALSE(uri.is_internationalized());
}

TEST(plain_uri_stays_false_after_resolve) {
  const sourcemeta::core::URI base{"https://example.com/dir/"};
  sourcemeta::core::URI reference{"file"};
  reference.resolve_from(base);
  EXPECT_FALSE(reference.is_internationalized());
}
