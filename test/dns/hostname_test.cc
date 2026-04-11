#include <gtest/gtest.h>

#include <sourcemeta/core/dns.h>

#include <string>

// VALID - RFC 1123 §2.1 / RFC 952 compliant inputs

// RFC 952 §B: <hname> ::= <name>*["."<name>]  (three labels, TS d7+ #7)
TEST(DNS_hostname, valid_simple_dotted) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("www.example.com"));
}

// RFC 952 §B: <hname> allows a single <name> (TS d7+ #8)
TEST(DNS_hostname, valid_single_label) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("hostname"));
}

// RFC 952 §B: interior digits are let-dig-hyp (TS d7+ #9)
TEST(DNS_hostname, valid_single_label_with_digits) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("h0stn4me"));
}

// RFC 1123 §2.1: first character may be a digit (TS d7+ #10)
TEST(DNS_hostname, valid_starts_with_digit) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("1host"));
}

// RFC 952 §B: <name> ends with <let-or-digit> (TS d7+ #11)
TEST(DNS_hostname, valid_ends_with_digit) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("hostnam3"));
}

// RFC 952 §B: interior '-' is let-dig-hyp (TS d7+ #17)
TEST(DNS_hostname, valid_interior_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("host-name"));
}

// RFC 952 §B: two minimal single-character labels
TEST(DNS_hostname, valid_two_labels) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a.b"));
}

// RFC 952 §B: single <let> is a valid <name>
TEST(DNS_hostname, valid_single_letter) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a"));
}

// RFC 1123 §2.1: single digit is a valid label
TEST(DNS_hostname, valid_single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("0"));
}

// RFC 952 ASSUMPTIONS: no distinction between upper and lower case
TEST(DNS_hostname, valid_single_uppercase) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("A"));
}

// RFC 952 ASSUMPTIONS: case-insensitive — mixed case is valid
TEST(DNS_hostname, valid_mixed_case) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("HosT.CoM"));
}

// RFC 1123 §2.1 MUST: label of exactly 63 chars (TS d4 #17, TS d7+ #23)
TEST(DNS_hostname, valid_label_exactly_63) {
  EXPECT_TRUE(sourcemeta::core::is_hostname(std::string(63, 'a') + ".com"));
}

// RFC 1123 §2.1 MUST: single label of exactly 63 chars
TEST(DNS_hostname, valid_single_label_63) {
  EXPECT_TRUE(sourcemeta::core::is_hostname(std::string(63, 'a')));
}

// RFC 1123 §2.1 SHOULD: 253-byte total is under the 255-char cap
TEST(DNS_hostname, valid_total_253) {
  // 63 + '.' + 63 + '.' + 63 + '.' + 61 = 253 bytes
  EXPECT_TRUE(sourcemeta::core::is_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(61, 'a')));
}

// RFC 1035 §2.3.4 via RFC 1123 §2.1: 254-byte hostname is under the 255 cap
TEST(DNS_hostname, valid_total_254) {
  // 63 + '.' + 63 + '.' + 63 + '.' + 62 = 254 bytes
  EXPECT_TRUE(sourcemeta::core::is_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(62, 'a')));
}

// RFC 1035 §2.3.4 via RFC 1123 §2.1: exactly at the 255-char SHOULD limit
TEST(DNS_hostname, valid_total_255) {
  // 4 * 63 + 3 dots = 255 bytes
  EXPECT_TRUE(sourcemeta::core::is_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(63, 'a')));
}

// RFC 952 §B: xn-- labels are plain ASCII; grammar accepts (TS d4 #8)
TEST(DNS_hostname, valid_punycoded_draft4) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--4gbwdl.xn--wgbh1c"));
}

// RFC 1123 §2.1: no positions-3-4 rule exists; we accept (documented TS d7+
// #20 divergence — test cites RFC 5891 §4.2.3.1, which is IDNA2008 only)
TEST(DNS_hostname, valid_xn_positions_34_both_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("XN--aa---o47jg78q"));
}

// RFC 952 §B: grammar has no rule against consecutive interior hyphens
TEST(DNS_hostname, valid_consecutive_interior_hyphens) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a--b"));
}

// RFC 1123 §2.1 DISCUSSION: numeric TLD is not forbidden by grammar
TEST(DNS_hostname, valid_numeric_tld) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("example.123"));
}

// RFC 952 §B: any number of labels separated by dots
TEST(DNS_hostname, valid_many_labels) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a.b.c.d.e.f"));
}

