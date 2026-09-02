#include <sourcemeta/core/test.h>
#include <sourcemeta/core/time.h>

#include <string_view> // std::string_view

using namespace std::literals::string_view_literals;

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

TEST(length_01_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2"));
}

TEST(length_02_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("22"));
}

TEST(length_03_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("222"));
}

TEST(length_04_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2222"));
}

TEST(length_05_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("22222"));
}

TEST(length_06_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("222222"));
}

TEST(length_07_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2222222"));
}

TEST(length_09_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("222222222"));
}

TEST(length_11_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("22222222222"));
}

TEST(length_12_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("222222222222"));
}

TEST(length_13_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2222222222222"));
}

TEST(length_20_digits_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("22222222222222222222"));
}

TEST(length_09_missing_last_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0"));
}

TEST(length_11_extra_trailing_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-011"));
}

TEST(length_11_trailing_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01\x0a"));
}

TEST(length_12_trailing_crlf) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01\x0d\x0a"));
}

TEST(length_11_trailing_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01\x09"));
}

TEST(length_11_trailing_cr) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01\x0d"));
}

TEST(length_11_trailing_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01\x00"sv));
}

TEST(length_11_leading_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\x0a"
                                                     "2020-01-01"));
}

TEST(length_11_leading_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\x09"
                                                     "2020-01-01"));
}

TEST(length_12_trailing_nbsp) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01\xc2\xa0"));
}

TEST(length_13_trailing_line_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01\xe2\x80\xa8"));
}

TEST(length_11_leading_bom) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xef\xbb\xbf"
                                                     "2020-01-01"));
}

TEST(length_13_embedded_zero_width_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-\xe2\x80\x8b"
                                                     "01-01"));
}

TEST(length_1000_digits) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate(
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "9999999999999999999999999999999999999999999999999999999999999999"));
}

TEST(ten_byte_arabic_indic_digit_in_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xd9\xa2"
                                                     "20-01-01"));
}

TEST(ten_byte_bengali_digit_in_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xe0\xa7\xa8"
                                                     "0-01-01"));
}

TEST(ten_byte_fullwidth_digit_in_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xef\xbc\x92"
                                                     "0-01-01"));
}

TEST(ten_byte_astral_digit_in_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xf0\x9d\x9f\x98-01-01"));
}

TEST(ten_byte_arabic_indic_digit_in_month) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-\xd9\xa2-01"));
}

TEST(ten_byte_arabic_indic_digit_in_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-\xd9\xa2"));
}

TEST(ten_byte_bengali_digit_in_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01\xe0\xa7\xa8"));
}

TEST(ten_byte_high_byte_as_first_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\xd9\xa2"
                                                     "1-01"));
}

TEST(ten_byte_lone_continuation_byte) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\x80"
                                                     "020-01-01"));
}

TEST(ten_byte_lone_lead_byte) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xe0"
                                                     "020-01-01"));
}

TEST(ten_byte_never_valid_utf8_byte_ff) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\xff"
                                                     "020-01-01"));
}

TEST(ten_byte_c1_control_byte) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\x9f"
                                                     "020-01-01"));
}

TEST(ten_byte_high_byte_every_slot) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate(
      "\xd9\xa2\xd9\xa2\xd9\xa2\xd9\xa2\xd9\xa2"));
}

TEST(first_separator_dot) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020.01-01"));
}

TEST(second_separator_dot) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01.01"));
}

TEST(both_separators_dot) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020.01.01"));
}

TEST(first_separator_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020_01-01"));
}

TEST(second_separator_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01_01"));
}

TEST(both_separators_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020_01_01"));
}

TEST(first_separator_colon) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020:01-01"));
}

TEST(second_separator_colon) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01:01"));
}

TEST(both_separators_colon) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020:01:01"));
}

TEST(first_separator_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020 01-01"));
}

TEST(second_separator_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01 01"));
}

TEST(both_separators_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020 01 01"));
}

TEST(first_separator_backslash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\x5c"
                                                     "01-01"));
}

TEST(second_separator_backslash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01\x5c"
                                                     "01"));
}

