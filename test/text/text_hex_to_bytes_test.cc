#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(empty_input) {
  const auto result{sourcemeta::core::hex_to_bytes("")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(single_byte_lowercase) {
  const auto result{sourcemeta::core::hex_to_bytes("ff")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xFF");
}

TEST(single_byte_uppercase) {
  const auto result{sourcemeta::core::hex_to_bytes("FF")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xFF");
}

TEST(mixed_case) {
  const auto result{sourcemeta::core::hex_to_bytes("DeadBeef")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xDE\xAD\xBE\xEF");
}

TEST(ascii_word) {
  const auto result{sourcemeta::core::hex_to_bytes("666f6f626172")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foobar");
}

TEST(nul_byte) {
  const auto result{sourcemeta::core::hex_to_bytes("00")};
  EXPECT_TRUE(result.has_value());
  const std::string expected("\x00", 1);
  EXPECT_EQ(result.value(), expected);
}

TEST(leading_zeros) {
  const auto result{sourcemeta::core::hex_to_bytes("0001")};
  EXPECT_TRUE(result.has_value());
  const std::string expected("\x00\x01", 2);
  EXPECT_EQ(result.value(), expected);
}

TEST(all_digit_pairs) {
  const auto result{sourcemeta::core::hex_to_bytes("0123456789abcdef")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\x01\x23\x45\x67\x89\xAB\xCD\xEF");
}

TEST(rejects_odd_length) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("abc").has_value());
}

TEST(rejects_single_character) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("a").has_value());
}

TEST(odd_length_allowed_single_character) {
  const auto result{sourcemeta::core::hex_to_bytes("a", true)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\x0A");
}

TEST(odd_length_allowed_assumes_leading_zero) {
  const auto result{sourcemeta::core::hex_to_bytes("abc", true)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\x0A\xBC");
}

TEST(even_length_allowed_is_unchanged) {
  const auto result{sourcemeta::core::hex_to_bytes("abcd", true)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xAB\xCD");
}

TEST(odd_length_allowed_rejects_non_hexadecimal) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("xbc", true).has_value());
}

TEST(rejects_non_hexadecimal_letter) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("zz").has_value());
}

TEST(rejects_interior_space) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("ab c").has_value());
}

TEST(rejects_prefix_notation) {
  EXPECT_FALSE(sourcemeta::core::hex_to_bytes("0xff").has_value());
}
