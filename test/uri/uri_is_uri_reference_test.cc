#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(uri_branch_full) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("http://example.com/a/b?q=1#f"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/a/b?q=1#f"));
}

TEST(uri_branch_scheme_only) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("a:"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:"));
}

TEST(uri_branch_path_rootless) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("a:b"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:b"));
}

TEST(uri_branch_colon_in_rootless_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("a:b:c"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:b:c"));
}

TEST(relative_branch_network_path) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//example.com/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com/a"));
}

TEST(relative_branch_absolute_path) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/a/b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a/b"));
}

TEST(relative_branch_no_scheme) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("a/b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("a/b"));
}

TEST(relative_branch_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(""));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(""));
}

TEST(network_path_bare) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//"));
}

TEST(network_path_empty_authority_with_path) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("///a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("///a"));
}

TEST(network_path_host_only) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//example.com"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com"));
}

TEST(network_path_trailing_slash) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//example.com/"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com/"));
}

TEST(absolute_path_bare) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/"));
}

TEST(absolute_path_single_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a"));
}

TEST(absolute_path_empty_trailing_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/a/"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a/"));
}

TEST(absolute_path_dot_blocks_authority) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/.//"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/.//"));
}

TEST(no_scheme_dot_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("."));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("."));
}

TEST(no_scheme_dot_dot_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(".."));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(".."));
}

TEST(no_scheme_at_sign) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("@"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("@"));
}

TEST(path_empty_with_query) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("?q=1"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?q=1"));
}

TEST(path_empty_with_fragment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("#f"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#f"));
}

TEST(path_empty_with_both) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("?q=1#f"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?q=1#f"));
}

TEST(first_segment_colon_digit_start) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("1:b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("1:b"));
}

TEST(first_segment_colon_digit_start_long) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("9foo:bar"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("9foo:bar"));
}

TEST(first_segment_colon_dot_start) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(".a:b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(".a:b"));
}

TEST(first_segment_colon_hyphen_start) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("-x:y"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("-x:y"));
}

TEST(first_segment_colon_plus_start) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("+x:y"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("+x:y"));
}

TEST(first_segment_colon_alone) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(":"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(":"));
}

TEST(first_segment_colon_then_slash) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(":/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(":/a"));
}

TEST(first_segment_scheme_shaped_is_a_uri) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("this:that"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("this:that"));
}

TEST(first_segment_colon_after_dot_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("./this:that"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("./this:that"));
}

TEST(colon_in_a_later_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("a/b:c"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("a/b:c"));
}

TEST(colon_in_a_later_segment_absolute) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/a:b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a:b"));
}

TEST(percent_encoded_colon_is_not_a_colon) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("%3Ab"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("%3Ab"));
}

TEST(percent_encoded_colon_lowercase_hex) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("%3ab"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("%3ab"));
}

TEST(userinfo_present) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//user@example.com/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//user@example.com/a"));
}

TEST(userinfo_with_colon) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("//user:pass@example.com/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//user:pass@example.com/a"));
}

TEST(userinfo_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//@example.com/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//@example.com/a"));
}

TEST(userinfo_percent_encoded) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//%41@example.com/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//%41@example.com/a"));
}

TEST(more_than_one_at_sign) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//a@b@example.com/"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//a@b@example.com/"));
}

TEST(port_present) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//example.com:80/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com:80/a"));
}

TEST(port_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//example.com:/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com:/a"));
}

TEST(port_zero) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//example.com:0/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com:0/a"));
}

TEST(port_non_numeric) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//example.com:abc/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com:abc/a"));
}

TEST(port_with_sign) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//example.com:+80/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com:+80/a"));
}

TEST(host_empty_with_port) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//:80/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//:80/a"));
}

TEST(host_empty_with_empty_port) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//:/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//:/a"));
}

TEST(reg_name_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//"));
}

TEST(reg_name_sub_delims) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("//a!b$c&d'e(f)g*h+i,j;k=l/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//a!b$c&d'e(f)g*h+i,j;k=l/p"));
}

TEST(reg_name_percent_encoded) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//ex%41mple.com/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//ex%41mple.com/p"));
}

TEST(reg_name_digits_and_dots_fall_back) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//999.999.999.999/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//999.999.999.999/p"));
}

TEST(reg_name_leading_zero_falls_back) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//087.10.0.1/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//087.10.0.1/p"));
}

