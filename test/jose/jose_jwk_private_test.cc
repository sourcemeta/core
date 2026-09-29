#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// A private RSA JSON Web Key in the two-prime form, its elliptic curve and
// octet key pair counterparts, and the same RSA key as a PKCS#8 PEM document.
// The material matches the keys exercised by the crypto module tests
static constexpr std::string_view RSA_PRIVATE_JWK{
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

static constexpr std::string_view EC_PUBLIC_X_B64URL{
    "2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI"};
static constexpr std::string_view EC_PUBLIC_Y_B64URL{
    "5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ"};

static constexpr std::string_view EC_PRIVATE_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ",)"
    R"("d":"ttepxcp-OwXCj4-v4sGcxRxQRXA8D5Svu02yhcHvbd0"})"};

static constexpr std::string_view OKP_PRIVATE_JWK{
    R"({"kty":"OKP","crv":"Ed25519",)"
    R"("x":"SFOeT7YjsNIh-M1-nCBGnB9VWYu1uyNqiizCZhwddDo",)"
    R"("d":"dB7DXOXgcInKeC-I3gDYEDoc2OixFqzYr9kiwBvueGc"})"};

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

// The curves a PEM document names only through its parsed key material, so the
// curve name on the resulting key has to be recovered rather than read
static constexpr std::string_view P256_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgttepxcp+OwXCj4+v
4sGcxRxQRXA8D5Svu02yhcHvbd2hRANCAATZMBEZJarwX3uKrc2DjzAIQ3bpzHPL
w4LNsVr7pl+douZZM2aoVjpHNptsyTqLZeit/8evk0rPTp80CsG9+Q8U
-----END PRIVATE KEY-----
)"};

static constexpr std::string_view P384_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MIG2AgEAMBAGByqGSM49AgEGBSuBBAAiBIGeMIGbAgEBBDAC1aRLTztUK9z9MRsG
l9LMj1k0U/znimWvJvotWi8VNGiA0JBLr9EDseDmoNfiW1ChZANiAAScDIbkBp+8
EzQDTmdGuiO85MKA02vBRyRbQi+XZ+1r+WDB9eExONv/n3LQEs7cYf+G1mQ2/WI9
/QBzVO8kHQtF0UgPUpX1bE6GJDQRzfZx1tNZpK7z426wpazqZ3zRXwY=
-----END PRIVATE KEY-----
)"};

static constexpr std::string_view P521_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MIHuAgEAMBAGByqGSM49AgEGBSuBBAAjBIHWMIHTAgEBBEIBHZw4ip91fwtmWn38
XId+7SQu4w7u6dTn+lu/6q4Z2ce/wJO3AEyugeREjregkpCA//RgC9JJP9h+r5VX
/tGqRf+hgYkDgYYABAFHGu0B4OvZSGpazDfebhko56qcTVQuY8T65Q7pEm0XHNEB
6Q84+dGN0pzGCUiaQGPoROa3JGy0iAj5EK44nnchbwFE2ZJjNBb+wcXofhVvRVHi
b/NPaklPfqnokcj0G8buWwjTAvE57aVlbDoTmrcfHbKHkXChpd7cMhTj585jD+9D
AA==
-----END PRIVATE KEY-----
)"};

static constexpr std::string_view ED25519_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MC4CAQAwBQYDK2VwBCIEIHQew1zl4HCJyngviN4A2BA6HNjosRas2K/ZIsAb7nhn
-----END PRIVATE KEY-----
)"};

static constexpr std::string_view ED448_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MEcCAQAwBQYDK2VxBDsEOXAnAoFAyetCKTXh0wngHmZnA4kXPaT2bEAWwkklE7TB
mJJj6hMlDmWnhKqMAamWCnQWwuKHmyoilQ==
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

TEST(jwk_private_oct_valid) {
  const auto document{sourcemeta::core::parse_json(
      R"({"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T)"
      R"(-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})")};
  const auto key{sourcemeta::core::JWKPrivate::from(document)};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(), sourcemeta::core::JWKPrivate::Type::Octet);
  EXPECT_EQ(key.value().secret().size(), 64);
  EXPECT_TRUE(key.value().private_key() == nullptr);
}

TEST(jwk_private_oct_with_key_id) {
  const auto document{sourcemeta::core::parse_json(
      R"({"kty":"oct","kid":"018c0ae5-4d9b-471b-bfd6-eef314bc7037",)"
      R"("k":"hJtXIZ2uSN5kbQfbtTNWbpdmhkV8FJG-Onbc6mxCcYg"})")};
  const auto key{sourcemeta::core::JWKPrivate::from(document)};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().key_id().has_value());
  EXPECT_EQ(key.value().key_id().value(),
            "018c0ae5-4d9b-471b-bfd6-eef314bc7037");
  EXPECT_EQ(key.value().secret().size(), 32);
}

TEST(jwk_private_oct_missing_key_value) {
  const auto document{sourcemeta::core::parse_json(R"({"kty":"oct"})")};
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_oct_non_string_key_value) {
  const auto document{sourcemeta::core::parse_json(R"({"kty":"oct","k":123})")};
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_oct_invalid_base64url_key_value) {
  const auto document{
      sourcemeta::core::parse_json(R"({"kty":"oct","k":"!!!"})")};
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_oct_empty_key_value) {
  const auto document{sourcemeta::core::parse_json(R"({"kty":"oct","k":""})")};
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_octets) {
  const auto key{sourcemeta::core::JWKPrivate::from_octets(
      "0123456789abcdef0123456789abcdef")};
  EXPECT_EQ(key.type(), sourcemeta::core::JWKPrivate::Type::Octet);
  EXPECT_EQ(key.secret(), "0123456789abcdef0123456789abcdef");
  EXPECT_FALSE(key.key_id().has_value());
  EXPECT_FALSE(key.algorithm().has_value());
  EXPECT_FALSE(key.public_jwk().has_value());
}

