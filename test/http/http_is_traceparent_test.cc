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

// Every flag bit beyond the lowest is reserved and a reader is told to mask
// rather than compare, so no value of the byte is refused
// https://www.w3.org/TR/trace-context/#trace-flags
TEST(any_flag_byte_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-ff"));
}

// Version ff is excluded by the grammar's own comment on the production
// https://www.w3.org/TR/trace-context/#version
TEST(version_ff_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "ff-12345678901234567890123456789012-1234567890123456-00"));
}

// A value of exactly 55 characters is well formed whatever its version, since
// the flags may be followed by "either the end of the string or a dash". These
// four cases come from the upstream corpus, whose permissive base parser
// accepts them, at vendor/w3c-trace-context/test/tracecontext
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(a_later_version_of_exactly_fifty_five_characters) {
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "01-12345678901234567890123456789012-1234567890123456-00"));
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      "02-12345678901234567890123456789012-1234567890123456-00"));
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

// The grammar admits only lowercase hexadecimal, and a reader "MUST ignore the
// traceparent when the parent-id is invalid (for example, if it contains
// non-lowercase hex characters)"
// https://www.w3.org/TR/trace-context/#parent-id
TEST(uppercase_hexadecimal_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-4BF92F3577B34DA6A3CE929D0E0E4736-00f067aa0ba902b7-01"));
}

TEST(a_non_hexadecimal_character_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e473g-00f067aa0ba902b7-01"));
}

// "When the version prefix cannot be parsed (it's not 2 hex characters
// followed by a dash), the implementation should restart the trace"
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(a_non_hexadecimal_version_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_traceparent(
      "zz-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01"));
}

// These five length cases come from the upstream corpus
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

TEST(a_runtime_value_is_accepted) {
  const std::string value{
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"};
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(value));
}
