#include <sourcemeta/core/test.h>

#include <sourcemeta/core/idna.h>

TEST(zwj_preceded_by_devanagari_virama) {
  // Devanagari KA (U+0915) + VIRAMA (U+094D) + ZWJ (U+200D)
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(U"\u0915\u094D\u200D", 2));
}

TEST(zwj_preceded_by_bengali_virama) {
  // Bengali KA (U+0995) + VIRAMA (U+09CD) + ZWJ (U+200D)
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(U"\u0995\u09CD\u200D", 2));
}

TEST(zwj_preceded_by_non_virama_letter) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"a\u200D", 1));
}

TEST(zwj_preceded_by_devanagari_letter_without_virama) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"\u0915\u200D", 1));
}

TEST(zwj_at_start_of_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"\u200D", 0));
}

TEST(zwnj_preceded_by_devanagari_virama) {
  // Devanagari KA + VIRAMA + ZWNJ
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(U"\u0915\u094D\u200C", 2));
}

TEST(zwnj_between_arabic_dual_joining) {
  // Arabic BEH (D) + ZWNJ + Arabic BEH (D)
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(U"\u0628\u200C\u0628", 1));
}

TEST(zwnj_between_arabic_dual_and_right_joining) {
  // Arabic BEH (D) + ZWNJ + Arabic ALEF (R)
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(U"\u0628\u200C\u0627", 1));
}

TEST(zwnj_with_transparent_before) {
  // Arabic BEH (D) + COMBINING ACUTE (T) + ZWNJ + Arabic ALEF (R)
  EXPECT_TRUE(
      sourcemeta::core::idna_passes_contextj(U"\u0628\u0301\u200C\u0627", 2));
}

TEST(zwnj_with_transparent_after) {
  // Arabic BEH (D) + ZWNJ + COMBINING ACUTE (T) + Arabic ALEF (R)
  EXPECT_TRUE(
      sourcemeta::core::idna_passes_contextj(U"\u0628\u200C\u0301\u0627", 1));
}

TEST(zwnj_with_transparents_both_sides) {
  // Arabic BEH (D) + COMBINING ACUTE (T) + ZWNJ + COMBINING ACUTE (T) + Arabic
  // ALEF (R)
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(
      U"\u0628\u0301\u200C\u0301\u0627", 2));
}

TEST(zwnj_with_no_joining_context) {
  // ASCII letters have Non_Joining type, so no L|D before or R|D after.
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"a\u200Cb", 1));
}

TEST(zwnj_at_start_of_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"\u200C\u0627", 0));
}

TEST(zwnj_at_end_of_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"\u0628\u200C", 1));
}

TEST(zwnj_only_backward_joining_match) {
  // Arabic BEH (D) + ZWNJ + ASCII a (Non_Joining)
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"\u0628\u200Ca", 1));
}

TEST(zwnj_only_forward_joining_match) {
  // ASCII a (Non_Joining) + ZWNJ + Arabic ALEF (R)
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"a\u200C\u0627", 1));
}

TEST(ascii_letter_has_no_rule) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(U"abc", 0));
}

TEST(devanagari_letter_has_no_rule) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(U"\u0905", 0));
}

TEST(middle_dot_has_no_contextj_rule) {
  // U+00B7 has a ContextO rule but not ContextJ
  EXPECT_TRUE(sourcemeta::core::idna_passes_contextj(U"l\u00B7l", 1));
}

TEST(position_out_of_range) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"abc", 10));
}

TEST(empty_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_contextj(U"", 0));
}
