#include <gtest/gtest.h>

#include <sourcemeta/core/semver.h>

TEST(SemVer_parse, zero_zero_zero) {
  const sourcemeta::core::SemVer version{"0.0.0"};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_TRUE(version.pre_release().empty());
  EXPECT_TRUE(version.build().empty());
}

TEST(SemVer_parse, one_zero_zero) {
  const sourcemeta::core::SemVer version{"1.0.0"};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse, zero_one_zero) {
  const sourcemeta::core::SemVer version{"0.1.0"};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 1);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse, zero_zero_one) {
  const sourcemeta::core::SemVer version{"0.0.1"};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 1);
}

TEST(SemVer_parse, one_two_three) {
  const sourcemeta::core::SemVer version{"1.2.3"};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
}

TEST(SemVer_parse, multi_digit) {
  const sourcemeta::core::SemVer version{"10.20.30"};
  EXPECT_EQ(version.major(), 10);
  EXPECT_EQ(version.minor(), 20);
  EXPECT_EQ(version.patch(), 30);
}

TEST(SemVer_parse, large_numbers) {
  const sourcemeta::core::SemVer version{"999999999.999999999.999999999"};
  EXPECT_EQ(version.major(), 999999999);
  EXPECT_EQ(version.minor(), 999999999);
  EXPECT_EQ(version.patch(), 999999999);
}

TEST(SemVer_parse, one_one_two) {
  const sourcemeta::core::SemVer version{"1.1.2"};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 1);
  EXPECT_EQ(version.patch(), 2);
}

TEST(SemVer_parse, max_minor) {
  const sourcemeta::core::SemVer version{"0.99.0"};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 99);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse, pre_release_alpha) {
  const sourcemeta::core::SemVer version{"1.0.0-alpha"};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "alpha");
  EXPECT_TRUE(version.build().empty());
}

TEST(SemVer_parse, pre_release_alpha_1) {
  const sourcemeta::core::SemVer version{"1.0.0-alpha.1"};
  EXPECT_EQ(version.pre_release(), "alpha.1");
}

TEST(SemVer_parse, pre_release_0_3_7) {
  const sourcemeta::core::SemVer version{"1.0.0-0.3.7"};
  EXPECT_EQ(version.pre_release(), "0.3.7");
}

TEST(SemVer_parse, pre_release_x_7_z_92) {
  const sourcemeta::core::SemVer version{"1.0.0-x.7.z.92"};
  EXPECT_EQ(version.pre_release(), "x.7.z.92");
}

TEST(SemVer_parse, pre_release_beta) {
  const sourcemeta::core::SemVer version{"1.0.0-beta"};
  EXPECT_EQ(version.pre_release(), "beta");
}

TEST(SemVer_parse, pre_release_beta_2) {
  const sourcemeta::core::SemVer version{"1.0.0-beta.2"};
  EXPECT_EQ(version.pre_release(), "beta.2");
}

TEST(SemVer_parse, pre_release_beta_11) {
  const sourcemeta::core::SemVer version{"1.0.0-beta.11"};
  EXPECT_EQ(version.pre_release(), "beta.11");
}

TEST(SemVer_parse, pre_release_rc_1) {
  const sourcemeta::core::SemVer version{"1.0.0-rc.1"};
  EXPECT_EQ(version.pre_release(), "rc.1");
}

TEST(SemVer_parse, pre_release_alpha_beta) {
  const sourcemeta::core::SemVer version{"1.0.0-alpha.beta"};
  EXPECT_EQ(version.pre_release(), "alpha.beta");
}

TEST(SemVer_parse, pre_release_single_zero) {
  const sourcemeta::core::SemVer version{"1.0.0-0"};
  EXPECT_EQ(version.pre_release(), "0");
}

TEST(SemVer_parse, pre_release_single_digit) {
  const sourcemeta::core::SemVer version{"1.0.0-1"};
  EXPECT_EQ(version.pre_release(), "1");
}

TEST(SemVer_parse, pre_release_hyphen_in_identifier) {
  const sourcemeta::core::SemVer version{"1.0.0-alpha-beta"};
  EXPECT_EQ(version.pre_release(), "alpha-beta");
}

