#include <gtest/gtest.h>

#include <sourcemeta/core/email.h>

#include <string>

// RFC 5321 §4.1.2: minimal Dot-string Atom + minimal Domain sub-domain
TEST(Email, valid_dot_string_single_letter) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@b"));
}

// RFC 5321 §4.1.2: atext includes DIGIT
TEST(Email, valid_dot_string_single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_email("1@b"));
}

// RFC 5321 §4.1.2: Atom = 1*atext
TEST(Email, valid_dot_string_multi_letter_atom) {
  EXPECT_TRUE(sourcemeta::core::is_email("abc@b"));
}

// RFC 5321 §4.1.2: atext mixes ALPHA and DIGIT
TEST(Email, valid_dot_string_alpha_digit_mix) {
  EXPECT_TRUE(sourcemeta::core::is_email("a1b2@b"));
}

// RFC 5321 §4.1.2: Dot-string = Atom *("." Atom) with two atoms
TEST(Email, valid_dot_string_two_atoms) {
  EXPECT_TRUE(sourcemeta::core::is_email("a.b@c"));
}

// RFC 5321 §4.1.2: Dot-string with many atoms
TEST(Email, valid_dot_string_many_atoms) {
  EXPECT_TRUE(sourcemeta::core::is_email("a.b.c.d.e@f"));
}

// RFC 5321 §4.1.2: ALPHA covers A-Z
TEST(Email, valid_dot_string_uppercase_atom) {
  EXPECT_TRUE(sourcemeta::core::is_email("ABC@b"));
}

// RFC 5321 §4.1.2: ALPHA covers both cases in one atom
TEST(Email, valid_dot_string_mixed_case_atom) {
  EXPECT_TRUE(sourcemeta::core::is_email("aBc@b"));
}

// RFC 5321 §4.1.2: "!" is atext
TEST(Email, valid_atext_bang) {
  EXPECT_TRUE(sourcemeta::core::is_email("!@b"));
}

// RFC 5321 §4.1.2: "#" is atext
TEST(Email, valid_atext_hash) {
  EXPECT_TRUE(sourcemeta::core::is_email("#@b"));
}

// RFC 5321 §4.1.2: "$" is atext
TEST(Email, valid_atext_dollar) {
  EXPECT_TRUE(sourcemeta::core::is_email("$@b"));
}

// RFC 5321 §4.1.2: "%" is atext
TEST(Email, valid_atext_percent) {
  EXPECT_TRUE(sourcemeta::core::is_email("%@b"));
}

// RFC 5321 §4.1.2: "&" is atext
TEST(Email, valid_atext_ampersand) {
  EXPECT_TRUE(sourcemeta::core::is_email("&@b"));
}

// RFC 5321 §4.1.2: "'" is atext
TEST(Email, valid_atext_apostrophe) {
  EXPECT_TRUE(sourcemeta::core::is_email("'@b"));
}

// RFC 5321 §4.1.2: "*" is atext
TEST(Email, valid_atext_asterisk) {
  EXPECT_TRUE(sourcemeta::core::is_email("*@b"));
}

// RFC 5321 §4.1.2: "+" is atext
TEST(Email, valid_atext_plus) {
  EXPECT_TRUE(sourcemeta::core::is_email("+@b"));
}

// RFC 5321 §4.1.2: "-" is atext
TEST(Email, valid_atext_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_email("-@b"));
}

// RFC 5321 §4.1.2: "/" is atext
TEST(Email, valid_atext_slash) {
  EXPECT_TRUE(sourcemeta::core::is_email("/@b"));
}

// RFC 5321 §4.1.2: "=" is atext
TEST(Email, valid_atext_equals) {
  EXPECT_TRUE(sourcemeta::core::is_email("=@b"));
}

// RFC 5321 §4.1.2: "?" is atext
TEST(Email, valid_atext_question) {
  EXPECT_TRUE(sourcemeta::core::is_email("?@b"));
}

// RFC 5321 §4.1.2: "^" is atext
TEST(Email, valid_atext_caret) {
  EXPECT_TRUE(sourcemeta::core::is_email("^@b"));
}

// RFC 5321 §4.1.2: "_" is atext
TEST(Email, valid_atext_underscore) {
  EXPECT_TRUE(sourcemeta::core::is_email("_@b"));
}

// RFC 5321 §4.1.2: "`" is atext
TEST(Email, valid_atext_backtick) {
  EXPECT_TRUE(sourcemeta::core::is_email("`@b"));
}

// RFC 5321 §4.1.2: "{" is atext
TEST(Email, valid_atext_lbrace) {
  EXPECT_TRUE(sourcemeta::core::is_email("{@b"));
}

// RFC 5321 §4.1.2: "|" is atext
TEST(Email, valid_atext_pipe) {
  EXPECT_TRUE(sourcemeta::core::is_email("|@b"));
}

// RFC 5321 §4.1.2: "}" is atext
TEST(Email, valid_atext_rbrace) {
  EXPECT_TRUE(sourcemeta::core::is_email("}@b"));
}

// RFC 5321 §4.1.2: "~" is atext
TEST(Email, valid_atext_tilde) {
  EXPECT_TRUE(sourcemeta::core::is_email("~@b"));
}

// RFC 5321 §4.1.2: a single Atom may include every atext special at once
TEST(Email, valid_dot_string_all_specials_one_atom) {
  EXPECT_TRUE(sourcemeta::core::is_email("!#$%&'*+-/=?^_`{|}~@b"));
}

// RFC 5321 §4.5.3.1.1: Local-part octet limit is 64
TEST(Email, valid_local_part_length_64) {
  EXPECT_TRUE(sourcemeta::core::is_email(std::string(64, 'a') + "@b"));
}

// RFC 5321 §4.1.2: Atom = 1*atext, single byte is the minimum
TEST(Email, valid_local_part_length_1) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@b"));
}

