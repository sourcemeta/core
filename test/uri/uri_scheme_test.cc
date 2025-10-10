#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_scheme, https_example_url) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "https");
}

TEST(URI_scheme, http_example_url) {
  const sourcemeta::core::URI uri{"http://example.com/foo"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "http");
}

TEST(URI_scheme, relative_url) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.scheme().has_value());
}

TEST(URI_scheme, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "urn");
}

TEST(URI_scheme, urn_with_fragment) {
  const sourcemeta::core::URI uri{"urn:example:schema#foo"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "urn");
}

TEST(URI_scheme, rfc3986_scheme_case_insensitive) {
  const sourcemeta::core::URI uri1{"HTTP://example.com"};
  const sourcemeta::core::URI uri2{"http://example.com"};
  EXPECT_EQ(uri1.scheme().value(), "http");
  EXPECT_EQ(uri2.scheme().value(), "http");
  EXPECT_EQ(uri1.scheme().value(), uri2.scheme().value());
}

TEST(URI_scheme, rfc3986_scheme_with_plus) {
  const sourcemeta::core::URI uri{"svn+ssh://example.com"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "svn+ssh");
}

TEST(URI_scheme, rfc3986_scheme_with_dash) {
  const sourcemeta::core::URI uri{"coap+tcp://example.com"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "coap+tcp");
}

TEST(URI_scheme, rfc3986_scheme_with_dot) {
  const sourcemeta::core::URI uri{"vnd.example://example.com"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "vnd.example");
}

TEST(URI_scheme, rfc3986_scheme_single_letter) {
  const sourcemeta::core::URI uri{"a://example.com"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "a");
}

TEST(URI_scheme, rfc3986_ftp_scheme) {
  const sourcemeta::core::URI uri{"ftp://ftp.example.com/file.txt"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "ftp");
}

TEST(URI_scheme, rfc3986_file_scheme) {
  const sourcemeta::core::URI uri{"file:///path/to/file"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "file");
}

TEST(URI_scheme, rfc3986_mailto_scheme) {
  const sourcemeta::core::URI uri{"mailto:user@example.com"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "mailto");
}

TEST(URI_scheme, rfc3986_tel_scheme) {
  const sourcemeta::core::URI uri{"tel:+1-816-555-1212"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "tel");
}

TEST(URI_scheme, rfc3986_data_scheme) {
  const sourcemeta::core::URI uri{"data:text/plain;base64,SGVsbG8="};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "data");
}

TEST(URI_scheme, rfc3986_tag_scheme) {
  const sourcemeta::core::URI uri{"tag:example.com,2023:resource"};
  EXPECT_TRUE(uri.scheme().has_value());
  EXPECT_EQ(uri.scheme().value(), "tag");
}
