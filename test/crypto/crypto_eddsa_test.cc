#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <optional>    // std::optional
#include <string_view> // std::string_view

#include <cstddef> // std::size_t
#include <string>  // std::string

// Public keys, messages, and signatures from the Ed25519 test vectors of RFC
// 8032 Section 7.1, the authoritative source

static const std::string TEST1_PUBLIC_KEY{
    "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a"};
static const std::string TEST1_MESSAGE{""};
static const std::string TEST1_SIGNATURE{
    "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555f"
    "b8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b"};

static const std::string TEST2_PUBLIC_KEY{
    "3d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c"};
static const std::string TEST2_MESSAGE{"72"};
static const std::string TEST2_SIGNATURE{
    "92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da08"
    "5ac1e43e15996e458f3613d0f11d8c387b2eaeb4302aeeb00d291612bb0c00"};

static const std::string TEST3_PUBLIC_KEY{
    "fc51cd8e6218a1a38da47ed00230f0580816ed13ba3303ac5deb911548908025"};
static const std::string TEST3_MESSAGE{"af82"};
static const std::string TEST3_SIGNATURE{
    "6291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18"
    "ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a"};

// The message of this vector is itself the SHA-512 digest of "abc"
static const std::string TEST_ABC_PUBLIC_KEY{
    "ec172b93ad5e563bf4932c70e1245034c35467ef2efd4d64ebf819683467e2bf"};
static const std::string TEST_ABC_MESSAGE{
    "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a21"
    "92992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f"};
static const std::string TEST_ABC_SIGNATURE{
    "dc2a4459e7369633a52b1bf277839a00201009a3efbf3ecb69bea2186c26b58909"
    "351fc9ac90b3ecfdfbc7c66431e0303dca179c138ac17ad9bef1177331a704"};

namespace {
auto verify_eddsa(const sourcemeta::core::EdwardsCurve curve,
                  const std::string_view public_key,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_eddsa_public_key(curve, public_key)};
  return key.has_value() &&
         sourcemeta::core::eddsa_verify(key.value(), message, signature);
}
} // namespace

TEST(verify_ed25519_empty_message) {
  EXPECT_TRUE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST1_PUBLIC_KEY).value(),
                   sourcemeta::core::hex_to_bytes(TEST1_MESSAGE).value(),
                   sourcemeta::core::hex_to_bytes(TEST1_SIGNATURE).value()));
}

TEST(verify_ed25519_one_byte_message) {
  EXPECT_TRUE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST2_PUBLIC_KEY).value(),
                   sourcemeta::core::hex_to_bytes(TEST2_MESSAGE).value(),
                   sourcemeta::core::hex_to_bytes(TEST2_SIGNATURE).value()));
}

TEST(verify_ed25519_two_byte_message) {
  EXPECT_TRUE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST3_PUBLIC_KEY).value(),
                   sourcemeta::core::hex_to_bytes(TEST3_MESSAGE).value(),
                   sourcemeta::core::hex_to_bytes(TEST3_SIGNATURE).value()));
}

TEST(verify_ed25519_sha512_abc_message) {
  EXPECT_TRUE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST_ABC_PUBLIC_KEY).value(),
                   sourcemeta::core::hex_to_bytes(TEST_ABC_MESSAGE).value(),
                   sourcemeta::core::hex_to_bytes(TEST_ABC_SIGNATURE).value()));
}

TEST(verify_rejects_different_message) {
  EXPECT_FALSE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST3_PUBLIC_KEY).value(),
                   sourcemeta::core::hex_to_bytes("af83").value(),
                   sourcemeta::core::hex_to_bytes(TEST3_SIGNATURE).value()));
}

TEST(verify_rejects_tampered_point) {
  auto signature{sourcemeta::core::hex_to_bytes(TEST3_SIGNATURE).value()};
  signature[0] = static_cast<char>(signature[0] ^ 0x01);
  EXPECT_FALSE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed25519,
      sourcemeta::core::hex_to_bytes(TEST3_PUBLIC_KEY).value(),
      sourcemeta::core::hex_to_bytes(TEST3_MESSAGE).value(), signature));
}

