#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

// A private RSA JSON Web Key in the two-prime form, its elliptic curve and
// octet key pair counterparts, and the same RSA key as a PKCS#8 PEM document.
// The material matches the keys exercised by the crypto module tests
static const std::string RSA_PRIVATE_JWK{
    R"({"kty":"RSA",)"
    R"("n":"n-ivWLN0CTSLXc7GNcHGOuZHdu6QcGT3SlFTaf2ckxLVEDcAU4fIez7rKtvZaCzp)"
    R"(KJ6cb5KZnDcIn-gOCrE0FI6OZdCvgAGNqoFH6D1AANk8lWSFc1bWoti5xzXls2IN4fDRDa)"
    R"(BoXeXBniurOF8zTMlpyFiX4sgmaPpaJuKGH97AyXkHg4AYAFOUxOMPDM4EglexkzWRo0z_)"
    R"(qwRfj4qKbaNqF2P24dLOclnojEqD1i-ZKZn4PzD4plw85DSjc6Sm90OOpFW54zxiX2Eff5)"
    R"(9ez46j91_2d6SlSzFhNwrvaZYKBJl-vNegJzxsHWPwrGcNR31jEXa6kepjbAsp0FOBJw",)"
    R"("e":"AQAB",)"
    R"("d":"DdJY8FEijcvNjWAwCw7NXuNEo2e0yGI8awoN1wglc85EPaXjZf-YjpPo_nKzn1OHx)"
    R"(mEJ-ILdZX6Zmj4T6KIEdGUWCIA54ICXNCOp3s2x2IuZDE38qEmF59wqbVOf9RoGmn7sMuy)"
    R"(og2U2j8C1M1GB56Mz0i4GLYNLU_Y_u0NdKyfnhYRijuGOIb_TcC0MOy269XhsAVr-Ntat0)"
    R"(OzlITbaGKGW0yOr96hUY2Ocax99xbtjMyiYhB5Hu99yHn7ZPRtfATnmAujjWPV7Knd9n1e)"
    R"(O-hC6X8f-eKl9v-iqLuhNnFBGHhd3So_FZ0W3qXTXfC0Oq4TUsRjf4k4EtOcGf80SoQ",)"
    R"("p":"zYCJC70qUjNIlxUZ4u_XmDF8FiIsEo5A09eRNQchLnGClp_pVQ9z77mSicWsyUJOt)"
    R"(I9YTKWKCwniN0RIPZM2O3cV4ZC8hdWP5noTxitnbLYeyfoZDr-i8uPqHHu3vaW-VJhGeJj)"
    R"(5yELFrOxXAlikA2QQkecn94hs3iQ9tdFFc-8",)"
    R"("q":"xzQIWpdrkeQnOk3_YoO7XY8ZyRiQwv-8kqQ8oae3Njo06oMWC0VbV8o5HPj-pCMWB)"
    R"(vwAfMdZznOhMGjV2Ajla-z2ANteWebFVYt1xu2bxsMDIPqMfXTxWUftHtmFi6PSS9ZN5mI)"
    R"(22AarR0nLDuFHr3KG3lWm173-RfgvhH_Xrkk",)"
    R"("dp":"WSWfTfZbu5j_rnq65hWBg0ZEPB3K3KnVOZDULxrOrCUVr13jjMDNXHs2NIoKMKyR)"
    R"(FAbzGRzey3cYKT130S5hYl6AoX92KODCMgtXNKpzjVdb9-aEpD9B4vg4AO8ygBS8glokiA)"
    R"(Bkqxk8Q42rGRt22vm3rnOGhP0rrRovowYLiQU",)"
    R"("dq":"o08lCh_ZMGG7RzFqjXkxwiHvIc3h3_uIvS-oBV9Z9DsD5r5Q9CyIFhDTgc0f9bBN)"
    R"(_qvaOnG0Tmy9WNKZfeLNMw2xIEK3tzxZyyfqBowFiY2WoxLE2pVkx60P2Jq7wR8s6L9oXd)"
    R"(dm3vOYt3jn3-sQueVKbDwL7BL2wqYVTqsARwE",)"
    R"("qi":"qNjXTrOsMEqKYSJNsk6Z0sjDB2yuHS47CbkWZjikg4TIR4WmpYVY7EOfidoboen5)"
    R"(jGKFMuXAJo4pz6IBgS5eN6uzPvfvqJUiMGc6lG_zBFKsc-WnveUIzYqqz3gNrA_gwN872o)"
    R"(JCzSsEiFwMKFkui2fIAH8-c7Z5Tpo63qroRxk"})"};

static const std::string EC_PUBLIC_X_B64URL{
    "2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI"};
