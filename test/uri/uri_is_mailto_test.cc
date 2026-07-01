#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(success) {
  const sourcemeta::core::URI uri{"mailto:jdoe@mail.com"};
  EXPECT_TRUE(uri.is_mailto());
}

TEST(https_url) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_mailto());
}

TEST(relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_mailto());
}

TEST(tag) {
  const sourcemeta::core::URI uri{"tag:yaml.org,2002:int"};
  EXPECT_FALSE(uri.is_mailto());
}

TEST(iri_mailto) {
  const auto uri{sourcemeta::core::URI::from_iri(
      "mailto:user@\xE4\xBE\x8B\xE3\x81\x88.jp")};
  EXPECT_TRUE(uri.is_mailto());
}
