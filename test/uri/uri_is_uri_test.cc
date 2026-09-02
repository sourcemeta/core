#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(scheme_minimal) { EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:")); }

TEST(scheme_uppercase) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("HTTP://example.com"));
}

TEST(scheme_mixed_case) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("HtTp://example.com"));
}

TEST(scheme_with_digits) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a0123456789://example.com"));
}

TEST(scheme_with_plus) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a+b://example.com"));
}

TEST(scheme_with_minus) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a-b://example.com"));
}

TEST(scheme_with_dot) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a.b://example.com"));
}

TEST(scheme_with_all_special_characters) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a+-.b://example.com"));
}

TEST(scheme_starting_with_digit) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("1a://example.com"));
}

TEST(scheme_starting_with_plus) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("+a://example.com"));
}

TEST(scheme_starting_with_minus) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("-a://example.com"));
}

TEST(scheme_starting_with_dot) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(".a://example.com"));
}

TEST(scheme_empty) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("://example.com"));
}

TEST(scheme_with_underscore) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("a_b://example.com"));
}

TEST(scheme_with_percent_encoding) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("ht%74tp://example.com"));
}

TEST(scheme_missing_colon) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http//example.com"));
}

TEST(hier_part_authority_with_empty_path) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com"));
}

TEST(hier_part_authority_with_path) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/a/b"));
}

TEST(hier_part_empty_authority) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://"));
}

TEST(hier_part_path_absolute) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:/b/c"));
}

TEST(hier_part_path_absolute_single_slash) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:/"));
}

TEST(hier_part_path_rootless) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:b/c"));
}

TEST(hier_part_path_empty) { EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:")); }

TEST(hier_part_path_empty_with_query) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:?q"));
}

TEST(hier_part_path_empty_with_fragment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:#f"));
}

TEST(hier_part_path_empty_with_query_and_fragment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("a:?q#f"));
}

TEST(userinfo_absent) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p"));
}

TEST(userinfo_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://@example.com/p"));
}

TEST(userinfo_single_character) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://u@example.com/p"));
}

TEST(userinfo_with_colon) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://user:pass@example.com/p"));
}

TEST(userinfo_with_multiple_colons) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://a:b:c@example.com/p"));
}

TEST(userinfo_with_sub_delims) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("http://a!$&'()*+,;=b@example.com/p"));
}

TEST(userinfo_with_unreserved) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://aZ0-._~@example.com/p"));
}

TEST(userinfo_with_percent_encoding) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://us%40er@example.com/p"));
}

TEST(userinfo_with_space) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://us er@example.com/p"));
}

TEST(userinfo_with_caret) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://us^er@example.com/p"));
}

TEST(userinfo_with_second_at_sign) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://a@b@example.com/p"));
}

TEST(host_reg_name_single_character) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://a"));
}

TEST(host_reg_name_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http:///p"));
}

TEST(host_reg_name_with_hyphen) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://ex-ample.com"));
}

TEST(host_reg_name_with_subdomains) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://a.b.c.example.com"));
}

TEST(host_reg_name_with_sub_delims) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://a!$&'()*+,;=b.com"));
}

TEST(host_reg_name_with_trailing_dot) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com."));
}

TEST(host_reg_name_with_underscore) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://ex_ample.com"));
}

TEST(host_reg_name_with_tilde) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://ex~ample.com"));
}

TEST(host_reg_name_with_percent_encoding) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://ex%2Fample.com"));
}

TEST(host_reg_name_with_invalid_percent_encoding) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://ex%2ample.com"));
}

TEST(host_ipv4_minimum) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://0.0.0.0"));
}

TEST(host_ipv4_maximum) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://255.255.255.255"));
}

TEST(host_ipv4_typical) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://192.168.0.1"));
}

TEST(host_ipv4_too_few_octets_is_reg_name) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://1.2.3"));
}

TEST(host_ipv4_too_many_octets_is_reg_name) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://1.2.3.4.5"));
}

TEST(host_ipv4_out_of_range_is_reg_name) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://256.1.1.1"));
}

TEST(host_ipv4_leading_zero_is_reg_name) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://01.2.3.4"));
}

TEST(dec_octet_single_digit_floor) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://0.1.1.1"));
}

TEST(dec_octet_single_digit_ceiling) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://9.1.1.1"));
}

TEST(dec_octet_two_digit_floor) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://10.1.1.1"));
}

TEST(dec_octet_two_digit_ceiling) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://99.1.1.1"));
}

TEST(dec_octet_hundreds_floor) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://100.1.1.1"));
}

TEST(dec_octet_hundreds_ceiling) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://199.1.1.1"));
}

TEST(dec_octet_two_hundreds_floor) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://200.1.1.1"));
}

TEST(dec_octet_two_hundreds_ceiling) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://249.1.1.1"));
}

TEST(dec_octet_two_fifties_floor) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://250.1.1.1"));
}

