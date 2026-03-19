#include <gtest/gtest.h>

#include <sourcemeta/core/semver.h>

TEST(SemVer_from, valid_basic) {
  const auto result = sourcemeta::core::SemVer::from("1.2.3");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 2);
  EXPECT_EQ(result->patch(), 3);
  EXPECT_TRUE(result->pre_release().empty());
  EXPECT_TRUE(result->build().empty());
}

TEST(SemVer_from, valid_zero) {
  const auto result = sourcemeta::core::SemVer::from("0.0.0");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 0);
  EXPECT_EQ(result->minor(), 0);
  EXPECT_EQ(result->patch(), 0);
}

TEST(SemVer_from, valid_pre_release) {
  const auto result = sourcemeta::core::SemVer::from("1.0.0-alpha.1");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->pre_release(), "alpha.1");
}

TEST(SemVer_from, valid_build) {
  const auto result = sourcemeta::core::SemVer::from("1.0.0+build.123");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->build(), "build.123");
}

TEST(SemVer_from, valid_pre_release_and_build) {
  const auto result =
      sourcemeta::core::SemVer::from("1.0.0-beta+exp.sha.5114f85");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->pre_release(), "beta");
  EXPECT_EQ(result->build(), "exp.sha.5114f85");
}

TEST(SemVer_from, valid_large_numbers) {
  const auto result =
      sourcemeta::core::SemVer::from("999999999.999999999.999999999");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 999999999);
  EXPECT_EQ(result->minor(), 999999999);
  EXPECT_EQ(result->patch(), 999999999);
}

TEST(SemVer_from, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("").has_value());
}

TEST(SemVer_from, invalid_v_prefix) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("v1.2.3").has_value());
}

TEST(SemVer_from, invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(" 1.2.3").has_value());
}

TEST(SemVer_from, invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3 ").has_value());
}

TEST(SemVer_from, invalid_major_only) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1").has_value());
}

TEST(SemVer_from, invalid_major_minor_only) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2").has_value());
}

TEST(SemVer_from, invalid_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("01.2.3").has_value());
}

TEST(SemVer_from, invalid_pre_release_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3-01").has_value());
}

TEST(SemVer_from, invalid_empty_pre_release) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3-").has_value());
}

TEST(SemVer_from, invalid_empty_build) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3+").has_value());
}

TEST(SemVer_from, invalid_text) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("not-a-version").has_value());
}

TEST(SemVer_from, invalid_four_components) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3.4").has_value());
}

TEST(SemVer_from, invalid_illegal_char_in_pre_release) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3-alpha#").has_value());
}

TEST(SemVer_from, invalid_underscore_in_build) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3+build_1").has_value());
}

TEST(SemVer_from, result_compares_correctly) {
  const auto left = sourcemeta::core::SemVer::from("1.0.0-alpha");
  const auto right = sourcemeta::core::SemVer::from("1.0.0");
  EXPECT_TRUE(left.has_value());
  EXPECT_TRUE(right.has_value());
  EXPECT_LT(*left, *right);
}

TEST(SemVer_from, result_to_string) {
  const auto result = sourcemeta::core::SemVer::from("1.2.3-beta+build");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->to_string(), "1.2.3-beta+build");
}

TEST(SemVer_from, invalid_not_valid) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("not valid").has_value());
}
