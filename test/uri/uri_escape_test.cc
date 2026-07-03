#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string>      // std::string
#include <string_view> // std::string_view

TEST(unreserved_characters_pass_through) {
  EXPECT_EQ(sourcemeta::core::URI::escape("ABCabc0123456789-._~"),
            "ABCabc0123456789-._~");
}

TEST(space) {
  EXPECT_EQ(sourcemeta::core::URI::escape("foo bar"), "foo%20bar");
}

TEST(slash_is_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("path/to/key"), "path%2Fto%2Fkey");
}

TEST(percent_is_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("100%"), "100%25");
}

TEST(sub_delimiters_are_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("a=b&c"), "a%3Db%26c");
}

TEST(plus_sign_is_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("a+b"), "a%2Bb");
}

TEST(generic_delimiters_are_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape(":?#[]@"), "%3A%3F%23%5B%5D%40");
}

TEST(hex_output_is_uppercase) {
  EXPECT_EQ(sourcemeta::core::URI::escape("\xff"), "%FF");
}

TEST(utf8_multibyte) {
  EXPECT_EQ(sourcemeta::core::URI::escape("\xc3\xa9"), "%C3%A9");
}

TEST(embedded_nul_byte) {
  EXPECT_EQ(sourcemeta::core::URI::escape(std::string_view{"a\x00"
                                                           "b",
                                                           3}),
            "a%00b");
}

TEST(empty) { EXPECT_EQ(sourcemeta::core::URI::escape(""), ""); }

TEST(append_preserves_existing_output) {
  std::string output{"prefix="};
  sourcemeta::core::URI::escape("a b/c", output);
  EXPECT_EQ(output, "prefix=a%20b%2Fc");
}

TEST(append_matches_returning_form) {
  std::string output;
  sourcemeta::core::URI::escape(":?#[]@", output);
  EXPECT_EQ(output, sourcemeta::core::URI::escape(":?#[]@"));
}

TEST(append_empty_leaves_output_untouched) {
  std::string output{"unchanged"};
  sourcemeta::core::URI::escape("", output);
  EXPECT_EQ(output, "unchanged");
}

TEST(normalized_raw_characters_are_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("a=b&c", true), "a%3Db%26c");
}

TEST(normalized_existing_escape_is_preserved) {
  EXPECT_EQ(sourcemeta::core::URI::escape("a%20b", true), "a%20b");
}

TEST(normalized_existing_encoded_delimiter_is_preserved) {
  EXPECT_EQ(sourcemeta::core::URI::escape("path%2Fto", true), "path%2Fto");
}

TEST(normalized_needlessly_encoded_unreserved_is_decoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("%41%42", true), "AB");
}

TEST(normalized_lowercase_escape_is_uppercased) {
  EXPECT_EQ(sourcemeta::core::URI::escape("a%2fb", true), "a%2Fb");
}

TEST(normalized_mixed_raw_and_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("a b%2Fc", true), "a%20b%2Fc");
}

TEST(normalized_lone_percent_is_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("100%", true), "100%25");
}

TEST(normalized_percent_without_hex_is_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("%zz", true), "%25zz");
}

TEST(normalized_percent_with_single_trailing_hex_is_encoded) {
  EXPECT_EQ(sourcemeta::core::URI::escape("%a", true), "%25a");
}

TEST(normalized_utf8_multibyte_encoded_is_preserved) {
  EXPECT_EQ(sourcemeta::core::URI::escape("%C3%A9", true), "%C3%A9");
}

TEST(normalized_is_idempotent_under_repeated_application) {
  const auto once{sourcemeta::core::URI::escape("a b%2Fc&d", true)};
  EXPECT_EQ(sourcemeta::core::URI::escape(once, true), once);
}

TEST(normalized_append_preserves_existing_output) {
  std::string output{"prefix="};
  sourcemeta::core::URI::escape("a b%2Fc", output, true);
  EXPECT_EQ(output, "prefix=a%20b%2Fc");
}
