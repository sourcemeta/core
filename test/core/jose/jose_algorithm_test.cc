#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

#include <cstdint> // std::uint16_t

TEST(rs256) {
  const auto result{sourcemeta::core::to_jws_algorithm("RS256")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::RS256);
}

TEST(ps384) {
  const auto result{sourcemeta::core::to_jws_algorithm("PS384")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::PS384);
}

TEST(es512) {
  const auto result{sourcemeta::core::to_jws_algorithm("ES512")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::ES512);
}

TEST(eddsa) {
  const auto result{sourcemeta::core::to_jws_algorithm("EdDSA")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::EdDSA);
}

TEST(rejects_none) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("none").has_value());
}

TEST(rejects_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("RS128").has_value());
}

TEST(rejects_empty) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("").has_value());
}

TEST(to_jws_algorithm_hs256) {
  const auto result{sourcemeta::core::to_jws_algorithm("HS256")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::HS256);
}

TEST(to_jws_algorithm_hs384) {
  const auto result{sourcemeta::core::to_jws_algorithm("HS384")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::HS384);
}

TEST(to_jws_algorithm_hs512) {
  const auto result{sourcemeta::core::to_jws_algorithm("HS512")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::HS512);
}

TEST(jws_algorithm_name_covers_every_algorithm) {
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::RS256),
            "RS256");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::RS384),
            "RS384");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::RS512),
            "RS512");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::PS256),
            "PS256");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::PS384),
            "PS384");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::PS512),
            "PS512");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::ES256),
            "ES256");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::ES384),
            "ES384");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::ES512),
            "ES512");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::EdDSA),
            "EdDSA");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::HS256),
            "HS256");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::HS384),
            "HS384");
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::HS512),
            "HS512");
}

TEST(jws_algorithm_name_round_trips) {
  EXPECT_EQ(sourcemeta::core::to_jws_algorithm(
                sourcemeta::core::jws_algorithm_name(
                    sourcemeta::core::JWSAlgorithm::PS384))
                .value(),
            sourcemeta::core::JWSAlgorithm::PS384);
}

TEST(jws_algorithm_is_asymmetric_covers_every_algorithm) {
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::RS256));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::RS384));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::RS512));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::PS256));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::PS384));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::PS512));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::ES256));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::ES384));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::ES512));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::EdDSA));
  EXPECT_FALSE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::HS256));
  EXPECT_FALSE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::HS384));
  EXPECT_FALSE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::HS512));
}

TEST(jws_algorithm_digest_bits_256_family) {
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::RS256),
            std::uint16_t{256});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::PS256),
            std::uint16_t{256});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::ES256),
            std::uint16_t{256});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::HS256),
            std::uint16_t{256});
}

TEST(jws_algorithm_digest_bits_384_family) {
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::RS384),
            std::uint16_t{384});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::PS384),
            std::uint16_t{384});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::ES384),
            std::uint16_t{384});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::HS384),
            std::uint16_t{384});
}

TEST(jws_algorithm_digest_bits_512_family) {
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::RS512),
            std::uint16_t{512});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::PS512),
            std::uint16_t{512});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::ES512),
            std::uint16_t{512});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::HS512),
            std::uint16_t{512});
  EXPECT_EQ(sourcemeta::core::jws_algorithm_digest_bits(
                sourcemeta::core::JWSAlgorithm::EdDSA),
            std::uint16_t{512});
}

TEST(jwe_algorithm_round_trips) {
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("RSA-OAEP").value(),
            sourcemeta::core::JWEAlgorithm::RSA_OAEP);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::RSA_OAEP),
            "RSA-OAEP");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("RSA-OAEP-256").value(),
            sourcemeta::core::JWEAlgorithm::RSA_OAEP_256);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::RSA_OAEP_256),
            "RSA-OAEP-256");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("ECDH-ES").value(),
            sourcemeta::core::JWEAlgorithm::ECDH_ES);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::ECDH_ES),
            "ECDH-ES");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("ECDH-ES+A128KW").value(),
            sourcemeta::core::JWEAlgorithm::ECDH_ES_A128KW);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::ECDH_ES_A128KW),
            "ECDH-ES+A128KW");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("ECDH-ES+A192KW").value(),
            sourcemeta::core::JWEAlgorithm::ECDH_ES_A192KW);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::ECDH_ES_A192KW),
            "ECDH-ES+A192KW");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("ECDH-ES+A256KW").value(),
            sourcemeta::core::JWEAlgorithm::ECDH_ES_A256KW);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::ECDH_ES_A256KW),
            "ECDH-ES+A256KW");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("A128KW").value(),
            sourcemeta::core::JWEAlgorithm::A128KW);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::A128KW),
            "A128KW");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("A192KW").value(),
            sourcemeta::core::JWEAlgorithm::A192KW);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::A192KW),
            "A192KW");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("A256KW").value(),
            sourcemeta::core::JWEAlgorithm::A256KW);
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::A256KW),
            "A256KW");
  EXPECT_EQ(sourcemeta::core::to_jwe_algorithm("dir").value(),
            sourcemeta::core::JWEAlgorithm::DIR);
  EXPECT_EQ(
      sourcemeta::core::jwe_algorithm_name(sourcemeta::core::JWEAlgorithm::DIR),
      "dir");
}

