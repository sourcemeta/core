#include <sourcemeta/core/http_tracecontext.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(the_example_from_the_specification) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"));
}

TEST(not_sampled) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-00"));
}

// "The behavior of other flags, such as (00000100) is not defined and is
// reserved for future use", and a reader is told to mask rather than compare,
// so no value of the byte is refused
// https://www.w3.org/TR/trace-context/#other-flags
// https://www.w3.org/TR/trace-context/#trace-flags
TEST(any_flag_byte_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-ff"));
}

// "Version ff is invalid"
// https://www.w3.org/TR/trace-context/#version
TEST(version_ff_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "ff-12345678901234567890123456789012-1234567890123456-00"));
}

// A version above the lowest is accepted, and at exactly 55 characters the
// flags end the value, which satisfies "either the end of the string or a
// dash". These three versions come from the upstream corpus, whose permissive
// base parser accepts each of them
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(version_01_of_exactly_fifty_five_characters) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-00"));
}

TEST(version_02_of_exactly_fifty_five_characters) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "02-12345678901234567890123456789012-1234567890123456-00"));
}

TEST(version_cc_of_exactly_fifty_five_characters) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "cc-12345678901234567890123456789012-1234567890123456-00"));
}

TEST(a_later_version_may_append_beyond_a_dash) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-00-anything"));
}

TEST(a_later_version_may_not_append_without_a_dash) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-0000"));
}

// Version 00 is the whole value, so it admits nothing after the flags
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(version_00_may_not_append) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-12345678901234567890123456789012-1234567890123456-00-anything"));
}

// "All bytes as zero (00000000000000000000000000000000) is considered an
// invalid value". The upstream corpus accepts these through its base parser
// and refuses them through the strict one, which is the behaviour here
// https://www.w3.org/TR/trace-context/#trace-id
TEST(an_all_zero_trace_id_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-00000000000000000000000000000000-1234567890123456-00"));
}

// https://www.w3.org/TR/trace-context/#parent-id
TEST(an_all_zero_parent_id_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-12345678901234567890123456789012-0000000000000000-00"));
}

TEST(an_identifier_of_one_non_zero_digit_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "00-00000000000000000000000000000001-0000000000000001-00"));
}

// The grammar admits only lowercase hexadecimal, and "If the trace-id value is
// invalid (for example if it contains non-allowed characters or all zeros),
// vendors MUST ignore the traceparent"
// https://www.w3.org/TR/trace-context/#trace-id
TEST(uppercase_hexadecimal_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-4BF92F3577B34DA6A3CE929D0E0E4736-00f067aa0ba902b7-01"));
}

TEST(a_non_hexadecimal_character_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e473g-00f067aa0ba902b7-01"));
}

// "When the version prefix cannot be parsed (it's not 2 hex characters
// followed by a dash (-)), the implementation should restart the trace"
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(a_non_hexadecimal_version_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "zz-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01"));
}

// These six length cases come from the upstream corpus
// https://www.w3.org/TR/trace-context/#traceparent-header-field-values
TEST(a_one_character_version_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "0-12345678901234567890123456789012-1234567890123456-00"));
}

TEST(a_three_character_version_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "000-12345678901234567890123456789012-1234567890123456-00"));
}

TEST(a_short_trace_id_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-1234567890123456789012345678901-1234567890123456-00"));
}

TEST(a_long_trace_id_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-123456789012345678901234567890123-1234567890123456-00"));
}

TEST(a_short_parent_id_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-12345678901234567890123456789012-123456789012345-00"));
}

// A parent id one character too long pushes the dash that ends it past where
// the flags begin, which is the only way to reach that dash test
TEST(a_long_parent_id_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-12345678901234567890123456789012-12345678901234567-00"));
}

TEST(an_empty_value_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(""));
}

TEST(a_truncated_value_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent("00-"));
}

TEST(a_missing_dash_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "0004bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01"));
}

// What a later version appends is left alone, since the rules for reading one
// enumerate three checks and no more and "Vendors MUST NOT parse or assume
// anything about unknown fields for this version", so refusing a value over
// those bytes would discard a trace the same section says to keep
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(an_appended_visible_field_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-00-cafe01"));
}

TEST(an_appended_space_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-00-a b"));
}

TEST(an_appended_control_character_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-00-a\nb"));
}

TEST(an_appended_byte_above_ascii_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-00-\xc3\xa9"));
}

TEST(a_bare_dash_after_the_flags_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-00-"));
}

// The version prefix is held to the same lowercase hexadecimal as every other
// field, so an uppercase one fails to parse
// https://www.w3.org/TR/trace-context/#version
TEST(an_uppercase_version_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "CC-12345678901234567890123456789012-1234567890123456-00"));
}

// "If the size of the header is shorter than 55 characters, the vendor should
// not parse the header and should restart the trace"
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(a_value_one_character_short_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-0"));
}
