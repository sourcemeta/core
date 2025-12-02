#include <gtest/gtest.h>

#include <sourcemeta/core/punycode.h>

#include <sstream> // std::istringstream, std::ostringstream
#include <string>  // std::string

// RFC 3492 Section 7.1 Sample strings
// See https://www.rfc-editor.org/rfc/rfc3492#section-7.1

// (A) Arabic (Egyptian)
TEST(Punycode_decode, rfc3492_sample_a_arabic) {
  const std::u32string expected{0x0644, 0x064A, 0x0647, 0x0645, 0x0627, 0x0628,
                                0x062A, 0x0643, 0x0644, 0x0645, 0x0648, 0x0634,
                                0x0639, 0x0631, 0x0628, 0x064A, 0x061F};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("egbpdaj6bu4bxfgehfvwxn"),
            expected);
}

// (B) Chinese (simplified)
TEST(Punycode_decode, rfc3492_sample_b_chinese_simplified) {
  const std::u32string expected{0x4ED6, 0x4EEC, 0x4E3A, 0x4EC0, 0x4E48,
                                0x4E0D, 0x8BF4, 0x4E2D, 0x6587};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("ihqwcrb4cv8a8dqg056pqjye"),
            expected);
}

// (C) Chinese (traditional)
TEST(Punycode_decode, rfc3492_sample_c_chinese_traditional) {
  const std::u32string expected{0x4ED6, 0x5011, 0x7232, 0x4EC0, 0x9EBD,
                                0x4E0D, 0x8AAA, 0x4E2D, 0x6587};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("ihqwctvzc91f659drss3x8bo0yb"),
            expected);
}

// (D) Czech
TEST(Punycode_decode, rfc3492_sample_d_czech) {
  const std::u32string expected{0x0050, 0x0072, 0x006F, 0x010D, 0x0070, 0x0072,
                                0x006F, 0x0073, 0x0074, 0x011B, 0x006E, 0x0065,
                                0x006D, 0x006C, 0x0075, 0x0076, 0x00ED, 0x010D,
                                0x0065, 0x0073, 0x006B, 0x0079};
  EXPECT_EQ(
      sourcemeta::core::punycode_to_utf32("Proprostnemluvesky-uyb24dma41a"),
      expected);
}

// (E) Hebrew
TEST(Punycode_decode, rfc3492_sample_e_hebrew) {
  const std::u32string expected{0x05DC, 0x05DE, 0x05D4, 0x05D4, 0x05DD, 0x05E4,
                                0x05E9, 0x05D5, 0x05D8, 0x05DC, 0x05D0, 0x05DE,
                                0x05D3, 0x05D1, 0x05E8, 0x05D9, 0x05DD, 0x05E2,
                                0x05D1, 0x05E8, 0x05D9, 0x05EA};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("4dbcagdahymbxekheh6e0a7fei0b"),
            expected);
}

// (F) Hindi (Devanagari)
TEST(Punycode_decode, rfc3492_sample_f_hindi) {
  const std::u32string expected{0x092F, 0x0939, 0x0932, 0x094B, 0x0917, 0x0939,
                                0x093F, 0x0928, 0x094D, 0x0926, 0x0940, 0x0915,
                                0x094D, 0x092F, 0x094B, 0x0902, 0x0928, 0x0939,
                                0x0940, 0x0902, 0x092C, 0x094B, 0x0932, 0x0938,
                                0x0915, 0x0924, 0x0947, 0x0939, 0x0948, 0x0902};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32(
                "i1baa7eci9glrd9b2ae1bj0hfcgg6iyaf8o0a1dig0cd"),
            expected);
}

// (G) Japanese (kanji and hiragana)
TEST(Punycode_decode, rfc3492_sample_g_japanese) {
  const std::u32string expected{0x306A, 0x305C, 0x307F, 0x3093, 0x306A, 0x65E5,
                                0x672C, 0x8A9E, 0x3092, 0x8A71, 0x3057, 0x3066,
                                0x304F, 0x308C, 0x306A, 0x3044, 0x306E, 0x304B};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32(
                "n8jok5ay5dzabd5bym9f0cm5685rrjetr6pdxa"),
            expected);
}

// (H) Korean (Hangul syllables)
TEST(Punycode_decode, rfc3492_sample_h_korean) {
  const std::u32string expected{0xC138, 0xACC4, 0xC758, 0xBAA8, 0xB4E0, 0xC0AC,
                                0xB78C, 0xB4E4, 0xC774, 0xD55C, 0xAD6D, 0xC5B4,
                                0xB97C, 0xC774, 0xD574, 0xD55C, 0xB2E4, 0xBA74,
                                0xC5BC, 0xB9C8, 0xB098, 0xC88B, 0xC744, 0xAE4C};
  EXPECT_EQ(
      sourcemeta::core::punycode_to_utf32(
          "989aomsvi5e83db1d2a355cv1e0vak1dwrv93d5xbh15a0dt30a5jpsd879ccm6f"
          "ea98c"),
      expected);
}

