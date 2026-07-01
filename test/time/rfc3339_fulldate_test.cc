#include <sourcemeta/core/test.h>

#include <sourcemeta/core/time.h>

TEST(valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2024-01-15"));
}

TEST(valid_year_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0000-01-01"));
}

TEST(valid_year_zero_leap_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0000-02-29"));
}

TEST(valid_min_non_zero_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0001-01-01"));
}

TEST(valid_max_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("9999-12-31"));
}

TEST(valid_year_2000_leap) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2000-02-29"));
}

TEST(valid_normal_leap_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-02-29"));
}

TEST(valid_february_28_non_leap) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2021-02-28"));
}

TEST(valid_april_30) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2024-04-30"));
}

TEST(valid_january_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2024-01-31"));
}

TEST(invalid_non_leap_feb29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2021-02-29"));
}

TEST(invalid_century_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2100-02-29"));
}

TEST(invalid_1900_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("1900-02-29"));
}

TEST(invalid_feb_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-02-30"));
}

TEST(invalid_april_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-04-31"));
}

TEST(invalid_june_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-06-31"));
}

TEST(invalid_september_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-09-31"));
}

TEST(invalid_november_31) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-11-31"));
}

TEST(invalid_month_13) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-13-01"));
}

TEST(invalid_month_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-00-01"));
}

TEST(invalid_day_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-01-00"));
}

TEST(invalid_day_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-01-32"));
}

TEST(invalid_two_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20-01-01"));
}

TEST(invalid_three_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("998-01-01"));
}

TEST(invalid_five_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("12020-01-01"));
}

TEST(invalid_one_digit_month) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-1-01"));
}

TEST(invalid_three_digit_month) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-001-01"));
}

TEST(invalid_one_digit_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-1"));
}

TEST(invalid_three_digit_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-001"));
}

TEST(invalid_positive_sign_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("+2020-01-01"));
}

TEST(invalid_negative_sign_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("-2020-01-01"));
}

TEST(invalid_slash_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024/01/15"));
}

TEST(invalid_en_dash_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024\xe2\x80\x93"
                                                     "01\xe2\x80\x93"
                                                     "15"));
}

TEST(invalid_bengali_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0\xe0\xa7\xaa-01"));
}

TEST(invalid_arabic_indic_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-\xd9\xa0\xd9\xa1"
                                                     "-01"));
}

TEST(invalid_trailing_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01T00:00:00"));
}

TEST(invalid_trailing_t) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01T"));
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-01-15 "));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate(" 2024-01-15"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("")); }

TEST(invalid_not_a_date) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("not-a-date"));
}

TEST(valid_march_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-03-31"));
}

TEST(invalid_march_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-03-32"));
}

TEST(valid_may_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-05-31"));
}

TEST(invalid_may_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-05-32"));
}

TEST(valid_july_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-07-31"));
}

TEST(invalid_july_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-07-32"));
}

TEST(valid_august_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-08-31"));
}

TEST(invalid_august_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-08-32"));
}

TEST(valid_october_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2020-10-31"));
}

TEST(invalid_october_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-10-32"));
}

TEST(invalid_century_0100_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("0100-02-29"));
}

TEST(valid_century_0400_leap) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0400-02-29"));
}

TEST(invalid_iso8601_ordinal_date) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2013-350"));
}

TEST(invalid_iso8601_no_dashes) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20230328"));
}

TEST(invalid_iso8601_week_number) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-W01"));
}

TEST(invalid_iso8601_week_with_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-W13-2"));
}

TEST(invalid_iso8601_week_rollover) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2022W527"));
}

TEST(invalid_full_datetime) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-11-28T23:55:45Z"));
}

TEST(invalid_trailing_x) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01X"));
}

TEST(invalid_trailing_z) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01Z"));
}

TEST(invalid_date_space_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01 00:00:00Z"));
}

TEST(invalid_embedded_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020 -01-01"));
}

TEST(invalid_bengali_digit_in_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("1963-06-1\xe0\xa7\xaa"));
}

TEST(invalid_bengali_digit_in_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xe0\xa7\xa8"
                                                     "020-01-01"));
}

TEST(invalid_alpha_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("YYYY-01-01"));
}
