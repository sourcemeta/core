#include <gtest/gtest.h>

#include <sourcemeta/core/dns.h>

#include <string>

// example.test rendered in Hangul (RFC 5890 §2.3.2.3)
// Bytes: 실=EC8BA4 례=EBA180 .=2E 테=ED858C 스=EC8AA4 트=ED8AB8
TEST(DNS_idn_hostname, valid_hangul_example_test) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xec\x8b\xa4\xeb\xa1\x80"
      ".\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
  EXPECT_FALSE(
      sourcemeta::core::is_hostname("\xec\x8b\xa4\xeb\xa1\x80"
                                    ".\xed\x85\x8c\xec\x8a\xa4\xed\x8a\xb8"));
}

// RFC 1123 §2.1: ASCII single label is a subset of the extended grammar
TEST(DNS_idn_hostname, valid_ascii_single_label) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("hostname"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("hostname"));
}

TEST(DNS_idn_hostname, valid_ascii_dotted) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("www.example.com"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("www.example.com"));
}

TEST(DNS_idn_hostname, valid_ascii_with_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("host-name"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("host-name"));
}

TEST(DNS_idn_hostname, valid_ascii_with_digits) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("h0stn4me"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("h0stn4me"));
}

TEST(DNS_idn_hostname, valid_ascii_leading_digit) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("1host"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("1host"));
}

TEST(DNS_idn_hostname, valid_ascii_trailing_digit) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("hostnam3"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("hostnam3"));
}

// RFC 5890 §2.3.2.1: A-label form xn--... (purely ASCII LDH)
TEST(DNS_idn_hostname, valid_ascii_a_label_form) {
  EXPECT_TRUE(
      sourcemeta::core::is_idn_hostname("xn--ihqwcrb4cv8a8dqg056pqjye"));
  EXPECT_TRUE(sourcemeta::core::is_hostname("xn--ihqwcrb4cv8a8dqg056pqjye"));
}

// RFC 5890 §2.3.2.3 / RFC 6532 §3.1: U-label (2-byte UTF-8: U+03B1)
TEST(DNS_idn_hostname, valid_label_two_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xce\xb1"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1"));
}

// RFC 5890 §2.3.2.3 / RFC 6532 §3.1: U-label (3-byte UTF-8: U+4E2D 中)
TEST(DNS_idn_hostname, valid_label_three_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xe4\xb8\xad"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe4\xb8\xad"));
}

// RFC 5890 §2.3.2.3 / RFC 6532 §3.1: U-label (4-byte UTF-8: U+1F600)
TEST(DNS_idn_hostname, valid_label_four_byte_utf8) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xf0\x9f\x98\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf0\x9f\x98\x80"));
}

TEST(DNS_idn_hostname, valid_mixed_ascii_and_utf8_labels) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("example.\xce\xb1.com"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("example.\xce\xb1.com"));
}

TEST(DNS_idn_hostname, valid_utf8_label_with_internal_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname("\xce\xb1-\xe4\xb8\xad"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1-\xe4\xb8\xad"));
}

TEST(DNS_idn_hostname, valid_utf8_only_dotted) {
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(
      "\xce\xb1.\xe4\xb8\xad.\xf0\x9f\x98\x80"));
  EXPECT_FALSE(
      sourcemeta::core::is_hostname("\xce\xb1.\xe4\xb8\xad.\xf0\x9f\x98\x80"));
}

// RFC 1035 §2.3.4: label may be exactly 63 octets
TEST(DNS_idn_hostname, valid_ascii_label_at_63) {
  const std::string label(63, 'a');
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(label));
  EXPECT_TRUE(sourcemeta::core::is_hostname(label));
}

// RFC 5321 §4.5.3.1.2 / RFC 1035 §2.3.4: total length may be exactly 255
TEST(DNS_idn_hostname, valid_total_at_255) {
  // 4 × 63 + 3 dots = 255
  std::string hostname;
  for (int index = 0; index < 3; ++index) {
    hostname.append(63, 'a');
    hostname.push_back('.');
  }
  hostname.append(63, 'a');
  EXPECT_EQ(hostname.size(), 255u);
  EXPECT_TRUE(sourcemeta::core::is_idn_hostname(hostname));
  EXPECT_TRUE(sourcemeta::core::is_hostname(hostname));
}

