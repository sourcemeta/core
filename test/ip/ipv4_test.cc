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

TEST(invalid_fullwidth_digits_all) {
  // valid-looking but fullwidth (U+FF10 block) digits
  EXPECT_FALSE(sourcemeta::core::is_ipv4("１９２.１６８.１.１"));
}

TEST(invalid_fullwidth_digit_first_octet) {
  // one fullwidth digit in first octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("１92.168.0.1"));
}

TEST(invalid_fullwidth_digit_last_octet) {
  // fullwidth digit in last octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("192.168.0.１"));
}

TEST(invalid_arabic_indic_digits) {
  // Arabic-Indic digits (U+0660 block)
  EXPECT_FALSE(sourcemeta::core::is_ipv4("١٩٢.١٦٨.٠.١"));
}

TEST(invalid_ext_arabic_indic_digits) {
  // Extended Arabic-Indic digits (U+06F0 block)
  EXPECT_FALSE(sourcemeta::core::is_ipv4("۱۹۲.۱۶۸.۰.۱"));
}

TEST(invalid_devanagari_digits) {
  // Devanagari digits (U+0966 block)
  EXPECT_FALSE(sourcemeta::core::is_ipv4("१९२.१६८.०.१"));
}

TEST(invalid_bengali_digit_inner) {
  // a Bengali 2 (U+09E8) inside octet one
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1২7.0.0.1"));
}

TEST(invalid_math_bold_digits) {
  // Mathematical Bold digits (U+1D7CE, astral)
  EXPECT_FALSE(sourcemeta::core::is_ipv4("𝟏𝟗𝟐.𝟏𝟔𝟖.𝟏.𝟏"));
}

TEST(invalid_superscript_two) {
  // superscript two (U+00B2)
  EXPECT_FALSE(sourcemeta::core::is_ipv4("².2.3.4"));
}

TEST(invalid_circled_digit) {
  // circled digit one (U+2460, not a decimal digit)
  EXPECT_FALSE(sourcemeta::core::is_ipv4("①.2.3.4"));
}

TEST(invalid_embedded_nul_trailing) {
  // valid address then NUL then trailing junk
  EXPECT_FALSE(sourcemeta::core::is_ipv4(
      std::string_view("192.168.0.1\0.evil.com", 21)));
}

TEST(invalid_trailing_nul) {
  // valid address then a single trailing NUL
  EXPECT_FALSE(
      sourcemeta::core::is_ipv4(std::string_view("192.168.0.1\0", 12)));
}

TEST(invalid_two_part_shorthand) {
  // inet_aton 2-part shorthand
  EXPECT_FALSE(sourcemeta::core::is_ipv4("127.1"));
}

TEST(invalid_three_part_shorthand) {
  // inet_aton 3-part shorthand
  EXPECT_FALSE(sourcemeta::core::is_ipv4("127.0.1"));
}

TEST(invalid_decimal_integer_form) {
  // the 32-bit integer for 127.0.0.1
  EXPECT_FALSE(sourcemeta::core::is_ipv4("2130706433"));
}

TEST(invalid_decimal_integer_form_high) {
  // the 32-bit integer for 192.168.0.1
  EXPECT_FALSE(sourcemeta::core::is_ipv4("3232235521"));
}

TEST(invalid_integer_overflow_2_32) {
  // 2^32, one past the 32-bit range
  EXPECT_FALSE(sourcemeta::core::is_ipv4("4294967296"));
}

TEST(invalid_hex_integer) {
  // hex integer form
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0x7f000001"));
}

TEST(invalid_hex_octet_first) {
  // 0x-prefixed first octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0x7f.0.0.1"));
}

TEST(invalid_octal_prefix_explicit) {
  // explicit 0o octal prefix
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0o10.0.0.1"));
}

TEST(invalid_octal_leading_zero_form) {
  // octal-looking leading-zero form
  EXPECT_FALSE(sourcemeta::core::is_ipv4("0300.0250.0.1"));
}

TEST(invalid_trailing_cr_only) {
  // bare trailing carriage return
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4\r"));
}

TEST(invalid_leading_cr_only) {
  // bare leading carriage return
  EXPECT_FALSE(sourcemeta::core::is_ipv4("\r1.2.3.4"));
}

TEST(invalid_trailing_vertical_tab) {
  // trailing vertical tab
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4\v"));
}

TEST(invalid_trailing_form_feed) {
  // trailing form feed
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4\f"));
}

TEST(invalid_trailing_nbsp) {
  // trailing non-breaking space (U+00A0)
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2.3.4 "));
}

TEST(invalid_internal_nbsp) {
  // non-breaking space before an octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.2. 3.4"));
}

TEST(invalid_exponent_octet) {
  // exponent notation in first octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1e2.0.0.1"));
}

TEST(invalid_plus_inner_octet) {
  // plus sign on an inner octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.+2.3.4"));
}

TEST(invalid_float_octet) {
  // extra fractional-looking component
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.5.3.4.0"));
}

TEST(invalid_pipe_separators) {
  // pipe separators
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1|2|3|4"));
}

TEST(invalid_space_separators) {
  // space separators
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1 2 3 4"));
}

TEST(invalid_first_octet_256) {
  // 256 in the first octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("256.1.1.1"));
}

TEST(invalid_second_octet_256) {
  // 256 in the second octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.256.1.1"));
}

TEST(invalid_third_octet_256_b) {
  // 256 in the third octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.1.256.1"));
}

TEST(invalid_fourth_octet_256) {
  // 256 in the fourth octet
  EXPECT_FALSE(sourcemeta::core::is_ipv4("1.1.1.256"));
}

TEST(valid_branch_200_249_low) {
  // dec-octet branch 2 %x30-34 DIGIT, low end
  EXPECT_TRUE(sourcemeta::core::is_ipv4("200.0.0.0"));
}

TEST(valid_branch_200_249_high) {
  // dec-octet branch 2 %x30-34 DIGIT, high end
  EXPECT_TRUE(sourcemeta::core::is_ipv4("249.1.2.3"));
}

TEST(valid_all_two_digit_octets) {
  // every octet in the %x31-39 DIGIT (10-99) branch
  EXPECT_TRUE(sourcemeta::core::is_ipv4("10.20.30.40"));
}

TEST(valid_mixed_all_branches) {
  // one octet from each of the four multi-value branches
  EXPECT_TRUE(sourcemeta::core::is_ipv4("9.42.200.255"));
}
