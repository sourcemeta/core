#include <sourcemeta/core/http_tracecontext.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(the_example_from_the_specification) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(
      "rojo=00f067aa0ba902b7,congo=t61rcWkgMzE"));
}

TEST(a_single_member) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("congo=t61rcWkgMzE"));
}

// "Empty and whitespace-only list members are allowed. Vendors MUST accept
// empty tracestate headers but SHOULD avoid sending them". Baggage forbids an
// empty member and an empty value outright
// https://www.w3.org/TR/trace-context/#tracestate-header-field-values
TEST(an_empty_value_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(""));
}

TEST(a_whitespace_only_member_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(" , "));
}

// A member made of nothing but padding is one of the whitespace-only members
// the rule allows, with or without a comma to sit beside
// https://www.w3.org/TR/trace-context/#tracestate-header-field-values
TEST(a_lone_whitespace_member_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(" "));
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("\t"));
}

TEST(a_trailing_comma_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("foo=1,"));
}

// Spaces and horizontal tabs surround a member rather than sitting inside one
// https://www.w3.org/TR/trace-context/#list
TEST(whitespace_around_a_comma_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("foo=1 , \tbar=2"));
}

// "Spaces and horizontal tabs surrounding list-members are ignored", which
// covers the first and the last as much as any other, so padding a value on
// either side leaves it well formed
// https://www.w3.org/TR/trace-context/#tracestate-header-field-values
TEST(whitespace_before_the_first_member_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(" foo=1"));
}

TEST(whitespace_after_the_last_member_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("foo=1\t"));
}

// There is no optional whitespace inside a member, so an equals sign with
// spaces around it is not a separator. Baggage is the opposite and permits it
// https://www.w3.org/TR/trace-context/#list
TEST(whitespace_inside_a_member_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("foo = 1"));
}

// From the upstream corpus at vendor/w3c-trace-context/test/tracecontext
TEST(a_member_without_an_equals_sign_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("foobarbaz"));
}

// "There can be a maximum of 32 list-members in a list"
// https://www.w3.org/TR/trace-context/#tracestate-header-field-values
TEST(thirty_two_members_are_accepted) {
  std::string value;
  for (auto index = 0; index < 32; ++index) {
    value += (index > 0 ? "," : "") + std::string{"k"} + std::to_string(index) +
             "=v";
  }
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(value));
}

TEST(thirty_three_members_are_rejected) {
  std::string value;
  for (auto index = 0; index < 33; ++index) {
    value += (index > 0 ? "," : "") + std::string{"k"} + std::to_string(index) +
             "=v";
  }
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate(value));
}

// An empty member counts toward the limit, being a list-member of its own
// https://www.w3.org/TR/trace-context/#list
TEST(empty_members_count_toward_the_limit) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate(std::string(33, ',')));
}

// A simple key starts with a lowercase letter and admits only lowercase
// letters, digits, underscore, dash, asterisk and forward slash
// https://www.w3.org/TR/trace-context/#key
TEST(an_uppercase_key_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("FOO=1"));
}

TEST(a_key_starting_with_a_digit_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("1foo=1"));
}

// From the upstream corpus: a dot is not in the permitted set
TEST(a_dotted_key_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("special.vendor=1"));
}

TEST(the_permitted_key_characters_are_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("a0_-*/=1"));
}

TEST(an_empty_key_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("=1"));
}

TEST(a_key_of_two_hundred_and_fifty_six_characters_is_accepted) {
  EXPECT_TRUE(
      sourcemeta::core::http_is_tracestate(std::string(256, 'z') + "=1"));
}

TEST(a_key_of_two_hundred_and_fifty_seven_characters_is_rejected) {
  EXPECT_FALSE(
      sourcemeta::core::http_is_tracestate(std::string(257, 'z') + "=1"));
}

// A tenant is at most 241 characters, a system at most 14, and only the tenant
// may start with a digit
// https://www.w3.org/TR/trace-context/#key
TEST(a_multi_tenant_key_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("1tenant@system=1"));
}

TEST(a_multi_tenant_key_with_an_empty_tenant_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("@vendor=1"));
}

TEST(a_system_starting_with_a_digit_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("tenant@1system=1"));
}

TEST(a_tenant_of_two_hundred_and_forty_one_characters_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(std::string(241, 'z') +
                                                   "@system=1"));
}

TEST(a_system_of_fourteen_characters_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(
      "tenant@" + std::string(14, 'z') + "=1"));
}

TEST(a_tenant_of_two_hundred_and_forty_two_characters_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate(std::string(242, 'z') +
                                                    "@system=1"));
}

TEST(a_system_of_fifteen_characters_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate(
      "tenant@" + std::string(15, 'z') + "=1"));
}

// A value ends in a mandatory non-blank character, so it can be neither empty
// nor space terminated. Baggage permits an empty value
// https://www.w3.org/TR/trace-context/#value
TEST(an_empty_member_value_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("foo="));
}

// A trailing space is stripped as padding before the value is read, so it is
// the mandatory non-blank character that ends the value either way
// https://www.w3.org/TR/trace-context/#value
TEST(a_space_terminated_value_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("foo=bar "));
}

TEST(a_value_containing_a_space_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("foo=bar baz"));
}

// The two ranges of the non-blank production leave out the comma at %x2C and
// the equals at %x3D
// https://www.w3.org/TR/trace-context/#value
TEST(a_value_containing_an_equals_sign_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("foo=bar=baz"));
}

TEST(a_value_of_two_hundred_and_fifty_six_characters_is_accepted) {
  EXPECT_TRUE(
      sourcemeta::core::http_is_tracestate("foo=" + std::string(256, 'x')));
}

TEST(a_value_of_two_hundred_and_fifty_seven_characters_is_rejected) {
  EXPECT_FALSE(
      sourcemeta::core::http_is_tracestate("foo=" + std::string(257, 'x')));
}

TEST(a_control_character_in_a_value_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("foo=bar\nbaz"));
}

// A repeated key is rejected, which is stricter than the grammar. The grammar
// permits one and the only guidance for a reader is non-normative, but a key
// records a position in the trace and "Only one entry per key is allowed", so
// a repeat is a producer that appended where it should have overwritten.
// Baggage is the opposite, where duplicates are legal and ordered
// https://www.w3.org/TR/trace-context/#combined-header-value
TEST(a_duplicate_key_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("a=b,a=c"));
}