// (I) Russian (Cyrillic)
TEST(Punycode_decode, rfc3492_sample_i_russian) {
  const std::u32string expected{0x043F, 0x043E, 0x0447, 0x0435, 0x043C, 0x0443,
                                0x0436, 0x0435, 0x043E, 0x043D, 0x0438, 0x043D,
                                0x0435, 0x0433, 0x043E, 0x0432, 0x043E, 0x0440,
                                0x044F, 0x0442, 0x043F, 0x043E, 0x0440, 0x0443,
                                0x0441, 0x0441, 0x043A, 0x0438};
  EXPECT_EQ(
      sourcemeta::core::punycode_to_utf32("b1abfaaepdrnnbgefbadotcwatmq2g4l"),
      expected);
}

// (J) Spanish
TEST(Punycode_decode, rfc3492_sample_j_spanish) {
  const std::u32string expected{
      0x0050, 0x006F, 0x0072, 0x0071, 0x0075, 0x00E9, 0x006E, 0x006F,
      0x0070, 0x0075, 0x0065, 0x0064, 0x0065, 0x006E, 0x0073, 0x0069,
      0x006D, 0x0070, 0x006C, 0x0065, 0x006D, 0x0065, 0x006E, 0x0074,
      0x0065, 0x0068, 0x0061, 0x0062, 0x006C, 0x0061, 0x0072, 0x0065,
      0x006E, 0x0045, 0x0073, 0x0070, 0x0061, 0x00F1, 0x006F, 0x006C};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32(
                "PorqunopuedensimplementehablarenEspaol-fmd56a"),
            expected);
}

// (K) Vietnamese
TEST(Punycode_decode, rfc3492_sample_k_vietnamese) {
  const std::u32string expected{
      0x0054, 0x1EA1, 0x0069, 0x0073, 0x0061, 0x006F, 0x0068, 0x1ECD,
      0x006B, 0x0068, 0x00F4, 0x006E, 0x0067, 0x0074, 0x0068, 0x1EC3,
      0x0063, 0x0068, 0x1EC9, 0x006E, 0x00F3, 0x0069, 0x0074, 0x0069,
      0x1EBF, 0x006E, 0x0067, 0x0056, 0x0069, 0x1EC7, 0x0074};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32(
                "TisaohkhngthchnitingVit-kjcr8268qyxafd2f1b9g"),
            expected);
}

// (L) 3nen B gumi Kinpachi sensei (Japanese)
TEST(Punycode_decode, rfc3492_sample_l_japanese_3b) {
  const std::u32string expected{0x0033, 0x5E74, 0x0042, 0x7D44,
                                0x91D1, 0x516B, 0x5148, 0x751F};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("3B-ww4c5e180e575a65lsy2b"),
            expected);
}

// (M) Amuro Namie with Super Monkeys (Japanese)
TEST(Punycode_decode, rfc3492_sample_m_japanese_monkeys) {
  const std::u32string expected{0x5B89, 0x5BA4, 0x5948, 0x7F8E, 0x6075, 0x002D,
                                0x0077, 0x0069, 0x0074, 0x0068, 0x002D, 0x0053,
                                0x0055, 0x0050, 0x0045, 0x0052, 0x002D, 0x004D,
                                0x004F, 0x004E, 0x004B, 0x0045, 0x0059, 0x0053};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32(
                "-with-SUPER-MONKEYS-pc58ag80a8qai00g7n9n"),
            expected);
}

// (N) Hello-Another-Way-...(Japanese)
TEST(Punycode_decode, rfc3492_sample_n_japanese_hello) {
  const std::u32string expected{
      0x0048, 0x0065, 0x006C, 0x006C, 0x006F, 0x002D, 0x0041, 0x006E, 0x006F,
      0x0074, 0x0068, 0x0065, 0x0072, 0x002D, 0x0057, 0x0061, 0x0079, 0x002D,
      0x305D, 0x308C, 0x305E, 0x308C, 0x306E, 0x5834, 0x6240};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32(
                "Hello-Another-Way--fc4qua05auwb3674vfr0b"),
            expected);
}

// (O) hitotsuyanenoshita2 (Japanese)
TEST(Punycode_decode, rfc3492_sample_o_japanese_hitotsu) {
  const std::u32string expected{0x3072, 0x3068, 0x3064, 0x5C4B,
                                0x6839, 0x306E, 0x4E0B, 0x0032};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("2-u9tlzr9756bt3uc0v"),
            expected);
}

// (P) MaijideKoisuru5byoumae (Japanese)
TEST(Punycode_decode, rfc3492_sample_p_japanese_maji) {
  const std::u32string expected{0x004D, 0x0061, 0x006A, 0x0069, 0x3067,
                                0x004B, 0x006F, 0x0069, 0x3059, 0x308B,
                                0x0035, 0x79D2, 0x524D};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("MajiKoi5-783gue6qz075azm5e"),
            expected);
}

// (Q) PafyiideRunba (Japanese katakana)
TEST(Punycode_decode, rfc3492_sample_q_japanese_pafyii) {
  const std::u32string expected{0x30D1, 0x30D5, 0x30A3, 0x30FC, 0x0064,
                                0x0065, 0x30EB, 0x30F3, 0x30D0};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("de-jg4avhby1noc0d"), expected);
}