TEST(dec_octet_two_fifties_ceiling) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://255.1.1.1"));
}

TEST(host_ipv6_full_form) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "http://[2001:db8:85a3:0:0:8a2e:370:7334]"));
}

TEST(host_ipv6_compressed) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[2001:db8::7]"));
}

TEST(host_ipv6_loopback) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[::1]"));
}

TEST(host_ipv6_unspecified) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[::]"));
}

TEST(host_ipv6_leading_compression) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[::8a2e:370:7334]"));
}

TEST(host_ipv6_trailing_compression) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[2001:db8::]"));
}

TEST(host_ipv6_with_port) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[::1]:8080/p"));
}

TEST(host_ipv6_embedded_ipv4) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[::ffff:192.168.1.1]"));
}

TEST(host_ipv6_embedded_ipv4_uncompressed) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[1:2:3:4:5:6:1.2.3.4]"));
}

TEST(host_ipv6_embedded_ipv4_out_of_range) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[::ffff:1.2.3.256]"));
}

TEST(host_ipv6_embedded_ipv4_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[::ffff:01.2.3.4]"));
}

TEST(host_ipv6_missing_brackets) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://2001:db8::7/p"));
}

TEST(host_ipv6_only_opening_bracket) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[2001:db8::7/p"));
}

TEST(host_ipv6_only_closing_bracket) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://2001:db8::7]/p"));
}

TEST(host_ipv6_empty_brackets) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[]/p"));
}

TEST(host_ipv6_group_with_five_hex_digits) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[2001:db8::12345]"));
}

TEST(host_ipv6_with_invalid_hex_digit) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[2001:db8::gggg]"));
}

TEST(host_ipv6_with_two_compressions) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[2001::db8::1]"));
}

TEST(host_ipv6_with_nine_groups) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[1:2:3:4:5:6:7:8:9]"));
}

TEST(host_ipv6_with_seven_groups_and_no_compression) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[1:2:3:4:5:6:7]"));
}

TEST(host_ipvfuture_lowercase_version) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[v1.fe]"));
}

TEST(host_ipvfuture_uppercase_version) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[V1.fe]"));
}

TEST(host_ipvfuture_multi_digit_version) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[vABCDEF.x]"));
}

TEST(host_ipvfuture_with_colon_in_body) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[v1.a:b]"));
}

TEST(host_ipvfuture_with_sub_delims_in_body) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://[v1.a$b]"));
}

TEST(host_ipvfuture_missing_version) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[v.fe]"));
}

TEST(host_ipvfuture_non_hex_version) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[vg.fe]"));
}

TEST(host_ipvfuture_missing_dot) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[v1fe]"));
}

TEST(host_ipvfuture_empty_body) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://[v1.]"));
}

TEST(port_absent) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p"));
}

TEST(port_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:/p"));
}

TEST(port_single_digit) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:8/p"));
}

TEST(port_typical) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:8080/p"));
}

TEST(port_maximum_registered) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:65535/p"));
}

TEST(port_above_sixteen_bits) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:99999999/p"));
}

TEST(port_with_leading_zero) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com:080/p"));
}

TEST(port_with_plus) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:+80/p"));
}

TEST(port_with_minus) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:-80/p"));
}

TEST(port_with_decimal_point) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:80.5/p"));
}

TEST(port_with_space) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:8 0/p"));
}

TEST(port_non_numeric) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:abc/p"));
}

TEST(port_with_percent_encoding) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:%38%30/p"));
}

TEST(port_with_second_colon) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com:80:90/p"));
}

TEST(path_abempty_single_slash) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/"));
}

TEST(path_abempty_deep) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/a/b/c/d"));
}

TEST(path_abempty_empty_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/a//b"));
}

TEST(path_abempty_all_empty_segments) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com///"));
}

TEST(path_with_dot_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/./a"));
}

TEST(path_with_double_dot_segment) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/../a"));
}

TEST(path_segment_with_colon) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/a:b"));
}

TEST(path_segment_with_at_sign) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/a@b"));
}

TEST(path_segment_with_sub_delims) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("http://example.com/a!$&'()*+,;=b"));
}

TEST(path_segment_with_unreserved) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/aZ0-._~"));
}

TEST(path_segment_with_square_brackets) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http:/[::1]"));
}

TEST(path_segment_with_open_brace) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/a{b"));
}

TEST(path_segment_with_caret) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/a^b"));
}

TEST(path_segment_with_backslash) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/a\\b"));
}

TEST(path_segment_with_space) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/a b"));
}

TEST(path_segment_with_double_quote) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/a\"b"));
}

TEST(path_segment_with_angle_brackets) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/a<b>c"));
}

TEST(path_segment_with_pipe) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/a|b"));
}

