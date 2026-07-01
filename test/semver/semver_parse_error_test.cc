#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/test.h>

#include <exception>

#define EXPECT_SEMVER_ERROR(input, expected_column)                            \
  try {                                                                        \
    sourcemeta::core::SemVer{(input)};                                         \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::SemVerParseError &error) {                  \
    EXPECT_EQ(error.column(), expected_column);                                \
  } catch (const std::exception &) {                                           \
    FAIL();                                                                    \
  }

#define EXPECT_SEMVER_OVERFLOW(input)                                          \
  try {                                                                        \
    sourcemeta::core::SemVer{(input)};                                         \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::SemVerOverflowError &) {                    \
  } catch (const std::exception &) {                                           \
    FAIL();                                                                    \
  }

TEST(empty) { EXPECT_SEMVER_ERROR("", 1); }

TEST(leading_space) { EXPECT_SEMVER_ERROR(" 1.2.3", 1); }

TEST(trailing_space) { EXPECT_SEMVER_ERROR("1.2.3 ", 6); }

TEST(inner_space) { EXPECT_SEMVER_ERROR("1. 2.3", 3); }

TEST(newline) { EXPECT_SEMVER_ERROR("1.2.3\n", 6); }

TEST(tab) { EXPECT_SEMVER_ERROR("\t1.2.3", 1); }

TEST(major_only) { EXPECT_SEMVER_ERROR("1", 2); }

TEST(major_minor_only) { EXPECT_SEMVER_ERROR("1.2", 4); }

TEST(four_components) { EXPECT_SEMVER_ERROR("1.2.3.4", 6); }

TEST(trailing_dot) { EXPECT_SEMVER_ERROR("1.2.3.", 6); }

TEST(leading_dot) { EXPECT_SEMVER_ERROR(".1.2.3", 1); }

TEST(double_dot) { EXPECT_SEMVER_ERROR("1..2.3", 3); }

TEST(empty_major) { EXPECT_SEMVER_ERROR(".2.3", 1); }

TEST(empty_minor) { EXPECT_SEMVER_ERROR("1..3", 3); }

TEST(empty_patch) { EXPECT_SEMVER_ERROR("1.2.", 5); }

TEST(leading_zero_major) { EXPECT_SEMVER_ERROR("01.2.3", 1); }

TEST(leading_zero_minor) { EXPECT_SEMVER_ERROR("1.02.3", 3); }

TEST(leading_zero_patch) { EXPECT_SEMVER_ERROR("1.2.03", 5); }

TEST(leading_zeros_all) { EXPECT_SEMVER_ERROR("01.02.03", 1); }

TEST(leading_zero_major_large) { EXPECT_SEMVER_ERROR("012.2.3", 1); }

TEST(v_prefix) { EXPECT_SEMVER_ERROR("v1.2.3", 1); }

TEST(V_prefix) { EXPECT_SEMVER_ERROR("V1.2.3", 1); }

TEST(equals_prefix) { EXPECT_SEMVER_ERROR("=1.2.3", 1); }

TEST(negative) { EXPECT_SEMVER_ERROR("-1.2.3", 1); }

TEST(text_suffix) { EXPECT_SEMVER_ERROR("1.2.3abc", 6); }

TEST(alpha_in_major) { EXPECT_SEMVER_ERROR("a.2.3", 1); }

TEST(alpha_in_minor) { EXPECT_SEMVER_ERROR("1.b.3", 3); }

TEST(alpha_in_patch) { EXPECT_SEMVER_ERROR("1.2.c", 5); }

TEST(empty_build) { EXPECT_SEMVER_ERROR("1.2.3+", 7); }

TEST(empty_pre_release) { EXPECT_SEMVER_ERROR("1.2.3-", 7); }

TEST(pre_release_empty_identifier) { EXPECT_SEMVER_ERROR("1.2.3-alpha..1", 7); }

TEST(pre_release_leading_zero_numeric) { EXPECT_SEMVER_ERROR("1.2.3-01", 7); }

TEST(pre_release_leading_zero_numeric_large) {
  EXPECT_SEMVER_ERROR("1.2.3-012", 7);
}

TEST(pre_release_leading_zero_in_dotted) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha.01", 7);
}

