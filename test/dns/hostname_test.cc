#include <sourcemeta/core/dns.h>
#include <sourcemeta/core/test.h>

#include <string>

// RFC 952 §B: <hname> ::= <name>*["."<name>]  (three labels, TS d7+ #7)
TEST(valid_simple_dotted) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("www.example.com"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("www.example.com"));
}

// RFC 952 §B: <hname> allows a single <name> (TS d7+ #8)
TEST(valid_single_label) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("hostname"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("hostname"));
}

// RFC 952 §B: interior digits are let-dig-hyp (TS d7+ #9)
TEST(valid_single_label_with_digits) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("h0stn4me"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("h0stn4me"));
}

// RFC 1123 §2.1: first character may be a digit (TS d7+ #10)
TEST(valid_starts_with_digit) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("1host"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("1host"));
}

// RFC 952 §B: <name> ends with <let-or-digit> (TS d7+ #11)
TEST(valid_ends_with_digit) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("hostnam3"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("hostnam3"));
}

// RFC 952 §B: interior '-' is let-dig-hyp (TS d7+ #17)
TEST(valid_interior_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("host-name"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("host-name"));
}

// RFC 952 §B: two minimal single-character labels
TEST(valid_two_labels) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a.b"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("a.b"));
}

// RFC 952 §B: single <let> is a valid <name>
TEST(valid_single_letter) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("a"));
}

// RFC 1123 §2.1: single digit is a valid label
TEST(valid_single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("0"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("0"));
}

// RFC 952 ASSUMPTIONS: no distinction between upper and lower case
TEST(valid_single_uppercase) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("A"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("A"));
}

// RFC 952 ASSUMPTIONS: case-insensitive — mixed case is valid
TEST(valid_mixed_case) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("HosT.CoM"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("HosT.CoM"));
}

// RFC 1123 §2.1 MUST: label of exactly 63 chars (TS d4 #17, TS d7+ #23)
TEST(valid_label_exactly_63) {
  EXPECT_TRUE(sourcemeta::core::is_hostname(std::string(63, 'a') + ".com"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(std::string(63, 'a') + ".com"));
}

// RFC 1123 §2.1 MUST: single label of exactly 63 chars
TEST(valid_single_label_63) {
  EXPECT_TRUE(sourcemeta::core::is_hostname(std::string(63, 'a')));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(std::string(63, 'a')));
}

// RFC 1123 §2.1 SHOULD: 253-byte total is under the 255-char cap
TEST(valid_total_253) {
  // 63 + '.' + 63 + '.' + 63 + '.' + 61 = 253 bytes
  EXPECT_TRUE(sourcemeta::core::is_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(61, 'a')));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(61, 'a')));
}

// RFC 1123 §2.1: 254-byte hostname is under the 255 cap.
// RFC 1035 §3.1: IDN strict presentation form caps the total at 253 octets
TEST(total_254) {
  // 63 + '.' + 63 + '.' + 63 + '.' + 62 = 254 bytes
  EXPECT_TRUE(sourcemeta::core::is_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(62, 'a')));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(62, 'a')));
}

// RFC 1123 §2.1: exactly at the 255-char SHOULD limit.
// RFC 1035 §3.1: IDN strict presentation form rejects 255 octets
TEST(total_255) {
  // 4 * 63 + 3 dots = 255 bytes
  EXPECT_TRUE(sourcemeta::core::is_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(63, 'a')));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(63, 'a')));
}

// RFC 952 §B: xn-- labels are plain ASCII; grammar accepts (TS d4 #8)
TEST(valid_punycoded_draft4) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--4gbwdl.xn--wgbh1c"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("xn--4gbwdl.xn--wgbh1c"));
}

// RFC 5891 §4.2.3.1: case-insensitive xn-- detection treats this as an
// A-label whose Punycode body fails the IDNA 2008 validation
TEST(xn_positions_34_both_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("XN--aa---o47jg78q"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("XN--aa---o47jg78q"));
}

// RFC 5890 §2.3.2.1: a one-character Punycode body that does not decode to a
// valid U-label cannot be a real A-label
TEST(xn_undecodable_punycode_body) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--X"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--X"));
}

// RFC 5891 §4.2.3.2: decoded A-label must not start with a Spacing
// Combining Mark
TEST(xn_leading_spacing_combining_mark) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--hello-txk"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--hello-txk"));
}

// RFC 5891 §4.2.3.2: decoded A-label must not start with a Nonspacing Mark
TEST(xn_leading_nonspacing_mark) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--hello-zed"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--hello-zed"));
}

// RFC 5891 §4.2.3.2: decoded A-label must not start with an Enclosing Mark
TEST(xn_leading_enclosing_mark) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--hello-6bf"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--hello-6bf"));
}

// RFC 5892 §2.6: DISALLOWED code points (e.g. U+302E HANGUL SINGLE DOT
// TONE MARK) must not appear in the decoded U-label
TEST(xn_disallowed_codepoint_in_first_label) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--07jt112bpxg.xn--9t4b11yi5a"));
  EXPECT_FALSE(
      sourcemeta::core::is_idn_hostname("xn--07jt112bpxg.xn--9t4b11yi5a"));
}

// RFC 5892 §2.6: DISALLOWED right-to-left exception code point
TEST(xn_disallowed_rtl_exception) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--chb89f"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--chb89f"));
}

// RFC 5892 §2.6: DISALLOWED combining marks in non-leading position
TEST(xn_disallowed_non_leading_combining_marks) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--07jceefgh4c"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--07jceefgh4c"));
}

