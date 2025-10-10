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

TEST(URI_query, rfc3986_query_with_slash) {
  const sourcemeta::core::URI uri{"http://example.com/path?query/with/slashes"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "query/with/slashes");
}

TEST(URI_query, rfc3986_query_with_question_mark) {
  const sourcemeta::core::URI uri{"http://example.com/path?query=value?more"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "query=value?more");
}

TEST(URI_query, rfc3986_query_with_colon) {
  const sourcemeta::core::URI uri{"http://example.com/path?query:with:colons"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "query:with:colons");
}

TEST(URI_query, rfc3986_query_with_at_sign) {
  const sourcemeta::core::URI uri{"http://example.com/path?query@with@at"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "query@with@at");
}

TEST(URI_query, rfc3986_query_percent_encoded) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?query%20with%20spaces"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "query%20with%20spaces");
}

TEST(URI_query, rfc3986_query_with_fragment) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?query=value#fragment"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "query=value");
}

TEST(URI_query, rfc3986_empty_query) {
  const sourcemeta::core::URI uri{"http://example.com/path?"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "");
}

TEST(URI_query, rfc3986_query_only_ampersands) {
  const sourcemeta::core::URI uri{"http://example.com/path?&&&"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "&&&");
}

TEST(URI_query, rfc3986_query_with_subdelims) {
  const sourcemeta::core::URI uri{"http://example.com/path?!$&'()*+,;="};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "!$&'()*+,;=");
}

TEST(URI_query, rfc3986_query_percent_encoded_brackets) {
  const sourcemeta::core::URI uri{
      "http://example.com/path?items%5B0%5D=a&items%5B1%5D=b"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_TRUE(uri.query().value().find("items") != std::string::npos);
}

TEST(URI_query, rfc3986_query_no_value) {
  const sourcemeta::core::URI uri{"http://example.com/path?key"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "key");
}

TEST(URI_query, rfc3986_query_multiple_equals) {
  const sourcemeta::core::URI uri{"http://example.com/path?key=value=extra"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "key=value=extra");
}
