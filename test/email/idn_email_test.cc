#include <sourcemeta/core/email.h>
#include <sourcemeta/core/test.h>

#include <string>

// example@example.test rendered in Hangul (RFC 6531 §3.3)
// Bytes: 실=EC8BA4 례=EBA180 @=40 실=EC8BA4 례=EBA180 .=2E 테=ED858C 스=EC8AA4
// 트=ED8AB8
TEST(valid_hangul_example_at_example_test) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email(
      "\xec\x8b\xa4\xeb\xa1\x80"
      "@"
      "\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
  EXPECT_FALSE(sourcemeta::core::is_email(
      "\xec\x8b\xa4\xeb\xa1\x80"
      "@"
      "\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
}

TEST(invalid_bare_number) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("2962"));
  EXPECT_FALSE(sourcemeta::core::is_email("2962"));
}

TEST(valid_typical_ascii_address) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("joe.bloggs@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("joe.bloggs@example.com"));
}

// RFC 5321 §4.1.2: ASCII Dot-string is a subset of the extended grammar
TEST(valid_ascii_single_atom) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a@b"));
  EXPECT_TRUE(sourcemeta::core::is_email("a@b"));
}

TEST(valid_ascii_two_atoms) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a.b@c"));
  EXPECT_TRUE(sourcemeta::core::is_email("a.b@c"));
}

TEST(valid_ascii_many_atoms) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a.b.c.d@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("a.b.c.d@example.com"));
}

TEST(valid_ascii_atext_symbols) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_email("a!#$%&'*+-/=?^_`{|}~@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("a!#$%&'*+-/=?^_`{|}~@example.com"));
}

TEST(valid_ascii_uppercase_local) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("ABC@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("ABC@example.com"));
}

TEST(valid_ascii_digit_local) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("123@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("123@example.com"));
}

// RFC 6531 §3.3: atext =/ UTF8-non-ascii (2-byte: U+03B1 GREEK SMALL ALPHA)
TEST(valid_local_two_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xce\xb1@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xce\xb1@b"));
}

// RFC 6531 §3.3: atext =/ UTF8-non-ascii (3-byte: U+4E2D CJK 中)
TEST(valid_local_three_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xe4\xb8\xad@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xe4\xb8\xad@b"));
}

// RFC 6531 §3.3: atext =/ UTF8-non-ascii (4-byte: U+1F600 GRINNING FACE)
TEST(valid_local_four_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xf0\x9f\x98\x80@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xf0\x9f\x98\x80@b"));
}

TEST(valid_local_mixed_ascii_and_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user.\xce\xb1@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("user.\xce\xb1@example.com"));
}

TEST(valid_local_multi_atom_with_utf8) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_email("\xe4\xb8\xad.\xce\xb1.user@example.com"));
  EXPECT_FALSE(
      sourcemeta::core::is_email("\xe4\xb8\xad.\xce\xb1.user@example.com"));
}

TEST(valid_local_utf8_only_two_atoms) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_email("\xce\xb1.\xe4\xb8\xad@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xce\xb1.\xe4\xb8\xad@example.com"));
}

// RFC 6531 §3.3: sub-domain =/ U-label (2-byte U-label only)
TEST(valid_domain_two_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a@\xce\xb1"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@\xce\xb1"));
}

// RFC 6531 §3.3: sub-domain =/ U-label (3-byte U-label only)
TEST(valid_domain_three_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a@\xe4\xb8\xad"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@\xe4\xb8\xad"));
}

TEST(valid_domain_mixed_labels) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a@example.\xce\xb1.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@example.\xce\xb1.com"));
}

TEST(valid_domain_utf8_with_hyphen) {
  // U-labels may contain hyphens; just not at the start/end of a label
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a@\xce\xb1-\xe4\xb8\xad"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@\xce\xb1-\xe4\xb8\xad"));
}

TEST(valid_domain_many_labels) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a@a.b.c.\xce\xb1.d.e"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@a.b.c.\xce\xb1.d.e"));
}

// RFC 5321 §4.1.2: Quoted-string with ASCII-only content
TEST(valid_quoted_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"a b\"@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("\"a b\"@example.com"));
}

// RFC 6531 §3.3: qtextSMTP =/ UTF8-non-ascii
TEST(valid_quoted_with_two_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"\xce\xb1\"@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"\xce\xb1\"@example.com"));
}

