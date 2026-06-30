#include <sourcemeta/core/test.h>

#include <sourcemeta/core/dns.h>

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
