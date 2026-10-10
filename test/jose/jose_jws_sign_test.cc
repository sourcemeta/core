#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// An Ed25519 key pair, since the JOSE cookbook carries no Edwards example
static constexpr std::string_view OKP_PRIVATE_JWK{
    R"({"kty":"OKP","crv":"Ed25519",)"
    R"("x":"SFOeT7YjsNIh-M1-nCBGnB9VWYu1uyNqiizCZhwddDo",)"
    R"("d":"dB7DXOXgcInKeC-I3gDYEDoc2OixFqzYr9kiwBvueGc"})"};
static constexpr std::string_view OKP_PUBLIC_JWK{
    R"({"kty":"OKP","crv":"Ed25519",)"
    R"("x":"SFOeT7YjsNIh-M1-nCBGnB9VWYu1uyNqiizCZhwddDo"})"};

// A P-256 key as PEM, whose parsed form carries no curve name, with its public
// coordinates
static constexpr std::string_view EC_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg+WFQs5q2XetYWqZm
Ci8zy4KBzd25YLQ/9CCEgd/fZVWhRANCAATbql1xB7Tw6SgrR2SdqcmKGpxOuTaz
tFKcM9RWrXyBKkkB1YI1epQBgrAVNIfpp4k/nzHACQBp5iZictVxHsBm
-----END PRIVATE KEY-----
)"};
static constexpr std::string_view EC_PUBLIC_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"26pdcQe08OkoK0dknanJihqcTrk2s7RSnDPUVq18gSo",)"
    R"("y":"SQHVgjV6lAGCsBU0h-mniT-fMcAJAGnmJmJy1XEewGY"})"};

// The RFC 7515 Appendix A.1 example key and signing input
static constexpr std::string_view OCT_JWK{
    R"({"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T)"
    R"(-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})"};
static constexpr std::string_view RFC7515_A1_SIGNING_INPUT{
    "eyJ0eXAiOiJKV1QiLA0KICJhbGciOiJIUzI1NiJ9."
    "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlL"
    "mNvbS9pc19yb290Ijp0cnVlfQ"};