TEST(valid_quoted_with_three_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"\xe4\xb8\xad\"@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"\xe4\xb8\xad\"@example.com"));
}

TEST(valid_quoted_with_four_byte_utf8) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_email("\"\xf0\x9f\x98\x80\"@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"\xf0\x9f\x98\x80\"@example.com"));
}

TEST(valid_quoted_mixed_ascii_and_utf8) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_email("\"\xce\xb1 \xe4\xb8\xad\"@example.com"));
  EXPECT_FALSE(
      sourcemeta::core::is_email("\"\xce\xb1 \xe4\xb8\xad\"@example.com"));
}

TEST(valid_quoted_with_quoted_pair) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"a\\\"b\"@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("\"a\\\"b\"@example.com"));
}

// RFC 5321 §4.1.3: address-literal IPv4 stays ASCII (no IDNA applies)
TEST(valid_address_literal_ipv4) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[192.168.1.1]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[192.168.1.1]"));
}

TEST(valid_address_literal_ipv6) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[IPv6:::1]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[IPv6:::1]"));
}

TEST(valid_address_literal_ipv6_lowercase_tag) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[ipv6:::1]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[ipv6:::1]"));
}

TEST(valid_address_literal_with_utf8_local) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xce\xb1@[192.168.1.1]"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xce\xb1@[192.168.1.1]"));
}

// RFC 5321 §4.5.3.1.1: Local-part is allowed up to 64 octets
TEST(valid_local_at_octet_limit) {
  const std::string local(64, 'a');
  EXPECT_TRUE(sourcemeta::core::is_idn_email(local + "@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email(local + "@example.com"));
}

TEST(valid_local_at_octet_limit_with_utf8) {
  // 21 Greek alpha (CE B1 = 2 bytes each) = 42 bytes, plus 22 ASCII 'a' = 64
  std::string local;
  for (int index = 0; index < 21; ++index) {
    local.append("\xce\xb1");
  }
  local.append(22, 'a');
  EXPECT_EQ(local.size(), 64u);
  EXPECT_TRUE(sourcemeta::core::is_idn_email(local + "@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email(local + "@example.com"));
}

TEST(invalid_missing_at) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("plain"));
  EXPECT_FALSE(sourcemeta::core::is_email("plain"));
}

TEST(invalid_empty_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("@example.com"));
}

TEST(invalid_empty_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@"));
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email(""));
  EXPECT_FALSE(sourcemeta::core::is_email(""));
}

TEST(invalid_two_at_signs) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@b@c"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@b@c"));
}

TEST(invalid_local_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email(".user@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email(".user@example.com"));
}

TEST(invalid_local_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user.@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("user.@example.com"));
}

TEST(invalid_local_consecutive_dots) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a..b@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("a..b@example.com"));
}

TEST(invalid_local_just_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email(".@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email(".@example.com"));
}

// RFC 6532 §3.1: lone continuation byte 0xBF is not the start of UTF-8
TEST(invalid_lone_continuation_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\xbf@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xbf@b"));
}

// RFC 6532 §3.1: 2-byte starter with no continuation byte
TEST(invalid_truncated_two_byte_at_end_of_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\xce@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xce@b"));
}

// RFC 6532 §3.1: %xE0 %x80-9F is overlong (codepoints < U+0800)
TEST(invalid_overlong_three_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\xe0\x80\xa0@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xe0\x80\xa0@b"));
}

// RFC 6532 §3.1: U+D800 surrogate is forbidden
TEST(invalid_surrogate_codepoint_in_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\xed\xa0\x80@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xed\xa0\x80@b"));
}

// RFC 6532 §3.1: codepoints above U+10FFFF are forbidden
TEST(invalid_above_max_codepoint_in_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\xf4\x90\x80\x80@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xf4\x90\x80\x80@b"));
}

// RFC 6532 §3.1: 4-byte starter with truncated continuation
TEST(invalid_truncated_four_byte_in_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\xf0\x9f\x98@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xf0\x9f\x98@b"));
}

// RFC 6532 §3.1: %xC0 is a forbidden lead byte (overlong U+0000)
TEST(invalid_overlong_c0_in_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\xc0\x80@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xc0\x80@b"));
}

// RFC 6532 §3.1: %xF5 is not a valid lead byte
TEST(invalid_lead_f5_in_local) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\xf5\x80\x80\x80@b"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xf5\x80\x80\x80@b"));
}

