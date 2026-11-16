#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// A 256-bit key selects A128CBC-HS256 (16-byte tag)
// The key material is binary, so it can only come from decoding at runtime
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
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
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

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

TEST(aes_cbc_hmac_decrypt_rejects_a_wrong_size_key) {
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, "hello")};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   std::string(20, '\x00'), IV, ASSOCIATED_DATA,
                   result.value().ciphertext(), result.value().tag())
                   .has_value());
}

TEST(aes_cbc_hmac_decrypt_rejects_a_wrong_size_iv) {
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, "hello")};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_256, std::string(8, '\x00'), ASSOCIATED_DATA,
                   result.value().ciphertext(), result.value().tag())
                   .has_value());
}

TEST(aes_cbc_hmac_decrypt_rejects_an_empty_ciphertext) {
  const auto result{sourcemeta::core::aes_cbc_hmac_encrypt(
      KEY_256, IV, ASSOCIATED_DATA, "hello")};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_256, IV, ASSOCIATED_DATA, "", result.value().tag())
                   .has_value());
}

// These vectors were generated independently with Python cryptography (AES-CBC
// without automatic padding) and hashlib/hmac. RFC 7518 Section 5.2.2.1
// authenticates AAD || IV || ciphertext || uint64_be(AAD bits), using the first
// half of the key and truncating to that half's length. AES uses the second
// half. IV and AAD are the constants above.
//
// The invalid decrypted inputs are 15 'A' bytes followed by 00, 15 'A' bytes
// followed by seventeen 11 (hex) bytes, and 13 'A' bytes followed by 03 02 03.
// The oversized case has two blocks, so its claimed padding fits the input
// but exceeds the AES block size. The inconsistent case disagrees at the second
// of its three claimed padding bytes. RFC 5652 Section 6.3 requires each
// padding byte to equal the padding length, between 1 and 16 here. The valid
// controls contain sixteen 10 (hex) bytes and decode to an empty plaintext.
// Valid tags are essential: unauthenticated tampering stops before padding.

TEST(aes_cbc_hmac_rejects_authenticated_zero_padding_a128cbc_hs256) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("e94ec8dd0636f4f080f8637884f55965")
          .value()};
  const auto tag{
      sourcemeta::core::hex_to_bytes("12b1b79458d58e0f6223d08a5995f1b9")
          .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_256, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_rejects_authenticated_oversized_padding_a128cbc_hs256) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes(
          "e730e508b5f4026f8b71f3c019a53366dc44d677f4c21ce1b7e9cf37471859e2")
          .value()};
  const auto tag{
      sourcemeta::core::hex_to_bytes("c859990d14225c0f4a98d1df0b733352")
          .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_256, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_rejects_authenticated_inconsistent_padding_a128cbc_hs256) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("99f3d8b001bf2327fde9064fe4d2de6b")
          .value()};
  const auto tag{
      sourcemeta::core::hex_to_bytes("4f943dc08046036d48669c9ae7160ad9")
          .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_256, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_accepts_authenticated_full_block_padding_a128cbc_hs256) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("9c54d571702cfa0f03f36215676bab78")
          .value()};
  const auto tag{
      sourcemeta::core::hex_to_bytes("c7cabdbae54d780c171afeb616e7398a")
          .value()};
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_256, IV, ASSOCIATED_DATA, ciphertext, tag)};
  EXPECT_TRUE(opened.has_value());
  EXPECT_TRUE(opened.value().empty());
}

TEST(aes_cbc_hmac_rejects_authenticated_zero_padding_a192cbc_hs384) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("1e7d0539bcb1c279a92efe2b9e113c49")
          .value()};
  const auto tag{sourcemeta::core::hex_to_bytes(
                     "908ae4162e8582c2764b6a865baf4b59ebb48d715c60f1a1")
                     .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_384, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_rejects_authenticated_oversized_padding_a192cbc_hs384) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes(
          "b80c41c1398ca0c76ae91a0897f13e8573b8c4a157614fd012dc57416a87d36f")
          .value()};
  const auto tag{sourcemeta::core::hex_to_bytes(
                     "69caa33064fe6720826fd0118412feae94961f3b3015b419")
                     .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_384, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_rejects_authenticated_inconsistent_padding_a192cbc_hs384) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("ff624af6ef36d999a74cef2e804d6fe8")
          .value()};
  const auto tag{sourcemeta::core::hex_to_bytes(
                     "64836f25a4113342de91e1fd4e5f86e956322c4447e1d09b")
                     .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_384, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_accepts_authenticated_full_block_padding_a192cbc_hs384) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("38fd20e8100f2ed8b8e95015efdedfb1")
          .value()};
  const auto tag{sourcemeta::core::hex_to_bytes(
                     "950617c9e3d5c0f900f7fa99501b0438af4cbdf7e94dbbdd")
                     .value()};
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_384, IV, ASSOCIATED_DATA, ciphertext, tag)};
  EXPECT_TRUE(opened.has_value());
  EXPECT_TRUE(opened.value().empty());
}

TEST(aes_cbc_hmac_rejects_authenticated_zero_padding_a256cbc_hs512) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("8ecd061ea6d151194e370b9452200eba")
          .value()};
  const auto tag{
      sourcemeta::core::hex_to_bytes(
          "1b07a240ae21af32a3bc5634ae3c618bbca7ce06bab87fa42094edd6b8618cbb")
          .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_512, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_rejects_authenticated_oversized_padding_a256cbc_hs512) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes(
          "3640b885e9e5888ac7bf7f1af42c1d4a33447c1bf811c35f5fb0b3c26f90b674")
          .value()};
  const auto tag{
      sourcemeta::core::hex_to_bytes(
          "880bc74b195252a1053fe50fd45fe1a32bfb1fc6b410ac90261091c94334b1c2")
          .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_512, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_rejects_authenticated_inconsistent_padding_a256cbc_hs512) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("1cab42f18566506fd5591f3b9289d559")
          .value()};
  const auto tag{
      sourcemeta::core::hex_to_bytes(
          "e0054d5f68ce190a27b8542a573daa28af75655618f18b870e5ca6fdd4b83a0b")
          .value()};
  EXPECT_FALSE(sourcemeta::core::aes_cbc_hmac_decrypt(
                   KEY_512, IV, ASSOCIATED_DATA, ciphertext, tag)
                   .has_value());
}

TEST(aes_cbc_hmac_accepts_authenticated_full_block_padding_a256cbc_hs512) {
  const auto ciphertext{
      sourcemeta::core::hex_to_bytes("a00f1ede67c6f803526f2e3c4ea929bc")
          .value()};
  const auto tag{
      sourcemeta::core::hex_to_bytes(
          "99b8e26bce0c261b2eed5f07a0dd3083d9617d745826c3e08ced87fad69cfe22")
          .value()};
  const auto opened{sourcemeta::core::aes_cbc_hmac_decrypt(
      KEY_512, IV, ASSOCIATED_DATA, ciphertext, tag)};
  EXPECT_TRUE(opened.has_value());
  EXPECT_TRUE(opened.value().empty());
}
