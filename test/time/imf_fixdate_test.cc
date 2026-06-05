#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <string>
#include <string_view>

#include <sourcemeta/core/time.h>

TEST(Time_imf_fixdate, format_round_trip) {
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

TEST(Time_imf_fixdate, parse_valid) {
  const auto point{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT")};
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

TEST(Time_imf_fixdate, parse_then_format) {
  const auto point{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT")};
  ASSERT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point.value()),
            "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(Time_imf_fixdate, parse_invalid_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("FOO").has_value());
}

TEST(Time_imf_fixdate, parse_empty_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("").has_value());
}

TEST(Time_imf_fixdate, parse_comparison_equal_1) {
  EXPECT_EQ(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT"),
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT"));
}

TEST(Time_imf_fixdate, parse_comparison_equal_2) {
  EXPECT_EQ(
      sourcemeta::core::from_imf_fixdate("Mon, 29 Jul 2024 16:30:29 GMT"),
      sourcemeta::core::from_imf_fixdate("Mon, 29 Jul 2024 16:30:29 GMT"));
}

TEST(Time_imf_fixdate, parse_comparison_less_than) {
  const auto earlier{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:27:00 GMT")};
  const auto later{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT")};
  ASSERT_TRUE(earlier.has_value());
  ASSERT_TRUE(later.has_value());
  EXPECT_TRUE(earlier.value() < later.value());
}

TEST(Time_imf_fixdate, parse_comparison_greater_than) {
  const auto future{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2100 11:28:00 GMT")};
  const auto past{
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT")};
  ASSERT_TRUE(future.has_value());
  ASSERT_TRUE(past.has_value());
  EXPECT_TRUE(future.value() > past.value());
}

TEST(Time_imf_fixdate, parse_with_string_view) {
  const std::string_view input{"Wed, 21 Oct 2015 11:28:00 GMT"};
  const auto point{sourcemeta::core::from_imf_fixdate(input)};
  ASSERT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point.value()),
            "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(Time_imf_fixdate, parse_with_string_view_subview) {
  const std::string buffer{"prefix:Wed, 21 Oct 2015 11:28:00 GMT:suffix"};
  const std::string_view input{buffer.data() + 7, 29};
  const auto point{sourcemeta::core::from_imf_fixdate(input)};
  ASSERT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point.value()),
            "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(Time_imf_fixdate, parse_rejects_lowercase_gmt) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 gmt")
          .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_wrong_zone) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 UTC")
          .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_missing_zone) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00")
                   .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_single_digit_day) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 1 Oct 2015 11:28:00 GMT")
          .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_double_space_after_comma) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed,  21 Oct 2015 11:28:00 GMT")
          .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_missing_space_after_comma) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed,21 Oct 2015 11:28:00 GMT")
          .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_two_digit_year) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 15 11:28:00 GMT")
                   .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_trailing_garbage) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Wed, 21 Oct 2015 11:28:00 GMT extra")
          .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_rfc850_shape) {
  EXPECT_FALSE(
      sourcemeta::core::from_imf_fixdate("Sunday, 06-Nov-94 08:49:37 GMT")
          .has_value());
}

TEST(Time_imf_fixdate, parse_rejects_asctime_shape) {
  EXPECT_FALSE(sourcemeta::core::from_imf_fixdate("Sun Nov  6 08:49:37 1994")
                   .has_value());
}

TEST(Time_imf_fixdate, format_epoch) {
  const auto point{std::chrono::system_clock::from_time_t(0)};
  EXPECT_EQ(sourcemeta::core::to_imf_fixdate(point),
            "Thu, 01 Jan 1970 00:00:00 GMT");
}
