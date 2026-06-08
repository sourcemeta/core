#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

#include <cstdint> // std::uint32_t
#include <string>  // std::string

TEST(Crypto_CRC32, empty_string) {
  EXPECT_EQ(sourcemeta::core::crc32(""), 0x00000000u);
}

// Try $ printf '%s' "a" | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(Crypto_CRC32, single_byte_a) {
  EXPECT_EQ(sourcemeta::core::crc32("a"), 0xE8B7BE43u);
}

// Try $ printf '%s' "abc" | gzip -c | tail -c 8 | head -c 4 | xxd
TEST(Crypto_CRC32, abc_string) {
  EXPECT_EQ(sourcemeta::core::crc32("abc"), 0x352441C2u);
}

// ITU-T V.42 canonical CRC-32 check value
TEST(Crypto_CRC32, check_string_123456789) {
  EXPECT_EQ(sourcemeta::core::crc32("123456789"), 0xCBF43926u);
}

TEST(Crypto_CRC32, alphabet_lowercase) {
  EXPECT_EQ(sourcemeta::core::crc32("abcdefghijklmnopqrstuvwxyz"), 0x4C2750BDu);
}

TEST(Crypto_CRC32, quick_brown_fox) {
  EXPECT_EQ(
      sourcemeta::core::crc32("The quick brown fox jumps over the lazy dog"),
      0x414FA339u);
}

TEST(Crypto_CRC32, embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x0854897Fu);
}

TEST(Crypto_CRC32, all_zero_bytes_eight) {
  const std::string input(8, '\0');
  EXPECT_EQ(sourcemeta::core::crc32(input), 0x6522DF69u);
}

TEST(Crypto_CRC32, update_zero_initial_matches_single_shot) {
  EXPECT_EQ(sourcemeta::core::crc32_update(0u, "abc"),
            sourcemeta::core::crc32("abc"));
}

TEST(Crypto_CRC32, update_composes_across_chunks) {
  const auto single{sourcemeta::core::crc32("hello world")};
  auto running{sourcemeta::core::crc32_update(0u, "hello")};
  running = sourcemeta::core::crc32_update(running, " world");
  EXPECT_EQ(running, single);
}

TEST(Crypto_CRC32, update_composes_across_three_chunks) {
  const auto single{sourcemeta::core::crc32("The quick brown fox")};
  auto running{sourcemeta::core::crc32_update(0u, "The ")};
  running = sourcemeta::core::crc32_update(running, "quick ");
  running = sourcemeta::core::crc32_update(running, "brown fox");
  EXPECT_EQ(running, single);
}

TEST(Crypto_CRC32, update_with_empty_input_returns_previous) {
  EXPECT_EQ(sourcemeta::core::crc32_update(0xCAFEBABEu, ""), 0xCAFEBABEu);
}

TEST(Crypto_CRC32, update_byte_by_byte_matches_single_shot) {
  const std::string input{"hello"};
  const auto single{sourcemeta::core::crc32(input)};
  std::uint32_t running{0u};
  running = sourcemeta::core::crc32_update(running, "h");
  running = sourcemeta::core::crc32_update(running, "e");
  running = sourcemeta::core::crc32_update(running, "l");
  running = sourcemeta::core::crc32_update(running, "l");
  running = sourcemeta::core::crc32_update(running, "o");
  EXPECT_EQ(running, single);
}
