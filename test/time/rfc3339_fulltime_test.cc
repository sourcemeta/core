#include <gtest/gtest.h>

#include <sourcemeta/core/time.h>

TEST(Time_rfc3339_fulltime, valid_basic_zulu) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00Z"));
}

TEST(Time_rfc3339_fulltime, valid_min_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("00:00:00Z"));
}

TEST(Time_rfc3339_fulltime, valid_max_normal_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:59Z"));
}

TEST(Time_rfc3339_fulltime, valid_lowercase_z) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00z"));
}

TEST(Time_rfc3339_fulltime, valid_positive_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+05:30"));
}

TEST(Time_rfc3339_fulltime, valid_negative_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00-08:00"));
}

TEST(Time_rfc3339_fulltime, valid_zero_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+00:00"));
}

TEST(Time_rfc3339_fulltime, valid_unknown_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00-00:00"));
}

TEST(Time_rfc3339_fulltime, valid_short_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00.5Z"));
}

TEST(Time_rfc3339_fulltime, valid_long_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00.123456789Z"));
}

TEST(Time_rfc3339_fulltime, valid_secfrac_with_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.283185+01:00"));
}

TEST(Time_rfc3339_fulltime, valid_leap_second_zulu) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:60Z"));
}

TEST(Time_rfc3339_fulltime, valid_leap_second_negative_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("15:59:60-08:00"));
}

TEST(Time_rfc3339_fulltime, valid_leap_second_positive_offset_wraparound) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("01:29:60+01:30"));
}

TEST(Time_rfc3339_fulltime, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(""));
}

TEST(Time_rfc3339_fulltime, invalid_no_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00"));
}

TEST(Time_rfc3339_fulltime, invalid_no_offset_with_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00.5"));
}

TEST(Time_rfc3339_fulltime, invalid_empty_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00.Z"));
}

TEST(Time_rfc3339_fulltime, invalid_one_digit_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("4:30:00Z"));
}

TEST(Time_rfc3339_fulltime, invalid_one_digit_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:3:00Z"));
}

TEST(Time_rfc3339_fulltime, invalid_one_digit_second) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:0Z"));
}

TEST(Time_rfc3339_fulltime, invalid_dot_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14.30.00Z"));
}

TEST(Time_rfc3339_fulltime, invalid_colonless_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+0530"));
}

TEST(Time_rfc3339_fulltime, invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00Z "));
}

TEST(Time_rfc3339_fulltime, invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(" 14:30:00Z"));
}

TEST(Time_rfc3339_fulltime, invalid_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("24:00:00Z"));
}

TEST(Time_rfc3339_fulltime, invalid_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("00:60:00Z"));
}

TEST(Time_rfc3339_fulltime, invalid_second_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("00:00:61Z"));
}

TEST(Time_rfc3339_fulltime, invalid_offset_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+24:00"));
}

TEST(Time_rfc3339_fulltime, invalid_offset_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+00:60"));
}

TEST(Time_rfc3339_fulltime, invalid_leap_second_wrong_hour_zulu) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("22:59:60Z"));
}

TEST(Time_rfc3339_fulltime, invalid_leap_second_wrong_minute_zulu) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:58:60Z"));
}

TEST(Time_rfc3339_fulltime, invalid_leap_second_wrong_offset_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60+01:00"));
}

TEST(Time_rfc3339_fulltime, invalid_leap_second_wrong_offset_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60+00:30"));
}

TEST(Time_rfc3339_fulltime, suite_valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_extra_leading_zeros) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("008:030:006Z"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_no_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("8:3:6Z"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_mixed_field_widths) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("8:0030:6Z"));
}

TEST(Time_rfc3339_fulltime, suite_valid_leap_second_zero_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:60+00:00"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_leap_wrong_hour_zero_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("22:59:60+00:00"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_leap_wrong_minute_zero_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:58:60+00:00"));
}

TEST(Time_rfc3339_fulltime, suite_valid_leap_large_positive_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:29:60+23:30"));
}

TEST(Time_rfc3339_fulltime, suite_valid_leap_large_negative_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("00:29:60-23:30"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_leap_wrong_hour_negative_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60-01:00"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_leap_wrong_minute_negative_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60-00:30"));
}

TEST(Time_rfc3339_fulltime, suite_valid_short_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:20:50.52Z"));
}

TEST(Time_rfc3339_fulltime, suite_valid_precise_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.283185Z"));
}

TEST(Time_rfc3339_fulltime, suite_valid_plus_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00:20"));
}

TEST(Time_rfc3339_fulltime, suite_valid_unknown_local_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("12:34:56-00:00"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_offset_field_widths) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06-8:000"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_both_z_and_numoffset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("01:02:03Z+00:30"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_offset_indicator_pst) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06 PST"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_iso8601_comma_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("01:01:01,1111"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_bengali_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("1\xe0\xa7\xa8"
                                                     ":00:00Z"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_hash_offset_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06#00:20"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_letters) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("ab:cd:ef"));
}

TEST(Time_rfc3339_fulltime, suite_invalid_datetime_passed_in) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("2020-11-28T23:55:45Z"));
}
