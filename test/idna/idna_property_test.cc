#include <gtest/gtest.h>

#include <sourcemeta/core/idna.h>

TEST(IDNA_property, ascii_letter) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'a'),
            sourcemeta::core::IDNAProperty::PValid);
}

TEST(IDNA_property, ascii_digit) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'0'),
            sourcemeta::core::IDNAProperty::PValid);
}

TEST(IDNA_property, ascii_hyphen) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'-'),
            sourcemeta::core::IDNAProperty::PValid);
}

TEST(IDNA_property, ascii_uppercase_letter_disallowed) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'A'),
            sourcemeta::core::IDNAProperty::Disallowed);
}

TEST(IDNA_property, ascii_full_stop_disallowed) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'.'),
            sourcemeta::core::IDNAProperty::Disallowed);
}

TEST(IDNA_property, control_character_disallowed) {
  EXPECT_EQ(sourcemeta::core::idna_property(0x0007),
            sourcemeta::core::IDNAProperty::Disallowed);
}

TEST(IDNA_property, hebrew_letter_alef_pvalid) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u05D0'),
            sourcemeta::core::IDNAProperty::PValid);
}

TEST(IDNA_property, arabic_letter_alef_pvalid) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u0627'),
            sourcemeta::core::IDNAProperty::PValid);
}

TEST(IDNA_property, devanagari_letter_pvalid) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u0905'),
            sourcemeta::core::IDNAProperty::PValid);
}

TEST(IDNA_property, zero_width_joiner_contextj) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u200D'),
            sourcemeta::core::IDNAProperty::ContextJ);
}

TEST(IDNA_property, zero_width_non_joiner_contextj) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u200C'),
            sourcemeta::core::IDNAProperty::ContextJ);
}

TEST(IDNA_property, middle_dot_contexto) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u00B7'),
            sourcemeta::core::IDNAProperty::ContextO);
}

TEST(IDNA_property, greek_lower_numeral_sign_contexto) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u0375'),
            sourcemeta::core::IDNAProperty::ContextO);
}

TEST(IDNA_property, hebrew_geresh_contexto) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u05F3'),
            sourcemeta::core::IDNAProperty::ContextO);
}

TEST(IDNA_property, hebrew_gershayim_contexto) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u05F4'),
            sourcemeta::core::IDNAProperty::ContextO);
}

TEST(IDNA_property, katakana_middle_dot_contexto) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u30FB'),
            sourcemeta::core::IDNAProperty::ContextO);
}

TEST(IDNA_property, arabic_indic_digit_zero_contexto) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u0660'),
            sourcemeta::core::IDNAProperty::ContextO);
}

TEST(IDNA_property, extended_arabic_indic_digit_zero_contexto) {
  EXPECT_EQ(sourcemeta::core::idna_property(U'\u06F0'),
            sourcemeta::core::IDNAProperty::ContextO);
}

TEST(IDNA_property, ltr_mark_disallowed) {
  EXPECT_EQ(sourcemeta::core::idna_property(0x200E),
            sourcemeta::core::IDNAProperty::Disallowed);
}

TEST(IDNA_property, rtl_mark_disallowed) {
  EXPECT_EQ(sourcemeta::core::idna_property(0x200F),
            sourcemeta::core::IDNAProperty::Disallowed);
}

TEST(IDNA_property, unassigned_in_lao_block) {
  EXPECT_EQ(sourcemeta::core::idna_property(0x0E80),
            sourcemeta::core::IDNAProperty::Unassigned);
}

TEST(IDNA_property, max_codepoint_disallowed_noncharacter) {
  EXPECT_EQ(sourcemeta::core::idna_property(0x10FFFF),
            sourcemeta::core::IDNAProperty::Disallowed);
}

TEST(IDNA_property, above_max_codepoint_unassigned) {
  EXPECT_EQ(sourcemeta::core::idna_property(0x110000),
            sourcemeta::core::IDNAProperty::Unassigned);
}
