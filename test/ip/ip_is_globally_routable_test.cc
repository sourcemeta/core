#include <sourcemeta/core/ip.h>
#include <sourcemeta/core/test.h>

TEST(public_ipv4_google) {
  EXPECT_TRUE(sourcemeta::core::ip_is_globally_routable("8.8.8.8"));
}

TEST(public_ipv4_cloudflare) {
  EXPECT_TRUE(sourcemeta::core::ip_is_globally_routable("1.1.1.1"));
}

TEST(public_ipv6_google) {
  EXPECT_TRUE(
      sourcemeta::core::ip_is_globally_routable("2001:4860:4860::8888"));
}

TEST(public_ipv6_cloudflare) {
  EXPECT_TRUE(
      sourcemeta::core::ip_is_globally_routable("2606:4700:4700::1111"));
}

TEST(blocks_ipv4_loopback) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("127.0.0.1"));
}

TEST(blocks_ipv4_private_ten) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("10.0.0.1"));
}

TEST(blocks_ipv4_private_192_168) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("192.168.1.1"));
}

TEST(blocks_ipv4_private_172_16) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("172.16.0.1"));
}

TEST(blocks_ipv4_link_local) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("169.254.169.254"));
}

TEST(blocks_ipv4_unspecified) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("0.0.0.0"));
}

TEST(blocks_ipv4_shared) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("100.64.0.1"));
}

TEST(blocks_ipv4_multicast) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("224.0.0.1"));
}

TEST(blocks_ipv4_broadcast) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("255.255.255.255"));
}

TEST(blocks_ipv4_documentation) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("198.51.100.1"));
}

TEST(blocks_ipv6_loopback) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("::1"));
}

TEST(blocks_ipv6_unspecified) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("::"));
}

TEST(blocks_ipv6_link_local) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("fe80::1"));
}

TEST(blocks_ipv6_unique_local) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("fc00::1"));
}

TEST(blocks_ipv6_multicast) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("ff02::1"));
}

TEST(blocks_ipv6_documentation) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("2001:db8::1"));
}

TEST(blocks_mapped_loopback_bypass) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("::ffff:127.0.0.1"));
}

TEST(blocks_mapped_private_bypass) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("::ffff:10.0.0.1"));
}

TEST(blocks_mapped_link_local_bypass) {
  EXPECT_FALSE(
      sourcemeta::core::ip_is_globally_routable("::ffff:169.254.169.254"));
}

TEST(blocks_compatible_loopback_bypass) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("::127.0.0.1"));
}

TEST(blocks_a_hostname) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("example.com"));
}

TEST(blocks_invalid_input) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable("not an address"));
}

TEST(blocks_empty_input) {
  EXPECT_FALSE(sourcemeta::core::ip_is_globally_routable(""));
}
