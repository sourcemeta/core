#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_is_absolute, example_url) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, relative_url) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, slash) {
  // The is a relative URI, even if the path is absolute.
  const sourcemeta::core::URI uri{"/foo"};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_http_scheme) {
  const sourcemeta::core::URI uri{"http://example.com/path"};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_https_scheme) {
  const sourcemeta::core::URI uri{"https://example.com/path"};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_ftp_scheme) {
  const sourcemeta::core::URI uri{"ftp://ftp.example.com/file"};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_file_scheme) {
  const sourcemeta::core::URI uri{"file:///path/to/file"};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_mailto_scheme) {
  const sourcemeta::core::URI uri{"mailto:user@example.com"};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_tel_scheme) {
  const sourcemeta::core::URI uri{"tel:+1-555-1212"};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_data_scheme) {
  const sourcemeta::core::URI uri{"data:text/plain;base64,SGVsbG8="};
  EXPECT_TRUE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_relative_path) {
  const sourcemeta::core::URI uri{"relative/path"};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_absolute_path) {
  const sourcemeta::core::URI uri{"/absolute/path"};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_network_path) {
  const sourcemeta::core::URI uri{"//example.com/path"};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_query_only) {
  const sourcemeta::core::URI uri{"?query=value"};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_fragment_only) {
  const sourcemeta::core::URI uri{"#fragment"};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_empty_uri) {
  const sourcemeta::core::URI uri{""};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_dot_relative) {
  const sourcemeta::core::URI uri{"."};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_dotdot_relative) {
  const sourcemeta::core::URI uri{".."};
  EXPECT_FALSE(uri.is_absolute());
}

TEST(URI_is_absolute, rfc3986_scheme_with_fragment) {
  const sourcemeta::core::URI uri{"http://example.com/path#fragment"};
  EXPECT_TRUE(uri.is_absolute());
}
