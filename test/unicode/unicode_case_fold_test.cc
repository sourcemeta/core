#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string_view> // std::u32string_view

TEST(ascii_uppercase_letter) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'A'), std::u32string_view{U"a"});
}

TEST(ascii_uppercase_letter_z) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'Z'), std::u32string_view{U"z"});
}

TEST(ascii_lowercase_letter_folds_to_itself) {
  EXPECT_TRUE(sourcemeta::core::case_fold(U'a').empty());
}

TEST(ascii_digit_folds_to_itself) {
  EXPECT_TRUE(sourcemeta::core::case_fold(U'0').empty());
}

TEST(null_folds_to_itself) {
  EXPECT_TRUE(sourcemeta::core::case_fold(0x0000).empty());
}

TEST(latin_small_sharp_s) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u00DF'), std::u32string_view{U"ss"});
}

TEST(latin_capital_i_with_dot_above) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u0130'),
            std::u32string_view{U"i\u0307"});
}

TEST(latin_capital_sharp_s_prefers_full_over_simple) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u1E9E'), std::u32string_view{U"ss"});
}

TEST(latin_small_ligature_ffi) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\uFB03'),
            std::u32string_view{U"ffi"});
}

TEST(greek_small_iota_with_dialytika_and_tonos) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u0390'),
            std::u32string_view{U"\u03B9\u0308\u0301"});
}

TEST(latin_capital_i_ignores_turkic) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'I'), std::u32string_view{U"i"});
}

TEST(latin_small_dotless_i_folds_to_itself) {
  EXPECT_TRUE(sourcemeta::core::case_fold(U'\u0131').empty());
}

TEST(greek_small_final_sigma) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u03C2'),
            std::u32string_view{U"\u03C3"});
}

TEST(greek_capital_sigma) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u03A3'),
            std::u32string_view{U"\u03C3"});
}

TEST(greek_small_sigma_folds_to_itself) {
  EXPECT_TRUE(sourcemeta::core::case_fold(U'\u03C3').empty());
}

TEST(micro_sign) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u00B5'),
            std::u32string_view{U"\u03BC"});
}

TEST(kelvin_sign) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u212A'), std::u32string_view{U"k"});
}

TEST(cherokee_small_letter_a) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\uAB70'),
            std::u32string_view{U"\u13A0"});
}

TEST(cherokee_small_letter_ye) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\u13F8'),
            std::u32string_view{U"\u13F0"});
}

TEST(cherokee_capital_letter_a_folds_to_itself) {
  EXPECT_TRUE(sourcemeta::core::case_fold(U'\u13A0').empty());
}

TEST(deseret_capital_letter_long_i) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\U00010400'),
            std::u32string_view{U"\U00010428"});
}

TEST(deseret_small_letter_long_i_folds_to_itself) {
  EXPECT_TRUE(sourcemeta::core::case_fold(U'\U00010428').empty());
}

TEST(adlam_capital_letter_sha) {
  EXPECT_EQ(sourcemeta::core::case_fold(U'\U0001E921'),
            std::u32string_view{U"\U0001E943"});
}

TEST(max_codepoint_folds_to_itself) {
  EXPECT_TRUE(sourcemeta::core::case_fold(0x10FFFF).empty());
}

TEST(above_max_codepoint) {
  EXPECT_TRUE(sourcemeta::core::case_fold(0x110000).empty());
}

TEST(above_max_codepoint_high) {
  EXPECT_TRUE(sourcemeta::core::case_fold(0xFFFFFFFF).empty());
}

TEST(string_empty) { EXPECT_EQ(sourcemeta::core::case_fold(U""), U""); }

TEST(string_ascii) {
  EXPECT_EQ(sourcemeta::core::case_fold(U"Hello World"), U"hello world");
}

TEST(string_already_folded) {
  EXPECT_EQ(sourcemeta::core::case_fold(U"strasse"), U"strasse");
}

TEST(string_mixed) {
  EXPECT_EQ(sourcemeta::core::case_fold(
                U"Stra\u00DFe \u03A3\u03C2 \u0130I \uFB03 \U00010400"),
            U"strasse \u03C3\u03C3 i\u0307i ffi \U00010428");
}
