#include <sourcemeta/core/test.h>

#include <sourcemeta/core/unicode.h>

TEST(ascii_letter) { EXPECT_FALSE(sourcemeta::core::is_combining_mark(U'A')); }

TEST(ascii_digit) { EXPECT_FALSE(sourcemeta::core::is_combining_mark(U'0')); }

TEST(null) { EXPECT_FALSE(sourcemeta::core::is_combining_mark(0x0000)); }

TEST(hebrew_letter_alef) {
  EXPECT_FALSE(sourcemeta::core::is_combining_mark(U'\u05D0'));
}

TEST(combining_acute_accent_nonspacing) {
  // U+0301 Mn (Nonspacing_Mark)
  EXPECT_TRUE(sourcemeta::core::is_combining_mark(U'\u0301'));
}

TEST(combining_grave_accent_below_nonspacing) {
  // U+0316 Mn
  EXPECT_TRUE(sourcemeta::core::is_combining_mark(U'\u0316'));
}

TEST(devanagari_virama_nonspacing) {
  // U+094D Mn
  EXPECT_TRUE(sourcemeta::core::is_combining_mark(U'\u094D'));
}

TEST(devanagari_vowel_sign_aa_spacing) {
  // U+093E Mc (Spacing_Mark)
  EXPECT_TRUE(sourcemeta::core::is_combining_mark(U'\u093E'));
}

TEST(devanagari_sign_visarga_spacing) {
  // U+0903 Mc
  EXPECT_TRUE(sourcemeta::core::is_combining_mark(U'\u0903'));
}

TEST(tamil_vowel_sign_e_spacing) {
  // U+0BC6 Mc
  EXPECT_TRUE(sourcemeta::core::is_combining_mark(U'\u0BC6'));
}

TEST(combining_enclosing_circle_enclosing) {
  // U+20DD Me (Enclosing_Mark)
  EXPECT_TRUE(sourcemeta::core::is_combining_mark(U'\u20DD'));
}

TEST(combining_enclosing_square_enclosing) {
  // U+20DE Me
  EXPECT_TRUE(sourcemeta::core::is_combining_mark(U'\u20DE'));
}

TEST(zero_width_joiner_not_mark) {
  // U+200D is bidi class BN but general category Cf (Format), not a mark
  EXPECT_FALSE(sourcemeta::core::is_combining_mark(U'\u200D'));
}

TEST(hangul_letter_not_mark) {
  // U+1100 is Lo (Other_Letter), not a mark
  EXPECT_FALSE(sourcemeta::core::is_combining_mark(U'\u1100'));
}

TEST(emoji_grinning_face_not_mark) {
  EXPECT_FALSE(sourcemeta::core::is_combining_mark(U'\U0001F600'));
}

TEST(unassigned_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_combining_mark(0x0E80));
}

TEST(max_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_combining_mark(0x10FFFF));
}

TEST(above_max_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_combining_mark(0x110000));
}
