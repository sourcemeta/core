#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string_view> // std::string_view

TEST(empty_input) { EXPECT_EQ(sourcemeta::core::utf8_sequence_size(""), 0); }

TEST(ascii) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("A"), 1);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size(std::string_view{"\0", 1}), 1);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\x7F"), 1);
}

TEST(two_byte_sequence) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xC3\xA9"), 2);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xC2\x80"), 2);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xDF\xBF"), 2);
}

TEST(three_byte_sequence) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xE2\x82\xAC"), 3);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xE0\xA0\x80"), 3);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xEF\xBF\xBD"), 3);
}

TEST(four_byte_sequence) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xF0\x9F\x8C\xA8"), 4);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xF0\x90\x80\x80"), 4);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xF4\x8F\xBF\xBF"), 4);
}

TEST(a_continuation_byte_alone) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\x80"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xBF"), 0);
}

TEST(an_out_of_range_lead) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xF5\x80\x80\x80"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xFF"), 0);
}

TEST(an_overlong_two_byte_lead) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xC0\x80"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xC1\xBF"), 0);
}

// RFC 3629 Section 4 narrows the byte after %xE0 to %xA0-BF, so as to exclude
// the three-byte encodings of what fits in two
TEST(an_overlong_three_byte_sequence) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xE0\x80\x80"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xE0\x9F\xBF"), 0);
}

// The byte after %xF0 is narrowed to %x90-BF for the same reason
TEST(an_overlong_four_byte_sequence) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xF0\x80\x80\x80"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xF0\x8F\xBF\xBF"), 0);
}

// The byte after %xED is narrowed to %x80-9F, which excludes the surrogates
TEST(a_surrogate) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xED\xA0\x80"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xED\xBF\xBF"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xED\x9F\xBF"), 3);
}

// The byte after %xF4 is narrowed to %x80-8F, which is the end of the codespace
TEST(beyond_the_codespace) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xF4\x90\x80\x80"), 0);
}

TEST(a_sequence_cut_short_by_the_end_of_the_input) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xC3"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xE2\x82"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xF0\x9F\x8C"), 0);
}

TEST(a_tail_that_is_not_a_continuation_byte) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xC3\x41"), 0);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xE2\x82\x41"), 0);
}

TEST(only_the_leading_sequence_is_measured) {
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("A\xFF"), 1);
  EXPECT_EQ(sourcemeta::core::utf8_sequence_size("\xC3\xA9zzz"), 2);
}
