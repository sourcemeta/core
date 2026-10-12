#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <string>      // std::string
#include <string_view> // std::string_view

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

// The overload that writes into caller storage is the primitive the allocating
// one is built on, so it is exercised on its own terms too
TEST(into_caller_storage) {
  const std::array<std::uint8_t, 3> input{{0x66, 0x6f, 0x6f}};
  std::array<char, 6> output{};
  sourcemeta::core::bytes_to_hex(input, output);
  EXPECT_EQ((std::string_view{output.data(), output.size()}), "666f6f");
}

TEST(into_caller_storage_empty) {
  const std::array<std::uint8_t, 0> input{};
  std::array<char, 0> output{};
  sourcemeta::core::bytes_to_hex(input, output);
  EXPECT_TRUE(output.empty());
}

TEST(into_caller_storage_every_nibble) {
  const std::array<std::uint8_t, 8> input{
      {0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd}};
  std::array<char, 16> output{};
  sourcemeta::core::bytes_to_hex(input, output);
  EXPECT_EQ((std::string_view{output.data(), output.size()}),
            "000123456789abcd");
}

TEST(into_caller_storage_writes_lowercase) {
  const std::array<std::uint8_t, 2> input{{0xEF, 0xFF}};
  std::array<char, 4> output{};
  sourcemeta::core::bytes_to_hex(input, output);
  EXPECT_EQ((std::string_view{output.data(), output.size()}), "efff");
}
