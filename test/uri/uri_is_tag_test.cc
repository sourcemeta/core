#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_is_tag, tag) {
  const sourcemeta::core::URI uri{"tag:yaml.org,2002:int"};
  EXPECT_TRUE(uri.is_tag());
}

TEST(URI_is_tag, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.is_tag());
}

TEST(URI_is_tag, https_url) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_tag());
}

TEST(URI_is_tag, relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_tag());
}
