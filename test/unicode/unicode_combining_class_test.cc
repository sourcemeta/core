#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(ascii_letter) { EXPECT_EQ(sourcemeta::core::combining_class(U'A'), 0); }

TEST(ascii_digit) { EXPECT_EQ(sourcemeta::core::combining_class(U'0'), 0); }

TEST(space) { EXPECT_EQ(sourcemeta::core::combining_class(U' '), 0); }

TEST(null) { EXPECT_EQ(sourcemeta::core::combining_class(0x0000), 0); }

TEST(combining_acute_accent) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0301'), 230);
}

TEST(combining_grave_accent_below) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0316'), 220);
}

TEST(combining_cedilla) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0327'), 202);
}

TEST(devanagari_nukta) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u093C'), 7);
}

TEST(devanagari_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u094D'), 9);
}

TEST(bengali_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u09CD'), 9);
}

TEST(gurmukhi_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0A4D'), 9);
}

TEST(malayalam_vertical_bar_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0D3B'), 9);
}

TEST(malayalam_circular_virama) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u0D3C'), 9);
}

TEST(hiragana_voicing_mark) {
  EXPECT_EQ(sourcemeta::core::combining_class(U'\u3099'), 8);
}

TEST(max_codepoint) {
  EXPECT_EQ(sourcemeta::core::combining_class(0x10FFFF), 0);
}

TEST(unassigned_high_codepoint) {
  EXPECT_EQ(sourcemeta::core::combining_class(0x0E000), 0);
}
