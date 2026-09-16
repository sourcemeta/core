#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string>      // std::string
#include <string_view> // std::string_view

TEST(empty) {
  std::string output;
  sourcemeta::core::URI::escape_reference("", output);
  EXPECT_EQ(output, "");
}

TEST(unreserved_characters_pass_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("ABCabc0123456789-._~", output);
  EXPECT_EQ(output, "ABCabc0123456789-._~");
}

TEST(sub_delimiters_pass_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("!$&'()*+,;=", output);
  EXPECT_EQ(output, "!$&'()*+,;=");
}

TEST(uri_reference_delimiters_pass_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference(
      "https://user@example.com:8080/a/b?c=d&e=f#g", output);
  EXPECT_EQ(output, "https://user@example.com:8080/a/b?c=d&e=f#g");
}

TEST(space_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("a b", output);
  EXPECT_EQ(output, "a%20b");
}

TEST(valid_percent_triplets_pass_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("a%20b%2Fc", output);
  EXPECT_EQ(output, "a%20b%2Fc");
}

TEST(lowercase_percent_triplet_passes_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("a%2fb", output);
  EXPECT_EQ(output, "a%2fb");
}

TEST(stray_percent_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("100%", output);
  EXPECT_EQ(output, "100%25");
}

TEST(percent_followed_by_one_hexadecimal_digit_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("%2G", output);
  EXPECT_EQ(output, "%252G");
}

TEST(square_brackets_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("a[b]", output);
  EXPECT_EQ(output, "a%5Bb%5D");
}

TEST(characters_outside_uri_references_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("\"<>\\^`{|}", output);
  EXPECT_EQ(output, "%22%3C%3E%5C%5E%60%7B%7C%7D");
}

TEST(control_characters_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference(std::string_view{"\0\t\x7F", 3},
                                          output);
  EXPECT_EQ(output, "%00%09%7F");
}

TEST(non_ascii_bytes_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("caf\xC3\xA9", output);
  EXPECT_EQ(output, "caf%C3%A9");
}

TEST(appends_to_existing_output) {
  std::string output{"href="};
  sourcemeta::core::URI::escape_reference("a b", output);
  EXPECT_EQ(output, "href=a%20b");
}

TEST(ip_literal_host_brackets_pass_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[::1]:8080/a", output);
  EXPECT_EQ(output, "http://[::1]:8080/a");
}

TEST(ip_literal_host_after_userinfo_passes_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("https://user:pass@[v1.fe]/", output);
  EXPECT_EQ(output, "https://user:pass@[v1.fe]/");
}

TEST(ip_literal_host_after_last_at_sign_passes_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://a@b@[::1]", output);
  EXPECT_EQ(output, "http://a@b@[::1]");
}

TEST(ip_literal_host_of_network_path_reference_passes_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("//[2001:db8::7]?q", output);
  EXPECT_EQ(output, "//[2001:db8::7]?q");
}

TEST(square_brackets_outside_the_host_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[::1]/[a]?[b]#[c]", output);
  EXPECT_EQ(output, "http://[::1]/%5Ba%5D?%5Bb%5D#%5Bc%5D");
}

TEST(square_brackets_without_authority_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("urn:[::1]", output);
  EXPECT_EQ(output, "urn:%5B::1%5D");
}

TEST(unterminated_ip_literal_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[::1/a]", output);
  EXPECT_EQ(output, "http://%5B::1/a%5D");
}

TEST(ip_literal_followed_by_other_characters_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[::1]x/", output);
  EXPECT_EQ(output, "http://%5B::1%5Dx/");
}

TEST(number_sign_after_the_fragment_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("/a#b#c#d", output);
  EXPECT_EQ(output, "/a#b%23c%23d");
}

TEST(brackets_around_text_that_is_not_an_ip_literal_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[not-an-ip]/x", output);
  EXPECT_EQ(output, "http://%5Bnot-an-ip%5D/x");
}

TEST(empty_brackets_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[]/x", output);
  EXPECT_EQ(output, "http://%5B%5D/x");
}

TEST(ip_future_literal_host_passes_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[v7.host:1]/x", output);
  EXPECT_EQ(output, "http://[v7.host:1]/x");
}

TEST(brackets_around_an_incomplete_ip_future_literal_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[v.host]/x", output);
  EXPECT_EQ(output, "http://%5Bv.host%5D/x");
}
