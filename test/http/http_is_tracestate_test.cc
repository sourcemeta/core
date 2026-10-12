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
TEST(an_empty_field_value_is_accepted) {
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

// A member boundary is the one place a tab may appear, since the key and the
// value admit none between them
// https://www.w3.org/TR/trace-context/#list
TEST(whitespace_around_a_comma_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("foo=1 , \tbar=2"));
}

// "Spaces and horizontal tabs surrounding list-members are ignored", which is
// read as covering the first and the last as much as any other. The list
// production brackets only its inner boundaries, so this accepts a little
// more than the grammar writes down, and the two readings differ only here
// https://www.w3.org/TR/trace-context/#tracestate-header-field-values
// https://www.w3.org/TR/trace-context/#list
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

// From the reference parser's own tests, read at commit acab820b of
// https://github.com/w3c/trace-context
TEST(a_member_without_an_equals_sign_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("foobarbaz"));
}

// "There can be a maximum of 32 list-members in a list"
// https://www.w3.org/TR/trace-context/#tracestate-header-field-values
TEST(thirty_two_members_are_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(
      "k0=v,k1=v,k2=v,k3=v,k4=v,k5=v,k6=v,k7=v,k8=v,k9=v,k10=v,k11=v,k12=v,"
      "k13=v,k14=v,k15=v,k16=v,k17=v,k18=v,k19=v,k20=v,k21=v,k22=v,k23=v,"
      "k24=v,k25=v,k26=v,k27=v,k28=v,k29=v,k30=v,k31=v"));
}

TEST(thirty_three_members_are_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate(
      "k0=v,k1=v,k2=v,k3=v,k4=v,k5=v,k6=v,k7=v,k8=v,k9=v,k10=v,k11=v,k12=v,"
      "k13=v,k14=v,k15=v,k16=v,k17=v,k18=v,k19=v,k20=v,k21=v,k22=v,k23=v,"
      "k24=v,k25=v,k26=v,k27=v,k28=v,k29=v,k30=v,k31=v,k32=v"));
}

// An empty member counts toward the limit, being a list-member of its own, so
// the boundary sits at 31 commas for 32 members and 32 commas for 33
// https://www.w3.org/TR/trace-context/#list
TEST(thirty_two_empty_members_are_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(std::string(31, ',')));
}

TEST(thirty_three_empty_members_are_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate(std::string(32, ',')));
}

// A trailing comma adds an empty member, so it is what tips a list of 32 over
TEST(thirty_one_members_and_a_trailing_comma_are_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate(
      "k0=v,k1=v,k2=v,k3=v,k4=v,k5=v,k6=v,k7=v,k8=v,k9=v,k10=v,k11=v,k12=v,"
      "k13=v,k14=v,k15=v,k16=v,k17=v,k18=v,k19=v,k20=v,k21=v,k22=v,k23=v,"
      "k24=v,k25=v,k26=v,k27=v,k28=v,k29=v,k30=v,"));
}

TEST(thirty_two_members_and_a_trailing_comma_are_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate(
      "k0=v,k1=v,k2=v,k3=v,k4=v,k5=v,k6=v,k7=v,k8=v,k9=v,k10=v,k11=v,k12=v,"
      "k13=v,k14=v,k15=v,k16=v,k17=v,k18=v,k19=v,k20=v,k21=v,k22=v,k23=v,"
      "k24=v,k25=v,k26=v,k27=v,k28=v,k29=v,k30=v,k31=v,"));
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

// A dot is not in the permitted set, which the reference parser's own tests
// also cover
// https://github.com/w3c/trace-context/tree/acab820b
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

// A key joins a tenant and a system with one at sign, so a second one lands
// inside the system, where the at sign is not a permitted character. The
// reference parser accepts both of these, following a note the specification
// marks non-normative over the grammar it also states
// https://www.w3.org/TR/trace-context/#key
// https://github.com/w3c/trace-context/tree/acab820b
TEST(a_key_with_two_at_signs_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("tenant@system@extra=1"));
}

TEST(a_key_ending_in_an_at_sign_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("tenant@=1"));
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

// A trailing space is stripped as padding before the value is read, on the
// same reading of "Spaces and horizontal tabs surrounding list-members are
// ignored", so what ends the value is the non-blank character its own
// production requires
// https://www.w3.org/TR/trace-context/#tracestate-header-field-values
// https://www.w3.org/TR/trace-context/#value
TEST(a_space_terminated_value_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("foo=bar "));
}

TEST(a_value_containing_a_space_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_tracestate("foo=bar baz"));
}

// The non-blank production runs in three ranges, leaving out the comma at
// %x2C and the equals at %x3D
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

// A repeated key is rejected, which is stricter than the grammar, on the rule
// that "Only one entry per key is allowed because the entry represents that
// last position in the trace". A reader is offered the alternative of dropping
// just the repeated entry, but only in a section marked non-normative
// https://www.w3.org/TR/trace-context/#combined-header-value
// https://www.w3.org/TR/trace-context/#a-traceparent-is-received
TEST(a_duplicate_key_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_tracestate("a=b,a=c"));
}
