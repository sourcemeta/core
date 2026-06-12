#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t
#include <sstream> // std::ostringstream
#include <string>  // std::string

// Try $ echo -n "" | shasum -a 384
TEST(Crypto_SHA384, empty_string) {
  std::ostringstream result;
  sourcemeta::core::sha384("", result);
  EXPECT_EQ(result.str(), "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c"
                          "0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");
}

// Try $ echo -n "abc" | shasum -a 384
TEST(Crypto_SHA384, abc_string) {
  std::ostringstream result;
  sourcemeta::core::sha384("abc", result);
  EXPECT_EQ(result.str(), "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a"
                          "8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7");
}

// Try $ echo -n "foo bar" | shasum -a 384
TEST(Crypto_SHA384, example_string) {
  std::ostringstream result;
  sourcemeta::core::sha384("foo bar", result);
  EXPECT_EQ(result.str(), "6839312f3db343477070d3c0b2becd417b357154d48794d01d"
                          "78cfb4617ed5ab819a77b6832f6542dd18bb738131ef7e");
}

// Try $ echo -n "The quick brown fox jumps over the lazy dog" | shasum -a 384
TEST(Crypto_SHA384, quick_brown_fox) {
  std::ostringstream result;
  sourcemeta::core::sha384("The quick brown fox jumps over the lazy dog",
                           result);
  EXPECT_EQ(result.str(), "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c4940"
                          "11e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1");
}

// FIPS 180-4 two-block message sample
TEST(Crypto_SHA384, fips_two_block_message) {
  std::ostringstream result;
  sourcemeta::core::sha384("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmgh"
                           "ijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnop"
                           "qrstnopqrstu",
                           result);
  EXPECT_EQ(result.str(), "09330c33f71147e83d192fc782cd1b4753111b173b3b05d22f"
                          "a08086e3b0f712fcc7c71a557e2db966c3e9fa91746039");
}

TEST(Crypto_SHA384, one_million_a_chars) {
  const std::string input(1000000, 'a');
  std::ostringstream result;
  sourcemeta::core::sha384(input, result);
  EXPECT_EQ(result.str(), "9d0e1809716474cb086e834e310a4a1ced149e9c00f2485279"
                          "72cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985");
}

TEST(Crypto_SHA384, embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  std::ostringstream result;
  sourcemeta::core::sha384(input, result);
  EXPECT_EQ(result.str(), "4f895854c1a4fc5aa2e0456eaf8d0ecaa70c196bd901153861"
                          "d76b8fa3cd95ceea29eab6a279f8b08437703ce0b4b91a");
}

TEST(Crypto_SHA384, to_string_empty) {
  EXPECT_EQ(sourcemeta::core::sha384(""),
            "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c"
            "0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");
}

TEST(Crypto_SHA384, to_string_abc) {
  EXPECT_EQ(sourcemeta::core::sha384("abc"),
            "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a"
            "8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7");
}

TEST(Crypto_SHA384, to_string_example) {
  EXPECT_EQ(sourcemeta::core::sha384("foo bar"),
            "6839312f3db343477070d3c0b2becd417b357154d48794d01d"
            "78cfb4617ed5ab819a77b6832f6542dd18bb738131ef7e");
}

TEST(Crypto_SHA384, to_string_quick_brown_fox) {
  EXPECT_EQ(
      sourcemeta::core::sha384("The quick brown fox jumps over the lazy dog"),
      "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c4940"
      "11e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1");
}

TEST(Crypto_SHA384, to_string_one_million_a_chars) {
  const std::string input(1000000, 'a');
  EXPECT_EQ(sourcemeta::core::sha384(input),
            "9d0e1809716474cb086e834e310a4a1ced149e9c00f2485279"
            "72cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985");
}

TEST(Crypto_SHA384, to_string_embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  EXPECT_EQ(sourcemeta::core::sha384(input),
            "4f895854c1a4fc5aa2e0456eaf8d0ecaa70c196bd901153861"
            "d76b8fa3cd95ceea29eab6a279f8b08437703ce0b4b91a");
}

TEST(Crypto_SHA384, digest_empty) {
  const std::array<std::uint8_t, 48> expected{
      {0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38, 0x4c, 0xd9, 0x32, 0x7e,
       0xb1, 0xb1, 0xe3, 0x6a, 0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
       0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda, 0x27, 0x4e, 0xde, 0xbf,
       0xe7, 0x6f, 0x65, 0xfb, 0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b}};
  EXPECT_EQ(sourcemeta::core::sha384_digest(""), expected);
}

TEST(Crypto_SHA384, digest_abc) {
  const std::array<std::uint8_t, 48> expected{
      {0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b, 0xb5, 0xa0, 0x3d, 0x69,
       0x9a, 0xc6, 0x50, 0x07, 0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63,
       0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed, 0x80, 0x86, 0x07, 0x2b,
       0xa1, 0xe7, 0xcc, 0x23, 0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7}};
  EXPECT_EQ(sourcemeta::core::sha384_digest("abc"), expected);
}
