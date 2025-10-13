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

TEST(URI_host, rfc3986_ipv4_address) {
  const sourcemeta::core::URI uri{"http://192.168.1.1/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "192.168.1.1");
}

TEST(URI_host, rfc3986_ipv4_address_with_port) {
  const sourcemeta::core::URI uri{"http://192.168.1.1:8080/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "192.168.1.1");
}

TEST(URI_host, rfc3986_ipv6_address) {
  const sourcemeta::core::URI uri{"http://[2001:db8::1]/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "2001:db8::1");
}

TEST(URI_host, rfc3986_ipv6_address_with_port) {
  const sourcemeta::core::URI uri{"http://[2001:db8::1]:8080/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "2001:db8::1");
}

TEST(URI_host, rfc3986_ipv6_localhost) {
  const sourcemeta::core::URI uri{"http://[::1]/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "::1");
}

TEST(URI_host, rfc3986_host_case_insensitive) {
  const sourcemeta::core::URI uri1{"http://EXAMPLE.COM/path"};
  const sourcemeta::core::URI uri2{"http://example.com/path"};
  EXPECT_EQ(uri1.host().value(), "example.com");
  EXPECT_EQ(uri2.host().value(), "example.com");
  EXPECT_EQ(uri1.host().value(), uri2.host().value());
}

TEST(URI_host, rfc3986_host_with_hyphen) {
  const sourcemeta::core::URI uri{"http://my-example-host.com/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "my-example-host.com");
}

TEST(URI_host, rfc3986_host_with_numbers) {
  const sourcemeta::core::URI uri{"http://example123.com/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "example123.com");
}

TEST(URI_host, rfc3986_subdomain) {
  const sourcemeta::core::URI uri{"http://www.sub.example.com/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "www.sub.example.com");
}

TEST(URI_host, rfc3986_localhost) {
  const sourcemeta::core::URI uri{"http://localhost/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "localhost");
}

TEST(URI_host, rfc3986_empty_host_with_authority) {
  const sourcemeta::core::URI uri{"file:///path/to/file"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "");
}

TEST(URI_host, rfc3986_percent_encoded_host) {
  const sourcemeta::core::URI uri{"http://example%2Ecom/path"};
  EXPECT_TRUE(uri.host().has_value());
  EXPECT_EQ(uri.host().value(), "example.com");
}
