#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

// The reference access token and its hashes, from the coreos/go-oidc test
// suite. The RS256 value is a Google-issued production value.
static constexpr std::string_view REFERENCE_TOKEN{
    "ya29.CjHSA1l5WUn8xZ6HanHFzzdHdbXm-"
    "14rxnC7JHch9eFIsZkQEGoWzaYG4o7k5f6BnPLj"};

TEST(token_hash_rs256) {
  const auto hash{sourcemeta::core::oidc_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::RS256)};
  EXPECT_TRUE(hash.has_value());
  EXPECT_EQ(hash.value(), "piwt8oCH-K2D9pXlaS1Y-w");
}

TEST(token_hash_rejects_eddsa) {
  // The correct digest for EdDSA depends on the signing curve, which the
  // algorithm alone does not convey, so it is rejected rather than guessed
  EXPECT_FALSE(sourcemeta::core::oidc_token_hash(
                   REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::EdDSA)
                   .has_value());
}

TEST(token_hash_es384) {
  const auto hash{sourcemeta::core::oidc_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::ES384)};
  EXPECT_TRUE(hash.has_value());
  EXPECT_EQ(hash.value(), "_ILKVQjbEzFKNJjUKC2kz9eReYi0A9Of");
}

TEST(token_hash_ps512) {
  const auto hash{sourcemeta::core::oidc_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::PS512)};
  EXPECT_TRUE(hash.has_value());
  EXPECT_EQ(hash.value(), "Spa_APgwBrarSeQbxI-rbragXho6dqFpH5x9PqaPfUI");
}

TEST(token_hash_es256_matches_rs256_by_digest) {
  const auto rs256{sourcemeta::core::oidc_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::RS256)};
  const auto es256{sourcemeta::core::oidc_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::ES256)};
  EXPECT_TRUE(rs256.has_value());
  EXPECT_TRUE(es256.has_value());
  EXPECT_EQ(rs256.value(), es256.value());
}

TEST(verify_token_hash_accepts_a_matching_claim) {
  EXPECT_TRUE(sourcemeta::core::oidc_verify_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::RS256,
      "piwt8oCH-K2D9pXlaS1Y-w"));
}

TEST(verify_token_hash_rejects_a_mismatched_claim) {
  EXPECT_FALSE(sourcemeta::core::oidc_verify_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::RS256,
      "Spa_APgwBrarSeQbxI-rbragXho6dqFpH5x9PqaPfUI"));
}

TEST(verify_token_hash_rejects_a_wrong_algorithm) {
  EXPECT_FALSE(sourcemeta::core::oidc_verify_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::ES384,
      "piwt8oCH-K2D9pXlaS1Y-w"));
}

TEST(verify_token_hash_rejects_an_empty_claim) {
  EXPECT_FALSE(sourcemeta::core::oidc_verify_token_hash(
      REFERENCE_TOKEN, sourcemeta::core::JWSAlgorithm::RS256, ""));
}

TEST(token_hash_of_a_code_uses_the_same_function) {
  const std::string_view code{
      "Qcb0Orv1zh30vL1MPRsbm-diHiMwcLyZvn1arpZv-Jxf_11jnpEX3Tgfvk"};
  const auto hash{sourcemeta::core::oidc_token_hash(
      code, sourcemeta::core::JWSAlgorithm::RS256)};
  EXPECT_TRUE(hash.has_value());
  EXPECT_EQ(hash.value(), "LDktKdoQak3Pk0cnXxCltA");
}
