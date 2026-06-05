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
