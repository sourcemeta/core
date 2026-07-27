#include <sourcemeta/core/ip.h>
#include <sourcemeta/core/test.h>

TEST(loopback_127) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("127.0.0.1").value(),
            sourcemeta::core::IPAddressClass::Loopback);
}

TEST(loopback_whole_block) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("127.255.255.255").value(),
            sourcemeta::core::IPAddressClass::Loopback);
}

TEST(unspecified_zero) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("0.0.0.0").value(),
            sourcemeta::core::IPAddressClass::Unspecified);
}

TEST(unspecified_whole_block) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("0.1.2.3").value(),
            sourcemeta::core::IPAddressClass::Unspecified);
}

TEST(private_ten) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("10.0.0.1").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(private_172_16_lower_bound) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("172.16.0.0").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(private_172_16_upper_bound) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("172.31.255.255").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(public_172_15_below_range) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("172.15.0.1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(public_172_32_above_range) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("172.32.0.1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(private_192_168) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("192.168.1.1").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(private_shared_100_64_lower_bound) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("100.64.0.0").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(private_shared_100_64_upper_bound) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("100.127.255.255").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(public_100_63_below_range) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("100.63.0.1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(public_100_128_above_range) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("100.128.0.1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(link_local_169_254) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("169.254.1.1").value(),
            sourcemeta::core::IPAddressClass::LinkLocal);
}

TEST(public_169_253_below_range) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("169.253.1.1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(reserved_protocol_assignments_192_0_0) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("192.0.0.1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(reserved_documentation_192_0_2) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("192.0.2.1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(reserved_benchmarking_198_18) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("198.18.0.1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(reserved_benchmarking_198_19) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("198.19.255.255").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(reserved_documentation_198_51_100) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("198.51.100.1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(reserved_documentation_203_0_113) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("203.0.113.1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(multicast_224_lower_bound) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("224.0.0.1").value(),
            sourcemeta::core::IPAddressClass::Multicast);
}

TEST(multicast_239_upper_bound) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("239.255.255.255").value(),
            sourcemeta::core::IPAddressClass::Multicast);
}

TEST(public_223_below_multicast) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("223.255.255.255").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(reserved_future_240) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("240.0.0.1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(reserved_broadcast_255) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("255.255.255.255").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(public_google) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("8.8.8.8").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(public_cloudflare) {
  EXPECT_EQ(sourcemeta::core::ipv4_classify("1.1.1.1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(invalid_hostname) {
  EXPECT_FALSE(sourcemeta::core::ipv4_classify("example.com").has_value());
}

TEST(invalid_out_of_range_octet) {
  EXPECT_FALSE(sourcemeta::core::ipv4_classify("999.0.0.1").has_value());
}

TEST(invalid_too_few_octets) {
  EXPECT_FALSE(sourcemeta::core::ipv4_classify("10.0.0").has_value());
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::ipv4_classify("").has_value());
}
