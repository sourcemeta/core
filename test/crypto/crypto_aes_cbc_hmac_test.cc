#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// A 256-bit key selects A128CBC-HS256 (16-byte tag)
static const std::string KEY_256{
    sourcemeta::core::hex_to_bytes(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f")
        .value()};

// A 384-bit key selects A192CBC-HS384 (24-byte tag)
static const std::string KEY_384{
    sourcemeta::core::hex_to_bytes(
        "000102030405060708090a0b0c0d0e0f101112131415"
        "161718191a1b1c1d1e1f202122232425262728292a2b"
        "2c2d2e2f")
        .value()};

// A 512-bit key selects A256CBC-HS512 (32-byte tag)
static const std::string KEY_512{
    sourcemeta::core::hex_to_bytes(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2021"
        "22232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f")
        .value()};

static const std::string IV{
    sourcemeta::core::hex_to_bytes("101112131415161718191a1b1c1d1e1f").value()};

static constexpr std::string_view ASSOCIATED_DATA{"protected-header"};

TEST(aes_cbc_hmac_round_trips_a128cbc_hs256) {
  const std::string_view plaintext{"a secret session cookie payload"};
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, plaintext)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().tag().size(), std::string::size_type{16});
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_256, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_cbc_hmac_round_trips_a192cbc_hs384) {
  const std::string_view plaintext{"a secret session cookie payload"};
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_384, IV, ASSOCIATED_DATA, plaintext)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().tag().size(), std::string::size_type{24});
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_384, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_cbc_hmac_round_trips_a256cbc_hs512) {
  const std::string_view plaintext{"a secret session cookie payload"};
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_512, IV, ASSOCIATED_DATA, plaintext)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().tag().size(), std::string::size_type{32});
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_512, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_cbc_hmac_pads_the_ciphertext_to_a_block_boundary) {
  // The plaintext is 31 bytes, which pads up to two 16-byte blocks
  const std::string_view plaintext{"a secret session cookie payload"};
  const auto result{
      sourcemeta::core::aes_cbc_hmac_encrypt(KEY_256, IV, "", plaintext)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().ciphertext().size(), std::string::size_type{32});
}

TEST(aes_cbc_hmac_round_trips_across_block_boundaries) {
  const std::string plaintext(100, 'z');
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, plaintext)};
  EXPECT_TRUE(result.has_value());
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_256, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_cbc_hmac_round_trips_an_empty_plaintext) {
  const auto result{
      sourcemeta::core::aes_cbc_hmac_encrypt(KEY_256, IV, ASSOCIATED_DATA, "")};
  EXPECT_TRUE(result.has_value());
  // An empty plaintext still pads up to a full block
  EXPECT_EQ(result.value().ciphertext().size(), std::string::size_type{16});
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_256, IV, ASSOCIATED_DATA, result.value().ciphertext(),
      result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), "");
}

TEST(aes_cbc_hmac_round_trips_empty_associated_data) {
  const std::string_view plaintext{"payload without associated data"};
  const auto result{
      sourcemeta::core::aes_cbc_hmac_encrypt(KEY_256, IV, "", plaintext)};
  EXPECT_TRUE(result.has_value());
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_256, IV, "", result.value().ciphertext(), result.value().tag())};
  EXPECT_TRUE(opened.has_value());
  EXPECT_EQ(opened.value(), plaintext);
}

TEST(aes_cbc_hmac_encrypt_is_deterministic_for_the_same_inputs) {
  const auto first{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, "identical")};
  const auto second{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, "identical")};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  EXPECT_EQ(first.value().ciphertext(), second.value().ciphertext());
  EXPECT_EQ(first.value().tag(), second.value().tag());
}

TEST(aes_cbc_hmac_decrypt_rejects_different_associated_data) {
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, "hello")};
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_256, IV, "different-header", result.value().ciphertext(),
      result.value().tag())};
  EXPECT_FALSE(opened.has_value());
}

TEST(aes_cbc_hmac_decrypt_rejects_a_tampered_tag) {
  auto result{sourcemeta::core::aes_cbc_hmac_encrypt(KEY_256, IV,
                                                     ASSOCIATED_DATA, "hello")
                  .value()};
  // The tag is the final bytes of the buffer
  result.data.back() = static_cast<char>(result.data.back() ^ 0x01);
  EXPECT_FALSE(
      sourcemeta::core::aes_cbc_hmac_decrypt(KEY_256, IV, ASSOCIATED_DATA,
                                             result.ciphertext(), result.tag())
          .has_value());
}

TEST(aes_cbc_hmac_decrypt_rejects_a_tampered_ciphertext) {
  auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
                  KEY_256, IV, ASSOCIATED_DATA, "a longer payload here")
                  .value()};
  // The first byte of the buffer is the first ciphertext byte
  result.data[0] = static_cast<char>(result.data[0] ^ 0x01);
  EXPECT_FALSE(
      sourcemeta::core::aes_cbc_hmac_decrypt(KEY_256, IV, ASSOCIATED_DATA,
                                             result.ciphertext(), result.tag())
          .has_value());
}

TEST(aes_cbc_hmac_decrypt_rejects_a_different_key) {
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, "hello")};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_512, IV, ASSOCIATED_DATA, result.value().ciphertext(),
                   result.value().tag())
                   .has_value());
}

TEST(aes_cbc_hmac_encrypt_rejects_a_wrong_size_key) {
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_encrypt(std::string(40, '\x00'),
                                                      IV, "", "x")
                   .has_value());
}

TEST(aes_cbc_hmac_encrypt_rejects_a_wrong_size_iv) {
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_encrypt(
                   KEY_256, std::string(12, '\x00'), "", "x")
                   .has_value());
}

TEST(aes_cbc_hmac_decrypt_rejects_a_wrong_size_tag) {
  const auto result{
      sourcemeta::core::aes_cbc_hmac_encrypt(KEY_256, IV, "", "hello")};
  EXPECT_FALSE(
      sourcemeta::core::aes_cbc_hmac_decrypt(
          KEY_256, IV, "", result.value().ciphertext(), std::string(15, '\x00'))
          .has_value());
}

TEST(aes_cbc_hmac_decrypt_rejects_a_non_block_ciphertext) {
  const auto result{
      sourcemeta::core::aes_cbc_hmac_encrypt(KEY_256, IV, "", "hello")};
  const std::string truncated{result.value().ciphertext().substr(1)};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_256, IV, "", truncated, result.value().tag())
                   .has_value());
}
