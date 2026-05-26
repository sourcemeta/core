#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

#include <sstream> // std::ostringstream
#include <string>  // std::string

TEST(Unicode_codepoint_to_utf8, ascii_letter) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x41), "A");
}

TEST(Unicode_codepoint_to_utf8, ascii_null) {
  const std::string expected(1, '\0');
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x00), expected);
}

TEST(Unicode_codepoint_to_utf8, ascii_max) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x7F), "\x7F");
}

TEST(Unicode_codepoint_to_utf8, two_byte_min) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x80), "\xC2\x80");
}

TEST(Unicode_codepoint_to_utf8, two_byte_latin_e_acute) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0xE9), "\xC3\xA9");
}

TEST(Unicode_codepoint_to_utf8, two_byte_max) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x7FF), "\xDF\xBF");
}

TEST(Unicode_codepoint_to_utf8, three_byte_min) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x800), "\xE0\xA0\x80");
}

TEST(Unicode_codepoint_to_utf8, three_byte_cjk) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x4E16), "\xE4\xB8\x96");
}

TEST(Unicode_codepoint_to_utf8, three_byte_max) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0xFFFF), "\xEF\xBF\xBF");
}

TEST(Unicode_codepoint_to_utf8, four_byte_min) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x10000), "\xF0\x90\x80\x80");
}

TEST(Unicode_codepoint_to_utf8, four_byte_emoji) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x1F600), "\xF0\x9F\x98\x80");
}

TEST(Unicode_codepoint_to_utf8, four_byte_max) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x10FFFF), "\xF4\x8F\xBF\xBF");
}

TEST(Unicode_codepoint_to_utf8, stream_ascii_letter) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0x41, output);
  EXPECT_EQ(output.str(), "A");
}

TEST(Unicode_codepoint_to_utf8, stream_two_byte_latin_e_acute) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0xE9, output);
  EXPECT_EQ(output.str(), "\xC3\xA9");
}

TEST(Unicode_codepoint_to_utf8, stream_three_byte_cjk) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0x4E16, output);
  EXPECT_EQ(output.str(), "\xE4\xB8\x96");
}

TEST(Unicode_codepoint_to_utf8, stream_four_byte_emoji) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0x1F600, output);
  EXPECT_EQ(output.str(), "\xF0\x9F\x98\x80");
}

TEST(Unicode_codepoint_to_utf8, stream_multiple_codepoints) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0x48, output);
  sourcemeta::core::codepoint_to_utf8(0xE9, output);
  sourcemeta::core::codepoint_to_utf8(0x1F600, output);
  EXPECT_EQ(output.str(), "H\xC3\xA9\xF0\x9F\x98\x80");
}

TEST(Unicode_codepoint_to_utf8, string_ascii_letter) {
  std::string output;
  sourcemeta::core::codepoint_to_utf8(0x41, output);
  EXPECT_EQ(output, "A");
}

TEST(Unicode_codepoint_to_utf8, string_two_byte_latin_e_acute) {
  std::string output;
  sourcemeta::core::codepoint_to_utf8(0xE9, output);
  EXPECT_EQ(output, "\xC3\xA9");
}

TEST(Unicode_codepoint_to_utf8, string_three_byte_cjk) {
  std::string output;
  sourcemeta::core::codepoint_to_utf8(0x4E16, output);
  EXPECT_EQ(output, "\xE4\xB8\x96");
}

TEST(Unicode_codepoint_to_utf8, string_four_byte_emoji) {
  std::string output;
  sourcemeta::core::codepoint_to_utf8(0x1F600, output);
  EXPECT_EQ(output, "\xF0\x9F\x98\x80");
}

TEST(Unicode_codepoint_to_utf8, string_multiple_codepoints) {
  std::string output;
  sourcemeta::core::codepoint_to_utf8(0x48, output);
  sourcemeta::core::codepoint_to_utf8(0xE9, output);
  sourcemeta::core::codepoint_to_utf8(0x1F600, output);
  EXPECT_EQ(output, "H\xC3\xA9\xF0\x9F\x98\x80");
}

TEST(Unicode_codepoint_to_utf8, string_four_byte_max) {
  std::string output;
  sourcemeta::core::codepoint_to_utf8(0x10FFFF, output);
  EXPECT_EQ(output, "\xF4\x8F\xBF\xBF");
}
