#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string> // std::string

TEST(plain_value_passes_through) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("foobar", output));
  EXPECT_EQ(output, "foobar");
}

TEST(plus_becomes_space) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("foo+bar", output));
  EXPECT_EQ(output, "foo bar");
}

TEST(percent_twenty_becomes_space) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("foo%20bar", output));
  EXPECT_EQ(output, "foo bar");
}

TEST(percent_two_f_becomes_slash) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("path%2Fto", output));
  EXPECT_EQ(output, "path/to");
}

TEST(uppercase_hex) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("%C3%A9", output));
  EXPECT_EQ(output, "\xc3\xa9");
}

TEST(lowercase_hex) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("%c3%a9", output));
  EXPECT_EQ(output, "\xc3\xa9");
}

TEST(encoded_plus_stays_a_plus) {
  // "%2B" is a literal "+", distinct from a "+" that means a space, which is
  // the round trip the plain percent decoder cannot preserve
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("a%2Bb", output));
  EXPECT_EQ(output, "a+b");
}

TEST(plus_and_encoded_plus_together) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("a+b%2Bc", output));
  EXPECT_EQ(output, "a b+c");
}

TEST(mixed_plus_and_percent) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("a+b%20c%2Fd", output));
  EXPECT_EQ(output, "a b c/d");
}

TEST(unreserved_characters_pass_through) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("ABCabc0123-._~", output));
  EXPECT_EQ(output, "ABCabc0123-._~");
}

TEST(empty_input_succeeds) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("", output));
  EXPECT_TRUE(output.empty());
}

TEST(appends_to_an_existing_output) {
  std::string output{"key="};
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("a+b", output));
  EXPECT_EQ(output, "key=a b");
}

TEST(rejects_a_trailing_percent) {
  std::string output;
  EXPECT_FALSE(sourcemeta::core::URI::unescape_form("abc%", output));
}

TEST(rejects_an_incomplete_sequence) {
  std::string output;
  EXPECT_FALSE(sourcemeta::core::URI::unescape_form("a%2", output));
}

TEST(rejects_a_non_hex_digit) {
  std::string output;
  EXPECT_FALSE(sourcemeta::core::URI::unescape_form("a%2Gb", output));
}

TEST(rejects_a_percent_before_the_end) {
  std::string output;
  EXPECT_FALSE(sourcemeta::core::URI::unescape_form("a%b", output));
}

TEST(restores_the_output_on_failure) {
  // The prior content and every byte decoded before the malformed escape are
  // rolled back, so a failed decode never leaves a partial value behind
  std::string output{"prefix:"};
  EXPECT_FALSE(sourcemeta::core::URI::unescape_form("good%20then%2", output));
  EXPECT_EQ(output, "prefix:");
}

TEST(decodes_a_null_byte) {
  std::string output;
  EXPECT_TRUE(sourcemeta::core::URI::unescape_form("a%00b", output));
  EXPECT_EQ(output.size(), 3);
  EXPECT_EQ(output.front(), 'a');
  EXPECT_EQ(static_cast<unsigned char>(output[1]), 0x00);
  EXPECT_EQ(output.back(), 'b');
}
