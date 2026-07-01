#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/test.h>

TEST(major_less) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0"},
            sourcemeta::core::SemVer{"2.0.0"});
}

TEST(minor_less) {
  EXPECT_LT(sourcemeta::core::SemVer{"2.0.0"},
            sourcemeta::core::SemVer{"2.1.0"});
}

TEST(patch_less) {
  EXPECT_LT(sourcemeta::core::SemVer{"2.1.0"},
            sourcemeta::core::SemVer{"2.1.1"});
}

TEST(equal_basic) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.2.3"},
            sourcemeta::core::SemVer{"1.2.3"});
}

TEST(not_equal) {
  EXPECT_NE(sourcemeta::core::SemVer{"1.2.3"},
            sourcemeta::core::SemVer{"1.2.4"});
}

TEST(greater_major) {
  EXPECT_GT(sourcemeta::core::SemVer{"2.0.0"},
            sourcemeta::core::SemVer{"1.0.0"});
}

TEST(greater_minor) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.1.0"},
            sourcemeta::core::SemVer{"1.0.0"});
}

TEST(greater_patch) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.0.1"},
            sourcemeta::core::SemVer{"1.0.0"});
}

TEST(less_equal) {
  EXPECT_LE(sourcemeta::core::SemVer{"1.0.0"},
            sourcemeta::core::SemVer{"1.0.0"});
  EXPECT_LE(sourcemeta::core::SemVer{"1.0.0"},
            sourcemeta::core::SemVer{"2.0.0"});
}

TEST(greater_equal) {
  EXPECT_GE(sourcemeta::core::SemVer{"1.0.0"},
            sourcemeta::core::SemVer{"1.0.0"});
  EXPECT_GE(sourcemeta::core::SemVer{"2.0.0"},
            sourcemeta::core::SemVer{"1.0.0"});
}

TEST(pre_release_lower_than_release) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-alpha"},
            sourcemeta::core::SemVer{"1.0.0"});
}

TEST(release_greater_than_pre_release) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.0.0"},
            sourcemeta::core::SemVer{"1.0.0-alpha"});
}

TEST(pre_release_lower_than_release_rc) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-rc.1"},
            sourcemeta::core::SemVer{"1.0.0"});
}

TEST(spec_example_alpha_lt_alpha_1) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-alpha"},
            sourcemeta::core::SemVer{"1.0.0-alpha.1"});
}

TEST(spec_example_alpha_1_lt_alpha_beta) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-alpha.1"},
            sourcemeta::core::SemVer{"1.0.0-alpha.beta"});
}

TEST(spec_example_alpha_beta_lt_beta) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-alpha.beta"},
            sourcemeta::core::SemVer{"1.0.0-beta"});
}

TEST(spec_example_beta_lt_beta_2) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-beta"},
            sourcemeta::core::SemVer{"1.0.0-beta.2"});
}

TEST(spec_example_beta_2_lt_beta_11) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-beta.2"},
            sourcemeta::core::SemVer{"1.0.0-beta.11"});
}

TEST(spec_example_beta_11_lt_rc_1) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-beta.11"},
            sourcemeta::core::SemVer{"1.0.0-rc.1"});
}

TEST(spec_example_rc_1_lt_release) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-rc.1"},
            sourcemeta::core::SemVer{"1.0.0"});
}

TEST(numeric_lt_alphanumeric) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-1"},
            sourcemeta::core::SemVer{"1.0.0-a"});
}

TEST(numeric_lt_alphanumeric_zero) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-0"},
            sourcemeta::core::SemVer{"1.0.0-a"});
}

TEST(numeric_lt_hyphen_identifier) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-0"},
            sourcemeta::core::SemVer{"1.0.0--"});
}

TEST(numeric_pre_release_2_lt_11) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-2"},
            sourcemeta::core::SemVer{"1.0.0-11"});
}

TEST(numeric_pre_release_0_lt_1) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-0"},
            sourcemeta::core::SemVer{"1.0.0-1"});
}

TEST(numeric_pre_release_99_lt_100) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-99"},
            sourcemeta::core::SemVer{"1.0.0-100"});
}

TEST(alpha_pre_release_lexical) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-aaa"},
            sourcemeta::core::SemVer{"1.0.0-aab"});
}

TEST(alpha_pre_release_case_sensitive) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-A"},
            sourcemeta::core::SemVer{"1.0.0-a"});
}

TEST(shorter_pre_release_lower) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-alpha"},
            sourcemeta::core::SemVer{"1.0.0-alpha.0"});
}

TEST(shorter_pre_release_lower_numeric) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-1"},
            sourcemeta::core::SemVer{"1.0.0-1.0"});
}

TEST(shorter_pre_release_three_vs_two) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-a.b"},
            sourcemeta::core::SemVer{"1.0.0-a.b.0"});
}

TEST(different_build_not_equal) {
  EXPECT_NE(sourcemeta::core::SemVer{"1.0.0+build1"},
            sourcemeta::core::SemVer{"1.0.0+build2"});
}

TEST(no_build_vs_build_not_equal) {
  EXPECT_NE(sourcemeta::core::SemVer{"1.0.0"},
            sourcemeta::core::SemVer{"1.0.0+build"});
}

TEST(different_builds_not_equal) {
  EXPECT_NE(sourcemeta::core::SemVer{"1.0.0+aaa"},
            sourcemeta::core::SemVer{"1.0.0+zzz"});
}

TEST(different_builds_with_pre_release_not_equal) {
  EXPECT_NE(sourcemeta::core::SemVer{"1.0.0-alpha+build1"},
            sourcemeta::core::SemVer{"1.0.0-alpha+build2"});
}

TEST(same_build_equal) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0+build"},
            sourcemeta::core::SemVer{"1.0.0+build"});
}

