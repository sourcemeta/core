#include <sourcemeta/core/ip.h>
#include <sourcemeta/core/test.h>

TEST(valid_all_zeros) { EXPECT_TRUE(sourcemeta::core::is_ipv4("0.0.0.0")); }

TEST(valid_all_max) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("255.255.255.255"));
}

TEST(valid_private_class_c) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("192.168.1.1"));
}

TEST(valid_private_class_a) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("10.0.0.1"));
}

TEST(valid_loopback) { EXPECT_TRUE(sourcemeta::core::is_ipv4("127.0.0.1")); }

TEST(valid_simple) { EXPECT_TRUE(sourcemeta::core::is_ipv4("1.2.3.4")); }

TEST(valid_three_digit_octets) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("100.200.30.40"));
}

TEST(valid_high_octets) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("249.249.249.249"));
}

TEST(valid_boundary_250s) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("250.251.252.253"));
}

TEST(valid_first_octet_zero) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("0.0.0.1"));
}

TEST(valid_last_octets_zero) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("1.0.0.0"));
}

TEST(valid_single_digits) { EXPECT_TRUE(sourcemeta::core::is_ipv4("1.1.1.1")); }

TEST(valid_two_digit_octets) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("99.88.77.66"));
}

TEST(valid_mixed_lengths) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("1.22.123.4"));
}

TEST(valid_254_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("254.254.254.254"));
}

TEST(valid_broadcast) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("255.255.255.0"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_ipv4("")); }

TEST(invalid_first_octet_overflow) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("256.0.0.0"));
}

TEST(invalid_last_octet_overflow) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0.0.0.256"));
}

TEST(invalid_five_octets) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4.5"));
}

TEST(invalid_three_octets) { EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3")); }

TEST(invalid_two_octets) { EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2")); }

TEST(invalid_one_octet) { EXPECT_FALSE(sourcemeta::core::is_ipv4("1")); }

TEST(invalid_leading_zeros_all) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("01.02.03.04"));
}

TEST(invalid_leading_zero_second_octet) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.02.3.4"));
}

TEST(invalid_double_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("00.0.0.0"));
}

TEST(invalid_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4(".1.2.3.4"));
}

TEST(invalid_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4."));
}

TEST(invalid_empty_octet) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1..2.3.4"));
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4 "));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4(" 1.2.3.4"));
}

TEST(invalid_alpha_in_octet) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.a"));
}

TEST(invalid_hex_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0x1.0x2.0x3.0x4"));
}

TEST(invalid_all_overflow) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("999.999.999.999"));
}

TEST(invalid_negative) { EXPECT_FALSE(sourcemeta::core::is_ipv4("-1.0.0.0")); }

TEST(invalid_cidr_suffix) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4/24"));
}

TEST(invalid_leading_zero_last_octet) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.04"));
}

TEST(invalid_three_digit_leading_zeros) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("192.168.001.1"));
}

TEST(invalid_port_appended) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4:80"));
}

TEST(invalid_only_dots) { EXPECT_FALSE(sourcemeta::core::is_ipv4("...")); }

TEST(invalid_four_dots) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4.5."));
}

TEST(invalid_words) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("one.two.three.four"));
}

TEST(invalid_octet_260) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("260.0.0.0"));
}

TEST(invalid_four_digit_octet) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1000.0.0.0"));
}

TEST(invalid_just_dot) { EXPECT_FALSE(sourcemeta::core::is_ipv4(".")); }

TEST(invalid_three_empty_dots) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4(".."));
}

TEST(invalid_plus_sign) { EXPECT_FALSE(sourcemeta::core::is_ipv4("+1.2.3.4")); }

TEST(invalid_leading_zero_first_octet_200) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0200.0.0.0"));
}

TEST(valid_boundary_199) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("199.199.199.199"));
}

TEST(valid_boundary_200) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("200.200.200.200"));
}

TEST(valid_boundary_9) { EXPECT_TRUE(sourcemeta::core::is_ipv4("9.9.9.9")); }

TEST(valid_boundary_10) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("10.10.10.10"));
}

TEST(valid_boundary_99) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("99.99.99.99"));
}

TEST(valid_boundary_100) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("100.100.100.100"));
}

TEST(valid_boundary_250) {
  EXPECT_TRUE(sourcemeta::core::is_ipv4("250.250.250.250"));
}

TEST(invalid_second_octet_overflow) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0.256.0.0"));
}

TEST(invalid_third_octet_overflow) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0.0.256.0"));
}

TEST(invalid_third_octet_300) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0.0.300.0"));
}

TEST(invalid_leading_zero_last_octet_double) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0.0.0.00"));
}

TEST(invalid_trailing_tab) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4\t"));
}

TEST(invalid_leading_tab) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("\t1.2.3.4"));
}

TEST(invalid_trailing_newline) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4\n"));
}

TEST(invalid_leading_newline) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("\n1.2.3.4"));
}

TEST(invalid_trailing_crlf) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4\r\n"));
}

TEST(invalid_space_after_dot) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1. 2.3.4"));
}

TEST(invalid_space_before_dot) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2 .3.4"));
}

TEST(invalid_comma_separators) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1,2,3,4"));
}

TEST(invalid_semicolon_separators) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1;2;3;4"));
}

TEST(invalid_colon_separators) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1:2:3:4"));
}

TEST(invalid_very_long_octet) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("12345678901234567890.0.0.0"));
}

TEST(invalid_eight_octets) {
  EXPECT_FALSE(sourcemeta::core::is_ipv4("255.255.255.255.255.255.255.255"));
}

TEST(invalid_whitespace_only) { EXPECT_FALSE(sourcemeta::core::is_ipv4(" ")); }