// RFC 5892 Appendix A.3: MIDDLE DOT must sit between two ASCII "l"
TEST(xn_middle_dot_no_leading_l) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--al-0ea"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--al-0ea"));
}

TEST(xn_middle_dot_no_preceding_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--l-fda"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--l-fda"));
}

TEST(xn_middle_dot_no_trailing_l) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--la-0ea"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--la-0ea"));
}

TEST(xn_middle_dot_no_following_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--l-gda"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--l-gda"));
}

// RFC 5892 Appendix A.4: Greek KERAIA must be followed by a Greek code point
TEST(xn_keraia_followed_by_non_greek) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--S-jib3p"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--S-jib3p"));
}

TEST(xn_keraia_at_end_of_label) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--wva3j"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--wva3j"));
}

// RFC 5892 Appendix A.5: Hebrew GERESH must be preceded by a Hebrew
// code point
TEST(xn_geresh_preceded_by_latin) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--A-2hc5h"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--A-2hc5h"));
}

TEST(xn_geresh_at_start_of_label) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--5db1e"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--5db1e"));
}

// RFC 5892 Appendix A.6: Hebrew GERSHAYIM must be preceded by a Hebrew
// code point
TEST(xn_gershayim_preceded_by_latin) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--A-2hc8h"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--A-2hc8h"));
}

TEST(xn_gershayim_at_start_of_label) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--5db3e"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--5db3e"));
}

// RFC 5892 Appendix A.7: KATAKANA MIDDLE DOT requires an adjacent
// Hiragana, Katakana, or Han code point
TEST(xn_katakana_middle_dot_only_latin_neighbours) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--defabc-k64e"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--defabc-k64e"));
}

TEST(xn_katakana_middle_dot_no_neighbours) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--vek"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--vek"));
}

// RFC 5892 Appendix A.8: Arabic-Indic digits and Extended Arabic-Indic
// digits must not coexist in the same label
TEST(xn_mixed_arabic_indic_digit_kinds) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--ngb6iyr"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--ngb6iyr"));
}

// RFC 5892 Appendix A.2: ZERO WIDTH JOINER must be preceded by a Virama
TEST(xn_zwj_not_preceded_by_virama) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--11b2er09f"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--11b2er09f"));
}

TEST(xn_zwj_at_start_of_label) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--02b508i"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--02b508i"));
}

// RFC 952 §B: grammar has no rule against consecutive interior hyphens
TEST(valid_consecutive_interior_hyphens) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a--b"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("a--b"));
}

// RFC 1123 §2.1 DISCUSSION: numeric TLD is not forbidden by grammar
TEST(valid_numeric_tld) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("example.123"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("example.123"));
}

// RFC 952 §B: any number of labels separated by dots
TEST(valid_many_labels) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a.b.c.d.e.f"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("a.b.c.d.e.f"));
}

// RFC 952 §B: <hname> requires at least one <name> / label (TS d7+ #12)
TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(""));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(""));
}

// RFC 952 §B: bare '.' has no <name> before or after (TS d7+ #13)
TEST(invalid_single_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("."));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("."));
}

// RFC 952 §B: leading '.' yields an empty label (TS d7+ #14)
TEST(invalid_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(".example"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(".example"));
}

// JSON Schema test suite: trailing dot is invalid from draft 4 onward (TS d7+
// #15)
TEST(invalid_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("example."));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("example."));
}

// JSON Schema test suite: trailing dot generalised to single label
TEST(invalid_trailing_dot_single) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("host."));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("host."));
}

// RFC 952 §B: double dot yields an empty label
TEST(invalid_double_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("example..com"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("example..com"));
}

// RFC 952 §B: double trailing dot yields empty labels
TEST(invalid_double_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.."));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("example.."));
}

// RFC 1123 §2.1: first char of label must be letter or digit (TS d7+ #18)
TEST(invalid_label_starts_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("-hostname"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("-hostname"));
}

// RFC 952 §B + ASSUMPTIONS: last char must not be minus sign (TS d7+ #19)
TEST(invalid_label_ends_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("hostname-"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("hostname-"));
}

// RFC 1123 §2.1: first-char rule applies to every label, not just the first
TEST(invalid_middle_label_starts_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.-b.c"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a.-b.c"));
}

// RFC 952 §B: final-char rule applies to every label, not just the last
TEST(invalid_middle_label_ends_with_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.b-.c"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a.b-.c"));
}

// RFC 952 ASSUMPTIONS: underscore is not in the alphabet (TS d4 #14)
TEST(invalid_underscore_start) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("_hostname"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("_hostname"));
}

// RFC 952 ASSUMPTIONS: underscore is not in the alphabet (TS d4 #15)
TEST(invalid_underscore_end) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("hostname_"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("hostname_"));
}

// RFC 952 ASSUMPTIONS: underscore is not in the alphabet (TS d4 #16, d7+ #21)
TEST(invalid_underscore_middle) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("host_name"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("host_name"));
}

// RFC 952 ASSUMPTIONS: no blank or space characters are permitted
TEST(invalid_space) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("host name"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("host name"));
}

// RFC 1123 §2.1 MUST: label exceeds 63-character limit (TS d4 #18, d7+ #24)
TEST(invalid_label_64) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string(64, 'a') + ".com"));
  EXPECT_FALSE(
      sourcemeta::core::is_idn_hostname(std::string(64, 'a') + ".com"));
}

// RFC 1123 §2.1 MUST: single label of 64 chars exceeds per-label limit
TEST(invalid_single_label_64) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string(64, 'a')));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(std::string(64, 'a')));
}

