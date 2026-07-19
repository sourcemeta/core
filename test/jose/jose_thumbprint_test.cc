#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

// A 2048-bit RSA private key in unencrypted PKCS#8, reused from the signing
// tests, to exercise the PEM public-recovery path
static const std::string RSA_PRIVATE_KEY{
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

TEST(thumbprint_of_rfc7638_section_3_1_rsa_example) {
  // The n value is on a single line because adjacent-literal concatenation
  // does not apply inside a raw string
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "kty": "RSA",
    "n": "0vx7agoebGcQSuuPiLJXZptN9nndrQmbXEps2aiAFbWhM78LhWx4cbbfAAtVT86zwu1RK7aPFFxuhDR1L6tSoc_BJECPebWKRXjBZCiFV4n3oknjhMstn64tZ_2W-5JsGY4Hc5n9yBXArwl93lqt7_RN5w6Cf0h4QyQ5v-65YGjQR0_FDW2QvzqY368QQMicAtaSqzs8KJZgnYb9c7d0zgdAZHzu6qMQvRL5hajrn1n91CbOpbISD08qNLyrdkt-bFTWhAI4vMQFh6WeZu0fM4lFd2NcRwr3XPksINHaQ-G_xBniIqbw0Ls1jF44-csFCur-kEgU8awapJzKnqDKgw",
    "e": "AQAB",
    "alg": "RS256",
    "kid": "2011-04-29"
  })JSON")};
  const auto key{sourcemeta::core::JWK::from(document)};
  EXPECT_TRUE(key.has_value());
  const auto thumbprint{key.value().thumbprint()};
  EXPECT_TRUE(thumbprint.has_value());
  EXPECT_EQ(thumbprint.value(), "NzbLsXh8uDCcd-6MNwXF4W_7noWXFZAfHkxZsRGC9Xs");
}

TEST(rsa_thumbprint_normalizes_a_non_minimal_exponent) {
  // The exponent carries a spurious leading zero octet (AAEAAQ decodes to
  // 00 01 00 01), which must normalize to the same thumbprint as AQAB
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "kty": "RSA",
    "n": "0vx7agoebGcQSuuPiLJXZptN9nndrQmbXEps2aiAFbWhM78LhWx4cbbfAAtVT86zwu1RK7aPFFxuhDR1L6tSoc_BJECPebWKRXjBZCiFV4n3oknjhMstn64tZ_2W-5JsGY4Hc5n9yBXArwl93lqt7_RN5w6Cf0h4QyQ5v-65YGjQR0_FDW2QvzqY368QQMicAtaSqzs8KJZgnYb9c7d0zgdAZHzu6qMQvRL5hajrn1n91CbOpbISD08qNLyrdkt-bFTWhAI4vMQFh6WeZu0fM4lFd2NcRwr3XPksINHaQ-G_xBniIqbw0Ls1jF44-csFCur-kEgU8awapJzKnqDKgw",
    "e": "AAEAAQ"
  })JSON")};
  const auto key{sourcemeta::core::JWK::from(document)};
  EXPECT_TRUE(key.has_value());
  const auto thumbprint{key.value().thumbprint()};
  EXPECT_TRUE(thumbprint.has_value());
  EXPECT_EQ(thumbprint.value(), "NzbLsXh8uDCcd-6MNwXF4W_7noWXFZAfHkxZsRGC9Xs");
}

TEST(rsa_thumbprint_normalizes_a_non_minimal_modulus) {
  // The modulus carries a spurious leading zero octet, which must normalize to
  // the same thumbprint as the canonical encoding of RFC 7638 Section 3.1
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "kty": "RSA",
    "n": "ANL8e2oKHmxnEErrj4iyV2abTfZ53a0Jm1xKbNmogBW1oTO_C4VseHG23wALVU_Os8LtUSu2jxRcboQ0dS-rUqHPwSRAj3m1ikV4wWQohVeJ96JJ44TLLZ-uLWf9lvuSbBmOB3OZ_cgVwK8Jfd5are_0TecOgn9IeEMkOb_uuWBo0EdPxQ1tkL86mN-vEEDInALWkqs7PCiWYJ2G_XO3dM4HQGR87uqjEL0S-YWo659Z_dQmzqWyEg9PKjS8q3ZLfmxU1oQCOLzEBYelnmbtHzOJRXdjXEcK91z5LCDR2kPhv8QZ4iKm8NC7NYxeOPnLBQrq_pBIFPGsGqScyp6gyoM",
    "e": "AQAB"
  })JSON")};
  const auto key{sourcemeta::core::JWK::from(document)};
  EXPECT_TRUE(key.has_value());
  const auto thumbprint{key.value().thumbprint()};
  EXPECT_TRUE(thumbprint.has_value());
  EXPECT_EQ(thumbprint.value(), "NzbLsXh8uDCcd-6MNwXF4W_7noWXFZAfHkxZsRGC9Xs");
}

TEST(public_jwk_of_an_ec_public_key_round_trips) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "kty": "EC", "crv": "P-256",
    "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
    "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM"
  })JSON")};
  const auto key{sourcemeta::core::JWK::from(document)};
  EXPECT_TRUE(key.has_value());
  const auto serialized{key.value().public_jwk()};
  EXPECT_TRUE(serialized.has_value());
  EXPECT_EQ(serialized.value(), document);
}

