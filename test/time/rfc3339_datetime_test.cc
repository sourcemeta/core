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

TEST(length_01_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2"));
}

TEST(length_02_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("20"));
}

TEST(length_03_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("202"));
}

TEST(length_04_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020"));
}

TEST(length_05_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-"));
}

TEST(length_06_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-0"));
}

TEST(length_07_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01"));
}

TEST(length_08_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-"));
}

TEST(length_09_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-0"));
}

TEST(length_10_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01"));
}

TEST(length_11_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T"));
}

TEST(length_12_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T0"));
}

TEST(length_13_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00"));
}

TEST(length_14_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:"));
}

TEST(length_15_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:0"));
}

TEST(length_16_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00"));
}

TEST(length_17_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:"));
}

TEST(length_18_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:0"));
}

TEST(length_19_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00"));
}

TEST(length_20_minimum_valid) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00Z"));
}

TEST(separator_lower_t) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2020-01-01t00:00:00Z"));
}

TEST(separator_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01 00:00:00Z"));
}

TEST(separator_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01_00:00:00Z"));
}

TEST(separator_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01-00:00:00Z"));
}

TEST(separator_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01/00:00:00Z"));
}

TEST(separator_colon) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01:00:00:00Z"));
}

TEST(separator_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01\x09"
                                                     "00:00:00Z"));
}

TEST(separator_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01\x0a"
                                                     "00:00:00Z"));
}

TEST(separator_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01000:00:00Z"));
}

TEST(separator_letter_z) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01Z00:00:00Z"));
}

TEST(separator_nul) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime(std::string_view("2020-01-01\x00"
                                                             "00:00:00Z",
                                                             20)));
}

TEST(separator_pipe) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01|00:00:00Z"));
}

TEST(separator_double_t) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01TT00:00:00Z"));
}

TEST(separator_missing_shifts_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-0100:00:00Z"));
}

TEST(date_half_month_13) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-13-01T00:00:00Z"));
}

TEST(date_half_month_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-00-01T00:00:00Z"));
}

TEST(date_half_day_32) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-32T00:00:00Z"));
}

TEST(date_half_day_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-00T00:00:00Z"));
}

TEST(date_half_non_leap_feb29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2021-02-29T00:00:00Z"));
}

TEST(date_half_leap_feb29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2020-02-29T00:00:00Z"));
}

TEST(date_half_max_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("9999-12-31T00:00:00Z"));
}

TEST(date_half_min_non_zero_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("0001-01-01T00:00:00Z"));
}

TEST(date_half_one_digit_month) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-1-01T00:00:00Z"));
}

TEST(date_half_sign_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("+2020-01-01T00:00:00Z"));
}

TEST(time_half_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T24:00:00Z"));
}

TEST(time_half_max_normal_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T23:59:59Z"));
}

TEST(time_half_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:60:00Z"));
}

TEST(time_half_second_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:61Z"));
}

TEST(time_half_zero_numoffset) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00+00:00"));
}

TEST(time_half_offset_hour_24) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00+24:00"));
}

TEST(time_half_colonless_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00+0000"));
}

TEST(time_half_empty_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00.Z"));
}

TEST(time_half_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00.5Z"));
}

TEST(time_half_lower_z) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00z"));
}

TEST(leap_june_30_utc) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2016-06-30T23:59:60Z"));
}

TEST(leap_december_31_utc) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2016-12-31T23:59:60Z"));
}

TEST(leap_december_31_lower_z) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2016-12-31T23:59:60z"));
}

TEST(leap_june_30_with_fraction) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("2016-06-30T23:59:60.5Z"));
}

TEST(leap_january_31_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-01-31T23:59:60Z"));
}

TEST(leap_march_31_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-03-31T23:59:60Z"));
}

TEST(leap_september_30_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-09-30T23:59:60Z"));
}

TEST(leap_june_29_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-06-29T23:59:60Z"));
}

TEST(leap_december_30_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-12-30T23:59:60Z"));
}

TEST(leap_mid_month_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-06-15T23:59:60Z"));
}

TEST(leap_offset_rolls_into_july) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2016-07-01T00:59:60+01:00"));
}

TEST(leap_offset_rolls_into_january) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2017-01-01T00:59:60+01:00"));
}

TEST(leap_offset_rolls_back_to_june_30) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2016-06-30T22:59:60-01:00"));
}

TEST(leap_offset_rolls_back_to_december_31) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2016-12-31T22:59:60-01:00"));
}

TEST(leap_offset_lands_on_july_1) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2016-06-30T23:59:60-01:00"));
}

TEST(leap_offset_lands_on_june_30_from_july) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2016-07-01T00:59:60+00:30"));
}

TEST(leap_max_positive_offset) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2016-07-01T23:58:60+23:59"));
}

TEST(leap_max_negative_offset) {
  EXPECT_TRUE(
      sourcemeta::core::is_rfc3339_datetime("2016-06-30T00:00:60-23:59"));
}

TEST(leap_not_2359_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-06-30T12:00:60Z"));
}

TEST(leap_2358_utc) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-06-30T23:58:60Z"));
}

TEST(leap_february_29_leap_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2016-02-29T23:59:60Z"));
}

TEST(leap_february_28_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2015-02-28T23:59:60Z"));
}

TEST(leap_december_31_year_9999) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("9999-12-31T23:59:60Z"));
}

TEST(leap_june_30_year_0001) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime("0001-06-30T23:59:60Z"));
}

TEST(leap_with_long_fraction) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime(
      "2016-06-30T23:59:60.0000000000000000000000000000000000000000Z"));
}

TEST(non_leap_second_59_with_long_fraction) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_datetime(
      "2020-01-01T00:00:59.0000000000000000000000000000000000000000Z"));
}

TEST(trailing_newline) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00Z\x0a"));
}

TEST(leading_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("\x0a"
                                                     "2020-01-01T00:00:00Z"));
}

TEST(trailing_cr) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00Z\x0d"));
}

TEST(leading_cr) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("\x0d"
                                                     "2020-01-01T00:00:00Z"));
}

TEST(trailing_crlf) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00Z\x0d\x0a"));
}

TEST(leading_crlf) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("\x0d\x0a"
                                                     "2020-01-01T00:00:00Z"));
}

TEST(trailing_tab) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00Z\x09"));
}

TEST(leading_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("\x09"
                                                     "2020-01-01T00:00:00Z"));
}

TEST(trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00Z "));
}

TEST(leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime(" 2020-01-01T00:00:00Z"));
}

TEST(trailing_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime(
      std::string_view("2020-01-01T00:00:00Z\x00", 21)));
}

TEST(leading_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime(
      std::string_view("\x00"
                       "2020-01-01T00:00:00Z",
                       21)));
}

TEST(trailing_letter) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00Zx"));
}

TEST(leading_letter) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("x2020-01-01T00:00:00Z"));
}

TEST(trailing_bom) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime(
      "2020-01-01T00:00:00Z\xef\xbb\xbf"));
}

TEST(leading_bom) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("\xef\xbb\xbf"
                                                     "2020-01-01T00:00:00Z"));
}

TEST(non_ascii_digit_in_date_half) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("\xd9\xa2"
                                                     "020-01-01T00:00:00Z"));
}

TEST(non_ascii_digit_in_time_half) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01T\xd9\xa0"
                                                     "0:00:00Z"));
}

TEST(unicode_minus_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_datetime("2020-01-01T00:00:00\xe2\x88\x92"
                                            "01:00"));
}

TEST(fullwidth_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_datetime("2020-01-01\xef\xbc\xb4"
                                                     "00:00:00Z"));
}
