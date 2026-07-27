#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string_view> // std::string_view

TEST(empty) { EXPECT_EQ(sourcemeta::core::utf8_codepoint_count(""), 0); }

TEST(single_ascii) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count("a"), 1);
}

TEST(ascii_word) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count("hello"), 5);
}

TEST(single_two_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count("\xc3\xa9"), 1);
}

TEST(single_three_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count("\xe2\x82\xac"), 1);
}

TEST(single_four_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count("\xf0\x9f\x98\x80"), 1);
}

TEST(ascii_and_two_byte) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count("caf\xc3\xa9"), 4);
}

TEST(three_cjk_code_points) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count(
                "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e"),
            3);
}

TEST(mixed_sequence_lengths) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count(
                "a\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80"),
            4);
}

TEST(embedded_null_byte_counts) {
  const std::string_view input{"a\0b", 3};
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_count(input), 3);
}
