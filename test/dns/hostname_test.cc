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

// Total length cap. RFC 1123 §2.1: "Host software MUST handle host
// names of up to 63 characters and SHOULD handle host names of up to 255
// characters." The existing tests only reach the cap through four 63-byte
// labels, so these reach it through many short ones instead

TEST(valid_total_255_many_short_labels) {
  // 128 single-character labels and 127 dots is 255 bytes, right at the cap
  std::string hostname{"a"};
  for (int index = 0; index < 127; ++index) {
    hostname.append(".a");
  }
  EXPECT_EQ(hostname.size(), 255u);
  EXPECT_TRUE(sourcemeta::core::is_hostname(hostname));
}

TEST(invalid_total_256_many_short_labels) {
  // the same shape one byte over the cap, with every label still well formed
  std::string hostname{"aa"};
  for (int index = 0; index < 127; ++index) {
    hostname.append(".a");
  }
  EXPECT_EQ(hostname.size(), 256u);
  EXPECT_FALSE(sourcemeta::core::is_hostname(hostname));
}

TEST(invalid_total_far_over_cap) {
  // the total-length check runs before any label scan, so an input far past
  // the cap is rejected without walking it
  EXPECT_FALSE(sourcemeta::core::is_hostname(std::string(100000, 'a')));
}

// Per-label cap in non-leading positions. RFC 1035 §2.3.4 caps a label at 63
// octets, and the existing tests only exercise the first label

TEST(valid_middle_label_63) {
  // 63-character label between two other labels
  EXPECT_TRUE(
      sourcemeta::core::is_hostname("a." + std::string(63, 'a') + ".b"));
}

TEST(valid_last_label_63) {
  // 63-character label in last position
  EXPECT_TRUE(sourcemeta::core::is_hostname("a." + std::string(63, 'a')));
}

TEST(invalid_middle_label_64) {
  // 64-character label between two other labels
  EXPECT_FALSE(
      sourcemeta::core::is_hostname("a." + std::string(64, 'a') + ".b"));
}

TEST(invalid_last_label_64) {
  // 64-character label in last position
  EXPECT_FALSE(sourcemeta::core::is_hostname("a." + std::string(64, 'a')));
}

// Hyphen placement. RFC 1123 §2.1 relaxes the first character to a letter or
// a digit, and RFC 952 ASSUMPTIONS says "The last character must not be a
// minus sign or period"

TEST(invalid_hyphen_only) {
  // a lone hyphen has no leading let-dig
  EXPECT_FALSE(sourcemeta::core::is_hostname("-"));
}

TEST(invalid_double_hyphen_only) {
  // two hyphens still have no leading let-dig
  EXPECT_FALSE(sourcemeta::core::is_hostname("--"));
}

TEST(invalid_minimal_trailing_hyphen) {
  // shortest label ending in a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("a-"));
}

TEST(invalid_minimal_leading_hyphen) {
  // shortest label starting with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("-a"));
}

TEST(invalid_first_of_two_leading_hyphen) {
  // first label of two starts with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("-a.b"));
}

TEST(invalid_first_of_two_trailing_hyphen) {
  // first label of two ends with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("a-.b"));
}

TEST(invalid_last_label_leading_hyphen) {
  // last label starts with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.-b"));
}

TEST(invalid_last_label_trailing_hyphen) {
  // last label ends with a hyphen
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.b-"));
}

TEST(invalid_hyphen_label_first) {
  // a whole label that is just a hyphen, first
  EXPECT_FALSE(sourcemeta::core::is_hostname("-.a"));
}

TEST(invalid_hyphen_label_last) {
  // a whole label that is just a hyphen, last
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.-"));
}

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

// Dot structure. RFC 952 §B: <hname> ::= <name>*["."<name>], so every dot
// must sit between two non-empty labels

TEST(invalid_two_dots_only) {
  // two dots with no label anywhere
  EXPECT_FALSE(sourcemeta::core::is_hostname(".."));
}

TEST(invalid_three_dots_only) {
  // three dots with no label anywhere
  EXPECT_FALSE(sourcemeta::core::is_hostname("..."));
}

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

TEST(valid_twenty_labels) {
  // twenty single-character labels
  EXPECT_TRUE(
      sourcemeta::core::is_hostname("a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p.q.r.s.t"));
}

// The "xn--" A-label branch. RFC 5890 §2.3.2.1 makes the ACE prefix
// case-insensitive and calls a label that carries the prefix without being a
// real Punycode encoding a "Fake A-label"

TEST(xn_prefix_only) {
  // the bare ACE prefix: the trailing-hyphen rule rejects it before the
  // A-label check runs
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--"));
}

TEST(xn_prefix_only_uppercase) {
  // the same, in uppercase
  EXPECT_FALSE(sourcemeta::core::is_hostname("XN--"));
}

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

TEST(xn_body_single_letter) {
  // a one-character Punycode body that does not decode to a U-label
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--a"));
}

TEST(xn_prefix_not_at_label_start) {
  // the ACE prefix only counts at the start of a label
  EXPECT_TRUE(sourcemeta::core::is_hostname("axn--1234"));
}

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

TEST(xn_single_label_valid) {
  // a valid A-label on its own
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--4gbwdl"));
}

TEST(xn_valid_second_label) {
  // a valid A-label after an ASCII label
  EXPECT_TRUE(sourcemeta::core::is_hostname("example.xn--wgbh1c"));
}