TEST(path_segment_with_backtick) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/a`b"));
}

TEST(query_absent) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p"));
}

TEST(query_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p?"));
}

TEST(query_key_value) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p?a=b"));
}

TEST(query_multiple_pairs) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p?a=b&c=d"));
}

TEST(query_with_slash) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p?a/b"));
}

TEST(query_with_question_mark) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p?a?b"));
}

TEST(query_with_at_sign) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p?a@b"));
}

TEST(query_with_colon) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p?a:b"));
}

TEST(query_with_percent_encoding) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p?a%20b"));
}

TEST(query_with_square_brackets) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/p?arr[]=1"));
}

TEST(query_with_caret) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/p?a^b"));
}

TEST(query_with_space) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/p?a b"));
}

TEST(fragment_empty) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p#"));
}

TEST(fragment_simple) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p#frag"));
}

TEST(fragment_with_slash) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p#a/b"));
}

TEST(fragment_with_question_mark) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p#a?b"));
}

TEST(fragment_with_at_sign) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p#a@b"));
}

TEST(fragment_with_percent_encoding) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/p#a%20b"));
}

TEST(fragment_with_second_hash) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/p#a#b"));
}

TEST(fragment_with_square_brackets) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/p#a[b]"));
}

TEST(fragment_with_caret) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/p#a^b"));
}

TEST(fragment_with_space) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/p#a b"));
}

TEST(percent_encoding_uppercase_hex) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%2F"));
}

TEST(percent_encoding_lowercase_hex) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%2f"));
}

TEST(percent_encoding_mixed_case_hex) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%2Fa%2f"));
}

TEST(percent_encoding_digits_only) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%20"));
}

TEST(percent_encoding_maximum_value) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%FF"));
}

TEST(percent_encoding_double_encoded) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%2520"));
}

TEST(percent_encoding_null_byte) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%00"));
}

TEST(percent_encoding_overlong_utf8) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("http://example.com/%C0%80"));
}

TEST(percent_encoding_non_hex_letter) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/%G0"));
}

TEST(percent_encoding_single_hex_digit) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/%A"));
}

TEST(percent_encoding_lone_percent) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/%"));
}

TEST(percent_encoding_percent_then_percent) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("http://example.com/%%41"));
}

TEST(relative_path_reference) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("/abc"));
}

TEST(network_path_reference) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("//example.com/p"));
}

TEST(query_only_reference) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("?q=1"));
}

TEST(fragment_only_reference) {
  EXPECT_FALSE(sourcemeta::core::URI::is_uri("#frag"));
}

TEST(empty_string) { EXPECT_FALSE(sourcemeta::core::URI::is_uri("")); }

TEST(bare_word) { EXPECT_FALSE(sourcemeta::core::URI::is_uri("abc")); }

TEST(dot_segment_only) { EXPECT_FALSE(sourcemeta::core::URI::is_uri(".")); }

TEST(rfc_example_ftp) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("ftp://ftp.is.co.za/rfc/rfc1808.txt"));
}

TEST(rfc_example_http) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("http://www.ietf.org/rfc/rfc2396.txt"));
}

TEST(rfc_example_ldap) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "ldap://[2001:db8::7]/c=GB?objectClass?one"));
}

TEST(rfc_example_mailto) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("mailto:John.Doe@example.com"));
}

TEST(rfc_example_news) {
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri("news:comp.infosystems.www.servers.unix"));
}

TEST(rfc_example_tel) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("tel:+1-816-555-1212"));
}

TEST(rfc_example_telnet) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("telnet://192.0.2.16:80/"));
}

TEST(rfc_example_urn) {
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(
      "urn:oasis:names:specification:docbook:dtd:xml:4.1.2"));
}

TEST(host_with_non_ascii) {
  const std::string input{"http://\xE4\xBE\x8B.example/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(path_with_non_ascii) {
  const std::string input{"http://example.com/caf\xC3\xA9"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(query_with_non_ascii) {
  const std::string input{"http://example.com/?q=\xC3\xA9"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(fragment_with_non_ascii) {
  const std::string input{"http://example.com/#\xC3\xA9"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(userinfo_with_non_ascii) {
  const std::string input{"http://us\xC3\xA9r@example.com/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(path_with_right_to_left_override) {
  // NOLINTNEXTLINE(misc-misleading-bidirectional)
  const std::string input{"http://example.com/a\xE2\x80\xAE"
                          "b"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(path_with_zero_width_space) {
  const std::string input{"http://example.com/a\xE2\x80\x8B"
                          "b"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(path_with_null_byte) {
  const std::string input{"http://example.com/a\x00"
                          "b",
                          22};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(path_with_delete_character) {
  const std::string input{"http://example.com/a\x7F"
                          "b"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(path_with_line_feed) {
  const std::string input{"http://example.com/a\x0A"
                          "b"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(trailing_line_feed) {
  const std::string input{"http://example.com/p\x0A"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(trailing_carriage_return) {
  const std::string input{"http://example.com/p\x0D"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(trailing_tab) {
  const std::string input{"http://example.com/p\x09"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}

TEST(leading_space) {
  const std::string input{" http://example.com/p"};
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
}
