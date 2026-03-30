#include <gtest/gtest.h>

#include <sourcemeta/core/ip.h>

TEST(IP_ipv6, valid_unspecified) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::"));
}

TEST(IP_ipv6, valid_loopback) { EXPECT_TRUE(sourcemeta::core::is_ipv6("::1")); }

TEST(IP_ipv6, valid_documentation_prefix) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("2001:db8::1"));
}

TEST(IP_ipv6, valid_full_form_leading_zeros) {
  EXPECT_TRUE(
      sourcemeta::core::is_ipv6("2001:0db8:0000:0000:0000:0000:0000:0001"));
}

TEST(IP_ipv6, valid_mixed_elision) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("2001:db8:85a3::8a2e:370:7334"));
}

TEST(IP_ipv6, valid_link_local) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("fe80::1"));
}

TEST(IP_ipv6, valid_multicast) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("ff02::1"));
}

TEST(IP_ipv6, valid_ipv4_mapped) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::ffff:192.168.1.1"));
}

TEST(IP_ipv6, valid_ipv4_mapped_with_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::ffff:0:192.168.1.1"));
}

TEST(IP_ipv6, valid_ipv4_mixed_with_compression) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("2001:db8::192.168.1.1"));
}

TEST(IP_ipv6, valid_all_zeros_explicit) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("0:0:0:0:0:0:0:0"));
}

TEST(IP_ipv6, valid_loopback_explicit) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("0:0:0:0:0:0:0:1"));
}

TEST(IP_ipv6, valid_uppercase) {
  EXPECT_TRUE(
      sourcemeta::core::is_ipv6("ABCD:EF01:2345:6789:ABCD:EF01:2345:6789"));
}

TEST(IP_ipv6, valid_lowercase) {
  EXPECT_TRUE(
      sourcemeta::core::is_ipv6("abcd:ef01:2345:6789:abcd:ef01:2345:6789"));
}

TEST(IP_ipv6, valid_mixed_case) {
  EXPECT_TRUE(
      sourcemeta::core::is_ipv6("AbCd:eF01:2345:6789:aBcD:Ef01:2345:6789"));
}

TEST(IP_ipv6, valid_all_single_digit_groups) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7:8"));
}

TEST(IP_ipv6, valid_elide_first_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::2:3:4:5:6:7:8"));
}

TEST(IP_ipv6, valid_elide_second_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1::3:4:5:6:7:8"));
}

TEST(IP_ipv6, valid_elide_third_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2::4:5:6:7:8"));
}

TEST(IP_ipv6, valid_elide_fourth_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3::5:6:7:8"));
}

TEST(IP_ipv6, valid_elide_fifth_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4::6:7:8"));
}

TEST(IP_ipv6, valid_elide_sixth_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4:5::7:8"));
}

TEST(IP_ipv6, valid_elide_seventh_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4:5:6::8"));
}

TEST(IP_ipv6, valid_elide_last_group) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7::"));
}

TEST(IP_ipv6, valid_elide_last_seven) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1::"));
}

TEST(IP_ipv6, valid_elide_first_seven) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::8"));
}

TEST(IP_ipv6, valid_ipv4_suffix_no_compression) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:1.2.3.4"));
}

TEST(IP_ipv6, valid_ipv4_suffix_all_elided) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::1.2.3.4"));
}

TEST(IP_ipv6, valid_ipv4_suffix_ffff) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::ffff:1.2.3.4"));
}

TEST(IP_ipv6, valid_ipv4_suffix_with_one_group_and_compression) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1::1.2.3.4"));
}

TEST(IP_ipv6, valid_ipv4_suffix_with_four_groups_and_compression) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4::1.2.3.4"));
}

TEST(IP_ipv6, valid_all_max) {
  EXPECT_TRUE(
      sourcemeta::core::is_ipv6("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF"));
}

TEST(IP_ipv6, valid_all_zeros_leading_zeros) {
  EXPECT_TRUE(
      sourcemeta::core::is_ipv6("0000:0000:0000:0000:0000:0000:0000:0000"));
}

TEST(IP_ipv6, valid_ipv4_suffix_max) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:255.255.255.255"));
}

TEST(IP_ipv6, valid_ipv4_suffix_zeros) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:0.0.0.0"));
}

TEST(IP_ipv6, valid_elide_multiple_middle) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2::7:8"));
}

TEST(IP_ipv6, valid_elide_six_groups) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1::8"));
}

TEST(IP_ipv6, valid_rfc2732_full) {
  EXPECT_TRUE(
      sourcemeta::core::is_ipv6("FEDC:BA98:7654:3210:FEDC:BA98:7654:3210"));
}

TEST(IP_ipv6, valid_rfc2732_with_zeros) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1080:0:0:0:8:800:200C:417A"));
}

TEST(IP_ipv6, valid_rfc2732_compressed) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("3ffe:2a00:100:7031::1"));
}

TEST(IP_ipv6, valid_ipv4_compatible_deprecated) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::192.9.5.5"));
}

TEST(IP_ipv6, valid_ipv4_mapped_uppercase_ffff) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::FFFF:129.144.52.38"));
}

TEST(IP_ipv6, valid_two_groups_with_compression) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("2010:836B:4179::836B:4179"));
}

TEST(IP_ipv6, valid_ipv4_suffix_two_groups_compressed) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2::5:6:1.2.3.4"));
}

