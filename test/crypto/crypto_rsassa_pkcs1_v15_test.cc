#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/text.h>

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

// A 2048-bit test key with the usual public exponent, along with
// signatures over the message below, one per supported hash function
static const std::string MESSAGE{"sourcemeta core JWS signing input"};

static const std::string MODULUS_HEX{
    "c18bb08bad1c930bb6eed38f60299c28f28e0dabf53557fa8ac9a63044413a3c"
    "74671a8b316ec972b25faa13826b10e0d78f90da1b697805202faf58c5768361"
    "ad0006364e5f5bbf6e95a222f894be4cdccfe52aca34b48a1f92a1192214f78e"
    "33cfb063b04a83b4234ef91248ba16c8a35655551cf4dfdb44bcc2e77e415d94"
    "097be4d911dac040098e4a76662d854c9218a8869be7f00bdac03e3b1e5aa9ec"
    "0d8d5c683ccbdbcf34eb04d7d0ba000b4692287d1a875927bd55325f1461d738"
    "4fa1506e0d5d5d9c7f531112485219c783228ae9b81ff65725513d559131ee06"
    "c5299d6531d0b9abe13ee19924f1fa47f25136fc948daf0cac721a277255af3d"};

static const std::string EXPONENT_HEX{"010001"};

static const std::string SIGNATURE_SHA256_HEX{
    "b03580590d438d7a0cec9dbaf9046414c8474ec5bfa0809ce456036110730d72"
    "1ce914ccda9da01d80d4f21490a1e9263391f434ede0c901d4bfcf983b8ad1c1"
    "cc97d879a0315b0b69ee13c5a2fd2ddfce4d407d65351a902f90630eb398a2d6"
    "7cc0232ec78365fbdffe540324fdafac86e6a5e371e4cc71b71c0d3758b2e6fa"
    "9648089d46217e51df97f1e73f2cbc475477b5406d12ecf3b64957b165821295"
    "3c69777330d9909d2a4a62f95672e0ff45b1b2691250f52841182b11869c88b0"
    "7080b42118a2380d03d58f488a4241f1624cbda93d9f50b95fbd0f2ff7e35b54"
    "f0c410dd41988958e0400e20442efdd2f8a7c963b9e874f50bf489b553269ba4"};

static const std::string SIGNATURE_SHA384_HEX{
    "6ef5d288aaead69bf4743721be8ade1b3d1e49070655159cba6d0c856a9c189f"
    "70d0862d18904b8ed991a8e6c29fc93493b59123da80ef8d20434c08fea261d6"
    "fa403cee6f322fc1d0dd23398f11df9e39456f08a2576b7b5b7051927769593f"
    "8a4712c07e70b26fe1690b511677cf3cab3fcb47cf9a36fa25fc57a1140f9942"
    "490e5d2667afb8e17542256bfd54c4cbc7072f55b631840ee50c1643ca3db06a"
    "04aa5f2402fc9c27b7417ed66c463fc86b57323fc731e439e584a8486406b2da"
    "c847a79311730949ae988a0665f009f17f6cd417f4885cdf7739a8f5f8d481ea"
    "5185cab358b6bd0605fcb868ec912e11f20e679d01310379ae0fbd71c16d94bf"};

static const std::string SIGNATURE_SHA512_HEX{
    "2b4e81f5f54a1e152b2369679c124869d0c9b8548bd561ad0f8db31c037126d3"
    "b122cca0202891ec827a14ecbbe3fa76dfb2aec9884eb45a54888f9f39614b73"
    "ca55cd270fa538a81a1c841757f390c8d04973e8eea09d35b6b95172c3a10545"
    "327468909d7b5665506821ce432baed7814c8ddd8d7bb1296c09708afb760efa"
    "36692287bf79d3d441dab17cd316954a8e60fa96021c04768b7e69083e79cf63"
    "fbeaee9b16cdae3d0fce1cca832acd3c5b2540a2a4cb9084154c47f0c4124f13"
    "0a3d8f7bf7cd5fc1e129b1008c9a2e9383c1b7057d436309ff0aff64242e80f5"
    "1aa161d0bef0b7db16332277d2db938886e94c1d81fdb71bcbef35a3514ca6e8"};

namespace {
auto verify_pkcs1(const sourcemeta::core::SignatureHashFunction hash,
                  const std::string_view modulus,
                  const std::string_view exponent,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_rsa_public_key(modulus, exponent)};
  return key.has_value() && sourcemeta::core::rsassa_pkcs1_v15_verify(
                                key.value(), hash, message, signature);
}
} // namespace

TEST(Crypto_rsassa_pkcs1_v15, verify_sha256_valid_signature) {
  EXPECT_TRUE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_sha384_valid_signature) {
  EXPECT_TRUE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA384,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA384_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_sha512_valid_signature) {
  EXPECT_TRUE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA512,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA512_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_exponent_with_leading_zeros) {
  EXPECT_TRUE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes("00000000" + EXPONENT_HEX).value(),
      MESSAGE, sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_different_message) {
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
      "a different message",
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_tampered_signature) {
  auto signature{sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()};
  signature[0] = static_cast<char>(signature[0] ^ 0x01);
  EXPECT_FALSE(
      verify_pkcs1(sourcemeta::core::SignatureHashFunction::SHA256,
                   sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                   sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                   MESSAGE, signature));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_hash_function_mismatch) {
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA384,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_truncated_signature) {
  auto signature{sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()};
  signature.pop_back();
  EXPECT_FALSE(
      verify_pkcs1(sourcemeta::core::SignatureHashFunction::SHA256,
                   sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                   sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                   MESSAGE, signature));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_extended_signature) {
  auto signature{sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()};
  signature.push_back('\x00');
  EXPECT_FALSE(
      verify_pkcs1(sourcemeta::core::SignatureHashFunction::SHA256,
                   sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                   sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                   MESSAGE, signature));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_signature_equal_to_modulus) {
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_wrong_exponent) {
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes("03").value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_empty_signature) {
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE, ""));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_empty_modulus) {
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256, "",
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_empty_exponent) {
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(), "", MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_empty_message) {
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), "",
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_modulus_with_leading_zeros) {
  EXPECT_TRUE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes("0000" + MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_oversized_modulus) {
  const std::string modulus(520, '\xFF');
  const std::string signature(520, '\x01');
  EXPECT_FALSE(
      verify_pkcs1(sourcemeta::core::SignatureHashFunction::SHA256, modulus,
                   sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                   MESSAGE, signature));
}

TEST(Crypto_rsassa_pkcs1_v15, verify_rejects_oversized_exponent) {
  const std::string exponent(520, '\xFF');
  EXPECT_FALSE(verify_pkcs1(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(), exponent, MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}
