#include <gtest/gtest.h>

#include <chrono>
#include <ctime>

#include <sourcemeta/core/time.h>

TEST(Time_rfc850_date, parse_rfc_example) {
  const auto point{
      sourcemeta::core::from_rfc850_date("Sunday, 06-Nov-94 08:49:37 GMT")};
  ASSERT_TRUE(point.has_value());

  std::tm parts = {};
  parts.tm_year = 94;
  parts.tm_mon = 10;
  parts.tm_mday = 6;
  parts.tm_hour = 8;
  parts.tm_min = 49;
  parts.tm_sec = 37;
  parts.tm_isdst = 0;
#if defined(_MSC_VER)
  const auto expected{
      std::chrono::system_clock::from_time_t(_mkgmtime(&parts))};
#else
  const auto expected{std::chrono::system_clock::from_time_t(timegm(&parts))};
#endif
  EXPECT_EQ(point.value(), expected);
}

TEST(Time_rfc850_date, parse_full_weekday_name) {
  EXPECT_TRUE(
      sourcemeta::core::from_rfc850_date("Wednesday, 21-Oct-15 11:28:00 GMT")
          .has_value());
}

TEST(Time_rfc850_date, parse_y2k_current_century) {
  const auto point{
      sourcemeta::core::from_rfc850_date("Friday, 01-Jan-21 00:00:00 GMT")};
  ASSERT_TRUE(point.has_value());

  std::tm parts = {};
  parts.tm_year = 121;
  parts.tm_mon = 0;
  parts.tm_mday = 1;
  parts.tm_hour = 0;
  parts.tm_min = 0;
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

TEST(Time_rfc850_date, parse_y2k_previous_century) {
  const auto point{
      sourcemeta::core::from_rfc850_date("Sunday, 06-Nov-94 08:49:37 GMT")};
  ASSERT_TRUE(point.has_value());

  std::tm parts = {};
  parts.tm_year = 94;
  parts.tm_mon = 10;
  parts.tm_mday = 6;
  parts.tm_hour = 8;
  parts.tm_min = 49;
  parts.tm_sec = 37;
  parts.tm_isdst = 0;
#if defined(_MSC_VER)
  const auto expected{
      std::chrono::system_clock::from_time_t(_mkgmtime(&parts))};
#else
  const auto expected{std::chrono::system_clock::from_time_t(timegm(&parts))};
#endif
  EXPECT_EQ(point.value(), expected);
}

TEST(Time_rfc850_date, parse_empty_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_rfc850_date("").has_value());
}

TEST(Time_rfc850_date, parse_missing_comma_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sunday 06-Nov-94 08:49:37 GMT")
          .has_value());
}

TEST(Time_rfc850_date, parse_wrong_separator_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sunday, 06 Nov 94 08:49:37 GMT")
          .has_value());
}

TEST(Time_rfc850_date, parse_imf_fixdate_form_returns_nullopt) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sun, 06 Nov 1994 08:49:37 GMT")
          .has_value());
}

TEST(Time_rfc850_date, parse_garbage_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_rfc850_date("FOO").has_value());
}

TEST(Time_rfc850_date, parse_rejects_abbreviated_weekday) {
  EXPECT_FALSE(sourcemeta::core::from_rfc850_date("Sun, 06-Nov-94 08:49:37 GMT")
                   .has_value());
}

TEST(Time_rfc850_date, parse_rejects_unknown_weekday) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Xyzzy, 06-Nov-94 08:49:37 GMT")
          .has_value());
}

TEST(Time_rfc850_date, parse_rejects_empty_weekday) {
  EXPECT_FALSE(sourcemeta::core::from_rfc850_date(", 06-Nov-94 08:49:37 GMT")
                   .has_value());
}

TEST(Time_rfc850_date, parse_rejects_leading_prefix) {
  EXPECT_FALSE(sourcemeta::core::from_rfc850_date(
                   "prefix Sunday, 06-Nov-94 08:49:37 GMT")
                   .has_value());
}

TEST(Time_rfc850_date, parse_rejects_four_digit_year) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sunday, 06-Nov-1994 08:49:37 GMT")
          .has_value());
}

TEST(Time_rfc850_date, parse_rejects_lowercase_gmt) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sunday, 06-Nov-94 08:49:37 gmt")
          .has_value());
}

// RFC 5322 §3.3: the day-of-month must be valid for the given month and year
TEST(Time_rfc850_date, parse_rejects_february_thirtieth) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Monday, 30-Feb-15 11:28:00 GMT")
          .has_value());
}

// RFC 5322 §3.3: 2015 is not a leap year so February has only 28 days
TEST(Time_rfc850_date, parse_rejects_february_twenty_ninth_non_leap) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sunday, 29-Feb-15 11:28:00 GMT")
          .has_value());
}

// RFC 5322 §3.3: the hour must be in the range 00-23
TEST(Time_rfc850_date, parse_rejects_hour_twenty_four) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sunday, 06-Nov-94 24:49:37 GMT")
          .has_value());
}

// RFC 5322 §3.3: the minute must be in the range 00-59
TEST(Time_rfc850_date, parse_rejects_minute_sixty) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sunday, 06-Nov-94 08:60:37 GMT")
          .has_value());
}

// RFC 5322 §3.3: the second must not exceed a leap second
TEST(Time_rfc850_date, parse_rejects_second_sixty_one) {
  EXPECT_FALSE(
      sourcemeta::core::from_rfc850_date("Sunday, 06-Nov-94 08:49:61 GMT")
          .has_value());
}

TEST(Time_rfc850_date, parse_y2k_boundary_at_threshold) {
  const auto point{
      sourcemeta::core::from_rfc850_date("Friday, 01-Jan-76 00:00:00 GMT")};
  ASSERT_TRUE(point.has_value());

  std::tm parts = {};
  parts.tm_year = 176;
  parts.tm_mon = 0;
  parts.tm_mday = 1;
  parts.tm_hour = 0;
  parts.tm_min = 0;
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

TEST(Time_rfc850_date, parse_y2k_boundary_just_over) {
  const auto point{
      sourcemeta::core::from_rfc850_date("Tuesday, 01-Jan-77 00:00:00 GMT")};
  ASSERT_TRUE(point.has_value());

  std::tm parts = {};
  parts.tm_year = 77;
  parts.tm_mon = 0;
  parts.tm_mday = 1;
  parts.tm_hour = 0;
  parts.tm_min = 0;
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

TEST(Time_rfc850_date, parse_y2k_yy_zero) {
  const auto point{
      sourcemeta::core::from_rfc850_date("Saturday, 01-Jan-00 00:00:00 GMT")};
  ASSERT_TRUE(point.has_value());

  std::tm parts = {};
  parts.tm_year = 100;
  parts.tm_mon = 0;
  parts.tm_mday = 1;
  parts.tm_hour = 0;
  parts.tm_min = 0;
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

TEST(Time_rfc850_date, format_round_trip) {
  std::tm parts = {};
  parts.tm_year = 70;
  parts.tm_mon = 0;
  parts.tm_mday = 1;
  parts.tm_hour = 0;
  parts.tm_min = 0;
  parts.tm_sec = 0;
  parts.tm_isdst = 0;
#if defined(_MSC_VER)
  const auto point{std::chrono::system_clock::from_time_t(_mkgmtime(&parts))};
#else
  const auto point{std::chrono::system_clock::from_time_t(timegm(&parts))};
#endif
  EXPECT_EQ(sourcemeta::core::to_rfc850_date(point),
            "Thursday, 01-Jan-70 00:00:00 GMT");
}