TEST(verify_rejects_tampered_scalar) {
  auto signature{sourcemeta::core::hex_to_bytes(TEST3_SIGNATURE).value()};
  signature.back() = static_cast<char>(signature.back() ^ 0x01);
  EXPECT_FALSE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed25519,
      sourcemeta::core::hex_to_bytes(TEST3_PUBLIC_KEY).value(),
      sourcemeta::core::hex_to_bytes(TEST3_MESSAGE).value(), signature));
}

TEST(verify_rejects_wrong_public_key) {
  EXPECT_FALSE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST2_PUBLIC_KEY).value(),
                   sourcemeta::core::hex_to_bytes(TEST3_MESSAGE).value(),
                   sourcemeta::core::hex_to_bytes(TEST3_SIGNATURE).value()));
}

TEST(verify_rejects_truncated_signature) {
  auto signature{sourcemeta::core::hex_to_bytes(TEST1_SIGNATURE).value()};
  signature.pop_back();
  EXPECT_FALSE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST1_PUBLIC_KEY).value(),
                   TEST1_MESSAGE, signature));
}

TEST(verify_rejects_empty_signature) {
  EXPECT_FALSE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST1_PUBLIC_KEY).value(),
                   TEST1_MESSAGE, ""));
}

TEST(verify_rejects_zero_signature) {
  const std::string signature(64, '\x00');
  EXPECT_FALSE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519,
                   sourcemeta::core::hex_to_bytes(TEST1_PUBLIC_KEY).value(),
                   TEST1_MESSAGE, signature));
}

TEST(verify_rejects_scalar_above_order) {
  // The high half of the signature is the scalar S, which the standard
  // requires to lie below the group order (RFC 8032 Section 5.1.7)
  auto signature{sourcemeta::core::hex_to_bytes(TEST3_SIGNATURE).value()};
  for (std::size_t index = 32; index < signature.size(); ++index) {
    signature[index] = static_cast<char>(0xff);
  }

  EXPECT_FALSE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed25519,
      sourcemeta::core::hex_to_bytes(TEST3_PUBLIC_KEY).value(),
      sourcemeta::core::hex_to_bytes(TEST3_MESSAGE).value(), signature));
}

TEST(verify_rejects_wrong_length_public_key) {
  auto public_key{sourcemeta::core::hex_to_bytes(TEST1_PUBLIC_KEY).value()};
  public_key.pop_back();
  EXPECT_FALSE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed25519, public_key, TEST1_MESSAGE,
      sourcemeta::core::hex_to_bytes(TEST1_SIGNATURE).value()));
}

TEST(verify_rejects_empty_public_key) {
  EXPECT_FALSE(
      verify_eddsa(sourcemeta::core::EdwardsCurve::Ed25519, "", TEST1_MESSAGE,
                   sourcemeta::core::hex_to_bytes(TEST1_SIGNATURE).value()));
}

// Public keys, messages, and signatures from the Ed448 test vectors of RFC
// 8032 Section 7.4, the authoritative source, restricted to the empty-context
// cases that JWS uses

// clang-format off
static const std::string ED448_BLANK_PUBLIC_KEY{"5fd7449b59b461fd2ce787ec616ad46a1da1342485a70e1f8a0ea75d80e96778edf124769b46c7061bd6783df1e50f6cd1fa1abeafe8256180"};
static const std::string ED448_BLANK_MESSAGE{""};
static const std::string ED448_BLANK_SIGNATURE{"533a37f6bbe457251f023c0d88f976ae2dfb504a843e34d2074fd823d41a591f2b233f034f628281f2fd7a22ddd47d7828c59bd0a21bfd3980ff0d2028d4b18a9df63e006c5d1c2d345b925d8dc00b4104852db99ac5c7cdda8530a113a0f4dbb61149f05a7363268c71d95808ff2e652600"};

static const std::string ED448_ONE_BYTE_PUBLIC_KEY{"43ba28f430cdff456ae531545f7ecd0ac834a55d9358c0372bfa0c6c6798c0866aea01eb00742802b8438ea4cb82169c235160627b4c3a9480"};
static const std::string ED448_ONE_BYTE_MESSAGE{"03"};
static const std::string ED448_ONE_BYTE_SIGNATURE{"26b8f91727bd62897af15e41eb43c377efb9c610d48f2335cb0bd0087810f4352541b143c4b981b7e18f62de8ccdf633fc1bf037ab7cd779805e0dbcc0aae1cbcee1afb2e027df36bc04dcecbf154336c19f0af7e0a6472905e799f1953d2a0ff3348ab21aa4adafd1d234441cf807c03a00"};