// INVALID - non-compliant inputs

// RFC 952 §B: <hname> requires at least one <name> / label (TS d7+ #12)
TEST(DNS_hostname, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(""));
}

// RFC 952 §B: bare '.' has no <name> before or after (TS d7+ #13)
TEST(DNS_hostname, invalid_single_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("."));
}

// RFC 952 §B: leading '.' yields an empty label (TS d7+ #14)
TEST(DNS_hostname, invalid_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(".example"));
}

// JSON Schema test suite: trailing dot is invalid from draft 4 onward (TS d7+
// #15)
TEST(DNS_hostname, invalid_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("example."));
}

// JSON Schema test suite: trailing dot generalised to single label
TEST(DNS_hostname, invalid_trailing_dot_single) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("host."));
}

// RFC 952 §B: double dot yields an empty label
TEST(DNS_hostname, invalid_double_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("example..com"));
}

// RFC 952 §B: double trailing dot yields empty labels
TEST(DNS_hostname, invalid_double_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.."));
}

// RFC 1123 §2.1: first char of label must be letter or digit (TS d7+ #18)
TEST(DNS_hostname, invalid_label_starts_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("-hostname"));
}

// RFC 952 §B + ASSUMPTIONS: last char must not be minus sign (TS d7+ #19)
TEST(DNS_hostname, invalid_label_ends_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("hostname-"));
}

// RFC 1123 §2.1: first-char rule applies to every label, not just the first
TEST(DNS_hostname, invalid_middle_label_starts_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.-b.c"));
}

// RFC 952 §B: final-char rule applies to every label, not just the last
TEST(DNS_hostname, invalid_middle_label_ends_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.b-.c"));
}

// RFC 952 ASSUMPTIONS: underscore is not in the alphabet (TS d4 #14)
TEST(DNS_hostname, invalid_underscore_start) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("_hostname"));
}

// RFC 952 ASSUMPTIONS: underscore is not in the alphabet (TS d4 #15)
TEST(DNS_hostname, invalid_underscore_end) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("hostname_"));
}

// RFC 952 ASSUMPTIONS: underscore is not in the alphabet (TS d4 #16, d7+ #21)
TEST(DNS_hostname, invalid_underscore_middle) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("host_name"));
}

// RFC 952 ASSUMPTIONS: no blank or space characters are permitted
TEST(DNS_hostname, invalid_space) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("host name"));
}

// RFC 1123 §2.1 MUST: label exceeds 63-character limit (TS d4 #18, d7+ #24)
TEST(DNS_hostname, invalid_label_64) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string(64, 'a') + ".com"));
}

// RFC 1123 §2.1 MUST: single label of 64 chars exceeds per-label limit
TEST(DNS_hostname, invalid_single_label_64) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string(64, 'a')));
}

// RFC 1035 §2.3.4 via RFC 1123 §2.1: total length exceeds 255 (constructed)
TEST(DNS_hostname, invalid_total_256) {
  // 63 + '.' + 63 + '.' + 63 + '.' + 62 + '.' + 'a' = 256 bytes; all labels
  // ≤ 63
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(62, 'a') + ".a"));
}

// RFC 1035 §2.3.4 via RFC 1123 §2.1: exact TS d7+ #22 input (259 bytes,
// mislabelled "256" in the test suite description)
TEST(DNS_hostname, invalid_ts_256_string) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "com"));
}

// RFC 952 ASSUMPTIONS: U+FF0E (fullwidth full stop) is not in the ASCII
// alphabet; UTF-8 bytes 0xEF 0xBC 0x8E (TS d4 #27, d7+ #16)
TEST(DNS_hostname, invalid_fullwidth_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\xef\xbc\x8e"
                                             "com"));
}

// RFC 952 ASSUMPTIONS: any byte >= 0x80 is outside the ASCII alphabet
TEST(DNS_hostname, invalid_high_bit_byte) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string_view{"a\x80"
                                                              "b",
                                                              3}));
}

// RFC 952 ASSUMPTIONS: NUL byte (0x00) is not in the ASCII alphabet
TEST(DNS_hostname, invalid_nul_byte) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string_view{"a\x00"
                                                              "b",
                                                              3}));
}

// RFC 952 ASSUMPTIONS: '@' is not in the alphabet (A-Z 0-9 '-' '.')
TEST(DNS_hostname, invalid_at_sign) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("user@host"));
}
