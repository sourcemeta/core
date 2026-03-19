#include <gtest/gtest.h>

#include <sourcemeta/core/semver.h>

TEST(SemVer_parse_loose, v_prefix_full) {
  const sourcemeta::core::SemVer version{"v1.2.3",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
  EXPECT_TRUE(version.pre_release().empty());
  EXPECT_TRUE(version.build().empty());
}

TEST(SemVer_parse_loose, V_prefix_full) {
  const sourcemeta::core::SemVer version{"V1.2.3",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
}

TEST(SemVer_parse_loose, major_minor_only) {
  const sourcemeta::core::SemVer version{"1.2",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, major_only) {
  const sourcemeta::core::SemVer version{"1",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, zero_only) {
  const sourcemeta::core::SemVer version{"0",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, v_prefix_major_minor) {
  const sourcemeta::core::SemVer version{"v1.2",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, v_prefix_major_only) {
  const sourcemeta::core::SemVer version{"v1",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, V_prefix_major_minor) {
  const sourcemeta::core::SemVer version{"V1.2",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, V_prefix_major_only) {
  const sourcemeta::core::SemVer version{"V1",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, v_prefix_zero) {
  const sourcemeta::core::SemVer version{"v0",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, v_prefix_zero_zero) {
  const sourcemeta::core::SemVer version{"v0.0",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 0);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, multi_digit_major_minor) {
  const sourcemeta::core::SemVer version{"v10.20",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 10);
  EXPECT_EQ(version.minor(), 20);
  EXPECT_EQ(version.patch(), 0);
}

TEST(SemVer_parse_loose, major_minor_with_pre_release) {
  const sourcemeta::core::SemVer version{"1.2-alpha",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "alpha");
}

TEST(SemVer_parse_loose, major_only_with_pre_release) {
  const sourcemeta::core::SemVer version{"1-alpha",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "alpha");
}

TEST(SemVer_parse_loose, v_prefix_major_minor_with_build) {
  const sourcemeta::core::SemVer version{"v1.2+build",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.build(), "build");
}

TEST(SemVer_parse_loose, v_prefix_major_minor_pre_release_and_build) {
  const sourcemeta::core::SemVer version{"v1.2-alpha+build",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.pre_release(), "alpha");
  EXPECT_EQ(version.build(), "build");
}

TEST(SemVer_parse_loose, v_prefix_full_with_pre_release_and_build) {
  const sourcemeta::core::SemVer version{"v1.2.3-alpha.1+build.123",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
  EXPECT_EQ(version.pre_release(), "alpha.1");
  EXPECT_EQ(version.build(), "build.123");
}

TEST(SemVer_parse_loose, strict_valid_still_works) {
  const sourcemeta::core::SemVer version{"1.2.3",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 2);
  EXPECT_EQ(version.patch(), 3);
}

TEST(SemVer_parse_loose, from_v_prefix) {
  const auto result = sourcemeta::core::SemVer::from(
      "v1.2.3", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 2);
  EXPECT_EQ(result->patch(), 3);
}

TEST(SemVer_parse_loose, from_major_only) {
  const auto result = sourcemeta::core::SemVer::from(
      "1", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 0);
  EXPECT_EQ(result->patch(), 0);
}

TEST(SemVer_parse_loose, from_v_prefix_major_minor) {
  const auto result = sourcemeta::core::SemVer::from(
      "v1.2", sourcemeta::core::SemVer::Mode::Loose);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->major(), 1);
  EXPECT_EQ(result->minor(), 2);
  EXPECT_EQ(result->patch(), 0);
}

TEST(SemVer_parse_loose, from_invalid_still_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::SemVer::from(
                   "v01.2.3", sourcemeta::core::SemVer::Mode::Loose)
                   .has_value());
}

TEST(SemVer_parse_loose, to_string_major_only) {
  const sourcemeta::core::SemVer version{"1",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.to_string(), "1.0.0");
}

TEST(SemVer_parse_loose, to_string_major_minor) {
  const sourcemeta::core::SemVer version{"1.2",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.to_string(), "1.2.0");
}

TEST(SemVer_parse_loose, to_string_v_prefix) {
  const sourcemeta::core::SemVer version{"v1.2.3",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.to_string(), "1.2.3");
}

TEST(SemVer_parse_loose, major_only_with_build) {
  const sourcemeta::core::SemVer version{"1+build",
                                         sourcemeta::core::SemVer::Mode::Loose};
  EXPECT_EQ(version.major(), 1);
  EXPECT_EQ(version.minor(), 0);
  EXPECT_EQ(version.patch(), 0);
  EXPECT_EQ(version.build(), "build");
}