TEST(invalid_invalid_utf8_in_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@\xc0\x80"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@\xc0\x80"));
}

TEST(invalid_surrogate_in_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@\xed\xa0\x80"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@\xed\xa0\x80"));
}

TEST(invalid_lone_continuation_in_domain) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@\xbf"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@\xbf"));
}

TEST(invalid_invalid_utf8_in_quoted) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\"\xc0\x80\"@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"\xc0\x80\"@example.com"));
}

TEST(invalid_truncated_utf8_in_quoted) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\"\xce\"@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"\xce\"@example.com"));
}

// RFC 6531 §3.3: domain label cannot start with a hyphen
TEST(invalid_domain_leading_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@-example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@-example.com"));
}

// RFC 6531 §3.3: domain label cannot end with a hyphen
TEST(invalid_domain_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@example-.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@example-.com"));
}

TEST(invalid_domain_label_trailing_hyphen_with_utf8) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@\xce\xb1-"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@\xce\xb1-"));
}

TEST(invalid_domain_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@example.com."));
  EXPECT_FALSE(sourcemeta::core::is_email("a@example.com."));
}

TEST(invalid_domain_empty_label_in_middle) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@example..com"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@example..com"));
}

TEST(invalid_domain_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@.example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("a@.example.com"));
}

// RFC 5321 §4.5.3.1.1: Local-part > 64 octets is invalid
TEST(invalid_local_one_over_octet_limit) {
  const std::string local(65, 'a');
  EXPECT_FALSE(sourcemeta::core::is_idn_email(local + "@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email(local + "@example.com"));
}

TEST(invalid_local_one_over_octet_limit_with_utf8) {
  // 21 alpha (42 bytes) + 23 'a' = 65 bytes
  std::string local;
  for (int index = 0; index < 21; ++index) {
    local.append("\xce\xb1");
  }
  local.append(23, 'a');
  EXPECT_EQ(local.size(), 65u);
  EXPECT_FALSE(sourcemeta::core::is_idn_email(local + "@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email(local + "@example.com"));
}

// RFC 1035 §2.3.4: single label > 63 octets is invalid
TEST(invalid_domain_label_too_long) {
  const std::string label(64, 'a');
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@" + label));
  EXPECT_FALSE(sourcemeta::core::is_email("a@" + label));
}

TEST(valid_domain_label_at_max_length) {
  const std::string label(63, 'a');
  EXPECT_TRUE(sourcemeta::core::is_idn_email("a@" + label));
  EXPECT_TRUE(sourcemeta::core::is_email("a@" + label));
}

// RFC 5321 §4.5.3.1.2: total domain > 255 octets is invalid.
// Construction must avoid trailing-dot and per-label (>63) confounds: 5
// labels of 51/51/51/51/48 'a' chars separated by 4 dots = 256 octets, no
// trailing dot, every label within the 63-octet RFC 1035 cap
TEST(invalid_domain_total_too_long) {
  std::string domain;
  for (int index = 0; index < 4; ++index) {
    domain.append(51, 'a');
    domain.push_back('.');
  }
  domain.append(48, 'a');
  EXPECT_EQ(domain.size(), 256u);
  EXPECT_NE(domain.back(), '.');
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a@" + domain));
  EXPECT_FALSE(sourcemeta::core::is_email("a@" + domain));
}

// RFC 6531 §3.3 extends the local-part with UTF8-non-ascii but, unlike the
// domain U-label (RFC 5890 §2.3.2.1), imposes no NFC requirement on it. So
// "cafe" + U+0301 COMBINING ACUTE (NFD, bytes 63 61 66 65 CC 81) is a valid
// local-part even though it is not in NFC.
TEST(valid_local_non_nfc) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("cafe\xcc\x81@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("cafe\xcc\x81@example.com"));
}

// RFC 5890 §2.3.2.1: a domain U-label MUST be in NFC. The same "cafe" +
// U+0301 (NFD) sequence that is valid in the local-part is invalid as a
// domain label.
TEST(invalid_domain_non_nfc) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@cafe\xcc\x81.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@cafe\xcc\x81.com"));
}

// RFC 5321 §4.1.2: SP is not atext and is only allowed inside a quoted
// string, so an unquoted space in the local-part is invalid.
TEST(invalid_local_unquoted_space) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("a b@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("a b@example.com"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP = %d92 %d32-126, so the octet after a
// backslash must be ASCII. A backslash followed by UTF8-non-ascii (U+03B1,
// bytes CE B1) is invalid even under RFC 6531.
TEST(invalid_quoted_pair_non_ascii) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\"\\\xce\xb1\"@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"\\\xce\xb1\"@example.com"));
}

// RFC 6531 §3.3 delegates the domain to IDNA (is_idn_hostname). Fullwidth
// digits U+FF11 U+FF12 U+FF13 (bytes EF BC 91 EF BC 92 EF BC 93) are
// DISALLOWED by RFC 5892 §2.6, so the domain - and the whole address - is
// invalid.
TEST(invalid_domain_idna_disallowed_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email(
      "user@\xef\xbc\x91\xef\xbc\x92\xef\xbc\x93.com"));
  EXPECT_FALSE(sourcemeta::core::is_email(
      "user@\xef\xbc\x91\xef\xbc\x92\xef\xbc\x93.com"));
}

