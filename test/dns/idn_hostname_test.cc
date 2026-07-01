#include <sourcemeta/core/test.h>

#include <sourcemeta/core/dns.h>

#include <string>

// example.test rendered in Hangul (RFC 5890 §2.3.2.3)
// Bytes: 실=EC8BA4 례=EBA180 .=2E 테=ED858C 스=EC8AA4 트=ED8AB8
TEST(valid_hangul_example_test) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xec\x8b\xa4\xeb\xa1\x80"
      ".\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
  EXPECT_FALSE(
      sourcemeta::core::is_hostname("\xec\x8b\xa4\xeb\xa1\x80"
                                    ".\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
}

// RFC 1123 §2.1: ASCII single label is a subset of the extended grammar
TEST(valid_ascii_single_label) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("hostname"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("hostname"));
}

TEST(valid_ascii_dotted) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("www.example.com"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("www.example.com"));
}

TEST(valid_ascii_with_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("host-name"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("host-name"));
}

TEST(valid_ascii_with_digits) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("h0stn4me"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("h0stn4me"));
}

TEST(valid_ascii_leading_digit) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("1host"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("1host"));
}

TEST(valid_ascii_trailing_digit) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("hostnam3"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("hostnam3"));
}

// RFC 5890 §2.3.2.1: A-label form xn--... (purely ASCII LDH)
TEST(valid_ascii_a_label_form) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_hostname("xn--ihqwcrb4cv8a8dqg056pqjye"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--ihqwcrb4cv8a8dqg056pqjye"));
}

// RFC 5890 §2.3.2.3 / RFC 6532 §3.1: U-label (2-byte UTF-8: U+03B1)
TEST(valid_label_two_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xce\xb1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1"));
}

// RFC 5890 §2.3.2.3 / RFC 6532 §3.1: U-label (3-byte UTF-8: U+4E2D 中)
TEST(valid_label_three_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xe4\xb8\xad"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe4\xb8\xad"));
}

// RFC 5890 §2.3.2.3 / RFC 6532 §3.1: U-label with a 4-byte UTF-8 codepoint
// (U+20000 CJK UNIFIED IDEOGRAPH-20000, PVALID per RFC 5892)
TEST(valid_label_four_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xf0\xa0\x80\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf0\xa0\x80\x80"));
}

TEST(valid_mixed_ascii_and_utf8_labels) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("example.\xce\xb1.com"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.\xce\xb1.com"));
}

TEST(valid_utf8_label_with_internal_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xce\xb1-\xe4\xb8\xad"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1-\xe4\xb8\xad"));
}

// U+03B1, U+4E2D, U+20000 are all PVALID per RFC 5892
TEST(valid_utf8_only_dotted) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xce\xb1.\xe4\xb8\xad.\xf0\xa0\x80\x80"));
  EXPECT_FALSE(
      sourcemeta::core::is_hostname("\xce\xb1.\xe4\xb8\xad.\xf0\xa0\x80\x80"));
}

// RFC 1035 §2.3.4: label may be exactly 63 octets
TEST(valid_ascii_label_at_63) {
  const std::string label(63, 'a');
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(label));
  EXPECT_TRUE(sourcemeta::core::is_hostname(label));
}

// RFC 5891 §4.2.4: per-label A-label form must be ≤ 63 octets. A U-label of
// 55 Greek codepoints Punycode-encodes to a 61-character body, giving an
// A-label of 65 octets even though the codepoint count is well under 63.
TEST(invalid_u_label_a_form_over_63) {
  // "παράδειγμα" (10 codepoints) × 5 + "παράδ" (5 codepoints) = 55
  std::string label;
  for (int index = 0; index < 5; ++index) {
    label.append("\xcf\x80\xce\xb1\xcf\x81\xce\xac\xce\xb4"
                 "\xce\xb5\xce\xb9\xce\xb3\xce\xbc\xce\xb1");
  }
  label.append("\xcf\x80\xce\xb1\xcf\x81\xce\xac\xce\xb4");
  EXPECT_EQ(label.size(), 110u);
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(label));
  EXPECT_FALSE(sourcemeta::core::is_hostname(label));
}

// RFC 5891 §4.2.4: 52 Greek codepoints Punycode-encode to a 57-character
// body, giving a 61-octet A-label that just fits the per-label cap
TEST(valid_u_label_a_form_at_61) {
  // "παράδειγμα" × 5 + "πα" = 52 codepoints
  std::string label;
  for (int index = 0; index < 5; ++index) {
    label.append("\xcf\x80\xce\xb1\xcf\x81\xce\xac\xce\xb4"
                 "\xce\xb5\xce\xb9\xce\xb3\xce\xbc\xce\xb1");
  }
  label.append("\xcf\x80\xce\xb1");
  EXPECT_EQ(label.size(), 104u);
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(label));
  EXPECT_FALSE(sourcemeta::core::is_hostname(label));
}

