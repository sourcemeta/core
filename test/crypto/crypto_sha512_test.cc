#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t
#include <sstream> // std::ostringstream
#include <string>  // std::string

// Try $ echo -n "" | shasum -a 512
TEST(Crypto_SHA512, empty_string) {
  std::ostringstream result;
  sourcemeta::core::sha512("", result);
  EXPECT_EQ(result.str(),
            "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
            "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
}

// Try $ echo -n "abc" | shasum -a 512
TEST(Crypto_SHA512, abc_string) {
  std::ostringstream result;
  sourcemeta::core::sha512("abc", result);
  EXPECT_EQ(result.str(),
            "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
            "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
}

// Try $ echo -n "foo bar" | shasum -a 512
TEST(Crypto_SHA512, example_string) {
  std::ostringstream result;
  sourcemeta::core::sha512("foo bar", result);
  EXPECT_EQ(result.str(),
            "65019286222ace418f742556366f9b9da5aaf6797527d2f0cba5bfe6b2f8ed24"
            "746542a0f2be1da8d63c2477f688b608eb53628993afa624f378b03f10090ce7");
}

// Try $ echo -n "The quick brown fox jumps over the lazy dog" | shasum -a 512
TEST(Crypto_SHA512, quick_brown_fox) {
  std::ostringstream result;
  sourcemeta::core::sha512("The quick brown fox jumps over the lazy dog",
                           result);
  EXPECT_EQ(result.str(),
            "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb64"
            "2e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6");
}

// FIPS 180-4 two-block message sample
TEST(Crypto_SHA512, fips_two_block_message) {
  std::ostringstream result;
  sourcemeta::core::sha512("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmgh"
                           "ijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnop"
                           "qrstnopqrstu",
                           result);
  EXPECT_EQ(result.str(),
            "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018"
            "501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909");
}

TEST(Crypto_SHA512, one_million_a_chars) {
  const std::string input(1000000, 'a');
  std::ostringstream result;
  sourcemeta::core::sha512(input, result);
  EXPECT_EQ(result.str(),
            "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973eb"
            "de0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b");
}

TEST(Crypto_SHA512, embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  std::ostringstream result;
  sourcemeta::core::sha512(input, result);
  EXPECT_EQ(result.str(),
            "8081da5f9c1e3d0e1aa16f604d5e5064543cff5d7bace2bb312252461e151b3f"
            "e0f034ea8dc1dacff3361a892d625fbe1b614cda265f87a473c24b0fa1d91dfd");
}

TEST(Crypto_SHA512, to_string_empty) {
  EXPECT_EQ(sourcemeta::core::sha512(""),
            "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
            "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
}

TEST(Crypto_SHA512, to_string_abc) {
  EXPECT_EQ(sourcemeta::core::sha512("abc"),
            "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
            "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
}

TEST(Crypto_SHA512, to_string_example) {
  EXPECT_EQ(sourcemeta::core::sha512("foo bar"),
            "65019286222ace418f742556366f9b9da5aaf6797527d2f0cba5bfe6b2f8ed24"
            "746542a0f2be1da8d63c2477f688b608eb53628993afa624f378b03f10090ce7");
}

TEST(Crypto_SHA512, to_string_quick_brown_fox) {
  EXPECT_EQ(
      sourcemeta::core::sha512("The quick brown fox jumps over the lazy dog"),
      "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb64"
      "2e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6");
}

TEST(Crypto_SHA512, to_string_one_million_a_chars) {
  const std::string input(1000000, 'a');
  EXPECT_EQ(sourcemeta::core::sha512(input),
            "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973eb"
            "de0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b");
}

TEST(Crypto_SHA512, to_string_embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  EXPECT_EQ(sourcemeta::core::sha512(input),
            "8081da5f9c1e3d0e1aa16f604d5e5064543cff5d7bace2bb312252461e151b3f"
            "e0f034ea8dc1dacff3361a892d625fbe1b614cda265f87a473c24b0fa1d91dfd");
}

TEST(Crypto_SHA512, digest_empty) {
  const std::array<std::uint8_t, 64> expected{
      {0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd, 0xf1, 0x54, 0x28,
       0x50, 0xd6, 0x6d, 0x80, 0x07, 0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57,
       0x15, 0xdc, 0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce, 0x47,
       0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0, 0xff, 0x83, 0x18, 0xd2,
       0x87, 0x7e, 0xec, 0x2f, 0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a,
       0x81, 0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e}};
  EXPECT_EQ(sourcemeta::core::sha512_digest(""), expected);
}

TEST(Crypto_SHA512, digest_abc) {
  const std::array<std::uint8_t, 64> expected{
      {0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba, 0xcc, 0x41, 0x73,
       0x49, 0xae, 0x20, 0x41, 0x31, 0x12, 0xe6, 0xfa, 0x4e, 0x89, 0xa9,
       0x7e, 0xa2, 0x0a, 0x9e, 0xee, 0xe6, 0x4b, 0x55, 0xd3, 0x9a, 0x21,
       0x92, 0x99, 0x2a, 0x27, 0x4f, 0xc1, 0xa8, 0x36, 0xba, 0x3c, 0x23,
       0xa3, 0xfe, 0xeb, 0xbd, 0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c, 0xe8,
       0x0e, 0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4, 0x9f}};
  EXPECT_EQ(sourcemeta::core::sha512_digest("abc"), expected);
}
