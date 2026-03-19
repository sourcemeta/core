#include <gtest/gtest.h>

#include <sourcemeta/core/semver.h>

#include <exception>

#define EXPECT_SEMVER_ERROR(input, expected_column)                            \
  try {                                                                        \
    sourcemeta::core::SemVer{(input)};                                         \
    FAIL() << "The constructor was expected to throw";                         \
  } catch (const sourcemeta::core::SemVerParseError &error) {                  \
    EXPECT_EQ(error.column(), expected_column);                                \
    SUCCEED();                                                                 \
  } catch (const std::exception &) {                                           \
    FAIL() << "The constructor was expected to throw SemVerParseError";        \
  }

#define EXPECT_SEMVER_OVERFLOW(input)                                          \
  try {                                                                        \
    sourcemeta::core::SemVer{(input)};                                         \
    FAIL() << "The constructor was expected to throw";                         \
  } catch (const sourcemeta::core::SemVerOverflowError &) {                    \
    SUCCEED();                                                                 \
  } catch (const std::exception &) {                                           \
    FAIL() << "The constructor was expected to throw SemVerOverflowError";     \
  }

TEST(SemVer_parse_error, empty) { EXPECT_SEMVER_ERROR("", 1); }

TEST(SemVer_parse_error, leading_space) { EXPECT_SEMVER_ERROR(" 1.2.3", 1); }

TEST(SemVer_parse_error, trailing_space) { EXPECT_SEMVER_ERROR("1.2.3 ", 6); }

TEST(SemVer_parse_error, inner_space) { EXPECT_SEMVER_ERROR("1. 2.3", 3); }

TEST(SemVer_parse_error, newline) { EXPECT_SEMVER_ERROR("1.2.3\n", 6); }

TEST(SemVer_parse_error, tab) { EXPECT_SEMVER_ERROR("\t1.2.3", 1); }

TEST(SemVer_parse_error, major_only) { EXPECT_SEMVER_ERROR("1", 2); }

TEST(SemVer_parse_error, major_minor_only) { EXPECT_SEMVER_ERROR("1.2", 4); }

TEST(SemVer_parse_error, four_components) { EXPECT_SEMVER_ERROR("1.2.3.4", 6); }

TEST(SemVer_parse_error, trailing_dot) { EXPECT_SEMVER_ERROR("1.2.3.", 6); }

TEST(SemVer_parse_error, leading_dot) { EXPECT_SEMVER_ERROR(".1.2.3", 1); }

TEST(SemVer_parse_error, double_dot) { EXPECT_SEMVER_ERROR("1..2.3", 3); }

TEST(SemVer_parse_error, empty_major) { EXPECT_SEMVER_ERROR(".2.3", 1); }

TEST(SemVer_parse_error, empty_minor) { EXPECT_SEMVER_ERROR("1..3", 3); }

TEST(SemVer_parse_error, empty_patch) { EXPECT_SEMVER_ERROR("1.2.", 5); }

TEST(SemVer_parse_error, leading_zero_major) {
  EXPECT_SEMVER_ERROR("01.2.3", 1);
}

TEST(SemVer_parse_error, leading_zero_minor) {
  EXPECT_SEMVER_ERROR("1.02.3", 3);
}

TEST(SemVer_parse_error, leading_zero_patch) {
  EXPECT_SEMVER_ERROR("1.2.03", 5);
}

TEST(SemVer_parse_error, leading_zeros_all) {
  EXPECT_SEMVER_ERROR("01.02.03", 1);
}

TEST(SemVer_parse_error, leading_zero_major_large) {
  EXPECT_SEMVER_ERROR("012.2.3", 1);
}

TEST(SemVer_parse_error, v_prefix) { EXPECT_SEMVER_ERROR("v1.2.3", 1); }

TEST(SemVer_parse_error, V_prefix) { EXPECT_SEMVER_ERROR("V1.2.3", 1); }

TEST(SemVer_parse_error, equals_prefix) { EXPECT_SEMVER_ERROR("=1.2.3", 1); }

TEST(SemVer_parse_error, negative) { EXPECT_SEMVER_ERROR("-1.2.3", 1); }

TEST(SemVer_parse_error, text_suffix) { EXPECT_SEMVER_ERROR("1.2.3abc", 6); }

TEST(SemVer_parse_error, alpha_in_major) { EXPECT_SEMVER_ERROR("a.2.3", 1); }

TEST(SemVer_parse_error, alpha_in_minor) { EXPECT_SEMVER_ERROR("1.b.3", 3); }

TEST(SemVer_parse_error, alpha_in_patch) { EXPECT_SEMVER_ERROR("1.2.c", 5); }

TEST(SemVer_parse_error, empty_build) { EXPECT_SEMVER_ERROR("1.2.3+", 7); }

TEST(SemVer_parse_error, empty_pre_release) {
  EXPECT_SEMVER_ERROR("1.2.3-", 7);
}

TEST(SemVer_parse_error, pre_release_empty_identifier) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha..1", 7);
}

TEST(SemVer_parse_error, pre_release_leading_zero_numeric) {
  EXPECT_SEMVER_ERROR("1.2.3-01", 7);
}

TEST(SemVer_parse_error, pre_release_leading_zero_numeric_large) {
  EXPECT_SEMVER_ERROR("1.2.3-012", 7);
}