TEST(ipv4_address) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//192.0.2.1/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//192.0.2.1/p"));
}

TEST(ip_literal_ipv6_loopback) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[::1]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::1]/p"));
}

TEST(ip_literal_ipv6_all_zero) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[::]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::]/p"));
}

TEST(ip_literal_ipv6_full) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:8]/p"));
}

TEST(ip_literal_ipv6_embedded_ipv4) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:1.2.3.4]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:1.2.3.4]/p"));
}

TEST(ip_literal_ipv6_embedded_ipv4_leading_zero) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[::ffff:192.168.0.01]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::ffff:192.168.0.01]/p"));
}

TEST(ip_literal_ipv6_embedded_ipv4_out_of_range) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[::ffff:1.2.3.256]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::ffff:1.2.3.256]/p"));
}

TEST(ip_literal_ipv6_too_many_groups) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:8:9]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:8:9]/p"));
}

TEST(ip_literal_ipv6_h16_too_long) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[12345::]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[12345::]/p"));
}

TEST(ip_literal_empty) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[]/p"));
}

TEST(ip_literal_unterminated) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[::1/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::1/p"));
}

TEST(ip_literal_trailing_content) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[::1]x/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::1]x/p"));
}

TEST(ip_literal_brackets_in_a_path) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/[::1]"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/[::1]"));
}

TEST(ipvfuture_lowercase) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[v1.fe]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[v1.fe]/p"));
}

TEST(ipvfuture_uppercase_version) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[V1.fe]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[V1.fe]/p"));
}

TEST(ipvfuture_uppercase_hex) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[vF.fe]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[vF.fe]/p"));
}

TEST(ipvfuture_colon_in_address) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[v1.a:b]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[v1.a:b]/p"));
}

TEST(ipvfuture_no_hex_digits) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[v.a]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[v.a]/p"));
}

TEST(ipvfuture_empty_address) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[v1.]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[v1.]/p"));
}

TEST(ipvfuture_non_hex_version) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[vg.a]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[vg.a]/p"));
}

TEST(ipvfuture_percent_encoding_not_allowed) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[v1.%41]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[v1.%41]/p"));
}

TEST(ipv6_alt1_six_groups_then_ls32) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:8]/p"));
}

TEST(ipv6_alt2_leading_double_colon) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[::2:3:4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::2:3:4:5:6:7:8]/p"));
}

TEST(ipv6_alt3_one_group_before) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1::3:4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1::3:4:5:6:7:8]/p"));
}

TEST(ipv6_alt4_two_groups_before) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2::4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2::4:5:6:7:8]/p"));
}

TEST(ipv6_alt5_three_groups_before) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2:3::5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3::5:6:7:8]/p"));
}

TEST(ipv6_alt6_four_groups_before) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4::6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4::6:7:8]/p"));
}

TEST(ipv6_alt7_five_groups_before) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5::7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5::7:8]/p"));
}

TEST(ipv6_alt8_six_groups_before) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6::8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6::8]/p"));
}

TEST(ipv6_alt9_seven_groups_before) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7::]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7::]/p"));
}

TEST(ipv6_alt9_bare_double_colon) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[::]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::]/p"));
}

TEST(ipv6_alt1_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:8:9]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:8:9]/p"));
}

TEST(ipv6_alt2_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[::1:2:3:4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[::1:2:3:4:5:6:7:8]/p"));
}

TEST(ipv6_alt3_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1::2:3:4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1::2:3:4:5:6:7:8]/p"));
}

TEST(ipv6_alt4_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2::3:4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2::3:4:5:6:7:8]/p"));
}

TEST(ipv6_alt5_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3::4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3::4:5:6:7:8]/p"));
}

TEST(ipv6_alt6_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4::5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4::5:6:7:8]/p"));
}

TEST(ipv6_alt7_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5::6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5::6:7:8]/p"));
}

TEST(ipv6_alt8_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6::7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6::7:8]/p"));
}

TEST(ipv6_alt9_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:8::]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:8::]/p"));
}

TEST(ipv6_under_by_one) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7]/p"));
}

TEST(ipv6_two_compressions) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[1::2::3]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1::2::3]/p"));
}

TEST(h16_four_digits_trailing_slot) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:aBcD]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:aBcD]/p"));
}

TEST(h16_five_digits_trailing_slot) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:12345]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:12345]/p"));
}