// RFC 5321 §4.1.2: Mailbox requires both a Local-part and a Domain
TEST(Email, invalid_no_at_sign) {
  EXPECT_FALSE(sourcemeta::core::is_email("plain"));
}

// RFC 5321 §4.1.2: Local-part = Dot-string / Quoted-string, both non-empty
TEST(Email, invalid_only_at_sign) {
  EXPECT_FALSE(sourcemeta::core::is_email("@"));
}

// RFC 5321 §4.1.2: Atom = 1*atext, empty Local-part is invalid
TEST(Email, invalid_empty_local) {
  EXPECT_FALSE(sourcemeta::core::is_email("@b"));
}

// RFC 5321 §4.1.2: Domain = sub-domain *("." sub-domain), empty is invalid
TEST(Email, invalid_empty_domain) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@"));
}

// RFC 5321 §4.1.2: only one "@" allowed outside a Quoted-string
TEST(Email, invalid_two_at_signs_unquoted) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@b@c"));
}

// RFC 5321 §4.1.2: three unquoted "@" signs
TEST(Email, invalid_three_at_signs_unquoted) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@b@c@d"));
}

// RFC 5321 §4.1.2: Dot-string requires a leading Atom
TEST(Email, invalid_local_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email(".a@b"));
}

// RFC 5321 §4.1.2: Dot-string requires a trailing Atom
TEST(Email, invalid_local_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email("a.@b"));
}

// RFC 5321 §4.1.2: Atom = 1*atext, double dot yields an empty Atom
TEST(Email, invalid_local_double_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email("a..b@c"));
}

// RFC 5321 §4.1.2: lone "." has no atext on either side
TEST(Email, invalid_local_only_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email(".@b"));
}

// RFC 5321 §4.1.2: ".." has no atext between the dots
TEST(Email, invalid_local_only_dots) {
  EXPECT_FALSE(sourcemeta::core::is_email("..@b"));
}

// RFC 5321 §4.5.3.1.1: Local-part may not exceed 64 octets
TEST(Email, invalid_local_part_length_65) {
  EXPECT_FALSE(sourcemeta::core::is_email(std::string(65, 'a') + "@b"));
}

// RFC 5321 §4.1.2: "(" is not in atext
TEST(Email, invalid_atext_lparen) {
  EXPECT_FALSE(sourcemeta::core::is_email("(@b"));
}

// RFC 5321 §4.1.2: ")" is not in atext
TEST(Email, invalid_atext_rparen) {
  EXPECT_FALSE(sourcemeta::core::is_email(")@b"));
}

// RFC 5321 §4.1.2: "," is not in atext
TEST(Email, invalid_atext_comma) {
  EXPECT_FALSE(sourcemeta::core::is_email(",@b"));
}

// RFC 5321 §4.1.2: ":" is not in atext
TEST(Email, invalid_atext_colon) {
  EXPECT_FALSE(sourcemeta::core::is_email(":@b"));
}

// RFC 5321 §4.1.2: ";" is not in atext
TEST(Email, invalid_atext_semicolon) {
  EXPECT_FALSE(sourcemeta::core::is_email(";@b"));
}

// RFC 5321 §4.1.2: "<" is not in atext
TEST(Email, invalid_atext_lt) {
  EXPECT_FALSE(sourcemeta::core::is_email("<@b"));
}

// RFC 5321 §4.1.2: ">" is not in atext
TEST(Email, invalid_atext_gt) {
  EXPECT_FALSE(sourcemeta::core::is_email(">@b"));
}

// RFC 5321 §4.1.2: "[" is not in atext
TEST(Email, invalid_atext_lbracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("[@b"));
}

// RFC 5321 §4.1.2: "\" is not in atext (only valid inside quoted-pair)
TEST(Email, invalid_atext_backslash) {
  EXPECT_FALSE(sourcemeta::core::is_email("\\@b"));
}

// RFC 5321 §4.1.2: "]" is not in atext
TEST(Email, invalid_atext_rbracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("]@b"));
}

// RFC 5321 §4.1.2: SP is not in atext
TEST(Email, invalid_atext_space_unquoted) {
  EXPECT_FALSE(sourcemeta::core::is_email(" @b"));
}

// RFC 5321 §4.1.2: atext is ASCII; bytes >= 0x80 are excluded
TEST(Email, invalid_local_high_bit_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("a\x80@b"));
}

// RFC 5321 §4.1.2: NUL is not in atext
TEST(Email, invalid_local_nul_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email(std::string_view{"a\x00@b", 4}));
}

// RFC 5321 §4.1.2: control characters are not in atext
TEST(Email, invalid_local_control_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("a\x01@b"));
}

// RFC 5321 §4.1.2: DEL is not in atext
TEST(Email, invalid_local_del_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("a\x7f@b"));
}

// RFC 5321 §4.1.2 Domain: sub-domain *("." sub-domain) with one label
TEST(Email, valid_domain_single_label) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@b"));
}

// RFC 5321 §4.1.2 Domain: two labels separated by "."
TEST(Email, valid_domain_two_labels) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@b.c"));
}

// RFC 5321 §4.1.2 Domain: many labels
TEST(Email, valid_domain_three_labels) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@b.c.d"));
}

// RFC 5321 §4.1.2 Domain: sub-domain = Let-dig [Ldh-str], digit is Let-dig
TEST(Email, valid_domain_label_starts_with_digit) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@1b.c"));
}

// RFC 5321 §4.1.2 Domain: grammar allows numeric-only labels
TEST(Email, valid_domain_numeric_tld) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@example.123"));
}

// RFC 5321 §4.1.2 Domain: case is preserved but accepted
TEST(Email, valid_domain_uppercase) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@EXAMPLE.COM"));
}

