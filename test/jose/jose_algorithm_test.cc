#include <gtest/gtest.h>

#include <sourcemeta/core/jose.h>

TEST(JOSE_algorithm, rs256) {
  const auto result{sourcemeta::core::to_jws_algorithm("RS256")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::RS256);
}

TEST(JOSE_algorithm, ps384) {
  const auto result{sourcemeta::core::to_jws_algorithm("PS384")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::PS384);
}

TEST(JOSE_algorithm, es512) {
  const auto result{sourcemeta::core::to_jws_algorithm("ES512")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::JWSAlgorithm::ES512);
}

TEST(JOSE_algorithm, rejects_none) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("none").has_value());
}

TEST(JOSE_algorithm, rejects_hmac) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("HS256").has_value());
}

TEST(JOSE_algorithm, rejects_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("RS128").has_value());
}

TEST(JOSE_algorithm, rejects_empty) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("").has_value());
}