// RFC 1035 §2.3.4 via RFC 1123 §2.1: total length exceeds 255 (constructed)
TEST(invalid_total_256) {
  // 63 + '.' + 63 + '.' + 63 + '.' + 62 + '.' + 'a' = 256 bytes; all labels
  // ≤ 63
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(62, 'a') + ".a"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      std::string(63, 'a') + "." + std::string(63, 'a') + "." +
      std::string(63, 'a') + "." + std::string(62, 'a') + ".a"));
}

// RFC 1035 §2.3.4 via RFC 1123 §2.1: exact TS d7+ #22 input (259 bytes,
// mislabelled "256" in the test suite description)
TEST(invalid_ts_256_string) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "com"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
      "com"));
}

// RFC 952 ASSUMPTIONS: U+FF0E (fullwidth full stop) is not in the ASCII
// alphabet; UTF-8 bytes 0xEF 0xBC 0x8E (TS d4 #27, d7+ #16)
TEST(invalid_fullwidth_dot) {
  // RFC 1123 §2.1: U+FF0E (FULLWIDTH FULL STOP) is not the ASCII label
  // separator. is_idn_hostname accepts it as a normal UTF-8 codepoint within
  // a label per RFC 5890 §2.3.2.3 (best-effort lexical handling); strict
  // IDNA2008 would map and reject, but that is out of scope here
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\xef\xbc\x8e"
                                             "com"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("example\xef\xbc\x8e"
                                                "com"));
}

// RFC 952 ASSUMPTIONS: any byte >= 0x80 is outside the ASCII alphabet
TEST(invalid_high_bit_byte) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string_view{"a\x80"
                                                              "b",
                                                              3}));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(std::string_view{"a\x80"
                                                                  "b",
                                                                  3}));
}

// RFC 952 ASSUMPTIONS: NUL byte (0x00) is not in the ASCII alphabet
TEST(invalid_nul_byte) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string_view{"a\x00"
                                                              "b",
                                                              3}));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(std::string_view{"a\x00"
                                                                  "b",
                                                                  3}));
}

// RFC 952 ASSUMPTIONS: '@' is not in the alphabet (A-Z 0-9 '-' '.')
TEST(invalid_at_sign) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("user@host"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("user@host"));
}

// RFC 1123 §2.1: 255-byte cap via many short labels, not four long ones
TEST(valid_total_255_many_short_labels) {
  // 128 single-character labels and 127 dots is 255 bytes, right at the cap
  std::string hostname{"a"};
  for (int index = 0; index < 127; ++index) {
    hostname.append(".a");
  }
  EXPECT_EQ(hostname.size(), 255u);
  EXPECT_TRUE(sourcemeta::core::is_hostname(hostname));
}

// RFC 1123 §2.1: one byte over the 255 cap, same many-short-labels shape
TEST(invalid_total_256_many_short_labels) {
  // the same shape one byte over the cap, with every label still well formed
  std::string hostname{"aa"};
  for (int index = 0; index < 127; ++index) {
    hostname.append(".a");
  }
  EXPECT_EQ(hostname.size(), 256u);
  EXPECT_FALSE(sourcemeta::core::is_hostname(hostname));
}

// RFC 1123 §2.1: the length check runs before any label scan
TEST(invalid_total_far_over_cap) {
  // the total-length check runs before any label scan, so an input far past
  // the cap is rejected without walking it
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string(100000, 'a')));
}

// RFC 1035 §2.3.4: a 63-octet label, not in the first position
TEST(valid_middle_label_63) {
  // 63-character label between two other labels
  EXPECT_TRUE(
      sourcemeta::core::is_hostname("a." + std::string(63, 'a') + ".b"));
}

TEST(valid_last_label_63) {
  // 63-character label in last position
  EXPECT_TRUE(sourcemeta::core::is_hostname("a." + std::string(63, 'a')));
}

// RFC 1035 §2.3.4: a 64-octet label, not in the first position
TEST(invalid_middle_label_64) {
  // 64-character label between two other labels
  EXPECT_FALSE(
      sourcemeta::core::is_hostname("a." + std::string(64, 'a') + ".b"));
}

TEST(invalid_last_label_64) {
  // 64-character label in last position
  EXPECT_FALSE(sourcemeta::core::is_hostname("a." + std::string(64, 'a')));
}

// RFC 952 §B: a lone hyphen (or two) has no leading let-dig
TEST(invalid_hyphen_only) {
  // a lone hyphen has no leading let-dig
  EXPECT_FALSE(sourcemeta::core::is_hostname("-"));
}

TEST(invalid_double_hyphen_only) {
  // two hyphens still have no leading let-dig
  EXPECT_FALSE(sourcemeta::core::is_hostname("--"));
}

// RFC 952 ASSUMPTIONS + RFC 1123 §2.1: shortest leading/trailing hyphen label
TEST(invalid_minimal_trailing_hyphen) {
  // shortest label ending in a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("a-"));
}

TEST(invalid_minimal_leading_hyphen) {
  // shortest label starting with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("-a"));
}

// RFC 1123 §2.1 + RFC 952 ASSUMPTIONS: the hyphen rule on a non-last label
TEST(invalid_first_of_two_leading_hyphen) {
  // first label of two starts with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("-a.b"));
}

TEST(invalid_first_of_two_trailing_hyphen) {
  // first label of two ends with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("a-.b"));
}

// RFC 1123 §2.1 + RFC 952 ASSUMPTIONS: the hyphen rule on the last label
TEST(invalid_last_label_leading_hyphen) {
  // last label starts with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.-b"));
}

TEST(invalid_last_label_trailing_hyphen) {
  // last label ends with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.b-"));
}