TEST(jwk_private_from_json_rejects_unknown_key_type) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(R"({"kty":"XYZ","d":"abc"})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_missing_key_type) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(R"({"crv":"P-256","d":"abc"})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_non_string_key_type) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(R"({"kty":42})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_rsa_multi_prime) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(
                       R"({"kty":"RSA","n":"n","e":"AQAB","d":"d","p":"p",)"
                       R"("q":"q","dp":"dp","dq":"dq","qi":"qi","oth":[]})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_rsa_missing_component) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(
                       R"({"kty":"RSA","n":"n","e":"AQAB","d":"d","p":"p",)"
                       R"("q":"q","dp":"dp","dq":"dq"})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_rsa_with_invalid_base64url) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(
                       R"({"kty":"RSA","n":"!!!","e":"AQAB","d":"d","p":"p",)"
                       R"("q":"q","dp":"dp","dq":"dq","qi":"qi"})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_ec_unknown_curve) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(
                       R"({"kty":"EC","crv":"P-999","x":"x","y":"y","d":"d"})"))
                   .has_value());
}

TEST(jwk_private_from_json_rejects_okp_unknown_curve) {
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(
                   sourcemeta::core::parse_json(
                       R"({"kty":"OKP","crv":"X25519","x":"x","d":"d"})"))
                   .has_value());
}

TEST(jwk_private_from_pem_recovers_the_p256_curve_name) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(P256_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(),
            sourcemeta::core::JWKPrivate::Type::EllipticCurve);
  EXPECT_EQ(key.value().curve(), "P-256");
}

TEST(jwk_private_from_pem_recovers_the_p384_curve_name) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(P384_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(),
            sourcemeta::core::JWKPrivate::Type::EllipticCurve);
  EXPECT_EQ(key.value().curve(), "P-384");
}

TEST(jwk_private_from_pem_recovers_the_p521_curve_name) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(P521_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(),
            sourcemeta::core::JWKPrivate::Type::EllipticCurve);
  EXPECT_EQ(key.value().curve(), "P-521");
}

TEST(jwk_private_from_pem_recovers_the_ed25519_curve_name) {
  const auto key{
      sourcemeta::core::JWKPrivate::from_pem(ED25519_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(),
            sourcemeta::core::JWKPrivate::Type::OctetKeyPair);
  EXPECT_EQ(key.value().curve(), "Ed25519");
}

TEST(jwk_private_from_pem_recovers_the_ed448_curve_name) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(ED448_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(key.has_value());
  EXPECT_EQ(key.value().type(),
            sourcemeta::core::JWKPrivate::Type::OctetKeyPair);
  EXPECT_EQ(key.value().curve(), "Ed448");
}

TEST(jwk_private_from_json_rejects_an_rsa_key_missing_a_prime_factor) {
  auto document{sourcemeta::core::parse_json(RSA_PRIVATE_JWK)};
  document.erase("qi");
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_rejects_an_rsa_key_missing_its_private_exponent) {
  auto document{sourcemeta::core::parse_json(RSA_PRIVATE_JWK)};
  document.erase("d");
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_rejects_a_modulus_below_the_required_size) {
  // RFC 7518 Section 3.3 requires a modulus of at least 2048 bits, so a
  // thousand-bit one has to be refused however well formed the rest is
  auto document{sourcemeta::core::parse_json(RSA_PRIVATE_JWK)};
  document.assign("n", sourcemeta::core::JSON{std::string(171, 'A')});
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_rejects_an_elliptic_curve_key_without_a_curve) {
  auto document{sourcemeta::core::parse_json(EC_PRIVATE_JWK)};
  document.erase("crv");
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_rejects_an_elliptic_curve_with_a_non_string_name) {
  auto document{sourcemeta::core::parse_json(EC_PRIVATE_JWK)};
  document.assign("crv", sourcemeta::core::JSON{256});
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_rejects_an_octet_key_pair_without_a_curve) {
  auto document{sourcemeta::core::parse_json(OKP_PRIVATE_JWK)};
  document.erase("crv");
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_rejects_an_octet_key_pair_with_a_non_string_name) {
  auto document{sourcemeta::core::parse_json(OKP_PRIVATE_JWK)};
  document.assign("crv", sourcemeta::core::JSON{25519});
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_rejects_a_non_string_key_identifier) {
  auto document{sourcemeta::core::parse_json(RSA_PRIVATE_JWK)};
  document.assign("kid", sourcemeta::core::JSON{7});
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_rejects_a_non_string_algorithm) {
  auto document{sourcemeta::core::parse_json(RSA_PRIVATE_JWK)};
  document.assign("alg", sourcemeta::core::JSON{256});
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(jwk_private_from_json_honours_an_algorithm_that_suits_an_rsa_key) {
  auto document{sourcemeta::core::parse_json(RSA_PRIVATE_JWK)};
  document.assign("alg", sourcemeta::core::JSON{"RS256"});
  const auto key{sourcemeta::core::JWKPrivate::from(document)};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().algorithm().has_value());
  EXPECT_EQ(key.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::RS256);
}

TEST(jwk_private_from_json_ignores_an_algorithm_that_suits_another_key_type) {
  // RFC 7517 Section 4.4 makes the algorithm advisory, so one that names a
  // different key type leaves the hint unset rather than refusing the key
  auto document{sourcemeta::core::parse_json(RSA_PRIVATE_JWK)};
  document.assign("alg", sourcemeta::core::JSON{"ES256"});
  const auto key{sourcemeta::core::JWKPrivate::from(document)};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(key.value().algorithm().has_value());
}
