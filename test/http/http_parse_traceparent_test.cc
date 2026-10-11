#include <sourcemeta/core/http_tracecontext.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(the_example_from_the_specification) {
  const auto parent{sourcemeta::core::http_parse_traceparent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01")};
  EXPECT_TRUE(parent.has_value());
  EXPECT_EQ(parent->version, 0);
  EXPECT_EQ(parent->trace_id, "0af7651916cd43dd8448eb211c80319c");
  EXPECT_EQ(parent->parent_id, "b7ad6b7169203331");
  EXPECT_EQ(parent->flags, 1);
}

// The worked example in the specification spells out each field
// https://www.w3.org/TR/trace-context/#examples-of-http-traceparent-headers
TEST(the_sampled_example) {
  const auto parent{sourcemeta::core::http_parse_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01")};
  EXPECT_TRUE(parent.has_value());
  EXPECT_EQ(parent->trace_id, "4bf92f3577b34da6a3ce929d0e0e4736");
  EXPECT_EQ(parent->parent_id, "00f067aa0ba902b7");
  EXPECT_TRUE(sourcemeta::core::http_traceparent_sampled(parent->flags));
}

TEST(the_unsampled_example) {
  const auto parent{sourcemeta::core::http_parse_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-00")};
  EXPECT_TRUE(parent.has_value());
  EXPECT_EQ(parent->flags, 0);
  EXPECT_FALSE(sourcemeta::core::http_traceparent_sampled(parent->flags));
}

TEST(a_later_version_is_reported_as_itself) {
  const auto parent{sourcemeta::core::http_parse_traceparent(
      "cc-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01")};
  EXPECT_TRUE(parent.has_value());
  EXPECT_EQ(parent->version, 0xcc);
}

// A later version appends beyond the flags and a reader "MUST NOT parse or
// assume anything about unknown fields", so the four fields read the same
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(a_later_version_ignores_what_follows_the_flags) {
  const auto parent{sourcemeta::core::http_parse_traceparent(
      "cc-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01-future")};
  EXPECT_TRUE(parent.has_value());
  EXPECT_EQ(parent->trace_id, "4bf92f3577b34da6a3ce929d0e0e4736");
  EXPECT_EQ(parent->parent_id, "00f067aa0ba902b7");
  EXPECT_EQ(parent->flags, 1);
}

// Every bit above the lowest is reserved, so the byte is reported whole and
// the accessor masks for the one bit that is defined
// https://www.w3.org/TR/trace-context/#other-flags
TEST(a_reserved_flag_bit_is_reported_and_masked) {
  const auto parent{sourcemeta::core::http_parse_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-05")};
  EXPECT_TRUE(parent.has_value());
  EXPECT_EQ(parent->flags, 5);
  EXPECT_TRUE(sourcemeta::core::http_traceparent_sampled(parent->flags));
}

TEST(a_reserved_bit_alone_is_not_sampled) {
  const auto parent{sourcemeta::core::http_parse_traceparent(
      "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-04")};
  EXPECT_TRUE(parent.has_value());
  EXPECT_EQ(parent->flags, 4);
  EXPECT_FALSE(sourcemeta::core::http_traceparent_sampled(parent->flags));
}

TEST(a_malformed_value_yields_nothing) {
  EXPECT_FALSE(sourcemeta::core::http_parse_traceparent(
                   "ff-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01")
                   .has_value());
}

TEST(an_empty_value_yields_nothing) {
  EXPECT_FALSE(sourcemeta::core::http_parse_traceparent("").has_value());
}

// The identifiers borrow the input rather than copying it
TEST(the_identifiers_borrow_the_input) {
  const std::string value{
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"};
  const auto parent{sourcemeta::core::http_parse_traceparent(value)};
  EXPECT_TRUE(parent.has_value());
  EXPECT_EQ(parent->trace_id.data(), value.data() + 3);
}

TEST(sampled_masks_rather_than_compares) {
  EXPECT_TRUE(sourcemeta::core::http_traceparent_sampled(0x01));
  EXPECT_TRUE(sourcemeta::core::http_traceparent_sampled(0xff));
  EXPECT_FALSE(sourcemeta::core::http_traceparent_sampled(0x00));
  EXPECT_FALSE(sourcemeta::core::http_traceparent_sampled(0xfe));
}
