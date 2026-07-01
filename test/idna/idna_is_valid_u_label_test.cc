#include <sourcemeta/core/idna.h>
#include <sourcemeta/core/test.h>

#include <string> // std::u32string

TEST(ascii_letters) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"abc"));
}

TEST(ascii_letters_and_digits) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"abc123"));
}

TEST(ascii_with_internal_hyphen) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"ab-cd"));
}

TEST(latin_with_diacritic) {
  // d, e+acute, j, a+grave
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"d\u00E9j\u00E0"));
}

TEST(hebrew_label) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"\u05D0\u05D1\u05D2"));
}

TEST(arabic_label) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"\u0627\u0628\u062A"));
}

TEST(devanagari_label) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"\u0905\u0915\u092E"));
}

TEST(devanagari_with_virama_zwj) {
  // KA + VIRAMA + ZWJ + KA forms a valid ContextJ sequence
  EXPECT_TRUE(
      sourcemeta::core::idna_is_valid_u_label(U"\u0915\u094D\u200D\u0915"));
}

TEST(arabic_with_zwnj_context) {
  // Arabic BEH (D) + ZWNJ + Arabic ALEF (R) satisfies ContextJ A.1
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"\u0628\u200C\u0627"));
}

TEST(catalan_middle_dot) {
  // l + U+00B7 + l satisfies ContextO A.3
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"l\u00B7l"));
}

TEST(single_letter) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"a"));
}

TEST(empty_label) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U""));
}

TEST(leading_hyphen) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"-abc"));
}

TEST(trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"abc-"));
}

TEST(double_hyphen_at_positions_3_and_4) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"ab--cd"));
}

TEST(xn_prefix_pattern) {
  // The "xn--" pattern is the A-label prefix and must not appear in U-labels
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"xn--abc"));
}

TEST(exactly_four_chars_with_double_hyphen) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"ab--"));
}

TEST(leading_nonspacing_combining_mark) {
  // U+0301 combining acute is general category Mn
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"\u0301abc"));
}

TEST(leading_spacing_combining_mark) {
  // U+093E Devanagari vowel sign Aa is general category Mc
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"\u093E\u0915"));
}

TEST(contains_uppercase_letter) {
  // Uppercase ASCII is DISALLOWED in IDNA2008
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"abcD"));
}

TEST(contains_full_stop) {
  // ASCII full stop is DISALLOWED in a label
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"abc.def"));
}

TEST(contains_disallowed_control) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u0007b"));
}

TEST(contains_disallowed_bidi_mark) {
  // U+200E LEFT-TO-RIGHT MARK is DISALLOWED
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u200Eb"));
}

TEST(contains_unassigned_codepoint) {
  // U+0E80 is unassigned in the Lao block
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u0E80b"));
}

TEST(zwj_without_virama_context_rejected) {
  // ZWJ between two non-virama letters fails ContextJ A.2
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u200Db"));
}

TEST(zwnj_with_no_joining_context_rejected) {
  // ZWNJ between two ASCII letters fails ContextJ A.1
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u200Cb"));
}

TEST(middle_dot_not_flanked_by_l_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u00B7b"));
}

TEST(greek_keraia_followed_by_non_greek_rejected) {
  // U+0375 must be followed by Greek script
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u0375b"));
}

TEST(arabic_indic_digit_mixed_with_extended_rejected) {
  // U+0660 and U+06F0 must not appear together
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"\u0627\u0660\u06F0"));
}

TEST(single_hyphen_in_position_3_only) {
  // Only position 3 is hyphen, position 4 is not, so allowed
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"ab-cdef"));
}

TEST(single_hyphen_in_position_4_only) {
  // Only position 4 is hyphen, position 3 is not, so allowed
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"abc-def"));
}

// RFC 5891 §4.1.2.A: a U-label must be in Unicode Normalisation Form C.
// `a` (U+0061) and combining grave (U+0300) are both PVALID, so the only
// reason this decomposed sequence is rejected is the missing NFC. The
// precomposed `\u00E0` counterpart is accepted
TEST(decomposed_lowercase_a_grave_rejected) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u0300"));
}

TEST(precomposed_lowercase_a_grave_accepted) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"\u00E0"));
}

// Out-of-order combining marks violate canonical CCC ordering, so the
// label is not in NFC even though every codepoint is PVALID
TEST(out_of_order_combining_marks_rejected) {
  // a + U+0301 (CCC=230) + U+0316 (CCC=220)
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(U"a\u0301\u0316"));
}

// Hangul precomposed syllable is PVALID and already in NFC
TEST(hangul_precomposed_syllable_accepted) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(U"\uAC00"));
}

// RFC 5890 \u00A72.3.2.1: the corresponding A-label must be at most 63 octets.
// A pure-ASCII label of 58 "a" characters encodes to a 59-octet Punycode
// body, which with the 4-octet "xn--" prefix is exactly 63 octets
TEST(a_label_form_exactly_63_octets_accepted) {
  const std::u32string label(58, U'a');
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_u_label(label));
}

// One additional character pushes the A-label form to 64 octets
TEST(a_label_form_64_octets_rejected) {
  const std::u32string label(59, U'a');
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_u_label(label));
}
