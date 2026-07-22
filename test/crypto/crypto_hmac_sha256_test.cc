#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <span>        // std::span
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace {
auto digest_to_hex(const std::array<std::uint8_t, 32> &digest) -> std::string {
  return sourcemeta::core::bytes_to_hex(
      {reinterpret_cast<const char *>(digest.data()), digest.size()});
}
} // namespace

// RFC 4231 Section 4.2 (Test Case 1)
TEST(rfc4231_case_1) {
  const std::string key(20, '\x0b');
  EXPECT_EQ(sourcemeta::core::hmac_sha256(key, "Hi There"),
            "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
}

// RFC 4231 Section 4.3 (Test Case 2)
TEST(rfc4231_case_2) {
  EXPECT_EQ(
      sourcemeta::core::hmac_sha256("Jefe", "what do ya want for nothing?"),
      "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");
}

// RFC 4231 Section 4.4 (Test Case 3)
TEST(rfc4231_case_3) {
  const std::string key(20, '\xaa');
  const std::string message(50, '\xdd');
  EXPECT_EQ(sourcemeta::core::hmac_sha256(key, message),
            "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe");
}

// RFC 4231 Section 4.5 (Test Case 4)
TEST(rfc4231_case_4) {
  const auto key{sourcemeta::core::hex_to_bytes(
                     "0102030405060708090a0b0c0d0e0f10111213141516171819")
                     .value()};
  const std::string message(50, '\xcd');
  EXPECT_EQ(sourcemeta::core::hmac_sha256(key, message),
            "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b");
}

// RFC 4231 Section 4.7 (Test Case 6), a key longer than the block size
TEST(rfc4231_case_6) {
  const std::string key(131, '\xaa');
  EXPECT_EQ(sourcemeta::core::hmac_sha256(
                key, "Test Using Larger Than Block-Size Key - Hash Key First"),
            "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54");
}

// RFC 4231 Section 4.8 (Test Case 7), a key longer than the block size
TEST(rfc4231_case_7) {
  const std::string key(131, '\xaa');
  EXPECT_EQ(sourcemeta::core::hmac_sha256(
                key, "This is a test using a larger than block-size key and a "
                     "larger than block-size data. The key needs to be hashed "
                     "before being used by the HMAC algorithm."),
            "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2");
}

TEST(empty_key_and_message) {
  EXPECT_EQ(sourcemeta::core::hmac_sha256("", ""),
            "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad");
}

TEST(null_key_view) {
  EXPECT_EQ(sourcemeta::core::hmac_sha256(std::string_view{}, ""),
            "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad");
}

TEST(ostream_case_2) {
  std::ostringstream result;
  sourcemeta::core::hmac_sha256("Jefe", "what do ya want for nothing?", result);
  EXPECT_EQ(result.str(),
            "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");
}

// RFC 4231 Section 4.3 (Test Case 2)
TEST(digest_case_2) {
  const std::array<std::uint8_t, 32> expected{
      {0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e, 0x6a, 0x04, 0x24,
       0x26, 0x08, 0x95, 0x75, 0xc7, 0x5a, 0x00, 0x3f, 0x08, 0x9d, 0x27,
       0x39, 0x83, 0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec, 0x38, 0x43}};
  EXPECT_EQ(sourcemeta::core::hmac_sha256_digest(
                "Jefe", "what do ya want for nothing?"),
            expected);
}

// RFC 4231 Section 4.2 (Test Case 1)
TEST(digest_parts_rfc4231_case_1_single) {
  const std::string key(20, '\x0b');
  const std::array<std::string_view, 1> parts{{"Hi There"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(key, parts)),
            "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
}

// RFC 4231 Section 4.2 (Test Case 1)
TEST(digest_parts_rfc4231_case_1_split) {
  const std::string key(20, '\x0b');
  const std::array<std::string_view, 2> parts{{"Hi ", "There"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(key, parts)),
            "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
}

// RFC 4231 Section 4.2 (Test Case 1)
TEST(digest_parts_rfc4231_case_1_with_empty_parts) {
  const std::string key(20, '\x0b');
  const std::array<std::string_view, 3> parts{{"", "Hi There", ""}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(key, parts)),
            "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
}

// RFC 4231 Section 4.3 (Test Case 2)
TEST(digest_parts_rfc4231_case_2_split) {
  const std::array<std::string_view, 2> parts{
      {"what do ya want ", "for nothing?"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest("Jefe", parts)),
            "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");
}

// RFC 4231 Section 4.4 (Test Case 3)
TEST(digest_parts_rfc4231_case_3_split_halves) {
  const std::string key(20, '\xaa');
  const std::string first(25, '\xdd');
  const std::string second(25, '\xdd');
  const std::array<std::string_view, 2> parts{{first, second}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(key, parts)),
            "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe");
}

// RFC 4231 Section 4.4 (Test Case 3)
TEST(digest_parts_rfc4231_case_3_single_byte_parts) {
  const std::string key(20, '\xaa');
  const std::vector<std::string_view> parts(50, "\xdd");
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(key, parts)),
            "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe");
}

// RFC 4231 Section 4.5 (Test Case 4)
TEST(digest_parts_rfc4231_case_4_split) {
  const auto key{sourcemeta::core::hex_to_bytes(
                     "0102030405060708090a0b0c0d0e0f10111213141516171819")
                     .value()};
  const std::string message(50, '\xcd');
  const std::string_view view{message};
  const std::array<std::string_view, 2> parts{
      {view.substr(0, 10), view.substr(10)}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(key, parts)),
            "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b");
}

// RFC 4231 Section 4.7 (Test Case 6), a key longer than the block size
TEST(digest_parts_rfc4231_case_6_split) {
  const std::string key(131, '\xaa');
  const std::array<std::string_view, 2> parts{
      {"Test Using Larger Than Block-Size Key", " - Hash Key First"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(key, parts)),
            "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54");
}

// RFC 4231 Section 4.8 (Test Case 7), a key longer than the block size and a
// message split so that a part crosses the block boundary
TEST(digest_parts_rfc4231_case_7_three_parts) {
  const std::string key(131, '\xaa');
  const std::string message{
      "This is a test using a larger than block-size key and a "
      "larger than block-size data. The key needs to be hashed "
      "before being used by the HMAC algorithm."};
  const std::string_view view{message};
  const std::array<std::string_view, 3> parts{
      {view.substr(0, 40), view.substr(40, 40), view.substr(80)}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(key, parts)),
            "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2");
}

TEST(digest_parts_empty_key_empty_span) {
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(
                "", std::span<const std::string_view>{})),
            "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad");
}

TEST(digest_parts_empty_key_empty_part) {
  const std::array<std::string_view, 1> parts{{""}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest("", parts)),
            "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad");
}

TEST(digest_parts_null_key_view_empty_span) {
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest(
                std::string_view{}, std::span<const std::string_view>{})),
            "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad");
}

TEST(digest_parts_message_part_crossing_block_boundary) {
  const std::array<std::string_view, 2> parts{
      {"0123456789012345678901234567890123456789",
       "0123456789012345678901234567890123456789"}};
  EXPECT_EQ(digest_to_hex(sourcemeta::core::hmac_sha256_digest("key", parts)),
            "fb39f382d4c6474312831ee858de2c270451a90f6e6dcc48594aa24eec785584");
}
