#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_host, https_example_url) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
}

TEST(URI_host, https_example_url_slash) {
  const sourcemeta::core::URI uri{"https://example.com/"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
}

TEST(URI_host, https_example_url_with_port) {
  const sourcemeta::core::URI uri{"https://example.com:8000/foo"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
}

TEST(URI_host, relative_url) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.host().has_value());
}

TEST(URI_host, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.host().has_value());
}