// The RSA key the private key tests carry, in the document form this module
// parses, with its public half as a JSON Web Key. The modulus was taken from
// the same key rather than transcribed
static constexpr std::string_view RSA_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCf6K9Ys3QJNItd
zsY1wcY65kd27pBwZPdKUVNp/ZyTEtUQNwBTh8h7Pusq29loLOkonpxvkpmcNwif
6A4KsTQUjo5l0K+AAY2qgUfoPUAA2TyVZIVzVtai2LnHNeWzYg3h8NENoGhd5cGe
K6s4XzNMyWnIWJfiyCZo+lom4oYf3sDJeQeDgBgAU5TE4w8MzgSCV7GTNZGjTP+r
BF+Piopto2oXY/bh0s5yWeiMSoPWL5kpmfg/MPimXDzkNKNzpKb3Q46kVbnjPGJf
YR9/n17PjqP3X/Z3pKVLMWE3Cu9plgoEmX6816AnPGwdY/CsZw1HfWMRdrqR6mNs
CynQU4EnAgMBAAECggEADdJY8FEijcvNjWAwCw7NXuNEo2e0yGI8awoN1wglc85E
PaXjZf+YjpPo/nKzn1OHxmEJ+ILdZX6Zmj4T6KIEdGUWCIA54ICXNCOp3s2x2IuZ
DE38qEmF59wqbVOf9RoGmn7sMuyog2U2j8C1M1GB56Mz0i4GLYNLU/Y/u0NdKyfn
hYRijuGOIb/TcC0MOy269XhsAVr+Ntat0OzlITbaGKGW0yOr96hUY2Ocax99xbtj
MyiYhB5Hu99yHn7ZPRtfATnmAujjWPV7Knd9n1eO+hC6X8f+eKl9v+iqLuhNnFBG
Hhd3So/FZ0W3qXTXfC0Oq4TUsRjf4k4EtOcGf80SoQKBgQDNgIkLvSpSM0iXFRni
79eYMXwWIiwSjkDT15E1ByEucYKWn+lVD3PvuZKJxazJQk60j1hMpYoLCeI3REg9
kzY7dxXhkLyF1Y/mehPGK2dsth7J+hkOv6Ly4+oce7e9pb5UmEZ4mPnIQsWs7FcC
WKQDZBCR5yf3iGzeJD210UVz7wKBgQDHNAhal2uR5Cc6Tf9ig7tdjxnJGJDC/7yS
pDyhp7c2OjTqgxYLRVtXyjkc+P6kIxYG/AB8x1nOc6EwaNXYCOVr7PYA215Z5sVV
i3XG7ZvGwwMg+ox9dPFZR+0e2YWLo9JL1k3mYjbYBqtHScsO4UevcobeVabXvf5F
+C+Ef9euSQKBgFkln032W7uY/656uuYVgYNGRDwdytyp1TmQ1C8azqwlFa9d44zA
zVx7NjSKCjCskRQG8xkc3st3GCk9d9EuYWJegKF/dijgwjILVzSqc41XW/fmhKQ/
QeL4OADvMoAUvIJaJIgAZKsZPEONqxkbdtr5t65zhoT9K60aL6MGC4kFAoGBAKNP
JQof2TBhu0cxao15McIh7yHN4d/7iL0vqAVfWfQ7A+a+UPQsiBYQ04HNH/WwTf6r
2jpxtE5svVjSmX3izTMNsSBCt7c8Wcsn6gaMBYmNlqMSxNqVZMetD9iau8EfLOi/
aF3XZt7zmLd459/rELnlSmw8C+wS9sKmFU6rAEcBAoGBAKjY106zrDBKimEiTbJO
mdLIwwdsrh0uOwm5FmY4pIOEyEeFpqWFWOxDn4naG6Hp+YxihTLlwCaOKc+iAYEu
Xjersz7376iVIjBnOpRv8wRSrHPlp73lCM2Kqs94DawP4MDfO9qCQs0rBIhcDChZ
LotnyAB/PnO2eU6aOt6q6EcZ
-----END PRIVATE KEY-----
)"};
static constexpr std::string_view RSA_PUBLIC_JWK{
    R"({"kty":"RSA","e":"AQAB","n":")"
    "n-ivWLN0CTSLXc7GNcHGOuZHdu6QcGT3SlFTaf2ckxLVEDcAU4fIez7rKtvZaCzp"
    "KJ6cb5KZnDcIn-gOCrE0FI6OZdCvgAGNqoFH6D1AANk8lWSFc1bWoti5xzXls2IN"
    "4fDRDaBoXeXBniurOF8zTMlpyFiX4sgmaPpaJuKGH97AyXkHg4AYAFOUxOMPDM4E"
    "glexkzWRo0z_qwRfj4qKbaNqF2P24dLOclnojEqD1i-ZKZn4PzD4plw85DSjc6Sm"
    "90OOpFW54zxiX2Eff59ez46j91_2d6SlSzFhNwrvaZYKBJl-vNegJzxsHWPwrGcN"
    "R31jEXa6kepjbAsp0FOBJw"
    R"("})"};

// A secret of exactly the SHA-256 output size, which the smallest MAC accepts
// and the larger ones do not
static constexpr std::string_view OCT_JWK_32_BYTES{
    R"({"kty":"oct","k":"QUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUE"})"};

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

TEST(jws_sign_hs256_known_answer) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{
      sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::HS256,
                                 RFC7515_A1_SIGNING_INPUT, key.value())};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(sourcemeta::core::base64url_encode(signature.value()),
            "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk");
}

TEST(jws_sign_hs384_round_trips) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{
      sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::HS384,
                                 RFC7515_A1_SIGNING_INPUT, key.value())};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(signature.value().size(), 48);
  const auto public_key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS384, RFC7515_A1_SIGNING_INPUT,
      signature.value(), public_key.value()));
}

TEST(jws_sign_hs512_round_trips) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{
      sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::HS512,
                                 RFC7515_A1_SIGNING_INPUT, key.value())};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(signature.value().size(), 64);
  const auto public_key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS512, RFC7515_A1_SIGNING_INPUT,
      signature.value(), public_key.value()));
}

