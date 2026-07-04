#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string>      // std::string
#include <string_view> // std::string_view

static const std::string KEY{
    sourcemeta::core::hex_to_bytes(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f")
        .value()};

TEST(aes_256_gcm_round_trips) {
  const std::string_view plaintext{"a secret session cookie payload"};
  const auto sealed{sourcemeta::core::aes_256_gcm_seal(KEY, plaintext)};
  EXPECT_TRUE(sealed.has_value());
  const auto opened{sourcemeta::core::aes_256_gcm_unseal(KEY, sealed.value())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_256_gcm_round_trips_empty_plaintext) {
  const auto sealed{sourcemeta::core::aes_256_gcm_seal(KEY, "")};
  EXPECT_TRUE(sealed.has_value());
  const auto opened{sourcemeta::core::aes_256_gcm_unseal(KEY, sealed.value())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), "");
}

TEST(aes_256_gcm_round_trips_across_block_boundaries) {
  const std::string plaintext(100, 'z');
  const auto sealed{sourcemeta::core::aes_256_gcm_seal(KEY, plaintext)};
  EXPECT_TRUE(sealed.has_value());
  const auto opened{sourcemeta::core::aes_256_gcm_unseal(KEY, sealed.value())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_256_gcm_uses_a_fresh_nonce_per_seal) {
  // The random nonce makes two seals of the same plaintext differ
  const auto first{sourcemeta::core::aes_256_gcm_seal(KEY, "identical")};
  const auto second{sourcemeta::core::aes_256_gcm_seal(KEY, "identical")};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  EXPECT_NE(first.value(), second.value());
}

TEST(aes_256_gcm_unseal_rejects_a_tampered_tag) {
  auto sealed{sourcemeta::core::aes_256_gcm_seal(KEY, "hello").value()};
  sealed.back() = static_cast<char>(sealed.back() ^ 0x01);
  EXPECT_FALSE(sourcemeta::core::aes_256_gcm_unseal(KEY, sealed).has_value());
}

TEST(aes_256_gcm_unseal_rejects_a_tampered_ciphertext) {
  auto sealed{
      sourcemeta::core::aes_256_gcm_seal(KEY, "a longer payload here").value()};
  // The first ciphertext byte follows the twelve byte nonce
  sealed[12] = static_cast<char>(sealed[12] ^ 0x01);
  EXPECT_FALSE(sourcemeta::core::aes_256_gcm_unseal(KEY, sealed).has_value());
}

TEST(aes_256_gcm_unseal_rejects_a_different_key) {
  const auto sealed{sourcemeta::core::aes_256_gcm_seal(KEY, "hello").value()};
  const auto other{sourcemeta::core::hex_to_bytes(
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff")};
  EXPECT_FALSE(
      sourcemeta::core::aes_256_gcm_unseal(other.value(), sealed).has_value());
}

TEST(aes_256_gcm_seal_rejects_a_wrong_size_key) {
  EXPECT_FALSE(sourcemeta::core::aes_256_gcm_seal(std::string(31, '\x00'), "x")
                   .has_value());
}

TEST(aes_256_gcm_unseal_rejects_a_short_input) {
  EXPECT_FALSE(
      sourcemeta::core::aes_256_gcm_unseal(KEY, "too short").has_value());
}
