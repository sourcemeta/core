#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(empty_default_constructor) {
  const sourcemeta::core::URI uri;
  EXPECT_TRUE(uri.empty());
}

TEST(empty_string_constructor) {
  const sourcemeta::core::URI uri{""};
  EXPECT_TRUE(uri.empty());
}

TEST(hier_only) {
  const sourcemeta::core::URI uri{"example.com"};
  EXPECT_FALSE(uri.empty());
}

TEST(path_only) {
  const sourcemeta::core::URI uri{"/foo/bar"};
  EXPECT_FALSE(uri.empty());
}

TEST(query_only) {
  const sourcemeta::core::URI uri{"?foo=bar"};
  EXPECT_FALSE(uri.empty());
}

TEST(fragment_only) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_FALSE(uri.empty());
}

TEST(fragment_empty_only) {
  const sourcemeta::core::URI uri{"#"};
  EXPECT_FALSE(uri.empty());
}

TEST(urn_path) {
  const sourcemeta::core::URI uri{"urn:example:animal:ferret:nose"};
  EXPECT_FALSE(uri.empty());
}

TEST(iri_not_empty) {
  const auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  EXPECT_FALSE(uri.empty());
}
