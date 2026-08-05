#include <sourcemeta/core/email.h>
#include <sourcemeta/core/test.h>

// RFC 7565 §4: the account "foobar" at "status.example.net" is expressed as
// "acct:foobar@status.example.net"
TEST(acct_iri_rfc_plain_account) {
  const auto result{sourcemeta::core::acct_iri("foobar@status.example.net")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:foobar@status.example.net");
}

// RFC 7565 §4: registering with the email address "juliet@capulet.example"
// yields "acct:juliet%40capulet.example@shoppingsite.example"
TEST(acct_iri_rfc_email_as_account_name) {
  const auto result{sourcemeta::core::acct_iri(
      "juliet@capulet.example@shoppingsite.example")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            "acct:juliet%40capulet.example@shoppingsite.example");
}

// RFC 7565 §7: the userpart splits at the last "@", so every earlier "@" is
// part of the account name and percent-encoded
TEST(acct_iri_encodes_every_inner_at_sign) {
  const auto result{sourcemeta::core::acct_iri("a@b@c@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%40b%40c@example.com");
}

// RFC 3986 §2.3: unreserved characters pass through untouched
TEST(acct_iri_preserves_unreserved_symbols) {
  const auto result{sourcemeta::core::acct_iri("a-b.c_d~e@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a-b.c_d~e@example.com");
}

// RFC 7565 §7: userpart admits the full RFC 3986 §2.2 sub-delims set
TEST(acct_iri_preserves_all_sub_delims) {
  const auto result{sourcemeta::core::acct_iri("a!$&'()*+,;=b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a!$&'()*+,;=b@example.com");
}

// RFC 7565 §7: "&" is a sub-delim and stays raw in a userpart
TEST(acct_iri_preserves_ampersand) {
  const auto result{sourcemeta::core::acct_iri("a&b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a&b@example.com");
}

// RFC 7565 §7: ";" is a sub-delim and stays raw in a userpart
TEST(acct_iri_preserves_semicolon) {
  const auto result{sourcemeta::core::acct_iri("a;b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a;b@example.com");
}

// RFC 7565 §7: "=" is a sub-delim and stays raw in a userpart
TEST(acct_iri_preserves_equals) {
  const auto result{sourcemeta::core::acct_iri("a=b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a=b@example.com");
}

// RFC 7565 §7: "," is a sub-delim and stays raw in a userpart
TEST(acct_iri_preserves_comma) {
  const auto result{sourcemeta::core::acct_iri("a,b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a,b@example.com");
}

// RFC 3986 §6.2.2.1: only the scheme and host are case-insensitive, so the
// userpart case is preserved
TEST(acct_iri_preserves_userpart_case) {
  const auto result{sourcemeta::core::acct_iri("FooBar@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:FooBar@example.com");
}

// RFC 7565 §4: comparison uses RFC 3986 §6.2.2.1 case normalization, which
// lowercases the host
TEST(acct_iri_lowercases_host) {
  const auto result{sourcemeta::core::acct_iri("foobar@Status.Example.NET")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:foobar@status.example.net");
}

// RFC 7565 §7: "/" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_slash) {
  const auto result{sourcemeta::core::acct_iri("a/b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%2Fb@example.com");
}

// RFC 7565 §7: ":" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_colon) {
  const auto result{sourcemeta::core::acct_iri("a:b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%3Ab@example.com");
}

// RFC 7565 §7: "?" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_question_mark) {
  const auto result{sourcemeta::core::acct_iri("a?b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%3Fb@example.com");
}

// RFC 7565 §7: "#" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_hash) {
  const auto result{sourcemeta::core::acct_iri("a#b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%23b@example.com");
}

// RFC 7565 §7: "%" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_percent) {
  const auto result{sourcemeta::core::acct_iri("a%b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%25b@example.com");
}

// RFC 7565 §7: "[" and "]" are not unreserved nor sub-delims, so they are
// percent-encoded
TEST(acct_iri_encodes_square_brackets) {
  const auto result{sourcemeta::core::acct_iri("a[b]c@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%5Bb%5Dc@example.com");
}

// RFC 7565 §7: "\" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_backslash) {
  const auto result{sourcemeta::core::acct_iri("a\\b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%5Cb@example.com");
}

// RFC 7565 §7: the DQUOTE is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_double_quote) {
  const auto result{sourcemeta::core::acct_iri("a\"b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%22b@example.com");
}

// RFC 7565 §7: "^" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_caret) {
  const auto result{sourcemeta::core::acct_iri("a^b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%5Eb@example.com");
}

// RFC 7565 §7: "`" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_backtick) {
  const auto result{sourcemeta::core::acct_iri("a`b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%60b@example.com");
}

// RFC 7565 §7: "{" and "}" are not unreserved nor sub-delims, so they are
// percent-encoded
TEST(acct_iri_encodes_braces) {
  const auto result{sourcemeta::core::acct_iri("a{b}c@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%7Bb%7Dc@example.com");
}

// RFC 7565 §7: "|" is not unreserved nor a sub-delim, so it is
// percent-encoded
TEST(acct_iri_encodes_pipe) {
  const auto result{sourcemeta::core::acct_iri("a|b@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%7Cb@example.com");
}

// RFC 7565 §7: "<" and ">" are not unreserved nor sub-delims, so they are
// percent-encoded
TEST(acct_iri_encodes_angle_brackets) {
  const auto result{sourcemeta::core::acct_iri("a<b>c@example.com")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:a%3Cb%3Ec@example.com");
}

// RFC 7565 §7: acctURI requires both a userpart and a host
TEST(acct_iri_rejects_empty) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("").has_value());
}

// RFC 7565 §7: acctURI requires the "@" separator
TEST(acct_iri_rejects_missing_at_sign) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("foobar").has_value());
}

// RFC 7565 §7: userpart is one or more characters
TEST(acct_iri_rejects_empty_userpart) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("@example.com").has_value());
}

// RFC 7565 §7: host cannot be empty
TEST(acct_iri_rejects_empty_host) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("foobar@").has_value());
}

// RFC 7564 §9.14: spaces are disallowed in the PRECIS IdentifierClass that
// RFC 7565 §6 imposes on the userpart
TEST(acct_iri_rejects_space_in_userpart) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("foo bar@example.com").has_value());
}

// RFC 7564 §9.11: the PRECIS IdentifierClass ASCII repertoire is %x21-7E,
// which excludes control characters
TEST(acct_iri_rejects_control_character_in_userpart) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("foo\tbar@example.com").has_value());
}

// RFC 7565 §6: a non-ASCII userpart requires PRECIS IdentifierClass
// preparation, which is outside the ASCII account form accepted here
TEST(acct_iri_rejects_non_ascii_userpart) {
  EXPECT_FALSE(
      sourcemeta::core::acct_iri("caf\xC3\xA9@example.com").has_value());
}

// RFC 4343: DNS names compare case-insensitively, so an A-label spelled
// with uppercase Punycode names the same host as its lowercase form and
// canonicalizes to it
TEST(acct_iri_lowercases_uppercase_ace_host) {
  const auto result{sourcemeta::core::acct_iri("user@XN--MNCHEN-3YA.example")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "acct:user@xn--mnchen-3ya.example");
}

// RFC 7565 §4: the host portion is a DNS domain name, and underscores are
// not in the hostname alphabet
TEST(acct_iri_rejects_underscore_in_host) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("user@ex_ample.com").has_value());
}

// RFC 7565 §4: the host portion is a DNS domain name, and a label cannot
// start with a hyphen
TEST(acct_iri_rejects_leading_hyphen_host_label) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("user@-bad.example").has_value());
}

// RFC 7565 §4: the host portion is a DNS domain name, and empty labels are
// invalid
TEST(acct_iri_rejects_empty_host_label) {
  EXPECT_FALSE(sourcemeta::core::acct_iri("user@ex..ample.com").has_value());
}
