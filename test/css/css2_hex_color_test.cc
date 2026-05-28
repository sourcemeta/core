#include <gtest/gtest.h>

#include <sourcemeta/core/css.h>

TEST(CSS_css2_hex_color, valid_six_digit_lower) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#cc8899"));
}

TEST(CSS_css2_hex_color, valid_six_digit_upper) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#CC8899"));
}

TEST(CSS_css2_hex_color, valid_six_digit_mixed) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#Cc88Aa"));
}

TEST(CSS_css2_hex_color, valid_three_digit) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#C89"));
}

TEST(CSS_css2_hex_color, valid_three_digit_all_zero) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#000"));
}

TEST(CSS_css2_hex_color, valid_three_digit_all_f) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#fff"));
}

TEST(CSS_css2_hex_color, invalid_missing_hash) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("cc8899"));
}

TEST(CSS_css2_hex_color, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color(""));
}

TEST(CSS_css2_hex_color, invalid_only_hash) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#"));
}

TEST(CSS_css2_hex_color, invalid_two_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc"));
}

TEST(CSS_css2_hex_color, invalid_four_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc88"));
}

TEST(CSS_css2_hex_color, invalid_five_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc889"));
}

TEST(CSS_css2_hex_color, invalid_seven_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc8899e"));
}

TEST(CSS_css2_hex_color, invalid_eight_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#00332520"));
}

TEST(CSS_css2_hex_color, invalid_non_hex_letter) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#zzgggg"));
}

TEST(CSS_css2_hex_color, invalid_internal_whitespace) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("# cc8899"));
}

TEST(CSS_css2_hex_color, invalid_trailing_whitespace) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc8899 "));
}

TEST(CSS_css2_hex_color, invalid_with_named_keyword) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("red"));
}

TEST(CSS_css2_hex_color, invalid_with_rgb_function) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("rgb(0,0,0)"));
}
