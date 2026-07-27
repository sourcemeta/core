#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(mailto) {
  const sourcemeta::core::URI uri{"mailto:jdoe@mail.com"};
  EXPECT_FALSE(uri.is_file());
}

TEST(https_url) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_file());
}

TEST(relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_file());
}

TEST(tag) {
  const sourcemeta::core::URI uri{"tag:yaml.org,2002:int"};
  EXPECT_FALSE(uri.is_file());
}

TEST(unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_TRUE(uri.is_file());
}

TEST(windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_TRUE(uri.is_file());
}

TEST(iri_file) {
  const auto uri{sourcemeta::core::URI::from_iri("file:///caf\xC3\xA9.txt")};
  EXPECT_TRUE(uri.is_file());
}
