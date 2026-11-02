#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string> // std::string

TEST(empty_input) { EXPECT_EQ(sourcemeta::core::to_valid_utf8(""), ""); }

TEST(well_formed_input_is_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("hello"), "hello");
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("caf\xC3\xA9"), "caf\xC3\xA9");
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("\xE2\x82\xAC"), "\xE2\x82\xAC");
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("\xF0\x9F\x8C\xA8"),
            "\xF0\x9F\x8C\xA8");
}

TEST(an_out_of_range_byte_is_replaced) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("a\xFF"
                                            "b"),
            "a\xEF\xBF\xBD"
            "b");
}

TEST(a_stray_continuation_byte_is_replaced) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("a\x80"
                                            "b"),
            "a\xEF\xBF\xBD"
            "b");
}

// One replacement stands in for the whole maximal subpart rather than one per
// byte, so a two byte prefix of a three byte sequence yields a single one
TEST(a_sequence_cut_short_is_one_replacement) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("a\xE2\x82"
                                            "b"),
            "a\xEF\xBF\xBD"
            "b");
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("a\xF0\x9F\x8C"
                                            "b"),
            "a\xEF\xBF\xBD"
            "b");
}

TEST(a_sequence_cut_short_by_the_end_of_the_input) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("a\xE2\x82"), "a\xEF\xBF\xBD");
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("\xC3"), "\xEF\xBF\xBD");
}

// A surrogate is three separate ill-formed bytes, as the lead calls for a tail
// the second byte is not
TEST(a_surrogate_becomes_three_replacements) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("a\xED\xA0\x80"),
            "a\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD");
}

TEST(an_overlong_encoding_is_replaced) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("\xC0\x80"),
            "\xEF\xBF\xBD\xEF\xBF\xBD");
}

TEST(consecutive_stray_bytes_are_replaced_individually) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("\xFF\xFF"),
            "\xEF\xBF\xBD\xEF\xBF\xBD");
}

TEST(a_replacement_character_in_the_input_survives) {
  EXPECT_EQ(sourcemeta::core::to_valid_utf8("\xEF\xBF\xBD"), "\xEF\xBF\xBD");
}

TEST(the_result_is_always_well_formed) {
  const std::string result{sourcemeta::core::to_valid_utf8("\xFF\xC3\xE2\x82")};
  EXPECT_TRUE(sourcemeta::core::utf8_to_utf32(result).has_value());
}
