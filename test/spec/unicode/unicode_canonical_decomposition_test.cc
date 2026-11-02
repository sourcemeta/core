#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string_view>

TEST(ascii_letter_has_no_decomposition) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(U'A').empty());
}

TEST(ascii_digit_has_no_decomposition) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(U'0').empty());
}

TEST(null_has_no_decomposition) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(0x0000).empty());
}

// U+00C0 LATIN CAPITAL LETTER A WITH GRAVE decomposes to U+0041 U+0300
TEST(latin_a_with_grave) {
  EXPECT_EQ(sourcemeta::core::canonical_decomposition(U'\u00C0'),
            std::u32string_view{U"A\u0300"});
}

// U+00C1 LATIN CAPITAL LETTER A WITH ACUTE decomposes to U+0041 U+0301
TEST(latin_a_with_acute) {
  EXPECT_EQ(sourcemeta::core::canonical_decomposition(U'\u00C1'),
            std::u32string_view{U"A\u0301"});
}

// U+00FC LATIN SMALL LETTER U WITH DIAERESIS decomposes to U+0075 U+0308
TEST(latin_u_with_diaeresis) {
  EXPECT_EQ(sourcemeta::core::canonical_decomposition(U'\u00FC'),
            std::u32string_view{U"u\u0308"});
}

// U+2126 OHM SIGN: singleton decomposition to U+03A9 GREEK CAPITAL OMEGA
TEST(ohm_sign_singleton) {
  EXPECT_EQ(sourcemeta::core::canonical_decomposition(U'\u2126'),
            std::u32string_view{U"\u03A9"});
}

// U+1E9B LATIN SMALL LETTER LONG S WITH DOT ABOVE: decomposes one step to
// U+017F U+0307. Full recursive decomposition is the algorithm's job
TEST(long_s_with_dot_above_non_recursive) {
  EXPECT_EQ(sourcemeta::core::canonical_decomposition(U'\u1E9B'),
            std::u32string_view{U"\u017F\u0307"});
}

// Combining marks themselves have no decomposition
TEST(combining_grave_no_decomposition) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(U'\u0300').empty());
}

TEST(combining_acute_no_decomposition) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(U'\u0301').empty());
}

// U+212B ANGSTROM SIGN: singleton decomposition to U+00C5 LATIN A WITH RING
TEST(angstrom_sign_singleton) {
  EXPECT_EQ(sourcemeta::core::canonical_decomposition(U'\u212B'),
            std::u32string_view{U"\u00C5"});
}

// U+0958 DEVANAGARI LETTER QA decomposes to U+0915 U+093C
TEST(devanagari_qa) {
  EXPECT_EQ(sourcemeta::core::canonical_decomposition(U'\u0958'),
            std::u32string_view{U"\u0915\u093C"});
}

// U+212C SCRIPT CAPITAL B has decomposition `<font> 0042` (compatibility)
TEST(compatibility_decomposition_font_excluded) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(U'\u212C').empty());
}

// U+FF21 FULLWIDTH LATIN CAPITAL LETTER A has decomposition `<wide> 0041`
TEST(compatibility_decomposition_wide_excluded) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(U'\uFF21').empty());
}

// U+212A KELVIN SIGN, by contrast, has an untagged singleton canonical
// decomposition to U+004B (ASCII K), so it must still be present
TEST(kelvin_sign_canonical_singleton) {
  EXPECT_EQ(sourcemeta::core::canonical_decomposition(U'\u212A'),
            std::u32string_view{U"K"});
}

// Hangul precomposed syllables have no entry in UnicodeData.txt. Their
// algorithmic decomposition is the caller's job
TEST(hangul_syllable_first) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(U'\uAC00').empty());
}

TEST(hangul_syllable_last) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(U'\uD7A3').empty());
}

// Past the Unicode maximum: empty by definition
TEST(beyond_max_codepoint) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(0x110000).empty());
}

TEST(beyond_max_codepoint_high) {
  EXPECT_TRUE(sourcemeta::core::canonical_decomposition(0xFFFFFFFF).empty());
}

TEST(view_outlives_call) {
  const auto first{sourcemeta::core::canonical_decomposition(U'\u00C0')};
  const auto second{sourcemeta::core::canonical_decomposition(U'\u00C0')};
  EXPECT_EQ(first.data(), second.data());
  EXPECT_EQ(first.size(), 2u);
}
