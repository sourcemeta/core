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
