#include <sourcemeta/core/test.h>

#include <sourcemeta/core/uri.h>

TEST(ascii_https_absolute) {
  EXPECT_TRUE(sourcemeta::core::URI::is_iri("https://example.com/path"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_iri_reference("https://example.com/path"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("https://example.com/path"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("https://example.com/path"));
}

TEST(fragment_with_ucschar) {
  const std::string input{"https://example.com/#section-\xCE\xB1"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(fragment_with_iprivate_rejected) {
  const std::string input{"https://example.com/#\xEE\x80\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(fragment_with_invalid_utf8) {
  const std::string input{"https://example.com/#\xC3"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(query_with_ucschar) {
  const std::string input{"https://example.com/?key=caf\xC3\xA9"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(query_with_iprivate) {
  const std::string input{"https://example.com/?\xEE\x80\x80"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(query_with_invalid_utf8) {
  const std::string input{"https://example.com/?\xC0"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(path_with_ucschar) {
  const std::string input{"https://example.com/\xE8\xB7\xAF\xE5\xBE\x84"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(path_with_iprivate_rejected) {
  const std::string input{"https://example.com/\xEE\x80\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(relative_path_with_ucschar) {
  const std::string input{"\xE8\xB7\xAF\xE5\xBE\x84/sub"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(host_with_ucschar) {
  const std::string input{"https://\xE4\xBE\x8B\xE3\x81\x88.jp/"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(userinfo_with_ucschar) {
  const std::string input{"https://user\xE5\x90\x8D@example.com/"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(host_with_iprivate_rejected) {
  const std::string input{"https://\xEE\x80\x80.example/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ucschar_lower_bound_in_path) {
  const std::string input{"https://example.com/\xC2\xA0"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(control_below_ucschar_in_path_rejected) {
  const std::string input{"https://example.com/\xC2\x9F"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(surrogate_encoded_as_utf8_rejected) {
  const std::string input{"https://example.com/\xED\xA0\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(overlong_two_byte_rejected) {
  const std::string input{"https://example.com/\xC0\xAF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(overlong_three_byte_for_ucschar_rejected) {
  const std::string input{"https://example.com/\xE0\x82\xAF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(overlong_four_byte_for_ucschar_rejected) {
  const std::string input{"https://example.com/\xF0\x80\x82\xAF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(full_internationalised_iri) {
  const std::string input{
      "https://user\xE5\x90\x8D@\xE4\xBE\x8B\xE3\x81\x88.jp/"
      "\xE8\xB7\xAF\xE5\xBE\x84?q=caf\xC3\xA9#section-\xCE\xB1"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(percent_encoded_non_ascii_is_also_valid_uri) {
  const std::string input{"https://example.com/caf%C3%A9"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(scheme_with_non_ascii_rejected) {
  const std::string input{"h\xC3\xA9ttp://example.com/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ucs_in_host_query_and_fragment) {
  const std::string input{"http://\xC6\x92\xC3\xB8\xC3\xB8.\xC3\x9F\xC3\xA5r/?"
                          "\xE2\x88\x82\xC3\xA9\xC5\x93=\xCF\x80\xC3\xAEx#"
                          "\xCF\x80\xC3\xAE\xC3\xBCx"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(parens_and_ucs_in_path_and_fragment) {
  const std::string input{
      "http://\xC6\x92\xC3\xB8\xC3\xB8.com/blah_(w\xC3\xAEk\xC3\xAFp\xC3\xA9"
      "di\xC3\xA5)_blah#\xC3\x9Fit\xC3\xA9-1"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ucs_host_with_percent_encoded_query) {
  const std::string input{"http://\xC6\x92\xC3\xB8\xC3\xB8.\xC3\x9F\xC3\xA5r/"
                          "?q=Test%20URL-encoded%20stuff"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(heavy_subdelims_and_colons_in_userinfo) {
  const std::string input{"http://-.~_!$&'()*+,;=:%40:80%2f::::::@example.com"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ipv6_host_in_brackets) {
  const std::string input{"http://[2001:0db8:85a3:0000:0000:8a2e:0370:7334]"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ipv6_host_without_brackets_rejected) {
  const std::string input{"http://2001:0db8:85a3:0000:0000:8a2e:0370:7334"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(backslashes_with_ucs_rejected) {
  const std::string input{"\\\\WINDOWS\\fil\xC3\xAB\xC3\x9F\xC3\xA5r\xC3\xA9"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(only_ucs_chars_is_relative_iri_reference) {
  const std::string input{"\xC3\xA2\xCF\x80\xCF\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(protocol_relative_with_ucs_is_iri_reference) {
  const std::string input{"//\xC6\x92\xC3\xB8\xC3\xB8.\xC3\x9F\xC3\xA5r/?"
                          "\xE2\x88\x82\xC3\xA9\xC5\x93=\xCF\x80\xC3\xAEx#"
                          "\xCF\x80\xC3\xAE\xC3\xBCx"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(absolute_path_with_ucs_is_iri_reference) {
  const std::string input{"/\xC3\xA2\xCF\x80\xCF\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(fragment_only_with_ucs_is_iri_reference) {
  const std::string input{"#\xC6\x92r\xC3\xA4gm\xC3\xAAnt"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(fragment_with_backslash_rejected) {
  const std::string input{"#\xC6\x92r\xC3\xA4g\\m\xC3\xAAnt"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ucschar_before_surrogate_gap_in_path) {
  const std::string input{"https://example.com/\xED\x9F\xBF"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ucschar_after_private_use_gap_in_path) {
  const std::string input{"https://example.com/\xEF\xA4\x80"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ucschar_before_noncharacter_block_in_path) {
  const std::string input{"https://example.com/\xEF\xB7\x8F"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(noncharacter_fdd0_in_path_rejected) {
  const std::string input{"https://example.com/\xEF\xB7\x90"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(noncharacter_fdef_in_path_rejected) {
  const std::string input{"https://example.com/\xEF\xB7\xAF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ucschar_after_noncharacter_block_in_path) {
  const std::string input{"https://example.com/\xEF\xB7\xB0"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ucschar_upper_bmp_bound_in_path) {
  const std::string input{"https://example.com/\xEF\xBF\xAF"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(replacement_character_in_path_rejected) {
  const std::string input{"https://example.com/\xEF\xBF\xBD"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(noncharacter_fffe_in_path_rejected) {
  const std::string input{"https://example.com/\xEF\xBF\xBE"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(noncharacter_ffff_in_path_rejected) {
  const std::string input{"https://example.com/\xEF\xBF\xBF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(plane_fourteen_below_ucschar_in_path_rejected) {
  const std::string input{"https://example.com/\xF3\xA0\xBF\xBF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(plane_fourteen_ucschar_in_path) {
  const std::string input{"https://example.com/\xF3\xA1\x80\x80"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(astral_ucschar_in_path) {
  const std::string input{"https://example.com/\xF0\x90\x80\x80"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(astral_ucschar_upper_bound_in_path) {
  const std::string input{"https://example.com/\xF0\x9F\xBF\xBD"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(astral_noncharacter_in_path_rejected) {
  const std::string input{"https://example.com/\xF0\x9F\xBF\xBE"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(astral_plane_two_ucschar_in_path) {
  const std::string input{"https://example.com/\xF0\xA0\x80\x80"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(userinfo_with_iprivate_rejected) {
  const std::string input{"https://u\xEE\x80\x80@example.com/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(port_with_iprivate_rejected) {
  const std::string input{"https://example.com:\xEE\x80\x80/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(supplementary_iprivate_in_query) {
  const std::string input{"https://example.com/?q=\xF3\xB0\x80\x80"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(supplementary_iprivate_in_path_rejected) {
  const std::string input{"https://example.com/\xF3\xB0\x80\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(plane_sixteen_iprivate_in_query) {
  const std::string input{"https://example.com/?q=\xF4\x80\x80\x80"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(iprivate_upper_bound_in_query) {
  const std::string input{"https://example.com/?q=\xF4\x8F\xBF\xBD"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ipv4_host) {
  const std::string input{"http://192.168.0.1/p"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ipvfuture_host_lowercase) {
  const std::string input{"http://[v1.fe]/"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ipvfuture_host_uppercase) {
  const std::string input{"http://[V1.fe]/"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(ipvfuture_host_non_hex_version_rejected) {
  const std::string input{"http://[vG.fe]/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(opaque_path_with_scheme) {
  const std::string input{"urn:example:resource"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(absolute_path_with_scheme) {
  const std::string input{"file:/etc/hosts"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(explicit_port) {
  const std::string input{"http://example.com:8080/p"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(scheme_with_empty_path) {
  const std::string input{"http:"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(scheme_with_empty_authority) {
  const std::string input{"http://"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}
