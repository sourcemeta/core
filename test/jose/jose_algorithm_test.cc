#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

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

TEST(jws_algorithm_name_rs256) {
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::RS256),
            "RS256");
}

TEST(jws_algorithm_name_es256) {
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::ES256),
            "ES256");
}

TEST(jws_algorithm_name_eddsa) {
  EXPECT_EQ(sourcemeta::core::jws_algorithm_name(
                sourcemeta::core::JWSAlgorithm::EdDSA),
            "EdDSA");
}

TEST(jws_algorithm_name_hs512) {
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

TEST(jws_algorithm_is_asymmetric_ec) {
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::ES256));
}

TEST(jws_algorithm_is_asymmetric_rsa) {
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::RS256));
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::PS512));
}

TEST(jws_algorithm_is_asymmetric_eddsa) {
  EXPECT_TRUE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::EdDSA));
}

TEST(jws_algorithm_is_asymmetric_rejects_mac) {
  EXPECT_FALSE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::HS256));
  EXPECT_FALSE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::HS384));
  EXPECT_FALSE(sourcemeta::core::jws_algorithm_is_asymmetric(
      sourcemeta::core::JWSAlgorithm::HS512));
}
