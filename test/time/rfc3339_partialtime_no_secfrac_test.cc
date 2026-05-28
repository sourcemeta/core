#include <gtest/gtest.h>

#include <sourcemeta/core/time.h>

TEST(Time_rfc3339_partialtime_no_secfrac, valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, valid_min_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("00:00:00"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, valid_max_normal_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("23:59:59"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, valid_leap_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("23:59:60"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac(""));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_with_secfrac) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06.5"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_with_long_secfrac) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06.283185"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_with_zulu) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06Z"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_with_lowercase_z) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06z"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_with_positive_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14:30:00+05:30"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_with_negative_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14:30:00-08:00"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_one_digit_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("4:30:00"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_one_digit_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14:3:00"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_one_digit_second) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14:30:0"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_dot_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14.30.00"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("24:00:00"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("00:60:00"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_second_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("00:00:61"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_leap_second_wrong_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("22:59:60"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_leap_second_wrong_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("23:58:60"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_trailing_space) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06 "));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_leading_space) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac(" 08:30:06"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_letters) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("ab:cd:ef"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_extra_leading_zeros) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("008:030:06"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_no_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("8:3:6"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, invalid_bengali_digit) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("1\xe0\xa7\xa8"
                                                          ":00:00"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, suite_valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, suite_invalid_am_pm) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("8:30 AM"));
}

TEST(Time_rfc3339_partialtime_no_secfrac, suite_invalid_datetime_passed_in) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac(
      "2020-11-28T23:55:45Z"));
}