// RFC 1035 §3.1: A-label-form presentation total must be ≤ 253 octets. 32
// single-α labels have UTF-8 input of only 95 bytes (well below the 255
// RFC 1123 hostname cap) but the A-label total is 32 × 7 + 31 = 255 octets
TEST(invalid_a_label_total_over_253_short_utf8) {
  std::string hostname{"\xce\xb1"};
  for (int index = 0; index < 31; ++index) {
    hostname.append(".\xce\xb1");
  }
  EXPECT_EQ(hostname.size(), 95u);
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(hostname));
  EXPECT_FALSE(sourcemeta::core::is_hostname(hostname));
}

// 31 single-α labels: A-label total = 31 × 7 + 30 = 247 ≤ 253
TEST(valid_a_label_total_at_247_short_utf8) {
  std::string hostname{"\xce\xb1"};
  for (int index = 0; index < 30; ++index) {
    hostname.append(".\xce\xb1");
  }
  EXPECT_EQ(hostname.size(), 92u);
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(hostname));
  EXPECT_FALSE(sourcemeta::core::is_hostname(hostname));
}

// RFC 1035 §3.1: presentation form may be exactly 253 octets for IDN.
// RFC 1123 §2.1 is more permissive and accepts up to 255 characters
TEST(total_length_boundary_253_vs_255) {
  // 4 × 63 + 3 dots = 255 (RFC 1123) but not RFC 1035 presentation
  std::string at_255;
  for (int index = 0; index < 3; ++index) {
    at_255.append(63, 'a');
    at_255.push_back('.');
  }
  at_255.append(63, 'a');
  EXPECT_EQ(at_255.size(), 255u);
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(at_255));
  EXPECT_TRUE(sourcemeta::core::is_hostname(at_255));

  // 4 × 63 + 3 dots - 2 = 253: drop two octets from the last label
  std::string at_253;
  for (int index = 0; index < 3; ++index) {
    at_253.append(63, 'a');
    at_253.push_back('.');
  }
  at_253.append(61, 'a');
  EXPECT_EQ(at_253.size(), 253u);
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(at_253));
  EXPECT_TRUE(sourcemeta::core::is_hostname(at_253));
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(""));
  EXPECT_FALSE(sourcemeta::core::is_hostname(""));
}

// RFC 1123 §2.1: a label may not begin with a hyphen
TEST(invalid_leading_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("-hello"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("-hello"));
}

// RFC 952 §B: a label may not end with a hyphen
TEST(invalid_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("hello-"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("hello-"));
}

TEST(invalid_leading_and_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("-hello-"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("-hello-"));
}

TEST(invalid_utf8_label_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xce\xb1-"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1-"));
}

TEST(invalid_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(".example"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(".example"));
}

TEST(invalid_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("example."));
  EXPECT_FALSE(sourcemeta::core::is_hostname("example."));
}

TEST(invalid_consecutive_dots) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a..b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a..b"));
}

// Space and "$" are neither ASCII LDH nor UTF-8 lead bytes
TEST(invalid_dollar_amount_with_arrows) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("-> $1.00 <--"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("-> $1.00 <--"));
}

TEST(invalid_internal_space) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a b"));
}

TEST(invalid_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a_b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a_b"));
}

// RFC 1035 §2.3.4: 64-octet label exceeds the per-label cap
TEST(invalid_ascii_label_at_64) {
  const std::string label(64, 'a');
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(label));
  EXPECT_FALSE(sourcemeta::core::is_hostname(label));
}

// RFC 5321 §4.5.3.1.2: 256-octet total exceeds the cap. Construction avoids
// trailing-dot and per-label confounds (see idn_email_test.cc rationale)
TEST(invalid_total_at_256) {
  std::string hostname;
  for (int index = 0; index < 4; ++index) {
    hostname.append(51, 'a');
    hostname.push_back('.');
  }
  hostname.append(48, 'a');
  EXPECT_EQ(hostname.size(), 256u);
  EXPECT_NE(hostname.back(), '.');
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(hostname));
  EXPECT_FALSE(sourcemeta::core::is_hostname(hostname));
}

// RFC 6532 §3.1: lone continuation byte (0xBF) cannot start a UTF-8 sequence
TEST(invalid_lone_continuation_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xbf"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xbf"));
}

