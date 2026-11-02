#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/test.h>

TEST(basic) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.2.3"}.to_string(), "1.2.3");
}

TEST(zero) {
  EXPECT_EQ(sourcemeta::core::SemVer{"0.0.0"}.to_string(), "0.0.0");
}

TEST(large_numbers) {
  EXPECT_EQ(
      sourcemeta::core::SemVer{"999999999.999999999.999999999"}.to_string(),
      "999999999.999999999.999999999");
}

TEST(pre_release) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-alpha"}.to_string(), "1.0.0-alpha");
}

TEST(pre_release_dotted) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-alpha.1"}.to_string(),
            "1.0.0-alpha.1");
}

TEST(pre_release_numeric) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-0.3.7"}.to_string(), "1.0.0-0.3.7");
}

TEST(build) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0+build"}.to_string(), "1.0.0+build");
}

TEST(build_dotted) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0+build.1.2.3"}.to_string(),
            "1.0.0+build.1.2.3");
}

TEST(build_leading_zeros) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0+001"}.to_string(), "1.0.0+001");
}

TEST(pre_release_and_build) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-beta+exp.sha.5114f85"}.to_string(),
            "1.0.0-beta+exp.sha.5114f85");
}

TEST(pre_release_rc_and_build) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-rc.1+build.123"}.to_string(),
            "1.0.0-rc.1+build.123");
}

TEST(complex) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.2.3-alpha.1.beta.2+build.42.sha.abc"}
                .to_string(),
            "1.2.3-alpha.1.beta.2+build.42.sha.abc");
}

TEST(pre_release_hyphens) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0---"}.to_string(), "1.0.0---");
}

TEST(pre_release_DEV_SNAPSHOT) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-DEV-SNAPSHOT"}.to_string(),
            "1.0.0-DEV-SNAPSHOT");
}

TEST(node_semver_pre_release_0) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.2.3-0"}.to_string(), "1.2.3-0");
}

TEST(node_semver_pre_release_1a) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.2.3-1a"}.to_string(), "1.2.3-1a");
}

TEST(node_semver_pre_release_alpha_hyphen_dot_hyphen_beta) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.2.3-alpha-.-beta"}.to_string(),
            "1.2.3-alpha-.-beta");
}

TEST(node_semver_build_new_hyphen_build) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.2.3+new-build"}.to_string(),
            "1.2.3+new-build");
}

TEST(node_semver_pre_release_and_build) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.2.3-alpha+build"}.to_string(),
            "1.2.3-alpha+build");
}

TEST(spec_item_9_example) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-x-y-z.--"}.to_string(),
            "1.0.0-x-y-z.--");
}

TEST(spec_item_10_build_hex) {
  EXPECT_EQ(
      sourcemeta::core::SemVer{"1.0.0+21AF26D3----117B344092BD"}.to_string(),
      "1.0.0+21AF26D3----117B344092BD");
}

TEST(build_single_hyphen) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0+-"}.to_string(), "1.0.0+-");
}

TEST(build_leading_zeros_007) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0+007"}.to_string(), "1.0.0+007");
}
