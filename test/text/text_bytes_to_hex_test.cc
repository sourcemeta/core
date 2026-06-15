#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(Text_bytes_to_hex, empty_input) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(""), "");
}

TEST(Text_bytes_to_hex, single_high_byte) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex("\xFF"), "ff");
}

TEST(Text_bytes_to_hex, ascii_word) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex("foobar"), "666f6f626172");
}

TEST(Text_bytes_to_hex, nul_byte) {
  const std::string input("\x00", 1);
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(input), "00");
}

TEST(Text_bytes_to_hex, leading_zero_byte) {
  const std::string input("\x00\x01", 2);
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(input), "0001");
}

TEST(Text_bytes_to_hex, all_byte_values_sample) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex("\x01\x23\x45\x67\x89\xAB\xCD\xEF"),
            "0123456789abcdef");
}

TEST(Text_bytes_to_hex, lowercase_output) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex("\xDE\xAD\xBE\xEF"), "deadbeef");
}

TEST(Text_bytes_to_hex, roundtrip_with_hex_to_bytes) {
  const std::string input("\x00\x10\x20\xFF\x7F", 5);
  const auto encoded{sourcemeta::core::bytes_to_hex(input)};
  const auto decoded{sourcemeta::core::hex_to_bytes(encoded)};
  EXPECT_TRUE(decoded.has_value());
  EXPECT_EQ(decoded.value(), input);
}
