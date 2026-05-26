#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_combining_class, ascii_letter) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'A'), 0);
}

TEST(Unicode_combining_class, ascii_digit) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'0'), 0);
}

TEST(Unicode_combining_class, space) {
  EXPECT_EQ(sourcemeta::core::combining_class(U' '), 0);
}

TEST(Unicode_combining_class, null) {
  EXPECT_EQ(sourcemeta::core::combining_class(0x0000), 0);
}

TEST(Unicode_combining_class, combining_acute_accent) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0301'), 230);
}

TEST(Unicode_combining_class, combining_grave_accent_below) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0316'), 220);
}

TEST(Unicode_combining_class, combining_cedilla) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0327'), 202);
}

TEST(Unicode_combining_class, devanagari_nukta) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u093C'), 7);
}

TEST(Unicode_combining_class, devanagari_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u094D'), 9);
}

TEST(Unicode_combining_class, bengali_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u09CD'), 9);
}

TEST(Unicode_combining_class, gurmukhi_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0A4D'), 9);
}

TEST(Unicode_combining_class, malayalam_vertical_bar_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0D3B'), 9);
}

TEST(Unicode_combining_class, malayalam_circular_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0D3C'), 9);
}

TEST(Unicode_combining_class, hiragana_voicing_mark) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u3099'), 8);
}

TEST(Unicode_combining_class, max_codepoint) {
  EXPECT_EQ(sourcemeta::core::combining_class(0x10FFFF), 0);
}

TEST(Unicode_combining_class, unassigned_high_codepoint) {
  EXPECT_EQ(sourcemeta::core::combining_class(0x0E000), 0);
}