TEST(build_ignored_in_precedence_not_less) {
  const sourcemeta::core::SemVer left{"1.0.0+build1"};
  const sourcemeta::core::SemVer right{"1.0.0+build2"};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(no_build_vs_build_same_precedence) {
  const sourcemeta::core::SemVer left{"1.0.0"};
  const sourcemeta::core::SemVer right{"1.0.0+build"};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(build_ignored_in_precedence_with_pre_release) {
  const sourcemeta::core::SemVer left{"1.0.0-alpha+build1"};
  const sourcemeta::core::SemVer right{"1.0.0-alpha+build2"};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(pre_release_with_build_lt_release) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-alpha+build"},
            sourcemeta::core::SemVer{"1.0.0"});
}

TEST(pre_release_with_build_lt_release_with_build) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0-alpha+build"},
            sourcemeta::core::SemVer{"1.0.0+build"});
}

TEST(equal_pre_release) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-alpha"},
            sourcemeta::core::SemVer{"1.0.0-alpha"});
}

TEST(equal_pre_release_dotted) {
  EXPECT_EQ(sourcemeta::core::SemVer{"1.0.0-alpha.1"},
            sourcemeta::core::SemVer{"1.0.0-alpha.1"});
}

TEST(different_major_ignores_pre_release) {
  EXPECT_LT(sourcemeta::core::SemVer{"1.0.0"},
            sourcemeta::core::SemVer{"2.0.0-alpha"});
}

TEST(zero_versions) {
  EXPECT_EQ(sourcemeta::core::SemVer{"0.0.0"},
            sourcemeta::core::SemVer{"0.0.0"});
}

TEST(zero_lt_zero_zero_one) {
  EXPECT_LT(sourcemeta::core::SemVer{"0.0.0"},
            sourcemeta::core::SemVer{"0.0.1"});
}

TEST(not_less_when_equal) {
  const sourcemeta::core::SemVer first{"1.2.3"};
  const sourcemeta::core::SemVer second{"1.2.3"};
  EXPECT_FALSE(first < second);
  EXPECT_FALSE(first > second);
}

TEST(node_semver_release_gt_pre_release_foo) {
  EXPECT_GT(sourcemeta::core::SemVer{"0.0.0"},
            sourcemeta::core::SemVer{"0.0.0-foo"});
}

TEST(node_semver_release_gt_pre_release_asdf) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3"},
            sourcemeta::core::SemVer{"1.2.3-asdf"});
}

TEST(node_semver_release_gt_pre_release_4) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3"},
            sourcemeta::core::SemVer{"1.2.3-4"});
}

TEST(node_semver_release_gt_pre_release_4_foo) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3"},
            sourcemeta::core::SemVer{"1.2.3-4-foo"});
}

TEST(node_semver_patch_gt) {
  EXPECT_GT(sourcemeta::core::SemVer{"0.0.1"},
            sourcemeta::core::SemVer{"0.0.0"});
}

TEST(node_semver_major_gt_high_minor_patch) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.0.0"},
            sourcemeta::core::SemVer{"0.9.9"});
}

TEST(node_semver_minor_10_gt_9) {
  EXPECT_GT(sourcemeta::core::SemVer{"0.10.0"},
            sourcemeta::core::SemVer{"0.9.0"});
}

TEST(node_semver_minor_99_gt_10) {
  EXPECT_GT(sourcemeta::core::SemVer{"0.99.0"},
            sourcemeta::core::SemVer{"0.10.0"});
}

TEST(node_semver_major_gt_lower) {
  EXPECT_GT(sourcemeta::core::SemVer{"2.0.0"},
            sourcemeta::core::SemVer{"1.2.3"});
}

TEST(node_semver_pre_release_more_identifiers) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-5-foo"},
            sourcemeta::core::SemVer{"1.2.3-5"});
}

TEST(node_semver_pre_release_numeric_5_gt_4) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-5"},
            sourcemeta::core::SemVer{"1.2.3-4"});
}

TEST(node_semver_pre_release_case_sensitive_lower_gt_upper) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-5-foo"},
            sourcemeta::core::SemVer{"1.2.3-5-Foo"});
}

TEST(node_semver_build_ignored_in_gt) {
  EXPECT_GT(sourcemeta::core::SemVer{"3.0.0"},
            sourcemeta::core::SemVer{"2.7.2+asdf"});
}

TEST(node_semver_pre_release_dotted_numeric_10_gt_5) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-a.10"},
            sourcemeta::core::SemVer{"1.2.3-a.5"});
}

TEST(node_semver_pre_release_string_gt_numeric) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-a.b"},
            sourcemeta::core::SemVer{"1.2.3-a.5"});
}

TEST(node_semver_pre_release_more_segments_gt_fewer) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-a.b"},
            sourcemeta::core::SemVer{"1.2.3-a"});
}

TEST(node_semver_multi_segment_differs_at_4th) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-a.b.c.10.d.5"},
            sourcemeta::core::SemVer{"1.2.3-a.b.c.5.d.100"});
}

TEST(node_semver_lexical_r2_gt_r100) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-r2"},
            sourcemeta::core::SemVer{"1.2.3-r100"});
}

TEST(node_semver_lexical_r100_gt_R2) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.2.3-r100"},
            sourcemeta::core::SemVer{"1.2.3-R2"});
}

TEST(overflow_pre_release_longer_digits_wins) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.0.0-99999999999999999999"},
            sourcemeta::core::SemVer{"1.0.0-9999999999999999999"});
}

TEST(overflow_pre_release_same_length_lexical) {
  EXPECT_GT(sourcemeta::core::SemVer{"1.0.0-99999999999999999999"},
            sourcemeta::core::SemVer{"1.0.0-99999999999999999998"});
}
