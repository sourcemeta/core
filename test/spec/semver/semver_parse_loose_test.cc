#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/test.h>

TEST(v_prefix_full) {
  const sourcemeta::core::SemVer version{"v1.2.3",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
  EXPECT_TRUE(version.pre_release().empty());
  EXPECT_TRUE(version.build().empty());
}

TEST(V_prefix_full) {
  const sourcemeta::core::SemVer version{"V1.2.3",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
}

TEST(major_minor_only) {
  const sourcemeta::core::SemVer version{"1.2",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
}

TEST(major_only) {
  const sourcemeta::core::SemVer version{"1",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(zero_only) {
  const sourcemeta::core::SemVer version{"0",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(v_prefix_major_minor) {
  const sourcemeta::core::SemVer version{"v1.2",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
}

TEST(v_prefix_major_only) {
  const sourcemeta::core::SemVer version{"v1",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(V_prefix_major_minor) {
  const sourcemeta::core::SemVer version{"V1.2",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
}

TEST(V_prefix_major_only) {
  const sourcemeta::core::SemVer version{"V1",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(v_prefix_zero) {
  const sourcemeta::core::SemVer version{"v0",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(v_prefix_zero_zero) {
  const sourcemeta::core::SemVer version{"v0.0",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(multi_digit_major_minor) {
  const sourcemeta::core::SemVer version{"v10.20",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 10);
  EXPECT_EQ(version.minor(), 20);
  EXPECT_EQ(version.patch(), 0);
}

TEST(major_minor_with_pre_release) {
  const sourcemeta::core::SemVer version{"1.2-alpha",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "alpha");
}

TEST(major_only_with_pre_release) {
  const sourcemeta::core::SemVer version{"1-alpha",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "alpha");
}

TEST(v_prefix_major_minor_with_build) {
  const sourcemeta::core::SemVer version{"v1.2+build",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.build(), "build");
}

TEST(v_prefix_major_minor_pre_release_and_build) {
  const sourcemeta::core::SemVer version{"v1.2-alpha+build",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "alpha");
  EXPECT_EQ(version.build(), "build");
}

TEST(v_prefix_full_with_pre_release_and_build) {
  const sourcemeta::core::SemVer version{"v1.2.3-alpha.1+build.123",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
  EXPECT_EQ(version.pre_release(), "alpha.1");
  EXPECT_EQ(version.build(), "build.123");
}

TEST(strict_valid_still_works) {
  const sourcemeta::core::SemVer version{"1.2.3",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
}

TEST(from_v_prefix) {
  const auto result = sourcemeta::core::SemVer::from(
      "v1.2.3", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 2);
  EXPECT_EQ(result->patch(), 3);
}

TEST(from_major_only) {
  const auto result = sourcemeta::core::SemVer::from(
      "1", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 0);
  EXPECT_EQ(result->patch(), 0);
}

TEST(from_v_prefix_major_minor) {
  const auto result = sourcemeta::core::SemVer::from(
      "v1.2", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 2);
  EXPECT_EQ(result->patch(), 0);
}

TEST(from_invalid_still_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "v01.2.3", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

TEST(to_string_major_only) {
  const sourcemeta::core::SemVer version{"1",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.to_string(), "1.0.0");
}

TEST(to_string_major_minor) {
  const sourcemeta::core::SemVer version{"1.2",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.to_string(), "1.2.0");
}

TEST(to_string_v_prefix) {
  const sourcemeta::core::SemVer version{"v1.2.3",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.to_string(), "1.2.3");
}

TEST(major_only_with_build) {
  const sourcemeta::core::SemVer version{"1+build",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.build(), "build");
}

TEST(from_loose_invalid_major_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "x.2.3", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

TEST(from_loose_overflow_major_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::SemVer::from("99999999999999999999.0.0",
                                     sourcemeta::core::SemVer::Mode::Loose)
          .has_value());
}

TEST(from_loose_overflow_minor_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::SemVer::from("1.99999999999999999999.0",
                                     sourcemeta::core::SemVer::Mode::Loose)
          .has_value());
}

TEST(from_loose_invalid_minor_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "1.x.3", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

TEST(from_loose_overflow_patch_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::SemVer::from("1.2.99999999999999999999",
                                     sourcemeta::core::SemVer::Mode::Loose)
          .has_value());
}

TEST(from_loose_invalid_patch_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "1.2.x", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

TEST(from_loose_invalid_pre_release_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "1.2.3-!", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

TEST(from_loose_invalid_build_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "1.2.3+!", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

TEST(from_loose_trailing_garbage_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "1.2.3x", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

// Tests to hit position < input.size() branches in parse_semver loose mode
TEST(from_loose_major_no_dot_ends_at_input_end) {
  const auto result = sourcemeta::core::SemVer::from(
      "42", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 42);
  EXPECT_EQ(result->minor(), 0);
  EXPECT_EQ(result->patch(), 0);
}

TEST(from_loose_major_minor_no_dot_ends_at_input_end) {
  const auto result = sourcemeta::core::SemVer::from(
      "5.7", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 5);
  EXPECT_EQ(result->minor(), 7);
  EXPECT_EQ(result->patch(), 0);
}

TEST(from_loose_v_prefix_major_only_ends_at_input_end) {
  const auto result = sourcemeta::core::SemVer::from(
      "v8", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 8);
}

TEST(from_loose_V_prefix_major_minor_ends_at_input_end) {
  const auto result = sourcemeta::core::SemVer::from(
      "V3.4", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 3);
  EXPECT_EQ(result->minor(), 4);
}

// Test position >= input.size() after scanning pre-release
TEST(from_loose_ends_exactly_after_pre_release) {
  const auto result = sourcemeta::core::SemVer::from(
      "1.0-rc1", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->pre_release(), "rc1");
  EXPECT_TRUE(result->build().empty());
}

// Tests to ensure all parse branches are hit in loose mode
TEST(from_loose_major_with_pre_release_no_dots) {
  const auto result = sourcemeta::core::SemVer::from(
      "9-beta", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 9);
  EXPECT_EQ(result->minor(), 0);
  EXPECT_EQ(result->patch(), 0);
  EXPECT_EQ(result->pre_release(), "beta");
}

TEST(from_loose_major_with_build_no_dots) {
  const auto result = sourcemeta::core::SemVer::from(
      "7+snapshot", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 7);
  EXPECT_EQ(result->build(), "snapshot");
}

TEST(from_loose_major_minor_with_pre_release_no_patch_dot) {
  const auto result = sourcemeta::core::SemVer::from(
      "2.5-alpha", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 2);
  EXPECT_EQ(result->minor(), 5);
  EXPECT_EQ(result->pre_release(), "alpha");
}

TEST(from_loose_major_minor_with_build_no_patch_dot) {
  const auto result = sourcemeta::core::SemVer::from(
      "3.6+dev", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 3);
  EXPECT_EQ(result->minor(), 6);
  EXPECT_EQ(result->build(), "dev");
}

// Tests using constructor (ShouldThrow=true) to hit parse branches
TEST(constructor_loose_major_only_with_pre_release) {
  const sourcemeta::core::SemVer version{"4-rc",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 4);
  EXPECT_EQ(version.pre_release(), "rc");
}

// Test empty string in loose mode via constructor (hits line 254)
TEST(from_loose_empty_string_fails) {
  EXPECT_FALSE(
      sourcemeta::core::SemVer::from("", sourcemeta::core::SemVer::Mode::Loose)
          .has_value());
}

TEST(constructor_loose_major_only_with_build) {
  const sourcemeta::core::SemVer version{"5+build",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 5);
  EXPECT_EQ(version.build(), "build");
}

TEST(constructor_loose_major_minor_with_pre_release) {
  const sourcemeta::core::SemVer version{"6.7-alpha",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 6);
  EXPECT_EQ(version.minor(), 7);
  EXPECT_EQ(version.pre_release(), "alpha");
}

TEST(constructor_loose_major_minor_with_build) {
  const sourcemeta::core::SemVer version{"8.9+snapshot",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 8);
  EXPECT_EQ(version.minor(), 9);
  EXPECT_EQ(version.build(), "snapshot");
}

TEST(from_loose_pre_release_and_build) {
  const auto result = sourcemeta::core::SemVer::from(
      "v1.2-rc.1+build.5", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 2);
  EXPECT_EQ(result->patch(), 0);
  EXPECT_EQ(result->pre_release(), "rc.1");
  EXPECT_EQ(result->build(), "build.5");
}

TEST(from_loose_major_then_letter_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "1x", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

TEST(from_loose_major_minor_then_letter_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "1.2x", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}
