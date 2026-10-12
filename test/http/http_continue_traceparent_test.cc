#include <sourcemeta/core/http_tracecontext.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

TEST(the_value_is_well_formed) {
  const auto value{sourcemeta::core::http_continue_traceparent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01", true)};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value->size(), 55);
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      std::string_view{value->data(), value->size()}));
}

// The trace carries on, so the identifier that arrived is the one sent onward
// https://www.w3.org/TR/trace-context/#mutating-the-traceparent-field
TEST(the_trace_identifier_is_kept) {
  const auto value{sourcemeta::core::http_continue_traceparent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01", true)};
  EXPECT_TRUE(value.has_value());
  const std::string_view view{value->data(), value->size()};
  EXPECT_EQ(view.substr(3, 32), "0af7651916cd43dd8448eb211c80319c");
}

// The operation is this one rather than the caller's, which the specification
// calls "the most typical" mutation
// https://www.w3.org/TR/trace-context/#mutating-the-traceparent-field
TEST(the_operation_identifier_is_new) {
  const auto value{sourcemeta::core::http_continue_traceparent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01", true)};
  EXPECT_TRUE(value.has_value());
  const std::string_view view{value->data(), value->size()};
  EXPECT_NE(view.substr(36, 16), "b7ad6b7169203331");
}

TEST(the_sampling_decision_may_be_turned_off) {
  const auto value{sourcemeta::core::http_continue_traceparent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01", false)};
  EXPECT_TRUE(value.has_value());
  const std::string_view view{value->data(), value->size()};
  EXPECT_EQ(view.substr(53), "00");
}

TEST(the_sampling_decision_may_be_turned_on) {
  const auto value{sourcemeta::core::http_continue_traceparent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-00", true)};
  EXPECT_TRUE(value.has_value());
  const std::string_view view{value->data(), value->size()};
  EXPECT_EQ(view.substr(53), "01");
}

// The decision that arrived is carried over by reading it off the parsed flags
// https://www.w3.org/TR/trace-context/#sampled-flag
TEST(the_decision_that_arrived_can_be_passed_on) {
  const std::string_view incoming{
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"};
  const auto parent{sourcemeta::core::http_parse_traceparent(incoming)};
  EXPECT_TRUE(parent.has_value());
  const auto value{sourcemeta::core::http_continue_traceparent(
      incoming, sourcemeta::core::http_traceparent_sampled(parent->flags))};
  EXPECT_TRUE(value.has_value());
  const std::string_view view{value->data(), value->size()};
  EXPECT_EQ(view.substr(53), "01");
}

// A value read at a later revision is written back at the one this library
// implements, which is the downgrade a reader owes, and what that value
// appended past the flags goes with it
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(a_later_revision_is_written_back_at_the_lowest) {
  const auto value{sourcemeta::core::http_continue_traceparent(
      "cc-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01-future", true)};
  EXPECT_TRUE(value.has_value());
  const std::string_view view{value->data(), value->size()};
  EXPECT_EQ(view.size(), 55);
  EXPECT_TRUE(view.starts_with("00-"));
  EXPECT_EQ(view.substr(3, 32), "0af7651916cd43dd8448eb211c80319c");
}

// Two hops from the same arrival are distinct operations within one trace
// https://www.w3.org/TR/trace-context/#mutating-the-traceparent-field
TEST(two_hops_share_the_trace_and_differ_in_the_operation) {
  const std::string_view incoming{
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"};
  const auto first{sourcemeta::core::http_continue_traceparent(incoming, true)};
  const auto second{
      sourcemeta::core::http_continue_traceparent(incoming, true)};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  const std::string_view one{first->data(), first->size()};
  const std::string_view two{second->data(), second->size()};
  EXPECT_EQ(one.substr(3, 32), two.substr(3, 32));
  EXPECT_NE(one.substr(36, 16), two.substr(36, 16));
}

// A malformed value leaves no trace to carry on from
TEST(a_malformed_value_yields_nothing) {
  EXPECT_FALSE(
      sourcemeta::core::http_continue_traceparent(
          "ff-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01", true)
          .has_value());
}

TEST(an_all_zero_trace_identifier_yields_nothing) {
  EXPECT_FALSE(
      sourcemeta::core::http_continue_traceparent(
          "00-00000000000000000000000000000000-b7ad6b7169203331-01", true)
          .has_value());
}

TEST(an_empty_value_yields_nothing) {
  EXPECT_FALSE(
      sourcemeta::core::http_continue_traceparent("", true).has_value());
}