TEST(h16_five_digits_leading_slot) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[12345::]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[12345::]/p"));
}

TEST(h16_empty_trailing_slot) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:]/p"));
}

TEST(h16_non_hex_digit) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:g]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:g]/p"));
}

TEST(ls32_h16_pair_branch) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:8]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:8]/p"));
}

TEST(ls32_ipv4_branch) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:1.2.3.4]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:1.2.3.4]/p"));
}

TEST(ls32_ipv4_branch_over_by_one) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:7:1.2.3.4]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:7:1.2.3.4]/p"));
}

TEST(ls32_ipv4_three_octets) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:1.2.3]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:1.2.3]/p"));
}

TEST(ls32_ipv4_five_octets) {
  EXPECT_FALSE(
      sourcemeta::core::URI::is_uri_reference("//[1:2:3:4:5:6:1.2.3.4.5]/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//[1:2:3:4:5:6:1.2.3.4.5]/p"));
}

TEST(percent_encoded_uppercase_hex) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/%41"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/%41"));
}

TEST(percent_encoded_lowercase_hex) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/%2f"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/%2f"));
}

TEST(percent_encoded_mixed_case_hex) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("/%aB"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/%aB"));
}

TEST(percent_alone) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/%"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/%"));
}

TEST(percent_one_digit) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/%4"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/%4"));
}

TEST(percent_non_hex) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/%zz"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/%zz"));
}

TEST(percent_one_non_hex) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/%4g"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/%4g"));
}

TEST(percent_then_percent) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/%%41"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/%%41"));
}

TEST(percent_in_query) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("?%41"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?%41"));
}

TEST(percent_in_fragment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("#%41"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#%41"));
}

TEST(percent_in_reg_name) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//%41/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//%41/p"));
}

TEST(percent_in_userinfo) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("//%41@example.com/p"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//%41@example.com/p"));
}

TEST(query_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("?"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?"));
}

TEST(query_with_question_mark) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("?a?b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?a?b"));
}

TEST(query_with_slash) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("?a/b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?a/b"));
}

TEST(query_with_colon_and_at) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("?a:b@c"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?a:b@c"));
}

TEST(fragment_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("#"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#"));
}

TEST(fragment_with_question_mark) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("#a?b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#a?b"));
}

TEST(fragment_with_slash) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("#a/b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#a/b"));
}

TEST(more_than_one_hash) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("#a#b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#a#b"));
}

TEST(hash_before_question_mark_is_all_fragment) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("#a?b#c"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#a?b#c"));
}

TEST(space) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a b"));
}

TEST(double_quote) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a\"b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a\"b"));
}

TEST(backslash) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a\\b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a\\b"));
}

TEST(caret) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a^b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a^b"));
}

TEST(backtick) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a`b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a`b"));
}

TEST(curly_braces) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a{b}"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a{b}"));
}

TEST(angle_brackets) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a<b>"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a<b>"));
}

TEST(pipe) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a|b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a|b"));
}

TEST(delete_character) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a\x7F"
                                                       "b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a\x7F"
                                             "b"));
}

TEST(tab) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a\tb"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a\tb"));
}

TEST(carriage_return) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a\rb"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a\rb"));
}

TEST(line_feed) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a\nb"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a\nb"));
}

TEST(trailing_line_feed) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/a\n"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/a\n"));
}

TEST(leading_line_feed) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("\n/a"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("\n/a"));
}

TEST(line_feed_alone) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("\n"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("\n"));
}

TEST(nul) {
  const std::string_view input{"/a\x00"
                               "b",
                               4};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(non_ascii_latin) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/caf\xC3\xA9"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/caf\xC3\xA9"));
}

TEST(non_ascii_astral) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("/\xF0\x9F\x98\x80"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/\xF0\x9F\x98\x80"));
}

TEST(scheme_single_letter) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("a:b"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:b"));
}

TEST(scheme_uppercase) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("HTTP:b"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("HTTP:b"));
}

TEST(scheme_with_digits_plus_minus_dot) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference("a1+b-c.d:e"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a1+b-c.d:e"));
}

TEST(scheme_starting_with_a_digit_is_relative) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("1a:b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("1a:b"));
}

TEST(scheme_starting_with_plus_is_relative) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("+a:b"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("+a:b"));
}

TEST(scheme_with_underscore_is_relative) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference("a_b:c"));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("a_b:c"));
}