TEST(public_jwk_of_an_ec_private_key_drops_the_private_key) {
  const auto private_document{sourcemeta::core::parse_json(R"JSON({
    "kty": "EC", "crv": "P-256",
    "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
    "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
    "d": "870MB6gfuTJ4HtUnUvYMyJpr5eUZNP4Bk43bVdj3eAE"
  })JSON")};
  const auto public_document{sourcemeta::core::parse_json(R"JSON({
    "kty": "EC", "crv": "P-256",
    "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
    "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM"
  })JSON")};
  const auto key{sourcemeta::core::JWKPrivate::from(private_document)};
  EXPECT_TRUE(key.has_value());
  const auto serialized{key.value().public_jwk()};
  EXPECT_TRUE(serialized.has_value());
  EXPECT_EQ(serialized.value(), public_document);
}

TEST(ec_private_and_public_keys_share_a_thumbprint) {
  const auto private_document{sourcemeta::core::parse_json(R"JSON({
    "kty": "EC", "crv": "P-256",
    "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
    "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
    "d": "870MB6gfuTJ4HtUnUvYMyJpr5eUZNP4Bk43bVdj3eAE"
  })JSON")};
  const auto public_document{sourcemeta::core::parse_json(R"JSON({
    "kty": "EC", "crv": "P-256",
    "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
    "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM"
  })JSON")};
  const auto private_key{sourcemeta::core::JWKPrivate::from(private_document)};
  const auto public_key{sourcemeta::core::JWK::from(public_document)};
  EXPECT_TRUE(private_key.has_value());
  EXPECT_TRUE(public_key.has_value());
  EXPECT_EQ(private_key.value().thumbprint(), public_key.value().thumbprint());
}

TEST(from_rejects_an_ec_private_key_whose_public_does_not_match) {
  // The final base64url character of x carries significant bits, so the decoded
  // public point no longer corresponds to the private scalar d and the keypair
  // is inconsistent
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "kty": "EC", "crv": "P-256",
    "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7Dw",
    "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
    "d": "870MB6gfuTJ4HtUnUvYMyJpr5eUZNP4Bk43bVdj3eAE"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(from_accepts_a_consistent_ec_private_key) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "kty": "EC", "crv": "P-256",
    "x": "MKBCTNIcKUSDii11ySs3526iDZ8AiTo7Tu6KPAqv7D4",
    "y": "4Etl6SRW2YiLUrN5vfvVHuhp7x8PxltmWWlbbM4IFyM",
    "d": "870MB6gfuTJ4HtUnUvYMyJpr5eUZNP4Bk43bVdj3eAE"
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::JWKPrivate::from(document).has_value());
}

TEST(public_jwk_of_a_symmetric_key_is_absent) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "kty": "oct", "k": "c2VjcmV0LWtleS12YWx1ZQ" })JSON")};
  const auto key{sourcemeta::core::JWK::from(document)};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(key.value().public_jwk().has_value());
}

TEST(thumbprint_of_a_symmetric_key_hashes_the_secret) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "kty": "oct", "k": "c2VjcmV0LWtleS12YWx1ZQ" })JSON")};
  const auto key{sourcemeta::core::JWK::from(document)};
  EXPECT_TRUE(key.has_value());
  const auto thumbprint{key.value().thumbprint()};
  EXPECT_TRUE(thumbprint.has_value());
  EXPECT_EQ(thumbprint.value(), "piCdA_rPHG0sBzkvGUiX6uQMVeusQOdzxKHSfyoeZ3A");
}

TEST(pem_rsa_private_key_thumbprint_matches_its_public_jwk) {
  const auto key{sourcemeta::core::JWKPrivate::from_pem(RSA_PRIVATE_KEY)};
  EXPECT_TRUE(key.has_value());
  const auto thumbprint{key.value().thumbprint()};
  EXPECT_TRUE(thumbprint.has_value());
  const auto serialized{key.value().public_jwk()};
  EXPECT_TRUE(serialized.has_value());
  const auto public_key{sourcemeta::core::JWK::from(serialized.value())};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_EQ(public_key.value().thumbprint(), thumbprint);
}

TEST(pem_ec_private_key_thumbprint_matches_its_public_jwk) {
  const std::string pem{
      "-----BEGIN PRIVATE KEY-----\n"
      "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgUJlZcx3akefhQkHy\n"
      "hdm0LyliIIFyGpXJmWGGe30hqBChRANCAAQrQCBl4pxLtKex84zvju8dntvZF1Dm\n"
      "etJQhjqZwh81vCWJlgdqoA+F3pQN4jrapP2//GNWJ1pM+Yd7xZoDhKZI\n"
      "-----END PRIVATE KEY-----\n"};
  const auto key{sourcemeta::core::JWKPrivate::from_pem(pem)};
  EXPECT_TRUE(key.has_value());
  const auto thumbprint{key.value().thumbprint()};
  EXPECT_TRUE(thumbprint.has_value());
  const auto serialized{key.value().public_jwk()};
  EXPECT_TRUE(serialized.has_value());
  const auto public_key{sourcemeta::core::JWK::from(serialized.value())};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_EQ(public_key.value().thumbprint(), thumbprint);
}
