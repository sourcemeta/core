#include <gtest/gtest.h>

#include <sourcemeta/core/time.h>

TEST(Time_rfc3339_fulldate, valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2024-01-15"));
}

TEST(Time_rfc3339_fulldate, valid_year_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0000-01-01"));
}

TEST(Time_rfc3339_fulldate, valid_year_zero_leap_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0000-02-29"));
}

TEST(Time_rfc3339_fulldate, valid_min_non_zero_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0001-01-01"));
}

TEST(Time_rfc3339_fulldate, valid_max_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("9999-12-31"));
}

TEST(Time_rfc3339_fulldate, valid_year_2000_leap) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2000-02-29"));
}

TEST(Time_rfc3339_fulldate, valid_normal_leap_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-02-29"));
}

TEST(Time_rfc3339_fulldate, valid_february_28_non_leap) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2021-02-28"));
}

TEST(Time_rfc3339_fulldate, valid_april_30) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2024-04-30"));
}

TEST(Time_rfc3339_fulldate, valid_january_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2024-01-31"));
}

TEST(Time_rfc3339_fulldate, invalid_non_leap_feb29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2021-02-29"));
}

TEST(Time_rfc3339_fulldate, invalid_century_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2100-02-29"));
}

TEST(Time_rfc3339_fulldate, invalid_1900_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("1900-02-29"));
}

TEST(Time_rfc3339_fulldate, invalid_feb_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-02-30"));
}

TEST(Time_rfc3339_fulldate, invalid_april_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-04-31"));
}

TEST(Time_rfc3339_fulldate, invalid_june_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-06-31"));
}

TEST(Time_rfc3339_fulldate, invalid_september_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-09-31"));
}

TEST(Time_rfc3339_fulldate, invalid_november_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-11-31"));
}

TEST(Time_rfc3339_fulldate, invalid_month_13) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-13-01"));
}

TEST(Time_rfc3339_fulldate, invalid_month_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-00-01"));
}

TEST(Time_rfc3339_fulldate, invalid_day_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-01-00"));
}

TEST(Time_rfc3339_fulldate, invalid_day_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-01-32"));
}

TEST(Time_rfc3339_fulldate, invalid_two_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20-01-01"));
}

TEST(Time_rfc3339_fulldate, invalid_three_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("998-01-01"));
}

TEST(Time_rfc3339_fulldate, invalid_five_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("12020-01-01"));
}

TEST(Time_rfc3339_fulldate, invalid_one_digit_month) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-1-01"));
}

TEST(Time_rfc3339_fulldate, invalid_three_digit_month) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-001-01"));
}

TEST(Time_rfc3339_fulldate, invalid_one_digit_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-1"));
}

TEST(Time_rfc3339_fulldate, invalid_three_digit_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-001"));
}

TEST(Time_rfc3339_fulldate, invalid_positive_sign_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("+2020-01-01"));
}

TEST(Time_rfc3339_fulldate, invalid_negative_sign_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("-2020-01-01"));
}

TEST(Time_rfc3339_fulldate, invalid_slash_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024/01/15"));
}

TEST(Time_rfc3339_fulldate, invalid_en_dash_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024\xe2\x80\x93"
                                                     "01\xe2\x80\x93"
                                                     "15"));
}

TEST(Time_rfc3339_fulldate, invalid_bengali_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0\xe0\xa7\xaa-01"));
}

TEST(Time_rfc3339_fulldate, invalid_arabic_indic_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-\xd9\xa0\xd9\xa1"
                                                     "-01"));
}

TEST(Time_rfc3339_fulldate, invalid_trailing_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01T00:00:00"));
}

TEST(Time_rfc3339_fulldate, invalid_trailing_t) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01T"));
}

TEST(Time_rfc3339_fulldate, invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-01-15 "));
}

TEST(Time_rfc3339_fulldate, invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate(" 2024-01-15"));
}

TEST(Time_rfc3339_fulldate, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate(""));
}

TEST(Time_rfc3339_fulldate, invalid_not_a_date) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("not-a-date"));
}

TEST(Time_rfc3339_fulldate, valid_march_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-03-31"));
}

TEST(Time_rfc3339_fulldate, invalid_march_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-03-32"));
}

TEST(Time_rfc3339_fulldate, valid_may_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-05-31"));
}

TEST(Time_rfc3339_fulldate, invalid_may_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-05-32"));
}

TEST(Time_rfc3339_fulldate, valid_july_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-07-31"));
}

TEST(Time_rfc3339_fulldate, invalid_july_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-07-32"));
}

TEST(Time_rfc3339_fulldate, valid_august_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-08-31"));
}

TEST(Time_rfc3339_fulldate, invalid_august_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-08-32"));
}

TEST(Time_rfc3339_fulldate, valid_october_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-10-31"));
}

TEST(Time_rfc3339_fulldate, invalid_october_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-10-32"));
}

TEST(Time_rfc3339_fulldate, invalid_century_0100_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("0100-02-29"));
}

TEST(Time_rfc3339_fulldate, valid_century_0400_leap) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0400-02-29"));
}

TEST(Time_rfc3339_fulldate, invalid_iso8601_ordinal_date) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2013-350"));
}

TEST(Time_rfc3339_fulldate, invalid_iso8601_no_dashes) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20230328"));
}

TEST(Time_rfc3339_fulldate, invalid_iso8601_week_number) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-W01"));
}

TEST(Time_rfc3339_fulldate, invalid_iso8601_week_with_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-W13-2"));
}

TEST(Time_rfc3339_fulldate, invalid_iso8601_week_rollover) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2022W527"));
}

TEST(Time_rfc3339_fulldate, invalid_full_datetime) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-11-28T23:55:45Z"));
}

TEST(Time_rfc3339_fulldate, invalid_trailing_x) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01X"));
}

TEST(Time_rfc3339_fulldate, invalid_trailing_z) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01Z"));
}

TEST(Time_rfc3339_fulldate, invalid_date_space_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01 00:00:00Z"));
}

TEST(Time_rfc3339_fulldate, invalid_embedded_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020 -01-01"));
}

TEST(Time_rfc3339_fulldate, invalid_bengali_digit_in_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("1963-06-1\xe0\xa7\xaa"));
}

TEST(Time_rfc3339_fulldate, invalid_bengali_digit_in_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xe0\xa7\xa8"
                                                     "020-01-01"));
}

TEST(Time_rfc3339_fulldate, invalid_alpha_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("YYYY-01-01"));
}
