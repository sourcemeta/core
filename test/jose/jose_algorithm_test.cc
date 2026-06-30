#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jose.h>

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

TEST(rejects_hmac) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("HS256").has_value());
}

TEST(rejects_unknown) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("RS128").has_value());
}

TEST(rejects_empty) {
  EXPECT_FALSE(sourcemeta::core::to_jws_algorithm("").has_value());
}