static const std::string ED448_ELEVEN_BYTE_PUBLIC_KEY{"dcea9e78f35a1bf3499a831b10b86c90aac01cd84b67a0109b55a36e9328b1e365fce161d71ce7131a543ea4cb5f7e9f1d8b00696447001400"};
static const std::string ED448_ELEVEN_BYTE_MESSAGE{"0c3e544074ec63b0265e0c"};
static const std::string ED448_ELEVEN_BYTE_SIGNATURE{"1f0a8888ce25e8d458a21130879b840a9089d999aaba039eaf3e3afa090a09d389dba82c4ff2ae8ac5cdfb7c55e94d5d961a29fe0109941e00b8dbdeea6d3b051068df7254c0cdc129cbe62db2dc957dbb47b51fd3f213fb8698f064774250a5028961c9bf8ffd973fe5d5c206492b140e00"};
// clang-format on

TEST(verify_ed448_empty_message) {
  EXPECT_TRUE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed448,
      sourcemeta::core::hex_to_bytes(ED448_BLANK_PUBLIC_KEY).value(),
      sourcemeta::core::hex_to_bytes(ED448_BLANK_MESSAGE).value(),
      sourcemeta::core::hex_to_bytes(ED448_BLANK_SIGNATURE).value()));
}

TEST(verify_ed448_one_byte_message) {
  EXPECT_TRUE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed448,
      sourcemeta::core::hex_to_bytes(ED448_ONE_BYTE_PUBLIC_KEY).value(),
      sourcemeta::core::hex_to_bytes(ED448_ONE_BYTE_MESSAGE).value(),
      sourcemeta::core::hex_to_bytes(ED448_ONE_BYTE_SIGNATURE).value()));
}

TEST(verify_ed448_eleven_byte_message) {
  EXPECT_TRUE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed448,
      sourcemeta::core::hex_to_bytes(ED448_ELEVEN_BYTE_PUBLIC_KEY).value(),
      sourcemeta::core::hex_to_bytes(ED448_ELEVEN_BYTE_MESSAGE).value(),
      sourcemeta::core::hex_to_bytes(ED448_ELEVEN_BYTE_SIGNATURE).value()));
}

TEST(verify_ed448_rejects_different_message) {
  EXPECT_FALSE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed448,
      sourcemeta::core::hex_to_bytes(ED448_ELEVEN_BYTE_PUBLIC_KEY).value(),
      sourcemeta::core::hex_to_bytes("0c3e544074ec63b0265e0d").value(),
      sourcemeta::core::hex_to_bytes(ED448_ELEVEN_BYTE_SIGNATURE).value()));
}

TEST(verify_ed448_rejects_tampered_scalar) {
  auto signature{
      sourcemeta::core::hex_to_bytes(ED448_ONE_BYTE_SIGNATURE).value()};
  signature.back() = static_cast<char>(signature.back() ^ 0x01);
  EXPECT_FALSE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed448,
      sourcemeta::core::hex_to_bytes(ED448_ONE_BYTE_PUBLIC_KEY).value(),
      sourcemeta::core::hex_to_bytes(ED448_ONE_BYTE_MESSAGE).value(),
      signature));
}

TEST(verify_ed448_rejects_wrong_length_signature) {
  // A 64-byte signature is valid for Ed25519 but not for Ed448
  EXPECT_FALSE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed448,
      sourcemeta::core::hex_to_bytes(ED448_BLANK_PUBLIC_KEY).value(),
      ED448_BLANK_MESSAGE,
      sourcemeta::core::hex_to_bytes(TEST1_SIGNATURE).value()));
}

TEST(verify_ed448_rejects_ed25519_inputs) {
  // Ed25519-sized key and signature must not verify under Ed448
  EXPECT_FALSE(verify_eddsa(
      sourcemeta::core::EdwardsCurve::Ed448,
      sourcemeta::core::hex_to_bytes(TEST1_PUBLIC_KEY).value(), TEST1_MESSAGE,
      sourcemeta::core::hex_to_bytes(TEST1_SIGNATURE).value()));
}
