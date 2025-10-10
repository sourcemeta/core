#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

// Insipred from https://datatracker.ietf.org/doc/html/rfc2732#section-2

TEST(URI_is_ipv6, ipv6_1) {
  const sourcemeta::core::URI uri{
      "http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html"};
  EXPECT_TRUE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "fedc:ba98:7654:3210:fedc:ba98:7654:3210");
  EXPECT_EQ(uri.recompose(),
            "http://[fedc:ba98:7654:3210:fedc:ba98:7654:3210]:80/index.html");
}

TEST(URI_is_ipv6, ipv6_2) {
  const sourcemeta::core::URI uri{
      "http://[1080:0:0:0:8:800:200C:417A]/index.html"};
  EXPECT_TRUE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "1080:0:0:0:8:800:200c:417a");
  EXPECT_EQ(uri.recompose(), "http://[1080:0:0:0:8:800:200c:417a]/index.html");
}

TEST(URI_is_ipv6, ipv6_3) {
  const sourcemeta::core::URI uri{"http://[3ffe:2a00:100:7031::1]"};
  EXPECT_TRUE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "3ffe:2a00:100:7031::1");
  EXPECT_EQ(uri.recompose(), "http://[3ffe:2a00:100:7031::1]");
}

TEST(URI_is_ipv6, ipv6_4) {
  const sourcemeta::core::URI uri{"http://[1080::8:800:200C:417A]/foo"};
  EXPECT_TRUE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "1080::8:800:200c:417a");
  EXPECT_EQ(uri.recompose(), "http://[1080::8:800:200c:417a]/foo");
}

TEST(URI_is_ipv6, ipv6_5) {
  const sourcemeta::core::URI uri{"http://[::192.9.5.5]/ipng"};
  EXPECT_TRUE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "::192.9.5.5");
  EXPECT_EQ(uri.recompose(), "http://[::192.9.5.5]/ipng");
}

TEST(URI_is_ipv6, ipv6_6) {
  const sourcemeta::core::URI uri{
      "http://[::FFFF:129.144.52.38]:80/index.html"};
  EXPECT_TRUE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "::ffff:129.144.52.38");
  EXPECT_EQ(uri.recompose(), "http://[::ffff:129.144.52.38]:80/index.html");
}

TEST(URI_is_ipv6, ipv6_7) {
  const sourcemeta::core::URI uri{"http://[2010:836B:4179::836B:4179]"};
  EXPECT_TRUE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "2010:836b:4179::836b:4179");
  EXPECT_EQ(uri.recompose(), "http://[2010:836b:4179::836b:4179]");
}

TEST(URI_is_ipv6, ipv4) {
  const sourcemeta::core::URI uri{"http://192.168.1.1/index.html"};
  EXPECT_FALSE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "192.168.1.1");
  EXPECT_EQ(uri.recompose(), "http://192.168.1.1/index.html");
}

TEST(URI_is_ipv6, ipv4_with_port) {
  const sourcemeta::core::URI uri{"http://203.0.113.1:8080/api/data"};
  EXPECT_FALSE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "203.0.113.1");
  EXPECT_EQ(uri.recompose(), "http://203.0.113.1:8080/api/data");
}

TEST(URI_is_ipv6, ipv4_one_subdomain) {
  const sourcemeta::core::URI uri{
      "http://server.198.51.100.1.example.com/page"};
  EXPECT_FALSE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "server.198.51.100.1.example.com");
  EXPECT_EQ(uri.recompose(), "http://server.198.51.100.1.example.com/page");
}

TEST(URI_is_ipv6, ipv4_with_userinfo) {
  const sourcemeta::core::URI uri{"http://user:pass@192.0.2.1/secure"};
  EXPECT_FALSE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "192.0.2.1");
  EXPECT_EQ(uri.recompose(), "http://user:pass@192.0.2.1/secure");
}

TEST(URI_is_ipv6, ipv4_with_query) {
  const sourcemeta::core::URI uri{"http://198.51.100.42/search?q=test&page=1"};
  EXPECT_FALSE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "198.51.100.42");
  EXPECT_EQ(uri.recompose(), "http://198.51.100.42/search?q=test&page=1");
}

TEST(URI_is_ipv6, ipv4_with_fragment) {
  const sourcemeta::core::URI uri{"http://203.0.113.53/document#section-2"};
  EXPECT_FALSE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "203.0.113.53");
  EXPECT_EQ(uri.recompose(), "http://203.0.113.53/document#section-2");
}

TEST(URI_is_ipv6, ipv4_multiple_subdomains) {
  const sourcemeta::core::URI uri{
      "http://api.v1.198.51.100.5.example.com/endpoint"};
  EXPECT_FALSE(uri.is_ipv6());
  EXPECT_EQ(uri.host().value(), "api.v1.198.51.100.5.example.com");
  EXPECT_EQ(uri.recompose(), "http://api.v1.198.51.100.5.example.com/endpoint");
}
