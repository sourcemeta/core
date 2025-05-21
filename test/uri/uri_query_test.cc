#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_query, https_example_url_no_query) {
  const sourcemeta::core::URI uri{"https://example.com/test"};
  EXPECT_FALSE(uri.query().has_value());
}

TEST(URI_query, https_example_with_query_single) {
  const sourcemeta::core::URI uri{"https://example.com/test?foo=bar"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "foo=bar");
}

// RFC 3986 itself does not understand the query part of a URI as a list of
// key/value pairs.
TEST(URI_query, https_example_with_query_double) {
  const sourcemeta::core::URI uri{"https://example.com/test?foo=bar&bar=baz"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "foo=bar&bar=baz");
}
