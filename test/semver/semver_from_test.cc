#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/test.h>

TEST(valid_basic) {
  const auto result = sourcemeta::core::SemVer::from("1.2.3");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 2);
  EXPECT_EQ(result->patch(), 3);
  EXPECT_TRUE(result->pre_release().empty());
  EXPECT_TRUE(result->build().empty());
}

TEST(valid_zero) {
  const auto result = sourcemeta::core::SemVer::from("0.0.0");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 0);
  EXPECT_EQ(result->minor(), 0);
  EXPECT_EQ(result->patch(), 0);
}

TEST(valid_pre_release) {
  const auto result = sourcemeta::core::SemVer::from("1.0.0-alpha.1");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->pre_release(), "alpha.1");
}

TEST(valid_build) {
  const auto result = sourcemeta::core::SemVer::from("1.0.0+build.123");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->build(), "build.123");
}

TEST(valid_pre_release_and_build) {
  const auto result =
      sourcemeta::core::SemVer::from("1.0.0-beta+exp.sha.5114f85");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->pre_release(), "beta");
  EXPECT_EQ(result->build(), "exp.sha.5114f85");
}

TEST(valid_large_numbers) {
  const auto result =
      sourcemeta::core::SemVer::from("999999999.999999999.999999999");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 999999999);
  EXPECT_EQ(result->minor(), 999999999);
  EXPECT_EQ(result->patch(), 999999999);
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("").has_value());
}

TEST(invalid_v_prefix) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("v1.2.3").has_value());
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(" 1.2.3").has_value());
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3 ").has_value());
}

TEST(invalid_major_only) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1").has_value());
}

TEST(invalid_major_minor_only) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2").has_value());
}

TEST(invalid_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("01.2.3").has_value());
}

TEST(invalid_pre_release_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3-01").has_value());
}

TEST(invalid_empty_pre_release) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3-").has_value());
}

TEST(invalid_empty_build) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3+").has_value());
}

TEST(invalid_text) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("not-a-version").has_value());
}

TEST(invalid_four_components) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3.4").has_value());
}

TEST(invalid_illegal_char_in_pre_release) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3-alpha#").has_value());
}

TEST(invalid_underscore_in_build) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("1.2.3+build_1").has_value());
}

TEST(result_compares_correctly) {
  const auto left = sourcemeta::core::SemVer::from("1.0.0-alpha");
  const auto right = sourcemeta::core::SemVer::from("1.0.0");
  EXPECT_TRUE(left.has_value());
  EXPECT_TRUE(right.has_value());
  EXPECT_LT(*left, *right);
}

TEST(result_to_string) {
  const auto result = sourcemeta::core::SemVer::from("1.2.3-beta+build");
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->to_string(), "1.2.3-beta+build");
}

TEST(invalid_not_valid) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from("not valid").has_value());
}