// RFC 952 §B: a whole label that is just a hyphen
TEST(invalid_hyphen_label_first) {
  // a whole label that is just a hyphen, first
  EXPECT_FALSE(sourcemeta::core::is_hostname("-.a"));
}

TEST(invalid_hyphen_label_last) {
  // a whole label that is just a hyphen, last
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.-"));
}

// RFC 952 §B: grammar has no rule against consecutive interior hyphens
TEST(valid_three_interior_hyphens) {
  // three consecutive interior hyphens
  EXPECT_TRUE(sourcemeta::core::is_hostname("a---b"));
}

TEST(valid_hyphen_heavy_label_63) {
  // 63 characters that are hyphens except the two required let-dig ends
  EXPECT_TRUE(sourcemeta::core::is_hostname("a" + std::string(61, '-') + "b"));
}

TEST(valid_hyphen_in_every_label) {
  // an interior hyphen in each of three labels
  EXPECT_TRUE(sourcemeta::core::is_hostname("a-b.c-d.e-f"));
}

// RFC 952 §B: <hname> requires a <name> between dots
TEST(invalid_two_dots_only) {
  // two dots with no label anywhere
  EXPECT_FALSE(sourcemeta::core::is_hostname(".."));
}

TEST(invalid_three_dots_only) {
  // three dots with no label anywhere
  EXPECT_FALSE(sourcemeta::core::is_hostname("..."));
}

// RFC 952 §B: shortest empty-label forms, leading, trailing and interior
TEST(invalid_minimal_trailing_dot) {
  // shortest trailing-dot form
  EXPECT_FALSE(sourcemeta::core::is_hostname("a."));
}

TEST(invalid_minimal_leading_dot) {
  // shortest leading-dot form
  EXPECT_FALSE(sourcemeta::core::is_hostname(".a"));
}

TEST(invalid_minimal_double_dot) {
  // shortest empty interior label
  EXPECT_FALSE(sourcemeta::core::is_hostname("a..b"));
}

// RFC 952 §B: empty-label rule on names with more than one label
TEST(invalid_fqdn_trailing_dot) {
  // root-anchored FQDN form with three labels
  EXPECT_FALSE(sourcemeta::core::is_hostname("www.example.com."));
}

TEST(invalid_leading_dot_multi_label) {
  // leading dot before a two-label name
  EXPECT_FALSE(sourcemeta::core::is_hostname(".example.com"));
}

TEST(invalid_double_dot_before_tld) {
  // two trailing dots after a two-label name
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.com.."));
}

// RFC 952 §B: <hname> ::= <name>*["."<name>] places no cap on label count
TEST(valid_twenty_labels) {
  // twenty single-character labels
  EXPECT_TRUE(
      sourcemeta::core::is_hostname("a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p.q.r.s.t"));
}

// RFC 5890 §2.3.2.1: bare ACE prefix caught by the trailing-hyphen rule first
TEST(xn_prefix_only) {
  // the bare ACE prefix: the trailing-hyphen rule rejects it before the
  // A-label check runs
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--"));
}

// RFC 5890 §2.3.2.1: the ACE prefix is case-insensitive
TEST(xn_prefix_only_uppercase) {
  // the same, in uppercase
  EXPECT_FALSE(sourcemeta::core::is_hostname("XN--"));
}

// too short, or missing, the two-hyphen ACE prefix
TEST(xn_two_characters) {
  // two characters, too short to carry the ACE prefix
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn"));
}

TEST(xn_three_characters) {
  // three characters, ends in a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn-"));
}

TEST(xn_single_hyphen_at_three) {
  // hyphen at position 3 only, so not an A-label
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn-a"));
}

// RFC 5890 §2.3.2.1: a body that does not decode to a valid U-label
TEST(xn_body_single_letter) {
  // a one-character Punycode body that does not decode to a U-label
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--a"));
}

// RFC 5890 §2.3.2.1: the ACE prefix only counts at the start of a label
TEST(xn_prefix_not_at_label_start) {
  // the ACE prefix only counts at the start of a label
  EXPECT_TRUE(sourcemeta::core::is_hostname("axn--1234"));
}

// RFC 5890 §2.3.2.1: the case-insensitive ACE prefix, every case combination
TEST(xn_prefix_mixed_case_lower_body) {
  // mixed-case prefix, lowercase body
  EXPECT_TRUE(sourcemeta::core::is_hostname("Xn--4gbwdl"));
}

TEST(xn_prefix_mixed_case_second_char) {
  // mixed-case prefix on the second character
  EXPECT_TRUE(sourcemeta::core::is_hostname("xN--4gbwdl"));
}

TEST(xn_prefix_uppercase_lower_body) {
  // uppercase prefix, lowercase body
  EXPECT_TRUE(sourcemeta::core::is_hostname("XN--4gbwdl"));
}

// RFC 5890 §2.3.2.1: a valid A-label, alone and after an ASCII label
TEST(xn_single_label_valid) {
  // a valid A-label on its own
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--4gbwdl"));
}

TEST(xn_valid_second_label) {
  // a valid A-label after an ASCII label
  EXPECT_TRUE(sourcemeta::core::is_hostname("example.xn--wgbh1c"));
}

// RFC 5890 §2.3.2.1: the A-label check also runs on non-leading labels
TEST(xn_invalid_second_label) {
  // the A-label check also runs on non-leading labels
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.xn--X"));
}

// RFC 5890 §2.3.2.1: real A-labels, including two around an ASCII label
TEST(xn_valid_russian_tld) {
  // the A-label for the Cyrillic .rf top-level domain
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--p1ai"));
}

