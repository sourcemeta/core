#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(Text_hex_to_bytes, empty_input) {
  const auto result{sourcemeta::core::hex_to_bytes("")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(Text_hex_to_bytes, single_byte_lowercase) {
  const auto result{sourcemeta::core::hex_to_bytes("ff")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xFF");
}

TEST(Text_hex_to_bytes, single_byte_uppercase) {
  const auto result{sourcemeta::core::hex_to_bytes("FF")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xFF");
}

TEST(Text_hex_to_bytes, mixed_case) {
  const auto result{sourcemeta::core::hex_to_bytes("DeadBeef")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xDE\xAD\xBE\xEF");
}

TEST(Text_hex_to_bytes, ascii_word) {
  const auto result{sourcemeta::core::hex_to_bytes("666f6f626172")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foobar");
}

TEST(Text_hex_to_bytes, nul_byte) {
  const auto result{sourcemeta::core::hex_to_bytes("00")};
  EXPECT_TRUE(result.has_value());
  const std::string expected("\x00", 1);
  EXPECT_EQ(result.value(), expected);
}

TEST(Text_hex_to_bytes, leading_zeros) {
  const auto result{sourcemeta::core::hex_to_bytes("0001")};
  EXPECT_TRUE(result.has_value());
  const std::string expected("\x00\x01", 2);
  EXPECT_EQ(result.value(), expected);
}

TEST(Text_hex_to_bytes, all_digit_pairs) {
  const auto result{sourcemeta::core::hex_to_bytes("0123456789abcdef")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\x01\x23\x45\x67\x89\xAB\xCD\xEF");
}

TEST(Text_hex_to_bytes, rejects_odd_length) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("abc").has_value());
}

TEST(Text_hex_to_bytes, rejects_single_character) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("a").has_value());
}

TEST(Text_hex_to_bytes, odd_length_allowed_single_character) {
  const auto result{sourcemeta::core::hex_to_bytes("a", true)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\x0A");
}

TEST(Text_hex_to_bytes, odd_length_allowed_assumes_leading_zero) {
  const auto result{sourcemeta::core::hex_to_bytes("abc", true)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\x0A\xBC");
}

TEST(Text_hex_to_bytes, even_length_allowed_is_unchanged) {
  const auto result{sourcemeta::core::hex_to_bytes("abcd", true)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xAB\xCD");
}

TEST(Text_hex_to_bytes, odd_length_allowed_rejects_non_hexadecimal) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("xbc", true).has_value());
}

TEST(Text_hex_to_bytes, rejects_non_hexadecimal_letter) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("zz").has_value());
}

TEST(Text_hex_to_bytes, rejects_interior_space) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("ab c").has_value());
}

TEST(Text_hex_to_bytes, rejects_prefix_notation) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("0xff").has_value());
}
