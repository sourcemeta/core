#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

#include <sstream>
#include <string>

// Try $ echo -n "" | sha256sum
TEST(Crypto_SHA256, empty_string) {
  std::ostringstream result;
  sourcemeta::core::sha256("", result);
  EXPECT_EQ(result.str(),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

// Try $ echo -n "abc" | sha256sum
TEST(Crypto_SHA256, abc_string) {
  std::ostringstream result;
  sourcemeta::core::sha256("abc", result);
  EXPECT_EQ(result.str(),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Try $ echo -n "foo bar" | sha256sum
TEST(Crypto_SHA256, example_string) {
  std::ostringstream result;
  sourcemeta::core::sha256("foo bar", result);
  EXPECT_EQ(result.str(),
            "fbc1a9f858ea9e177916964bd88c3d37b91a1e84412765e29950777f265c4b75");
}

// Try $ echo -n "The quick brown fox jumps over the lazy dog" | sha256sum
TEST(Crypto_SHA256, quick_brown_fox) {
  std::ostringstream result;
  sourcemeta::core::sha256("The quick brown fox jumps over the lazy dog",
                           result);
  EXPECT_EQ(result.str(),
            "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}

TEST(Crypto_SHA256, one_million_a_chars) {
  const std::string input(1000000, 'a');
  std::ostringstream result;
  sourcemeta::core::sha256(input, result);
  EXPECT_EQ(result.str(),
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

TEST(Crypto_SHA256, embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  std::ostringstream result;
  sourcemeta::core::sha256(input, result);
  EXPECT_EQ(result.str(),
            "ae4b3280e56e2faf83f414a6e3dabe9d5fbe18976544c05fed121accb85b53fc");
}
