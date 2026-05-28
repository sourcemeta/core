#include <gtest/gtest.h>

#include <sourcemeta/core/css.h>

TEST(CSS_css2_color_keyword, valid_aqua) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("aqua"));
}

TEST(CSS_css2_color_keyword, valid_black) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("black"));
}

TEST(CSS_css2_color_keyword, valid_blue) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("blue"));
}

TEST(CSS_css2_color_keyword, valid_fuchsia) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("fuchsia"));
}

TEST(CSS_css2_color_keyword, valid_gray) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("gray"));
}

TEST(CSS_css2_color_keyword, valid_green) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("green"));
}

TEST(CSS_css2_color_keyword, valid_lime) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("lime"));
}

TEST(CSS_css2_color_keyword, valid_maroon) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("maroon"));
}

TEST(CSS_css2_color_keyword, valid_navy) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("navy"));
}

TEST(CSS_css2_color_keyword, valid_olive) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("olive"));
}

TEST(CSS_css2_color_keyword, valid_orange) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("orange"));
}

TEST(CSS_css2_color_keyword, valid_purple) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("purple"));
}

TEST(CSS_css2_color_keyword, valid_red) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("red"));
}

TEST(CSS_css2_color_keyword, valid_silver) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("silver"));
}

TEST(CSS_css2_color_keyword, valid_teal) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("teal"));
}

TEST(CSS_css2_color_keyword, valid_white) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("white"));
}

TEST(CSS_css2_color_keyword, valid_yellow) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("yellow"));
}

TEST(CSS_css2_color_keyword, valid_uppercase_red) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("RED"));
}

TEST(CSS_css2_color_keyword, valid_mixed_case_fuchsia) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("FuChSiA"));
}

TEST(CSS_css2_color_keyword, invalid_unknown_puce) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("puce"));
}

TEST(CSS_css2_color_keyword, invalid_underscore_dash) {
  EXPECT_FALSE(
      sourcemeta::core::is_css2_color_keyword("light_grayish_red-violet"));
}

TEST(CSS_css2_color_keyword, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword(""));
}

TEST(CSS_css2_color_keyword, invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword(" red"));
}

TEST(CSS_css2_color_keyword, invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("red "));
}

TEST(CSS_css2_color_keyword, invalid_transparent) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("transparent"));
}

TEST(CSS_css2_color_keyword, invalid_system_color) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("ButtonFace"));
}

TEST(CSS_css2_color_keyword, invalid_extended_x11) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("papayawhip"));
}

TEST(CSS_css2_color_keyword, invalid_hex_value) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("#C89"));
}
