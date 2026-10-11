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

// RFC 3986 Section 3.2.2 writes IPvFuture as "v" 1*HEXDIG "." 1*( unreserved /
// sub-delims / ":" ), and RFC 5234 Section 2.3 reads a literal of a grammar
// without regard to case, which the version marker is
TEST(brackets_around_an_uppercase_ip_future_literal_pass_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[V7.host]/x", output);
  EXPECT_EQ(output, "http://[V7.host]/x");
}

TEST(brackets_around_an_ip_future_literal_of_a_sub_delimiter_pass_through) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[v7.a$b]/x", output);
  EXPECT_EQ(output, "http://[v7.a$b]/x");
}

TEST(brackets_around_an_ip_future_literal_without_a_point_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[v7]/x", output);
  EXPECT_EQ(output, "http://%5Bv7%5D/x");
}

TEST(brackets_around_an_ip_future_version_cut_short_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[v7x]/x", output);
  EXPECT_EQ(output, "http://%5Bv7x%5D/x");
}

TEST(
    brackets_around_an_ip_future_literal_with_nothing_after_the_point_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[v7.]/x", output);
  EXPECT_EQ(output, "http://%5Bv7.%5D/x");
}

TEST(brackets_around_an_ip_future_literal_of_a_delimiter_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://[v7.a%b]/x", output);
  EXPECT_EQ(output, "http://%5Bv7.a%25b%5D/x");
}

// RFC 3986 Section 3.2 has an authority follow a "//", which only a valid
// scheme or the start of the reference can precede, so brackets anywhere else
// are no IP literal
TEST(brackets_after_a_prefix_that_is_no_scheme_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("1x://[::1]/", output);
  EXPECT_EQ(output, "1x://%5B::1%5D/");
}

TEST(brackets_in_a_path_rather_than_a_host_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http://a/[x]", output);
  EXPECT_EQ(output, "http://a/%5Bx%5D");
}

TEST(brackets_after_an_empty_authority_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_reference("http:///[x]", output);
  EXPECT_EQ(output, "http:///%5Bx%5D");
}
