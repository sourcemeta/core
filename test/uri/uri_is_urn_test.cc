#include <sourcemeta/core/test.h>

#include <sourcemeta/core/uri.h>

TEST(urn_1) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_TRUE(uri.is_urn());
}

TEST(https_url) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_urn());
}

TEST(relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_urn());
}

TEST(tag) {
  const sourcemeta::core::URI uri{"tag:yaml.org,2002:int"};
  EXPECT_FALSE(uri.is_urn());
}

TEST(iri_urn) {
  const auto uri{sourcemeta::core::URI::from_iri("urn:example:caf\xC3\xA9")};
  EXPECT_TRUE(uri.is_urn());
}