TEST(both_separators_backslash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\x5c"
                                                     "01\x5c"
                                                     "01"));
}

TEST(first_separator_pipe) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020|01-01"));
}

TEST(second_separator_pipe) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01|01"));
}

TEST(both_separators_pipe) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020|01|01"));
}

TEST(first_separator_comma) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020,01-01"));
}

TEST(second_separator_comma) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01,01"));
}

TEST(both_separators_comma) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020,01,01"));
}

TEST(first_separator_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020+01-01"));
}

TEST(second_separator_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01+01"));
}

TEST(both_separators_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020+01+01"));
}

TEST(first_separator_tilde) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020~01-01"));
}

TEST(second_separator_tilde) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01~01"));
}

TEST(both_separators_tilde) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020~01~01"));
}

TEST(first_separator_equals) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020=01-01"));
}

TEST(second_separator_equals) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01=01"));
}

TEST(both_separators_equals) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020=01=01"));
}

TEST(first_separator_asterisk) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020*01-01"));
}

TEST(second_separator_asterisk) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01*01"));
}

TEST(both_separators_asterisk) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020*01*01"));
}

TEST(first_separator_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020001-01"));
}

TEST(second_separator_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01001"));
}

TEST(both_separators_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020001001"));
}

TEST(first_separator_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\x09"
                                                     "01-01"));
}

TEST(second_separator_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01\x09"
                                                     "01"));
}

TEST(both_separators_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\x09"
                                                     "01\x09"
                                                     "01"));
}

TEST(first_separator_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\x0a"
                                                     "01-01"));
}

TEST(second_separator_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01\x0a"
                                                     "01"));
}

TEST(both_separators_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\x0a"
                                                     "01\x0a"
                                                     "01"));
}

TEST(first_separator_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\x00"
                                                     "01-01"sv));
}

TEST(second_separator_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01\x00"
                                                     "01"sv));
}

TEST(both_separators_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020\x00"
                                                     "01\x00"
                                                     "01"sv));
}

TEST(digit_slot_year_d1_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("a020-01-01"));
}

TEST(digit_slot_year_d1_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("-020-01-01"));
}

TEST(digit_slot_year_d1_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate(" 020-01-01"));
}

TEST(digit_slot_year_d1_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("+020-01-01"));
}

TEST(digit_slot_year_d1_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("/020-01-01"));
}

TEST(digit_slot_year_d1_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("\x00"
                                                     "020-01-01"sv));
}

TEST(digit_slot_year_d2_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2a20-01-01"));
}

TEST(digit_slot_year_d2_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2-20-01-01"));
}

TEST(digit_slot_year_d2_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2 20-01-01"));
}

TEST(digit_slot_year_d2_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2+20-01-01"));
}

TEST(digit_slot_year_d2_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2/20-01-01"));
}

TEST(digit_slot_year_d2_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2\x00"
                                                     "20-01-01"sv));
}

TEST(digit_slot_year_d3_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20a0-01-01"));
}

TEST(digit_slot_year_d3_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20-0-01-01"));
}

TEST(digit_slot_year_d3_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20 0-01-01"));
}

TEST(digit_slot_year_d3_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20+0-01-01"));
}

TEST(digit_slot_year_d3_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20/0-01-01"));
}

TEST(digit_slot_year_d3_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("20\x00"
                                                     "0-01-01"sv));
}

TEST(digit_slot_year_d4_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("202a-01-01"));
}

TEST(digit_slot_year_d4_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("202--01-01"));
}

TEST(digit_slot_year_d4_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("202 -01-01"));
}

TEST(digit_slot_year_d4_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("202+-01-01"));
}

TEST(digit_slot_year_d4_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("202/-01-01"));
}

TEST(digit_slot_year_d4_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("202\x00"
                                                     "-01-01"sv));
}

TEST(digit_slot_month_d1_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-a1-01"));
}

TEST(digit_slot_month_d1_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020--1-01"));
}

TEST(digit_slot_month_d1_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020- 1-01"));
}

TEST(digit_slot_month_d1_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-+1-01"));
}

TEST(digit_slot_month_d1_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-/1-01"));
}

TEST(digit_slot_month_d1_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-\x00"
                                                     "1-01"sv));
}

