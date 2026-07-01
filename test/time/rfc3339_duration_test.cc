#include <sourcemeta/core/test.h>
#include <sourcemeta/core/time.h>

TEST(valid_year_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y"));
}

TEST(valid_month_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M"));
}

TEST(valid_day_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1D"));
}

TEST(valid_week_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1W"));
}

TEST(valid_year_month) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M"));
}

TEST(valid_year_month_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3D"));
}

TEST(valid_month_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2D"));
}

TEST(valid_zero_day) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P0D"));
}

TEST(valid_multi_digit_year) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P100Y"));
}

TEST(valid_leading_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P01D"));
}

TEST(valid_hour_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1H"));
}

TEST(valid_minute_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1M"));
}

TEST(valid_second_only) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1S"));
}

TEST(valid_zero_seconds) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT0S"));
}

TEST(valid_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1H30M"));
}

TEST(valid_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1H30M45S"));
}

TEST(valid_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1M30S"));
}

TEST(valid_large_hours) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT36H"));
}

TEST(valid_day_with_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT12H"));
}

TEST(valid_year_with_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT1H"));
}

TEST(valid_all_components) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT4H5M6S"));
}

TEST(valid_multi_digit_all) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P10Y10M10DT10H10M10S"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("")); }

TEST(invalid_only_p) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"));
}

TEST(invalid_only_pt) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT"));
}

TEST(invalid_t_no_time_units) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1YT"));
}

TEST(invalid_no_p_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("1Y"));
}

TEST(invalid_lowercase_p) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("p1Y"));
}

TEST(invalid_digit_without_unit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1"));
}

TEST(invalid_unit_without_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PY"));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(" P1D"));
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D "));
}

TEST(invalid_internal_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y 2M"));
}

TEST(invalid_year_after_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2Y"));
}

TEST(invalid_month_after_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2M"));
}

TEST(invalid_year_skip_to_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2D"));
}

TEST(invalid_hour_skip_to_second) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H2S"));
}

TEST(invalid_duplicate_year) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2Y"));
}

TEST(invalid_duplicate_minute_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M2M"));
}

TEST(invalid_day_in_time_section) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1D"));
}

TEST(invalid_hour_in_date_section) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1H"));
}

TEST(invalid_second_in_date_section) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1S"));
}

TEST(invalid_missing_t_with_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2H"));
}

TEST(invalid_year_then_week) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2W"));
}

TEST(invalid_week_then_day) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W2D"));
}

TEST(invalid_week_then_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1WT1H"));
}

TEST(invalid_fractional_seconds) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0.5S"));
}

TEST(invalid_fractional_years) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5Y"));
}

TEST(invalid_negative_value) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P-1D"));
}

TEST(invalid_positive_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P+1D"));
}

TEST(invalid_bengali_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xe0\xa7\xa8"
                                                     "Y"));
}

TEST(invalid_arabic_indic_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xd9\xa1"
                                                     "Y"));
}

TEST(invalid_unknown_letter_date) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1X"));
}

TEST(invalid_unknown_letter_time) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1X"));
}

TEST(invalid_lowercase_unit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1y"));
}

TEST(invalid_lowercase_t) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Dt1H"));
}

TEST(suite_valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P4DT12H30M5S"));
}

TEST(suite_invalid_no_p_prefix_full) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("4DT12H30M5S"));
}

TEST(suite_valid_four_years) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P4Y"));
}

TEST(suite_valid_two_weeks) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P2W"));
}
