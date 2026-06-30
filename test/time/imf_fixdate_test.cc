#include <sourcemeta/core/test.h>

#include <chrono>
#include <ctime>
#include <string>
#include <string_view>

#include <sourcemeta/core/time.h>

TEST(format_round_trip) {
  std::tm parts = {};
  parts.tm_year = 115;
  parts.tm_mon = 9;
  parts.tm_mday = 21;
  parts.tm_hour = 11;
  parts.tm_min = 28;
  parts.tm_sec = 0;
  parts.tm_isdst = 0;

#if defined(_MSC_VER)
  const auto point{std::chrono::system_clock::from_time_t(_mkgmtime(&parts))};
#else
  const auto point{std::chrono::system_clock::from_time_t(timegm(&parts))};
#endif

  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point),
            "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(parse_valid) {
  const auto point{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT")};
  EXPECT_TRUE(point.has_value());

  std::tm parts = {};
  parts.tm_year = 115;
  parts.tm_mon = 9;
  parts.tm_mday = 21;
  parts.tm_hour = 11;
  parts.tm_min = 28;
  parts.tm_sec = 0;
  parts.tm_isdst = 0;

#if defined(_MSC_VER)
  const auto expected{
      std::chrono::system_clock::from_time_t(_mkgmtime(&parts))};
#else
  const auto expected{std::chrono::system_clock::from_time_t(timegm(&parts))};
#endif

  EXPECT_EQ(point.value(), expected);
}

TEST(parse_then_format) {
  const auto point{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT")};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point.value()),
            "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(parse_invalid_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("FOO").has_value());
}

TEST(parse_empty_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("").has_value());
}

TEST(parse_comparison_equal_1) {
  EXPECT_EQ(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT"),
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT"));
}

TEST(parse_comparison_equal_2) {
  EXPECT_EQ(
      sourcemeta::core::from_imf_fixdate("Mon, 29 Jul 2024 16:30:29 GMT"),
      sourcemeta::core::from_imf_fixdate("Mon, 29 Jul 2024 16:30:29 GMT"));
}

TEST(parse_comparison_less_than) {
  const auto earlier{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:27:00 GMT")};
  const auto later{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT")};
  EXPECT_TRUE(earlier.has_value());
  EXPECT_TRUE(later.has_value());
  EXPECT_TRUE(earlier.value() < later.value());
}

TEST(parse_comparison_greater_than) {
  const auto future{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2100 11:28:00 GMT")};
  const auto past{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT")};
  EXPECT_TRUE(future.has_value());
  EXPECT_TRUE(past.has_value());
  EXPECT_TRUE(future.value() > past.value());
}

TEST(parse_with_string_view) {
  const std::string_view input{"Wed, 21 Oct 2015 11:28:00 GMT"};
  const auto point{sourcemeta::core::from_imf_fixdate(input)};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point.value()),
            "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(parse_with_string_view_subview) {
  const std::string buffer{"prefix:Wed, 21 Oct 2015 11:28:00 GMT:suffix"};
  const std::string_view input{buffer.data() + 7, 29};
  const auto point{sourcemeta::core::from_imf_fixdate(input)};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point.value()),
            "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(parse_rejects_lowercase_gmt) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 gmt")
          .has_value());
}

TEST(parse_rejects_wrong_zone) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 UTC")
          .has_value());
}

TEST(parse_rejects_missing_zone) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00")
                   .has_value());
}

TEST(parse_rejects_single_digit_day) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 1 Oct 2015 11:28:00 GMT")
          .has_value());
}

TEST(parse_rejects_double_space_after_comma) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed,  21 Oct 2015 11:28:00 GMT")
          .has_value());
}

TEST(parse_rejects_missing_space_after_comma) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed,21 Oct 2015 11:28:00 GMT")
          .has_value());
}

TEST(parse_rejects_two_digit_year) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 15 11:28:00 GMT")
                   .has_value());
}

TEST(parse_rejects_trailing_garbage) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT extra")
          .has_value());
}

TEST(parse_rejects_rfc850_shape) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Sunday, 06-Nov-94 08:49:37 GMT")
          .has_value());
}

TEST(parse_rejects_asctime_shape) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("Sun Nov  6 08:49:37 1994")
                   .has_value());
}

// RFC 9110 §5.6.7: the day-of-month must be valid for the given month and year
TEST(parse_rejects_february_thirtieth) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Mon, 30 Feb 2015 11:28:00 GMT")
          .has_value());
}

// RFC 9110 §5.6.7: April has only 30 days
TEST(parse_rejects_april_thirty_first) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 31 Apr 2015 11:28:00 GMT")
          .has_value());
}

// RFC 9110 §5.6.7: 2015 is not a leap year so February has only 28 days
TEST(parse_rejects_february_twenty_ninth_non_leap) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Sun, 29 Feb 2015 11:28:00 GMT")
          .has_value());
}

// RFC 9110 §5.6.7: 2020 is a leap year so February has 29 days
TEST(parse_accepts_february_twenty_ninth_leap) {
  EXPECT_TRUE(
      sourcemeta::core::from_imf_fixdate("Sat, 29 Feb 2020 11:28:00 GMT")
          .has_value());
}

// RFC 9110 §5.6.7: the day-of-month must be at least one
TEST(parse_rejects_zero_day) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 00 Oct 2015 11:28:00 GMT")
          .has_value());
}

// RFC 9110 §5.6.7: the hour must be in the range 00-23
TEST(parse_rejects_hour_twenty_four) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 24:28:00 GMT")
          .has_value());
}

// RFC 9110 §5.6.7: the minute must be in the range 00-59
TEST(parse_rejects_minute_sixty) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:60:00 GMT")
          .has_value());
}

// RFC 9110 §5.6.7: the second must not exceed a leap second
TEST(parse_rejects_second_sixty_one) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:61 GMT")
          .has_value());
}

TEST(format_epoch) {
  const auto point{std::chrono::system_clock::from_time_t(0)};
  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point),
            "Thu, 01 Jan 1970 00:00:00 GMT");
}
