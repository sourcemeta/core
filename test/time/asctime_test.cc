#include <gtest/gtest.h>

#include <chrono>
#include <ctime>

#include <sourcemeta/core/time.h>

TEST(Time_asctime, parse_rfc_example) {
  const auto point{sourcemeta::core::from_asctime("Sun Nov  6 08:49:37 1994")};
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

TEST(Time_asctime, parse_two_digit_day) {
  const auto point{sourcemeta::core::from_asctime("Wed Oct 21 11:28:00 2015")};
  ASSERT_TRUE(point.has_value());

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

TEST(Time_asctime, parse_empty_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_asctime("").has_value());
}

TEST(Time_asctime, parse_wrong_length_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_asctime("Sun Nov  6 08:49:37 1994 GMT")
                   .has_value());
}

TEST(Time_asctime, parse_imf_fixdate_form_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_asctime("Sun, 06 Nov 1994 08:49:37 GMT")
                   .has_value());
}

TEST(Time_asctime, parse_rfc850_form_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_asctime("Sunday, 06-Nov-94 08:49:37 GMT")
                   .has_value());
}

TEST(Time_asctime, parse_garbage_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_asctime("FOO").has_value());
}

TEST(Time_asctime, format_round_trip) {
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
  EXPECT_EQ(sourcemeta::core::to_asctime(point), "Thu Jan  1 00:00:00 1970");
}

TEST(Time_asctime, parse_accepts_zero_padded_single_digit_day) {
  EXPECT_TRUE(
      sourcemeta::core::from_asctime("Sun Nov 06 08:49:37 1994").has_value());
}

TEST(Time_asctime, parse_rejects_trailing_newline) {
  EXPECT_FALSE(
      sourcemeta::core::from_asctime("Sun Nov  6 08:49:37 1994\n").has_value());
}

TEST(Time_asctime, parse_rejects_sign_prefix_in_day) {
  EXPECT_FALSE(
      sourcemeta::core::from_asctime("Sun Nov +6 08:49:37 1994").has_value());
}

TEST(Time_asctime, format_output_length_is_24) {
  const auto point{std::chrono::system_clock::from_time_t(0)};
  EXPECT_EQ(sourcemeta::core::to_asctime(point).size(), 24u);
}

TEST(Time_asctime, format_output_has_no_trailing_newline) {
  const auto point{std::chrono::system_clock::from_time_t(0)};
  EXPECT_NE(sourcemeta::core::to_asctime(point).back(), '\n');
}

TEST(Time_asctime, format_two_digit_day) {
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
  EXPECT_EQ(sourcemeta::core::to_asctime(point), "Wed Oct 21 11:28:00 2015");
}
