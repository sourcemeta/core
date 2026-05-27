#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_nfc_quick_check, ascii_letter) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'A'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(Unicode_nfc_quick_check, ascii_digit) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'0'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(Unicode_nfc_quick_check, ascii_space) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U' '),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(Unicode_nfc_quick_check, null) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(0x0000),
            sourcemeta::core::NFCQuickCheck::Yes);
}

// Latin-1 precomposed character that IS its own NFC form
TEST(Unicode_nfc_quick_check, latin_a_with_grave) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u00C0'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(Unicode_nfc_quick_check, latin_small_u_with_diaeresis) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u00FC'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

// U+0300 COMBINING GRAVE ACCENT may compose with a preceding starter
TEST(Unicode_nfc_quick_check, combining_grave_accent_is_maybe) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u0300'),
            sourcemeta::core::NFCQuickCheck::Maybe);
}

// U+0301 COMBINING ACUTE ACCENT
TEST(Unicode_nfc_quick_check, combining_acute_accent_is_maybe) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u0301'),
            sourcemeta::core::NFCQuickCheck::Maybe);
}

// U+2126 OHM SIGN is a singleton decomposition of U+03A9 GREEK CAPITAL
// LETTER OMEGA, so it never appears in NFC output
TEST(Unicode_nfc_quick_check, ohm_sign_is_no) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u2126'),
            sourcemeta::core::NFCQuickCheck::No);
}

// U+0958 DEVANAGARI LETTER QA decomposes to U+0915 U+093C in NFC
TEST(Unicode_nfc_quick_check, devanagari_qa_is_no) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u0958'),
            sourcemeta::core::NFCQuickCheck::No);
}

// U+FB1D HEBREW LETTER YOD WITH HIRIQ decomposes in NFC
TEST(Unicode_nfc_quick_check, hebrew_yod_with_hiriq_is_no) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\uFB1D'),
            sourcemeta::core::NFCQuickCheck::No);
}

// U+2000 EN QUAD is a singleton decomposition of U+0020 SPACE
TEST(Unicode_nfc_quick_check, en_quad_is_no) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u2000'),
            sourcemeta::core::NFCQuickCheck::No);
}

// Hangul precomposed syllable, allowed in NFC
TEST(Unicode_nfc_quick_check, hangul_syllable_is_yes) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\uAC00'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

// Hangul L jamo (leading consonant): cannot start a precomposed syllable
// alone, but composes only with following jamos so is Yes itself
TEST(Unicode_nfc_quick_check, hangul_l_jamo_is_yes) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u1100'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

// Hangul V jamo (vowel): may compose with a preceding L jamo into a LV
// syllable, so the quick check is Maybe
TEST(Unicode_nfc_quick_check, hangul_v_jamo_is_maybe) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u1161'),
            sourcemeta::core::NFCQuickCheck::Maybe);
}

// Hangul T jamo (trailing consonant): may compose with a preceding LV
// syllable into an LVT syllable
TEST(Unicode_nfc_quick_check, hangul_t_jamo_is_maybe) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u11A8'),
            sourcemeta::core::NFCQuickCheck::Maybe);
}

// Past the Unicode maximum: default per @missing rule is Yes
TEST(Unicode_nfc_quick_check, beyond_max_codepoint) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(0x110000),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(Unicode_nfc_quick_check, beyond_max_codepoint_high) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(0xFFFFFFFF),
            sourcemeta::core::NFCQuickCheck::Yes);
}
