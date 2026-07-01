#include <sourcemeta/core/css.h>
#include <sourcemeta/core/test.h>

TEST(suite_valid_keyword_fuchsia) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("fuchsia"));
}

TEST(suite_valid_six_digit_hex) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("#CC8899"));
}

TEST(suite_valid_three_digit_hex) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("#C89"));
}

TEST(suite_invalid_eight_digit_hex) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("#00332520"));
}

TEST(suite_invalid_unknown_keyword) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("puce"));
}

TEST(suite_invalid_underscore_dash) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("light_grayish_red-violet"));
}

TEST(valid_rgb_int) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("rgb(255, 0, 0)"));
}

TEST(valid_rgb_percent) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color("rgb(100%, 0%, 0%)"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_css2_color("")); }

TEST(invalid_random) { EXPECT_FALSE(sourcemeta::core::is_css2_color("hello")); }

TEST(invalid_transparent_keyword) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("transparent"));
}

TEST(invalid_rgba_function) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color("rgba(0, 0, 0, 1)"));
}