static const std::string EC_PUBLIC_Y_B64URL{
    "5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ"};

static const std::string EC_PRIVATE_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ",)"
    R"("d":"ttepxcp-OwXCj4-v4sGcxRxQRXA8D5Svu02yhcHvbd0"})"};

static const std::string OKP_PRIVATE_JWK{
    R"({"kty":"OKP","crv":"Ed25519",)"
    R"("x":"SFOeT7YjsNIh-M1-nCBGnB9VWYu1uyNqiizCZhwddDo",)"
    R"("d":"dB7DXOXgcInKeC-I3gDYEDoc2OixFqzYr9kiwBvueGc"})"};

static const std::string RSA_PRIVATE_KEY_PEM{
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

TEST(jwk_private_from_pem_parses_rsa) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(RSA_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWKPrivate::Type::RSA);
  EXPECT_TRUE(key.value().private_key() != nullptr);
}

TEST(jwk_private_from_json_parses_rsa) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(RSA_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWKPrivate::Type::RSA);
  EXPECT_TRUE(key.value().private_key() != nullptr);
}

TEST(jwk_private_from_json_parses_ec) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(),
            sourcemeta::core::JWKPrivate::Type::EllipticCurve);
  EXPECT_EQ(key.value().curve(), "P-256");
  EXPECT_TRUE(key.value().private_key() != nullptr);
}

TEST(jwk_private_from_json_parses_okp) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OKP_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(),
            sourcemeta::core::JWKPrivate::Type::OctetKeyPair);
  EXPECT_EQ(key.value().curve(), "Ed25519");
  EXPECT_TRUE(key.value().private_key() != nullptr);
}

TEST(jwk_private_from_json_reads_key_id) {
  const auto key{sourcemeta::core::JWKPrivate::from(sourcemeta::core::parse_json(
      R"({"kty":"OKP","crv":"Ed25519",)"
      R"("x":"SFOeT7YjsNIh-M1-nCBGnB9VWYu1uyNqiizCZhwddDo",)"
      R"("d":"dB7DXOXgcInKeC-I3gDYEDoc2OixFqzYr9kiwBvueGc","kid":"my-key"})"))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().key_id().has_value());
  EXPECT_EQ(key.value().key_id().value(), "my-key");
}

TEST(jwk_private_from_json_reads_matching_algorithm) {
  const auto key{sourcemeta::core::JWKPrivate::from(sourcemeta::core::parse_json(
      R"({"kty":"EC","crv":"P-256",)"
      R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
      R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ",)"
      R"("d":"ttepxcp-OwXCj4-v4sGcxRxQRXA8D5Svu02yhcHvbd0","alg":"ES256"})"))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().algorithm().has_value());
  EXPECT_EQ(key.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::ES256);
}

TEST(jwk_private_from_json_ignores_mismatched_algorithm) {
  // A P-256 key cannot be used with ES384, so the advisory hint is dropped
  // while the key itself still parses
  const auto key{sourcemeta::core::JWKPrivate::from(sourcemeta::core::parse_json(
      R"({"kty":"EC","crv":"P-256",)"
      R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
      R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ",)"
      R"("d":"ttepxcp-OwXCj4-v4sGcxRxQRXA8D5Svu02yhcHvbd0","alg":"ES384"})"))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(key.value().algorithm().has_value());
}

TEST(jwk_private_from_json_rejects_public_only_rsa) {
  // The private parameters are required, so a public key is rejected
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(
                       R"({"kty":"RSA","n":"n-iv","e":"AQAB"})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_ec_without_scalar) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(
                       R"({"kty":"EC","crv":"P-256",)"
                       R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
                       R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ"})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_non_object) {
  EXPECT_FALSE(
      sourcemeta::core::JWKPrivate::from(sourcemeta::core::parse_json("42"))
          .has_value());
}

TEST(jwk_private_from_pem_rejects_garbage) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from_pem("not a pem").has_value());
}

TEST(jwk_private_ec_private_key_signs_and_verifies) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().private_key() != nullptr);
  const std::string_view message{"jwk private key signing input"};
  const auto signature{sourcemeta::core::ecdsa_sign(
      *key.value().private_key(),
      sourcemeta::core::SignatureHashFunction::SHA256, message)};
  EXPECT_TRUE(signature.has_value());
  const auto public_key{sourcemeta::core::make_ec_public_key(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::base64url_decode(EC_PUBLIC_X_B64URL).value(),
      sourcemeta::core::base64url_decode(EC_PUBLIC_Y_B64URL).value())};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::ecdsa_verify(
      public_key.value(), sourcemeta::core::SignatureHashFunction::SHA256,
      message, signature.value()));
}
