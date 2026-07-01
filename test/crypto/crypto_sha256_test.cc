#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t
#include <sstream>
#include <string>

// Try $ echo -n "" | sha256sum
TEST(empty_string) {
  std::ostringstream result;
  sourcemeta::core::sha256("", result);
  EXPECT_EQ(result.str(),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

// Try $ echo -n "abc" | sha256sum
TEST(abc_string) {
  std::ostringstream result;
  sourcemeta::core::sha256("abc", result);
  EXPECT_EQ(result.str(),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Try $ echo -n "foo bar" | sha256sum
TEST(example_string) {
  std::ostringstream result;
  sourcemeta::core::sha256("foo bar", result);
  EXPECT_EQ(result.str(),
            "fbc1a9f858ea9e177916964bd88c3d37b91a1e84412765e29950777f265c4b75");
}

// Try $ echo -n "The quick brown fox jumps over the lazy dog" | sha256sum
TEST(quick_brown_fox) {
  std::ostringstream result;
  sourcemeta::core::sha256("The quick brown fox jumps over the lazy dog",
                           result);
  EXPECT_EQ(result.str(),
            "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}

TEST(one_million_a_chars) {
  const std::string input(1000000, 'a');
  std::ostringstream result;
  sourcemeta::core::sha256(input, result);
  EXPECT_EQ(result.str(),
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

TEST(embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  std::ostringstream result;
  sourcemeta::core::sha256(input, result);
  EXPECT_EQ(result.str(),
            "ae4b3280e56e2faf83f414a6e3dabe9d5fbe18976544c05fed121accb85b53fc");
}

TEST(to_string_empty) {
  EXPECT_EQ(sourcemeta::core::sha256(""),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(to_string_abc) {
  EXPECT_EQ(sourcemeta::core::sha256("abc"),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST(to_string_example) {
  EXPECT_EQ(sourcemeta::core::sha256("foo bar"),
            "fbc1a9f858ea9e177916964bd88c3d37b91a1e84412765e29950777f265c4b75");
}

TEST(to_string_quick_brown_fox) {
  EXPECT_EQ(
      sourcemeta::core::sha256("The quick brown fox jumps over the lazy dog"),
      "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}

TEST(to_string_one_million_a_chars) {
  const std::string input(1000000, 'a');
  EXPECT_EQ(sourcemeta::core::sha256(input),
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

TEST(to_string_embedded_nuls_and_binary_bytes) {
  const std::string input("\x00\x01\x02", 3);
  EXPECT_EQ(sourcemeta::core::sha256(input),
            "ae4b3280e56e2faf83f414a6e3dabe9d5fbe18976544c05fed121accb85b53fc");
}

TEST(digest_empty) {
  const std::array<std::uint8_t, 32> expected{
      {0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4,
       0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b,
       0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55}};
  EXPECT_EQ(sourcemeta::core::sha256_digest(""), expected);
}

TEST(digest_abc) {
  const std::array<std::uint8_t, 32> expected{
      {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40,
       0xde, 0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17,
       0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad}};
  EXPECT_EQ(sourcemeta::core::sha256_digest("abc"), expected);
}
