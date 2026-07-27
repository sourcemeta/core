#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string>

TEST(empty_input) { EXPECT_EQ(sourcemeta::core::nfc(U""), U""); }

TEST(ascii_passthrough) {
  EXPECT_EQ(sourcemeta::core::nfc(U"hello world"), U"hello world");
}

// A precomposed character that IS its own NFC form stays unchanged
TEST(precomposed_a_grave_unchanged) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u00C0"), U"\u00C0");
}

TEST(precomposed_u_diaeresis_unchanged) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u00FC"), U"\u00FC");
}

// A decomposed sequence canonically composes into its precomposed form
TEST(decomposed_a_grave_composes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"A\u0300"), U"\u00C0");
}

TEST(decomposed_u_diaeresis_composes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"u\u0308"), U"\u00FC");
}

// "café" decomposes to "cafe" + combining acute, which composes
// into "café"
TEST(multiple_composing_pairs) {
  EXPECT_EQ(sourcemeta::core::nfc(U"cafe\u0301"), U"caf\u00E9");
}

// Out-of-order combining marks are canonically reordered before composition.
// Input: a + U+0301 (acute, CCC=230) + U+0316 (grave below, CCC=220).
// After reorder: a + U+0316 + U+0301. After compose: a + U+0316 has no
// composition so U+0316 stays, then a + U+0301 composes (220 < 230, the
// intervening mark does not block). Result: á̖
TEST(combining_marks_reordered) {
  EXPECT_EQ(sourcemeta::core::nfc(U"a\u0301\u0316"), U"\u00E1\u0316");
}

// U+1E9B LATIN SMALL LETTER LONG S WITH DOT ABOVE decomposes recursively
// to [U+017F, U+0307] which then composes back to U+1E9B
TEST(recursive_decompose_then_compose) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u1E9B"), U"\u1E9B");
}

// U+212B ANGSTROM SIGN has a singleton canonical decomposition to U+00C5.
// NFC discards the ANGSTROM SIGN form and produces U+00C5
TEST(singleton_angstrom_to_latin_a_with_ring) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u212B"), U"\u00C5");
}

// U+2126 OHM SIGN has a singleton canonical decomposition to U+03A9
// GREEK CAPITAL LETTER OMEGA. NFC produces U+03A9
TEST(singleton_ohm_to_greek_omega) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u2126"), U"\u03A9");
}

// U+0958 DEVANAGARI LETTER QA decomposes to [U+0915, U+093C] but is in the
// composition exclusion list, so it does not recompose
TEST(composition_exclusion_devanagari_qa_decomposes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u0958"), U"\u0915\u093C");
}

// Decomposed [U+0915, U+093C] also stays decomposed for the same reason
TEST(composition_exclusion_devanagari_qa_stays_decomposed) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u0915\u093C"), U"\u0915\u093C");
}

// U+09DC BENGALI LETTER RRA: same exclusion pattern
TEST(composition_exclusion_bengali_rra) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u09DC"), U"\u09A1\u09BC");
}

// Hangul precomposed syllable stays precomposed
TEST(hangul_precomposed_unchanged) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\uAC00"), U"\uAC00");
}

// Hangul L jamo + V jamo composes algorithmically to an LV syllable
TEST(hangul_l_v_composes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u1100\u1161"), U"\uAC00");
}

// Hangul L + V + T composes algorithmically to an LVT syllable
TEST(hangul_l_v_t_composes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u1100\u1161\u11A8"), U"\uAC01");
}

// Multi-syllable decomposed Hangul
TEST(hangul_word_composes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u1100\u1161\u11AB\u1100\u1175\u11AF"),
            U"\uAC04\uAE38");
}

// A precomposed LV syllable plus a separate T jamo composes into an LVT
TEST(hangul_precomposed_lv_plus_t_composes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\uAC00\u11A8"), U"\uAC01");
}

// A combining mark with no preceding starter is left in place
TEST(leading_combining_mark) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u0301abc"), U"\u0301abc");
}

// Input: A + U+0308 (diaeresis, CCC=230) + U+0301 (acute, CCC=230). After
// reorder (stable, same CCC): unchanged. A + U+0308 composes to Ä.
// Ä + U+0301 has no precomposed form, so the acute stays separate
TEST(two_same_class_marks_first_composes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"A\u0308\u0301"), U"\u00C4\u0301");
}

// NFC is idempotent: applying it twice yields the same result
TEST(idempotent_on_already_composed) {
  const std::u32string input{U"caf\u00E9\u00C0"};
  EXPECT_EQ(sourcemeta::core::nfc(input), input);
}

TEST(idempotent_on_complex_input) {
  const auto once{sourcemeta::core::nfc(U"A\u0301\u0316\u00C0\u1100\u1161")};
  EXPECT_EQ(sourcemeta::core::nfc(once), once);
}

// Codepoints above U+FFFF (supplementary plane) with no decomposition pass
// through unchanged
TEST(supplementary_plane_passthrough) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\U00020000"), U"\U00020000");
}

// Greek text in precomposed form is unchanged
TEST(greek_precomposed_unchanged) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u03C0\u03B1\u03C1\u03AC\u03B4"
                                  U"\u03B5\u03B9\u03B3\u03BC\u03B1"),
            U"\u03C0\u03B1\u03C1\u03AC\u03B4"
            U"\u03B5\u03B9\u03B3\u03BC\u03B1");
}

// Decomposed Greek "παράδειγμα" (with U+03AC replaced by alpha + acute)
// composes back to the precomposed form
TEST(greek_decomposed_composes) {
  EXPECT_EQ(sourcemeta::core::nfc(U"\u03C0\u03B1\u03C1\u03B1\u0301\u03B4"
                                  U"\u03B5\u03B9\u03B3\u03BC\u03B1"),
            U"\u03C0\u03B1\u03C1\u03AC\u03B4"
            U"\u03B5\u03B9\u03B3\u03BC\u03B1");
}