// RFC 6532 §3.1: 2-byte starter without a continuation byte
TEST(invalid_truncated_two_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xce"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce"));
}

// RFC 6532 §3.1: %xE0 %x80-9F is an overlong 3-byte encoding
TEST(invalid_overlong_three_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xe0\x80\xa0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe0\x80\xa0"));
}

// RFC 6532 §3.1: U+D800 surrogate codepoint is forbidden
TEST(invalid_surrogate_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xed\xa0\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xed\xa0\x80"));
}

// RFC 6532 §3.1: codepoints above U+10FFFF are forbidden
TEST(invalid_above_max_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xf4\x90\x80\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf4\x90\x80\x80"));
}

// RFC 6532 §3.1: 4-byte starter with truncated continuation
TEST(invalid_truncated_four_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xf0\x9f\x98"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf0\x9f\x98"));
}

// RFC 6532 §3.1: %xC0 is a forbidden lead byte (overlong U+0000)
TEST(invalid_overlong_c0) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xc0\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc0\x80"));
}

// RFC 6532 §3.1: %xF5 is not a valid lead byte
TEST(invalid_lead_f5) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xf5\x80\x80\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf5\x80\x80\x80"));
}

TEST(invalid_invalid_utf8_in_middle_label) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a.\xc0\x80.b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.\xc0\x80.b"));
}

// RFC 5890 §2.3.2.3: 실례.테스트 ("example.test" in Hangul)
TEST(valid_hangul_example_test_full) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
}

// RFC 5891 §4.2.3.2: first character must not be a combining mark.
// U+302E (HANGUL SINGLE DOT TONE MARK) is a Spacing Combining Mark
TEST(invalid_first_char_combining_mark_u302e) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\xe3\x80\xae\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a"
      "\xb8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "\xe3\x80\xae\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a"
      "\xb8"));
}

// RFC 5892 Table A: U+302E is DISALLOWED, so it cannot appear anywhere
TEST(invalid_disallowed_u302e_middle) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\xec\x8b\xa4\xe3\x80\xae\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a"
      "\xb8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "\xec\x8b\xa4\xe3\x80\xae\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a"
      "\xb8"));
}

// RFC 5891 §4.2.3.2: first character is a Nonspacing Mark (U+0300)
TEST(invalid_first_char_nonspacing_mark) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xcc\x80hello"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xcc\x80hello"));
}

// RFC 5891 §4.2.3.2: first character is a Spacing Combining Mark (U+0903)
TEST(invalid_first_char_spacing_combining_mark) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xe0\xa4\x83hello"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe0\xa4\x83hello"));
}

// RFC 5891 §4.2.3.2: first character is an Enclosing Mark (U+0488)
TEST(invalid_first_char_enclosing_mark) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xd2\x88hello"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd2\x88hello"));
}

// RFC 3492 §6.2: 'xn--X' has no Punycode body that decodes meaningfully,
// and RFC 5890 §2.3.2.1 routes the ACE prefix through A-label validation
TEST(invalid_punycode_xn_dash_dash_X) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("xn--X"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("xn--X"));
}

// RFC 5892 §2.6: PVALID exceptions ßς་〇 (LTR)
TEST(valid_pvalid_exceptions_ltr) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xc3\x9f\xcf\x82\xe0\xbc\x8b\xe3\x80\x87"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "\xc3\x9f\xcf\x82\xe0\xbc\x8b\xe3\x80\x87"));
}

// RFC 5892 §2.6: PVALID exceptions ۽۾ (RTL, Arabic signs)
TEST(valid_pvalid_exceptions_rtl) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xdb\xbd\xdb\xbe"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xdb\xbd\xdb\xbe"));
}

// RFC 5892 §2.6: ـ (U+0640 TATWEEL) and ߺ (U+07FA) are DISALLOWED
TEST(invalid_disallowed_exceptions_rtl) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xd9\x80\xdf\xba"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd9\x80\xdf\xba"));
}

// RFC 5892 §2.6: 〱〲〳〴〵〮〯〻 are all DISALLOWED
TEST(invalid_disallowed_exceptions_ltr) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\xe3\x80\xb1\xe3\x80\xb2\xe3\x80\xb3\xe3\x80\xb4\xe3\x80\xb5\xe3\x80"
      "\xae\xe3\x80\xaf\xe3\x80\xbb"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "\xe3\x80\xb1\xe3\x80\xb2\xe3\x80\xb3\xe3\x80\xb4\xe3\x80\xb5\xe3\x80"
      "\xae\xe3\x80\xaf\xe3\x80\xbb"));
}

