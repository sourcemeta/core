#include <gtest/gtest.h>

#include <sourcemeta/core/css.h>

TEST(CSS_css2_rgb_function, valid_int_basic) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(255, 0, 128)"));
}

TEST(CSS_css2_rgb_function, valid_int_no_spaces) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(255,0,128)"));
}

TEST(CSS_css2_rgb_function, valid_int_extra_whitespace) {
  EXPECT_TRUE(
      sourcemeta::core::is_css2_rgb_function("  rgb (  255 , 0 ,  128  )  "));
}

TEST(CSS_css2_rgb_function, valid_int_tabs_and_newlines) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(\t255,\n0,\r128)"));
}

TEST(CSS_css2_rgb_function, valid_percentage_basic) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(100%, 0%, 50%)"));
}

TEST(CSS_css2_rgb_function, valid_percentage_decimal) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(33.3%, 66.6%, 0%)"));
}

TEST(CSS_css2_rgb_function, valid_percentage_leading_dot) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(.5%, .5%, .5%)"));
}

TEST(CSS_css2_rgb_function, valid_uppercase_function) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("RGB(0,0,0)"));
}

TEST(CSS_css2_rgb_function, valid_mixed_case_function) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("Rgb(0,0,0)"));
}

TEST(CSS_css2_rgb_function, valid_out_of_range_clamped) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(300, -5, 128)"));
}

TEST(CSS_css2_rgb_function, valid_all_zero) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(0, 0, 0)"));
}

TEST(CSS_css2_rgb_function, valid_positive_signed_int) {
  EXPECT_TRUE(sourcemeta::core::is_css2_rgb_function("rgb(+10, +20, +30)"));
}

TEST(CSS_css2_rgb_function, invalid_mixed_types) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb(100%, 0, 50%)"));
}

TEST(CSS_css2_rgb_function, invalid_only_two_values) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb(0, 0)"));
}

TEST(CSS_css2_rgb_function, invalid_four_values) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb(0, 0, 0, 0)"));
}

TEST(CSS_css2_rgb_function, invalid_missing_open_paren) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb 0, 0, 0)"));
}

TEST(CSS_css2_rgb_function, invalid_missing_close_paren) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb(0, 0, 0"));
}

TEST(CSS_css2_rgb_function, invalid_unknown_function) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("hsl(0, 100%, 50%)"));
}

TEST(CSS_css2_rgb_function, invalid_rgba) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgba(0, 0, 0, 1)"));
}

TEST(CSS_css2_rgb_function, invalid_trailing_garbage) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb(0,0,0)foo"));
}

TEST(CSS_css2_rgb_function, invalid_int_with_decimal) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb(0.5, 0, 0)"));
}

TEST(CSS_css2_rgb_function, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function(""));
}

TEST(CSS_css2_rgb_function, invalid_hex_value) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("#000000"));
}

TEST(CSS_css2_rgb_function, invalid_keyword) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("red"));
}

TEST(CSS_css2_rgb_function, invalid_empty_value) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb(, 0, 0)"));
}

TEST(CSS_css2_rgb_function, invalid_dot_only) {
  EXPECT_FALSE(sourcemeta::core::is_css2_rgb_function("rgb(., ., .)"));
}
