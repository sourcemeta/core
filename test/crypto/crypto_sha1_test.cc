#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

#include <sstream>
#include <string>

// Try $ echo -n "" | sha1sum
TEST(Crypto_SHA1, empty_string) {
  std::ostringstream result;
  sourcemeta::core::sha1("", result);
  EXPECT_EQ(result.str(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

// RFC 3174 Section 7.3 TEST1
// Try $ echo -n "abc" | sha1sum
TEST(Crypto_SHA1, abc_string) {
  std::ostringstream result;
  sourcemeta::core::sha1("abc", result);
  EXPECT_EQ(result.str(), "a9993e364706816aba3e25717850c26c9cd0d89d");
}

// RFC 3174 Section 7.3 TEST2
TEST(Crypto_SHA1, rfc3174_test_2) {
  std::ostringstream result;
  sourcemeta::core::sha1(
      "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", result);
  EXPECT_EQ(result.str(), "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
}

// RFC 3174 Section 7.3 TEST4
TEST(Crypto_SHA1, rfc3174_test_4) {
  std::string input;
  for (int repetition = 0; repetition < 10; ++repetition) {
    input += "0123456701234567012345670123456701234567012345670123456701234567";
  }

  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "dea356a2cddd90c7a7ecedc5ebb563934f460452");
}

// Try $ echo -n "foo bar" | sha1sum
TEST(Crypto_SHA1, example_string) {
  std::ostringstream result;
  sourcemeta::core::sha1("foo bar", result);
  EXPECT_EQ(result.str(), "3773dea65156909838fa6c22825cafe090ff8030");
}

// Try $ echo -n "The quick brown fox jumps over the lazy dog" | sha1sum
TEST(Crypto_SHA1, quick_brown_fox) {
  std::ostringstream result;
  sourcemeta::core::sha1("The quick brown fox jumps over the lazy dog", result);
  EXPECT_EQ(result.str(), "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
}

TEST(Crypto_SHA1, one_million_a_chars) {
  const std::string input(1000000, 'a');
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "34aa973cd4c4daa4f61eeb2bdbad27316534016f");
}

TEST(Crypto_SHA1, embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "0c7a623fd2bbc05b06423be359e4021d36e721ad");
}

// 55 bytes is the largest input that fits in a single padded block
TEST(Crypto_SHA1, padding_boundary_55_chars) {
  const std::string input(55, 'a');
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "c1c8bbdc22796e28c0e15163d20899b65621d65a");
}

// 56 bytes forces the length to spill into a second padded block
TEST(Crypto_SHA1, padding_boundary_56_chars) {
  const std::string input(56, 'a');
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "c2db330f6083854c99d4b5bfb6e8f29f201be699");
}

TEST(Crypto_SHA1, padding_boundary_63_chars) {
  const std::string input(63, 'a');
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "03f09f5b158a7a8cdad920bddc29b81c18a551f5");
}

TEST(Crypto_SHA1, padding_boundary_64_chars) {
  const std::string input(64, 'a');
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "0098ba824b5c16427bd7a1122a5a442a25ec644d");
}

TEST(Crypto_SHA1, padding_boundary_65_chars) {
  const std::string input(65, 'a');
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "11655326c708d70319be2610e8a57d9a5b959d3b");
}

TEST(Crypto_SHA1, padding_boundary_119_chars) {
  const std::string input(119, 'a');
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "ee971065aaa017e0632a8ca6c77bb3bf8b1dfc56");
}

TEST(Crypto_SHA1, padding_boundary_120_chars) {
  const std::string input(120, 'a');
  std::ostringstream result;
  sourcemeta::core::sha1(input, result);
  EXPECT_EQ(result.str(), "f34c1488385346a55709ba056ddd08280dd4c6d6");
}

TEST(Crypto_SHA1, to_string_empty) {
  EXPECT_EQ(sourcemeta::core::sha1(""),
            "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST(Crypto_SHA1, to_string_abc) {
  EXPECT_EQ(sourcemeta::core::sha1("abc"),
            "a9993e364706816aba3e25717850c26c9cd0d89d");
}

TEST(Crypto_SHA1, to_string_example) {
  EXPECT_EQ(sourcemeta::core::sha1("foo bar"),
            "3773dea65156909838fa6c22825cafe090ff8030");
}

TEST(Crypto_SHA1, to_string_quick_brown_fox) {
  EXPECT_EQ(
      sourcemeta::core::sha1("The quick brown fox jumps over the lazy dog"),
      "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
}

TEST(Crypto_SHA1, to_string_one_million_a_chars) {
  const std::string input(1000000, 'a');
  EXPECT_EQ(sourcemeta::core::sha1(input),
            "34aa973cd4c4daa4f61eeb2bdbad27316534016f");
}

TEST(Crypto_SHA1, to_string_embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  EXPECT_EQ(sourcemeta::core::sha1(input),
            "0c7a623fd2bbc05b06423be359e4021d36e721ad");
}
