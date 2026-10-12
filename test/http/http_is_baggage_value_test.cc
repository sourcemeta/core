#include <sourcemeta/core/http_baggage.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

TEST(a_plain_value) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value("alice"));
}

// The value production counts zero or more octets, so nothing at all is a
// value, where a tracestate value has to end in a mandatory non-blank
// character and so cannot be empty
// https://www.w3.org/TR/baggage/#value
// https://www.w3.org/TR/trace-context/#value
TEST(an_empty_value_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value(""));
}

// The permitted octets leave out whitespace, so a space has to be encoded
// https://www.w3.org/TR/baggage/#definition
TEST(a_space_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("DF 28"));
}

TEST(an_encoded_space_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value("DF%2028"));
}

// "The uppercase hexadecimal digits 'A' through 'F' are equivalent to the
// lowercase digits 'a' through 'f', respectively", so an escape is read in
// either case
// https://www.rfc-editor.org/rfc/rfc3986#section-2.1
TEST(a_lowercase_escape_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value("%3b%2c"));
}

TEST(an_uppercase_escape_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value("%3B%2C"));
}

// A comma is what separates members, so a value can only carry one encoded
// https://www.w3.org/TR/baggage/#definition
TEST(an_encoded_comma_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value("a%2Cb"));
}

TEST(a_null_byte_is_rejected) {
  EXPECT_FALSE(
      sourcemeta::core::http_is_baggage_value(std::string_view{"a\0b", 3}));
}

TEST(an_encoded_non_ascii_character_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value("Am%C3%A9lie"));
}

// The grammar's own comment lists what the octet ranges leave out, and the
// double quote and the backslash are two of them
// https://www.w3.org/TR/baggage/#definition
TEST(a_double_quote_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("say\"what"));
}

TEST(a_backslash_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("back\\slash"));
}

TEST(a_tab_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("a\tb"));
}

TEST(a_newline_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("a\nb"));
}

TEST(a_delete_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("a\x7f"
                                                       "b"));
}

TEST(a_byte_above_ascii_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("caf\xc3\xa9"));
}

// "Note, value MAY contain any number of the equal sign (U+003D) code points"
// https://www.w3.org/TR/baggage/#value
TEST(an_equals_sign_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value("a=b=c"));
}

// A percent is inside the permitted octet ranges, so the grammar alone would
// admit a bare one. This is stricter on the strength of the prose: "The
// percent code point (U+0025) MUST be percent-encoded"
// https://www.w3.org/TR/baggage/#value
TEST(a_bare_percent_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("100%"));
}

TEST(an_encoded_percent_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value("100%25"));
}

TEST(a_non_hexadecimal_escape_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("%ZZ"));
}

TEST(a_truncated_escape_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("%A"));
}

TEST(an_escape_at_the_end_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage_value("a%"));
}

// A value is not capped by the grammar, and the byte figure in the limits is
// about what a platform must propagate rather than what is well formed
// https://www.w3.org/TR/baggage/#limits
TEST(a_very_long_value_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage_value(std::string(9000, 'x')));
}
