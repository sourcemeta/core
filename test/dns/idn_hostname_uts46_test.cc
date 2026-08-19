#include <sourcemeta/core/dns.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

// A plain ASCII host name is unaffected by the mapping
TEST(valid_ascii_dotted) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("www.example.com"));
}

// An already-valid U-label (U+03B1) passes through the mapping unchanged
TEST(valid_u_label_unchanged) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("\xce\xb1"));
}

// A lowercase A-label is unchanged and still validates
TEST(valid_a_label_unchanged) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("xn--mnchen-3ya"));
}

// UTS #46 step 1: uppercase ASCII is case-folded, so a form the strict
// validator also accepts stays valid here
TEST(valid_uppercase_ascii_is_folded) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("WWW.EXAMPLE.COM"));
}

// Fullwidth Latin letters U+FF21..U+FF23 map to "abc". The strict validator
// rejects them because they are DISALLOWED under IDNA 2008
TEST(valid_fullwidth_letters_are_mapped) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(
      "\xef\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\xef\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3"));
}

// A fullwidth digit U+FF11 maps to ASCII "1"
TEST(valid_fullwidth_digit_is_mapped) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("a\xef\xbc\x91"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a\xef\xbc\x91"));
}

// Uppercase non-ASCII: MÜNCHEN (U+00DC uppercase) folds to "münchen". The
// strict validator rejects U+00DC as DISALLOWED
TEST(valid_uppercase_non_ascii_is_folded) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("M\xc3\x9cNCHEN"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("M\xc3\x9cNCHEN"));
}

// An uppercase A-label is lowercased by the mapping and then validates. RFC
// 5891 §5.3 has the strict validator lowercase it too, so both accept it
TEST(valid_uppercase_a_label_is_folded) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("XN--MNCHEN-3YA"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("XN--MNCHEN-3YA"));
}

// UTS #46 step 1: SOFT HYPHEN (U+00AD) is ignored, so "a<SHY>b" maps to "ab".
// The strict validator rejects it as DISALLOWED
TEST(valid_soft_hyphen_is_ignored) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("a\xc2\xad"
                                                      "b"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a\xc2\xad"
                                                 "b"));
}

// UTS #46 step 1: ZERO WIDTH SPACE (U+200B) is ignored, so "a<ZWSP>b" maps to
// "ab". The strict validator rejects it as DISALLOWED
TEST(valid_zero_width_space_is_ignored) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("\x61\xe2\x80\x8b\x62"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\x61\xe2\x80\x8b\x62"));
}

// UTS #46 step 1: the ideographic full stop U+3002 maps to U+002E, so it acts
// as a label separator after mapping
TEST(valid_ideographic_full_stop_is_mapped_to_separator) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("a\xe3\x80\x82"
                                                      "b"));
}

// Nontransitional Processing: the deviation character SHARP S (U+00DF) is kept
// (not folded to "ss"), and "faß" is a valid U-label
TEST(valid_deviation_sharp_s_kept) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("fa\xc3\x9f"));
}

// Nontransitional Processing: ZWNJ (U+200C) is kept as a deviation rather than
// removed, so a ZWNJ without the required CONTEXTJ context is invalid. Under
// Transitional Processing it would be stripped and the label would validate
TEST(invalid_deviation_zwnj_without_context) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46("a\xe2\x80\x8c"
                                                       "b"));
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(""));
}

// Input that maps to the empty string (only ignored code points) is invalid
TEST(invalid_all_ignored_maps_to_empty) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46("\xc2\xad"));
}

// A disallowed code point (U+0080) causes the mapping to fail
TEST(invalid_disallowed_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46("a\xc2\x80"
                                                       "b"));
}

// The mapping does not rescue structurally invalid host names
TEST(invalid_leading_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46("-bad"));
}

TEST(invalid_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46("example."));
}

// Malformed UTF-8 is rejected before mapping
TEST(invalid_utf8) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46("\xc0\x80"));
}

