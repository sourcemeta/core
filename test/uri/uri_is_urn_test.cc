#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/uri.h>

TEST(URI_is_urn, urn_1) {
  const sourcemeta::jsontoolkit::URI uri{"urn:example:schema"};
  EXPECT_TRUE(uri.is_urn());
}

TEST(URI_is_urn, https_url) {
  const sourcemeta::jsontoolkit::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_urn());
}

TEST(URI_is_urn, relative) {
  const sourcemeta::jsontoolkit::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_urn());
}

TEST(URI_is_urn, tag) {
  const sourcemeta::jsontoolkit::URI uri{"tag:yaml.org,2002:int"};
  EXPECT_FALSE(uri.is_urn());
}