TEST(digit_slot_month_d2_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0a-01"));
}

TEST(digit_slot_month_d2_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0--01"));
}

TEST(digit_slot_month_d2_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0 -01"));
}

TEST(digit_slot_month_d2_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0+-01"));
}

TEST(digit_slot_month_d2_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0/-01"));
}

TEST(digit_slot_month_d2_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0\x00"
                                                     "-01"sv));
}

TEST(digit_slot_day_d1_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-a1"));
}

TEST(digit_slot_day_d1_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01--1"));
}

TEST(digit_slot_day_d1_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01- 1"));
}

TEST(digit_slot_day_d1_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-+1"));
}

TEST(digit_slot_day_d1_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-/1"));
}

TEST(digit_slot_day_d1_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-\x00"
                                                     "1"sv));
}

TEST(digit_slot_day_d2_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0a"));
}

TEST(digit_slot_day_d2_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0-"));
}

TEST(digit_slot_day_d2_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0 "));
}

TEST(digit_slot_day_d2_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0+"));
}

TEST(digit_slot_day_d2_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0/"));
}

TEST(digit_slot_day_d2_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0\x00"sv));
}

TEST(month_second_digit_colon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0:-01"));
}

TEST(month_second_digit_semicolon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0;-01"));
}

TEST(month_second_digit_less_than_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-0<-01"));
}

TEST(day_second_digit_colon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0:"));
}

TEST(day_second_digit_semicolon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0;"));
}

TEST(day_second_digit_less_than_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0<"));
}

TEST(day_second_digit_equals_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0="));
}

TEST(day_second_digit_greater_than_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0>"));
}

TEST(day_second_digit_question_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-0?"));
}

TEST(day_first_digit_colon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-:1"));
}

TEST(month_value_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-00-15"));
}

TEST(month_value_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-01-15"));
}

TEST(month_value_02) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-02-15"));
}

TEST(month_value_09) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-09-15"));
}

TEST(month_value_10) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-10-15"));
}

TEST(month_value_11) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-11-15"));
}

TEST(month_value_12) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-12-15"));
}

TEST(month_value_13) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-13-15"));
}

TEST(month_value_14) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-14-15"));
}

TEST(month_value_19) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-19-15"));
}

TEST(month_value_20) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-20-15"));
}

TEST(month_value_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-29-15"));
}

TEST(month_value_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-30-15"));
}

TEST(month_value_50) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-50-15"));
}

TEST(month_value_90) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-90-15"));
}

TEST(month_value_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-99-15"));
}

TEST(day_january_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-01-00"));
}

TEST(day_january_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-01-01"));
}

TEST(day_january_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-01-31"));
}

TEST(day_january_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-01-32"));
}

TEST(day_january_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-01-99"));
}

TEST(day_february_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-02-00"));
}

TEST(day_february_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-02-01"));
}

TEST(day_february_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-02-28"));
}

TEST(day_february_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-02-29"));
}

TEST(day_february_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-02-99"));
}

TEST(day_march_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-03-00"));
}

TEST(day_march_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-03-01"));
}

TEST(day_march_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-03-31"));
}

TEST(day_march_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-03-32"));
}

TEST(day_march_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-03-99"));
}

TEST(day_april_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-04-00"));
}

TEST(day_april_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-04-01"));
}

TEST(day_april_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-04-30"));
}

TEST(day_april_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-04-31"));
}

TEST(day_april_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-04-99"));
}

TEST(day_may_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-05-00"));
}

TEST(day_may_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-05-01"));
}

TEST(day_may_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-05-31"));
}

TEST(day_may_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-05-32"));
}

TEST(day_may_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-05-99"));
}

TEST(day_june_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-06-00"));
}

TEST(day_june_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-06-01"));
}

TEST(day_june_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-06-30"));
}

TEST(day_june_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-06-31"));
}

TEST(day_june_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-06-99"));
}

TEST(day_july_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-07-00"));
}

TEST(day_july_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-07-01"));
}

TEST(day_july_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-07-31"));
}

TEST(day_july_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-07-32"));
}

TEST(day_july_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-07-99"));
}

