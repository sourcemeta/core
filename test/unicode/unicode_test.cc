#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

#include <sstream> // std::istringstream, std::ostringstream
#include <string>  // std::string, std::u32string

TEST(Unicode, codepoint_to_utf8_ascii_letter) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x41), "A");
}

TEST(Unicode, codepoint_to_utf8_ascii_null) {
  const std::string expected(1, '\0');
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x00), expected);
}

TEST(Unicode, codepoint_to_utf8_ascii_max) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x7F), "\x7F");
}

TEST(Unicode, codepoint_to_utf8_two_byte_min) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x80), "\xC2\x80");
}

TEST(Unicode, codepoint_to_utf8_two_byte_latin_e_acute) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0xE9), "\xC3\xA9");
}

TEST(Unicode, codepoint_to_utf8_two_byte_max) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x7FF), "\xDF\xBF");
}

TEST(Unicode, codepoint_to_utf8_three_byte_min) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x800), "\xE0\xA0\x80");
}

TEST(Unicode, codepoint_to_utf8_three_byte_cjk) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x4E16), "\xE4\xB8\x96");
}

TEST(Unicode, codepoint_to_utf8_three_byte_max) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0xFFFF), "\xEF\xBF\xBF");
}

TEST(Unicode, codepoint_to_utf8_four_byte_min) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x10000), "\xF0\x90\x80\x80");
}

TEST(Unicode, codepoint_to_utf8_four_byte_emoji) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x1F600), "\xF0\x9F\x98\x80");
}

TEST(Unicode, codepoint_to_utf8_four_byte_max) {
  EXPECT_EQ(sourcemeta::core::codepoint_to_utf8(0x10FFFF), "\xF4\x8F\xBF\xBF");
}

TEST(Unicode, codepoint_to_utf8_stream_ascii_letter) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0x41, output);
  EXPECT_EQ(output.str(), "A");
}

TEST(Unicode, codepoint_to_utf8_stream_two_byte_latin_e_acute) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0xE9, output);
  EXPECT_EQ(output.str(), "\xC3\xA9");
}

TEST(Unicode, codepoint_to_utf8_stream_three_byte_cjk) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0x4E16, output);
  EXPECT_EQ(output.str(), "\xE4\xB8\x96");
}

TEST(Unicode, codepoint_to_utf8_stream_four_byte_emoji) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0x1F600, output);
  EXPECT_EQ(output.str(), "\xF0\x9F\x98\x80");
}

TEST(Unicode, codepoint_to_utf8_stream_multiple_codepoints) {
  std::ostringstream output;
  sourcemeta::core::codepoint_to_utf8(0x48, output);
  sourcemeta::core::codepoint_to_utf8(0xE9, output);
  sourcemeta::core::codepoint_to_utf8(0x1F600, output);
  EXPECT_EQ(output.str(), "H\xC3\xA9\xF0\x9F\x98\x80");
}

TEST(Unicode, utf8_to_utf32_ascii) {
  std::istringstream input{"Hello"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x48, 0x65, 0x6C, 0x6C, 0x6F};
  EXPECT_EQ(result.value(), expected);
}

TEST(Unicode, utf8_to_utf32_empty) {
  std::istringstream input{""};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().empty());
}

TEST(Unicode, utf8_to_utf32_two_byte) {
  std::istringstream input{"\xC3\xA9"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0xE9};
  EXPECT_EQ(result.value(), expected);
}

TEST(Unicode, utf8_to_utf32_three_byte_cjk) {
  std::istringstream input{"\xE4\xB8\x96"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x4E16};
  EXPECT_EQ(result.value(), expected);
}

TEST(Unicode, utf8_to_utf32_four_byte_emoji) {
  std::istringstream input{"\xF0\x9F\x98\x80"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x1F600};
  EXPECT_EQ(result.value(), expected);
}

TEST(Unicode, utf8_to_utf32_mixed) {
  std::istringstream input{"H\xC3\xA9\xE4\xB8\x96\xF0\x9F\x98\x80"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x48, 0xE9, 0x4E16, 0x1F600};
  EXPECT_EQ(result.value(), expected);
}

TEST(Unicode, utf8_to_utf32_invalid_continuation) {
  std::istringstream input{"\xC3\x00"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Unicode, utf8_to_utf32_truncated_sequence) {
  std::istringstream input{"\xE4\xB8"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Unicode, utf8_to_utf32_overlong_encoding) {
  std::istringstream input{"\xC0\x80"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Unicode, utf8_to_utf32_surrogate_codepoint) {
  std::istringstream input{"\xED\xA0\x80"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(Unicode, utf8_to_utf32_invalid_start_byte) {
  std::istringstream input{"\xFF"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}
