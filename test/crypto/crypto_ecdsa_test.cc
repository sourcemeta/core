#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/text.h>

#include <string> // std::string

// Test keys and signatures over the message below, one per supported curve,
// generated with OpenSSL using the curve's matching hash function
static const std::string MESSAGE{"sourcemeta core JWS signing input"};

static const std::string P256_QX{
    "670f3fbe0718a819c012bbf3fe3f60cabb941546f9691e044289ed3fd2f6d58a"};
static const std::string P256_QY{
    "9f8fe89b6e79f099906ec1711876e95d88bf1d53ef4f19319a497b2d7a97fc91"};
static const std::string P256_SIG{
    "c6982861d40a279c8fdda0f0ca904b3e46502fb9a397f68f1c172055685ebaaa"
    "a86f9d6252831a1ac7003c3d5dc35843596667951a57113935de42159e2c8740"};

static const std::string P384_QX{
    "ae8aaf3ffbf5fa91be07ade1b3b6730c62189d757436e004de067f545cfe4185"
    "e93388e7a719a2b90afdd110936583d8"};
static const std::string P384_QY{
    "530f85af20faa09c68ccc6c0d5905ba08a56db76f833dc4d0191e84c1797abdc"
    "08170a18efca85ddb0d3a1341f3251ed"};
static const std::string P384_SIG{
    "299af5ab271c78b3c49aafb0303cf2502665752501f14ca9b645fd8580d3a7ca"
    "34cd4a35e213b6d19c0840140229450ed44317cd43218f12da2fcb20a294a162"
    "2f99bb00729180abf5677c74d38ef109bdf8db849806b8517e67c59517e2ad51"};

static const std::string P521_QX{
    "01a5c29e777348a0a53ae56ec0ab917caf0a8e7709b7769bc8c969950a3a4da0"
    "2cbd85d8d63fad67542426e1ba04775b3d9e0dec2cdaf7e76bafafa522bc4bd0"
    "9e65"};
static const std::string P521_QY{
    "0192bcdaf4cfebaa8e6e46753e31ddfb71607593a4a4307f773679ff31c6f4e6"
    "b362da98b3b214a5f395b647f5880e752d66038f956cdf5e0fec6e4c5618bcda"
    "5582"};
static const std::string P521_SIG{
    "0049a787d04b2174abb7639ee4649c13b95ba11267e6a4fd93c0f96159acf7ce"
    "2a9deac7f1b4a54984954aecdab9976f16d63fb872f0838cba198cd6feb2c26b"
    "b43500073f02202696ef9c8f9670737fe829beac30eed4b6f94990ed07288545"
    "03f3165be805564f612a5e228207b3007258023bbfe33014bbe759aaef96f28c"
    "56018df4"};

TEST(Crypto_ecdsa, verify_p256_sha256_valid_signature) {
  EXPECT_TRUE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(P256_SIG).value()));
}

TEST(Crypto_ecdsa, verify_p384_sha384_valid_signature) {
  EXPECT_TRUE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P384,
      sourcemeta::core::SignatureHashFunction::SHA384,
      sourcemeta::core::hex_to_bytes(P384_QX).value(),
      sourcemeta::core::hex_to_bytes(P384_QY).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(P384_SIG).value()));
}

TEST(Crypto_ecdsa, verify_p521_sha512_valid_signature) {
  EXPECT_TRUE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P521,
      sourcemeta::core::SignatureHashFunction::SHA512,
      sourcemeta::core::hex_to_bytes(P521_QX).value(),
      sourcemeta::core::hex_to_bytes(P521_QY).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(P521_SIG).value()));
}

TEST(Crypto_ecdsa, verify_rejects_different_message) {
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), "a different message",
      sourcemeta::core::hex_to_bytes(P256_SIG).value()));
}

TEST(Crypto_ecdsa, verify_rejects_tampered_r) {
  auto signature{sourcemeta::core::hex_to_bytes(P256_SIG).value()};
  signature[0] = static_cast<char>(signature[0] ^ 0x01);
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE, signature));
}

TEST(Crypto_ecdsa, verify_rejects_tampered_s) {
  auto signature{sourcemeta::core::hex_to_bytes(P256_SIG).value()};
  signature.back() = static_cast<char>(signature.back() ^ 0x01);
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE, signature));
}

TEST(Crypto_ecdsa, verify_rejects_curve_mismatch) {
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P384,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(P256_SIG).value()));
}

TEST(Crypto_ecdsa, verify_rejects_hash_function_mismatch) {
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA384,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(P256_SIG).value()));
}

TEST(Crypto_ecdsa, verify_rejects_truncated_signature) {
  auto signature{sourcemeta::core::hex_to_bytes(P256_SIG).value()};
  signature.pop_back();
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE, signature));
}

TEST(Crypto_ecdsa, verify_rejects_zero_signature) {
  const std::string signature(64, '\x00');
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE, signature));
}

TEST(Crypto_ecdsa, verify_rejects_point_not_on_curve) {
  auto coordinate_y{sourcemeta::core::hex_to_bytes(P256_QY).value()};
  coordinate_y.back() = static_cast<char>(coordinate_y.back() ^ 0x01);
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(), coordinate_y, MESSAGE,
      sourcemeta::core::hex_to_bytes(P256_SIG).value()));
}

TEST(Crypto_ecdsa, verify_rejects_empty_signature) {
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(P256_QX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE, ""));
}

TEST(Crypto_ecdsa, verify_rejects_empty_coordinate) {
  EXPECT_FALSE(sourcemeta::core::ecdsa_verify(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::SignatureHashFunction::SHA256, "",
      sourcemeta::core::hex_to_bytes(P256_QY).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(P256_SIG).value()));
}