// RFC 1035 §2.3.4 via §4.1.2 Domain: 63-byte label is the cap
TEST(Email, valid_domain_label_63) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@" + std::string(63, 'b')));
}

// RFC 5321 §4.5.3.1.2 Domain: 255-byte domain is the cap
TEST(Email, valid_domain_total_255) {
  EXPECT_TRUE(sourcemeta::core::is_email(
      "a@" + std::string(63, 'b') + "." + std::string(63, 'c') + "." +
      std::string(63, 'd') + "." + std::string(63, 'e')));
}

// RFC 5321 §4.1.2 Domain: Ldh-str excludes "_"
TEST(Email, invalid_domain_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@host_name"));
}

// RFC 5321 §4.1.2 Domain: sub-domain must start with Let-dig
TEST(Email, invalid_domain_leading_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@-host"));
}

// RFC 5321 §4.1.2 Domain: Ldh-str must end with Let-dig
TEST(Email, invalid_domain_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@host-"));
}

// RFC 5321 §4.1.2 Domain: no trailing "."
TEST(Email, invalid_domain_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@host."));
}

// RFC 5321 §4.1.2 Domain: "." between labels requires a sub-domain on each side
TEST(Email, invalid_domain_double_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@x..y"));
}

// RFC 5321 §4.1.2 Domain: no leading "."
TEST(Email, invalid_domain_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@.host"));
}

// RFC 1035 §2.3.4 via §4.1.2 Domain: 64-byte label exceeds cap
TEST(Email, invalid_domain_label_64) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@" + std::string(64, 'b')));
}

// RFC 5321 §4.5.3.1.2 Domain: 256-byte domain exceeds cap (63 + "." + 63 +
// "." + 63 + "." + 62 + "." + "f" = 256 bytes)
TEST(Email, invalid_domain_total_256) {
  EXPECT_FALSE(sourcemeta::core::is_email(
      "a@" + std::string(63, 'b') + "." + std::string(63, 'c') + "." +
      std::string(63, 'd') + "." + std::string(62, 'e') + ".f"));
}

// RFC 5321 §4.1.2 Domain: ASCII only, bytes >= 0x80 excluded
TEST(Email, invalid_domain_high_bit_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@hos\x80t"));
}

// RFC 5321 §4.1.2 Domain: SP is not in Ldh-str
TEST(Email, invalid_domain_space) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@host name"));
}

// RFC 5321 §4.1.2: Quoted-string = DQUOTE *QcontentSMTP DQUOTE permits zero
// content bytes
TEST(Email, valid_quoted_empty) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP includes %d32 SP
TEST(Email, valid_quoted_single_space) {
  EXPECT_TRUE(sourcemeta::core::is_email("\" \"@b"));
}

// RFC 5321 §4.1.2: any atext byte is also in qtextSMTP
TEST(Email, valid_quoted_single_atext) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"a\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP %d35-91 includes "@"
TEST(Email, valid_quoted_at_inside) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"a@b\"@c"));
}

// RFC 5321 §4.1.2: Dot-string rules do not apply inside Quoted-string
TEST(Email, valid_quoted_dot_inside) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"a.b\"@c"));
}

// RFC 5321 §4.1.2: Quoted-string permits a leading "." inside
TEST(Email, valid_quoted_dot_at_start) {
  EXPECT_TRUE(sourcemeta::core::is_email("\".a\"@b"));
}

// RFC 5321 §4.1.2: Quoted-string permits a trailing "." inside
TEST(Email, valid_quoted_dot_at_end) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"a.\"@b"));
}

// RFC 5321 §4.1.2: Quoted-string permits consecutive "." inside
TEST(Email, valid_quoted_double_dot_inside) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"a..b\"@c"));
}

// RFC 5321 §4.1.2: qtextSMTP starts at %d32 (SP)
TEST(Email, valid_quoted_qtext_d32_space) {
  EXPECT_TRUE(sourcemeta::core::is_email("\" \"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP includes %d33 "!"
TEST(Email, valid_quoted_qtext_d33_bang) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"!\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP resumes at %d35 "#" after skipping DQUOTE
TEST(Email, valid_quoted_qtext_d35_hash) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"#\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP %d35-91 ends at "[" (%d91)
TEST(Email, valid_quoted_qtext_d91_lbracket) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"[\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP resumes at "]" (%d93) after skipping "\\"
TEST(Email, valid_quoted_qtext_d93_rbracket) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"]\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP ends at "~" (%d126)
TEST(Email, valid_quoted_qtext_d126_tilde) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"~\"@b"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP escapes DQUOTE
TEST(Email, valid_quoted_pair_dquote) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"\\\"\"@b"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP escapes backslash itself
TEST(Email, valid_quoted_pair_backslash) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"\\\\\"@b"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP body %d32 is SP
TEST(Email, valid_quoted_pair_space) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"\\ \"@b"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP body covers any ASCII graphic
TEST(Email, valid_quoted_pair_letter) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"\\a\"@b"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP body %d126 is "~"
TEST(Email, valid_quoted_pair_tilde) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"\\~\"@b"));
}

// RFC 5321 §4.1.2: Quoted-string accepts qtextSMTP and quoted-pairSMTP mixed
TEST(Email, valid_quoted_mixed_qtext_and_pair) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"a\\\"b\\\\c\"@d"));
}

// RFC 5321 §4.1.2: qtextSMTP includes ',' and ';' (both in %d35-91)
TEST(Email, valid_quoted_with_comma_semicolon) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"a,b;c\"@d"));
}

// RFC 5321 §4.1.2: qtextSMTP includes '(' and ')' (both in %d35-91)
TEST(Email, valid_quoted_with_parens) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"(comment)\"@d"));
}

// RFC 5321 §4.5.3.1.1: 62 qtext bytes plus two DQUOTEs equals 64 octets
TEST(Email, valid_quoted_local_length_64) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"" + std::string(62, 'a') + "\"@b"));
}

