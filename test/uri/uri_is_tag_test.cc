#include <sourcemeta/core/test.h>

#include <sourcemeta/core/uri.h>

TEST(tag) {
  const sourcemeta::core::URI uri{"tag:yaml.org,2002:int"};
  EXPECT_TRUE(uri.is_tag());
}

TEST(urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.is_tag());
}

TEST(https_url) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_tag());
}

TEST(relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_tag());
}

TEST(iri_tag) {
  const auto uri{
      sourcemeta::core::URI::from_iri("tag:example.com,2024:caf\xC3\xA9")};
  EXPECT_TRUE(uri.is_tag());
}