TEST(jws_sign_hs256_rejects_asymmetric_key) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(EC_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  const auto signature{
      sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::HS256,
                                 RFC7515_A1_SIGNING_INPUT, key.value())};
  EXPECT_FALSE(signature.has_value());
}

TEST(jws_sign_es256_rejects_oct_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{
      sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::ES256,
                                 RFC7515_A1_SIGNING_INPUT, key.value())};
  EXPECT_FALSE(signature.has_value());
}

TEST(jws_sign_hs512_rejects_short_secret) {
  const auto key{sourcemeta::core::JWKPrivate::from(sourcemeta::core::parse_json(
      R"({"kty":"oct","k":"hJtXIZ2uSN5kbQfbtTNWbpdmhkV8FJG-Onbc6mxCcYg"})"))};
  EXPECT_TRUE(key.has_value());
  const auto signature{
      sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::HS512,
                                 RFC7515_A1_SIGNING_INPUT, key.value())};
  EXPECT_FALSE(signature.has_value());
}

// RFC 8017 Section 8.2.1 makes RSASSA-PKCS1-v1_5 deterministic, so the exact
// octets say which hash the algorithm reached for. A round trip cannot, because
// signing and verifying share that choice. These were produced outside this
// library from the same key and the same input
TEST(jws_sign_rs384_produces_the_known_signature) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(RSA_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  const std::string_view signing_input{
      "eyJhbGciOiJSUzM4NCJ9.eyJpc3MiOiJhY21lIn0"};
  const auto signature{sourcemeta::core::jws_sign(
      sourcemeta::core::JWSAlgorithm::RS384, signing_input, key.value())};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(sourcemeta::core::base64url_encode(signature.value()),
            "PRiZ0jtUZdPKfdtTopoxiyA9hcuQmt5VzMGYI3Q-nqGEsej5cFrKA_sUKn06GwBD"
            "8Ppj75BsSrrIRnnfTRiEbMpw_ozZG8cA8C6fxSiH4VyHVhz4GlkEfHZ5Q1L6oFMf"
            "B_2BXUwY2_wnYf4zqJZmRD1NtlBv2XzPvRb21A-hSO8brmxdfd14OQnmaSEDOlPW"
            "0W6YLe5KQ8Xm46MDCTxncLOvJAaJjgv04cmMGtIW1skU_lYrVdTaq81UlDIH_YiG"
            "PCgRpnvj1b0FVgd1VIH2zQo7yCYMwPcfWBOah_XojRtLKh4be9HNxNrTOijmdvdh"
            "NNONph9sG2OfrZcChn4WYQ");
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(RSA_PUBLIC_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS384, signing_input, signature.value(),
      public_key.value()));
}

TEST(jws_sign_rs512_produces_the_known_signature) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(RSA_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  const std::string_view signing_input{
      "eyJhbGciOiJSUzUxMiJ9.eyJpc3MiOiJhY21lIn0"};
  const auto signature{sourcemeta::core::jws_sign(
      sourcemeta::core::JWSAlgorithm::RS512, signing_input, key.value())};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(sourcemeta::core::base64url_encode(signature.value()),
            "BNanFqTOr_8bCDJHKTvoJEasNu2pDOmNYlghRwg-I96ABTbjLWVnJjrN-RAN3PPc"
            "6TnAoAJgnFJuf9klQZGZgfBKwBqjXRNtjX1Jc5So2E2W-pzwq3KzpEohm_jcFj18"
            "WIh_2I2ui4E3C2tmmWK4SONCwGiZXv0tAyym9rvakLEqJkKEhLZCG5jxnYkzAWHQ"
            "-V-g9ySJR5EZjXERIRR4fBQJCxRbxPow8bA8fXenabMMZCAlDlcnLGPO-8ON_bJY"
            "WOxAh3CaV6R9R4pXHYN_-0X0n3c0squefLoY6bLkeUizYA0iazYX9lK4ni-FB7Oy"
            "r5UONNYAKP0IXFnzyFuMTw");
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(RSA_PUBLIC_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS512, signing_input, signature.value(),
      public_key.value()));
}

