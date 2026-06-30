#include <sourcemeta/core/test.h>

#include <sourcemeta/core/time.h>

TEST(valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06"));
}

TEST(valid_min_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("00:00:00"));
}

TEST(valid_max_normal_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("23:59:59"));
}

TEST(valid_leap_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("23:59:60"));
}

TEST(invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac(""));
}

TEST(invalid_with_secfrac) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06.5"));
}

TEST(invalid_with_long_secfrac) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06.283185"));
}

TEST(invalid_with_zulu) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06Z"));
}

TEST(invalid_with_lowercase_z) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06z"));
}

TEST(invalid_with_positive_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14:30:00+05:30"));
}

TEST(invalid_with_negative_offset) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14:30:00-08:00"));
}

TEST(invalid_one_digit_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("4:30:00"));
}

TEST(invalid_one_digit_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14:3:00"));
}

TEST(invalid_one_digit_second) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14:30:0"));
}

TEST(invalid_dot_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("14.30.00"));
}

TEST(invalid_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("24:00:00"));
}

TEST(invalid_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("00:60:00"));
}

TEST(invalid_second_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("00:00:61"));
}

TEST(invalid_leap_second_wrong_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("22:59:60"));
}

TEST(invalid_leap_second_wrong_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("23:58:60"));
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06 "));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac(" 08:30:06"));
}

TEST(invalid_letters) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("ab:cd:ef"));
}

TEST(invalid_extra_leading_zeros) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("008:030:06"));
}

TEST(invalid_no_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("8:3:6"));
}

TEST(invalid_bengali_digit) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_partialtime_no_secfrac("1\xe0\xa7\xa8"
                                                          ":00:00"));
}

TEST(suite_valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("08:30:06"));
}

TEST(suite_invalid_am_pm) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac("8:30 AM"));
}

TEST(suite_invalid_datetime_passed_in) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_partialtime_no_secfrac(
      "2020-11-28T23:55:45Z"));
}
