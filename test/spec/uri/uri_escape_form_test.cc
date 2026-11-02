#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string> // std::string

TEST(alphanumerics_pass_through) {
  std::string output;
  sourcemeta::core::URI::escape_form("abcXYZ019", output);
  EXPECT_EQ(output, "abcXYZ019");
}

TEST(the_unencoded_punctuation_passes_through) {
  std::string output;
  sourcemeta::core::URI::escape_form("*-._", output);
  EXPECT_EQ(output, "*-._");
}

TEST(space_becomes_plus) {
  std::string output;
  sourcemeta::core::URI::escape_form("a b c", output);
  EXPECT_EQ(output, "a+b+c");
}

TEST(tilde_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_form("~", output);
  EXPECT_EQ(output, "%7E");
}

TEST(asterisk_is_not_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_form("*", output);
  EXPECT_EQ(output, "*");
}

TEST(plus_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_form("+", output);
  EXPECT_EQ(output, "%2B");
}

TEST(percent_is_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_form("%", output);
  EXPECT_EQ(output, "%25");
}

TEST(delimiters_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_form("&=", output);
  EXPECT_EQ(output, "%26%3D");
}

TEST(reserved_characters_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_form("/?:@!$'()", output);
  EXPECT_EQ(output, "%2F%3F%3A%40%21%24%27%28%29");
}

TEST(non_ascii_is_encoded_in_uppercase_hexadecimal) {
  std::string output;
  sourcemeta::core::URI::escape_form("\xC2\xA3", output);
  EXPECT_EQ(output, "%C2%A3");
}

TEST(control_characters_are_encoded) {
  std::string output;
  sourcemeta::core::URI::escape_form("\n\t", output);
  EXPECT_EQ(output, "%0A%09");
}

TEST(empty_input) {
  std::string output;
  sourcemeta::core::URI::escape_form("", output);
  EXPECT_EQ(output, "");
}

TEST(appends_to_existing_output) {
  std::string output{"prefix="};
  sourcemeta::core::URI::escape_form("a b", output);
  EXPECT_EQ(output, "prefix=a+b");
}

TEST(rfc6749_appendix_b_example) {
  std::string output;
  sourcemeta::core::URI::escape_form(" %&+\xC2\xA3\xE2\x82\xAC", output);
  EXPECT_EQ(output, "+%25%26%2B%C2%A3%E2%82%AC");
}
