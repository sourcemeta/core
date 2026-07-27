#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t
#include <span>    // std::span
#include <sstream>
#include <string>
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace {
auto digest_to_hex(const std::array<std::uint8_t, 32> &digest) -> std::string {
  return sourcemeta::core::bytes_to_hex(
      {reinterpret_cast<const char *>(digest.data()), digest.size()});
}
} // namespace

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

// Try $ echo -n "" | sha256sum
TEST(digest_parts_empty_span) {
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(
                std::span<const std::string_view>{})),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

// Try $ echo -n "" | sha256sum
TEST(digest_parts_one_empty_part) {
  const std::array<std::string_view, 1> parts{{""}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

// Try $ echo -n "" | sha256sum
TEST(digest_parts_three_empty_parts) {
  const std::array<std::string_view, 3> parts{{"", "", ""}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

// Try $ echo -n "abc" | sha256sum
TEST(digest_parts_single_abc) {
  const std::array<std::string_view, 1> parts{{"abc"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Try $ echo -n "abc" | sha256sum
TEST(digest_parts_abc_first_letter_split) {
  const std::array<std::string_view, 2> parts{{"a", "bc"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Try $ echo -n "abc" | sha256sum
TEST(digest_parts_abc_last_letter_split) {
  const std::array<std::string_view, 2> parts{{"ab", "c"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Try $ echo -n "abc" | sha256sum
TEST(digest_parts_abc_letter_by_letter) {
  const std::array<std::string_view, 3> parts{{"a", "b", "c"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Try $ echo -n "abc" | sha256sum
TEST(digest_parts_abc_with_empty_parts_between) {
  const std::array<std::string_view, 5> parts{{"", "a", "", "bc", ""}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Try $ echo -n "foo bar" | sha256sum
TEST(digest_parts_example_string) {
  const std::array<std::string_view, 2> parts{{"foo ", "bar"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "fbc1a9f858ea9e177916964bd88c3d37b91a1e84412765e29950777f265c4b75");
}

// Try $ echo -n "The quick brown fox jumps over the lazy dog" | sha256sum
TEST(digest_parts_quick_brown_fox) {
  const std::array<std::string_view, 3> parts{
      {"The quick brown ", "fox jumps ", "over the lazy dog"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}

// FIPS 180-4 example message of 56 bytes, filling the final block up to the
// point where the length no longer fits and a second padding block is needed
TEST(digest_parts_two_padding_blocks_single) {
  const std::array<std::string_view, 1> parts{
      {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

// FIPS 180-4 example message of 56 bytes, filling the final block up to the
// point where the length no longer fits and a second padding block is needed
TEST(digest_parts_two_padding_blocks_split) {
  const std::array<std::string_view, 2> parts{
      {"abcdbcdecdefdefgefghfghighij", "hijkijkljklmklmnlmnomnopnopq"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

TEST(digest_parts_55_bytes_single) {
  const std::string input(55, 'a');
  const std::array<std::string_view, 1> parts{{input}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "9f4390f8d30c2dd92ec9f095b65e2b9ae9b0a925a5258e241c9f1e910f734318");
}

TEST(digest_parts_55_bytes_split) {
  const std::string first(27, 'a');
  const std::string second(28, 'a');
  const std::array<std::string_view, 2> parts{{first, second}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "9f4390f8d30c2dd92ec9f095b65e2b9ae9b0a925a5258e241c9f1e910f734318");
}

TEST(digest_parts_56_bytes_split) {
  const std::string first(55, 'a');
  const std::array<std::string_view, 2> parts{{first, "a"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "b35439a4ac6f0948b6d6f9e3c6af0f5f590ce20f1bde7090ef7970686ec6738a");
}

TEST(digest_parts_57_bytes_split) {
  const std::string second(56, 'a');
  const std::array<std::string_view, 2> parts{{"a", second}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "f13b2d724659eb3bf47f2dd6af1accc87b81f09f59f2b75e5c0bed6589dfe8c6");
}

TEST(digest_parts_63_bytes_split) {
  const std::string first(62, 'a');
  const std::array<std::string_view, 2> parts{{first, "a"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "7d3e74a05d7db15bce4ad9ec0658ea98e3f06eeecf16b4c6fff2da457ddc2f34");
}

TEST(digest_parts_64_bytes_single) {
  const std::string input(64, 'a');
  const std::array<std::string_view, 1> parts{{input}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb");
}

TEST(digest_parts_64_bytes_split_last_byte) {
  const std::string first(63, 'a');
  const std::array<std::string_view, 2> parts{{first, "a"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb");
}

TEST(digest_parts_64_bytes_split_first_byte) {
  const std::string second(63, 'a');
  const std::array<std::string_view, 2> parts{{"a", second}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb");
}

TEST(digest_parts_64_bytes_split_halves) {
  const std::string first(32, 'a');
  const std::string second(32, 'a');
  const std::array<std::string_view, 2> parts{{first, second}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb");
}

TEST(digest_parts_65_bytes_split_after_block) {
  const std::string first(64, 'a');
  const std::array<std::string_view, 2> parts{{first, "a"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "635361c48bb9eab14198e76ea8ab7f1a41685d6ad62aa9146d301d4f17eb0ae0");
}

TEST(digest_parts_65_bytes_split_before_block) {
  const std::string second(64, 'a');
  const std::array<std::string_view, 2> parts{{"a", second}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "635361c48bb9eab14198e76ea8ab7f1a41685d6ad62aa9146d301d4f17eb0ae0");
}

// FIPS 180-4 example message of 112 bytes spanning two full blocks
TEST(digest_parts_two_block_message_split_at_block) {
  const std::array<std::string_view, 2> parts{
      {"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn",
       "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1");
}

// FIPS 180-4 example message of 112 bytes spanning two full blocks
TEST(digest_parts_two_block_message_split_unevenly) {
  const std::array<std::string_view, 2> parts{
      {"a", "bcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
            "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1");
}

TEST(digest_parts_part_crossing_block_boundary) {
  const std::array<std::string_view, 2> parts{
      {"0123456789012345678901234567890123456789",
       "0123456789012345678901234567890123456789"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "af1909413b96cbb29927b3a67f3a8879c801a37be383e5f9b31df5fa8d10fa2b");
}

TEST(digest_parts_small_part_then_multi_block_part) {
  const std::string second(200, 'b');
  const std::array<std::string_view, 2> parts{{"0123456789", second}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "c65c2079436f4c3794c35a33d3b74abe2af91e7dd0e73b214ae3fb519af79b7e");
}

TEST(digest_parts_one_hundred_fifty_single_byte_parts) {
  const std::vector<std::string_view> parts(150, "a");
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "7595af82ae2fa59cd9bf3b4405d31c69b98de71fed5945fd777d8ab3b393a85f");
}

TEST(digest_parts_one_million_a_chars_three_parts) {
  const std::string input(1000000, 'a');
  const std::string_view view{input};
  const std::array<std::string_view, 3> parts{
      {view.substr(0, 1), view.substr(1, 999998), view.substr(999999)}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

TEST(digest_parts_embedded_nuls_and_binary_bytes) {
  const std::array<std::string_view, 2> parts{
      {std::string_view{"\x00\x01", 2}, std::string_view{"\x02", 1}}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "ae4b3280e56e2faf83f414a6e3dabe9d5fbe18976544c05fed121accb85b53fc");
}

// Try $ echo -n "The quick brown fox jumps over the lazy dog" | sha256sum
TEST(digest_parts_views_into_same_buffer) {
  const std::string input{"The quick brown fox jumps over the lazy dog"};
  const std::string_view view{input};
  const std::array<std::string_view, 4> parts{
      {view.substr(0, 10), view.substr(10, 1), view.substr(11, 21),
       view.substr(32)}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::sha256_digest(parts)),
            "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}
