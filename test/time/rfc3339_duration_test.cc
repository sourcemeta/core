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

// Exhaustive RFC 3339 Appendix A duration coverage (per-mechanism variants).
// Each verdict matches sourcemeta::core::is_rfc3339_duration as built.

// valid structural shapes (date x time combinations)
TEST(Time_rfc3339_duration, ext_valid_day_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT4H"));
}
TEST(Time_rfc3339_duration, ext_valid_day_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT4H5M"));
}
TEST(Time_rfc3339_duration, ext_valid_day_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT4H5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_day_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT5M"));
}
TEST(Time_rfc3339_duration, ext_valid_day_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_day_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT6S"));
}
TEST(Time_rfc3339_duration, ext_valid_month_day_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT4H"));
}
TEST(Time_rfc3339_duration, ext_valid_month_day_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT4H5M"));
}
TEST(Time_rfc3339_duration, ext_valid_month_day_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT4H5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_month_day_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT5M"));
}
TEST(Time_rfc3339_duration, ext_valid_month_day_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_month_day_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT6S"));
}
TEST(Time_rfc3339_duration, ext_valid_month_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT4H"));
}
TEST(Time_rfc3339_duration, ext_valid_month_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT4H5M"));
}
TEST(Time_rfc3339_duration, ext_valid_month_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT4H5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_month_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT5M"));
}
TEST(Time_rfc3339_duration, ext_valid_month_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_month_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT6S"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_day_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT4H"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_day_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT4H5M"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_day_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT5M"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_day_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_day_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT6S"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT4H"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT4H5M"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT4H5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT5M"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_year_month_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT6S"));
}
TEST(Time_rfc3339_duration, ext_valid_year_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT4H"));
}
TEST(Time_rfc3339_duration, ext_valid_year_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT4H5M"));
}
TEST(Time_rfc3339_duration, ext_valid_year_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT4H5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_year_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT5M"));
}
TEST(Time_rfc3339_duration, ext_valid_year_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_year_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT6S"));
}
TEST(Time_rfc3339_duration, ext_valid_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT4H"));
}
TEST(Time_rfc3339_duration, ext_valid_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT4H5M"));
}
TEST(Time_rfc3339_duration, ext_valid_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT4H5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT5M"));
}
TEST(Time_rfc3339_duration, ext_valid_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT5M6S"));
}
TEST(Time_rfc3339_duration, ext_valid_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT6S"));
}

// boundary values (zero, leading zero, large counts - 1*DIGIT is unbounded)
TEST(Time_rfc3339_duration, ext_valid_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P0W"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_2) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P00W"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_3) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P000Y"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_4) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P0Y0M0DT0H0M0S"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_5) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P2147483648D"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_6) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P4294967296D"));
}
TEST(Time_rfc3339_duration, ext_invalid_boundary) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P9223372036854775808S"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_7) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P18446744073709551616Y"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_8) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P99999999999999999999W"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_9) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P2147483648M"));
}
TEST(Time_rfc3339_duration, ext_valid_boundary_10) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P999999999999999999999D"));
}

// leading sign (no sign in Appendix A)
TEST(Time_rfc3339_duration, ext_invalid_leading_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_leading_sign_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("+P1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_leading_sign_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-PT1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_leading_sign_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1Y2M3DT4H5M6S"));
}
TEST(Time_rfc3339_duration, ext_invalid_leading_sign_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1W"));
}

// internal sign
TEST(Time_rfc3339_duration, ext_invalid_internal_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT-1S"));
}
TEST(Time_rfc3339_duration, ext_invalid_internal_sign_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y-2M"));
}
TEST(Time_rfc3339_duration, ext_invalid_internal_sign_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P--1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_internal_sign_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("+-P1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_internal_sign_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P"));
}
TEST(Time_rfc3339_duration, ext_invalid_internal_sign_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("+P"));
}

// fraction with dot and comma (Appendix A is 1*DIGIT only)
TEST(Time_rfc3339_duration, ext_invalid_fraction) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0,5S"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0,5Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5M"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.5H"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.5M"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2.5W"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5D"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_9) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT.5S"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_10) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P.5D"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_11) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.D"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_12) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5DT1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_13) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0.5H30M"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_14) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1..5D"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_15) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5.5D"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_16) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5M"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_17) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5Y1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_18) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("PT10000000000000000000.999S"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_19) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.S"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_20) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5W"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_21) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5Y2M"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_22) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5Y2.1M"));
}
TEST(Time_rfc3339_duration, ext_invalid_fraction_23) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P15DT1H22M1.5S"));
}

