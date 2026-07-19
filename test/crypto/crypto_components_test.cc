#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string> // std::string

// RFC 7638 Section 3.1: the RSA public key whose thumbprint the RFC publishes
static const std::string RFC7638_RSA_N{
    "0vx7agoebGcQSuuPiLJXZptN9nndrQmbXEps2aiAFbWhM78LhWx4cbbfAAtVT86zwu1RK"
    "7aPFFxuhDR1L6tSoc_BJECPebWKRXjBZCiFV4n3oknjhMstn64tZ_2W-5JsGY4Hc5n9yB"
    "XArwl93lqt7_RN5w6Cf0h4QyQ5v-65YGjQR0_FDW2QvzqY368QQMicAtaSqzs8KJZgnYb"
    "9c7d0zgdAZHzu6qMQvRL5hajrn1n91CbOpbISD08qNLyrdkt-bFTWhAI4vMQFh6WeZu0f"
    "M4lFd2NcRwr3XPksINHaQ-G_xBniIqbw0Ls1jF44-csFCur-kEgU8awapJzKnqDKgw"};

// RFC 6979 Appendix A.2.5: the NIST P-256 example key and its public point
static const std::string P256_D_HEX{
    "c9afa9d845ba75166b5c215767b1d6934e50c3db36e89b127b8a622b120f6721"};
static const std::string P256_QX_HEX{
    "60fed4ba255a9d31c961eb74c6356d68c049b8923b61fa6ce669622e60f29fb6"};
static const std::string P256_QY_HEX{
    "7903fe1008b8bc99a41ae9e95628bc64f2f1b20c2d7e9f5177a3c294d4462299"};

// The Ed25519 and Ed448 seeds and public keys from the signing test vectors
static const std::string ED25519_SEED_HEX{
    "741ec35ce5e07089ca782f88de00d8103a1cd8e8b116acd8afd922c01bee7867"};
static const std::string ED25519_PUBLIC_HEX{
    "48539e4fb623b0d221f8cd7e9c20469c1f55598bb5bb236a8a2cc2661c1d743a"};
static const std::string ED448_SEED_HEX{
    "7027028140c9eb422935e1d309e01e66670389173da4f66c4016c2492513b4c1"
    "989263ea13250e65a784aa8c01a9960a7416c2e2879b2a2295"};
static const std::string ED448_PUBLIC_HEX{
    "04a74611a3714c06363868b8801b3c81d1b373ac7621eef20cf2793518450e60"
    "7e455b12f21c98fc5ad5098e15604e4e5236a8a455e1280f00"};

TEST(rsa_public_components_round_trips) {
  const auto modulus{sourcemeta::core::base64url_decode(RFC7638_RSA_N)};
  const auto exponent{sourcemeta::core::base64url_decode("AQAB")};
  const auto key{
      sourcemeta::core::make_rsa_public_key(modulus.value(), exponent.value())};
  EXPECT_TRUE(key.has_value());
  const auto components{sourcemeta::core::rsa_public_components(key.value())};
  EXPECT_TRUE(components.has_value());
  EXPECT_EQ(sourcemeta::core::base64url_encode(components.value().modulus),
            RFC7638_RSA_N);
  EXPECT_EQ(sourcemeta::core::base64url_encode(components.value().exponent),
            "AQAB");
}

TEST(ec_public_components_round_trips_with_curve) {
  const auto coordinate_x{sourcemeta::core::hex_to_bytes(P256_QX_HEX)};
  const auto coordinate_y{sourcemeta::core::hex_to_bytes(P256_QY_HEX)};
  const auto key{sourcemeta::core::make_ec_public_key(
      sourcemeta::core::EllipticCurve::P256, coordinate_x.value(),
      coordinate_y.value())};
  EXPECT_TRUE(key.has_value());
  const auto components{sourcemeta::core::ec_public_components(key.value())};
  EXPECT_TRUE(components.has_value());
  EXPECT_TRUE(components.value().curve ==
              sourcemeta::core::EllipticCurve::P256);
  EXPECT_EQ(components.value().x, coordinate_x.value());
  EXPECT_EQ(components.value().y, coordinate_y.value());
}

TEST(edwards_public_components_returns_point_and_curve) {
  const auto point{sourcemeta::core::hex_to_bytes(ED25519_PUBLIC_HEX)};
  const auto key{sourcemeta::core::make_eddsa_public_key(
      sourcemeta::core::EdwardsCurve::Ed25519, point.value())};
  EXPECT_TRUE(key.has_value());
  const auto components{
      sourcemeta::core::edwards_public_components(key.value())};
  EXPECT_TRUE(components.has_value());
  EXPECT_TRUE(components.value().curve ==
              sourcemeta::core::EdwardsCurve::Ed25519);
  EXPECT_EQ(components.value().point, point.value());
}

