#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <sstream> // std::istringstream
#include <string>  // std::u32string

TEST(ascii) {
  std::istringstream input{"Hello"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x48, 0x65, 0x6C, 0x6C, 0x6F};
  EXPECT_EQ(result.value(), expected);
}

TEST(empty) {
  std::istringstream input{""};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().empty());
}

TEST(two_byte) {
  std::istringstream input{"\xC3\xA9"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0xE9};
  EXPECT_EQ(result.value(), expected);
}

TEST(three_byte_cjk) {
  std::istringstream input{"\xE4\xB8\x96"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x4E16};
  EXPECT_EQ(result.value(), expected);
}

TEST(four_byte_emoji) {
  std::istringstream input{"\xF0\x9F\x98\x80"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x1F600};
  EXPECT_EQ(result.value(), expected);
}

TEST(mixed) {
  std::istringstream input{"H\xC3\xA9\xE4\xB8\x96\xF0\x9F\x98\x80"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x48, 0xE9, 0x4E16, 0x1F600};
  EXPECT_EQ(result.value(), expected);
}

TEST(invalid_continuation) {
  std::istringstream input{"\xC3\x28"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(truncated_sequence) {
  std::istringstream input{"\xE4\xB8"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(overlong_encoding) {
  std::istringstream input{"\xC0\x80"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(surrogate_codepoint) {
  std::istringstream input{"\xED\xA0\x80"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(invalid_start_byte) {
  std::istringstream input{"\xFF"};
  const auto result{sourcemeta::core::utf8_to_utf32(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(string_view_ascii) {
  const auto result{sourcemeta::core::utf8_to_utf32("Hello")};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x48, 0x65, 0x6C, 0x6C, 0x6F};
  EXPECT_EQ(result.value(), expected);
}

TEST(string_view_empty) {
  const auto result{sourcemeta::core::utf8_to_utf32("")};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().empty());
}

TEST(string_view_mixed) {
  const auto result{
      sourcemeta::core::utf8_to_utf32("H\xC3\xA9\xE4\xB8\x96\xF0\x9F\x98\x80")};
  EXPECT_TRUE(result.has_value());
  const std::u32string expected{0x48, 0xE9, 0x4E16, 0x1F600};
  EXPECT_EQ(result.value(), expected);
}

TEST(string_view_invalid) {
  const auto result{sourcemeta::core::utf8_to_utf32("\xFF")};
  EXPECT_FALSE(result.has_value());
}
