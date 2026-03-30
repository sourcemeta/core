#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_is_ipv4, ipv4_simple) {
  const sourcemeta::core::URI uri{"http://192.168.1.1/index.html"};
  EXPECT_TRUE(uri.is_ipv4());
  EXPECT_FALSE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "192.168.1.1");
}

TEST(URI_is_ipv4, ipv4_loopback) {
  const sourcemeta::core::URI uri{"http://127.0.0.1/"};
  EXPECT_TRUE(uri.is_ipv4());
  EXPECT_EQ(uri.host().value(), "127.0.0.1");
}

TEST(URI_is_ipv4, ipv4_with_port) {
  const sourcemeta::core::URI uri{"http://10.0.0.1:8080/api"};
  EXPECT_TRUE(uri.is_ipv4());
  EXPECT_EQ(uri.host().value(), "10.0.0.1");
  EXPECT_EQ(uri.port().value(), 8080);
}

TEST(URI_is_ipv4, ipv4_with_query) {
  const sourcemeta::core::URI uri{"http://1.2.3.4/search?q=test"};
  EXPECT_TRUE(uri.is_ipv4());
  EXPECT_EQ(uri.host().value(), "1.2.3.4");
}

TEST(URI_is_ipv4, ipv4_with_fragment) {
  const sourcemeta::core::URI uri{"http://0.0.0.0/doc#top"};
  EXPECT_TRUE(uri.is_ipv4());
  EXPECT_EQ(uri.host().value(), "0.0.0.0");
}

TEST(URI_is_ipv4, ipv4_with_userinfo) {
  const sourcemeta::core::URI uri{"http://user:pass@255.255.255.255/"};
  EXPECT_TRUE(uri.is_ipv4());
  EXPECT_EQ(uri.host().value(), "255.255.255.255");
}

TEST(URI_is_ipv4, ipv4_recompose) {
  const sourcemeta::core::URI uri{"http://192.168.1.1:80/path"};
  EXPECT_TRUE(uri.is_ipv4());
  EXPECT_EQ(uri.recompose(), "http://192.168.1.1:80/path");
}

TEST(URI_is_ipv4, not_ipv4_hostname) {
  const sourcemeta::core::URI uri{"http://example.com/"};
  EXPECT_FALSE(uri.is_ipv4());
}

TEST(URI_is_ipv4, not_ipv4_hostname_with_numbers) {
  const sourcemeta::core::URI uri{"http://host123.example.com/"};
  EXPECT_FALSE(uri.is_ipv4());
}

TEST(URI_is_ipv4, not_ipv4_ipv4_like_hostname) {
  const sourcemeta::core::URI uri{
      "http://server.198.51.100.1.example.com/page"};
  EXPECT_FALSE(uri.is_ipv4());
}

TEST(URI_is_ipv4, not_ipv4_ipv6) {
  const sourcemeta::core::URI uri{"http://[::1]/"};
  EXPECT_FALSE(uri.is_ipv4());
  EXPECT_TRUE(uri.is_ipv6());
}

TEST(URI_is_ipv4, not_ipv4_ipvfuture) {
  const sourcemeta::core::URI uri{"http://[v1.test]/"};
  EXPECT_FALSE(uri.is_ipv4());
  EXPECT_FALSE(uri.is_ipv6());
}

TEST(URI_is_ipv4, not_ipv4_no_host) {
  const sourcemeta::core::URI uri{"file:///path"};
  EXPECT_FALSE(uri.is_ipv4());
}

TEST(URI_is_ipv4, not_ipv4_invalid_octet_in_hostname) {
  const sourcemeta::core::URI uri{"http://999.999.999.999/"};
  EXPECT_FALSE(uri.is_ipv4());
}
