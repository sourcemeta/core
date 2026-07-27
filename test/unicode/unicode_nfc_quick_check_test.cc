#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(ascii_letter) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'A'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(ascii_digit) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'0'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(ascii_space) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U' '),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(null) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(0x0000),
            sourcemeta::core::NFCQuickCheck::Yes);
}

// Latin-1 precomposed character that IS its own NFC form
TEST(latin_a_with_grave) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u00C0'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(latin_small_u_with_diaeresis) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u00FC'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

// U+0300 COMBINING GRAVE ACCENT may compose with a preceding starter
TEST(combining_grave_accent_is_maybe) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u0300'),
            sourcemeta::core::NFCQuickCheck::Maybe);
}

// U+0301 COMBINING ACUTE ACCENT
TEST(combining_acute_accent_is_maybe) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u0301'),
            sourcemeta::core::NFCQuickCheck::Maybe);
}

// U+2126 OHM SIGN is a singleton decomposition of U+03A9 GREEK CAPITAL
// LETTER OMEGA, so it never appears in NFC output
TEST(ohm_sign_is_no) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u2126'),
            sourcemeta::core::NFCQuickCheck::No);
}

// U+0958 DEVANAGARI LETTER QA decomposes to U+0915 U+093C in NFC
TEST(devanagari_qa_is_no) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u0958'),
            sourcemeta::core::NFCQuickCheck::No);
}

// U+FB1D HEBREW LETTER YOD WITH HIRIQ decomposes in NFC
TEST(hebrew_yod_with_hiriq_is_no) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\uFB1D'),
            sourcemeta::core::NFCQuickCheck::No);
}

// U+2000 EN QUAD is a singleton decomposition of U+0020 SPACE
TEST(en_quad_is_no) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u2000'),
            sourcemeta::core::NFCQuickCheck::No);
}

// Hangul precomposed syllable, allowed in NFC
TEST(hangul_syllable_is_yes) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\uAC00'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

// Hangul L jamo (leading consonant): cannot start a precomposed syllable
// alone, but composes only with following jamos so is Yes itself
TEST(hangul_l_jamo_is_yes) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u1100'),
            sourcemeta::core::NFCQuickCheck::Yes);
}

// Hangul V jamo (vowel): may compose with a preceding L jamo into a LV
// syllable, so the quick check is Maybe
TEST(hangul_v_jamo_is_maybe) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u1161'),
            sourcemeta::core::NFCQuickCheck::Maybe);
}

// Hangul T jamo (trailing consonant): may compose with a preceding LV
// syllable into an LVT syllable
TEST(hangul_t_jamo_is_maybe) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(U'\u11A8'),
            sourcemeta::core::NFCQuickCheck::Maybe);
}

// Past the Unicode maximum: default per @missing rule is Yes
TEST(beyond_max_codepoint) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(0x110000),
            sourcemeta::core::NFCQuickCheck::Yes);
}

TEST(beyond_max_codepoint_high) {
  EXPECT_EQ(sourcemeta::core::nfc_quick_check(0xFFFFFFFF),
            sourcemeta::core::NFCQuickCheck::Yes);
}