TEST(DNS_idn_hostname, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(""));
  EXPECT_FALSE(sourcemeta::core::is_hostname(""));
}

// RFC 1123 §2.1: a label may not begin with a hyphen
TEST(DNS_idn_hostname, invalid_leading_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("-hello"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("-hello"));
}

// RFC 952 §B: a label may not end with a hyphen
TEST(DNS_idn_hostname, invalid_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("hello-"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("hello-"));
}

TEST(DNS_idn_hostname, invalid_leading_and_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("-hello-"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("-hello-"));
}

TEST(DNS_idn_hostname, invalid_utf8_label_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xce\xb1-"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce\xb1-"));
}

TEST(DNS_idn_hostname, invalid_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(".example"));
  EXPECT_FALSE(sourcemeta::core::is_hostname(".example"));
}

TEST(DNS_idn_hostname, invalid_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("example."));
  EXPECT_FALSE(sourcemeta::core::is_hostname("example."));
}

TEST(DNS_idn_hostname, invalid_consecutive_dots) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a..b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a..b"));
}

// Space and "$" are neither ASCII LDH nor UTF-8 lead bytes
TEST(DNS_idn_hostname, invalid_dollar_amount_with_arrows) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("-> $1.00 <--"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("-> $1.00 <--"));
}

TEST(DNS_idn_hostname, invalid_internal_space) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a b"));
}

TEST(DNS_idn_hostname, invalid_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a_b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a_b"));
}

// RFC 1035 §2.3.4: 64-octet label exceeds the per-label cap
TEST(DNS_idn_hostname, invalid_ascii_label_at_64) {
  const std::string label(64, 'a');
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname(label));
  EXPECT_FALSE(sourcemeta::core::is_hostname(label));
}

// RFC 5321 §4.5.3.1.2: 256-octet total exceeds the cap. Construction avoids
// trailing-dot and per-label confounds (see idn_email_test.cc rationale)
TEST(DNS_idn_hostname, invalid_total_at_256) {
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
TEST(DNS_idn_hostname, invalid_lone_continuation_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xbf"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xbf"));
}

// RFC 6532 §3.1: 2-byte starter without a continuation byte
TEST(DNS_idn_hostname, invalid_truncated_two_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xce"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xce"));
}

// RFC 6532 §3.1: %xE0 %x80-9F is an overlong 3-byte encoding
TEST(DNS_idn_hostname, invalid_overlong_three_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xe0\x80\xa0"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xe0\x80\xa0"));
}

// RFC 6532 §3.1: U+D800 surrogate codepoint is forbidden
TEST(DNS_idn_hostname, invalid_surrogate_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xed\xa0\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xed\xa0\x80"));
}

// RFC 6532 §3.1: codepoints above U+10FFFF are forbidden
TEST(DNS_idn_hostname, invalid_above_max_codepoint) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xf4\x90\x80\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf4\x90\x80\x80"));
}

// RFC 6532 §3.1: 4-byte starter with truncated continuation
TEST(DNS_idn_hostname, invalid_truncated_four_byte) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xf0\x9f\x98"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf0\x9f\x98"));
}

// RFC 6532 §3.1: %xC0 is a forbidden lead byte (overlong U+0000)
TEST(DNS_idn_hostname, invalid_overlong_c0) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xc0\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xc0\x80"));
}

// RFC 6532 §3.1: %xF5 is not a valid lead byte
TEST(DNS_idn_hostname, invalid_lead_f5) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("\xf5\x80\x80\x80"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("\xf5\x80\x80\x80"));
}

TEST(DNS_idn_hostname, invalid_invalid_utf8_in_middle_label) {
  EXPECT_FALSE(sourcemeta::core::is_idn_hostname("a.\xc0\x80.b"));
  EXPECT_FALSE(sourcemeta::core::is_hostname("a.\xc0\x80.b"));
}
