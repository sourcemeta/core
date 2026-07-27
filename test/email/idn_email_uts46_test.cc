#include <sourcemeta/core/email.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

// The lookup variant accepts everything the strict variant accepts
TEST(valid_ascii_parity) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("joe.bloggs@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_idn_email("joe.bloggs@example.com"));
}

TEST(valid_single_letter_parity) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("a@b"));
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a@b"));
}

// RFC 5890 §2.3.2.3: Hangul local part and domain
TEST(valid_utf8_local_and_domain) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46(
      "\xec\x8b\xa4\xeb\xa1\x80@"
      "\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
}

TEST(valid_quoted_local) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("\"a b\"@example.com"));
}

TEST(valid_uppercase_local) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("ABC@b"));
}

// RFC 6532 §3.1: a UTF-8 local part (Greek alpha U+03B1)
TEST(valid_utf8_local_part) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("\xce\xb1@b"));
}

// Address literals are unaffected by UTS #46 domain processing
TEST(valid_ipv4_address_literal) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("user@[192.168.1.1]"));
}

TEST(invalid_bad_ipv4_address_literal) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("user@[999.0.0.1]"));
}

// RFC 5890 §2.3.2.1 states the domain U-label NFC requirement as a SHOULD, so
// the lookup profile must not reject a non-NFC domain. The mapping normalises
// it; the strict variant rejects it. Domain is "caf" + "e" + U+0301 ("cafe"
// decomposed).
TEST(non_nfc_domain_accepted_by_uts46_rejected_by_strict) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("user@cafe\xcc\x81"));
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@cafe\xcc\x81"));
}

// RFC 6531 sets no normalisation requirement on the local part, so a non-NFC
// local part is accepted by both variants
TEST(non_nfc_local_part_accepted_by_both) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("cafe\xcc\x81@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_idn_email("cafe\xcc\x81@example.com"));
}

// Fullwidth Latin letters U+FF21..U+FF23 in the domain map to "abc"; the strict
// variant rejects them as DISALLOWED
TEST(fullwidth_domain_accepted_by_uts46_rejected_by_strict) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46(
      "user@\xef\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3"));
  EXPECT_FALSE(sourcemeta::core::is_idn_email(
      "user@\xef\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3"));
}

// Soft hyphen (U+00AD) in the domain is ignored by the mapping; the strict
// variant rejects it as DISALLOWED
TEST(soft_hyphen_domain_accepted_by_uts46_rejected_by_strict) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("user@ab\xc2\xad"
                                                   "c"));
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@ab\xc2\xad"
                                              "c"));
}

// Uppercase ASCII domains are case-insensitive (RFC 1123) and accepted by both
TEST(uppercase_ascii_domain_accepted_by_both) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("user@EXAMPLE.COM"));
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@EXAMPLE.COM"));
}

// UTS #46 §4 step 1: the ideographic full stop U+3002 maps to a label separator
TEST(ideographic_full_stop_domain_separator) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("user@a\xe3\x80\x82"
                                                   "b"));
}

// Nontransitional Processing: the deviation character sharp s (U+00DF) is kept
// and forms a valid domain label
TEST(deviation_sharp_s_domain) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46("user@fa\xc3\x9f"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("")); }

TEST(invalid_no_at) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("2962"));
}

TEST(invalid_missing_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("user@"));
}

TEST(invalid_missing_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("@example.com"));
}

TEST(invalid_leading_dot_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46(".user@example.com"));
}

TEST(invalid_trailing_dot_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("user.@example.com"));
}

TEST(invalid_consecutive_dots_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("a..b@example.com"));
}

// RFC 5321 §4.5.3.1.1: local part limit is 64 octets
TEST(valid_local_part_at_64) {
  const std::string local(64, 'a');
  EXPECT_TRUE(sourcemeta::core::is_idn_email_uts46(local + "@b"));
}

TEST(invalid_local_part_over_64) {
  const std::string local(65, 'a');
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46(local + "@b"));
}

// The mapping does not rescue a structurally invalid domain
TEST(invalid_leading_hyphen_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("user@-bad"));
}

TEST(invalid_trailing_dot_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("user@example."));
}

// A disallowed codepoint (U+0080 control) in the domain is preserved by the
// mapping and rejected by the per-label validity check
TEST(invalid_disallowed_domain_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("user@a\xc2\x80"
                                                    "b"));
}

// RFC 5893 §2: a digit-first label in a Bidi domain is invalid. Domain is
// "0" + Hebrew letter Alef (U+05D0)
TEST(invalid_bidi_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("user@0\xd7\x90"));
}

// Malformed UTF-8 in the domain is rejected before mapping
TEST(invalid_utf8_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("user@\xc0\x80"));
}

// Malformed UTF-8 in the local part is rejected
TEST(invalid_utf8_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email_uts46("\xc0\x80@b"));
}