TEST(SemVer_parse, pre_release_with_hyphen_and_digits) {
  const sourcemeta::core::SemVer version{"1.0.0-alpha-1"};
  EXPECT_EQ(version.pre_release(), "alpha-1");
}

TEST(SemVer_parse, pre_release_0alpha) {
  const sourcemeta::core::SemVer version{"1.0.0-0alpha"};
  EXPECT_EQ(version.pre_release(), "0alpha");
}

TEST(SemVer_parse, pre_release_alpha0valid) {
  const sourcemeta::core::SemVer version{"1.0.0-alpha.0valid"};
  EXPECT_EQ(version.pre_release(), "alpha.0valid");
}

TEST(SemVer_parse, pre_release_many_identifiers) {
  const sourcemeta::core::SemVer version{"1.0.0-a.b.c.d.e.f"};
  EXPECT_EQ(version.pre_release(), "a.b.c.d.e.f");
}

TEST(SemVer_parse, pre_release_all_hyphens) {
  const sourcemeta::core::SemVer version{"1.0.0---"};
  EXPECT_EQ(version.pre_release(), "--");
}

TEST(SemVer_parse, pre_release_hyphen_only) {
  const sourcemeta::core::SemVer version{"1.0.0--"};
  EXPECT_EQ(version.pre_release(), "-");
}

TEST(SemVer_parse, pre_release_RC_uppercase) {
  const sourcemeta::core::SemVer version{"1.0.0-RC.1"};
  EXPECT_EQ(version.pre_release(), "RC.1");
}

TEST(SemVer_parse, pre_release_DEV_SNAPSHOT) {
  const sourcemeta::core::SemVer version{"1.0.0-DEV-SNAPSHOT"};
  EXPECT_EQ(version.pre_release(), "DEV-SNAPSHOT");
}

TEST(SemVer_parse, pre_release_mixed_case) {
  const sourcemeta::core::SemVer version{"1.0.0-AlPhA.1"};
  EXPECT_EQ(version.pre_release(), "AlPhA.1");
}

TEST(SemVer_parse, build_only) {
  const sourcemeta::core::SemVer version{"1.0.0+build"};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_TRUE(version.pre_release().empty());
  EXPECT_EQ(version.build(), "build");
}

TEST(SemVer_parse, build_timestamp) {
  const sourcemeta::core::SemVer version{"1.0.0+20130313144700"};
  EXPECT_EQ(version.build(), "20130313144700");
}

TEST(SemVer_parse, build_dotted) {
  const sourcemeta::core::SemVer version{"1.0.0+build.1.2.3"};
  EXPECT_EQ(version.build(), "build.1.2.3");
}

TEST(SemVer_parse, build_sha) {
  const sourcemeta::core::SemVer version{"1.0.0+exp.sha.5114f85"};
  EXPECT_EQ(version.build(), "exp.sha.5114f85");
}

TEST(SemVer_parse, build_leading_zeros_allowed) {
  const sourcemeta::core::SemVer version{"1.0.0+001"};
  EXPECT_EQ(version.build(), "001");
}

TEST(SemVer_parse, build_leading_zeros_dotted) {
  const sourcemeta::core::SemVer version{"1.0.0+01.02.03"};
  EXPECT_EQ(version.build(), "01.02.03");
}

TEST(SemVer_parse, build_single_digit) {
  const sourcemeta::core::SemVer version{"1.0.0+1"};
  EXPECT_EQ(version.build(), "1");
}

TEST(SemVer_parse, build_with_hyphen) {
  const sourcemeta::core::SemVer version{"1.0.0+build-123"};
  EXPECT_EQ(version.build(), "build-123");
}

TEST(SemVer_parse, pre_release_and_build) {
  const sourcemeta::core::SemVer version{"1.0.0-beta+exp.sha.5114f85"};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "beta");
  EXPECT_EQ(version.build(), "exp.sha.5114f85");
}

