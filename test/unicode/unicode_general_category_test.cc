#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(null) {
  EXPECT_EQ(sourcemeta::core::general_category(0x0000),
            sourcemeta::core::GeneralCategory::Control);
}

TEST(delete_control) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u007F'),
            sourcemeta::core::GeneralCategory::Control);
}

TEST(c1_control) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u009F'),
            sourcemeta::core::GeneralCategory::Control);
}

TEST(ascii_uppercase_letter) {
  EXPECT_EQ(sourcemeta::core::general_category(U'A'),
            sourcemeta::core::GeneralCategory::UppercaseLetter);
}

TEST(ascii_lowercase_letter) {
  EXPECT_EQ(sourcemeta::core::general_category(U'z'),
            sourcemeta::core::GeneralCategory::LowercaseLetter);
}

TEST(latin_capital_d_with_small_z_with_caron) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u01C5'),
            sourcemeta::core::GeneralCategory::TitlecaseLetter);
}

TEST(greek_capital_alpha_with_prosgegrammeni) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u1FBC'),
            sourcemeta::core::GeneralCategory::TitlecaseLetter);
}

TEST(modifier_letter_small_h) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u02B0'),
            sourcemeta::core::GeneralCategory::ModifierLetter);
}

TEST(ideographic_iteration_mark) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u3005'),
            sourcemeta::core::GeneralCategory::ModifierLetter);
}

TEST(hebrew_letter_alef) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u05D0'),
            sourcemeta::core::GeneralCategory::OtherLetter);
}

TEST(cjk_unified_ideograph) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u4E00'),
            sourcemeta::core::GeneralCategory::OtherLetter);
}

TEST(hangul_syllable) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\uAC00'),
            sourcemeta::core::GeneralCategory::OtherLetter);
}

TEST(combining_acute_accent) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u0301'),
            sourcemeta::core::GeneralCategory::NonspacingMark);
}

TEST(devanagari_sign_visarga) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u0903'),
            sourcemeta::core::GeneralCategory::SpacingMark);
}

TEST(combining_enclosing_circle) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u20DD'),
            sourcemeta::core::GeneralCategory::EnclosingMark);
}

TEST(ascii_digit) {
  EXPECT_EQ(sourcemeta::core::general_category(U'0'),
            sourcemeta::core::GeneralCategory::DecimalNumber);
}

TEST(arabic_indic_digit_zero) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u0660'),
            sourcemeta::core::GeneralCategory::DecimalNumber);
}

TEST(roman_numeral_one) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u2160'),
            sourcemeta::core::GeneralCategory::LetterNumber);
}

TEST(superscript_two) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u00B2'),
            sourcemeta::core::GeneralCategory::OtherNumber);
}

TEST(low_line) {
  EXPECT_EQ(sourcemeta::core::general_category(U'_'),
            sourcemeta::core::GeneralCategory::ConnectorPunctuation);
}

TEST(hyphen_minus) {
  EXPECT_EQ(sourcemeta::core::general_category(U'-'),
            sourcemeta::core::GeneralCategory::DashPunctuation);
}

TEST(em_dash) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u2014'),
            sourcemeta::core::GeneralCategory::DashPunctuation);
}

TEST(left_parenthesis) {
  EXPECT_EQ(sourcemeta::core::general_category(U'('),
            sourcemeta::core::GeneralCategory::OpenPunctuation);
}

TEST(right_parenthesis) {
  EXPECT_EQ(sourcemeta::core::general_category(U')'),
            sourcemeta::core::GeneralCategory::ClosePunctuation);
}

TEST(left_pointing_double_angle_quotation_mark) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u00AB'),
            sourcemeta::core::GeneralCategory::InitialPunctuation);
}

TEST(right_pointing_double_angle_quotation_mark) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u00BB'),
            sourcemeta::core::GeneralCategory::FinalPunctuation);
}

TEST(exclamation_mark) {
  EXPECT_EQ(sourcemeta::core::general_category(U'!'),
            sourcemeta::core::GeneralCategory::OtherPunctuation);
}

TEST(plus_sign) {
  EXPECT_EQ(sourcemeta::core::general_category(U'+'),
            sourcemeta::core::GeneralCategory::MathSymbol);
}

TEST(dollar_sign) {
  EXPECT_EQ(sourcemeta::core::general_category(U'$'),
            sourcemeta::core::GeneralCategory::CurrencySymbol);
}