// RFC 5321 §4.1.2: Quoted-string must terminate with DQUOTE
TEST(Email, invalid_quoted_unterminated) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"foo@b"));
}

// RFC 5321 §4.1.2: extra content after closing DQUOTE is not Mailbox grammar
TEST(Email, invalid_quoted_followed_by_atext) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"a\"b@c"));
}

// RFC 5321 §4.1.2: Mailbox grammar does not permit mixing Quoted-string and
// Dot-string in a Local-part
TEST(Email, invalid_quoted_followed_by_dot_atext) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"a\".b@c"));
}

// RFC 5321 §4.1.2: a Quoted-string cannot be preceded by atext
TEST(Email, invalid_quoted_preceded_by_atext) {
  EXPECT_FALSE(sourcemeta::core::is_email("a\"b\"@c"));
}

// RFC 5321 §4.1.2: bare DQUOTE inside a Quoted-string closes it; the trailing
// bytes break the Mailbox grammar
TEST(Email, invalid_quoted_bare_dquote_inside) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"a\"b\"@c"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP consumes the byte after "\\"; if that byte
// is DQUOTE the Quoted-string is left unterminated
TEST(Email, invalid_quoted_dangling_backslash) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"a\\\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP excludes controls (%d0-31)
TEST(Email, invalid_quoted_qtext_control_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"\x01\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP excludes NUL
TEST(Email, invalid_quoted_qtext_nul) {
  EXPECT_FALSE(sourcemeta::core::is_email(std::string_view{"\"\x00\"@b", 5}));
}

// RFC 5321 §4.1.2: qtextSMTP is ASCII, bytes >= 0x80 are excluded
TEST(Email, invalid_quoted_qtext_high_bit) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"\x80\"@b"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP body is %d32-126, controls are excluded
TEST(Email, invalid_quoted_pair_control_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"\\\x01\"@b"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP body ends at %d126, DEL is excluded
TEST(Email, invalid_quoted_pair_del_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"\\\x7f\"@b"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP body is ASCII, bytes >= 0x80 are excluded
TEST(Email, invalid_quoted_pair_high_bit_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"\\\x80\"@b"));
}

// RFC 5321 §4.5.3.1.1: 63 qtext bytes plus two DQUOTEs equals 65 octets, one
// past the cap
TEST(Email, invalid_quoted_local_length_65) {
  EXPECT_FALSE(
      sourcemeta::core::is_email("\"" + std::string(63, 'a') + "\"@b"));
}

// RFC 5321 §4.1.2: a Quoted-string may contain an unquoted "@" but the outer
// boundary "@" is still required after the closing DQUOTE
TEST(Email, valid_two_at_signs_one_quoted) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"a@b\"@c"));
}

// RFC 5321 §4.1.2: two embedded "@" inside a Quoted-string are still qtextSMTP
TEST(Email, valid_two_at_signs_quoted_with_atext) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"x@y@z\"@d"));
}

// RFC 5321 §4.1.3: IPv4-address-literal Snum 3("." Snum) covers 0.0.0.0
TEST(Email, valid_ipv4_literal_zeros) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[0.0.0.0]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal Snum max is 255
TEST(Email, valid_ipv4_literal_max) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[255.255.255.255]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal typical RFC 1918 address
TEST(Email, valid_ipv4_literal_typical) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[192.168.1.1]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal loopback
TEST(Email, valid_ipv4_literal_loopback) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[127.0.0.1]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal one-digit Snum
TEST(Email, valid_ipv4_literal_single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[1.2.3.4]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal two-digit Snum
TEST(Email, valid_ipv4_literal_two_digit) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[10.20.30.40]"));
}

// RFC 5321 §4.1.2: Domain accepts numeric-only labels without brackets
TEST(Email, valid_domain_dotted_numeric) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@1.2.3.4"));
}

// RFC 5321 §4.1.3: Snum value range 0-255, 256 is out of range
TEST(Email, invalid_ipv4_octet_256) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[256.0.0.0]"));
}

// RFC 5321 §4.1.3: Snum value range 0-255, 999 is out of range
TEST(Email, invalid_ipv4_octet_999) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[999.0.0.1]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal requires exactly four Snum octets
TEST(Email, invalid_ipv4_three_octets) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[1.2.3]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal rejects five Snum octets
TEST(Email, invalid_ipv4_five_octets) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[1.2.3.4.5]"));
}

// RFC 5321 §4.1.3: Snum = 1*3DIGIT, leading zero in multi-digit Snum is
// rejected by is_ipv4
TEST(Email, invalid_ipv4_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[01.2.3.4]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal cannot end with a "."
TEST(Email, invalid_ipv4_trailing_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[1.2.3.4.]"));
}

// RFC 5321 §4.1.3: IPv4-address-literal cannot begin with a "."
TEST(Email, invalid_ipv4_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[.1.2.3.4]"));
}

// RFC 5321 §4.1.3: Snum = 1*3DIGIT, "-" is not a digit
TEST(Email, invalid_ipv4_negative_octet) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[-1.2.3.4]"));
}

// RFC 5321 §4.1.3: Snum = 1*3DIGIT, alphabetic bytes are not digits
TEST(Email, invalid_ipv4_alpha_octet) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[1.2.a.4]"));
}

// RFC 5321 §4.1.3: address-literal requires a closing "]"
TEST(Email, invalid_ipv4_missing_close_bracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[1.2.3.4"));
}

// RFC 5321 §4.1.2 Domain: "]" is not in Ldh-str, so unbracketed forms fail
TEST(Email, invalid_ipv4_missing_open_bracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@1.2.3.4]"));
}

// RFC 5321 §4.1.3: no content is permitted after the closing "]"
TEST(Email, invalid_ipv4_trailing_garbage) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[1.2.3.4]x"));
}

