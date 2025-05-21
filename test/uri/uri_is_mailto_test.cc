#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_is_mailto, success) {
  const sourcemeta::core::URI uri{"mailto:jdoe@mail.com"};
  EXPECT_TRUE(uri.is_mailto());
}

TEST(URI_is_mailto, https_url) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_mailto());
}

TEST(URI_is_mailto, relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_mailto());
}

TEST(URI_is_mailto, tag) {
  const sourcemeta::core::URI uri{"tag:yaml.org,2002:int"};
  EXPECT_FALSE(uri.is_mailto());
}
