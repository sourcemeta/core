#include <sourcemeta/core/idna.h>
#include <sourcemeta/core/test.h>

TEST(condition_1_first_char_latin_letter) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"abc"));
}

TEST(condition_1_first_char_hebrew_letter) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0"));
}

TEST(condition_1_first_char_arabic_letter) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u0627"));
}

TEST(condition_1_first_char_ascii_digit_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"0abc"));
}

TEST(condition_1_first_char_combining_mark_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"\u0301abc"));
}

TEST(condition_1_first_char_punctuation_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"@abc"));
}

TEST(condition_1_first_char_arabic_indic_digit_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"\u0660"));
}

TEST(condition_2_rtl_with_allowed_mix) {
  // Hebrew + ASCII digits + Hebrew
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0123\u05D0"));
}

TEST(condition_2_rtl_with_latin_letter_rejected) {
  // RTL label cannot contain L
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0a\u05D0"));
}

TEST(condition_2_rtl_with_paragraph_separator_rejected) {
  // U+2029 is bidi class B, not allowed
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0\u2029\u05D0"));
}

TEST(condition_2_rtl_with_lrm_isolate_rejected) {
  // U+2066 is LRI, not allowed
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0\u2066\u05D0"));
}

TEST(condition_3_rtl_ends_with_hebrew) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0\u05D0"));
}

TEST(condition_3_rtl_ends_with_arabic) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u0627\u0627"));
}

TEST(condition_3_rtl_ends_with_ascii_digit) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D01"));
}

TEST(condition_3_rtl_ends_with_arabic_indic_digit) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u0627\u0660"));
}

TEST(condition_3_rtl_ends_with_trailing_nsm) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0\u0301"));
}

TEST(condition_3_rtl_ends_with_multiple_trailing_nsms) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0\u0301\u0301"));
}

TEST(condition_3_rtl_ends_with_punctuation_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0!"));
}

TEST(condition_3_rtl_ends_with_comma_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0,"));
}

TEST(condition_4_rtl_with_only_european_numbers) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D012"));
}

TEST(condition_4_rtl_with_only_arabic_numbers) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u0627\u0660\u0661"));
}

TEST(condition_4_rtl_with_both_en_and_an_rejected) {
  // Hebrew + ASCII digit + Arabic-Indic digit
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D01\u0660"));
}

TEST(condition_5_ltr_with_allowed_mix) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"abc123abc"));
}

TEST(condition_5_ltr_with_hebrew_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"abc\u05D0"));
}

TEST(condition_5_ltr_with_arabic_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"abc\u0627"));
}

TEST(condition_5_ltr_with_arabic_indic_digit_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"abc\u0660"));
}

TEST(condition_5_ltr_with_paragraph_separator_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"abc\u2029abc"));
}

TEST(condition_6_ltr_ends_with_letter) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"abc"));
}

TEST(condition_6_ltr_ends_with_digit) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"abc123"));
}

TEST(condition_6_ltr_ends_with_trailing_nsm) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"abc\u0301"));
}

TEST(condition_6_ltr_ends_with_punctuation_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"abc!"));
}

TEST(condition_6_ltr_ends_with_comma_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U"abc,"));
}

TEST(empty_label) {
  EXPECT_FALSE(sourcemeta::core::idna_passes_bidi_rule(U""));
}

TEST(single_latin_letter) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"a"));
}

TEST(single_hebrew_letter) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0"));
}

TEST(single_arabic_letter) {
  EXPECT_TRUE(sourcemeta::core::idna_passes_bidi_rule(U"\u0627"));
}
