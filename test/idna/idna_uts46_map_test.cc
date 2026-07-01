#include <sourcemeta/core/idna.h>
#include <sourcemeta/core/test.h>

#include <optional>
#include <string>

TEST(empty_input_maps_to_empty) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U""), std::u32string{U""});
}

TEST(valid_ascii_is_unchanged) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"example"),
            std::u32string{U"example"});
}

// UTS #46 step 1: uppercase ASCII is case-folded to lowercase
TEST(uppercase_ascii_is_mapped_to_lowercase) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"EXAMPLE"),
            std::u32string{U"example"});
}

TEST(mixed_case_is_mapped_to_lowercase) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"ExAmPlE"),
            std::u32string{U"example"});
}

// Fullwidth digits U+FF11..U+FF13 map to ASCII "123"
TEST(fullwidth_digits_are_mapped_to_ascii) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"\uFF11\uFF12\uFF13"),
            std::u32string{U"123"});
}

// U+FF21 FULLWIDTH LATIN CAPITAL LETTER A maps to "a"
TEST(fullwidth_capital_maps_to_ascii_lowercase) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"\uFF21"), std::u32string{U"a"});
}

// A non-ASCII valid codepoint is preserved (U+00F6 o with diaeresis)
TEST(valid_non_ascii_is_unchanged) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"b\u00F6b"),
            std::u32string{U"b\u00F6b"});
}

// UTS #46 step 1: ignored codepoints are removed (U+00AD SOFT HYPHEN)
TEST(soft_hyphen_is_ignored) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"a\u00ADb"),
            std::u32string{U"ab"});
}

// U+200B ZERO WIDTH SPACE is ignored
TEST(zero_width_space_is_ignored) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"a\u200Bb"),
            std::u32string{U"ab"});
}

// Nontransitional Processing: the deviation characters map to themselves.
// U+00DF SHARP S is preserved (not folded to "ss")
TEST(deviation_sharp_s_is_preserved) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"\u00DF"),
            std::u32string{U"\u00DF"});
}

// U+03C2 GREEK SMALL LETTER FINAL SIGMA is preserved (not folded to sigma)
TEST(deviation_final_sigma_is_preserved) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"\u03C2"),
            std::u32string{U"\u03C2"});
}

// U+200C ZWNJ and U+200D ZWJ are deviations, preserved under Nontransitional
TEST(deviation_joiners_are_preserved) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"a\u200Cb\u200Dc"),
            std::u32string{U"a\u200Cb\u200Dc"});
}

// A disallowed codepoint fails the whole input (U+0080 control)
TEST(disallowed_control_returns_nullopt) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"\u0080"), std::nullopt);
}

// A codepoint beyond the Unicode range is disallowed
TEST(out_of_range_codepoint_returns_nullopt) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(std::u32string{U"a"} +
                                             char32_t{0x110000}),
            std::nullopt);
}

// A multi-codepoint mapping is expanded. U+00A8 DIAERESIS maps to
// U+0020 U+0308, which do not compose under NFC
TEST(multi_codepoint_mapping_is_expanded) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"\u00A8"),
            std::u32string{U"\u0020\u0308"});
}

// UTS #46 step 2: the mapped result is normalised to NFC. U+0041 maps to
// "a" and the following U+030A composes with it into U+00E5
TEST(mapped_result_is_nfc_normalised) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"A\u030A"),
            std::u32string{U"\u00E5"});
}

TEST(ascii_full_stop_is_preserved) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"a.b.c"),
            std::u32string{U"a.b.c"});
}

// UTS #46 step 1: the alternate label separators U+3002, U+FF0E and U+FF61
// are mapped to U+002E FULL STOP
TEST(alternate_label_separators_are_mapped_to_full_stop) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"a\u3002b\uFF0Ec\uFF61d"),
            std::u32string{U"a.b.c.d"});
}

// UTS #46 step 2: NFC composes valid, unmapped input. U+006F U+0308 (o plus
// combining diaeresis) composes into U+00F6
TEST(nfc_composes_unmapped_valid_input) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"o\u0308"),
            std::u32string{U"\u00F6"});
}

// Input consisting only of ignored code points maps to the empty string,
// which is a value rather than an error
TEST(all_ignored_input_maps_to_empty_string) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"\u00AD"), std::u32string{U""});
}

// A lone surrogate is disallowed
TEST(surrogate_returns_nullopt) {
  EXPECT_EQ(
      sourcemeta::core::idna_uts46_map(std::u32string{U"a"} + char32_t{0xD800}),
      std::nullopt);
}

// A "valid ; NV8" code point (U+002F SOLIDUS) is kept by the mapper. Its
// rejection under IDNA 2008 is the downstream validator's responsibility, not
// the mapper's
TEST(valid_nv8_codepoint_is_kept_by_mapper) {
  EXPECT_EQ(sourcemeta::core::idna_uts46_map(U"a/b"), std::u32string{U"a/b"});
}
