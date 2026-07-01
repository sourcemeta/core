#include <sourcemeta/core/test.h>

#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

TEST(no_whitespace_returns_input_unchanged) {
  EXPECT_EQ(sourcemeta::core::trim("hello"), "hello");
}

TEST(leading_spaces) { EXPECT_EQ(sourcemeta::core::trim("   hello"), "hello"); }

TEST(trailing_spaces) {
  EXPECT_EQ(sourcemeta::core::trim("hello   "), "hello");
}

TEST(both_ends_spaces) {
  EXPECT_EQ(sourcemeta::core::trim("  hello  "), "hello");
}

TEST(preserves_internal_whitespace) {
  EXPECT_EQ(sourcemeta::core::trim("  hello world  "), "hello world");
}

TEST(tabs) { EXPECT_EQ(sourcemeta::core::trim("\thello\t"), "hello"); }

TEST(line_feed) { EXPECT_EQ(sourcemeta::core::trim("\nhello\n"), "hello"); }

TEST(carriage_return) {
  EXPECT_EQ(sourcemeta::core::trim("\rhello\r"), "hello");
}

TEST(vertical_tab) { EXPECT_EQ(sourcemeta::core::trim("\vhello\v"), "hello"); }

TEST(form_feed) { EXPECT_EQ(sourcemeta::core::trim("\fhello\f"), "hello"); }

TEST(mixed_whitespace_characters) {
  EXPECT_EQ(sourcemeta::core::trim("\t\n\r\v\f hello \f\v\r\n\t"), "hello");
}

TEST(all_whitespace_returns_empty) {
  EXPECT_EQ(sourcemeta::core::trim("    "), "");
}

TEST(all_whitespace_mixed_returns_empty) {
  EXPECT_EQ(sourcemeta::core::trim("\t\n \r"), "");
}

TEST(empty_input_returns_empty) { EXPECT_EQ(sourcemeta::core::trim(""), ""); }

TEST(single_non_whitespace_character) {
  EXPECT_EQ(sourcemeta::core::trim("a"), "a");
}

TEST(single_whitespace_character) {
  EXPECT_EQ(sourcemeta::core::trim(" "), "");
}

TEST(non_ascii_bytes_pass_through) {
  const std::string_view input{"\xC3\xA9 caf\xC3\xA9 "};
  EXPECT_EQ(sourcemeta::core::trim(input), "\xC3\xA9 caf\xC3\xA9");
}

TEST(null_byte_is_not_whitespace) {
  const std::string_view input{"\0hello\0", 7};
  EXPECT_EQ(sourcemeta::core::trim(input), input);
}
