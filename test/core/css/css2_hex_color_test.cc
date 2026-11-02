#include <sourcemeta/core/css.h>
#include <sourcemeta/core/test.h>

TEST(valid_six_digit_lower) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#cc8899"));
}

TEST(valid_six_digit_upper) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#CC8899"));
}

TEST(valid_six_digit_mixed) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#Cc88Aa"));
}

TEST(valid_three_digit) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#C89"));
}

TEST(valid_three_digit_all_zero) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#000"));
}

TEST(valid_three_digit_all_f) {
  EXPECT_TRUE(sourcemeta::core::is_css2_hex_color("#fff"));
}

TEST(invalid_missing_hash) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("cc8899"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("")); }

TEST(invalid_only_hash) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#"));
}

TEST(invalid_two_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc"));
}

TEST(invalid_four_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc88"));
}

TEST(invalid_five_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc889"));
}

TEST(invalid_seven_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc8899e"));
}

TEST(invalid_eight_digits) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#00332520"));
}

TEST(invalid_non_hex_letter) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#zzgggg"));
}

TEST(invalid_internal_whitespace) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("# cc8899"));
}

TEST(invalid_trailing_whitespace) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("#cc8899 "));
}

TEST(invalid_with_named_keyword) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("red"));
}

TEST(invalid_with_rgb_function) {
  EXPECT_FALSE(sourcemeta::core::is_css2_hex_color("rgb(0,0,0)"));
}
