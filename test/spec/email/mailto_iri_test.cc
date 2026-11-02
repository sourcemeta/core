#include <sourcemeta/core/email.h>
#include <sourcemeta/core/test.h>

#include <string>

// RFC 6068 §6.1: a URI for an ordinary individual mailing address
TEST(mailto_iri_rfc_ordinary_address) {
  const auto result{sourcemeta::core::mailto_iri("chris@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:chris@example.com");
}

// RFC 6068 §6.1: to indicate the address "gorby%kremvax@example.com" one
// would use <mailto:gorby%25kremvax@example.com>
TEST(mailto_iri_rfc_percent_in_local_part) {
  const auto result{sourcemeta::core::mailto_iri("gorby%kremvax@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:gorby%25kremvax@example.com");
}

// RFC 6068 §6.1: to indicate the address "unlikely?address@example.com" one
// would use <mailto:unlikely%3Faddress@example.com>
TEST(mailto_iri_rfc_question_mark_in_local_part) {
  const auto result{
      sourcemeta::core::mailto_iri("unlikely?address@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:unlikely%3Faddress@example.com");
}

// RFC 6068 §6.1: the 'mailto' URI to send mail to "Mike&family@example.org"
// is <mailto:Mike%26family@example.org>
TEST(mailto_iri_rfc_ampersand_in_local_part) {
  const auto result{sourcemeta::core::mailto_iri("Mike&family@example.org")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:Mike%26family@example.org");
}

// RFC 6068 §6.2: email address "not@me"@example.org corresponds to
// <mailto:%22not%40me%22@example.org>
TEST(mailto_iri_rfc_quoted_at_sign) {
  const auto result{sourcemeta::core::mailto_iri("\"not@me\"@example.org")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22not%40me%22@example.org");
}

// RFC 6068 §6.2: email address "oh\\no"@example.org corresponds to
// <mailto:%22oh%5C%5Cno%22@example.org>
TEST(mailto_iri_rfc_quoted_backslash_pairs) {
  const auto result{sourcemeta::core::mailto_iri(R"("oh\\no"@example.org)")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22oh%5C%5Cno%22@example.org");
}

// RFC 6068 §6.2: email address "\\\"it's\ ugly\\\""@example.org corresponds
// to <mailto:%22%5C%5C%5C%22it's%5C%20ugly%5C%5C%5C%22%22@example.org>
TEST(mailto_iri_rfc_quoted_ugly_address) {
  const auto result{
      sourcemeta::core::mailto_iri(R"("\\\"it's\ ugly\\\""@example.org)")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            "mailto:%22%5C%5C%5C%22it's%5C%20ugly%5C%5C%5C%22%22@example.org");
}

// RFC 6068 §5: '+' characters MAY be encoded, so the canonical spelling
// keeps the subaddress form <bill+ietf@example.org> raw
TEST(mailto_iri_preserves_plus) {
  const auto result{sourcemeta::core::mailto_iri("bill+ietf@example.org")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:bill+ietf@example.org");
}

// RFC 5321 §4.1.2: Dot-string = Atom *("." Atom), "." passes through as
// RFC 3986 §2.3 unreserved
TEST(mailto_iri_preserves_dots) {
  const auto result{
      sourcemeta::core::mailto_iri("first.middle.last@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:first.middle.last@example.com");
}

// RFC 3986 §6.2.3: "mailto:Joe@Example.COM" is equivalent to
// "mailto:Joe@example.com", so the canonical spelling lowercases the domain
// name
TEST(mailto_iri_rfc_case_normalization_example) {
  const auto result{sourcemeta::core::mailto_iri("Joe@Example.COM")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:Joe@example.com");
}

// RFC 5321 §2.4: the local-part of a mailbox MUST BE treated as case
// sensitive, while mailbox domains follow normal DNS rules and are hence
// not case sensitive
TEST(mailto_iri_lowercases_domain_name_only) {
  const auto result{sourcemeta::core::mailto_iri("John.Doe@Example.COM")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:John.Doe@example.com");
}

// RFC 5321 §2.4: for some hosts, the user "smith" is different from the
// user "Smith"
TEST(mailto_iri_preserves_local_part_case) {
  const auto result{sourcemeta::core::mailto_iri("Smith@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:Smith@example.com");
}

// RFC 3986 §6.2.3 licenses case normalization for an Internet hostname
// subcomponent, and an address literal is not a DNS name, so its spelling
// is preserved
TEST(mailto_iri_preserves_address_literal_case) {
  const auto result{sourcemeta::core::mailto_iri("user@[IPv6:2001:DB8::1]")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:user@%5BIPv6:2001:DB8::1%5D");
}

// RFC 3986 §2.2: "!" is a sub-delim that RFC 6068 §2 does not reserve
TEST(mailto_iri_preserves_bang) {
  const auto result{sourcemeta::core::mailto_iri("a!b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a!b@example.com");
}

// RFC 3986 §2.2: "$" is a sub-delim that RFC 6068 §2 does not reserve
TEST(mailto_iri_preserves_dollar) {
  const auto result{sourcemeta::core::mailto_iri("a$b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a$b@example.com");
}

// RFC 3986 §2.2: "'" is a sub-delim that RFC 6068 §2 does not reserve
TEST(mailto_iri_preserves_apostrophe) {
  const auto result{sourcemeta::core::mailto_iri("a'b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a'b@example.com");
}

// RFC 3986 §2.2: "*" is a sub-delim that RFC 6068 §2 does not reserve
TEST(mailto_iri_preserves_asterisk) {
  const auto result{sourcemeta::core::mailto_iri("a*b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a*b@example.com");
}

// RFC 3986 §2.3: "-" and "_" and "~" are unreserved
TEST(mailto_iri_preserves_unreserved_symbols) {
  const auto result{sourcemeta::core::mailto_iri("a-b_c~d@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a-b_c~d@example.com");
}

// RFC 6068 §2: "#" is a gen-delim other than "@" and ":" and MUST be
// percent-encoded, otherwise the fragment would swallow the address tail
TEST(mailto_iri_encodes_hash) {
  const auto result{sourcemeta::core::mailto_iri("a#b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a%23b@example.com");
}

// RFC 6068 §2: "%" MUST be percent-encoded because it is used for
// percent-encoding itself
TEST(mailto_iri_encodes_percent) {
  const auto result{sourcemeta::core::mailto_iri("50%off@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:50%25off@example.com");
}

// RFC 6068 §2: "/" is a gen-delim other than "@" and ":" and MUST be
// percent-encoded
TEST(mailto_iri_encodes_slash) {
  const auto result{sourcemeta::core::mailto_iri("a/b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a%2Fb@example.com");
}

// RFC 6068 §2: "=" is one of the reserved sub-delims and MUST be
// percent-encoded
TEST(mailto_iri_encodes_equals) {
  const auto result{sourcemeta::core::mailto_iri("a=b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a%3Db@example.com");
}

// RFC 3986 §2: "^" cannot appear in a URI, so RFC 6068 §2 requires it
// percent-encoded
TEST(mailto_iri_encodes_caret) {
  const auto result{sourcemeta::core::mailto_iri("a^b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a%5Eb@example.com");
}

// RFC 3986 §2: "`" cannot appear in a URI, so RFC 6068 §2 requires it
// percent-encoded
TEST(mailto_iri_encodes_backtick) {
  const auto result{sourcemeta::core::mailto_iri("a`b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a%60b@example.com");
}

// RFC 3986 §2: "{" cannot appear in a URI, so RFC 6068 §2 requires it
// percent-encoded
TEST(mailto_iri_encodes_open_brace) {
  const auto result{sourcemeta::core::mailto_iri("a{b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a%7Bb@example.com");
}

// RFC 3986 §2: "}" cannot appear in a URI, so RFC 6068 §2 requires it
// percent-encoded
TEST(mailto_iri_encodes_close_brace) {
  const auto result{sourcemeta::core::mailto_iri("a}b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a%7Db@example.com");
}

// RFC 3986 §2: "|" cannot appear in a URI, so RFC 6068 §2 requires it
// percent-encoded
TEST(mailto_iri_encodes_pipe) {
  const auto result{sourcemeta::core::mailto_iri("a|b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:a%7Cb@example.com");
}

// RFC 5321 §4.1.2: Quoted-string = DQUOTE *QcontentSMTP DQUOTE, the DQUOTE
// cannot appear in a URI and is percent-encoded
TEST(mailto_iri_encodes_simple_quoted_local_part) {
  const auto result{sourcemeta::core::mailto_iri("\"abc\"@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22abc%22@example.com");
}

// RFC 6068 §5: when producing 'mailto' URIs, all spaces SHOULD be encoded
// as %20
TEST(mailto_iri_encodes_quoted_space) {
  const auto result{sourcemeta::core::mailto_iri("\"a b\"@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22a%20b%22@example.com");
}

// RFC 6068 §2: the "to" production takes "," as the address list separator,
// so a literal comma is percent-encoded
TEST(mailto_iri_encodes_quoted_comma) {
  const auto result{sourcemeta::core::mailto_iri("\"a,b\"@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22a%2Cb%22@example.com");
}

// RFC 6068 §2: ";" is one of the reserved sub-delims and MUST be
// percent-encoded
TEST(mailto_iri_encodes_quoted_semicolon) {
  const auto result{sourcemeta::core::mailto_iri("\"a;b\"@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22a%3Bb%22@example.com");
}

// RFC 6068 §2: the gen-delims "@" and ":" are exempt from mandatory
// encoding, and ":" carries no structure inside the address, so it stays raw
TEST(mailto_iri_preserves_quoted_colon) {
  const auto result{sourcemeta::core::mailto_iri("\"a:b\"@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22a:b%22@example.com");
}

// RFC 3986 §2.2: "(" and ")" are sub-delims that RFC 6068 §2 does not
// reserve
TEST(mailto_iri_preserves_quoted_parentheses) {
  const auto result{sourcemeta::core::mailto_iri("\"(ab)\"@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22(ab)%22@example.com");
}

// RFC 3986 §2: "<" and ">" cannot appear in a URI and are percent-encoded
TEST(mailto_iri_encodes_quoted_angle_brackets) {
  const auto result{sourcemeta::core::mailto_iri("\"<ab>\"@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22%3Cab%3E%22@example.com");
}

// RFC 5321 §4.1.2: quoted-pairSMTP = %d92 %d32-126, the backslash cannot
// appear in a URI and is percent-encoded
TEST(mailto_iri_encodes_quoted_pair) {
  const auto result{sourcemeta::core::mailto_iri(R"("\a"@example.com)")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:%22%5Ca%22@example.com");
}

// RFC 5321 §4.1.2: sub-domain = Let-dig [Ldh-str Let-dig], hyphens and
// digits pass through untouched
TEST(mailto_iri_preserves_hostname_hyphen_digits) {
  const auto result{sourcemeta::core::mailto_iri("user@ex-1.example")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:user@ex-1.example");
}

// RFC 5890 §2.3.2.1: an A-label domain is plain LDH ASCII and passes
// through untouched
TEST(mailto_iri_preserves_ace_domain_label) {
  const auto result{
      sourcemeta::core::mailto_iri("user@xn--mnchen-3ya.example")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:user@xn--mnchen-3ya.example");
}

// RFC 6068 §2: "[" and "]" are gen-delims other than "@" and ":" and MUST
// be percent-encoded, so an IPv4 address literal is encoded
TEST(mailto_iri_encodes_ipv4_address_literal) {
  const auto result{sourcemeta::core::mailto_iri("user@[192.0.2.1]")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:user@%5B192.0.2.1%5D");
}

// RFC 6068 §2: only the brackets of an IPv6 address literal need encoding
// because ":" is exempt from mandatory encoding
TEST(mailto_iri_encodes_ipv6_address_literal) {
  const auto result{sourcemeta::core::mailto_iri("user@[IPv6:2001:db8::1]")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "mailto:user@%5BIPv6:2001:db8::1%5D");
}

// RFC 5321 §4.1.3: a Standardized-tag has to be registered with IANA before
// being used, and only the IPv6 tag is, so a literal under any other tag is
// not a Mailbox to render
TEST(mailto_iri_rejects_general_address_literal) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("user@[foo:bar/baz]").has_value());
}

// RFC 5321 §4.1.2: Mailbox = Local-part "@" ( Domain / address-literal ),
// the empty string is not a Mailbox
TEST(mailto_iri_rejects_empty) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("").has_value());
}

// RFC 5321 §4.1.2: a Mailbox requires the "@" separator
TEST(mailto_iri_rejects_missing_at_sign) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("plain").has_value());
}

// RFC 5321 §4.1.2: Local-part cannot be empty
TEST(mailto_iri_rejects_missing_local_part) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("@example.com").has_value());
}

// RFC 5321 §4.1.2: Domain cannot be empty
TEST(mailto_iri_rejects_missing_domain) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("a@").has_value());
}

// RFC 5321 §4.1.2: atext does not include SP, so an unquoted space is
// invalid
TEST(mailto_iri_rejects_unquoted_space) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("a b@example.com").has_value());
}

// RFC 5321 §4.1.2: Dot-string = Atom *("." Atom) forbids empty atoms
TEST(mailto_iri_rejects_consecutive_dots) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("a..b@example.com").has_value());
}

// RFC 5321 §4.1.2: Dot-string cannot start with "."
TEST(mailto_iri_rejects_leading_dot) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri(".a@example.com").has_value());
}

// RFC 5321 §4.1.2: Quoted-string requires the closing DQUOTE
TEST(mailto_iri_rejects_unterminated_quoted_string) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("\"abc@example.com").has_value());
}

// RFC 5321 §4.1.2: atext is ASCII, so a non-ASCII local part is outside the
// Mailbox grammar
TEST(mailto_iri_rejects_non_ascii_local_part) {
  EXPECT_FALSE(
      sourcemeta::core::mailto_iri("caf\xC3\xA9@example.com").has_value());
}

// RFC 5321 §4.1.2: Domain is ASCII LDH, so a non-ASCII domain is outside
// the Mailbox grammar
TEST(mailto_iri_rejects_non_ascii_domain) {
  EXPECT_FALSE(
      sourcemeta::core::mailto_iri("user@caf\xC3\xA9.example").has_value());
}

// RFC 5321 §4.1.3: Snum is limited to the range 0 through 255
TEST(mailto_iri_rejects_invalid_ipv4_address_literal) {
  EXPECT_FALSE(sourcemeta::core::mailto_iri("a@[300.1.1.1]").has_value());
}

// RFC 5321 §4.5.3.1.1: Local-part octet limit is 64
TEST(mailto_iri_rejects_local_part_over_limit) {
  EXPECT_FALSE(
      sourcemeta::core::mailto_iri(std::string(65, 'a') + "@example.com")
          .has_value());
}

// RFC 5321 §4.5.3.1.3: a path is at most 256 octets including the enclosing
// angle brackets, so the mailbox it carries is at most 254
TEST(mailto_iri_rejects_mailbox_over_limit) {
  EXPECT_FALSE(
      sourcemeta::core::mailto_iri(std::string(250, 'a') + "@example.com")
          .has_value());
}
