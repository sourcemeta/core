#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_port, https_example_url_no_port) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_FALSE(uri.port().has_value());
}

TEST(URI_port, https_example_url_443) {
  const sourcemeta::core::URI uri{"https://example.com:443/foo"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 443);
}

TEST(URI_port, http_example_url_8000) {
  const sourcemeta::core::URI uri{"http://example.com:8000/foo"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 8000);
}

TEST(URI_port, relative_url) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.port().has_value());
}

TEST(URI_port, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.port().has_value());
}
