#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string_view> // std::string_view

TEST(empty_input) { EXPECT_TRUE(sourcemeta::core::is_valid_utf8("")); }

TEST(ascii) { EXPECT_TRUE(sourcemeta::core::is_valid_utf8("hello world")); }

TEST(ascii_with_a_null_byte) {
  EXPECT_TRUE(sourcemeta::core::is_valid_utf8(std::string_view{"a\0b", 3}));
}

TEST(ascii_spanning_several_words) {
  EXPECT_TRUE(
      sourcemeta::core::is_valid_utf8("abcdefghijklmnopqrstuvwxyz0123456789"));
}

TEST(two_three_and_four_byte_sequences) {
  EXPECT_TRUE(sourcemeta::core::is_valid_utf8(
      "caf\xC3\xA9 \xE2\x82\xAC \xF0\x9F\x8C\xA8"));
}

TEST(sequence_after_several_ascii_words) {
  EXPECT_TRUE(sourcemeta::core::is_valid_utf8("abcdefghijklmnop\xE2\x82\xAC"));
}

TEST(sequence_across_a_word_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_valid_utf8("abcdefg\xE2\x82\xACqrstuvwx"));
}

TEST(an_invalid_byte) {
  EXPECT_FALSE(sourcemeta::core::is_valid_utf8("a\xFF"));
}

TEST(an_invalid_byte_after_several_ascii_words) {
  EXPECT_FALSE(sourcemeta::core::is_valid_utf8("abcdefghijklmnop\xFF"));
}

TEST(a_continuation_byte_alone) {
  EXPECT_FALSE(sourcemeta::core::is_valid_utf8("\x80"));
}

TEST(an_overlong_encoding) {
  EXPECT_FALSE(sourcemeta::core::is_valid_utf8("\xC0\x80"));
}

TEST(a_surrogate) {
  EXPECT_FALSE(sourcemeta::core::is_valid_utf8("\xED\xA0\x80"));
}

TEST(beyond_the_codespace) {
  EXPECT_FALSE(sourcemeta::core::is_valid_utf8("\xF4\x90\x80\x80"));
}

TEST(a_sequence_cut_short_by_the_end_of_the_input) {
  EXPECT_FALSE(sourcemeta::core::is_valid_utf8("abc\xE2\x82"));
}

TEST(an_incomplete_sequence_after_a_valid_one) {
  EXPECT_FALSE(sourcemeta::core::is_valid_utf8("\xC3\xA9\xC3"));
}
