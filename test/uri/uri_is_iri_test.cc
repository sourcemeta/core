#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_is_iri, ascii_https_absolute) {
  EXPECT_TRUE(sourcemeta::core::URI::is_iri("https://example.com/path"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_iri_reference("https://example.com/path"));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri("https://example.com/path"));
  EXPECT_TRUE(
      sourcemeta::core::URI::is_uri_reference("https://example.com/path"));
}

TEST(URI_is_iri, fragment_with_ucschar) {
  const std::string input{"https://example.com/#section-\xCE\xB1"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, fragment_with_iprivate_rejected) {
  const std::string input{"https://example.com/#\xEE\x80\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, fragment_with_invalid_utf8) {
  const std::string input{"https://example.com/#\xC3"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, query_with_ucschar) {
  const std::string input{"https://example.com/?key=caf\xC3\xA9"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, query_with_iprivate) {
  const std::string input{"https://example.com/?\xEE\x80\x80"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, query_with_invalid_utf8) {
  const std::string input{"https://example.com/?\xC0"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, path_with_ucschar) {
  const std::string input{"https://example.com/\xE8\xB7\xAF\xE5\xBE\x84"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, path_with_iprivate_rejected) {
  const std::string input{"https://example.com/\xEE\x80\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, relative_path_with_ucschar) {
  const std::string input{"\xE8\xB7\xAF\xE5\xBE\x84/sub"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, host_with_ucschar) {
  const std::string input{"https://\xE4\xBE\x8B\xE3\x81\x88.jp/"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, userinfo_with_ucschar) {
  const std::string input{"https://user\xE5\x90\x8D@example.com/"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, host_with_iprivate_rejected) {
  const std::string input{"https://\xEE\x80\x80.example/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, ucschar_lower_bound_in_path) {
  const std::string input{"https://example.com/\xC2\xA0"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, control_below_ucschar_in_path_rejected) {
  const std::string input{"https://example.com/\xC2\x9F"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, surrogate_encoded_as_utf8_rejected) {
  const std::string input{"https://example.com/\xED\xA0\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, overlong_two_byte_rejected) {
  const std::string input{"https://example.com/\xC0\xAF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, overlong_three_byte_for_ucschar_rejected) {
  const std::string input{"https://example.com/\xE0\x82\xAF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, overlong_four_byte_for_ucschar_rejected) {
  const std::string input{"https://example.com/\xF0\x80\x82\xAF"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, full_internationalised_iri) {
  const std::string input{
      "https://user\xE5\x90\x8D@\xE4\xBE\x8B\xE3\x81\x88.jp/"
      "\xE8\xB7\xAF\xE5\xBE\x84?q=caf\xC3\xA9#section-\xCE\xB1"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, percent_encoded_non_ascii_is_also_valid_uri) {
  const std::string input{"https://example.com/caf%C3%A9"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, scheme_with_non_ascii_rejected) {
  const std::string input{"h\xC3\xA9ttp://example.com/"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, ucs_in_host_query_and_fragment) {
  const std::string input{"http://\xC6\x92\xC3\xB8\xC3\xB8.\xC3\x9F\xC3\xA5r/?"
                          "\xE2\x88\x82\xC3\xA9\xC5\x93=\xCF\x80\xC3\xAEx#"
                          "\xCF\x80\xC3\xAE\xC3\xBCx"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, parens_and_ucs_in_path_and_fragment) {
  const std::string input{
      "http://\xC6\x92\xC3\xB8\xC3\xB8.com/blah_(w\xC3\xAEk\xC3\xAFp\xC3\xA9"
      "di\xC3\xA5)_blah#\xC3\x9Fit\xC3\xA9-1"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, ucs_host_with_percent_encoded_query) {
  const std::string input{"http://\xC6\x92\xC3\xB8\xC3\xB8.\xC3\x9F\xC3\xA5r/"
                          "?q=Test%20URL-encoded%20stuff"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, heavy_subdelims_and_colons_in_userinfo) {
  const std::string input{"http://-.~_!$&'()*+,;=:%40:80%2f::::::@example.com"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, ipv6_host_in_brackets) {
  const std::string input{"http://[2001:0db8:85a3:0000:0000:8a2e:0370:7334]"};
  EXPECT_TRUE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, ipv6_host_without_brackets_rejected) {
  const std::string input{"http://2001:0db8:85a3:0000:0000:8a2e:0370:7334"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, backslashes_with_ucs_rejected) {
  const std::string input{"\\\\WINDOWS\\fil\xC3\xAB\xC3\x9F\xC3\xA5r\xC3\xA9"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, only_ucs_chars_is_relative_iri_reference) {
  const std::string input{"\xC3\xA2\xCF\x80\xCF\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, protocol_relative_with_ucs_is_iri_reference) {
  const std::string input{"//\xC6\x92\xC3\xB8\xC3\xB8.\xC3\x9F\xC3\xA5r/?"
                          "\xE2\x88\x82\xC3\xA9\xC5\x93=\xCF\x80\xC3\xAEx#"
                          "\xCF\x80\xC3\xAE\xC3\xBCx"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, absolute_path_with_ucs_is_iri_reference) {
  const std::string input{"/\xC3\xA2\xCF\x80\xCF\x80"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, fragment_only_with_ucs_is_iri_reference) {
  const std::string input{"#\xC6\x92r\xC3\xA4gm\xC3\xAAnt"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_TRUE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}

TEST(URI_is_iri, fragment_with_backslash_rejected) {
  const std::string input{"#\xC6\x92r\xC3\xA4g\\m\xC3\xAAnt"};
  EXPECT_FALSE(sourcemeta::core::URI::is_iri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_iri_reference(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri(input));
  EXPECT_FALSE(sourcemeta::core::URI::is_uri_reference(input));
}