TEST(xn_valid_two_a_labels_and_ascii) {
  // A-labels around an ASCII label
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--p1ai.example.xn--p1ai"));
}

// RFC 952 ASSUMPTIONS: the last-character rule runs before the A-label check
TEST(xn_body_trailing_hyphen) {
  // an ACE-prefixed label ending in a hyphen, rejected by the LDH rule first
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--a-"));
}

// RFC 5890 §2.3.2.1: the bare ACE prefix as a middle label
TEST(xn_prefix_only_middle_label) {
  // the bare ACE prefix as a middle label
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.xn--.b"));
}

// RFC 1035 §2.3.4: a real A-label at the 63-octet cap and one octet past it
TEST(xn_real_a_label_at_63) {
  // the A-label for 55 letter a followed by U+00E4, exactly at the 63-octet
  // cap
  EXPECT_TRUE(
      sourcemeta::core::is_hostname("xn--" + std::string(55, 'a') + "-uve"));
}

TEST(xn_real_a_label_at_64) {
  // the same shape with one more letter a, one octet past the cap
  EXPECT_FALSE(
      sourcemeta::core::is_hostname("xn--" + std::string(56, 'a') + "-qye"));
}

// RFC 5890 §2.3.2.1: Fake A-label, LDH-clean but not real Punycode
TEST(xn_fake_body_leading_delimiter) {
  // RFC 5890 §2.3.2.1 Fake A-label: LDH-clean, but the Punycode body starts
  // with the delimiter
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn---p1ai"));
}

TEST(xn_fake_body_trailing_basic) {
  // RFC 5890 §2.3.2.1 Fake A-label: the body does not round-trip through
  // Punycode
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--p1ai-a"));
}

// RFC 5890 §2.3.2.1: a valid A-label followed by an ASCII label
TEST(xn_valid_a_label_then_ascii) {
  // a valid A-label followed by an ASCII label
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--p1ai.example"));
}

// RFC 952 ASSUMPTIONS: no blank characters permitted (trailing terminator)
TEST(invalid_trailing_newline) {
  // a trailing newline, the classic line-oriented reader artefact
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.com\n"));
}

TEST(invalid_trailing_carriage_return) {
  // a trailing carriage return
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.com\r"));
}

TEST(invalid_trailing_crlf) {
  // a trailing CRLF pair
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.com\r\n"));
}

// RFC 952 ASSUMPTIONS: no blank characters permitted (leading newline)
TEST(invalid_leading_newline) {
  // a leading newline
  EXPECT_FALSE(sourcemeta::core::is_hostname("\nexample.com"));
}

TEST(invalid_newline_only) {
  // a lone newline
  EXPECT_FALSE(sourcemeta::core::is_hostname("\n"));
}

// RFC 952 ASSUMPTIONS: no blank characters permitted (tab and space)
TEST(invalid_trailing_tab) {
  // a trailing tab
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.com\t"));
}

TEST(invalid_leading_space) {
  // a leading space
  EXPECT_FALSE(sourcemeta::core::is_hostname(" example.com"));
}

TEST(invalid_trailing_space) {
  // a trailing space
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.com "));
}

TEST(invalid_space_only) {
  // a lone space
  EXPECT_FALSE(sourcemeta::core::is_hostname(" "));
}

// RFC 952 ASSUMPTIONS: a newline where the label separator would go
TEST(invalid_interior_newline) {
  // a newline where the label separator would go
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\ncom"));
}

// RFC 952 ASSUMPTIONS: NUL is outside the alphabet, kept at an explicit length
TEST(invalid_nul_only) {
  // a lone NUL byte
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string_view("\0", 1)));
}

TEST(invalid_trailing_nul) {
  // a valid name followed by a NUL
  EXPECT_FALSE(
      sourcemeta::core::is_hostname(std::string_view("example.com\0", 12)));
}

TEST(invalid_leading_nul) {
  // a NUL before a valid name
  EXPECT_FALSE(
      sourcemeta::core::is_hostname(std::string_view("\0example.com", 12)));
}

TEST(invalid_nul_truncation_payload) {
  // a NUL that would truncate the name in a C string reader
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      std::string_view("example.com\0.evil.test", 22)));
}

// RFC 1123 §2.1: leading-position let-dig, every byte value 0x00-0xFF
TEST(byte_alphabet_leading_0x00_0x1f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string_view("\x00"
                                                              "ab",
                                                              3)));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x01"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x02"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x03"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x04"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x05"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x06"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x07"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x08"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x09"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x0a"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x0b"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x0c"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x0d"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x0e"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x0f"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x10"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x11"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x12"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x13"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x14"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x15"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x16"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x17"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x18"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x19"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x1a"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x1b"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x1c"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x1d"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x1e"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x1f"
                                             "ab"));
}

