#include <sourcemeta/core/test.h>

#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(empty_input) { EXPECT_EQ(sourcemeta::core::bytes_to_hex(""), ""); }

TEST(single_high_byte) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex("\xFF"), "ff");
}

TEST(ascii_word) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex("foobar"), "666f6f626172");
}

TEST(nul_byte) {
  const std::string input("\x00", 1);
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(input), "00");
}

TEST(leading_zero_byte) {
  const std::string input("\x00\x01", 2);
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(input), "0001");
}

TEST(all_byte_values_sample) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex("\x01\x23\x45\x67\x89\xAB\xCD\xEF"),
            "0123456789abcdef");
}

TEST(lowercase_output) {
  EXPECT_EQ(sourcemeta::core::bytes_to_hex("\xDE\xAD\xBE\xEF"), "deadbeef");
}

TEST(roundtrip_with_hex_to_bytes) {
  const std::string input("\x00\x10\x20\xFF\x7F", 5);
  const auto encoded{sourcemeta::core::bytes_to_hex(input)};
  const auto decoded{sourcemeta::core::hex_to_bytes(encoded)};
  EXPECT_TRUE(decoded.has_value());
  EXPECT_EQ(decoded.value(), input);
}
