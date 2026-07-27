#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

TEST(empty) { EXPECT_TRUE(sourcemeta::core::is_nfc(U"")); }

TEST(ascii_letter) { EXPECT_TRUE(sourcemeta::core::is_nfc(U"hello")); }

TEST(ascii_digits_and_punctuation) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(U"abc-123_xyz."));
}

// Precomposed character is in NFC
TEST(precomposed_latin_a_grave) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(U"\u00C0"));
}

TEST(precomposed_latin_u_diaeresis) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(U"\u00FC"));
}

TEST(precomposed_cafe_word) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(U"caf\u00E9"));
}

// Decomposed sequence is not in NFC (quick check returns Maybe for the
// combining mark, slow path compares against the composed form)
TEST(decomposed_latin_a_grave) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"A\u0300"));
}

TEST(decomposed_latin_u_diaeresis) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"u\u0308"));
}

TEST(decomposed_cafe_word) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"cafe\u0301"));
}

// U+2126 OHM SIGN has NFC_QC=No, so the quick check rejects immediately
TEST(ohm_sign_is_not_nfc) { EXPECT_FALSE(sourcemeta::core::is_nfc(U"\u2126")); }

// U+03A9 GREEK CAPITAL OMEGA (the NFC form of OHM SIGN) IS in NFC
TEST(greek_omega_is_nfc) { EXPECT_TRUE(sourcemeta::core::is_nfc(U"\u03A9")); }

// U+212B ANGSTROM SIGN has NFC_QC=No
TEST(angstrom_sign_is_not_nfc) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"\u212B"));
}

// U+0958 DEVANAGARI LETTER QA has NFC_QC=No (decomposes in NFC)
TEST(devanagari_qa_precomposed_is_not_nfc) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"\u0958"));
}

// Decomposed [U+0915, U+093C] IS the NFC form of U+0958
TEST(devanagari_qa_decomposed_is_nfc) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(U"\u0915\u093C"));
}

// Out-of-order combining marks: quick check rejects on the CCC compare.
// a + U+0301 (CCC=230) + U+0316 (CCC=220): 230 > 220 with ccc != 0
TEST(out_of_order_combining_marks) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"a\u0301\u0316"));
}

// In-order combining marks pass the CCC check; with a composable pair the
// slow path confirms the input is not in NFC. Input: a + U+0316 + U+0301.
// NFC would compose a + U+0301 to á (the intervening U+0316 does not
// block since 220 < 230), so the input is NOT in NFC
TEST(in_order_combining_marks_with_composition) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"a\u0316\u0301"));
}

// Hangul precomposed syllable is in NFC
TEST(hangul_precomposed_syllable) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(U"\uAC00"));
}

// Hangul L + V jamo decomposed: not in NFC
TEST(hangul_l_v_decomposed_is_not_nfc) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"\u1100\u1161"));
}

// Hangul L + V + T decomposed: not in NFC
TEST(hangul_l_v_t_decomposed_is_not_nfc) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"\u1100\u1161\u11A8"));
}

// Hangul LV syllable + T (precomposed LV plus T jamo) is not in NFC
TEST(hangul_precomposed_lv_plus_t_is_not_nfc) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(U"\uAC00\u11A8"));
}

// A leading combining mark with no preceding starter is in NFC.
// nfc(input) leaves the combining mark in place
TEST(leading_combining_mark_is_nfc) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(U"\u0301abc"));
}

// Supplementary plane codepoint with no decomposition is in NFC
TEST(supplementary_plane_codepoint) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(U"\U00020000"));
}

// Greek precomposed text is in NFC
TEST(greek_precomposed) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(
      U"\u03C0\u03B1\u03C1\u03AC\u03B4\u03B5\u03B9\u03B3\u03BC\u03B1"));
}

// Greek with ά decomposed to α + acute is not in NFC
TEST(greek_decomposed_is_not_nfc) {
  EXPECT_FALSE(sourcemeta::core::is_nfc(
      U"\u03C0\u03B1\u03C1\u03B1\u0301\u03B4\u03B5\u03B9\u03B3\u03BC\u03B1"));
}

// is_nfc(nfc(x)) is always true (NFC is idempotent and a fixed point)
TEST(after_nfc_is_nfc) {
  EXPECT_TRUE(sourcemeta::core::is_nfc(sourcemeta::core::nfc(U"A\u0300")));
  EXPECT_TRUE(sourcemeta::core::is_nfc(sourcemeta::core::nfc(U"\u1100\u1161")));
  EXPECT_TRUE(sourcemeta::core::is_nfc(sourcemeta::core::nfc(U"\u2126")));
}