TEST(byte_alphabet_leading_0x20_0x2f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(" ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("!ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x22"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("#ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("$ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("%ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("&ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("'ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("(ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(")ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("*ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("+ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(",ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("-ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(".ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("/ab"));
}

TEST(byte_alphabet_leading_0x30_0x39) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("0ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("1ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("2ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("3ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("4ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("5ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("6ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("7ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("8ab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("9ab"));
}

TEST(byte_alphabet_leading_0x3a_0x40) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(":ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(";ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("<ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("=ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(">ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("?ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("@ab"));
}

TEST(byte_alphabet_leading_0x41_0x5a) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("Aab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Bab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Cab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Dab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Eab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Fab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Gab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Hab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Iab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Jab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Kab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Lab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Mab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Nab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Oab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Pab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Qab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Rab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Sab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Tab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Uab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Vab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Wab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Xab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Yab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("Zab"));
}

TEST(byte_alphabet_leading_0x5b_0x60) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("[ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x5c"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("]ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("^ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("_ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("`ab"));
}

TEST(byte_alphabet_leading_0x61_0x7a) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("aab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("bab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("cab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("dab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("eab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("fab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("gab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("hab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("iab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("jab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("kab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("lab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("mab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("nab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("oab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("pab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("qab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("rab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("sab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("tab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("uab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("vab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("wab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("xab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("yab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("zab"));
}

TEST(byte_alphabet_leading_0x7b_0x7f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("{ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("|ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("}ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("~ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x7f"
                                             "ab"));
}

TEST(byte_alphabet_leading_0x80_0x9f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x80"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x81"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x82"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x83"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x84"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x85"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x86"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x87"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x88"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x89"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x8a"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x8b"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x8c"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x8d"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x8e"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x8f"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x90"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x91"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x92"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x93"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x94"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x95"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x96"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x97"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x98"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x99"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x9a"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x9b"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x9c"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x9d"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x9e"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x9f"
                                             "ab"));
}

TEST(byte_alphabet_leading_0xa0_0xbf) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa0"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa1"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa2"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa3"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa4"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa5"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa6"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa7"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa8"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xa9"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xaa"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xab"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xac"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xad"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xae"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xaf"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb0"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb1"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb2"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb3"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb4"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb5"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb6"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb7"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb8"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xb9"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xba"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xbb"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xbc"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xbd"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xbe"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xbf"
                                             "ab"));
}

TEST(byte_alphabet_leading_0xc0_0xdf) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc0"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc1"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc2"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc3"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc4"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc5"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc6"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc7"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc8"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc9"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xca"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xcb"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xcc"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xcd"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xcf"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd0"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd1"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd2"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd3"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd4"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd5"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd6"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd7"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd8"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd9"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xda"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xdb"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xdc"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xdd"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xde"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xdf"
                                             "ab"));
}

TEST(byte_alphabet_leading_0xe0_0xff) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe0"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe1"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe2"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe3"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe4"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe5"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe6"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe7"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe8"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe9"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xea"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xeb"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xec"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xed"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xee"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xef"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf0"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf1"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf2"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf3"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf4"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf5"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf6"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf7"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf8"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf9"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xfa"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xfb"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xfc"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xfd"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xfe"
                                             "ab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xff"
                                             "ab"));
}

// RFC 952 §B: interior-position let-dig-hyp and the dot, every byte value
TEST(byte_alphabet_interior_0x00_0x1f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string_view("a"
                                                              "\x00"
                                                              "b",
                                                              3)));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x01"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x02"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x03"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x04"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x05"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x06"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x07"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x08"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x09"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x0a"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x0b"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x0c"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x0d"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x0e"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x0f"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x10"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x11"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x12"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x13"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x14"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x15"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x16"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x17"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x18"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x19"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x1a"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x1b"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x1c"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x1d"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x1e"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x1f"
                                             "b"));
}

TEST(byte_alphabet_interior_0x20_0x2f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a!b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x22"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a#b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a$b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a%b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a&b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a'b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a(b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a)b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a*b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a+b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a,b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a-b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a.b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a/b"));
}

TEST(byte_alphabet_interior_0x30_0x39) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("a0b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a1b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a2b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a3b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a4b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a5b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a6b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a7b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a8b"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("a9b"));
}

TEST(byte_alphabet_interior_0x3a_0x40) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a:b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a;b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a<b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a=b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a>b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a?b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a@b"));
}

TEST(byte_alphabet_interior_0x41_0x5a) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("aAb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aBb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aCb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aDb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aEb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aFb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aGb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aHb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aIb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aJb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aKb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aLb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aMb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aNb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aOb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aPb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aQb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aRb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aSb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aTb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aUb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aVb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aWb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aXb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aYb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aZb"));
}

TEST(byte_alphabet_interior_0x5b_0x60) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a[b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x5c"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a]b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a^b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a_b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a`b"));
}

TEST(byte_alphabet_interior_0x61_0x7a) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("aab"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("acb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("adb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aeb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("afb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("agb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ahb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aib"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ajb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("akb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("alb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("amb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("anb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aob"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("apb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aqb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("arb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("asb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("atb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aub"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("avb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("awb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("axb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ayb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("azb"));
}

TEST(byte_alphabet_interior_0x7b_0x7f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a{b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a|b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a}b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a~b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x7f"
                                             "b"));
}

TEST(byte_alphabet_interior_0x80_0x9f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x80"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x81"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x82"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x83"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x84"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x85"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x86"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x87"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x88"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x89"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x8a"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x8b"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x8c"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x8d"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x8e"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x8f"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x90"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x91"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x92"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x93"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x94"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x95"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x96"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x97"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x98"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x99"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x9a"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x9b"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x9c"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x9d"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x9e"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\x9f"
                                             "b"));
}

TEST(byte_alphabet_interior_0xa0_0xbf) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa0"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa1"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa2"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa3"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa4"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa5"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa6"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa7"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa8"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xa9"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xaa"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xab"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xac"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xad"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xae"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xaf"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb0"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb1"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb2"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb3"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb4"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb5"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb6"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb7"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb8"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xb9"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xba"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xbb"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xbc"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xbd"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xbe"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xbf"
                                             "b"));
}

TEST(byte_alphabet_interior_0xc0_0xdf) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc0"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc1"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc2"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc3"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc4"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc5"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc6"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc7"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc8"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xc9"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xca"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xcb"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xcc"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xcd"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xce"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xcf"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd0"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd1"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd2"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd3"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd4"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd5"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd6"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd7"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd8"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xd9"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xda"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xdb"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xdc"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xdd"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xde"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xdf"
                                             "b"));
}

TEST(byte_alphabet_interior_0xe0_0xff) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe0"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe1"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe2"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe3"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe4"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe5"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe6"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe7"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe8"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xe9"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xea"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xeb"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xec"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xed"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xee"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xef"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf0"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf1"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf2"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf3"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf4"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf5"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf6"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf7"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf8"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xf9"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xfa"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xfb"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xfc"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xfd"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xfe"
                                             "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a"
                                             "\xff"
                                             "b"));
}

// RFC 952 §B: trailing-position let-dig, every byte value 0x00-0xFF
TEST(byte_alphabet_trailing_0x00_0x1f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string_view("ab"
                                                              "\x00",
                                                              3)));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x01"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x02"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x03"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x04"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x05"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x06"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x07"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x08"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x09"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x0a"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x0b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x0c"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x0d"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x0e"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x0f"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x10"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x11"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x12"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x13"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x14"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x15"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x16"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x17"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x18"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x19"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x1a"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x1b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x1c"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x1d"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x1e"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x1f"));
}