TEST(SemVer_parse_error, pre_release_leading_zero_in_dotted) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha.01", 7);
}

TEST(SemVer_parse_error, pre_release_illegal_char) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha#", 7);
}

TEST(SemVer_parse_error, pre_release_exclamation) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha!", 7);
}

TEST(SemVer_parse_error, pre_release_space) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha beta", 7);
}

TEST(SemVer_parse_error, pre_release_trailing_dot) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha.", 7);
}

TEST(SemVer_parse_error, pre_release_leading_dot) {
  EXPECT_SEMVER_ERROR("1.2.3-.alpha", 7);
}

TEST(SemVer_parse_error, build_empty_identifier) {
  EXPECT_SEMVER_ERROR("1.2.3+build..1", 7);
}

TEST(SemVer_parse_error, build_illegal_char) {
  EXPECT_SEMVER_ERROR("1.2.3+build!", 7);
}

TEST(SemVer_parse_error, build_trailing_dot) {
  EXPECT_SEMVER_ERROR("1.2.3+build.", 7);
}

TEST(SemVer_parse_error, build_leading_dot) {
  EXPECT_SEMVER_ERROR("1.2.3+.build", 7);
}

TEST(SemVer_parse_error, build_space) {
  EXPECT_SEMVER_ERROR("1.2.3+build 1", 7);
}

TEST(SemVer_parse_error, just_dots) { EXPECT_SEMVER_ERROR("..", 1); }

TEST(SemVer_parse_error, text) { EXPECT_SEMVER_ERROR("not-a-version", 1); }

TEST(SemVer_parse_error, negative_major) { EXPECT_SEMVER_ERROR("-1.0.0", 1); }

TEST(SemVer_parse_error, star) { EXPECT_SEMVER_ERROR("*.0.0", 1); }

TEST(SemVer_parse_error, x_range) { EXPECT_SEMVER_ERROR("1.x.0", 3); }

TEST(SemVer_parse_error, unicode) { EXPECT_SEMVER_ERROR("1.0.0-\xc3\xa9", 7); }

TEST(SemVer_parse_error, pre_release_000) {
  EXPECT_SEMVER_ERROR("1.0.0-000", 7);
}

TEST(SemVer_parse_error, pre_release_00) { EXPECT_SEMVER_ERROR("1.0.0-00", 7); }

TEST(SemVer_parse_error, underscore_in_build) {
  EXPECT_SEMVER_ERROR("1.2.3+build_1", 7);
}

TEST(SemVer_parse_error, underscore_in_pre_release) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha_1", 7);
}

TEST(SemVer_parse_error, at_sign) { EXPECT_SEMVER_ERROR("1.2.3-alpha@1", 7); }

TEST(SemVer_parse_error, hello_world) {
  EXPECT_SEMVER_ERROR("hello, world", 1);
}

TEST(SemVer_parse_error, xyz) { EXPECT_SEMVER_ERROR("xyz", 1); }

TEST(SemVer_parse_error, pre_release_plus_in_middle) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha+beta+gamma", 13);
}

TEST(SemVer_parse_error, empty_first_pre_release_dot) {
  EXPECT_SEMVER_ERROR("1.2.3-.", 7);
}

TEST(SemVer_parse_error, empty_trailing_pre_release_dot) {
  EXPECT_SEMVER_ERROR("1.2.3-a.", 7);
}

TEST(SemVer_parse_error, empty_middle_pre_release) {
  EXPECT_SEMVER_ERROR("1.2.3-a..b", 7);
}

TEST(SemVer_parse_error, empty_first_build_dot) {
  EXPECT_SEMVER_ERROR("1.2.3+.", 7);
}

TEST(SemVer_parse_error, empty_middle_build) {
  EXPECT_SEMVER_ERROR("1.2.3+a..b", 7);
}

TEST(SemVer_parse_error, empty_trailing_build_dot) {
  EXPECT_SEMVER_ERROR("1.2.3+a.", 7);
}

TEST(SemVer_parse_error, pre_release_leading_zero_007) {
  EXPECT_SEMVER_ERROR("1.0.0-007", 7);
}

TEST(SemVer_parse_error, pre_release_leading_zero_01) {
  EXPECT_SEMVER_ERROR("1.0.0-01", 7);
}

TEST(SemVer_parse_error, overflow_major) {
  EXPECT_SEMVER_OVERFLOW("99999999999999999999.0.0");
}

TEST(SemVer_parse_error, overflow_minor) {
  EXPECT_SEMVER_OVERFLOW("0.99999999999999999999.0");
}

TEST(SemVer_parse_error, overflow_patch) {
  EXPECT_SEMVER_OVERFLOW("0.0.99999999999999999999");
}

TEST(SemVer_parse_error, overflow_major_boundary) {
  EXPECT_SEMVER_OVERFLOW("18446744073709551616.0.0");
}

TEST(SemVer_parse_error, overflow_from_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::SemVer::from("99999999999999999999.0.0").has_value());
}

TEST(SemVer_parse_error, leading_zero_long_is_parse_error_not_overflow) {
  EXPECT_SEMVER_ERROR("09999999999999999999999.0.0", 1);
}