TEST(SemVer_parse, pre_release_rc_and_build) {
  const sourcemeta::core::SemVer version{"1.0.0-rc.1+build.123"};
  EXPECT_EQ(version.pre_release(), "rc.1");
  EXPECT_EQ(version.build(), "build.123");
}

TEST(SemVer_parse, pre_release_alpha_and_build_001) {
  const sourcemeta::core::SemVer version{"1.0.0-alpha+001"};
  EXPECT_EQ(version.pre_release(), "alpha");
  EXPECT_EQ(version.build(), "001");
}

TEST(SemVer_parse, complex_pre_release_and_build) {
  const sourcemeta::core::SemVer version{
      "1.2.3-alpha.1.beta.2+build.42.sha.abc"};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
  EXPECT_EQ(version.pre_release(), "alpha.1.beta.2");
  EXPECT_EQ(version.build(), "build.42.sha.abc");
}

TEST(SemVer_parse, double_hyphen_valid_pre_release) {
  const sourcemeta::core::SemVer version{"1.2.3--"};
  EXPECT_EQ(version.pre_release(), "-");
}

TEST(SemVer_parse, pre_release_alphanumeric_0a1) {
  const sourcemeta::core::SemVer version{"1.0.0-0a1"};
  EXPECT_EQ(version.pre_release(), "0a1");
}

TEST(SemVer_parse, pre_release_only_hyphens) {
  const sourcemeta::core::SemVer version{"1.0.0----"};
  EXPECT_EQ(version.pre_release(), "---");
}

TEST(SemVer_parse, node_semver_valid_1_2_3_0) {
  const sourcemeta::core::SemVer version{"1.2.3-0"};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
  EXPECT_EQ(version.pre_release(), "0");
}

TEST(SemVer_parse, node_semver_valid_1_2_3_123) {
  const sourcemeta::core::SemVer version{"1.2.3-123"};
  EXPECT_EQ(version.pre_release(), "123");
}

TEST(SemVer_parse, node_semver_valid_1_2_3_1_2_3) {
  const sourcemeta::core::SemVer version{"1.2.3-1.2.3"};
  EXPECT_EQ(version.pre_release(), "1.2.3");
}

TEST(SemVer_parse, node_semver_valid_1_2_3_1a) {
  const sourcemeta::core::SemVer version{"1.2.3-1a"};
  EXPECT_EQ(version.pre_release(), "1a");
}

TEST(SemVer_parse, node_semver_valid_1_2_3_a1) {
  const sourcemeta::core::SemVer version{"1.2.3-a1"};
  EXPECT_EQ(version.pre_release(), "a1");
}

TEST(SemVer_parse, node_semver_valid_1_2_3_alpha_hyphen_1) {
  const sourcemeta::core::SemVer version{"1.2.3-alpha-1"};
  EXPECT_EQ(version.pre_release(), "alpha-1");
}

TEST(SemVer_parse, node_semver_valid_pre_release_dot_hyphen) {
  const sourcemeta::core::SemVer version{"1.2.3-alpha-.-beta"};
  EXPECT_EQ(version.pre_release(), "alpha-.-beta");
}

TEST(SemVer_parse, node_semver_valid_build_456) {
  const sourcemeta::core::SemVer version{"1.2.3+456"};
  EXPECT_EQ(version.build(), "456");
}

TEST(SemVer_parse, node_semver_valid_build_new_hyphen_build) {
  const sourcemeta::core::SemVer version{"1.2.3+new-build"};
  EXPECT_EQ(version.build(), "new-build");
}

TEST(SemVer_parse, node_semver_valid_build_1a) {
  const sourcemeta::core::SemVer version{"1.2.3+build.1a"};
  EXPECT_EQ(version.build(), "build.1a");
}

TEST(SemVer_parse, node_semver_valid_build_a1) {
  const sourcemeta::core::SemVer version{"1.2.3+build.a1"};
  EXPECT_EQ(version.build(), "build.a1");
}

TEST(SemVer_parse, node_semver_valid_build_alpha_beta) {
  const sourcemeta::core::SemVer version{"1.2.3+build.alpha.beta"};
  EXPECT_EQ(version.build(), "build.alpha.beta");
}