TEST(byte_alphabet_trailing_0x20_0x2f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab "));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab!"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x22"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab#"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab$"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab%"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab&"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab'"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab("));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab)"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab*"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab+"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab,"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab-"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab."));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab/"));
}

TEST(byte_alphabet_trailing_0x30_0x39) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab0"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab1"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab2"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab3"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab4"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab5"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab6"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab7"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab8"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("ab9"));
}

TEST(byte_alphabet_trailing_0x3a_0x40) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab:"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab;"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab<"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab="));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab>"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab?"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab@"));
}

TEST(byte_alphabet_trailing_0x41_0x5a) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("abA"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abB"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abC"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abD"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abE"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abF"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abG"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abH"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abI"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abJ"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abK"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abL"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abM"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abN"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abO"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abP"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abQ"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abR"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abS"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abT"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abU"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abV"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abW"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abX"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abY"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abZ"));
}

TEST(byte_alphabet_trailing_0x5b_0x60) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab["));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x5c"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab]"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab^"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab_"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab`"));
}

TEST(byte_alphabet_trailing_0x61_0x7a) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("aba"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abb"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abc"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abd"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abe"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abf"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abg"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abh"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abi"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abj"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abk"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abl"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abm"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abn"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abo"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abp"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abq"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abr"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abs"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abt"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abu"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abv"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abw"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abx"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("aby"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("abz"));
}

TEST(byte_alphabet_trailing_0x7b_0x7f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab{"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab|"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab}"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab~"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x7f"));
}

TEST(byte_alphabet_trailing_0x80_0x9f) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x81"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x82"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x83"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x84"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x85"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x86"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x87"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x88"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x89"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x8a"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x8b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x8c"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x8d"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x8e"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x8f"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x90"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x91"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x92"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x93"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x94"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x95"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x96"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x97"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x98"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x99"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x9a"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x9b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x9c"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x9d"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x9e"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\x9f"));
}

TEST(byte_alphabet_trailing_0xa0_0xbf) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa2"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa3"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa4"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa5"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa6"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xa9"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xaa"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xab"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xac"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xad"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xae"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xaf"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb2"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb3"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb4"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb5"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb6"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xb9"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xba"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xbb"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xbc"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xbd"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xbe"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xbf"));
}

TEST(byte_alphabet_trailing_0xc0_0xdf) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc2"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc3"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc4"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc5"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc6"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xc9"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xca"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xcb"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xcc"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xcd"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xce"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xcf"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd2"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd3"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd4"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd5"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd6"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xd9"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xda"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xdb"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xdc"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xdd"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xde"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xdf"));
}

TEST(byte_alphabet_trailing_0xe0_0xff) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe2"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe3"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe4"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe5"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe6"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xe9"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xea"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xeb"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xec"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xed"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xee"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xef"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf2"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf3"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf4"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf5"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf6"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xf9"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xfa"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xfb"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xfc"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xfd"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xfe"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab"
                                             "\xff"));
}

// RFC 952 ASSUMPTIONS: an ASCII-case-fold is still outside the alphabet
TEST(invalid_kelvin_sign) {
  // U+212A KELVIN SIGN, which case-folds to ASCII k
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xe2\x84\xaa"
                                             "b"));
}

TEST(invalid_long_s) {
  // U+017F LATIN SMALL LETTER LONG S, which case-folds to ASCII s
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xc5\xbf"
                                             "b"));
}

TEST(invalid_dotless_i) {
  // U+0131 LATIN SMALL LETTER DOTLESS I
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xc4\xb1"
                                             "b"));
}

// RFC 952 ASSUMPTIONS: only ASCII 0-9 are digits; other digit scripts are not
TEST(invalid_arabic_indic_digit) {
  // U+0660 ARABIC-INDIC DIGIT ZERO
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xd9\xa0"
                                             "b"));
}

TEST(invalid_fullwidth_digit) {
  // U+FF10 FULLWIDTH DIGIT ZERO
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xef\xbc\x90"
                                             "b"));
}

TEST(invalid_devanagari_digit) {
  // U+0966 DEVANAGARI DIGIT ZERO
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xe0\xa5\xa6"
                                             "b"));
}

// RFC 952 ASSUMPTIONS: an accented letter, a non-ASCII space, both outside it
TEST(invalid_latin_small_e_acute) {
  // U+00E9, a plain accented Latin letter
  EXPECT_FALSE(sourcemeta::core::is_hostname("caf\xc3\xa9.test"));
}