TEST(jwe_algorithm_rejects_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_jwe_algorithm("A512KW").has_value());
  EXPECT_FALSE(sourcemeta::core::to_jwe_algorithm("").has_value());
}

TEST(jwe_encryption_round_trips) {
  EXPECT_EQ(sourcemeta::core::to_jwe_encryption("A128GCM").value(),
            sourcemeta::core::JWEEncryption::A128GCM);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_name(
                sourcemeta::core::JWEEncryption::A128GCM),
            "A128GCM");
  EXPECT_EQ(sourcemeta::core::to_jwe_encryption("A192GCM").value(),
            sourcemeta::core::JWEEncryption::A192GCM);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_name(
                sourcemeta::core::JWEEncryption::A192GCM),
            "A192GCM");
  EXPECT_EQ(sourcemeta::core::to_jwe_encryption("A256GCM").value(),
            sourcemeta::core::JWEEncryption::A256GCM);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_name(
                sourcemeta::core::JWEEncryption::A256GCM),
            "A256GCM");
  EXPECT_EQ(sourcemeta::core::to_jwe_encryption("A128CBC-HS256").value(),
            sourcemeta::core::JWEEncryption::A128CBC_HS256);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_name(
                sourcemeta::core::JWEEncryption::A128CBC_HS256),
            "A128CBC-HS256");
  EXPECT_EQ(sourcemeta::core::to_jwe_encryption("A192CBC-HS384").value(),
            sourcemeta::core::JWEEncryption::A192CBC_HS384);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_name(
                sourcemeta::core::JWEEncryption::A192CBC_HS384),
            "A192CBC-HS384");
  EXPECT_EQ(sourcemeta::core::to_jwe_encryption("A256CBC-HS512").value(),
            sourcemeta::core::JWEEncryption::A256CBC_HS512);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_name(
                sourcemeta::core::JWEEncryption::A256CBC_HS512),
            "A256CBC-HS512");
}

TEST(jwe_encryption_rejects_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_jwe_encryption("A512GCM").has_value());
  EXPECT_FALSE(sourcemeta::core::to_jwe_encryption("").has_value());
}

TEST(jwe_algorithm_asymmetry) {
  EXPECT_TRUE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::RSA_OAEP));
  EXPECT_TRUE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::RSA_OAEP_256));
  EXPECT_TRUE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::ECDH_ES));
  EXPECT_TRUE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::ECDH_ES_A128KW));
  EXPECT_TRUE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::ECDH_ES_A192KW));
  EXPECT_TRUE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::ECDH_ES_A256KW));
  EXPECT_FALSE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::A128KW));
  EXPECT_FALSE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::A192KW));
  EXPECT_FALSE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::A256KW));
  EXPECT_FALSE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::DIR));
}

TEST(jwe_encryption_key_sizes) {
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A128GCM),
            16);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A192GCM),
            24);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A256GCM),
            32);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A128CBC_HS256),
            32);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A192CBC_HS384),
            48);
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A256CBC_HS512),
            64);
}

TEST(to_jws_algorithm_rs512) {
  EXPECT_EQ(sourcemeta::core::to_jws_algorithm("RS512").value(),
            sourcemeta::core::JWSAlgorithm::RS512);
}

TEST(to_jws_algorithm_ps512) {
  EXPECT_EQ(sourcemeta::core::to_jws_algorithm("PS512").value(),
            sourcemeta::core::JWSAlgorithm::PS512);
}