TEST(IP_ipv6, valid_ipv4_suffix_three_groups_compressed) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1::4:5:6:1.2.3.4"));
}

TEST(IP_ipv6, invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_ipv6("")); }

TEST(IP_ipv6, invalid_nine_groups) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7:8:9"));
}

TEST(IP_ipv6, invalid_double_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1::2::3"));
}

TEST(IP_ipv6, invalid_seven_groups_no_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7"));
}

TEST(IP_ipv6, invalid_non_hex_character) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::gggg"));
}

TEST(IP_ipv6, invalid_five_hex_digits) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7:00001"));
}

TEST(IP_ipv6, invalid_leading_single_colon) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6(":1:2:3:4:5:6:7:8"));
}

TEST(IP_ipv6, invalid_trailing_single_colon) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7:8:"));
}

TEST(IP_ipv6, invalid_triple_colon_start) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6(":::1"));
}

TEST(IP_ipv6, invalid_triple_colon_middle) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:::2"));
}

TEST(IP_ipv6, invalid_plain_ipv4) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("192.168.1.1"));
}

TEST(IP_ipv6, invalid_brackets_included) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("[2001:db8::1]"));
}

TEST(IP_ipv6, invalid_zone_id) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("2001:db8::1%eth0"));
}

TEST(IP_ipv6, invalid_too_many_groups_with_ipv4) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7:8:1.2.3.4"));
}

TEST(IP_ipv6, invalid_eight_groups_with_ipv4) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7:1.2.3.4"));
}

TEST(IP_ipv6, invalid_ipv4_octet_overflow_in_mixed) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::1.2.3.256"));
}

TEST(IP_ipv6, invalid_incomplete_ipv4_in_mixed) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::1.2.3"));
}

TEST(IP_ipv6, invalid_five_groups_before_ipv4) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:1.2.3.4"));
}

TEST(IP_ipv6, invalid_words) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("hello"));
}

TEST(IP_ipv6, invalid_single_colon) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6(":"));
}

TEST(IP_ipv6, invalid_trailing_colon_not_double) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7:"));
}

TEST(IP_ipv6, invalid_leading_colon_not_double) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6(":1:2:3:4:5:6:7"));
}

TEST(IP_ipv6, invalid_ipv4_leading_zero_in_mixed) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:1.2.3.04"));
}

TEST(IP_ipv6, invalid_space_in_address) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("2001:db8:: 1"));
}

TEST(IP_ipv6, invalid_empty_group) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3::5:6:7:8:9"));
}

TEST(IP_ipv6, invalid_too_many_with_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7::8:9"));
}

TEST(IP_ipv6, invalid_ipv4_five_octets_in_mixed) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::1.2.3.4.5"));
}

TEST(IP_ipv6, invalid_hex_in_ipv4_suffix) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::ffff:0x1.0x2.0x3.0x4"));
}

TEST(IP_ipv6, invalid_ipv4_negative_in_mixed) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::ffff:-1.2.3.4"));
}

TEST(IP_ipv6, invalid_slash_notation) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("2001:db8::1/64"));
}

TEST(IP_ipv6, invalid_six_groups_no_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6"));
}

TEST(IP_ipv6, invalid_five_groups_no_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5"));
}

TEST(IP_ipv6, invalid_eight_groups_after_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::1:2:3:4:5:6:7:8"));
}

TEST(IP_ipv6, invalid_seven_before_one_after_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6:7::8"));
}

TEST(IP_ipv6, invalid_one_before_seven_after_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1::2:3:4:5:6:7:8"));
}

TEST(IP_ipv6, invalid_six_hex_plus_ipv4_after_compression) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::1:2:3:4:5:6:1.2.3.4"));
}

TEST(IP_ipv6, invalid_six_hex_before_compression_plus_ipv4) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5:6::1.2.3.4"));
}

TEST(IP_ipv6, invalid_five_before_one_after_compression_plus_ipv4) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("1:2:3:4:5::6:1.2.3.4"));
}

TEST(IP_ipv6, invalid_trailing_content_after_ipv4_suffix) {
  EXPECT_FALSE(sourcemeta::core::is_ipv6("::1.2.3.4:0"));
}

TEST(IP_ipv6, valid_five_hex_after_compression_plus_ipv4) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::2:3:4:5:6:1.2.3.4"));
}

TEST(IP_ipv6, valid_five_hex_before_compression_plus_ipv4) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("1:2:3:4:5::1.2.3.4"));
}

TEST(IP_ipv6, valid_all_zero_hex_plus_ipv4_no_compression) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("0:0:0:0:0:0:1.2.3.4"));
}

TEST(IP_ipv6, valid_all_zero_ipv4_with_compression) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::0.0.0.0"));
}

TEST(IP_ipv6, valid_all_max_ipv4_with_compression) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("::255.255.255.255"));
}

TEST(IP_ipv6, valid_single_alpha_hex_groups) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("a:b:c:d:e:f:0:1"));
}

TEST(IP_ipv6, valid_two_digit_hex_groups_with_leading_zeros) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("01:02:03:04:05:06:07:08"));
}

TEST(IP_ipv6, valid_three_digit_hex_groups_with_leading_zeros) {
  EXPECT_TRUE(sourcemeta::core::is_ipv6("001:002:003:004:005:006:007:008"));
}
