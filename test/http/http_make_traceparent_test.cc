#include <sourcemeta/core/http_tracecontext.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

TEST(the_value_is_well_formed) {
  const auto value{sourcemeta::core::http_make_traceparent(true)};
  EXPECT_EQ(value.size(), 55);
  EXPECT_TRUE(sourcemeta::core::http_is_traceparent(
      std::string_view{value.data(), value.size()}));
}

// A value is written at the revision this library implements rather than at
// any later one
// https://www.w3.org/TR/trace-context/#versioning-of-traceparent
TEST(the_revision_is_the_lowest) {
  const auto value{sourcemeta::core::http_make_traceparent(false)};
  const std::string_view view{value.data(), value.size()};
  EXPECT_TRUE(view.starts_with("00-"));
}

TEST(the_sampled_flag_is_set_when_asked) {
  const auto value{sourcemeta::core::http_make_traceparent(true)};
  const std::string_view view{value.data(), value.size()};
  EXPECT_EQ(view.substr(53), "01");
}

// Only the lowest bit carries a meaning, and "Vendors MUST set those to zero"
// covers the rest, so no other bit is ever written
// https://www.w3.org/TR/trace-context/#other-flags
TEST(no_reserved_flag_bit_is_written) {
  const auto value{sourcemeta::core::http_make_traceparent(false)};
  const std::string_view view{value.data(), value.size()};
  EXPECT_EQ(view.substr(53), "00");
}

// "All bytes as zero (00000000000000000000000000000000) is considered an
// invalid value", which is what the validator above would refuse
// https://www.w3.org/TR/trace-context/#trace-id
TEST(neither_identifier_is_all_zeroes) {
  const auto value{sourcemeta::core::http_make_traceparent(true)};
  const std::string_view view{value.data(), value.size()};
  EXPECT_NE(view.substr(3, 32), "00000000000000000000000000000000");
  EXPECT_NE(view.substr(36, 16), "0000000000000000");
}

// Every property is regenerated, so two traces do not share an identifier
// https://www.w3.org/TR/trace-context/#mutating-the-traceparent-field
TEST(two_traces_differ) {
  const auto first{sourcemeta::core::http_make_traceparent(true)};
  const auto second{sourcemeta::core::http_make_traceparent(true)};
  const std::string_view one{first.data(), first.size()};
  const std::string_view two{second.data(), second.size()};
  EXPECT_NE(one.substr(3, 32), two.substr(3, 32));
  EXPECT_NE(one.substr(36, 16), two.substr(36, 16));
}
