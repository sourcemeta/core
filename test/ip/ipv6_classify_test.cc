#include <sourcemeta/core/ip.h>
#include <sourcemeta/core/test.h>

TEST(unspecified) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::").value(),
            sourcemeta::core::IPAddressClass::Unspecified);
}

TEST(loopback) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::1").value(),
            sourcemeta::core::IPAddressClass::Loopback);
}

TEST(link_local_fe80) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("fe80::1").value(),
            sourcemeta::core::IPAddressClass::LinkLocal);
}

TEST(link_local_upper_bound_febf) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("febf::").value(),
            sourcemeta::core::IPAddressClass::LinkLocal);
}

TEST(unique_local_fc00) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("fc00::1").value(),
            sourcemeta::core::IPAddressClass::UniqueLocal);
}

TEST(unique_local_fd00) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("fd12:3456:789a::1").value(),
            sourcemeta::core::IPAddressClass::UniqueLocal);
}

TEST(multicast_ff02) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("ff02::1").value(),
            sourcemeta::core::IPAddressClass::Multicast);
}

TEST(multicast_ff00) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("ff00::").value(),
            sourcemeta::core::IPAddressClass::Multicast);
}

TEST(documentation_2001_db8) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:db8::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(public_google) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:4860:4860::8888").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(public_cloudflare) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2606:4700:4700::1111").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(mapped_loopback) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::ffff:127.0.0.1").value(),
            sourcemeta::core::IPAddressClass::Loopback);
}

TEST(mapped_private) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::ffff:192.168.1.1").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(mapped_link_local) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::ffff:169.254.1.1").value(),
            sourcemeta::core::IPAddressClass::LinkLocal);
}

TEST(mapped_public) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::ffff:8.8.8.8").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(compatible_loopback) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::127.0.0.1").value(),
            sourcemeta::core::IPAddressClass::Loopback);
}

TEST(compatible_private) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::192.168.1.1").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(compatible_public) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::8.8.8.8").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(invalid_hostname) {
  EXPECT_FALSE(sourcemeta::core::ipv6_classify("not:an:address").has_value());
}

TEST(invalid_group) {
  EXPECT_FALSE(sourcemeta::core::ipv6_classify("12345::").has_value());
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::ipv6_classify("").has_value());
}

TEST(invalid_ipv4) {
  EXPECT_FALSE(sourcemeta::core::ipv6_classify("127.0.0.1").has_value());
}
