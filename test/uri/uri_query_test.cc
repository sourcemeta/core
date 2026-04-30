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
  // Space is not unreserved, so %20 must not be decoded
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
  // [ and ] are gen-delims (reserved), must not be decoded
  EXPECT_EQ(uri.query().value(), "items%5B0%5D=a&items%5B1%5D=b");
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

TEST(URI_query, encoded_ampersand_preserved) {
  const sourcemeta::core::URI uri{"http://example.com/path?foo%26bar=baz"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "foo%26bar=baz");
}

TEST(URI_query, encoded_hash_not_fragment_delimiter) {
  const sourcemeta::core::URI uri{"http://example.com/path?foo%23bar"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "foo%23bar");
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_query, encoded_equals_preserved) {
  const sourcemeta::core::URI uri{"http://example.com/path?key%3Dvalue"};
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "key%3Dvalue");
}

TEST(URI_query_setter, set_on_uri_without_query) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(URI_query_setter, replace_existing_query) {
  sourcemeta::core::URI uri{"https://example.com/path?old=value"};
  uri.query("foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(URI_query_setter, clear_with_empty_string) {
  sourcemeta::core::URI uri{"https://example.com/path?foo=bar"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path");
}

TEST(URI_query_setter, clear_when_already_absent) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path");
}

TEST(URI_query_setter, leading_question_mark_stripped) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("?foo=bar");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "foo=bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=bar");
}

TEST(URI_query_setter, clear_preserves_fragment) {
  sourcemeta::core::URI uri{"https://example.com/path?foo=bar#section"};
  uri.query("");
  EXPECT_FALSE(uri.query().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/path#section");
}

TEST(URI_query_setter, percent_encoding_applied_on_recompose) {
  sourcemeta::core::URI uri{"https://example.com/path"};
  uri.query("foo=hello world");
  EXPECT_TRUE(uri.query().has_value());
  EXPECT_EQ(uri.query().value(), "foo=hello world");
  EXPECT_EQ(uri.recompose(), "https://example.com/path?foo=hello%20world");
}

TEST(URI_query_setter, returns_reference_for_chaining) {
  sourcemeta::core::URI uri{"https://example.com"};
  uri.query("a=1").fragment("section");
  EXPECT_EQ(uri.recompose(), "https://example.com?a=1#section");
}
