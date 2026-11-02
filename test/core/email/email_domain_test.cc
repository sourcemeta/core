#include <sourcemeta/core/email.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

TEST(email_domain_ordinary_address) {
  EXPECT_EQ(sourcemeta::core::email_domain("user@example.com"), "example.com");
}

// RFC 5321 §4.1.2: Local-part = Dot-string / Quoted-string, and qtextSMTP
// admits the at sign, so the separator is not the first one in the string
TEST(email_domain_quoted_local_part_carrying_an_at_sign) {
  EXPECT_EQ(sourcemeta::core::email_domain("\"not@me\"@example.org"),
            "example.org");
}

// RFC 5321 §4.1.3: no address-literal carries an at sign, as the only tag the
// registry holds is the IPv6 one and no address spells one, so a literal that
// does is not a Mailbox and reports no domain at all
TEST(email_domain_address_literal_carrying_an_at_sign) {
  EXPECT_TRUE(sourcemeta::core::email_domain("user@[tag:a@b]").empty());
}

TEST(email_domain_ipv4_address_literal) {
  EXPECT_EQ(sourcemeta::core::email_domain("user@[192.168.1.1]"),
            "[192.168.1.1]");
}

// RFC 5321 §2.4: mailbox domains are not case sensitive, but the domain is
// reported as it was written, since the view borrows from the input
TEST(email_domain_preserves_the_spelling_of_the_domain) {
  EXPECT_EQ(sourcemeta::core::email_domain("user@Example.COM"), "Example.COM");
}

TEST(email_domain_borrows_from_the_input) {
  const std::string_view address{"user@example.com"};
  const auto result{sourcemeta::core::email_domain(address)};
  EXPECT_TRUE(result.data() == address.data() + 5);
}

TEST(email_domain_without_a_separator) {
  EXPECT_TRUE(sourcemeta::core::email_domain("plain").empty());
}

TEST(email_domain_empty_input) {
  EXPECT_TRUE(sourcemeta::core::email_domain("").empty());
}

TEST(email_domain_empty_local_part) {
  EXPECT_TRUE(sourcemeta::core::email_domain("@example.com").empty());
}

TEST(email_domain_empty_domain) {
  EXPECT_TRUE(sourcemeta::core::email_domain("user@").empty());
}

TEST(email_domain_invalid_domain) {
  EXPECT_TRUE(sourcemeta::core::email_domain("user@-example.com").empty());
}

// RFC 6531 §3.3 extends the mailbox grammar with non-ASCII, which this
// function does not accept, since only the ASCII grammar is in scope
TEST(email_domain_internationalized_address) {
  EXPECT_TRUE(
      sourcemeta::core::email_domain(
          "\xec\x8b\xa4\xeb\xa1\x80@\xec\x8b\xa4\xeb\xa1\x80.\xed\x85\x8c"
          "\xec\x8a\xa4\xed\x8a\xb8")
          .empty());
}