TEST(euro_sign) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u20AC'),
            sourcemeta::core::GeneralCategory::CurrencySymbol);
}

TEST(circumflex_accent) {
  EXPECT_EQ(sourcemeta::core::general_category(U'^'),
            sourcemeta::core::GeneralCategory::ModifierSymbol);
}

TEST(copyright_sign) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u00A9'),
            sourcemeta::core::GeneralCategory::OtherSymbol);
}

TEST(grinning_face) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\U0001F600'),
            sourcemeta::core::GeneralCategory::OtherSymbol);
}

TEST(space) {
  EXPECT_EQ(sourcemeta::core::general_category(U' '),
            sourcemeta::core::GeneralCategory::SpaceSeparator);
}

TEST(no_break_space) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u00A0'),
            sourcemeta::core::GeneralCategory::SpaceSeparator);
}

TEST(ogham_space_mark) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u1680'),
            sourcemeta::core::GeneralCategory::SpaceSeparator);
}

TEST(en_quad) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u2000'),
            sourcemeta::core::GeneralCategory::SpaceSeparator);
}

TEST(hair_space) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u200A'),
            sourcemeta::core::GeneralCategory::SpaceSeparator);
}

TEST(narrow_no_break_space) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u202F'),
            sourcemeta::core::GeneralCategory::SpaceSeparator);
}

TEST(medium_mathematical_space) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u205F'),
            sourcemeta::core::GeneralCategory::SpaceSeparator);
}

TEST(ideographic_space) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u3000'),
            sourcemeta::core::GeneralCategory::SpaceSeparator);
}

TEST(line_separator) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u2028'),
            sourcemeta::core::GeneralCategory::LineSeparator);
}

TEST(paragraph_separator) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u2029'),
            sourcemeta::core::GeneralCategory::ParagraphSeparator);
}

TEST(soft_hyphen) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u00AD'),
            sourcemeta::core::GeneralCategory::Format);
}

TEST(zero_width_joiner) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u200D'),
            sourcemeta::core::GeneralCategory::Format);
}

TEST(zero_width_no_break_space) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\uFEFF'),
            sourcemeta::core::GeneralCategory::Format);
}

TEST(first_high_surrogate) {
  EXPECT_EQ(sourcemeta::core::general_category(0xD800),
            sourcemeta::core::GeneralCategory::Surrogate);
}

TEST(last_low_surrogate) {
  EXPECT_EQ(sourcemeta::core::general_category(0xDFFF),
            sourcemeta::core::GeneralCategory::Surrogate);
}

TEST(first_private_use) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\uE000'),
            sourcemeta::core::GeneralCategory::PrivateUse);
}

TEST(last_basic_private_use) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\uF8FF'),
            sourcemeta::core::GeneralCategory::PrivateUse);
}

TEST(first_supplementary_private_use_area_a) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\U000F0000'),
            sourcemeta::core::GeneralCategory::PrivateUse);
}

TEST(last_supplementary_private_use_area_b) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\U0010FFFD'),
            sourcemeta::core::GeneralCategory::PrivateUse);
}

TEST(reserved_greek) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u0378'),
            sourcemeta::core::GeneralCategory::Unassigned);
}

TEST(noncharacter_fdd0) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\uFDD0'),
            sourcemeta::core::GeneralCategory::Unassigned);
}

TEST(noncharacter_ffff) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\uFFFF'),
            sourcemeta::core::GeneralCategory::Unassigned);
}

TEST(unassigned_plane_five) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\U00050000'),
            sourcemeta::core::GeneralCategory::Unassigned);
}

TEST(saudi_riyal_sign) {
  EXPECT_EQ(sourcemeta::core::general_category(U'\u20C1'),
            sourcemeta::core::GeneralCategory::CurrencySymbol);
}

TEST(max_codepoint) {
  EXPECT_EQ(sourcemeta::core::general_category(0x10FFFF),
            sourcemeta::core::GeneralCategory::Unassigned);
}

TEST(above_max_codepoint) {
  EXPECT_EQ(sourcemeta::core::general_category(0x110000),
            sourcemeta::core::GeneralCategory::Unassigned);
}

TEST(above_max_codepoint_high) {
  EXPECT_EQ(sourcemeta::core::general_category(0xFFFFFFFF),
            sourcemeta::core::GeneralCategory::Unassigned);
}
