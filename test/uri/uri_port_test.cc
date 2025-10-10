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

TEST(URI_port, rfc3986_port_80) {
  const sourcemeta::core::URI uri{"http://example.com:80/path"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 80);
}

TEST(URI_port, rfc3986_port_443) {
  const sourcemeta::core::URI uri{"https://example.com:443/path"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 443);
}

TEST(URI_port, rfc3986_port_21) {
  const sourcemeta::core::URI uri{"ftp://ftp.example.com:21/file.txt"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 21);
}

TEST(URI_port, rfc3986_high_port_number) {
  const sourcemeta::core::URI uri{"http://example.com:65535/path"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 65535);
}

TEST(URI_port, rfc3986_port_with_ipv4) {
  const sourcemeta::core::URI uri{"http://192.168.1.1:3000/path"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 3000);
}

TEST(URI_port, rfc3986_port_with_ipv6) {
  const sourcemeta::core::URI uri{"http://[2001:db8::1]:9000/path"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 9000);
}

TEST(URI_port, rfc3986_port_single_digit) {
  const sourcemeta::core::URI uri{"http://example.com:8/path"};
  EXPECT_TRUE(uri.port().has_value());
  EXPECT_EQ(uri.port().value(), 8);
}

TEST(URI_port, rfc3986_no_port_http) {
  const sourcemeta::core::URI uri{"http://example.com/path"};
  EXPECT_FALSE(uri.port().has_value());
}

TEST(URI_port, rfc3986_no_port_https) {
  const sourcemeta::core::URI uri{"https://example.com/path"};
  EXPECT_FALSE(uri.port().has_value());
}

TEST(URI_port, rfc3986_no_port_ftp) {
  const sourcemeta::core::URI uri{"ftp://ftp.example.com/file.txt"};
  EXPECT_FALSE(uri.port().has_value());
}
