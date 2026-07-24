#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

#include <cstddef> // std::size_t

TEST(to_jwe_algorithm_rsa_oaep) {
  const auto result{sourcemeta::core::to_jwe_algorithm("RSA-OAEP")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWEAlgorithm::RSA_OAEP);
}

TEST(to_jwe_algorithm_ecdh_es_a256kw) {
  const auto result{sourcemeta::core::to_jwe_algorithm("ECDH-ES+A256KW")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWEAlgorithm::ECDH_ES_A256KW);
}

TEST(to_jwe_algorithm_dir) {
  const auto result{sourcemeta::core::to_jwe_algorithm("dir")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWEAlgorithm::DIR);
}

TEST(to_jwe_algorithm_rejects_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_jwe_algorithm("RSA1_5").has_value());
}

TEST(to_jwe_algorithm_rejects_a_content_encryption_name) {
  EXPECT_FALSE(sourcemeta::core::to_jwe_algorithm("A128GCM").has_value());
}

TEST(jwe_algorithm_name_rsa_oaep_256) {
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::RSA_OAEP_256),
            "RSA-OAEP-256");
}

TEST(jwe_algorithm_name_ecdh_es) {
  EXPECT_EQ(sourcemeta::core::jwe_algorithm_name(
                sourcemeta::core::JWEAlgorithm::ECDH_ES),
            "ECDH-ES");
}

TEST(jwe_algorithm_name_dir) {
  EXPECT_EQ(
      sourcemeta::core::jwe_algorithm_name(sourcemeta::core::JWEAlgorithm::DIR),
      "dir");
}

TEST(to_jwe_encryption_a128gcm) {
  const auto result{sourcemeta::core::to_jwe_encryption("A128GCM")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWEEncryption::A128GCM);
}

TEST(to_jwe_encryption_a256cbc_hs512) {
  const auto result{sourcemeta::core::to_jwe_encryption("A256CBC-HS512")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWEEncryption::A256CBC_HS512);
}

TEST(to_jwe_encryption_rejects_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_jwe_encryption("A64GCM").has_value());
}

TEST(jwe_encryption_name_a192gcm) {
  EXPECT_EQ(sourcemeta::core::jwe_encryption_name(
                sourcemeta::core::JWEEncryption::A192GCM),
            "A192GCM");
}

TEST(jwe_encryption_name_a128cbc_hs256) {
  EXPECT_EQ(sourcemeta::core::jwe_encryption_name(
                sourcemeta::core::JWEEncryption::A128CBC_HS256),
            "A128CBC-HS256");
}

TEST(jwe_algorithm_is_asymmetric_rsa_oaep) {
  EXPECT_TRUE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::RSA_OAEP));
}

TEST(jwe_algorithm_is_asymmetric_ecdh_es_a128kw) {
  EXPECT_TRUE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::ECDH_ES_A128KW));
}

TEST(jwe_algorithm_is_asymmetric_a128kw) {
  EXPECT_FALSE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::A128KW));
}

TEST(jwe_algorithm_is_asymmetric_dir) {
  EXPECT_FALSE(sourcemeta::core::jwe_algorithm_is_asymmetric(
      sourcemeta::core::JWEAlgorithm::DIR));
}

TEST(jwe_encryption_key_bytes_a128gcm) {
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A128GCM),
            std::size_t{16});
}

TEST(jwe_encryption_key_bytes_a256gcm) {
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A256GCM),
            std::size_t{32});
}

TEST(jwe_encryption_key_bytes_a128cbc_hs256) {
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A128CBC_HS256),
            std::size_t{32});
}

TEST(jwe_encryption_key_bytes_a256cbc_hs512) {
  EXPECT_EQ(sourcemeta::core::jwe_encryption_key_bytes(
                sourcemeta::core::JWEEncryption::A256CBC_HS512),
            std::size_t{64});
}
