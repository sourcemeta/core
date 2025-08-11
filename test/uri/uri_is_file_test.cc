#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_is_file, mailto) {
  const sourcemeta::core::URI uri{"mailto:jdoe@mail.com"};
  EXPECT_FALSE(uri.is_file());
}

TEST(URI_is_file, https_url) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_file());
}

TEST(URI_is_file, relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_file());
}

TEST(URI_is_file, tag) {
  const sourcemeta::core::URI uri{"tag:yaml.org,2002:int"};
  EXPECT_FALSE(uri.is_file());
}

TEST(URI_is_file, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_TRUE(uri.is_file());
}

TEST(URI_is_file, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_TRUE(uri.is_file());
}
