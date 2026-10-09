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

TEST(reserved_ietf_zero_block) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("1::").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(reserved_outside_global_unicast) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("100::").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(teredo_2001) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(benchmarking_2001_2) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:2::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(orchid_deprecated_2001_10) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:10::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(ietf_protocol_assignments_unallocated) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:1ff::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(port_control_protocol_anycast) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:1::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(turn_anycast) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:1::2").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(dns_sd_anycast) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:1::3").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(beyond_anycast_2001_1_4) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:1::4").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(amt_2001_3) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:3::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(as112_2001_4_112) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:4:112::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(orchidv2_2001_20) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:20::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(drone_remote_id_2001_30) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:30::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(six_to_four_public) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2002:808:808::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(six_to_four_loopback) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2002:7f00:1::1").value(),
            sourcemeta::core::IPAddressClass::Loopback);
}

TEST(six_to_four_private) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2002:a00:1::1").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(six_to_four_unspecified) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2002::1").value(),
            sourcemeta::core::IPAddressClass::Unspecified);
}

TEST(documentation_3fff) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("3fff::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(documentation_3fff_upper_bound) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("3fff:fff::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(beyond_documentation_3fff_1000) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("3fff:1000::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(segment_routing_5f00) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("5f00::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(beyond_segment_routing_5f01) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("5f01::1").value(),
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

TEST(nat64_wellknown_public) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("64:ff9b::8.8.8.8").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(nat64_wellknown_loopback) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("64:ff9b::127.0.0.1").value(),
            sourcemeta::core::IPAddressClass::Loopback);
}

TEST(nat64_wellknown_private) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("64:ff9b::192.168.0.1").value(),
            sourcemeta::core::IPAddressClass::Private);
}

TEST(nat64_local_use_reserved) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("64:ff9b:1::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
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

TEST(anycast_block_with_nonzero_interface_is_reserved) {
  const auto result{sourcemeta::core::ipv6_classify("2001:1:0:0:0:0:100:1")};
  EXPECT_EQ(result.value(), sourcemeta::core::IPAddressClass::Reserved);
}

TEST(leading_zero_but_not_mapped_form) {
  const auto result{sourcemeta::core::ipv6_classify("::100:0:0")};
  EXPECT_EQ(result.value(), sourcemeta::core::IPAddressClass::Reserved);
}

// RFC 4291 Section 2.5.5.2 gives the mapped prefix as ::ffff:0:0/96, so an
// address that matches all but its last octet is not one
TEST(near_miss_of_the_mapped_prefix) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::ff00:0:1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_compatible_prefix) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::1:0:0").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

// Anything the low 32 bits hold here falls to the deprecated IPv4-compatible
// form, and RFC 6890 gives the whole of 0.0.0.0/8 as this host on this
// network, so the class these carry is the embedded address's own
TEST(near_miss_of_the_unspecified_address_on_its_third_byte) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::100").value(),
            sourcemeta::core::IPAddressClass::Unspecified);
}

TEST(near_miss_of_the_unspecified_address_on_its_second_byte) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::1:0").value(),
            sourcemeta::core::IPAddressClass::Unspecified);
}

// The one address that is neither of the two the section names, so it is the
// last comparison of each that turns it away rather than an earlier one. That
// it does not read as a loopback is the part worth pinning
TEST(near_miss_of_both_named_addresses_on_their_last_byte) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("::2").value(),
            sourcemeta::core::IPAddressClass::Unspecified);
}

TEST(near_miss_of_the_link_local_prefix) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("fe00::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_nat64_well_known_prefix) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("64:1::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_nat64_local_use_prefix) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("64:ff9b:2::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_anycast_addresses_above_the_range) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:1::4").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_anycast_addresses_with_a_set_interior_byte) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:1::1:1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_as112_prefix_on_its_fifth_byte) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:4::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_as112_prefix_on_its_sixth_byte) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:4:113::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_orchid_prefix) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:40::1").value(),
            sourcemeta::core::IPAddressClass::Reserved);
}

TEST(near_miss_of_the_protocol_assignments_block) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:200::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(near_miss_of_the_documentation_prefix) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2001:db9::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(near_miss_of_the_6to4_prefix) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("2003::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(near_miss_of_the_later_documentation_prefix_on_its_second_byte) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("3ffe::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}

TEST(near_miss_of_the_later_documentation_prefix_on_its_third_byte) {
  EXPECT_EQ(sourcemeta::core::ipv6_classify("3fff:1000::1").value(),
            sourcemeta::core::IPAddressClass::Public);
}