TEST(SemVer_parse, node_semver_valid_pre_release_and_build) {
  const sourcemeta::core::SemVer version{"1.2.3-alpha+build"};
  EXPECT_EQ(version.pre_release(), "alpha");
  EXPECT_EQ(version.build(), "build");
}

TEST(SemVer_parse, valid_zero_zero_zero_pre_release_zero) {
  const sourcemeta::core::SemVer version{"0.0.0-0"};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "0");
}

TEST(SemVer_parse, valid_zero_zero_zero_build_zero) {
  const sourcemeta::core::SemVer version{"0.0.0+0"};
  EXPECT_EQ(version.build(), "0");
}

TEST(SemVer_parse, spec_example_item_9_x_y_z_double_hyphen) {
  const sourcemeta::core::SemVer version{"1.0.0-x-y-z.--"};
  EXPECT_EQ(version.pre_release(), "x-y-z.--");
}

TEST(SemVer_parse, spec_example_item_10_build_hex) {
  const sourcemeta::core::SemVer version{"1.0.0+21AF26D3----117B344092BD"};
  EXPECT_EQ(version.build(), "21AF26D3----117B344092BD");
}

TEST(SemVer_parse, single_letter_a) {
  const sourcemeta::core::SemVer version{"1.0.0-a"};
  EXPECT_EQ(version.pre_release(), "a");
}

TEST(SemVer_parse, single_letter_Z) {
  const sourcemeta::core::SemVer version{"1.0.0-Z"};
  EXPECT_EQ(version.pre_release(), "Z");
}

TEST(SemVer_parse, single_letter_z) {
  const sourcemeta::core::SemVer version{"1.0.0-z"};
  EXPECT_EQ(version.pre_release(), "z");
}

TEST(SemVer_parse, single_letter_A) {
  const sourcemeta::core::SemVer version{"1.0.0-A"};
  EXPECT_EQ(version.pre_release(), "A");
}

TEST(SemVer_parse, build_leading_zeros_007) {
  const sourcemeta::core::SemVer version{"1.0.0+007"};
  EXPECT_EQ(version.build(), "007");
}

TEST(SemVer_parse, build_leading_zeros_00) {
  const sourcemeta::core::SemVer version{"1.0.0+00"};
  EXPECT_EQ(version.build(), "00");
}

TEST(SemVer_parse, build_single_zero) {
  const sourcemeta::core::SemVer version{"1.0.0+0"};
  EXPECT_EQ(version.build(), "0");
}

TEST(SemVer_parse, pre_release_alphanumeric_00a) {
  const sourcemeta::core::SemVer version{"1.0.0-00a"};
  EXPECT_EQ(version.pre_release(), "00a");
}

TEST(SemVer_parse, pre_release_alphanumeric_007a) {
  const sourcemeta::core::SemVer version{"1.0.0-007a"};
  EXPECT_EQ(version.pre_release(), "007a");
}

TEST(SemVer_parse, pre_release_zero_hyphen_zero) {
  const sourcemeta::core::SemVer version{"1.0.0-0-0"};
  EXPECT_EQ(version.pre_release(), "0-0");
}

TEST(SemVer_parse, build_single_hyphen) {
  const sourcemeta::core::SemVer version{"1.0.0+-"};
  EXPECT_EQ(version.build(), "-");
}

TEST(SemVer_parse, build_double_hyphen) {
  const sourcemeta::core::SemVer version{"1.0.0+--"};
  EXPECT_EQ(version.build(), "--");
}

TEST(SemVer_parse, build_triple_hyphen) {
  const sourcemeta::core::SemVer version{"1.0.0+---"};
  EXPECT_EQ(version.build(), "---");
}

TEST(SemVer_parse, build_after_zero_patch) {
  const sourcemeta::core::SemVer version{"0.0.0+build"};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_TRUE(version.pre_release().empty());
  EXPECT_EQ(version.build(), "build");
}

TEST(SemVer_parse, build_meta) {
  const sourcemeta::core::SemVer version{"1.2.3+meta"};
  EXPECT_EQ(version.build(), "meta");
}