// RFC 5321 §4.1.2 Domain: "[" is not in Ldh-str, so embedded brackets fail
TEST(Email, invalid_ipv4_leading_garbage) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@x[1.2.3.4]"));
}

// RFC 5321 §4.1.3: address-literal requires at least one of IPv4, IPv6, or
// General-address-literal between the brackets
TEST(Email, invalid_ipv4_empty_brackets) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[]"));
}

// RFC 5321 §4.1.3: IPv6-address-literal = "IPv6:" IPv6-addr, loopback form
TEST(Email, valid_ipv6_literal_loopback) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6:::1]"));
}

// RFC 4291 §2.2: "::" compresses an all-zeros address
TEST(Email, valid_ipv6_literal_all_zeros) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6:::]"));
}

// RFC 5321 §4.1.3: IPv6-address-literal with one compressed group
TEST(Email, valid_ipv6_literal_typical) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6:2001:db8::1]"));
}

// RFC 4291 §2.2: IPv6-full form with eight 1-4 hex groups
TEST(Email, valid_ipv6_literal_full_form) {
  EXPECT_TRUE(sourcemeta::core::is_email(
      "a@[IPv6:2001:0db8:0000:0000:0000:0000:0000:0001]"));
}

// RFC 4291 §2.5.6: link-local prefix fe80::/10
TEST(Email, valid_ipv6_literal_link_local) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6:fe80::1]"));
}

// RFC 4291 §2.2: IPv4-mapped IPv6 address form
TEST(Email, valid_ipv6_literal_v4_mapped) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6:::ffff:192.168.1.1]"));
}

// RFC 4291 §2.5.5: IPv4-compatible IPv6 address form
TEST(Email, valid_ipv6_literal_v4_compat) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6:::192.168.1.1]"));
}

// RFC 5321 §4.1.3: without the "IPv6:" tag the literal is parsed as
// General-address-literal (tag "2001", content "db8::1"), which is valid
// because ":" is in dcontent (%d58 is within %d33-90)
TEST(Email, valid_no_ipv6_prefix_as_general) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[2001:db8::1]"));
}

// RFC 5234 §2.3: ABNF literal strings are case-insensitive by default, so the
// "IPv6:" prefix matches "ipv6:"
TEST(Email, valid_lowercase_ipv6_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[ipv6:::1]"));
}

// RFC 5234 §2.3: case-insensitive literal also matches "IPV6:"
TEST(Email, valid_uppercase_ipv6_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPV6:::1]"));
}

// RFC 5234 §2.3: mixed-case prefix also matches the IPv6 tag
TEST(Email, valid_mixed_case_ipv6_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[iPv6:::1]"));
}

// RFC 5234 §3.2: ABNF alternatives are unordered. The literal five-byte
// prefix "IPv6:" is stripped, ":1" fails IPv6-addr, and the input falls
// through to General-address-literal with tag "IPv6" and content ":1"
TEST(Email, valid_ipv6_prefix_no_colon_as_general) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6::1]"));
}

// RFC 5234 §3.2: a failed IPv6-addr match falls through to General-address-
// literal with tag "IPv6" and content "not-an-address" (all dcontent)
TEST(Email, valid_ipv6_body_garbage_as_general) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6:not-an-address]"));
}

// RFC 5321 §4.1.3: IPv6-addr requires at least one group
TEST(Email, invalid_ipv6_body_empty) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[IPv6:]"));
}

// RFC 5234 §3.2: nine groups fail IPv6-addr but the input still matches
// General-address-literal with tag "IPv6" and content "1:2:3:4:5:6:7:8:9"
TEST(Email, valid_ipv6_too_many_groups_as_general) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv6:1:2:3:4:5:6:7:8:9]"));
}

// RFC 5321 §4.1.3: address-literal needs a closing "]"
TEST(Email, invalid_ipv6_missing_close_bracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[IPv6:::1"));
}

// RFC 5321 §4.1.3: no content is permitted after the closing "]"
TEST(Email, invalid_ipv6_trailing_garbage) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[IPv6:::1]x"));
}

// RFC 5321 §4.1.3: General-address-literal = Standardized-tag ":" 1*dcontent
TEST(Email, valid_general_literal_minimal) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[X:y]"));
}

// RFC 5321 §4.1.3: typical X400 tag from the IANA Standardized-tag registry
TEST(Email, valid_general_literal_x400) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[X400:foo]"));
}

// RFC 5321 §4.1.2: Ldh-str body permits DIGIT before the terminal Let-dig
TEST(Email, valid_general_literal_tag_with_digits) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[tag1:foo]"));
}

// RFC 5321 §4.1.2: Ldh-str body permits interior "-"
TEST(Email, valid_general_literal_tag_with_interior_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[tag-name:foo]"));
}

// RFC 5321 §4.1.2: Ldh-str = *( ALPHA / DIGIT / "-" ) Let-dig permits a
// leading "-" because Standardized-tag is Ldh-str, not Let-dig [Ldh-str]
TEST(Email, valid_general_literal_tag_leading_hyphen) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[-tag:foo]"));
}

// RFC 5321 §4.1.3: dcontent starts at %d33 "!"
TEST(Email, valid_general_literal_dcontent_lower_bound) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[Tag:!]"));
}

// RFC 5321 §4.1.3: dcontent %d33-90 ends at "Z"
TEST(Email, valid_general_literal_dcontent_upper_first_range) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[Tag:Z]"));
}

// RFC 5321 §4.1.3: dcontent %d94-126 starts at "^"
TEST(Email, valid_general_literal_dcontent_lower_second_range) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[Tag:^]"));
}

// RFC 5321 §4.1.3: dcontent ends at "~" (%d126)
TEST(Email, valid_general_literal_dcontent_upper_bound) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[Tag:~]"));
}

