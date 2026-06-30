#include <sourcemeta/core/test.h>

#include <sourcemeta/core/css.h>

TEST(valid_aqua) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("aqua"));
}

TEST(valid_black) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("black"));
}

TEST(valid_blue) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("blue"));
}

TEST(valid_fuchsia) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("fuchsia"));
}

TEST(valid_gray) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("gray"));
}

TEST(valid_green) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("green"));
}

TEST(valid_lime) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("lime"));
}

TEST(valid_maroon) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("maroon"));
}

TEST(valid_navy) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("navy"));
}

TEST(valid_olive) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("olive"));
}

TEST(valid_orange) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("orange"));
}

TEST(valid_purple) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("purple"));
}

TEST(valid_red) { EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("red")); }

TEST(valid_silver) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("silver"));
}

TEST(valid_teal) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("teal"));
}

TEST(valid_white) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("white"));
}

TEST(valid_yellow) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("yellow"));
}

TEST(valid_uppercase_red) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("RED"));
}

TEST(valid_mixed_case_fuchsia) {
  EXPECT_TRUE(sourcemeta::core::is_css2_color_keyword("FuChSiA"));
}

TEST(invalid_unknown_puce) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("puce"));
}

TEST(invalid_underscore_dash) {
  EXPECT_FALSE(
      sourcemeta::core::is_css2_color_keyword("light_grayish_red-violet"));
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword(""));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword(" red"));
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("red "));
}

TEST(invalid_transparent) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("transparent"));
}

TEST(invalid_system_color) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("ButtonFace"));
}

TEST(invalid_extended_x11) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("papayawhip"));
}

TEST(invalid_hex_value) {
  EXPECT_FALSE(sourcemeta::core::is_css2_color_keyword("#C89"));
}