TEST(day_august_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-08-00"));
}

TEST(day_august_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-08-01"));
}

TEST(day_august_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-08-31"));
}

TEST(day_august_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-08-32"));
}

TEST(day_august_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-08-99"));
}

TEST(day_september_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-09-00"));
}

TEST(day_september_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-09-01"));
}

TEST(day_september_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-09-30"));
}

TEST(day_september_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-09-31"));
}

TEST(day_september_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-09-99"));
}

TEST(day_october_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-10-00"));
}

TEST(day_october_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-10-01"));
}

TEST(day_october_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-10-31"));
}

TEST(day_october_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-10-32"));
}

TEST(day_october_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-10-99"));
}

TEST(day_november_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-11-00"));
}

TEST(day_november_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-11-01"));
}

TEST(day_november_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-11-30"));
}

TEST(day_november_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-11-31"));
}

TEST(day_november_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-11-99"));
}

TEST(day_december_00) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-12-00"));
}

TEST(day_december_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-12-01"));
}

TEST(day_december_max) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2023-12-31"));
}

TEST(day_december_max_plus_one) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-12-32"));
}

TEST(day_december_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-12-99"));
}

TEST(february_leap_28) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2024-02-28"));
}

TEST(february_leap_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2024-02-29"));
}

TEST(february_leap_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2024-02-30"));
}

TEST(february_non_leap_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2023-02-30"));
}

TEST(leap_0001_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("0001-02-29"));
}

TEST(leap_0003_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("0003-02-29"));
}

TEST(leap_0004_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0004-02-29"));
}

TEST(leap_0008_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0008-02-29"));
}

TEST(leap_0096_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0096-02-29"));
}

TEST(leap_0099_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("0099-02-29"));
}

TEST(leap_0101_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("0101-02-29"));
}

TEST(leap_0104_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0104-02-29"));
}

TEST(leap_0200_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("0200-02-29"));
}

TEST(leap_0300_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("0300-02-29"));
}

TEST(leap_0396_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0396-02-29"));
}

TEST(leap_0404_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0404-02-29"));
}

TEST(leap_0800_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0800-02-29"));
}

TEST(leap_1000_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("1000-02-29"));
}

TEST(leap_1200_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("1200-02-29"));
}

TEST(leap_1500_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("1500-02-29"));
}

TEST(leap_1600_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("1600-02-29"));
}

TEST(leap_1700_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("1700-02-29"));
}

TEST(leap_1800_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("1800-02-29"));
}

TEST(leap_1996_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("1996-02-29"));
}

TEST(leap_2004_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2004-02-29"));
}

TEST(leap_2025_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2025-02-29"));
}

TEST(leap_2096_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2096-02-29"));
}

TEST(leap_2200_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2200-02-29"));
}

TEST(leap_2300_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2300-02-29"));
}

TEST(leap_2400_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("2400-02-29"));
}

TEST(leap_9600_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("9600-02-29"));
}

TEST(leap_9900_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("9900-02-29"));
}

TEST(leap_9996_feb_29) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("9996-02-29"));
}

TEST(leap_9999_feb_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("9999-02-29"));
}

TEST(sign_or_extended_plus_sign_five_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("+10000-01-01"));
}

TEST(sign_or_extended_unsigned_five_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("10000-01-01"));
}

TEST(sign_or_extended_plus_sign_year_zero) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("+0000-01-01"));
}

TEST(sign_or_extended_minus_sign_year_zero) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("-0000-01-01"));
}

TEST(sign_or_extended_plus_sign_max_extended_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("+99999-01-01"));
}

TEST(sign_or_extended_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate(" 2020-01-01"));
}

TEST(sign_or_extended_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulldate("2020-01-01 "));
}

TEST(boundary_0001_12_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("0001-12-31"));
}

TEST(boundary_1000_01_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("1000-01-01"));
}

TEST(boundary_9998_12_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("9998-12-31"));
}

TEST(boundary_9999_01_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("9999-01-01"));
}

TEST(boundary_9999_02_28) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("9999-02-28"));
}

TEST(boundary_5000_06_15) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulldate("5000-06-15"));
}
