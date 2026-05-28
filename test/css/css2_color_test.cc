#include <gtest/gtest.h>

#include <sourcemeta/core/css.h>

TEST(CSS_css2_color, suite_valid_keyword_fuchsia) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("fuchsia"));
}

TEST(CSS_css2_color, suite_valid_six_digit_hex) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("#CC8899"));
}

TEST(CSS_css2_color, suite_valid_three_digit_hex) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("#C89"));
}

TEST(CSS_css2_color, suite_invalid_eight_digit_hex) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("#00332520"));
}

TEST(CSS_css2_color, suite_invalid_unknown_keyword) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("puce"));
}

TEST(CSS_css2_color, suite_invalid_underscore_dash) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("light_grayish_red-violet"));
}

TEST(CSS_css2_color, valid_rgb_int) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("rgb(255, 0, 0)"));
}

TEST(CSS_css2_color, valid_rgb_percent) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("rgb(100%, 0%, 0%)"));
}

TEST(CSS_css2_color, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color(""));
}

TEST(CSS_css2_color, invalid_random) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("hello"));
}

TEST(CSS_css2_color, invalid_transparent_keyword) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("transparent"));
}

TEST(CSS_css2_color, invalid_rgba_function) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("rgba(0, 0, 0, 1)"));
}
