#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string>      // std::string
#include <string_view> // std::string_view

static const std::string KEK_128{
    sourcemeta::core::hex_to_bytes("000102030405060708090a0b0c0d0e0f").value()};

static const std::string KEK_192{
    sourcemeta::core::hex_to_bytes(
        "000102030405060708090a0b0c0d0e0f1011121314151617")
        .value()};

static const std::string KEK_256{
    sourcemeta::core::hex_to_bytes(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f")
        .value()};

// The key being wrapped, a 128-bit content encryption key
static const std::string KEY{
    sourcemeta::core::hex_to_bytes("00112233445566778899aabbccddeeff").value()};

TEST(aes_key_wrap_round_trips_with_a_128_bit_kek) {
  const auto wrapped{sourcemeta::core::aes_key_wrap(KEK_128, KEY)};
  EXPECT_TRUE(wrapped.has_value());
  const auto unwrapped{
      sourcemeta::core::aes_key_unwrap(KEK_128, wrapped.value())};
  EXPECT_TRUE(unwrapped.has_value());
  EXPECT_EQ(unwrapped.value(), KEY);
}

TEST(aes_key_wrap_round_trips_with_a_192_bit_kek) {
  const auto wrapped{sourcemeta::core::aes_key_wrap(KEK_192, KEY)};
  EXPECT_TRUE(wrapped.has_value());
  const auto unwrapped{
      sourcemeta::core::aes_key_unwrap(KEK_192, wrapped.value())};
  EXPECT_TRUE(unwrapped.has_value());
  EXPECT_EQ(unwrapped.value(), KEY);
}

TEST(aes_key_wrap_round_trips_with_a_256_bit_kek) {
  const auto wrapped{sourcemeta::core::aes_key_wrap(KEK_256, KEY)};
  EXPECT_TRUE(wrapped.has_value());
  const auto unwrapped{
      sourcemeta::core::aes_key_unwrap(KEK_256, wrapped.value())};
  EXPECT_TRUE(unwrapped.has_value());
  EXPECT_EQ(unwrapped.value(), KEY);
}

TEST(aes_key_wrap_output_is_eight_bytes_longer) {
  const auto wrapped{sourcemeta::core::aes_key_wrap(KEK_128, KEY)};
  EXPECT_TRUE(wrapped.has_value());
  EXPECT_EQ(wrapped.value().size(), KEY.size() + 8);
}

TEST(aes_key_wrap_round_trips_a_longer_key) {
  const std::string key(32, 'k');
  const auto wrapped{sourcemeta::core::aes_key_wrap(KEK_256, key)};
  EXPECT_TRUE(wrapped.has_value());
  const auto unwrapped{
      sourcemeta::core::aes_key_unwrap(KEK_256, wrapped.value())};
  EXPECT_TRUE(unwrapped.has_value());
  EXPECT_EQ(unwrapped.value(), key);
}

TEST(aes_key_wrap_is_deterministic) {
  const auto first{sourcemeta::core::aes_key_wrap(KEK_128, KEY)};
  const auto second{sourcemeta::core::aes_key_wrap(KEK_128, KEY)};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  EXPECT_EQ(first.value(), second.value());
}

TEST(aes_key_unwrap_rejects_a_tampered_wrapping) {
  auto wrapped{sourcemeta::core::aes_key_wrap(KEK_128, KEY).value()};
  wrapped.front() = static_cast<char>(wrapped.front() ^ 0x01);
  EXPECT_FALSE(sourcemeta::core::aes_key_unwrap(KEK_128, wrapped).has_value());
}

TEST(aes_key_unwrap_rejects_a_different_key) {
  const auto wrapped{sourcemeta::core::aes_key_wrap(KEK_128, KEY)};
  EXPECT_FALSE(
      sourcemeta::core::aes_key_unwrap(KEK_256, wrapped.value()).has_value());
}

TEST(aes_key_wrap_rejects_a_wrong_size_key_encryption_key) {
  EXPECT_FALSE(
      sourcemeta::core::aes_key_wrap(std::string(20, '\x00'), KEY).has_value());
}

TEST(aes_key_wrap_rejects_a_key_below_the_minimum) {
  EXPECT_FALSE(sourcemeta::core::aes_key_wrap(KEK_128, std::string(8, '\x00'))
                   .has_value());
}

TEST(aes_key_wrap_rejects_a_non_block_key) {
  EXPECT_FALSE(sourcemeta::core::aes_key_wrap(KEK_128, std::string(20, '\x00'))
                   .has_value());
}

TEST(aes_key_unwrap_rejects_a_short_input) {
  EXPECT_FALSE(
      sourcemeta::core::aes_key_unwrap(KEK_128, std::string(16, '\x00'))
          .has_value());
}

TEST(aes_key_unwrap_rejects_a_non_block_input) {
  EXPECT_FALSE(
      sourcemeta::core::aes_key_unwrap(KEK_128, std::string(28, '\x00'))
          .has_value());
}