// RFC 5321 §4.1.3: dcontent %d33-90 includes ":" (%d58)
TEST(Email, valid_general_literal_dcontent_with_colon) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[Tag:foo:bar]"));
}

// RFC 5321 §4.1.3: 1*dcontent permits long content
TEST(Email, valid_general_literal_dcontent_long) {
  EXPECT_TRUE(
      sourcemeta::core::is_email("a@[Tag:" + std::string(200, 'a') + "]"));
}

// RFC 5321 §4.1.2: Ldh-str must end with Let-dig, trailing "-" is invalid
TEST(Email, invalid_general_tag_trailing_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[tag-:x]"));
}

// RFC 5321 §4.1.2: Ldh-str requires a terminal Let-dig, lone "-" is invalid
TEST(Email, invalid_general_tag_single_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[-:x]"));
}

// RFC 5321 §4.1.2: Ldh-str alphabet excludes "_"
TEST(Email, invalid_general_tag_with_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[tag_name:x]"));
}

// RFC 5321 §4.1.2: Standardized-tag = Ldh-str, minimum length is one byte
TEST(Email, invalid_general_tag_empty) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[:x]"));
}

// RFC 5321 §4.1.3: General-address-literal = tag ":" 1*dcontent, empty content
// is invalid
TEST(Email, invalid_general_empty_dcontent) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[X400:]"));
}

// RFC 5321 §4.1.3: General-address-literal requires ":" between tag and
// content
TEST(Email, invalid_general_no_colon) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[X400foo]"));
}

// RFC 5321 §4.1.3: dcontent excludes "[" (%d91)
TEST(Email, invalid_general_dcontent_lbracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[X400:[]"));
}

// RFC 5321 §4.1.3: dcontent excludes "\\" (%d92)
TEST(Email, invalid_general_dcontent_backslash) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[X400:\\]"));
}

// RFC 5321 §4.1.3: dcontent excludes "]" (%d93)
TEST(Email, invalid_general_dcontent_rbracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[X400:a]b]"));
}

// RFC 5321 §4.1.3: dcontent excludes SP (%d32)
TEST(Email, invalid_general_dcontent_space) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[X400:a b]"));
}

// RFC 5321 §4.1.3: dcontent excludes controls (%d0-31)
TEST(Email, invalid_general_dcontent_control) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[X400:\x01]"));
}

// RFC 5321 §4.1.3: dcontent is ASCII, bytes >= 0x80 are excluded
TEST(Email, invalid_general_dcontent_high_bit) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[X400:\x80]"));
}

// RFC 5321 §4.5.3.1: 64-byte Local-part plus "@" plus a 254-byte Domain still
// fits within both caps
TEST(Email, valid_max_length_email) {
  EXPECT_TRUE(sourcemeta::core::is_email(
      std::string(64, 'a') + "@" + std::string(63, 'b') + "." +
      std::string(63, 'c') + "." + std::string(63, 'd') + "." +
      std::string(62, 'e')));
}

// RFC 5321 §4.5.3.1.1: 65-byte Local-part exceeds the cap even with a valid
// Domain
TEST(Email, invalid_local_65_with_valid_domain) {
  EXPECT_FALSE(sourcemeta::core::is_email(std::string(65, 'a') + "@b"));
}

// RFC 5321 §4.5.3.1.2: a Domain exceeding the cap is rejected even with a
// valid Local-part
TEST(Email, invalid_local_64_with_domain_over_cap) {
  EXPECT_FALSE(sourcemeta::core::is_email(
      std::string(64, 'a') + "@" + std::string(63, 'b') + "." +
      std::string(63, 'c') + "." + std::string(63, 'd') + "." +
      std::string(63, 'e') + ".f"));
}

// RFC 5321 §4.1.2 + §4.1.3: Quoted-string Local-part with IPv4 address-literal
TEST(Email, valid_quoted_local_with_ipv4_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"foo\"@[192.168.1.1]"));
}

// RFC 5321 §4.1.2 + §4.1.3: Quoted-string Local-part with IPv6 address-literal
TEST(Email, valid_quoted_local_with_ipv6_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"foo\"@[IPv6:::1]"));
}

// RFC 5321 §4.1.2 + §4.1.3: Quoted-string Local-part with General-address-
// literal
TEST(Email, valid_quoted_local_with_general_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"foo\"@[X400:bar]"));
}

// RFC 5321 §4.1.2 + §4.1.3: Dot-string Local-part with IPv4 address-literal
TEST(Email, valid_dot_string_with_ipv4_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("foo@[192.168.1.1]"));
}

// RFC 5321 §4.1.2 + §4.1.3: Dot-string Local-part with IPv6 address-literal
TEST(Email, valid_dot_string_with_ipv6_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("foo@[IPv6:::1]"));
}

// RFC 5321 §4.1.2 + §4.1.3: Dot-string Local-part with General-address-literal
TEST(Email, valid_dot_string_with_general_literal) {
  EXPECT_TRUE(sourcemeta::core::is_email("foo@[X400:bar]"));
}

// RFC 5321 §4.5.3.1.2: an address-literal whose total length equals the
// 255-octet domain cap (including the surrounding "[" and "]") is accepted
TEST(Email, valid_address_literal_length_255) {
  EXPECT_TRUE(
      sourcemeta::core::is_email("a@[X:" + std::string(251, 'a') + "]"));
}

// RFC 5321 §4.5.3.1.2: an address-literal one octet past the 255-octet cap is
// rejected
TEST(Email, invalid_address_literal_length_256) {
  EXPECT_FALSE(
      sourcemeta::core::is_email("a@[X:" + std::string(252, 'a') + "]"));
}

// RFC 5321 §4.1.2: Mailbox cannot be empty
TEST(Email, invalid_empty_input) {
  EXPECT_FALSE(sourcemeta::core::is_email(""));
}