// RFC 5892 Appendix A.3: MIDDLE DOT requires surrounding 'l' codepoints
TEST(valid_middle_dot_between_l) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("l\xc2\xb7l"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("l\xc2\xb7l"));
}

TEST(invalid_middle_dot_no_preceding_l) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a\xc2\xb7l"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xc2\xb7l"));
}

TEST(invalid_middle_dot_no_following_l) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("l\xc2\xb7"
                                                 "a"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("l\xc2\xb7"
                                             "a"));
}

TEST(invalid_middle_dot_leading) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xc2\xb7l"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc2\xb7l"));
}

TEST(invalid_middle_dot_trailing) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("l\xc2\xb7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("l\xc2\xb7"));
}

// RFC 5892 Appendix A.4: Greek KERAIA must be followed by a Greek codepoint
TEST(valid_keraia_followed_by_greek) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xce\xb1\xcd\xb5\xce\xb2"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1\xcd\xb5\xce\xb2"));
}

TEST(invalid_keraia_followed_by_latin) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xce\xb1\xcd\xb5S"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1\xcd\xb5S"));
}

TEST(invalid_keraia_trailing) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xce\xb1\xcd\xb5"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1\xcd\xb5"));
}

// RFC 5892 Appendix A.5: Hebrew GERESH must be preceded by a Hebrew codepoint
TEST(valid_geresh_preceded_by_hebrew) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xd7\x90\xd7\xb3\xd7\x91"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd7\x90\xd7\xb3\xd7\x91"));
}

TEST(invalid_geresh_preceded_by_latin) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("A\xd7\xb3\xd7\x91"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("A\xd7\xb3\xd7\x91"));
}

TEST(invalid_geresh_leading) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xd7\xb3\xd7\x91"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd7\xb3\xd7\x91"));
}

// RFC 5892 Appendix A.6: Hebrew GERSHAYIM must be preceded by Hebrew
TEST(valid_gershayim_preceded_by_hebrew) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xd7\x90\xd7\xb4\xd7\x91"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd7\x90\xd7\xb4\xd7\x91"));
}

TEST(invalid_gershayim_preceded_by_latin) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("A\xd7\xb4\xd7\x91"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("A\xd7\xb4\xd7\x91"));
}

// RFC 5892 Appendix A.7: KATAKANA MIDDLE DOT requires Hiragana/Katakana/Han
TEST(valid_katakana_middle_dot_with_hiragana) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xe3\x83\xbb\xe3\x81\x81"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe3\x83\xbb\xe3\x81\x81"));
}

TEST(valid_katakana_middle_dot_with_katakana) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xe3\x83\xbb\xe3\x82\xa1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe3\x83\xbb\xe3\x82\xa1"));
}

TEST(valid_katakana_middle_dot_with_han) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xe3\x83\xbb\xe4\xb8\x88"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe3\x83\xbb\xe4\xb8\x88"));
}

TEST(invalid_katakana_middle_dot_without_japanese) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("def\xe3\x83\xbb"
                                                 "abc"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("def\xe3\x83\xbb"
                                             "abc"));
}

// RFC 5892 Appendix A.8/A.9: Arabic-Indic and Extended Arabic-Indic digits
// must not mix within a label
TEST(valid_arabic_indic_digits_only) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xd8\xa8\xd9\xa0\xd8\xa8"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd8\xa8\xd9\xa0\xd8\xa8"));
}

TEST(invalid_mixed_arabic_indic_digits) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xd8\xa8\xd9\xa0\xdb\xb0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xd8\xa8\xd9\xa0\xdb\xb0"));
}

// RFC 5892 Appendix A.2: ZERO WIDTH JOINER must be preceded by Virama
TEST(valid_zwj_after_virama) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xe0\xa4\x95\xe0\xa5\x8d\xe2\x80\x8d\xe0\xa4\xb7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "\xe0\xa4\x95\xe0\xa5\x8d\xe2\x80\x8d\xe0\xa4\xb7"));
}

TEST(invalid_zwj_not_after_virama) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\xe0\xa4\x95\xe2\x80\x8d\xe0\xa4\xb7"));
  EXPECT_FALSE(
      sourcemeta::core::is_hostname("\xe0\xa4\x95\xe2\x80\x8d\xe0\xa4\xb7"));
}

// RFC 5892 Appendix A.1: ZERO WIDTH NON-JOINER requires Virama or proper
// joining-context match
TEST(valid_zwnj_after_virama) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xe0\xa4\x95\xe0\xa5\x8d\xe2\x80\x8c\xe0\xa4\xb7"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "\xe0\xa4\x95\xe0\xa5\x8d\xe2\x80\x8c\xe0\xa4\xb7"));
}