// RFC 7518 Section 3.5 ties the PSS hash and the MGF1 hash to one function, so
// a signature made under one of these verifies under that one alone. The salt
// makes the scheme randomised, so what it produces cannot be pinned to fixed
// octets and the hash is pinned by which algorithm accepts it instead
TEST(jws_sign_ps512_verifies_under_ps512_alone) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(RSA_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  const std::string_view signing_input{
      "eyJhbGciOiJQUzUxMiJ9.eyJpc3MiOiJhY21lIn0"};
  const auto signature{sourcemeta::core::jws_sign(
      sourcemeta::core::JWSAlgorithm::PS512, signing_input, key.value())};
  EXPECT_TRUE(signature.has_value());
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(RSA_PUBLIC_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::PS512, signing_input, signature.value(),
      public_key.value()));
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::PS384, signing_input, signature.value(),
      public_key.value()));
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::PS256, signing_input, signature.value(),
      public_key.value()));
}

TEST(jws_sign_ps256_verifies_under_ps256_alone) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(RSA_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  const std::string_view signing_input{
      "eyJhbGciOiJQUzI1NiJ9.eyJpc3MiOiJhY21lIn0"};
  const auto signature{sourcemeta::core::jws_sign(
      sourcemeta::core::JWSAlgorithm::PS256, signing_input, key.value())};
  EXPECT_TRUE(signature.has_value());
  const auto public_key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(RSA_PUBLIC_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::PS256, signing_input, signature.value(),
      public_key.value()));
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::PS384, signing_input, signature.value(),
      public_key.value()));
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::PS512, signing_input, signature.value(),
      public_key.value()));
}

// RFC 7518 Section 3.3 gives RSASSA-PKCS1-v1_5 an RSA key
TEST(jws_sign_rs256_rejects_an_ec_key) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(EC_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::RS256,
                                          "eyJhbGciOiJSUzI1NiJ9.e30",
                                          key.value()));
}

// RFC 7518 Section 3.5 gives PSS an RSA key
TEST(jws_sign_ps256_rejects_an_ec_key) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(EC_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::PS256,
                                          "eyJhbGciOiJQUzI1NiJ9.e30",
                                          key.value()));
}

// RFC 8037 Section 3.1 gives EdDSA an octet key pair
TEST(jws_sign_eddsa_rejects_an_rsa_key) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(RSA_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::EdDSA,
                                          "eyJhbGciOiJFZERTQSJ9.e30",
                                          key.value()));
}

// RFC 7518 Section 3.2 gives every MAC a symmetric key
TEST(jws_sign_hs384_rejects_an_rsa_key) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(RSA_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::HS384,
                                          "eyJhbGciOiJIUzM4NCJ9.e30",
                                          key.value()));
}

// RFC 7518 Section 3.2 asks for a key of at least the hash output size, which
// for this algorithm is more than the thirty-two octets here
TEST(jws_sign_hs384_rejects_a_secret_below_the_hash_size) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK_32_BYTES))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::HS384,
                                          "eyJhbGciOiJIUzM4NCJ9.e30",
                                          key.value()));
}

// RFC 7518 Section 3.2 gives every MAC a symmetric key
TEST(jws_sign_hs512_rejects_an_ec_key) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(EC_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::HS512,
                                          "eyJhbGciOiJIUzUxMiJ9.e30",
                                          key.value()));
}

// An asymmetric algorithm signs with the platform key the JWK was parsed into,
// and a symmetric key has none, so there is nothing to sign with

TEST(jws_sign_rs256_with_a_symmetric_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK_32_BYTES))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::RS256,
                                          "eyJhbGciOiJSUzI1NiJ9.e30",
                                          key.value())
                   .has_value());
}

TEST(jws_sign_ps256_with_a_symmetric_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK_32_BYTES))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::PS256,
                                          "eyJhbGciOiJQUzI1NiJ9.e30",
                                          key.value())
                   .has_value());
}

TEST(jws_sign_eddsa_with_a_symmetric_key) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK_32_BYTES))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_sign(sourcemeta::core::JWSAlgorithm::EdDSA,
                                          "eyJhbGciOiJFZERTQSJ9.e30",
                                          key.value())
                   .has_value());
}