TEST(ec_public_components_on_an_rsa_key_is_absent) {
  const auto modulus{sourcemeta::core::base64url_decode(RFC7638_RSA_N)};
  const auto exponent{sourcemeta::core::base64url_decode("AQAB")};
  const auto key{
      sourcemeta::core::make_rsa_public_key(modulus.value(), exponent.value())};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::ec_public_components(key.value()).has_value());
}

TEST(rsa_public_components_on_an_ec_key_is_absent) {
  const auto coordinate_x{sourcemeta::core::hex_to_bytes(P256_QX_HEX)};
  const auto coordinate_y{sourcemeta::core::hex_to_bytes(P256_QY_HEX)};
  const auto key{sourcemeta::core::make_ec_public_key(
      sourcemeta::core::EllipticCurve::P256, coordinate_x.value(),
      coordinate_y.value())};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::rsa_public_components(key.value()).has_value());
}

TEST(derive_public_key_from_ec_private_matches_coordinates) {
  const auto coordinate_x{sourcemeta::core::hex_to_bytes(P256_QX_HEX)};
  const auto coordinate_y{sourcemeta::core::hex_to_bytes(P256_QY_HEX)};
  const auto scalar{sourcemeta::core::hex_to_bytes(P256_D_HEX)};
  const auto private_key{sourcemeta::core::make_ec_private_key(
      sourcemeta::core::EllipticCurve::P256, scalar.value(),
      coordinate_x.value(), coordinate_y.value())};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  EXPECT_TRUE(public_key.has_value());
  const auto components{
      sourcemeta::core::ec_public_components(public_key.value())};
  EXPECT_TRUE(components.has_value());
  EXPECT_EQ(components.value().x, coordinate_x.value());
  EXPECT_EQ(components.value().y, coordinate_y.value());
}

TEST(derive_public_key_from_ec_pem_produces_a_verifying_public_key) {
  const std::string pem{
      "-----BEGIN PRIVATE KEY-----\n"
      "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgUJlZcx3akefhQkHy\n"
      "hdm0LyliIIFyGpXJmWGGe30hqBChRANCAAQrQCBl4pxLtKex84zvju8dntvZF1Dm\n"
      "etJQhjqZwh81vCWJlgdqoA+F3pQN4jrapP2//GNWJ1pM+Yd7xZoDhKZI\n"
      "-----END PRIVATE KEY-----\n"};
  const auto private_key{sourcemeta::core::make_private_key(pem)};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  EXPECT_TRUE(public_key.has_value());
  const auto components{
      sourcemeta::core::ec_public_components(public_key.value())};
  EXPECT_TRUE(components.has_value());
  // The derived coordinates must form the public key that verifies a signature
  // the private key produces, which self-consistent thumbprint checks miss
  const auto rebuilt{sourcemeta::core::make_ec_public_key(
      components.value().curve, components.value().x, components.value().y)};
  EXPECT_TRUE(rebuilt.has_value());
  const std::string message{"public key derivation verification message"};
  const auto signature{sourcemeta::core::ecdsa_sign(
      private_key.value(), sourcemeta::core::SignatureHashFunction::SHA256,
      message)};
  EXPECT_TRUE(signature.has_value());
  EXPECT_TRUE(sourcemeta::core::ecdsa_verify(
      rebuilt.value(), sourcemeta::core::SignatureHashFunction::SHA256, message,
      signature.value()));
}

TEST(derive_public_key_from_ed25519_private_matches_public) {
  const auto seed{sourcemeta::core::hex_to_bytes(ED25519_SEED_HEX)};
  const auto expected{sourcemeta::core::hex_to_bytes(ED25519_PUBLIC_HEX)};
  const auto private_key{sourcemeta::core::make_edwards_private_key(
      sourcemeta::core::EdwardsCurve::Ed25519, seed.value())};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  EXPECT_TRUE(public_key.has_value());
  const auto components{
      sourcemeta::core::edwards_public_components(public_key.value())};
  EXPECT_TRUE(components.has_value());
  EXPECT_EQ(components.value().point, expected.value());
}

TEST(derive_public_key_from_ed448_private_matches_public) {
  const auto seed{sourcemeta::core::hex_to_bytes(ED448_SEED_HEX)};
  const auto expected{sourcemeta::core::hex_to_bytes(ED448_PUBLIC_HEX)};
  const auto private_key{sourcemeta::core::make_edwards_private_key(
      sourcemeta::core::EdwardsCurve::Ed448, seed.value())};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  EXPECT_TRUE(public_key.has_value());
  const auto components{
      sourcemeta::core::edwards_public_components(public_key.value())};
  EXPECT_TRUE(components.has_value());
  EXPECT_EQ(components.value().point, expected.value());
}