// RFC 5321 §4.1.2: Quoted-string = DQUOTE *QcontentSMTP DQUOTE, so zero quoted
// content is permitted - an empty quoted string is a valid local-part.
TEST(valid_quoted_empty) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"\"@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("\"\"@example.com"));
}

// RFC 5321 §4.1.2: "@" (%d64) is qtextSMTP, so it is ordinary content inside a
// quoted string and does not start the domain - the address has one @
// separator.
TEST(valid_quoted_local_with_at) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"a@b\"@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("\"a@b\"@example.com"));
}

// RFC 5321 §4.1.2: "." (%d46) is qtextSMTP, so the Dot-string rules (no
// leading, trailing, or consecutive dots) do not apply inside a quoted string.
TEST(valid_quoted_local_consecutive_dots) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"a..b\"@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("\"a..b\"@example.com"));
}

// RFC 6531 §3.3: atext =/ UTF8-non-ascii puts no IDNA codepoint restriction on
// the local-part, so U+2665 BLACK HEART (bytes E2 99 A5) is a valid local atom
// even though it is DISALLOWED (RFC 5892 §2.6) in a domain U-label.
TEST(valid_local_symbol_codepoint) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xe2\x99\xa5@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xe2\x99\xa5@example.com"));
}

// RFC 6531 §3.3: the local-part has no Bidi constraint (unlike a domain
// U-label, RFC 5893), so a right-to-left character U+05D0 HEBREW ALEF (bytes D7
// 90) is a valid local atom.
TEST(valid_local_rtl_codepoint) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xd7\x90@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xd7\x90@example.com"));
}

// RFC 6531 §3.3: the local-part has no leading-combining-mark rule (unlike a
// domain U-label, RFC 5891 §4.2.3.2), so a lone U+0301 COMBINING ACUTE (bytes
// CC 81) is a valid local atom.
TEST(valid_local_combining_mark) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xcc\x81@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xcc\x81@example.com"));
}

// RFC 5893 §2: in a domain that has a right-to-left label, every label must
// satisfy the Bidi rule. The LTR label "0a" (starts with EN digit U+0030) in a
// name that also has the RTL label U+05D0 (bytes D7 90) violates it.
TEST(invalid_domain_bidi_rule) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@0a.\xd7\x90"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@0a.\xd7\x90"));
}

// RFC 5892 §2.6: U+200B ZERO WIDTH SPACE (bytes E2 80 8B) is DISALLOWED, so a
// domain label containing it is invalid (lenient processors silently strip it).
TEST(invalid_domain_zero_width) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@a\xe2\x80\x8b"
                                              "b.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@a\xe2\x80\x8b"
                                          "b.com"));
}

// RFC 5891 §4.2.3.2: a domain U-label MUST NOT begin with a combining mark. A
// leading U+0301 COMBINING ACUTE (bytes CC 81) is invalid in the domain - the
// mirror of valid_local_combining_mark, where the same mark is fine in the
// local.
TEST(invalid_domain_leading_combining_mark) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@\xcc\x81"
                                              "a.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@\xcc\x81"
                                          "a.com"));
}

