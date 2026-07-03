#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// An Ed25519 key pair, since the JOSE cookbook carries no Edwards example
static const std::string OKP_PRIVATE_JWK{
    R"({"kty":"OKP","crv":"Ed25519",)"
    R"("x":"SFOeT7YjsNIh-M1-nCBGnB9VWYu1uyNqiizCZhwddDo",)"
    R"("d":"dB7DXOXgcInKeC-I3gDYEDoc2OixFqzYr9kiwBvueGc"})"};
static const std::string OKP_PUBLIC_JWK{
    R"({"kty":"OKP","crv":"Ed25519",)"
    R"("x":"SFOeT7YjsNIh-M1-nCBGnB9VWYu1uyNqiizCZhwddDo"})"};

// A P-256 key as PEM, whose parsed form carries no curve name, with its public
// coordinates
static const std::string EC_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg+WFQs5q2XetYWqZm
Ci8zy4KBzd25YLQ/9CCEgd/fZVWhRANCAATbql1xB7Tw6SgrR2SdqcmKGpxOuTaz
tFKcM9RWrXyBKkkB1YI1epQBgrAVNIfpp4k/nzHACQBp5iZictVxHsBm
-----END PRIVATE KEY-----
)"};
static const std::string EC_PUBLIC_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"26pdcQe08OkoK0dknanJihqcTrk2s7RSnDPUVq18gSo",)"
    R"("y":"SQHVgjV6lAGCsBU0h-mniT-fMcAJAGnmJmJy1XEewGY"})"};

TEST(jws_sign_eddsa_round_trips) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OKP_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const std::string_view signing_input{
      "eyJhbGciOiJFZERTQSJ9.eyJpc3MiOiJhY21lIn0"};
  const auto signature{sourcemeta::core::jws_sign(
      sourcemeta::core::JWSAlgorithm::EdDSA, signing_input, key.value())};
  EXPECT_TRUE(signature.has_value());
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(OKP_PUBLIC_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::EdDSA, signing_input, signature.value(),
      public_key.value()));
}

TEST(jws_sign_ec_from_pem_signs_es256) {
  // A key parsed from PEM carries no curve name, so signing relies on the
  // signature width to bind the curve to the algorithm
  const auto key{sourcemeta::core::JWKPrivate::from_pem(EC_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  const std::string_view signing_input{
      "eyJhbGciOiJFUzI1NiJ9.eyJpc3MiOiJhY21lIn0"};
  const auto signature{sourcemeta::core::jws_sign(
      sourcemeta::core::JWSAlgorithm::ES256, signing_input, key.value())};
  EXPECT_TRUE(signature.has_value());
  const auto public_key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_PUBLIC_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::ES256, signing_input, signature.value(),
      public_key.value()));
}

TEST(jws_sign_ec_from_pem_rejects_mismatched_curve_algorithm) {
  // The P-256 key cannot serve ES384, which the width check catches even
  // without a curve name
  const auto key{sourcemeta::core::JWKPrivate::from_pem(EC_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::ES384,
                                          "header.payload", key.value())
                   .has_value());
}

TEST(jws_sign_rejects_wrong_key_type) {
  // An Edwards key cannot produce an ECDSA signature
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OKP_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::ES256,
                                          "header.payload", key.value())
                   .has_value());
}

TEST(jws_sign_rejects_contradicting_declared_algorithm) {
  // The key declares EdDSA, so signing under any other algorithm is refused
  // (RFC 7517 Section 4.4), independently of the type check
  const auto key{sourcemeta::core::JWKPrivate::from(sourcemeta::core::parse_json(
      R"({"kty":"OKP","crv":"Ed25519",)"
      R"("x":"SFOeT7YjsNIh-M1-nCBGnB9VWYu1uyNqiizCZhwddDo",)"
      R"("d":"dB7DXOXgcInKeC-I3gDYEDoc2OixFqzYr9kiwBvueGc","alg":"EdDSA"})"))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().algorithm().has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::ES256,
                                          "header.payload", key.value())
                   .has_value());
}