// RFC 5321 §4.1.2: SP is not in atext, qtextSMTP, or Ldh-str outside a
// Quoted-string
TEST(Email, invalid_whitespace_only) {
  EXPECT_FALSE(sourcemeta::core::is_email("   "));
}

// RFC 5321 §4.1.2: leading SP is not part of Dot-string or Quoted-string
TEST(Email, invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_email(" a@b"));
}

// RFC 5321 §4.1.2 Domain: trailing SP is not in Ldh-str
TEST(Email, invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@b "));
}

// RFC 5321 §4.1.2 Domain: NUL is not in Ldh-str
TEST(Email, invalid_nul_in_domain) {
  EXPECT_FALSE(sourcemeta::core::is_email(std::string_view{"a@b\x00c", 5}));
}

// RFC 5321 §4.1.2: CRLF bytes are not in the Mailbox alphabet
TEST(Email, invalid_crlf) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@b\r\n"));
}

// RFC 5321 §4.1.2: LF is not in the Mailbox alphabet
TEST(Email, invalid_lf) { EXPECT_FALSE(sourcemeta::core::is_email("a@b\n")); }

// RFC 5321 §4.1.2: TAB is not in atext or Ldh-str
TEST(Email, invalid_tab_in_local) {
  EXPECT_FALSE(sourcemeta::core::is_email("a\tb@c"));
}

// RFC 5321 §4.1.2 Domain: TAB is not in Ldh-str
TEST(Email, invalid_tab_in_domain) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@b\tc"));
}

// RFC 5321 §4.1.2: two consecutive "@" produce an empty Local-part on the
// left and a Dot-string Atom on the right that contains "@"
TEST(Email, invalid_consecutive_at_signs) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@@b"));
}

// RFC 5321 §4.1.2: a single Quoted-string byte must still be followed by the
// "@" boundary and a Domain
TEST(Email, invalid_quoted_only) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"a\""));
}

// RFC 5321 §4.1.2: a lone DQUOTE is an unterminated Quoted-string
TEST(Email, invalid_lone_dquote) {
  EXPECT_FALSE(sourcemeta::core::is_email("\""));
}

// RFC 5321 §4.1.2: a lone "@" has neither Local-part nor Domain
TEST(Email, invalid_lone_at) { EXPECT_FALSE(sourcemeta::core::is_email("@")); }

// RFC 5321 §4.1.2: minimum-length Mailbox is a single atext byte plus "@"
// plus a single Let-dig byte
TEST(Email, valid_minimum_length_mailbox) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@b"));
}

// RFC 5321 §4.1.3: a single dcontent byte is the minimum 1*dcontent
TEST(Email, valid_general_literal_minimum_dcontent) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[A:B]"));
}

// RFC 5321 §4.1.2 ALPHA upper bound: "Z" (%d90) is in atext
TEST(Email, valid_atext_alpha_upper_Z) {
  EXPECT_TRUE(sourcemeta::core::is_email("Z@b"));
}

// RFC 5321 §4.1.2 ALPHA upper bound: "z" (%d122) is in atext
TEST(Email, valid_atext_alpha_lower_z) {
  EXPECT_TRUE(sourcemeta::core::is_email("z@b"));
}

// RFC 5321 §4.1.2 DIGIT upper bound: "9" (%d57) is in atext
TEST(Email, valid_atext_digit_nine) {
  EXPECT_TRUE(sourcemeta::core::is_email("9@b"));
}

// RFC 5321 §4.1.2: Atom = 1*atext, an all-digit atom is permitted
TEST(Email, valid_dot_string_numeric_atom) {
  EXPECT_TRUE(sourcemeta::core::is_email("123@b"));
}

// RFC 5321 §4.1.2: atext mixes letters, digits, and specials in one atom
TEST(Email, valid_dot_string_alpha_digit_special_mix) {
  EXPECT_TRUE(sourcemeta::core::is_email("aB1!c2#@d"));
}

// RFC 5321 §4.1.2: qtextSMTP %d93-126 excludes %d127 (DEL)
TEST(Email, invalid_quoted_qtext_del_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"\x7f\"@b"));
}

// RFC 5321 §4.1.2: qtextSMTP excludes %d9 (TAB)
TEST(Email, invalid_quoted_qtext_tab) {
  EXPECT_FALSE(sourcemeta::core::is_email("\"a\tb\"@c"));
}

// RFC 5321 §4.1.2: quoted-pairSMTP body excludes NUL (%d0)
TEST(Email, invalid_quoted_pair_nul_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email(std::string_view{"\"\\\x00\"@b", 6}));
}

// RFC 5321 §4.1.2: quoted-pairSMTP body permits "@" (%d64)
TEST(Email, valid_quoted_pair_at_sign) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"\\@\"@b"));
}

// RFC 5321 §4.1.2: two consecutive quoted-pairs back-to-back inside a
// Quoted-string
TEST(Email, valid_quoted_two_consecutive_pairs) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"\\\\\\\"\"@b"));
}

// RFC 5321 §4.1.2 Domain: a single Let-dig digit is a valid sub-domain
TEST(Email, valid_domain_single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@1"));
}

// RFC 5321 §4.1.2: a single atext byte is not a valid Mailbox without "@"
TEST(Email, invalid_single_atext_no_at) {
  EXPECT_FALSE(sourcemeta::core::is_email("a"));
}

// RFC 5321 §4.1.2 Domain: a stray "]" with no opening "[" cannot match
// address-literal and "]" is not in Ldh-str
TEST(Email, invalid_unbalanced_closing_bracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@b]"));
}

// RFC 5321 §4.1.2 Domain: "[" embedded in a Domain is not in Ldh-str
TEST(Email, invalid_bracket_in_middle_of_domain) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@b[c]d"));
}

