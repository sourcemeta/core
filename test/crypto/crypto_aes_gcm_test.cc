#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string>      // std::string
#include <string_view> // std::string_view

static const std::string KEY{
    sourcemeta::core::hex_to_bytes(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f")
        .value()};

static const std::string KEY_128{
    sourcemeta::core::hex_to_bytes("000102030405060708090a0b0c0d0e0f").value()};

static const std::string KEY_192{
    sourcemeta::core::hex_to_bytes(
        "000102030405060708090a0b0c0d0e0f1011121314151617")
        .value()};

static const std::string IV{
    sourcemeta::core::hex_to_bytes("101112131415161718191a1b").value()};

static constexpr std::string_view ASSOCIATED_DATA{"protected-header"};

static constexpr std::string::size_type TAG_SIZE{16};

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

TEST(aes_gcm_round_trips_with_a_128_bit_key) {
  const std::string_view plaintext{"a secret session cookie payload"};
  const auto result{sourcemeta::core::aes_gcm_encrypt(
      KEY_128, IV, ASSOCIATED_DATA, plaintext)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().ciphertext().size(), plaintext.size());
  EXPECT_EQ(result.value().tag().size(), TAG_SIZE);
  const auto opened{sourcemeta::core::aes_gcm_decrypt(
      KEY_128, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_gcm_round_trips_with_a_192_bit_key) {
  const std::string_view plaintext{"a secret session cookie payload"};
  const auto result{sourcemeta::core::aes_gcm_encrypt(
      KEY_192, IV, ASSOCIATED_DATA, plaintext)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().ciphertext().size(), plaintext.size());
  EXPECT_EQ(result.value().tag().size(), TAG_SIZE);
  const auto opened{sourcemeta::core::aes_gcm_decrypt(
      KEY_192, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_gcm_round_trips_with_a_256_bit_key) {
  const std::string_view plaintext{"a secret session cookie payload"};
  const auto result{
      sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA, plaintext)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().ciphertext().size(), plaintext.size());
  EXPECT_EQ(result.value().tag().size(), TAG_SIZE);
  const auto opened{sourcemeta::core::aes_gcm_decrypt(
      KEY, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_gcm_round_trips_across_block_boundaries) {
  const std::string plaintext(100, 'z');
  const auto result{
      sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA, plaintext)};
  EXPECT_TRUE(result.has_value());
  const auto opened{sourcemeta::core::aes_gcm_decrypt(
      KEY, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_gcm_round_trips_an_empty_plaintext) {
  const auto result{
      sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA, "")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().ciphertext().size(), std::string::size_type{0});
  EXPECT_EQ(result.value().tag().size(), TAG_SIZE);
  const auto opened{sourcemeta::core::aes_gcm_decrypt(
      KEY, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), "");
}

TEST(aes_gcm_round_trips_empty_associated_data) {
  const std::string_view plaintext{"payload without associated data"};
  const auto result{sourcemeta::core::aes_gcm_encrypt(KEY, IV, "", plaintext)};
  EXPECT_TRUE(result.has_value());
  const auto opened{sourcemeta::core::aes_gcm_decrypt(
      KEY, IV, "", result.value().ciphertext(), result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_gcm_encrypt_is_deterministic_for_the_same_inputs) {
  // Unlike the self-sealing helper, this primitive takes the initialization
  // vector from the caller, so the same inputs produce the same output
  const auto first{
      sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA, "identical")};
  const auto second{
      sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA, "identical")};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  EXPECT_EQ(first.value().ciphertext(), second.value().ciphertext());
  EXPECT_EQ(first.value().tag(), second.value().tag());
}

TEST(aes_gcm_decrypt_rejects_different_associated_data) {
  const auto result{
      sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA, "hello")};
  const auto opened{sourcemeta::core::aes_gcm_decrypt(
      KEY, IV, "different-header", result.value().ciphertext(),
      result.value().tag())};
  EXPECT_FALSE(opened.has_value());
}

TEST(aes_gcm_decrypt_rejects_a_tampered_tag) {
  auto result{
      sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA, "hello")
          .value()};
  // The tag is the final sixteen bytes of the buffer
  result.data.back() = static_cast<char>(result.data.back() ^ 0x01);
  EXPECT_FALSE(sourcemeta::core::aes_gcm_decrypt(
                   KEY, IV, ASSOCIATED_DATA, result.ciphertext(), result.tag())
                   .has_value());
}

TEST(aes_gcm_decrypt_rejects_a_tampered_ciphertext) {
  auto result{sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA,
                                                "a longer payload here")
                  .value()};
  // The first byte of the buffer is the first ciphertext byte
  result.data[0] = static_cast<char>(result.data[0] ^ 0x01);
  EXPECT_FALSE(sourcemeta::core::aes_gcm_decrypt(
                   KEY, IV, ASSOCIATED_DATA, result.ciphertext(), result.tag())
                   .has_value());
}

TEST(aes_gcm_decrypt_rejects_a_different_key) {
  const auto result{
      sourcemeta::core::aes_gcm_encrypt(KEY, IV, ASSOCIATED_DATA, "hello")};
  EXPECT_FALSE(sourcemeta::core::aes_gcm_decrypt(KEY_128, IV, ASSOCIATED_DATA,
                                                 result.value().ciphertext(),
                                                 result.value().tag())
                   .has_value());
}

TEST(aes_gcm_encrypt_rejects_a_wrong_size_key) {
  EXPECT_FALSE(
      sourcemeta::core::aes_gcm_encrypt(std::string(20, '\x00'), IV, "", "x")
          .has_value());
}

TEST(aes_gcm_encrypt_rejects_a_wrong_size_iv) {
  EXPECT_FALSE(
      sourcemeta::core::aes_gcm_encrypt(KEY, std::string(16, '\x00'), "", "x")
          .has_value());
}

TEST(aes_gcm_decrypt_rejects_a_wrong_size_tag) {
  const auto result{sourcemeta::core::aes_gcm_encrypt(KEY, IV, "", "hello")};
  EXPECT_FALSE(sourcemeta::core::aes_gcm_decrypt(KEY, IV, "",
                                                 result.value().ciphertext(),
                                                 std::string(15, '\x00'))
                   .has_value());
}
