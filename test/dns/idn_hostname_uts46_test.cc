#include <sourcemeta/core/dns.h>
#include <sourcemeta/core/test.h>

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

// An uppercase A-label is lowercased by the mapping and then validates. The
// strict validator rejects the uppercase Punycode body
TEST(valid_uppercase_a_label_is_folded) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname_uts46("XN--MNCHEN-3YA"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("XN--MNCHEN-3YA"));
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
