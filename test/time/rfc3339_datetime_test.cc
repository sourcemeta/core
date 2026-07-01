#include <sourcemeta/core/test.h>
#include <sourcemeta/core/time.h>

TEST(valid_rfc_example_1) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("1985-04-12T23:20:50.52Z"));
}

TEST(valid_rfc_example_negative) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1996-12-19T16:39:57-08:00"));
}

TEST(valid_rfc_leap_second_1) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("1990-12-31T23:59:60Z"));
}

TEST(valid_rfc_leap_second_2) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1990-12-31T15:59:60-08:00"));
}

TEST(valid_rfc_historical) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1937-01-01T12:00:27.87+00:20"));
}

TEST(valid_basic_utc) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00Z"));
}

TEST(valid_long_secfrac) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00.123456Z"));
}

TEST(valid_historical_leap_sec) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("1998-12-31T23:59:60Z"));
}

TEST(valid_year_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("0000-01-01T00:00:00Z"));
}

TEST(valid_year_zero_leap_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("0000-02-29T00:00:00Z"));
}

TEST(valid_max_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("9999-12-31T23:59:59Z"));
}

TEST(valid_lowercase_t_z) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2024-01-15t14:30:00z"));
}

TEST(valid_utc_offset_zero) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T00:00:00+00:00"));
}

TEST(valid_unknown_offset) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T00:00:00-00:00"));
}

TEST(valid_year_2000_leap) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2000-02-29T12:00:00Z"));
}

TEST(valid_normal_leap_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2004-02-29T12:00:00Z"));
}

TEST(valid_secfrac_with_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime(
      "1963-06-19T08:30:06.283185+01:00"));
}

TEST(invalid_no_timezone) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00"));
}

TEST(invalid_colonless_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00+0530"));
}

TEST(invalid_non_leap_feb29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2023-02-29T14:30:00Z"));
}

TEST(invalid_century_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2100-02-29T14:30:00Z"));
}

TEST(invalid_1900_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("1900-02-29T14:30:00Z"));
}

TEST(invalid_space_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15 14:30:00Z"));
}

TEST(invalid_tab_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15\t14:30:00Z"));
}

TEST(invalid_five_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("10000-01-01T00:00:00Z"));
}

TEST(invalid_month_13) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-13-01T14:30:00Z"));
}

TEST(invalid_month_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-00-01T14:30:00Z"));
}

TEST(invalid_day_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-00T14:30:00Z"));
}

TEST(invalid_day_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-32T14:30:00Z"));
}

TEST(invalid_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T24:00:00Z"));
}

TEST(invalid_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:60:00Z"));
}

TEST(invalid_second_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:61Z"));
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00Z "));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime(" 2024-01-15T14:30:00Z"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("")); }

TEST(invalid_not_a_date) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("not-a-date"));
}

TEST(invalid_offset_hour_25) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00+25:00"));
}

TEST(invalid_offset_minute_60) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00+00:60"));
}

TEST(invalid_empty_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-01-15T14:30:00.Z"));
}

TEST(invalid_feb29_year_2300) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2300-02-29T00:00:00Z"));
}

TEST(invalid_leap_second_wrong_minute_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("1998-12-31T23:58:60Z"));
}

TEST(invalid_leap_second_wrong_hour_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("1998-12-31T22:59:60Z"));
}

TEST(invalid_leap_second_wrong_local_negative_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("1998-12-31T22:59:60-08:00"));
}

TEST(invalid_leap_second_wrong_local_positive_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("1998-12-31T04:59:60+04:00"));
}

TEST(valid_leap_second_positive_offset_rollover) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1999-01-01T03:59:60+04:00"));
}

TEST(valid_leap_second_negative_offset_rollover) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1998-12-31T07:59:60-16:00"));
}

TEST(valid_leap_second_unknown_offset) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("1998-12-31T23:59:60-00:00"));
}

TEST(invalid_leap_second_zero_hour_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("1998-12-31T00:00:60Z"));
}

TEST(valid_leap_second_june_30) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2012-06-30T23:59:60Z"));
}

TEST(valid_leap_second_december_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2008-12-31T23:59:60Z"));
}

TEST(valid_leap_second_june_30_negative_offset) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2012-06-30T15:59:60-08:00"));
}

TEST(valid_leap_second_june_30_positive_offset_rollover) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2012-07-01T03:59:60+04:00"));
}

TEST(invalid_leap_second_december_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("1998-12-30T23:59:60Z"));
}

TEST(invalid_leap_second_march_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-03-31T23:59:60Z"));
}

TEST(invalid_leap_second_september_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-09-30T23:59:60Z"));
}

TEST(invalid_leap_second_june_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-06-29T23:59:60Z"));
}

TEST(invalid_leap_second_july_1) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2024-07-01T23:59:60Z"));
}

TEST(invalid_leap_second_january_1_utc_shifts_to_jan_1) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2024-01-01T23:59:60-00:00"));
}

TEST(invalid_leap_second_year_zero_jan_1_underflow) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("0000-01-01T00:59:60+01:00"));
}