TEST(valid_zwnj_between_arabic_joiners) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xd8\xa8\xd9\x8a\xe2\x80\x8c\xd8\xa8\xd9\x8a"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(
      "\xd8\xa8\xd9\x8a\xe2\x80\x8c\xd8\xa8\xd9\x8a"));
}

// UTS #46 §3.1: alternative label separators are recognised
TEST(valid_ideographic_full_stop_separator) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("a\xe3\x80\x82"
                                                "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xe3\x80\x82"
                                             "b"));
}

TEST(valid_fullwidth_full_stop_separator) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("a\xef\xbc\x8e"
                                                "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xef\xbc\x8e"
                                             "b"));
}

TEST(valid_halfwidth_ideographic_full_stop_separator) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("a\xef\xbd\xa1"
                                                "b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xef\xbd\xa1"
                                             "b"));
}

TEST(invalid_leading_ideographic_full_stop) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xe3\x80\x82"
                                                 "example"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe3\x80\x82"
                                             "example"));
}

TEST(invalid_trailing_ideographic_full_stop) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("example\xe3\x80\x82"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("example\xe3\x80\x82"));
}

TEST(invalid_single_ideographic_full_stop) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xe3\x80\x82"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe3\x80\x82"));
}

// RFC 5891 §4.1.2.A: a U-label must be in Unicode Normalisation Form C.
// Decomposed `a` + combining grave (both PVALID) is rejected; the
// precomposed `\u00E0` form is accepted
TEST(decomposed_label_rejected_not_nfc) {
  // UTF-8 of "a\u0300": "a" then \xCC\x80
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a\xcc\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xcc\x80"));
}

TEST(precomposed_label_accepted) {
  // UTF-8 of U+00E0: 0xC3 0xA0
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xc3\xa0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc3\xa0"));
}

// Out-of-order combining marks fail the NFC check inside the label
// validator, so the whole hostname is rejected
TEST(label_with_out_of_order_combining_marks_rejected) {
  // a + U+0301 + U+0316: "a" 0xCC 0x81 0xCC 0x96
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a\xcc\x81\xcc\x96"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a\xcc\x81\xcc\x96"));
}

// RFC 5893 sec 2: a digit-first label is invalid in a Bidi domain name
// (whole-name rule)
TEST(invalid_bidi_digit_first_in_bidi_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\x30\x61\x2e\xd7\x90"));
}

// RFC 5893 sec 2 cond 1: label must start with L, R or AL
TEST(invalid_bidi_single_label_digit_then_rtl) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\x30\xd8\xa7"));
}

// RFC 5893 sec 2 cond 5: an LTR label may not contain an RTL letter
TEST(invalid_bidi_ltr_label_with_rtl_letter) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\x61\xd7\x90"));
}

// RFC 5890 sec 2.3.2.1 + 5892 sec 2.6: decodes to U+00A1 (DISALLOWED)
TEST(invalid_a_label_decodes_to_disallowed) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\x78\x6e\x2d\x2d\x37\x61"));
}

// RFC 5890 sec 2.3.2.1: A-label decodes to the all-ASCII label example
TEST(invalid_a_label_decodes_to_ascii_only) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\x78\x6e\x2d\x2d\x65\x78\x61\x6d\x70\x6c\x65\x2d"));
}

// RFC 5890 sec 2.3.2.1 + 5893: decodes to a-grave + Hebrew aleph
TEST(invalid_a_label_decodes_to_bidi_violation) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\x78\x6e\x2d\x2d\x30\x63\x61\x32\x34\x77"));
}

// RFC 5891 sec 5.4: non-canonical Punycode; canonical form is xn--9uc
TEST(invalid_noncanonical_punycode) {
  EXPECT_FALSE(
      sourcemeta::core::is_idn_hostname("\x78\x6e\x2d\x2d\x2d\x39\x75\x63"));
}

// RFC 5892 sec 2.6: fullwidth digits U+FF11..U+FF13 are DISALLOWED
TEST(invalid_fullwidth_digits) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\xef\xbc\x91\xef\xbc\x92\xef\xbc\x93"));
}

// RFC 5892 sec 2.6: U+200B (ZERO WIDTH SPACE) is DISALLOWED
TEST(invalid_zero_width_space) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\x61\xe2\x80\x8b\x62"));
}

// RFC 5892 appendix A.1 (errata 3312): the ZWNJ rule must hold at every
// occurrence
TEST(invalid_zwnj_failing_at_one_occurrence) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(
      "\xe0\xa4\x95\xe0\xa5\x8d\xe2\x80\x8c\xe0\xa4\xb7\x78\xe2\x80\x8c\x79"));
}