// RFC 5321 §4.1.3: a domain consisting of just "[" never closes the
// address-literal
TEST(Email, invalid_domain_just_open_bracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@["));
}

// RFC 5321 §4.1.2 Domain: a domain consisting of just "]" is not Ldh-str
TEST(Email, invalid_domain_just_close_bracket) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@]"));
}

// RFC 5321 §4.1.3: "::1" between brackets has no "IPv6:" prefix and an empty
// Standardized-tag for General-address-literal, so all three alternatives
// reject it
TEST(Email, invalid_bracket_just_ipv6_addr_no_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[::1]"));
}

// RFC 5321 §4.1.3: a bracketed word without ":" cannot match General, and
// without digits cannot match IPv4
TEST(Email, invalid_bracket_with_plain_word) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[hello]"));
}

// RFC 5321 §4.1.3: leading SP inside the brackets fails IPv4 (non-digit) and
// fails General (Standardized-tag has no SP)
TEST(Email, invalid_bracket_with_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[ 1.2.3.4]"));
}

// RFC 5321 §4.1.3 + RFC 5234 §3.2: a case-insensitive "IPv6:" match that
// fails IPv6-addr still falls through to General-address-literal
TEST(Email, valid_lowercase_ipv6_fallthrough_to_general) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[ipv6:not-an-address]"));
}

// RFC 5321 §4.1.2 Ldh-str: leading "-" before another "-" before Let-dig is
// still a valid Ldh-str
TEST(Email, valid_general_literal_multiple_leading_hyphens) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[--a:b]"));
}

// RFC 5321 §4.1.3: any Ldh-str is a valid Standardized-tag per the grammar,
// including ones not registered with IANA such as "IPv7"
TEST(Email, valid_general_literal_ipv7_like_tag) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[IPv7:foo]"));
}

// RFC 5321 §4.1.2 Ldh-str alphabet excludes ".", so a tag with "." cannot
// match Standardized-tag
TEST(Email, invalid_general_tag_with_dot) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[a.b:c]"));
}

// RFC 5321 §4.1.3: General-address-literal content of just ":" is a single
// dcontent byte (%d58 is in %d33-90)
TEST(Email, valid_general_literal_content_just_colon) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@[a::]"));
}

// RFC 5321 §4.1.3 + RFC 5234 §3.2: bracketed input where the first colon
// produces an empty tag fails the Standardized-tag rule
TEST(Email, invalid_bracket_empty_tag) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[:foo]"));
}

// RFC 5321 §4.1.2 Domain: an address-literal whose Domain branch tries
// is_hostname must reject a stray "[" inside what would otherwise be Ldh-str
TEST(Email, invalid_domain_open_bracket_inside) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@b[c"));
}

// RFC 5321 §4.1.2 Dot-string: a Quoted-string opener "\"" inside an
// otherwise Dot-string Local-part is not in atext
TEST(Email, invalid_dquote_inside_dot_string) {
  EXPECT_FALSE(sourcemeta::core::is_email("a\"b@c"));
}

// RFC 5321 §4.1.2 Mailbox: a Mailbox cannot start with the boundary "@"
TEST(Email, invalid_starts_with_at) {
  EXPECT_FALSE(sourcemeta::core::is_email("@example.com"));
}

// RFC 5321 §4.1.2 Mailbox: a Mailbox cannot end with the boundary "@"
TEST(Email, invalid_ends_with_at) {
  EXPECT_FALSE(sourcemeta::core::is_email("user@"));
}

// RFC 5321 §4.1.2: Local-part = 64 octets via Dot-string that includes "."
TEST(Email, valid_local_part_length_64_with_dots) {
  EXPECT_TRUE(sourcemeta::core::is_email(std::string(31, 'a') + "." +
                                         std::string(32, 'b') + "@c"));
}

// RFC 5321 §4.5.3.1.1: 65-octet Dot-string Local-part that contains "."
TEST(Email, invalid_local_part_length_65_with_dots) {
  EXPECT_FALSE(sourcemeta::core::is_email(std::string(32, 'a') + "." +
                                          std::string(32, 'b') + "@c"));
}

// RFC 5321 §4.1.2: a Domain consisting of many short labels still parses
TEST(Email, valid_domain_many_short_labels) {
  EXPECT_TRUE(sourcemeta::core::is_email("a@a.b.c.d.e.f.g.h.i.j.k"));
}

// RFC 5321 §4.1.3: dcontent excludes DEL (%d127)
TEST(Email, invalid_general_dcontent_del_byte) {
  EXPECT_FALSE(sourcemeta::core::is_email("a@[Tag:\x7f]"));
}

// RFC 5321 §4.1.3 + §4.5.3.1.2: a General-address-literal whose Domain total
// length equals the 255-octet cap ("[" + "Tag" + ":" + 249 dcontent + "]") is
// accepted
TEST(Email, valid_general_literal_inner_at_cap) {
  EXPECT_TRUE(
      sourcemeta::core::is_email("a@[Tag:" + std::string(249, 'x') + "]"));
}

// RFC 5321 §4.5.3.1.2: General-address-literal one octet past the 255-octet
// Domain cap is rejected
TEST(Email, invalid_general_literal_inner_over_cap) {
  EXPECT_FALSE(
      sourcemeta::core::is_email("a@[Tag:" + std::string(250, 'x') + "]"));
}

// RFC 5321 §4.1.2: a single quoted byte plus minimal Domain
TEST(Email, valid_quoted_single_letter_then_minimal_domain) {
  EXPECT_TRUE(sourcemeta::core::is_email("\"x\"@y"));
}

// RFC 5321 §4.1.2: Dot-string ending with the boundary "@" right after the
// dot has no terminating Atom
TEST(Email, invalid_dot_then_at) {
  EXPECT_FALSE(sourcemeta::core::is_email("a.@b"));
}