// UTS #46 maps each U+FF41 to an ASCII letter, so the 63-codepoint label is 63
// octets after mapping even though it is 189 before
TEST(valid_length_measured_on_mapped_form) {
  static constexpr char value[] =
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81";
  static_assert(sizeof(value) - 1 == 189, "literal length");
  const std::string_view input{value, 189};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// the same label one character longer is over the 63-octet limit
TEST(invalid_length_measured_on_mapped_form_at_64) {
  static constexpr char value[] =
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81"
      "\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81\xef\xbd\x81";
  static_assert(sizeof(value) - 1 == 192, "literal length");
  const std::string_view input{value, 192};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+200B is ignored, so it must not count toward the 63-octet label limit
TEST(valid_ignored_codepoint_not_counted_in_length) {
  static constexpr char value[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                  "aaaaaaaaaaaaaaaaaaa\xe2\x80\x8b";
  static_assert(sizeof(value) - 1 == 66, "literal length");
  const std::string_view input{value, 66};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// the ignored codepoint does not rescue a label that is already too long
TEST(invalid_ignored_codepoint_label_still_over) {
  static constexpr char value[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                  "aaaaaaaaaaaaaaaaaaaa\xe2\x80\x8b";
  static_assert(sizeof(value) - 1 == 67, "literal length");
  const std::string_view input{value, 67};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+FF58 U+FF4E map to "xn", so the xn-- test has to run after mapping
TEST(valid_ace_prefix_produced_by_mapping) {
  static constexpr char value[] = "\xef\xbd\x98\xef\xbd\x8e--nxasmq6b";
  static_assert(sizeof(value) - 1 == 16, "literal length");
  const std::string_view input{value, 16};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// the mapped label is xn--b, which is not decodable Punycode
TEST(invalid_ace_prefix_produced_by_mapping_bad_payload) {
  static constexpr char value[] = "\xef\xbd\x98\xef\xbd\x8e--b";
  static_assert(sizeof(value) - 1 == 9, "literal length");
  const std::string_view input{value, 9};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+FF0D maps to a hyphen, producing a leading hyphen
TEST(invalid_hyphen_produced_by_mapping_leading) {
  static constexpr char value[] = "\xef\xbc\x8d"
                                  "abc";
  static_assert(sizeof(value) - 1 == 6, "literal length");
  const std::string_view input{value, 6};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+FF0D maps to a hyphen, producing a trailing hyphen
TEST(invalid_hyphen_produced_by_mapping_trailing) {
  static constexpr char value[] = "abc\xef\xbc\x8d";
  static_assert(sizeof(value) - 1 == 6, "literal length");
  const std::string_view input{value, 6};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// a pair of U+FF0D maps to -- in the third and fourth positions
TEST(invalid_double_hyphen_produced_by_mapping) {
  static constexpr char value[] = "ab\xef\xbc\x8d\xef\xbc\x8d"
                                  "cd";
  static_assert(sizeof(value) - 1 == 10, "literal length");
  const std::string_view input{value, 10};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// two U+FF0E map to full stops, producing an empty label
TEST(invalid_empty_label_produced_by_mapping) {
  static constexpr char value[] = "a\xef\xbc\x8e\xef\xbc\x8e"
                                  "b";
  static_assert(sizeof(value) - 1 == 8, "literal length");
  const std::string_view input{value, 8};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+03C2 is a deviation character; nontransitional processing keeps it
TEST(valid_deviation_final_sigma_kept) {
  static constexpr char value[] = "\xcf\x82";
  static_assert(sizeof(value) - 1 == 2, "literal length");
  const std::string_view input{value, 2};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(input));
}

// U+200D is a deviation character kept by nontransitional processing, and then
// fails its ContextJ rule
TEST(invalid_deviation_zwj_without_context) {
  static constexpr char value[] = "a\xe2\x80\x8d"
                                  "b";
  static_assert(sizeof(value) - 1 == 5, "literal length");
  const std::string_view input{value, 5};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+1D400 MATHEMATICAL BOLD CAPITAL A maps to a
TEST(valid_mathematical_bold_is_mapped) {
  static constexpr char value[] = "\xf0\x9d\x90\x80"
                                  "bc";
  static_assert(sizeof(value) - 1 == 6, "literal length");
  const std::string_view input{value, 6};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+FE00 VARIATION SELECTOR-1 is ignored by the mapping
TEST(valid_variation_selector_is_ignored) {
  static constexpr char value[] = "a\xef\xb8\x80"
                                  "b";
  static_assert(sizeof(value) - 1 == 5, "literal length");
  const std::string_view input{value, 5};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+24D0 CIRCLED LATIN SMALL LETTER A maps to a
TEST(valid_circled_latin_is_mapped) {
  static constexpr char value[] = "\xe2\x93\x90"
                                  "bc";
  static_assert(sizeof(value) - 1 == 5, "literal length");
  const std::string_view input{value, 5};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+FB01 LATIN SMALL LIGATURE FI maps to the two letters f and i
TEST(valid_ligature_is_mapped) {
  static constexpr char value[] = "\xef\xac\x81x";
  static_assert(sizeof(value) - 1 == 4, "literal length");
  const std::string_view input{value, 4};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// U+005F is UTS #46 valid but carries the IDNA 2008 NV8 tag, so it is excluded
// from an IDN label
TEST(invalid_underscore_nv8) {
  static constexpr char value[] = "a_b";
  static_assert(sizeof(value) - 1 == 3, "literal length");
  const std::string_view input{value, 3};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// the mapping step normalises to NFC, so the decomposed label is accepted
TEST(valid_nfc_composition_by_mapping) {
  static constexpr char value[] = "cafe\xcc\x81";
  static_assert(sizeof(value) - 1 == 6, "literal length");
  const std::string_view input{value, 6};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// RFC 5893 condition 2: an RTL label may not contain a character with Bidi
// property L
TEST(invalid_bidi_ltr_char_inside_rtl_label) {
  static constexpr char value[] = "\xd7\x90"
                                  "a\xd7\x90";
  static_assert(sizeof(value) - 1 == 5, "literal length");
  const std::string_view input{value, 5};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// RFC 5893 condition 3: an RTL label must end with R, AL, EN, AN or NSM
TEST(invalid_bidi_rtl_label_ends_with_on) {
  static constexpr char value[] = "\xd7\x90\xcb\x87";
  static_assert(sizeof(value) - 1 == 4, "literal length");
  const std::string_view input{value, 4};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// RFC 5893 condition 4: an RTL label may not contain both EN and AN
TEST(invalid_bidi_rtl_label_mixes_en_and_an) {
  static constexpr char value[] = "\xd7\x90"
                                  "0\xd9\xa0";
  static_assert(sizeof(value) - 1 == 5, "literal length");
  const std::string_view input{value, 5};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// RFC 5893 condition 1 allows only L, R or AL as a first character, and an
// Arabic-Indic digit is AN
TEST(invalid_bidi_pure_arabic_indic_digit_label) {
  static constexpr char value[] = "\xd9\xa0\xd9\xa1";
  static_assert(sizeof(value) - 1 == 4, "literal length");
  const std::string_view input{value, 4};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// RFC 5892 appendix A.9 permits extended Arabic-Indic digits when the label
// carries none of the U+0660 block
TEST(valid_extended_arabic_indic_digits_only) {
  static constexpr char value[] = "\xd8\xa8\xdb\xb0\xdb\xb1";
  static_assert(sizeof(value) - 1 == 6, "literal length");
  const std::string_view input{value, 6};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(input));
}

// appendix A.9 forbids the two digit blocks in one label
TEST(invalid_extended_arabic_indic_mixed_with_arabic_indic) {
  static constexpr char value[] = "\xd8\xa8\xdb\xb0\xd9\xa0";
  static_assert(sizeof(value) - 1 == 6, "literal length");
  const std::string_view input{value, 6};
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(input));
}

// appendices A.8 and A.9 scope the rule to "this label", so one block per label
// is permitted
TEST(valid_arabic_indic_digit_blocks_in_separate_labels) {
  static constexpr char value[] = "\xd8\xa8\xd9\xa0.\xd8\xa8\xdb\xb0";
  static_assert(sizeof(value) - 1 == 9, "literal length");
  const std::string_view input{value, 9};
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46(input));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(input));
}
