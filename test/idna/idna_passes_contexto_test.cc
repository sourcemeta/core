#include <sourcemeta/core/test.h>

#include <sourcemeta/core/idna.h>

TEST(middle_dot_between_latin_l) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"l\u00B7l", 1));
}

TEST(middle_dot_not_flanked_by_l) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"a\u00B7b", 1));
}

TEST(middle_dot_only_preceded_by_l) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"l\u00B7a", 1));
}

TEST(middle_dot_only_followed_by_l) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"a\u00B7l", 1));
}

TEST(middle_dot_at_start_of_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"\u00B7l", 0));
}

TEST(middle_dot_at_end_of_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"l\u00B7", 1));
}

TEST(greek_keraia_followed_by_greek) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u0375\u0391", 0));
}

TEST(greek_keraia_followed_by_latin) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"\u0375a", 0));
}

TEST(greek_keraia_at_end_of_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"a\u0375", 1));
}

TEST(hebrew_geresh_preceded_by_hebrew) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u05D0\u05F3", 1));
}

TEST(hebrew_geresh_preceded_by_latin) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"a\u05F3", 1));
}

TEST(hebrew_geresh_at_start_of_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"\u05F3a", 0));
}

TEST(hebrew_gershayim_preceded_by_hebrew) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u05D0\u05F4", 1));
}

TEST(hebrew_gershayim_preceded_by_latin) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"a\u05F4", 1));
}

TEST(hebrew_gershayim_at_start_of_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"\u05F4a", 0));
}

TEST(katakana_middle_dot_with_katakana) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u30A2\u30FB\u30A4", 1));
}

TEST(katakana_middle_dot_with_hiragana) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u3042\u30FB\u3044", 1));
}

TEST(katakana_middle_dot_with_han) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u6F22\u30FB\u5B57", 1));
}

TEST(katakana_middle_dot_with_only_latin) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"a\u30FBb", 1));
}

TEST(katakana_middle_dot_alone) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"\u30FB", 0));
}

TEST(arabic_indic_digits_only) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u0660\u0661\u0662", 0));
}

TEST(arabic_indic_digit_mixed_with_extended) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"\u0660\u06F0", 0));
}

TEST(extended_arabic_indic_digits_only) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u06F0\u06F1\u06F2", 0));
}

TEST(extended_arabic_indic_mixed_with_standard) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"\u06F0\u0660", 0));
}

TEST(position_out_of_range) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"abc", 10));
}

TEST(empty_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contexto(U"", 0));
}

TEST(ascii_letter_has_no_rule) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"abc", 0));
}

TEST(ascii_digit_has_no_rule) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"a0b", 1));
}

TEST(devanagari_letter_has_no_rule) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u0905", 0));
}

TEST(zero_width_joiner_has_no_contexto_rule) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contexto(U"\u200D", 0));
}