TEST(pre_release_illegal_char) { EXPECT_SEMVER_ERROR("1.2.3-alpha#", 7); }

TEST(pre_release_exclamation) { EXPECT_SEMVER_ERROR("1.2.3-alpha!", 7); }

TEST(pre_release_space) { EXPECT_SEMVER_ERROR("1.2.3-alpha beta", 7); }

TEST(pre_release_trailing_dot) { EXPECT_SEMVER_ERROR("1.2.3-alpha.", 7); }

TEST(pre_release_leading_dot) { EXPECT_SEMVER_ERROR("1.2.3-.alpha", 7); }

TEST(build_empty_identifier) { EXPECT_SEMVER_ERROR("1.2.3+build..1", 7); }

TEST(build_illegal_char) { EXPECT_SEMVER_ERROR("1.2.3+build!", 7); }

TEST(build_trailing_dot) { EXPECT_SEMVER_ERROR("1.2.3+build.", 7); }

TEST(build_leading_dot) { EXPECT_SEMVER_ERROR("1.2.3+.build", 7); }

TEST(build_space) { EXPECT_SEMVER_ERROR("1.2.3+build 1", 7); }

TEST(just_dots) { EXPECT_SEMVER_ERROR("..", 1); }

TEST(text) { EXPECT_SEMVER_ERROR("not-a-version", 1); }

TEST(negative_major) { EXPECT_SEMVER_ERROR("-1.0.0", 1); }

TEST(star) { EXPECT_SEMVER_ERROR("*.0.0", 1); }

TEST(x_range) { EXPECT_SEMVER_ERROR("1.x.0", 3); }

TEST(unicode) { EXPECT_SEMVER_ERROR("1.0.0-\xc3\xa9", 7); }

TEST(pre_release_000) { EXPECT_SEMVER_ERROR("1.0.0-000", 7); }

TEST(pre_release_00) { EXPECT_SEMVER_ERROR("1.0.0-00", 7); }

TEST(underscore_in_build) { EXPECT_SEMVER_ERROR("1.2.3+build_1", 7); }

TEST(underscore_in_pre_release) { EXPECT_SEMVER_ERROR("1.2.3-alpha_1", 7); }

TEST(at_sign) { EXPECT_SEMVER_ERROR("1.2.3-alpha@1", 7); }

TEST(hello_world) { EXPECT_SEMVER_ERROR("hello, world", 1); }

TEST(xyz) { EXPECT_SEMVER_ERROR("xyz", 1); }

TEST(pre_release_plus_in_middle) {
  EXPECT_SEMVER_ERROR("1.2.3-alpha+beta+gamma", 13);
}

TEST(empty_first_pre_release_dot) { EXPECT_SEMVER_ERROR("1.2.3-.", 7); }

TEST(empty_trailing_pre_release_dot) { EXPECT_SEMVER_ERROR("1.2.3-a.", 7); }

TEST(empty_middle_pre_release) { EXPECT_SEMVER_ERROR("1.2.3-a..b", 7); }

TEST(empty_first_build_dot) { EXPECT_SEMVER_ERROR("1.2.3+.", 7); }

TEST(empty_middle_build) { EXPECT_SEMVER_ERROR("1.2.3+a..b", 7); }

TEST(empty_trailing_build_dot) { EXPECT_SEMVER_ERROR("1.2.3+a.", 7); }

TEST(pre_release_leading_zero_007) { EXPECT_SEMVER_ERROR("1.0.0-007", 7); }

TEST(pre_release_leading_zero_01) { EXPECT_SEMVER_ERROR("1.0.0-01", 7); }

TEST(overflow_major) { EXPECT_SEMVER_OVERFLOW("99999999999999999999.0.0"); }

TEST(overflow_minor) { EXPECT_SEMVER_OVERFLOW("0.99999999999999999999.0"); }

TEST(overflow_patch) { EXPECT_SEMVER_OVERFLOW("0.0.99999999999999999999"); }

TEST(overflow_major_boundary) {
  EXPECT_SEMVER_OVERFLOW("18446744073709551616.0.0");
}

TEST(overflow_from_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::SemVer::from("99999999999999999999.0.0").has_value());
}

TEST(leading_zero_long_is_parse_error_not_overflow) {
  EXPECT_SEMVER_ERROR("09999999999999999999999.0.0", 1);
}
