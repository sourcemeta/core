#include <gtest/gtest.h>

#include <sourcemeta/core/semver.h>

#include <exception>

#define EXPECT_LOOSE_ERROR(input, expected_column)                             \
  try {                                                                        \
    sourcemeta::core::SemVer{(input), sourcemeta::core::SemVer::Mode::Loose};  \
    FAIL() << "The constructor was expected to throw";                         \
  } catch (const sourcemeta::core::SemVerParseError &error) {                  \
    EXPECT_EQ(error.column(), expected_column);                                \
    SUCCEED();                                                                 \
  } catch (const std::exception &) {                                           \
    FAIL() << "The constructor was expected to throw SemVerParseError";        \
  }

TEST(SemVer_parse_error_loose, empty) { EXPECT_LOOSE_ERROR("", 1); }

TEST(SemVer_parse_error_loose, v_only) { EXPECT_LOOSE_ERROR("v", 2); }

TEST(SemVer_parse_error_loose, V_only) { EXPECT_LOOSE_ERROR("V", 2); }

TEST(SemVer_parse_error_loose, v_dot) { EXPECT_LOOSE_ERROR("v.1.2", 2); }

TEST(SemVer_parse_error_loose, double_v) { EXPECT_LOOSE_ERROR("vv1.2.3", 2); }

TEST(SemVer_parse_error_loose, leading_zero_major) {
  EXPECT_LOOSE_ERROR("01.2.3", 1);
}

TEST(SemVer_parse_error_loose, v_leading_zero_major) {
  EXPECT_LOOSE_ERROR("v01.2.3", 2);
}

TEST(SemVer_parse_error_loose, leading_zero_minor) {
  EXPECT_LOOSE_ERROR("1.02", 3);
}

TEST(SemVer_parse_error_loose, v_leading_zero_minor) {
  EXPECT_LOOSE_ERROR("v1.02", 4);
}

TEST(SemVer_parse_error_loose, leading_zero_patch) {
  EXPECT_LOOSE_ERROR("1.2.03", 5);
}

TEST(SemVer_parse_error_loose, four_components) {
  EXPECT_LOOSE_ERROR("1.2.3.4", 6);
}

TEST(SemVer_parse_error_loose, v_four_components) {
  EXPECT_LOOSE_ERROR("v1.2.3.4", 7);
}

TEST(SemVer_parse_error_loose, empty_pre_release) {
  EXPECT_LOOSE_ERROR("1.2.3-", 7);
}

TEST(SemVer_parse_error_loose, v_empty_pre_release) {
  EXPECT_LOOSE_ERROR("v1.2.3-", 8);
}

TEST(SemVer_parse_error_loose, empty_build) { EXPECT_LOOSE_ERROR("1.2.3+", 7); }

TEST(SemVer_parse_error_loose, v_empty_build) {
  EXPECT_LOOSE_ERROR("v1.2.3+", 8);
}

TEST(SemVer_parse_error_loose, pre_release_leading_zero) {
  EXPECT_LOOSE_ERROR("1.2.3-01", 7);
}

TEST(SemVer_parse_error_loose, pre_release_illegal_char) {
  EXPECT_LOOSE_ERROR("1.2.3-alpha#", 7);
}

TEST(SemVer_parse_error_loose, leading_space) {
  EXPECT_LOOSE_ERROR(" v1.2.3", 1);
}

TEST(SemVer_parse_error_loose, trailing_space) {
  EXPECT_LOOSE_ERROR("v1.2.3 ", 7);
}

TEST(SemVer_parse_error_loose, v_negative) { EXPECT_LOOSE_ERROR("v-1.2.3", 2); }

TEST(SemVer_parse_error_loose, major_only_empty_pre_release) {
  EXPECT_LOOSE_ERROR("1-", 3);
}

TEST(SemVer_parse_error_loose, major_only_empty_build) {
  EXPECT_LOOSE_ERROR("1+", 3);
}

TEST(SemVer_parse_error_loose, major_minor_empty_pre_release) {
  EXPECT_LOOSE_ERROR("1.2-", 5);
}

TEST(SemVer_parse_error_loose, major_minor_empty_build) {
  EXPECT_LOOSE_ERROR("1.2+", 5);
}
