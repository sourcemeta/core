#include <sourcemeta/core/semver.h>
#include <sourcemeta/core/test.h>

#include <exception>

#define EXPECT_LOOSE_ERROR(input, expected_column)                             \
  try {                                                                        \
    sourcemeta::core::SemVer{(input), sourcemeta::core::SemVer::Mode::Loose};  \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::SemVerParseError &error) {                  \
    EXPECT_EQ(error.column(), expected_column);                                \
  } catch (const std::exception &) {                                           \
    FAIL();                                                                    \
  }

TEST(empty) { EXPECT_LOOSE_ERROR("", 1); }

TEST(v_only) { EXPECT_LOOSE_ERROR("v", 2); }

TEST(V_only) { EXPECT_LOOSE_ERROR("V", 2); }

TEST(v_dot) { EXPECT_LOOSE_ERROR("v.1.2", 2); }

TEST(double_v) { EXPECT_LOOSE_ERROR("vv1.2.3", 2); }

TEST(leading_zero_major) { EXPECT_LOOSE_ERROR("01.2.3", 1); }

TEST(v_leading_zero_major) { EXPECT_LOOSE_ERROR("v01.2.3", 2); }

TEST(leading_zero_minor) { EXPECT_LOOSE_ERROR("1.02", 3); }

TEST(v_leading_zero_minor) { EXPECT_LOOSE_ERROR("v1.02", 4); }

TEST(leading_zero_patch) { EXPECT_LOOSE_ERROR("1.2.03", 5); }

TEST(four_components) { EXPECT_LOOSE_ERROR("1.2.3.4", 6); }

TEST(v_four_components) { EXPECT_LOOSE_ERROR("v1.2.3.4", 7); }

TEST(empty_pre_release) { EXPECT_LOOSE_ERROR("1.2.3-", 7); }

TEST(v_empty_pre_release) { EXPECT_LOOSE_ERROR("v1.2.3-", 8); }

TEST(empty_build) { EXPECT_LOOSE_ERROR("1.2.3+", 7); }

TEST(v_empty_build) { EXPECT_LOOSE_ERROR("v1.2.3+", 8); }

TEST(pre_release_leading_zero) { EXPECT_LOOSE_ERROR("1.2.3-01", 7); }

TEST(pre_release_illegal_char) { EXPECT_LOOSE_ERROR("1.2.3-alpha#", 7); }

TEST(leading_space) { EXPECT_LOOSE_ERROR(" v1.2.3", 1); }

TEST(trailing_space) { EXPECT_LOOSE_ERROR("v1.2.3 ", 7); }

TEST(v_negative) { EXPECT_LOOSE_ERROR("v-1.2.3", 2); }

TEST(major_only_empty_pre_release) { EXPECT_LOOSE_ERROR("1-", 3); }

TEST(major_only_empty_build) { EXPECT_LOOSE_ERROR("1+", 3); }

TEST(major_minor_empty_pre_release) { EXPECT_LOOSE_ERROR("1.2-", 5); }

TEST(major_minor_empty_build) { EXPECT_LOOSE_ERROR("1.2+", 5); }
