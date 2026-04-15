#include <gtest/gtest.h>

#include <sourcemeta/core/time.h>

TEST(Time_rfc3339_datetime, valid_rfc_example_1) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("1985-04-12T23:20:50.52Z"));
}

TEST(Time_rfc3339_datetime, valid_rfc_example_negative) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1996-12-19T16:39:57-08:00"));
}

TEST(Time_rfc3339_datetime, valid_rfc_leap_second_1) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("1990-12-31T23:59:60Z"));
}

TEST(Time_rfc3339_datetime, valid_rfc_leap_second_2) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1990-12-31T15:59:60-08:00"));
}

TEST(Time_rfc3339_datetime, valid_rfc_historical) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1937-01-01T12:00:27.87+00:20"));
}

TEST(Time_rfc3339_datetime, valid_basic_utc) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, valid_long_secfrac) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00.123456Z"));
}

TEST(Time_rfc3339_datetime, valid_historical_leap_sec) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("1998-12-31T23:59:60Z"));
}

TEST(Time_rfc3339_datetime, valid_year_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("0000-01-01T00:00:00Z"));
}

TEST(Time_rfc3339_datetime, valid_year_zero_leap_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("0000-02-29T00:00:00Z"));
}

TEST(Time_rfc3339_datetime, valid_max_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("9999-12-31T23:59:59Z"));
}

TEST(Time_rfc3339_datetime, valid_lowercase_t_z) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2024-01-15t14:30:00z"));
}

TEST(Time_rfc3339_datetime, valid_utc_offset_zero) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T00:00:00+00:00"));
}

TEST(Time_rfc3339_datetime, valid_unknown_offset) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T00:00:00-00:00"));
}

TEST(Time_rfc3339_datetime, valid_year_2000_leap) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2000-02-29T12:00:00Z"));
}

TEST(Time_rfc3339_datetime, valid_normal_leap_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2004-02-29T12:00:00Z"));
}

TEST(Time_rfc3339_datetime, valid_secfrac_with_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime(
      "1963-06-19T08:30:06.283185+01:00"));
}

TEST(Time_rfc3339_datetime, invalid_no_timezone) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00"));
}

TEST(Time_rfc3339_datetime, invalid_colonless_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00+0530"));
}

TEST(Time_rfc3339_datetime, invalid_non_leap_feb29) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2023-02-29T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_century_non_leap) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2100-02-29T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_1900_non_leap) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("1900-02-29T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_space_separator) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15 14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_tab_separator) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15\t14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_five_digit_year) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("10000-01-01T00:00:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_month_13) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-13-01T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_month_00) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-00-01T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_day_00) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-00T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_day_32) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-32T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_hour_24) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T24:00:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_minute_60) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:60:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_second_61) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:61Z"));
}

TEST(Time_rfc3339_datetime, invalid_trailing_space) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00Z "));
}

TEST(Time_rfc3339_datetime, invalid_leading_space) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime(" 2024-01-15T14:30:00Z"));
}

TEST(Time_rfc3339_datetime, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime(""));
}

TEST(Time_rfc3339_datetime, invalid_not_a_date) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("not-a-date"));
}

TEST(Time_rfc3339_datetime, invalid_offset_hour_25) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00+25:00"));
}

TEST(Time_rfc3339_datetime, invalid_offset_minute_60) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00+00:60"));
}

TEST(Time_rfc3339_datetime, invalid_empty_secfrac) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00.Z"));
}

TEST(Time_rfc3339_datetime, invalid_feb29_year_2300) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2300-02-29T00:00:00Z"));
}