// scientific-notation exponent
TEST(Time_rfc3339_duration, ext_invalid_exponent) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e2D"));
}
TEST(Time_rfc3339_duration, ext_invalid_exponent_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E2D"));
}
TEST(Time_rfc3339_duration, ext_invalid_exponent_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e1S"));
}
TEST(Time_rfc3339_duration, ext_invalid_exponent_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e-2D"));
}
TEST(Time_rfc3339_duration, ext_invalid_exponent_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e+2D"));
}
TEST(Time_rfc3339_duration, ext_invalid_exponent_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1eD"));
}
TEST(Time_rfc3339_duration, ext_invalid_exponent_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E999999D"));
}

// very large exponent
TEST(Time_rfc3339_duration, ext_invalid_large_exponent) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E1000000D"));
}
TEST(Time_rfc3339_duration, ext_invalid_large_exponent_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1E1000000S"));
}

// week combined with another component (week is exclusive)
TEST(Time_rfc3339_duration, ext_invalid_week_combo) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_combo_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_combo_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_combo_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_combo_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_combo_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_combo_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P5D2W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_combo_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1DT13H23M34S"));
}

// week after a zero-valued component
TEST(Time_rfc3339_duration, ext_invalid_week_after_zero) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0Y1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_after_zero_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0Y0M1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_after_zero_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0M1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_after_zero_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0D1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_week_after_zero_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0Y0M0D1W"));
}

// a number before the T separator with no unit
TEST(Time_rfc3339_duration, ext_invalid_dangling_before_t) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2T3H"));
}
TEST(Time_rfc3339_duration, ext_invalid_dangling_before_t_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2T3M"));
}
TEST(Time_rfc3339_duration, ext_invalid_dangling_before_t_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2T1M"));
}
TEST(Time_rfc3339_duration, ext_invalid_dangling_before_t_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2MET5M6S"));
}
TEST(Time_rfc3339_duration, ext_invalid_dangling_before_t_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PE"));
}
TEST(Time_rfc3339_duration, ext_invalid_dangling_before_t_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTE"));
}

// a number with no trailing unit
TEST(Time_rfc3339_duration, ext_invalid_trailing_digits) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1MT1"));
}
TEST(Time_rfc3339_duration, ext_invalid_trailing_digits_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT000022.22"));
}

// fixed component order (Y->M->D, H->M->S)
TEST(Time_rfc3339_duration, ext_invalid_order) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_order_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1M"));
}
TEST(Time_rfc3339_duration, ext_invalid_order_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2D1Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_order_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_order_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S1M"));
}
TEST(Time_rfc3339_duration, ext_invalid_order_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_order_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1YT1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_order_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1MT1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_order_9) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1S1M"));
}

// repeated designator
TEST(Time_rfc3339_duration, ext_invalid_repeat) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M2M"));
}
TEST(Time_rfc3339_duration, ext_invalid_repeat_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2D"));
}
TEST(Time_rfc3339_duration, ext_invalid_repeat_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H2H"));
}
TEST(Time_rfc3339_duration, ext_invalid_repeat_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S2S"));
}
TEST(Time_rfc3339_duration, ext_invalid_repeat_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W2W"));
}
TEST(Time_rfc3339_duration, ext_invalid_repeat_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2M3M"));
}

// designator in the wrong section
TEST(Time_rfc3339_duration, ext_invalid_cross_placement) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2S"));
}
TEST(Time_rfc3339_duration, ext_invalid_cross_placement_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1H1M"));
}
TEST(Time_rfc3339_duration, ext_invalid_cross_placement_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_cross_placement_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1W"));
}
TEST(Time_rfc3339_duration, ext_invalid_cross_placement_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DT2D"));
}
TEST(Time_rfc3339_duration, ext_invalid_cross_placement_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DT1Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_cross_placement_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M1D"));
}

// T separator edge cases
TEST(Time_rfc3339_duration, ext_invalid_t_edge) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DT"));
}
TEST(Time_rfc3339_duration, ext_invalid_t_edge_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTT1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_t_edge_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1HT2M"));
}
TEST(Time_rfc3339_duration, ext_invalid_t_edge_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("T"));
}
TEST(Time_rfc3339_duration, ext_invalid_t_edge_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("TP1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_t_edge_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1YT "));
}
TEST(Time_rfc3339_duration, ext_invalid_t_edge_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1MT1S"));
}

// the P prefix requirement
TEST(Time_rfc3339_duration, ext_invalid_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("T1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_prefix_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_prefix_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("D"));
}
TEST(Time_rfc3339_duration, ext_invalid_prefix_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PP1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_prefix_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DP"));
}
TEST(Time_rfc3339_duration, ext_invalid_prefix_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("XP1D"));
}

// missing number or missing unit
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PD"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PW"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PYD"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1YM"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTS"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DY"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1HM"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_9) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2"));
}
TEST(Time_rfc3339_duration, ext_invalid_no_number_or_unit_10) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M2"));
}

