#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string> // std::string

TEST(space) {
  EXPECT_EQ(sourcemeta::core::URI::unescape("foo%20bar"), "foo bar");
}

TEST(slash) {
  EXPECT_EQ(sourcemeta::core::URI::unescape("path%2Fto%2Fkey"), "path/to/key");
}

TEST(percent) { EXPECT_EQ(sourcemeta::core::URI::unescape("100%25"), "100%"); }

TEST(uppercase_hex) {
  EXPECT_EQ(sourcemeta::core::URI::unescape("%C3%A9"), "\xc3\xa9");
}

TEST(lowercase_hex) {
  EXPECT_EQ(sourcemeta::core::URI::unescape("%c3%a9"), "\xc3\xa9");
}

TEST(unreserved_characters_pass_through) {
  EXPECT_EQ(sourcemeta::core::URI::unescape("ABCabc0123-._~"),
            "ABCabc0123-._~");
}

TEST(trailing_percent_is_literal) {
  EXPECT_EQ(sourcemeta::core::URI::unescape("100%"), "100%");
}

TEST(incomplete_sequence_is_literal) {
  EXPECT_EQ(sourcemeta::core::URI::unescape("a%2"), "a%2");
}

TEST(invalid_hex_is_literal) {
  EXPECT_EQ(sourcemeta::core::URI::unescape("a%2Gb"), "a%2Gb");
}

TEST(empty) { EXPECT_EQ(sourcemeta::core::URI::unescape(""), ""); }

TEST(round_trip_with_escape) {
  const std::string original{"a b/c%d?e=f&g:h@i"};
  EXPECT_EQ(
      sourcemeta::core::URI::unescape(sourcemeta::core::URI::escape(original)),
      original);
}
