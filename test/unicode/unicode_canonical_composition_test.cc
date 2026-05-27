#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

#include <optional>

// U+0041 + U+0300 compose to U+00C0 LATIN CAPITAL LETTER A WITH GRAVE
TEST(Unicode_canonical_composition, latin_a_grave) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'A', U'\u0300'),
            U'\u00C0');
}

// U+0041 + U+0301 compose to U+00C1 LATIN CAPITAL LETTER A WITH ACUTE
TEST(Unicode_canonical_composition, latin_a_acute) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'A', U'\u0301'),
            U'\u00C1');
}

// U+0075 + U+0308 compose to U+00FC LATIN SMALL LETTER U WITH DIAERESIS
TEST(Unicode_canonical_composition, latin_u_diaeresis) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'u', U'\u0308'),
            U'\u00FC');
}

// Two ASCII letters do not compose
TEST(Unicode_canonical_composition, no_composition_ascii_pair) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'A', U'B'), std::nullopt);
}

// Reverse order: combining mark on the left is never a starter
TEST(Unicode_canonical_composition, no_composition_reversed_order) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'\u0300', U'A'),
            std::nullopt);
}

// U+0F73 TIBETAN VOWEL SIGN II decomposes to U+0F71 U+0F72, but U+0F71 has
// CCC=129 making this a non-starter decomposition. Excluded from the
// composition table per UAX #15
TEST(Unicode_canonical_composition, no_composition_non_starter_decomposition) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'\u0F71', U'\u0F72'),
            std::nullopt);
}

// U+0958 DEVANAGARI LETTER QA decomposes to U+0915 U+093C but is in the
// composition exclusion list (script-specific exclusion)
TEST(Unicode_canonical_composition, no_composition_script_exclusion_qa) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'\u0915', U'\u093C'),
            std::nullopt);
}

// U+09DC BENGALI LETTER RRA decomposes to U+09A1 U+09BC and is in the
// composition exclusion list
TEST(Unicode_canonical_composition,
     no_composition_script_exclusion_bengali_rra) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'\u09A1', U'\u09BC'),
            std::nullopt);
}

// Hangul L + V composes algorithmically per UAX #15 §3.12, not via the
// table. The function must return nullopt for these pairs
TEST(Unicode_canonical_composition, no_composition_hangul_l_v) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'\u1100', U'\u1161'),
            std::nullopt);
}

// U+003C LESS-THAN SIGN + U+0338 COMBINING LONG SOLIDUS OVERLAY compose to
// U+226E NOT LESS-THAN. First entry in the sorted composition table
TEST(Unicode_canonical_composition, less_than_solidus_overlay) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'<', U'\u0338'),
            U'\u226E');
}

// U+003D + U+0338 compose to U+2260 NOT EQUAL TO
TEST(Unicode_canonical_composition, equals_solidus_overlay) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'=', U'\u0338'),
            U'\u2260');
}

// Out-of-range starter
TEST(Unicode_canonical_composition, out_of_range_starter) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(0x110000, U'\u0300'),
            std::nullopt);
}

// Out-of-range combining
TEST(Unicode_canonical_composition, out_of_range_combining) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'A', 0x110000),
            std::nullopt);
}

// A non-combining codepoint as the second argument exercises the
// binary-search miss path near a real starter cluster
TEST(Unicode_canonical_composition, near_match_does_not_compose) {
  EXPECT_EQ(sourcemeta::core::canonical_composition(U'A', U'\u0299'),
            std::nullopt);
}