// RFC 6531 §3.3: a quoted non-ASCII local-part with a U-label domain. Local
// U+03B1 GREEK ALPHA (CE B1) inside quotes; domain "m" + U+00FC (C3 BC) +
// "nchen.de".
TEST(valid_quoted_local_and_ulabel_domain) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"\xce\xb1\"@m\xc3\xbcnchen.de"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"\xce\xb1\"@m\xc3\xbcnchen.de"));
}

// RFC 6531 §3.3 + RFC 5890 §2.3.2.1: a U-label local-part (U+03B1 GREEK ALPHA,
// CE B1) combined with a valid A-label domain (xn--nxasmq6b).
TEST(valid_ulabel_local_and_alabel_domain) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xce\xb1@xn--nxasmq6b.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xce\xb1@xn--nxasmq6b.com"));
}

// RFC 5321 §4.1.3: Snum = 1*3DIGIT "representing a decimal integer value in
// the range 0 through 255". The rule constrains the VALUE, not the digit
// count, so leading zeros are inside the grammar. This is the documented
// difference from the RFC 3986 dec-octet that backs is_ipv4, which forbids
// them - so an implementation that reuses its ipv4 checker here is wrong.
TEST(valid_address_literal_snum_leading_zero) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[01.0.0.1]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[01.0.0.1]"));
}

TEST(valid_address_literal_snum_leading_zero_last_octet) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[0.0.0.01]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[0.0.0.01]"));
}

TEST(valid_address_literal_snum_leading_zeros_every_octet) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[001.002.003.004]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[001.002.003.004]"));
}

TEST(valid_address_literal_snum_padded_zero_octets) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[000.000.000.000]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[000.000.000.000]"));
}

TEST(valid_address_literal_snum_maximum) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[255.255.255.255]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[255.255.255.255]"));
}

// One over the range the comment in the ABNF gives
TEST(invalid_address_literal_snum_over_maximum) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[256.0.0.1]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[256.0.0.1]"));
}

// 1*3DIGIT caps the digit count at three even when the value fits
TEST(invalid_address_literal_snum_four_digits) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[0255.0.0.1]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[0255.0.0.1]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal = Snum 3("." Snum), so exactly four
TEST(invalid_address_literal_three_octets) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[1.2.3]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[1.2.3]"));
}

TEST(invalid_address_literal_five_octets) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[1.2.3.4.5]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[1.2.3.4.5]"));
}

TEST(invalid_address_literal_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[1.2.3.]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[1.2.3.]"));
}

TEST(invalid_address_literal_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[.1.2.3]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[.1.2.3]"));
}

TEST(invalid_address_literal_empty_octet) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[1..2.3]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[1..2.3]"));
}

// General-address-literal = Standardized-tag ":" 1*dcontent
TEST(valid_general_address_literal) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[unknown-tag:abc]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[unknown-tag:abc]"));
}

TEST(valid_general_address_literal_shortest) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[a:b]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[a:b]"));
}

// RFC 5321 §4.1.2: Ldh-str = *( ALPHA / DIGIT / "-" ) Let-dig constrains only
// the final character, so unlike sub-domain (Let-dig [Ldh-str]) a
// Standardized-tag may begin with a hyphen
TEST(valid_general_address_literal_leading_hyphen_tag) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[-tag:abc]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[-tag:abc]"));
}

TEST(invalid_general_address_literal_trailing_hyphen_tag) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[tag-:abc]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[tag-:abc]"));
}

// 1*dcontent requires at least one octet
TEST(invalid_general_address_literal_empty_content) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[tag:]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[tag:]"));
}

TEST(invalid_general_address_literal_empty_tag) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[:abc]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[:abc]"));
}

// RFC 5321 §4.1.3: dcontent = %d33-90 / %d94-126, which excludes SP, "[",
// "\" and "]"
TEST(invalid_general_address_literal_space_in_content) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[tag:ab c]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[tag:ab c]"));
}

TEST(invalid_general_address_literal_open_bracket_in_content) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[tag:ab[c]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[tag:ab[c]"));
}

// IPv6-comp permits an empty prefix and suffix around "::"
TEST(valid_address_literal_ipv6_fully_compressed) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[IPv6:::1]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[IPv6:::1]"));
}

// RFC 5234 §2.3: ABNF literal strings are case-insensitive, so the tag matches
// in any case. The lowercase form is already covered; this pins the uppercase.
TEST(valid_address_literal_ipv6_uppercase_tag) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("user@[IPV6:2001:db8::1]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[IPV6:2001:db8::1]"));
}

