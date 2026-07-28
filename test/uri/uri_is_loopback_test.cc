#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(ipv4_first_address) {
  const sourcemeta::core::URI uri{"http://127.0.0.1"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_with_port) {
  const sourcemeta::core::URI uri{"http://127.0.0.1:8000"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_with_path) {
  const sourcemeta::core::URI uri{"http://127.0.0.1/callback"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_with_userinfo) {
  const sourcemeta::core::URI uri{"http://user:pass@127.0.0.1/secure"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_with_query_and_fragment) {
  const sourcemeta::core::URI uri{"http://127.0.0.1/search?q=test#section"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_second_address) {
  const sourcemeta::core::URI uri{"http://127.0.0.2:8000"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_network_address) {
  const sourcemeta::core::URI uri{"http://127.0.0.0"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_last_address) {
  const sourcemeta::core::URI uri{"http://127.255.255.255"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_arbitrary_address_in_block) {
  const sourcemeta::core::URI uri{"http://127.63.10.4:1234"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_https_scheme) {
  const sourcemeta::core::URI uri{"https://127.0.0.1"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_no_scheme) {
  const sourcemeta::core::URI uri{"//127.0.0.1/path"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv4_below_block) {
  const sourcemeta::core::URI uri{"http://126.255.255.255"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv4_above_block) {
  const sourcemeta::core::URI uri{"http://128.0.0.1"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv4_private_address) {
  const sourcemeta::core::URI uri{"http://192.168.1.1"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv4_public_address) {
  const sourcemeta::core::URI uri{"http://203.0.113.1:8080"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv4_prefix_of_domain) {
  const sourcemeta::core::URI uri{"http://127.0.0.1.example.com"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv4_octet_out_of_range) {
  const sourcemeta::core::URI uri{"http://127.0.0.256"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv6_compressed) {
  const sourcemeta::core::URI uri{"http://[::1]"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv6_compressed_with_port) {
  const sourcemeta::core::URI uri{"http://[::1]:8000"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv6_uncompressed) {
  const sourcemeta::core::URI uri{"http://[0:0:0:0:0:0:0:1]"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv6_full_form) {
  const sourcemeta::core::URI uri{
      "http://[0000:0000:0000:0000:0000:0000:0000:0001]"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv6_partially_compressed) {
  const sourcemeta::core::URI uri{"http://[::0:1]"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv6_ipv4_mapped_loopback) {
  const sourcemeta::core::URI uri{"http://[::ffff:127.0.0.1]"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv6_ipv4_mapped_loopback_hexadecimal) {
  const sourcemeta::core::URI uri{"http://[::ffff:7f00:1]"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv6_ipv4_compatible_loopback) {
  const sourcemeta::core::URI uri{"http://[::127.0.0.1]"};
  EXPECT_TRUE(uri.is_loopback());
}

TEST(ipv6_ipv4_mapped_public) {
  const sourcemeta::core::URI uri{"http://[::ffff:192.0.2.1]"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv6_unspecified) {
  const sourcemeta::core::URI uri{"http://[::]"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv6_next_address) {
  const sourcemeta::core::URI uri{"http://[::2]"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv6_link_local) {
  const sourcemeta::core::URI uri{"http://[fe80::1]"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipv6_documentation_address) {
  const sourcemeta::core::URI uri{"http://[2001:db8::1]"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(ipvfuture) {
  const sourcemeta::core::URI uri{"http://[v1.loopback]"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(localhost) {
  const sourcemeta::core::URI uri{"http://localhost"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(localhost_with_port) {
  const sourcemeta::core::URI uri{"http://localhost:8000"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(localhost_uppercase) {
  const sourcemeta::core::URI uri{"http://LOCALHOST"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(localhost_subdomain) {
  const sourcemeta::core::URI uri{"http://foo.localhost"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(localhost_prefix_of_domain) {
  const sourcemeta::core::URI uri{"http://localhost.evil.com"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(domain) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(relative_starting_with_address) {
  const sourcemeta::core::URI uri{"127.0.0.1/path"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(fragment_only) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(mailto) {
  const sourcemeta::core::URI uri{"mailto:jdoe@mail.com"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(file_empty_host) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_FALSE(uri.is_loopback());
}

TEST(empty) {
  const sourcemeta::core::URI uri{""};
  EXPECT_FALSE(uri.is_loopback());
}