// whitespace
TEST(Time_rfc3339_duration, ext_invalid_whitespace) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P 1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_whitespace_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 D"));
}
TEST(Time_rfc3339_duration, ext_invalid_whitespace_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D"
                                                     "\x09"));
}
TEST(Time_rfc3339_duration, ext_invalid_whitespace_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x09"
                                                     "P1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_whitespace_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xc2\xa0"
                                                     "1D"));
}

// leading and trailing newline
TEST(Time_rfc3339_duration, ext_invalid_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D"
                                                     "\x0a"));
}
TEST(Time_rfc3339_duration, ext_invalid_newline_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D"
                                                     "\x0d\x0a"));
}
TEST(Time_rfc3339_duration, ext_invalid_newline_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0a"
                                                     "P1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_newline_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DT1H"
                                                     "\x0a"));
}

// BOM, NUL and control bytes
TEST(Time_rfc3339_duration, ext_invalid_encoding) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\xef\xbb\xbf"
                                                     "P1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_encoding_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(std::string_view("P1D"
                                                                      "\x00",
                                                                      4)));
}
TEST(Time_rfc3339_duration, ext_invalid_encoding_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(std::string_view("P"
                                                                      "\x00"
                                                                      "1D",
                                                                      4)));
}

// non-ASCII digit families (DIGIT is %x30-39 only)
TEST(Time_rfc3339_duration, ext_invalid_unicode_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xe0\xa7\xa8"
                                                     "Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_unicode_digit_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xe0\xa7\xa7"
                                                     "D"));
}
TEST(Time_rfc3339_duration, ext_invalid_unicode_digit_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xef\xbc\x91"
                                                     "D"));
}
TEST(Time_rfc3339_duration, ext_invalid_unicode_digit_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xd9\xa2"
                                                     "D"));
}
TEST(Time_rfc3339_duration, ext_invalid_unicode_digit_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xc2\xb2"
                                                     "D"));
}
TEST(Time_rfc3339_duration, ext_invalid_unicode_digit_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT"
                                                     "\xe0\xa7\xa7"
                                                     "H"));
}

// the ISO 8601 alternative datetime form (not in Appendix A)
TEST(Time_rfc3339_duration, ext_invalid_alternative_datetime_form) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0001-02-03T04:05:06"));
}
TEST(Time_rfc3339_duration, ext_invalid_alternative_datetime_form_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P00010203T040506"));
}
TEST(Time_rfc3339_duration, ext_invalid_alternative_datetime_form_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT04:05:06"));
}
TEST(Time_rfc3339_duration, ext_invalid_alternative_datetime_form_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2023-01-01T00:00:00"));
}
TEST(Time_rfc3339_duration, ext_invalid_alternative_datetime_form_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P20230101T000000S"));
}
TEST(Time_rfc3339_duration, ext_invalid_alternative_datetime_form_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0000-01-00"));
}
TEST(Time_rfc3339_duration, ext_invalid_alternative_datetime_form_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2020-123T00:00"));
}

// week valid in Appendix A but rejected by xs:duration
TEST(Time_rfc3339_duration, ext_valid_week) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P52W"));
}

// sign accepted by xs:duration, invalid in Appendix A
TEST(Time_rfc3339_duration, ext_invalid_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1Y"));
}
TEST(Time_rfc3339_duration, ext_invalid_sign_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1Y2M3D"));
}
TEST(Time_rfc3339_duration, ext_invalid_sign_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-PT1H30M"));
}

// comma decimal (ISO 8601 only)
TEST(Time_rfc3339_duration, ext_invalid_comma) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1,5S"));
}
TEST(Time_rfc3339_duration, ext_invalid_comma_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,5DT2H"));
}

// ISO alternative datetime form
TEST(Time_rfc3339_duration, ext_invalid_alternative_datetime_form_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0002-03-04T05:06:07"));
}

// a skipped intermediate component
TEST(Time_rfc3339_duration, ext_invalid_skipped_component) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2DT1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_skipped_component_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1DT1H"));
}
TEST(Time_rfc3339_duration, ext_invalid_skipped_component_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H1S"));
}

// non-duration input
TEST(Time_rfc3339_duration, ext_invalid_garbage) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("1 month"));
}
TEST(Time_rfc3339_duration, ext_invalid_garbage_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 month"));
}
TEST(Time_rfc3339_duration, ext_invalid_garbage_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("Pone"));
}
TEST(Time_rfc3339_duration, ext_invalid_garbage_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H30M5"));
}
TEST(Time_rfc3339_duration, ext_invalid_garbage_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2M3"));
}
TEST(Time_rfc3339_duration, ext_invalid_garbage_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xf0\x9f\x94\xa5"
                                                     "D"));
}
TEST(Time_rfc3339_duration, ext_invalid_garbage_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 1D"));
}
TEST(Time_rfc3339_duration, ext_invalid_garbage_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-111111"));
}