// IPv6v4-comp = [IPv6-hex *3(":" IPv6-hex)] "::" [...] IPv4-address-literal
TEST(valid_address_literal_ipv6v4_comp) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_email("user@[IPv6:2001:db8::192.0.2.1]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[IPv6:2001:db8::192.0.2.1]"));
}

// IPv6v4-full = IPv6-hex 5(":" IPv6-hex) ":" IPv4-address-literal
TEST(valid_address_literal_ipv6v4_full) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_email("user@[IPv6:1:2:3:4:5:6:1.2.3.4]"));
  EXPECT_TRUE(sourcemeta::core::is_email("user@[IPv6:1:2:3:4:5:6:1.2.3.4]"));
}

TEST(invalid_address_literal_ipv6_empty) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[IPv6:]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[IPv6:]"));
}

TEST(invalid_address_literal_empty) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[]"));
}

TEST(invalid_address_literal_unterminated) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[192.0.2.1"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[192.0.2.1"));
}

TEST(invalid_address_literal_doubled_brackets) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[[192.0.2.1]]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[[192.0.2.1]]"));
}

TEST(invalid_address_literal_followed_by_label) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[192.0.2.1].com"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[192.0.2.1].com"));
}

TEST(invalid_address_literal_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user@[192.0.2.1 ]"));
  EXPECT_FALSE(sourcemeta::core::is_email("user@[192.0.2.1 ]"));
}

// RFC 5321 §4.1.2: Mailbox splits on U+0040. A fullwidth (U+FF20, EF BC A0) or
// small (U+FE6B, EF B9 AB) commercial at is an ordinary character, so a string
// carrying one instead of "@" has no delimiter at all. Both codepoints map to
// U+0040 in the UTS #46 table, but that mapping applies to the domain the
// split produces, so it cannot manufacture the delimiter.
TEST(invalid_fullwidth_commercial_at) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user\xef\xbc\xa0"
                                              "example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("user\xef\xbc\xa0"
                                          "example.com"));
}

TEST(invalid_small_commercial_at) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("user\xef\xb9\xab"
                                              "example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("user\xef\xb9\xab"
                                          "example.com"));
}

// RFC 6531 §3.3 extends atext with UTF8-non-ascii and RFC 6532 §3.1 defines
// that as UTF8-2 / UTF8-3 / UTF8-4, with no codepoint-class filter. So a C1
// control, a noncharacter and a supplementary private-use codepoint are all
// inside the local-part grammar even though they are unusual.
// U+0085 NEXT LINE (C2 85)
TEST(valid_local_c1_control) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xc2\x85@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xc2\x85@example.com"));
}

// U+FFFE noncharacter (EF BF BE)
TEST(valid_local_noncharacter) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xef\xbf\xbe@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xef\xbf\xbe@example.com"));
}

// U+FDD0, the start of the noncharacter block (EF B7 90)
TEST(valid_local_noncharacter_block) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xef\xb7\x90@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xef\xb7\x90@example.com"));
}

// U+F0000, a supplementary private-use codepoint (F3 B0 80 80)
TEST(valid_local_supplementary_private_use) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\xf3\xb0\x80\x80@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\xf3\xb0\x80\x80@example.com"));
}

// RFC 5321 §4.1.2: Quoted-string = DQUOTE *QcontentSMTP DQUOTE, so the closing
// DQUOTE is mandatory
TEST(invalid_quoted_local_unterminated) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\"unterminated@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"unterminated@example.com"));
}

// Local-part = Dot-string / Quoted-string, so a quoted string cannot be
// followed by more local-part text
TEST(invalid_quoted_local_trailing_text) {
  EXPECT_FALSE(sourcemeta::core::is_idn_email("\"a\"b@example.com"));
  EXPECT_FALSE(sourcemeta::core::is_email("\"a\"b@example.com"));
}

// quoted-pairSMTP = %d92 %d32-126, so a backslash may escape the DQUOTE
TEST(valid_quoted_local_escaped_dquote) {
  EXPECT_TRUE(sourcemeta::core::is_idn_email("\"\\\"\"@example.com"));
  EXPECT_TRUE(sourcemeta::core::is_email("\"\\\"\"@example.com"));
}
