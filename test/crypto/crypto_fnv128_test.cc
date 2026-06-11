#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

#include <array>
#include <cstdint>
#include <sstream>
#include <string>

// The digest of the empty string is the FNV offset basis
// (draft-eastlake-fnv Section 5)
TEST(Crypto_FNV128, digest_empty_string) {
  const std::array<std::uint8_t, 16> expected{
      {0x6c, 0x62, 0x27, 0x2e, 0x07, 0xbb, 0x01, 0x42, 0x62, 0xb8, 0x21, 0x75,
       0x62, 0x95, 0xc5, 0x8d}};
  EXPECT_EQ(sourcemeta::core::fnv128_digest(""), expected);
}

TEST(Crypto_FNV128, digest_a_string) {
  const std::array<std::uint8_t, 16> expected{
      {0xd2, 0x28, 0xcb, 0x69, 0x10, 0x1a, 0x8c, 0xaf, 0x78, 0x91, 0x2b, 0x70,
       0x4e, 0x4a, 0x14, 0x1e}};
  EXPECT_EQ(sourcemeta::core::fnv128_digest("a"), expected);
}

TEST(Crypto_FNV128, digest_foobar_string) {
  const std::array<std::uint8_t, 16> expected{
      {0x78, 0x96, 0xbf, 0xea, 0x9c, 0x3c, 0x64, 0xbf, 0x6d, 0xc5, 0x83, 0x53,
       0xd2, 0xc2, 0x93, 0xaa}};
  EXPECT_EQ(sourcemeta::core::fnv128_digest("foobar"), expected);
}

TEST(Crypto_FNV128, digest_embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  const std::array<std::uint8_t, 16> expected{
      {0xa6, 0x8b, 0xb3, 0x93, 0x4b, 0x8b, 0x58, 0x22, 0x83, 0x6d, 0xbc, 0x78,
       0xc7, 0x42, 0x3b, 0xae}};
  EXPECT_EQ(sourcemeta::core::fnv128_digest(input), expected);
}

TEST(Crypto_FNV128, empty_string) {
  std::ostringstream result;
  sourcemeta::core::fnv128("", result);
  EXPECT_EQ(result.str(), "6c62272e07bb014262b821756295c58d");
}

TEST(Crypto_FNV128, a_string) {
  std::ostringstream result;
  sourcemeta::core::fnv128("a", result);
  EXPECT_EQ(result.str(), "d228cb69101a8caf78912b704e4a141e");
}

// FNV-1 hashes that differ only in the last input byte must differ
// only in the lowest bits, as the final operation is the XOR
TEST(Crypto_FNV128, b_string) {
  std::ostringstream result;
  sourcemeta::core::fnv128("b", result);
  EXPECT_EQ(result.str(), "d228cb69101a8caf78912b704e4a141d");
}

TEST(Crypto_FNV128, abc_string) {
  std::ostringstream result;
  sourcemeta::core::fnv128("abc", result);
  EXPECT_EQ(result.str(), "a68bb2a4348b5822836dbc78c6aee73b");
}

TEST(Crypto_FNV128, foobar_string) {
  std::ostringstream result;
  sourcemeta::core::fnv128("foobar", result);
  EXPECT_EQ(result.str(), "7896bfea9c3c64bf6dc58353d2c293aa");
}

TEST(Crypto_FNV128, example_string) {
  std::ostringstream result;
  sourcemeta::core::fnv128("foo bar", result);
  EXPECT_EQ(result.str(), "b5520317e94ff78c12089824c9add07c");
}

// Classic test string from the FNV reference implementation
TEST(Crypto_FNV128, chongo_was_here) {
  std::ostringstream result;
  sourcemeta::core::fnv128("chongo was here!\n", result);
  EXPECT_EQ(result.str(), "40ab469af9cf0fe57236785215beee65");
}

TEST(Crypto_FNV128, quick_brown_fox) {
  std::ostringstream result;
  sourcemeta::core::fnv128("The quick brown fox jumps over the lazy dog",
                           result);
  EXPECT_EQ(result.str(), "185adb693e7c97844ecfa9497cb529b6");
}

TEST(Crypto_FNV128, sixteen_bytes) {
  std::ostringstream result;
  sourcemeta::core::fnv128("0123456789abcdef", result);
  EXPECT_EQ(result.str(), "fdecd6dfdf1f1b9e66786beed1487911");
}

TEST(Crypto_FNV128, embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  std::ostringstream result;
  sourcemeta::core::fnv128(input, result);
  EXPECT_EQ(result.str(), "a68bb3934b8b5822836dbc78c7423bae");
}

TEST(Crypto_FNV128, one_million_a_chars) {
  const std::string input(1000000, 'a');
  std::ostringstream result;
  sourcemeta::core::fnv128(input, result);
  EXPECT_EQ(result.str(), "bdf8b5a26e7a7e826e6b7b4e16359fcd");
}

TEST(Crypto_FNV128, to_string_empty) {
  EXPECT_EQ(sourcemeta::core::fnv128(""), "6c62272e07bb014262b821756295c58d");
}

TEST(Crypto_FNV128, to_string_a) {
  EXPECT_EQ(sourcemeta::core::fnv128("a"), "d228cb69101a8caf78912b704e4a141e");
}

TEST(Crypto_FNV128, to_string_foobar) {
  EXPECT_EQ(sourcemeta::core::fnv128("foobar"),
            "7896bfea9c3c64bf6dc58353d2c293aa");
}

TEST(Crypto_FNV128, to_string_quick_brown_fox) {
  EXPECT_EQ(
      sourcemeta::core::fnv128("The quick brown fox jumps over the lazy dog"),
      "185adb693e7c97844ecfa9497cb529b6");
}

TEST(Crypto_FNV128, to_string_one_million_a_chars) {
  const std::string input(1000000, 'a');
  EXPECT_EQ(sourcemeta::core::fnv128(input),
            "bdf8b5a26e7a7e826e6b7b4e16359fcd");
}