TEST(xn_invalid_second_label) {
  // the A-label check also runs on non-leading labels
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.xn--X"));
}

TEST(xn_valid_russian_tld) {
  // the A-label for the Cyrillic .rf top-level domain
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--p1ai"));
}

TEST(xn_valid_two_a_labels_and_ascii) {
  // A-labels around an ASCII label
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--p1ai.example.xn--p1ai"));
}

TEST(xn_body_trailing_hyphen) {
  // an ACE-prefixed label ending in a hyphen, rejected by the LDH rule first
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--a-"));
}

TEST(xn_prefix_only_middle_label) {
  // the bare ACE prefix as a middle label
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.xn--.b"));
}

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

TEST(xn_valid_a_label_then_ascii) {
  // a valid A-label followed by an ASCII label
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--p1ai.example"));
}

// Line terminators and surrounding whitespace. RFC 952 ASSUMPTIONS: "No blank
// or space characters are permitted as part of a name"

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

TEST(invalid_leading_newline) {
  // a leading newline
  EXPECT_FALSE(sourcemeta::core::is_hostname("\nexample.com"));
}

TEST(invalid_newline_only) {
  // a lone newline
  EXPECT_FALSE(sourcemeta::core::is_hostname("\n"));
}

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

TEST(invalid_interior_newline) {
  // a newline where the label separator would go
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\ncom"));
}

// Embedded NUL. Each of these is passed with an explicit length, so the
// bytes after the NUL are part of the input rather than being cut off

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

// The label alphabet, swept over every byte value. RFC 952 §B builds
// <name> out of <let>, <let-or-digit> and <let-or-digit-or-hyphen>, and
// RFC 1123 §2.1 changes only the first-character rule, so the accepted set is
// closed and can be checked exhaustively rather than sampled

TEST(byte_alphabet_interior_position) {
  for (int value = 0; value < 256; ++value) {
    const auto character{static_cast<char>(value)};
    const bool is_let_dig{(character >= 'a' && character <= 'z') ||
                          (character >= 'A' && character <= 'Z') ||
                          (character >= '0' && character <= '9')};
    std::string candidate{"a"};
    candidate.push_back(character);
    candidate.append("b");
    // an interior hyphen is let-dig-hyp and an interior dot is the label
    // separator, so both keep the name well formed
    if (is_let_dig || character == '-' || character == '.') {
      EXPECT_TRUE(sourcemeta::core::is_hostname(candidate));
    } else {
      EXPECT_FALSE(sourcemeta::core::is_hostname(candidate));
    }
  }
}

// RFC 1123 §2.1: the first character of a label must be a letter or a digit,
// which rules out the hyphen and the dot that the interior position allows
TEST(byte_alphabet_leading_position) {
  for (int value = 0; value < 256; ++value) {
    const auto character{static_cast<char>(value)};
    const bool is_let_dig{(character >= 'a' && character <= 'z') ||
                          (character >= 'A' && character <= 'Z') ||
                          (character >= '0' && character <= '9')};
    std::string candidate;
    candidate.push_back(character);
    candidate.append("ab");
    if (is_let_dig) {
      EXPECT_TRUE(sourcemeta::core::is_hostname(candidate));
    } else {
      EXPECT_FALSE(sourcemeta::core::is_hostname(candidate));
    }
  }
}

// RFC 952 §B: <name> ends in <let-or-digit>, so a trailing hyphen is out, and
// a trailing dot is not part of the host name grammar either
TEST(byte_alphabet_trailing_position) {
  for (int value = 0; value < 256; ++value) {
    const auto character{static_cast<char>(value)};
    const bool is_let_dig{(character >= 'a' && character <= 'z') ||
                          (character >= 'A' && character <= 'Z') ||
                          (character >= '0' && character <= '9')};
    std::string candidate{"ab"};
    candidate.push_back(character);
    if (is_let_dig) {
      EXPECT_TRUE(sourcemeta::core::is_hostname(candidate));
    } else {
      EXPECT_FALSE(sourcemeta::core::is_hostname(candidate));
    }
  }
}

// Non-ASCII bytes. RFC 952 ASSUMPTIONS draws the alphabet from "(A-Z), digits
// (0-9), minus sign (-), and period (.)", so nothing above 0x7F belongs in a
// host name, however much a codepoint looks or case-folds like ASCII

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

TEST(invalid_latin_small_e_acute) {
  // U+00E9, a plain accented Latin letter
  EXPECT_FALSE(sourcemeta::core::is_hostname("caf\xc3\xa9.test"));
}

TEST(invalid_non_breaking_space) {
  // U+00A0 NO-BREAK SPACE
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xc2\xa0"
                                             "b"));
}

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

TEST(invalid_zero_width_joiner) {
  // U+200D ZERO WIDTH JOINER
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xe2\x80\x8d"
                                             "b"));
}

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

// Additional valid shapes

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

TEST(valid_digit_first_every_label) {
  // RFC 1123 §2.1 relaxes the first character to let-dig in every label
  EXPECT_TRUE(sourcemeta::core::is_hostname("1a.2b.3c"));
}

TEST(valid_all_digit_label_63) {
  // 63 digits in a single label
  EXPECT_TRUE(sourcemeta::core::is_hostname(std::string(63, '1')));
}

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