TEST(invalid_non_breaking_space) {
  // U+00A0 NO-BREAK SPACE
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xc2\xa0"
                                             "b"));
}

// RFC 952 ASSUMPTIONS: only U+002E is the dot; IDNA separators are not
TEST(invalid_ideographic_full_stop) {
  // U+3002 IDEOGRAPHIC FULL STOP, an IDNA separator but not an ASCII dot
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\xe3\x80\x82"
                                             "com"));
}

TEST(invalid_halfwidth_ideographic_full_stop) {
  // U+FF61 HALFWIDTH IDEOGRAPHIC FULL STOP
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\xef\xbd\xa1"
                                             "com"));
}

TEST(invalid_one_dot_leader) {
  // U+2024 ONE DOT LEADER, a full stop look-alike
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\xe2\x80\xa4"
                                             "com"));
}

// RFC 952 ASSUMPTIONS: a zero-width joiner is not in the alphabet
TEST(invalid_zero_width_joiner) {
  // U+200D ZERO WIDTH JOINER
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xe2\x80\x8d"
                                             "b"));
}

// RFC 952 ASSUMPTIONS: malformed and lone UTF-8 bytes are outside the alphabet
TEST(invalid_high_byte_leading) {
  // a lone UTF-8 continuation byte in leading position
  EXPECT_FALSE(sourcemeta::core::is_hostname("\x80"
                                             "ab"));
}

TEST(invalid_high_byte_trailing) {
  // 0xFF, which never appears in valid UTF-8
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab\xff"));
}

TEST(invalid_overlong_full_stop) {
  // an overlong UTF-8 encoding of the ASCII full stop
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\xc0\xae"
                                             "com"));
}

TEST(invalid_truncated_utf8) {
  // a truncated two-byte UTF-8 sequence
  EXPECT_FALSE(sourcemeta::core::is_hostname("ab\xc3"));
}

// RFC 1123 §2.1 DISCUSSION: a numeric TLD is not forbidden by the grammar
TEST(valid_localhost) {
  // the single-label name every resolver ships with
  EXPECT_TRUE(sourcemeta::core::is_hostname("localhost"));
}

TEST(valid_all_numeric_dotted_quad) {
  // RFC 1123 §2.1 mentions the dotted-decimal form only in a DISCUSSION
  // block, and the §2.5 requirements table has no entry for it, so the
  // grammar accepts it
  EXPECT_TRUE(sourcemeta::core::is_hostname("127.0.0.1"));
}

TEST(valid_all_numeric_single_label) {
  // a label with no letters at all
  EXPECT_TRUE(sourcemeta::core::is_hostname("1234567890"));
}

TEST(valid_max_dotted_quad) {
  // the all-numeric form at its widest
  EXPECT_TRUE(sourcemeta::core::is_hostname("255.255.255.255"));
}

// RFC 1123 §2.1: the first-character relaxation applies in every label
TEST(valid_digit_first_every_label) {
  // RFC 1123 §2.1 relaxes the first character to let-dig in every label
  EXPECT_TRUE(sourcemeta::core::is_hostname("1a.2b.3c"));
}

TEST(valid_all_digit_label_63) {
  // 63 digits in a single label
  EXPECT_TRUE(sourcemeta::core::is_hostname(std::string(63, '1')));
}

// RFC 952 §B: <let-dig> covers the full A-Z, a-z and 0-9 ranges
TEST(valid_uppercase_only) {
  // an entirely uppercase name
  EXPECT_TRUE(sourcemeta::core::is_hostname("EXAMPLE.COM"));
}

TEST(valid_single_uppercase_z) {
  // the top of the uppercase range
  EXPECT_TRUE(sourcemeta::core::is_hostname("Z"));
}

TEST(valid_single_lowercase_z) {
  // the top of the lowercase range
  EXPECT_TRUE(sourcemeta::core::is_hostname("z"));
}

TEST(valid_single_digit_nine) {
  // the top of the digit range
  EXPECT_TRUE(sourcemeta::core::is_hostname("9"));
}

// RFC 5891 §5.3: "first ensuring that the A-label is entirely in lowercase
// (converting it to lowercase if necessary)". RFC 5890 §2.3.2.4 notes an
// A-label matches "other (mixed-case or uppercase) potential labels in the DNS"
TEST(xn_uppercase_body) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--NXASMQ6B"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("xn--NXASMQ6B"));
}

TEST(xn_uppercase_body_with_tld) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--NXASMQ6B.com"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("xn--NXASMQ6B.com"));
}

TEST(xn_uppercase_prefix_and_body) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("XN--NXASMQ6B.COM"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("XN--NXASMQ6B.COM"));
}

TEST(xn_mixed_case_body) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--NxAsMq6B.com"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("xn--NxAsMq6B.com"));
}

TEST(xn_uppercase_basic_portion) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("XN--MNCHEN-3YA"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("XN--MNCHEN-3YA"));
}

TEST(xn_uppercase_multiple_a_labels) {
  EXPECT_TRUE(sourcemeta::core::is_hostname("XN--4GBWDL.XN--WGBH1C"));
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("XN--4GBWDL.XN--WGBH1C"));
}

// Case folding must not rescue a label that is invalid on its own merits
TEST(xn_uppercase_leading_spacing_combining_mark) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("XN--HELLO-TXK"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("XN--HELLO-TXK"));
}

TEST(xn_uppercase_leading_nonspacing_mark) {
  EXPECT_FALSE(sourcemeta::core::is_hostname("XN--HELLO-ZED"));
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("XN--HELLO-ZED"));
}
