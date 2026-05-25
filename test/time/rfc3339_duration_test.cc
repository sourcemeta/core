#include <gtest/gtest.h>

#include <sourcemeta/core/time.h>

TEST(Time_rfc3339_duration, valid_year_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y"));
}

TEST(Time_rfc3339_duration, valid_month_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M"));
}

TEST(Time_rfc3339_duration, valid_day_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1D"));
}

TEST(Time_rfc3339_duration, valid_week_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1W"));
}

TEST(Time_rfc3339_duration, valid_year_month) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M"));
}

TEST(Time_rfc3339_duration, valid_year_month_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3D"));
}

TEST(Time_rfc3339_duration, valid_month_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2D"));
}

TEST(Time_rfc3339_duration, valid_zero_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P0D"));
}

TEST(Time_rfc3339_duration, valid_multi_digit_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P100Y"));
}

TEST(Time_rfc3339_duration, valid_leading_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P01D"));
}

TEST(Time_rfc3339_duration, valid_hour_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1H"));
}

TEST(Time_rfc3339_duration, valid_minute_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1M"));
}

TEST(Time_rfc3339_duration, valid_second_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1S"));
}

TEST(Time_rfc3339_duration, valid_zero_seconds) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT0S"));
}

TEST(Time_rfc3339_duration, valid_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1H30M"));
}

TEST(Time_rfc3339_duration, valid_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1H30M45S"));
}

TEST(Time_rfc3339_duration, valid_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1M30S"));
}

TEST(Time_rfc3339_duration, valid_large_hours) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT36H"));
}

TEST(Time_rfc3339_duration, valid_day_with_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT12H"));
}

TEST(Time_rfc3339_duration, valid_year_with_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT1H"));
}

TEST(Time_rfc3339_duration, valid_all_components) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT4H5M6S"));
}

TEST(Time_rfc3339_duration, valid_multi_digit_all) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P10Y10M10DT10H10M10S"));
}

TEST(Time_rfc3339_duration, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(""));
}

TEST(Time_rfc3339_duration, invalid_only_p) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"));
}

TEST(Time_rfc3339_duration, invalid_only_pt) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT"));
}

TEST(Time_rfc3339_duration, invalid_t_no_time_units) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1YT"));
}

TEST(Time_rfc3339_duration, invalid_no_p_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("1Y"));
}

TEST(Time_rfc3339_duration, invalid_lowercase_p) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("p1Y"));
}

TEST(Time_rfc3339_duration, invalid_digit_without_unit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1"));
}

TEST(Time_rfc3339_duration, invalid_unit_without_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PY"));
}

TEST(Time_rfc3339_duration, invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(" P1D"));
}

TEST(Time_rfc3339_duration, invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D "));
}

TEST(Time_rfc3339_duration, invalid_internal_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y 2M"));
}

TEST(Time_rfc3339_duration, invalid_year_after_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2Y"));
}

TEST(Time_rfc3339_duration, invalid_month_after_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2M"));
}

TEST(Time_rfc3339_duration, invalid_year_skip_to_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2D"));
}

TEST(Time_rfc3339_duration, invalid_hour_skip_to_second) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H2S"));
}

TEST(Time_rfc3339_duration, invalid_duplicate_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2Y"));
}

TEST(Time_rfc3339_duration, invalid_duplicate_minute_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M2M"));
}

TEST(Time_rfc3339_duration, invalid_day_in_time_section) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1D"));
}

TEST(Time_rfc3339_duration, invalid_hour_in_date_section) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1H"));
}

TEST(Time_rfc3339_duration, invalid_second_in_date_section) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1S"));
}

TEST(Time_rfc3339_duration, invalid_missing_t_with_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2H"));
}

TEST(Time_rfc3339_duration, invalid_year_then_week) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2W"));
}

TEST(Time_rfc3339_duration, invalid_week_then_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W2D"));
}

TEST(Time_rfc3339_duration, invalid_week_then_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1WT1H"));
}

TEST(Time_rfc3339_duration, invalid_fractional_seconds) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0.5S"));
}

TEST(Time_rfc3339_duration, invalid_fractional_years) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5Y"));
}

TEST(Time_rfc3339_duration, invalid_negative_value) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P-1D"));
}

TEST(Time_rfc3339_duration, invalid_positive_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P+1D"));
}

TEST(Time_rfc3339_duration, invalid_bengali_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xe0\xa7\xa8"
                                                     "Y"));
}

TEST(Time_rfc3339_duration, invalid_arabic_indic_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xd9\xa1"
                                                     "Y"));
}

TEST(Time_rfc3339_duration, invalid_unknown_letter_date) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1X"));
}

TEST(Time_rfc3339_duration, invalid_unknown_letter_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1X"));
}

TEST(Time_rfc3339_duration, invalid_lowercase_unit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1y"));
}

TEST(Time_rfc3339_duration, invalid_lowercase_t) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Dt1H"));
}

TEST(Time_rfc3339_duration, suite_valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P4DT12H30M5S"));
}

TEST(Time_rfc3339_duration, suite_invalid_no_p_prefix_full) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("4DT12H30M5S"));
}

TEST(Time_rfc3339_duration, suite_valid_four_years) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P4Y"));
}

TEST(Time_rfc3339_duration, suite_valid_two_weeks) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P2W"));
}
