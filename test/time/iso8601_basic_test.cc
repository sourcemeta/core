#include <sourcemeta/core/test.h>

#include <chrono>
#include <ctime>
#include <string>
#include <string_view>

#include <sourcemeta/core/time.h>

namespace {
auto make_point(const int year, const int month, const int day, const int hour,
                const int minute, const int second)
    -> std::chrono::system_clock::time_point {
  std::tm parts = {};
  parts.tm_year = year - 1900;
  parts.tm_mon = month - 1;
  parts.tm_mday = day;
  parts.tm_hour = hour;
  parts.tm_min = minute;
  parts.tm_sec = second;
  parts.tm_isdst = 0;
#if defined(_MSC_VER)
  return std::chrono::system_clock::from_time_t(_mkgmtime(&parts));
#else
  return std::chrono::system_clock::from_time_t(timegm(&parts));
#endif
}
} // namespace

TEST(format_epoch) {
  const auto point{std::chrono::system_clock::from_time_t(0)};
  EXPECT_EQ(sourcemeta::core::to_iso8601_basic(point), "19700101T000000Z");
}

TEST(format_aws_reference) {
  EXPECT_EQ(
      sourcemeta::core::to_iso8601_basic(make_point(2015, 8, 30, 12, 36, 0)),
      "20150830T123600Z");
}

TEST(format_zero_pads_single_digit_month_and_day) {
  EXPECT_EQ(sourcemeta::core::to_iso8601_basic(make_point(2024, 1, 5, 9, 8, 7)),
            "20240105T090807Z");
}

TEST(format_end_of_year) {
  EXPECT_EQ(
      sourcemeta::core::to_iso8601_basic(make_point(2023, 12, 31, 23, 59, 59)),
      "20231231T235959Z");
}

TEST(format_leap_day) {
  EXPECT_EQ(
      sourcemeta::core::to_iso8601_basic(make_point(2020, 2, 29, 0, 0, 0)),
      "20200229T000000Z");
}

TEST(format_far_future) {
  EXPECT_EQ(
      sourcemeta::core::to_iso8601_basic(make_point(2100, 6, 15, 1, 2, 3)),
      "21000615T010203Z");
}

TEST(parse_epoch) {
  const auto point{sourcemeta::core::from_iso8601_basic("19700101T000000Z")};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(point.value(), std::chrono::system_clock::from_time_t(0));
}

TEST(parse_aws_reference) {
  const auto point{sourcemeta::core::from_iso8601_basic("20150830T123600Z")};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(point.value(), make_point(2015, 8, 30, 12, 36, 0));
}

TEST(parse_then_format) {
  const auto point{sourcemeta::core::from_iso8601_basic("20240105T090807Z")};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_iso8601_basic(point.value()),
            "20240105T090807Z");
}

TEST(format_then_parse) {
  const auto point{make_point(1999, 11, 7, 16, 30, 29)};
  const auto formatted{sourcemeta::core::to_iso8601_basic(point)};
  const auto parsed{sourcemeta::core::from_iso8601_basic(formatted)};
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed.value(), point);
}

TEST(parse_comparison_less_than) {
  const auto earlier{sourcemeta::core::from_iso8601_basic("20150830T123600Z")};
  const auto later{sourcemeta::core::from_iso8601_basic("20150830T123601Z")};
  EXPECT_TRUE(earlier.has_value());
  EXPECT_TRUE(later.has_value());
  EXPECT_TRUE(earlier.value() < later.value());
}

TEST(parse_with_string_view_subview) {
  const std::string buffer{"prefix:20150830T123600Z:suffix"};
  const std::string_view input{buffer.data() + 7, 16};
  const auto point{sourcemeta::core::from_iso8601_basic(input)};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_iso8601_basic(point.value()),
            "20150830T123600Z");
}

TEST(parse_leap_day) {
  EXPECT_TRUE(
      sourcemeta::core::from_iso8601_basic("20200229T000000Z").has_value());
}

TEST(parse_empty_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_iso8601_basic("").has_value());
}

TEST(parse_too_short_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830T12360Z").has_value());
}

TEST(parse_too_long_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830T1236000Z").has_value());
}

TEST(parse_rejects_missing_t) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830X123600Z").has_value());
}

TEST(parse_rejects_missing_z) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830T123600X").has_value());
}

TEST(parse_rejects_lowercase_t) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830t123600Z").has_value());
}

TEST(parse_rejects_lowercase_z) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830T123600z").has_value());
}

TEST(parse_rejects_extended_format) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("2015-08-30T12:36:00Z").has_value());
}

TEST(parse_rejects_non_digit_in_year) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("2O150830T123600Z").has_value());
}

TEST(parse_rejects_leading_sign) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("-0150830T123600Z").has_value());
}

TEST(parse_rejects_month_zero) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150030T123600Z").has_value());
}

TEST(parse_rejects_month_thirteen) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20151330T123600Z").has_value());
}

TEST(parse_rejects_day_zero) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150800T123600Z").has_value());
}

TEST(parse_rejects_february_thirtieth) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150230T123600Z").has_value());
}

TEST(parse_rejects_april_thirty_first) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150431T123600Z").has_value());
}

TEST(parse_rejects_february_twenty_ninth_non_leap) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150229T123600Z").has_value());
}

TEST(parse_rejects_hour_twenty_four) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830T243600Z").has_value());
}

TEST(parse_rejects_minute_sixty) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830T126000Z").has_value());
}

TEST(parse_rejects_second_sixty_one) {
  EXPECT_FALSE(
      sourcemeta::core::from_iso8601_basic("20150830T123661Z").has_value());
}