// (R) Sono Speed de (Japanese hiragana/katakana)
TEST(Punycode_decode, rfc3492_sample_r_japanese_sono) {
  const std::u32string expected{0x305D, 0x306E, 0x30B9, 0x30D4,
                                0x30FC, 0x30C9, 0x3067};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("d9juau41awczczp"), expected);
}

// (S) Pure ASCII -> $1.00 <-
TEST(Punycode_decode, rfc3492_sample_s_ascii) {
  const std::u32string expected{0x002D, 0x003E, 0x0020, 0x0024, 0x0031, 0x002E,
                                0x0030, 0x0030, 0x0020, 0x003C, 0x002D};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("-> $1.00 <--"), expected);
}

// Edge cases using UTF-32 API
TEST(Punycode_decode, empty_string) {
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32(""), std::u32string{});
}

TEST(Punycode_decode, pure_ascii_simple) {
  const std::u32string expected{0x0068, 0x0065, 0x006C, 0x006C, 0x006F};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("hello-"), expected);
}

TEST(Punycode_decode, pure_ascii_no_non_basic) {
  const std::u32string expected{0x0061, 0x0062, 0x0063};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("abc-"), expected);
}

TEST(Punycode_decode, single_non_ascii) {
  const std::u32string expected{0x00FC};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("tda"), expected);
}

TEST(Punycode_decode, mixed_ascii_and_non_ascii) {
  const std::u32string expected{0x004D, 0x00FC, 0x006E, 0x0063,
                                0x0068, 0x0065, 0x006E};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("Mnchen-3ya"), expected);
}

TEST(Punycode_decode, emoji_single) {
  const std::u32string expected{0x1F600};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("e28h"), expected);
}

TEST(Punycode_decode, mixed_with_emoji) {
  // hi + grinning face emoji
  const std::u32string expected{0x0068, 0x0069, 0x1F600};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("hi-oo82a"), expected);
}

TEST(Punycode_decode, supplementary_plane) {
  // LINEAR B SYLLABLE B008 A (U+10000)
  const std::u32string expected{0x10000};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("2n7c"), expected);
}

TEST(Punycode_decode, multiple_hyphens_in_basic) {
  const std::u32string expected{0x0061, 0x002D, 0x0062, 0x002D, 0x0063};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("a-b-c-"), expected);
}

TEST(Punycode_decode, only_non_basic) {
  const std::u32string expected{0x00E4, 0x00F6, 0x00FC};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("4ca0bs"), expected);
}

TEST(Punycode_decode, long_string_non_ascii) {
  // 中文 repeated 5 times
  const std::u32string expected{0x4E2D, 0x6587, 0x4E2D, 0x6587, 0x4E2D,
                                0x6587, 0x4E2D, 0x6587, 0x4E2D, 0x6587};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("fiqaaaa8796ababbb"), expected);
}

TEST(Punycode_decode, error_invalid_digit) {
  EXPECT_THROW(sourcemeta::core::punycode_to_utf32("abc!def"),
               sourcemeta::core::PunycodeError);
}

TEST(Punycode_decode, error_non_basic_before_delimiter) {
  // Non-ASCII character (0x80+) before the delimiter
  EXPECT_THROW(sourcemeta::core::punycode_to_utf32("\x80-abc"),
               sourcemeta::core::PunycodeError);
}

TEST(Punycode_decode, case_insensitive_basic_portion_preserved) {
  // The basic portion (before delimiter) preserves original case
  const std::u32string expected_lower{0x0061, 0x0062, 0x0063};
  const std::u32string expected_upper{0x0041, 0x0042, 0x0043};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("abc-"), expected_lower);
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("ABC-"), expected_upper);
}

TEST(Punycode_decode, case_insensitive_extended_digits) {
  // Digit values a-z and A-Z are equivalent per RFC 3492
  const std::u32string expected{0x00FC};
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("tda"), expected);
  EXPECT_EQ(sourcemeta::core::punycode_to_utf32("TDA"), expected);
}

TEST(Punycode_decode, utf8_api_simple) {
  EXPECT_EQ(sourcemeta::core::punycode_to_utf8("hello-"), "hello");
}

TEST(Punycode_decode, utf8_api_with_non_ascii) {
  // Decodes to "München" in UTF-8
  EXPECT_EQ(sourcemeta::core::punycode_to_utf8("Mnchen-3ya"), "M\xC3\xBCnchen");
}

TEST(Punycode_decode, utf8_stream_api_simple) {
  std::istringstream input{"hello-"};
  std::ostringstream output;
  sourcemeta::core::punycode_to_utf8(input, output);
  EXPECT_EQ(output.str(), "hello");
}

TEST(Punycode_decode, utf8_stream_api_with_non_ascii) {
  std::istringstream input{"Mnchen-3ya"};
  std::ostringstream output;
  sourcemeta::core::punycode_to_utf8(input, output);
  // Decodes to "München" in UTF-8
  EXPECT_EQ(output.str(), "M\xC3\xBCnchen");
}
