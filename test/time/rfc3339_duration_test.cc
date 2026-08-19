#include <sourcemeta/core/test.h>
#include <sourcemeta/core/time.h>

#include <string_view>

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

TEST(ext_valid_day_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT4H"));
}

TEST(ext_valid_day_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT4H5M"));
}

TEST(ext_valid_day_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT4H5M6S"));
}

TEST(ext_valid_day_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT5M"));
}

TEST(ext_valid_day_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT5M6S"));
}

TEST(ext_valid_day_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT6S"));
}

TEST(ext_valid_month_day_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT4H"));
}

TEST(ext_valid_month_day_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT4H5M"));
}

TEST(ext_valid_month_day_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT4H5M6S"));
}

TEST(ext_valid_month_day_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT5M"));
}

TEST(ext_valid_month_day_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT5M6S"));
}

TEST(ext_valid_month_day_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M2DT6S"));
}

TEST(ext_valid_month_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT4H"));
}

TEST(ext_valid_month_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT4H5M"));
}

TEST(ext_valid_month_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT4H5M6S"));
}

TEST(ext_valid_month_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT5M"));
}

TEST(ext_valid_month_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT5M6S"));
}

TEST(ext_valid_month_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT6S"));
}

TEST(ext_valid_year_month_day_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT4H"));
}

TEST(ext_valid_year_month_day_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT4H5M"));
}

TEST(ext_valid_year_month_day_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT5M"));
}

TEST(ext_valid_year_month_day_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT5M6S"));
}

TEST(ext_valid_year_month_day_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2M3DT6S"));
}

TEST(ext_valid_year_month_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT4H"));
}

TEST(ext_valid_year_month_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT4H5M"));
}

TEST(ext_valid_year_month_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT4H5M6S"));
}

TEST(ext_valid_year_month_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT5M"));
}

TEST(ext_valid_year_month_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT5M6S"));
}

TEST(ext_valid_year_month_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y2MT6S"));
}

TEST(ext_valid_year_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT4H"));
}

TEST(ext_valid_year_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT4H5M"));
}

TEST(ext_valid_year_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT4H5M6S"));
}

TEST(ext_valid_year_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT5M"));
}

TEST(ext_valid_year_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT5M6S"));
}

TEST(ext_valid_year_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT6S"));
}

TEST(ext_valid_t_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT4H"));
}

TEST(ext_valid_t_hour_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT4H5M"));
}

TEST(ext_valid_t_hour_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT4H5M6S"));
}

TEST(ext_valid_t_minute) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT5M"));
}

TEST(ext_valid_t_minute_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT5M6S"));
}

TEST(ext_valid_t_second) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT6S"));
}

TEST(ext_valid_boundary) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P0W"));
}

TEST(ext_valid_boundary_2) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P00W"));
}

TEST(ext_valid_boundary_3) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P000Y"));
}

TEST(ext_valid_boundary_4) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P0Y0M0DT0H0M0S"));
}

TEST(ext_valid_boundary_5) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P2147483648D"));
}

TEST(ext_valid_boundary_6) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P4294967296D"));
}

TEST(ext_invalid_boundary) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P9223372036854775808S"));
}

TEST(ext_valid_boundary_7) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P18446744073709551616Y"));
}

TEST(ext_valid_boundary_8) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P99999999999999999999W"));
}

TEST(ext_valid_boundary_9) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P2147483648M"));
}

TEST(ext_valid_boundary_10) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P999999999999999999999D"));
}

TEST(ext_invalid_leading_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1D"));
}

TEST(ext_invalid_leading_sign_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("+P1D"));
}

TEST(ext_invalid_leading_sign_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-PT1H"));
}

TEST(ext_invalid_leading_sign_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1Y2M3DT4H5M6S"));
}

TEST(ext_invalid_leading_sign_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1W"));
}

TEST(ext_invalid_internal_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT-1S"));
}

TEST(ext_invalid_internal_sign_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y-2M"));
}

TEST(ext_invalid_internal_sign_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P--1D"));
}

TEST(ext_invalid_internal_sign_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("+-P1D"));
}

TEST(ext_invalid_internal_sign_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P"));
}

TEST(ext_invalid_internal_sign_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("+P"));
}

TEST(ext_invalid_fraction) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0,5S"));
}

TEST(ext_invalid_fraction_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5Y"));
}

TEST(ext_invalid_fraction_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0,5Y"));
}

TEST(ext_invalid_fraction_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5M"));
}

TEST(ext_invalid_fraction_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.5H"));
}

TEST(ext_invalid_fraction_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.5M"));
}

TEST(ext_invalid_fraction_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2.5W"));
}

TEST(ext_invalid_fraction_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5D"));
}

TEST(ext_invalid_fraction_9) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT.5S"));
}

TEST(ext_invalid_fraction_10) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P.5D"));
}

TEST(ext_invalid_fraction_11) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.D"));
}

TEST(ext_invalid_fraction_12) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5DT1H"));
}

TEST(ext_invalid_fraction_13) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0.5H30M"));
}

TEST(ext_invalid_fraction_14) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1..5D"));
}

TEST(ext_invalid_fraction_15) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5.5D"));
}

TEST(ext_invalid_fraction_16) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5M"));
}

TEST(ext_invalid_fraction_17) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5Y1D"));
}

TEST(ext_invalid_fraction_18) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("PT10000000000000000000.999S"));
}

TEST(ext_invalid_fraction_19) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.S"));
}

TEST(ext_invalid_fraction_20) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5W"));
}

TEST(ext_invalid_fraction_21) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5Y2M"));
}

TEST(ext_invalid_fraction_22) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0.5Y2.1M"));
}

TEST(ext_invalid_fraction_23) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P15DT1H22M1.5S"));
}

TEST(ext_invalid_exponent) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e2D"));
}

TEST(ext_invalid_exponent_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E2D"));
}

TEST(ext_invalid_exponent_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e1S"));
}

TEST(ext_invalid_exponent_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e-2D"));
}

TEST(ext_invalid_exponent_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e+2D"));
}

TEST(ext_invalid_exponent_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1eD"));
}

TEST(ext_invalid_exponent_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E999999D"));
}

TEST(ext_invalid_large_exponent) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E1000000D"));
}

TEST(ext_invalid_large_exponent_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1E1000000S"));
}

TEST(ext_invalid_week_combo) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1W"));
}

TEST(ext_invalid_week_combo_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1D"));
}

TEST(ext_invalid_week_combo_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1W"));
}

TEST(ext_invalid_week_combo_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1W"));
}

TEST(ext_invalid_week_combo_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1Y"));
}

TEST(ext_invalid_week_combo_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1W"));
}

TEST(ext_invalid_week_combo_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P5D2W"));
}

TEST(ext_invalid_week_combo_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1DT13H23M34S"));
}

TEST(ext_invalid_week_after_zero) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0Y1W"));
}

TEST(ext_invalid_week_after_zero_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0Y0M1W"));
}

TEST(ext_invalid_week_after_zero_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0M1W"));
}

TEST(ext_invalid_week_after_zero_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0D1W"));
}

TEST(ext_invalid_week_after_zero_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0Y0M0D1W"));
}

TEST(ext_invalid_dangling_before_t) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2T3H"));
}

TEST(ext_invalid_dangling_before_t_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2T3M"));
}

TEST(ext_invalid_dangling_before_t_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2T1M"));
}

TEST(ext_invalid_dangling_before_t_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2MET5M6S"));
}

TEST(ext_invalid_dangling_before_t_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PE"));
}

TEST(ext_invalid_dangling_before_t_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTE"));
}

TEST(ext_invalid_trailing_digits) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1MT1"));
}

TEST(ext_invalid_trailing_digits_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT000022.22"));
}

TEST(ext_invalid_order) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1Y"));
}

TEST(ext_invalid_order_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1M"));
}

TEST(ext_invalid_order_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2D1Y"));
}

TEST(ext_invalid_order_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M1H"));
}

TEST(ext_invalid_order_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S1M"));
}

TEST(ext_invalid_order_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S1H"));
}

TEST(ext_invalid_order_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1YT1H"));
}

TEST(ext_invalid_order_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1MT1H"));
}

TEST(ext_invalid_order_9) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1S1M"));
}

TEST(ext_invalid_repeat) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M2M"));
}

TEST(ext_invalid_repeat_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D2D"));
}

TEST(ext_invalid_repeat_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H2H"));
}

TEST(ext_invalid_repeat_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S2S"));
}

TEST(ext_invalid_repeat_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W2W"));
}

TEST(ext_invalid_repeat_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2M3M"));
}

TEST(ext_invalid_cross_placement) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2S"));
}

TEST(ext_invalid_cross_placement_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1H1M"));
}

TEST(ext_invalid_cross_placement_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1Y"));
}

TEST(ext_invalid_cross_placement_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1W"));
}

TEST(ext_invalid_cross_placement_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DT2D"));
}

TEST(ext_invalid_cross_placement_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DT1Y"));
}

TEST(ext_invalid_cross_placement_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M1D"));
}

TEST(ext_invalid_t_edge) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DT"));
}

TEST(ext_invalid_t_edge_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTT1H"));
}

TEST(ext_invalid_t_edge_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1HT2M"));
}

TEST(ext_invalid_t_edge_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("T"));
}

TEST(ext_invalid_t_edge_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("TP1D"));
}

TEST(ext_invalid_t_edge_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1YT "));
}

TEST(ext_invalid_t_edge_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1MT1S"));
}

TEST(ext_invalid_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("T1H"));
}

TEST(ext_invalid_prefix_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("1D"));
}

TEST(ext_invalid_prefix_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("D"));
}

TEST(ext_invalid_prefix_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PP1D"));
}

TEST(ext_invalid_prefix_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DP"));
}

TEST(ext_invalid_prefix_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("XP1D"));
}

TEST(ext_invalid_no_number_or_unit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PD"));
}

TEST(ext_invalid_no_number_or_unit_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PW"));
}

TEST(ext_invalid_no_number_or_unit_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PYD"));
}

TEST(ext_invalid_no_number_or_unit_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1YM"));
}

TEST(ext_invalid_no_number_or_unit_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1"));
}

TEST(ext_invalid_no_number_or_unit_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTS"));
}

TEST(ext_invalid_no_number_or_unit_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DY"));
}

TEST(ext_invalid_no_number_or_unit_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1HM"));
}

TEST(ext_invalid_no_number_or_unit_9) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2"));
}

TEST(ext_invalid_no_number_or_unit_10) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M2"));
}

TEST(ext_invalid_whitespace) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P 1D"));
}

TEST(ext_invalid_whitespace_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 D"));
}

TEST(ext_invalid_whitespace_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D"
                                                     "\x09"));
}

TEST(ext_invalid_whitespace_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x09"
                                                     "P1D"));
}

TEST(ext_invalid_whitespace_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xc2\xa0"
                                                     "1D"));
}

TEST(ext_invalid_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D"
                                                     "\x0a"));
}

TEST(ext_invalid_newline_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D"
                                                     "\x0d\x0a"));
}

TEST(ext_invalid_newline_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0a"
                                                     "P1D"));
}

TEST(ext_invalid_newline_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1DT1H"
                                                     "\x0a"));
}

TEST(ext_invalid_encoding) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\xef\xbb\xbf"
                                                     "P1D"));
}

TEST(ext_invalid_encoding_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(std::string_view("P1D"
                                                                      "\x00",
                                                                      4)));
}

TEST(ext_invalid_encoding_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(std::string_view("P"
                                                                      "\x00"
                                                                      "1D",
                                                                      4)));
}

TEST(ext_invalid_unicode_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xe0\xa7\xa8"
                                                     "Y"));
}

TEST(ext_invalid_unicode_digit_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xe0\xa7\xa7"
                                                     "D"));
}

TEST(ext_invalid_unicode_digit_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xef\xbc\x91"
                                                     "D"));
}

TEST(ext_invalid_unicode_digit_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xd9\xa2"
                                                     "D"));
}

TEST(ext_invalid_unicode_digit_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xc2\xb2"
                                                     "D"));
}

TEST(ext_invalid_unicode_digit_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT"
                                                     "\xe0\xa7\xa7"
                                                     "H"));
}

TEST(ext_invalid_alternative_datetime_form) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0001-02-03T04:05:06"));
}

TEST(ext_invalid_alternative_datetime_form_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P00010203T040506"));
}

TEST(ext_invalid_alternative_datetime_form_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT04:05:06"));
}

TEST(ext_invalid_alternative_datetime_form_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2023-01-01T00:00:00"));
}

TEST(ext_invalid_alternative_datetime_form_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P20230101T000000S"));
}

TEST(ext_invalid_alternative_datetime_form_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0000-01-00"));
}

TEST(ext_invalid_alternative_datetime_form_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2020-123T00:00"));
}

TEST(ext_valid_week) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P52W"));
}

TEST(ext_invalid_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1Y"));
}

TEST(ext_invalid_sign_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-P1Y2M3D"));
}

TEST(ext_invalid_sign_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-PT1H30M"));
}

TEST(ext_invalid_comma) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1,5S"));
}

TEST(ext_invalid_comma_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,5DT2H"));
}

TEST(ext_invalid_alternative_datetime_form_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0002-03-04T05:06:07"));
}

TEST(ext_invalid_skipped_component) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2DT1H"));
}

TEST(ext_invalid_skipped_component_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1DT1H"));
}

TEST(ext_invalid_skipped_component_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H1S"));
}

TEST(ext_invalid_garbage) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("1 month"));
}

TEST(ext_invalid_garbage_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 month"));
}

TEST(ext_invalid_garbage_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("Pone"));
}

TEST(ext_invalid_garbage_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H30M5"));
}

TEST(ext_invalid_garbage_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y2M3"));
}

TEST(ext_invalid_garbage_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P"
                                                     "\xf0\x9f\x94\xa5"
                                                     "D"));
}

TEST(ext_invalid_garbage_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 1D"));
}

TEST(ext_invalid_garbage_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("-111111"));
}

TEST(delta_u0000_u0050_day_0) {
  static constexpr char VALUE[] = "\x00P1D";
  static_assert(sizeof(VALUE) - 1 == 4);
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration(std::string_view{VALUE, 4}));
}

TEST(delta_u0000_u0050_day_u0000_1) {
  static constexpr char VALUE[] = "\x00P1D\x00";
  static_assert(sizeof(VALUE) - 1 == 5);
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration(std::string_view{VALUE, 5}));
}

TEST(delta_u0009_u0050_day_u0009_2) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x09P1D\x09"));
}

TEST(delta_u000a_u0050_day_u000a_3) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0aP1D\x0a"));
}

TEST(delta_u000b_u0050_day_4) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0bP1D"));
}

TEST(delta_u000b_u0050_day_u000b_5) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0bP1D\x0b"));
}

TEST(delta_u000c_u0050_day_6) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0cP1D"));
}

TEST(delta_u000c_u0050_day_u000c_7) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0cP1D\x0c"));
}

TEST(delta_u000d_u000a_u0050_day_8) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0d\x0aP1D"));
}

TEST(delta_u000d_u000a_u0050_day_u000d_u000a_9) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0d\x0aP1D\x0d\x0a"));
}

TEST(delta_u000d_u0050_day_10) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0dP1D"));
}

TEST(delta_u000d_u0050_day_u000d_11) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x0dP1D\x0d"));
}

TEST(delta_u001b_u0050_day_12) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x1bP1D"));
}

TEST(delta_u001b_u0050_day_u001b_13) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x1bP1D\x1b"));
}

TEST(delta_u0020_u0050_day_u0020_14) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(" P1D "));
}

TEST(delta_u0020_month_15) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P 1M"));
}

TEST(delta_u0020_week_16) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P 1W"));
}

TEST(delta_u0020_year_17) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P 1Y"));
}

TEST(delta_u002b_month_18) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P+1M"));
}

TEST(delta_u002b_week_19) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P+1W"));
}

TEST(delta_u002b_year_20) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P+1Y"));
}

TEST(delta_u002c_day_21) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P,5D"));
}

TEST(delta_u002c_month_22) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P,5M"));
}

TEST(delta_u002c_week_23) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P,5W"));
}

TEST(delta_u002c_year_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P,5Y"));
}

TEST(delta_u002d_month_25) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P-1M"));
}

TEST(delta_u002d_week_26) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P-1W"));
}

TEST(delta_u002d_year_27) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P-1Y"));
}

TEST(delta_u002e_month_28) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P.5M"));
}

TEST(delta_u002e_week_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P.5W"));
}

TEST(delta_u002e_year_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P.5Y"));
}

TEST(delta_day_31) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P000000000000000000001D"));
}

TEST(delta_day_32) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P00000000000000000000D"));
}

TEST(delta_day_33) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P000000001D"));
}

TEST(delta_day_34) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P00000000D"));
}

TEST(delta_day_35) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P001D"));
}

TEST(delta_day_36) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P00D"));
}

TEST(delta_u0078_day_37) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0x1D"));
}

TEST(delta_u0078_month_38) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0x1M"));
}

TEST(delta_u0078_week_39) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0x1W"));
}

TEST(delta_u0078_year_40) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P0x1Y"));
}

TEST(delta_u0000_day_41) {
  static constexpr char VALUE[] = "P1\x00"
                                  "D";
  static_assert(sizeof(VALUE) - 1 == 4);
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration(std::string_view{VALUE, 4}));
}

TEST(delta_u0009_day_42) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\x09"
                                                     "D"));
}

TEST(delta_u000a_day_43) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\x0a"
                                                     "D"));
}

TEST(delta_u000b_day_44) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\x0b"
                                                     "D"));
}

TEST(delta_u000c_day_45) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\x0c"
                                                     "D"));
}

TEST(delta_u000d_u000a_day_46) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\x0d\x0a"
                                                     "D"));
}

TEST(delta_u000d_day_47) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\x0d"
                                                     "D"));
}

TEST(delta_u001b_day_48) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\x1b"
                                                     "D"));
}

TEST(delta_u0020_month_49) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 M"));
}

TEST(delta_u0020_week_50) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 W"));
}

TEST(delta_u0020_year_51) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1 Y"));
}

TEST(delta_u002c_day_52) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,5D"));
}

TEST(delta_u002c_month_53) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,5M"));
}

TEST(delta_u002c_week_54) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,5W"));
}

TEST(delta_u002c_year_55) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,5Y"));
}

TEST(delta_u002c_day_56) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,D"));
}

TEST(delta_u002c_month_57) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,M"));
}

TEST(delta_u002c_week_58) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,W"));
}

TEST(delta_u002c_year_59) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1,Y"));
}

TEST(delta_u002d_day_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1-D"));
}

TEST(delta_u002d_month_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1-M"));
}

TEST(delta_u002d_week_62) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1-W"));
}

TEST(delta_u002d_year_63) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1-Y"));
}

TEST(delta_u002e_u002e_month_64) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1..5M"));
}

TEST(delta_u002e_u002e_week_65) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1..5W"));
}

TEST(delta_u002e_u002e_year_66) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1..5Y"));
}

TEST(delta_u002e_day_67) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.5D"));
}

TEST(delta_u002e_month_68) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.M"));
}

TEST(delta_u002e_week_69) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.W"));
}

TEST(delta_u002e_year_70) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1.Y"));
}

TEST(delta_day_t_hour_month_second_71) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P15DT5H0M20S"));
}

TEST(delta_day_72) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P18446744073709551615D"));
}

TEST(delta_day_73) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P18446744073709551616D"));
}

TEST(delta_u0041_74) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1A"));
}

TEST(delta_u0042_75) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1B"));
}

TEST(delta_u0043_76) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1C"));
}

TEST(delta_day_u0000_77) {
  static constexpr char VALUE[] = "P1D\x00";
  static_assert(sizeof(VALUE) - 1 == 4);
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration(std::string_view{VALUE, 4}));
}

TEST(delta_day_u000b_78) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D\x0b"));
}

TEST(delta_day_u000c_79) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D\x0c"));
}

TEST(delta_day_u000d_80) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D\x0d"));
}

TEST(delta_day_u001b_81) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D\x1b"));
}

TEST(delta_day_u002f_u0050_day_82) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D/P2D"));
}

TEST(delta_day_day_83) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1D"));
}

TEST(delta_day_hour_84) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1H"));
}

TEST(delta_day_second_85) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1S"));
}

TEST(delta_day_year_86) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D1Y"));
}

TEST(delta_day_t_hour_87) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT1H"));
}

TEST(delta_day_t_hour_month_88) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT1H1M"));
}

TEST(delta_day_t_hour_month_second_89) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT1H1M1S"));
}

TEST(delta_day_t_month_90) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT1M"));
}

TEST(delta_day_t_month_second_91) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT1M1S"));
}

TEST(delta_day_t_second_92) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1DT1S"));
}

TEST(delta_day_u007f_93) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D\x7f"));
}

TEST(delta_day_u00c2_u00a0_94) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1D\xc3\x82\xc2\xa0"));
}

TEST(delta_day_u00e2_u0080_u008b_95) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1D\xc3\xa2\xc2\x80\xc2\x8b"));
}

TEST(delta_day_u00e2_u0080_u00a8_96) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1D\xc3\xa2\xc2\x80\xc2\xa8"));
}

TEST(delta_day_u00e2_u0080_u00a9_97) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1D\xc3\xa2\xc2\x80\xc2\xa9"));
}

TEST(delta_day_u00e3_u0080_u0080_98) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1D\xc3\xa3\xc2\x80\xc2\x80"));
}

TEST(delta_day_u00ef_u00bb_u00bf_99) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1D\xc3\xaf\xc2\xbb\xc2\xbf"));
}

TEST(delta_u0045_100) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E"));
}

TEST(delta_u0045_month_101) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E2M"));
}

TEST(delta_u0045_week_102) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E2W"));
}

TEST(delta_u0045_year_103) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1E2Y"));
}

TEST(delta_u0046_104) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1F"));
}

TEST(delta_u0047_105) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1G"));
}

TEST(delta_u0049_106) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1I"));
}

TEST(delta_u004a_107) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1J"));
}

TEST(delta_u004b_108) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1K"));
}

TEST(delta_u004c_109) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1L"));
}

TEST(delta_month_day_110) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M1D"));
}

TEST(delta_month_day_t_hour_111) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M1DT1H"));
}

TEST(delta_month_day_t_hour_month_112) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M1DT1H1M"));
}

TEST(delta_month_day_t_hour_month_second_113) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M1DT1H1M1S"));
}

TEST(delta_month_day_t_month_114) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M1DT1M"));
}

TEST(delta_month_day_t_month_second_115) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M1DT1M1S"));
}

TEST(delta_month_day_t_second_116) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1M1DT1S"));
}

TEST(delta_month_hour_117) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1H"));
}

TEST(delta_month_month_118) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1M"));
}

TEST(delta_month_second_119) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1M1S"));
}

TEST(delta_month_t_hour_120) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT1H"));
}

TEST(delta_month_t_hour_month_121) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT1H1M"));
}

TEST(delta_month_t_hour_month_second_122) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT1H1M1S"));
}

TEST(delta_month_t_month_123) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT1M"));
}

TEST(delta_month_t_month_second_124) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT1M1S"));
}

TEST(delta_month_t_second_125) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1MT1S"));
}

TEST(delta_u004e_126) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1N"));
}

TEST(delta_u004f_127) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1O"));
}

TEST(delta_u0051_128) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Q"));
}

TEST(delta_u0052_129) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1R"));
}

TEST(delta_u0055_130) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1U"));
}

TEST(delta_u0056_131) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1V"));
}

TEST(delta_week_hour_132) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1H"));
}

TEST(delta_week_month_133) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1M"));
}

TEST(delta_week_second_134) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1W1S"));
}

TEST(delta_year_135) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1"));
}

TEST(delta_year_day_136) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1D"));
}

TEST(delta_year_hour_137) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1H"));
}

TEST(delta_year_month_138) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1M"));
}

TEST(delta_year_month_day_139) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1M1D"));
}

TEST(delta_year_month_day_t_hour_140) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1M1DT1H"));
}

TEST(delta_year_month_day_t_hour_month_141) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1M1DT1H1M"));
}

TEST(delta_year_month_day_t_hour_month_second_142) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1M1DT1H1M1S"));
}

TEST(delta_year_month_day_t_month_143) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1M1DT1M"));
}

TEST(delta_year_month_day_t_month_second_144) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1M1DT1M1S"));
}

TEST(delta_year_month_day_t_second_145) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1M1DT1S"));
}

TEST(delta_year_month_t_hour_146) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1MT1H"));
}

TEST(delta_year_month_t_hour_month_147) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1MT1H1M"));
}

TEST(delta_year_month_t_hour_month_second_148) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1MT1H1M1S"));
}

TEST(delta_year_month_t_month_149) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1MT1M"));
}

TEST(delta_year_month_t_month_second_150) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1MT1M1S"));
}

TEST(delta_year_month_t_second_151) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1Y1MT1S"));
}

TEST(delta_year_second_152) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1S"));
}

TEST(delta_year_year_153) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Y1Y"));
}

TEST(delta_year_t_hour_month_154) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT1H1M"));
}

TEST(delta_year_t_hour_month_second_155) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT1H1M1S"));
}

TEST(delta_year_t_month_156) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT1M"));
}

TEST(delta_year_t_month_second_157) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT1M1S"));
}

TEST(delta_year_t_second_158) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P1YT1S"));
}

TEST(delta_u005a_159) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1Z"));
}

TEST(delta_u005f_day_160) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1_0D"));
}

TEST(delta_u005f_month_161) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1_0M"));
}

TEST(delta_u005f_week_162) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1_0W"));
}

TEST(delta_u005f_year_163) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1_0Y"));
}

TEST(delta_day_164) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1d"));
}

TEST(delta_u0065_u002b_month_165) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e+2M"));
}

TEST(delta_u0065_u002b_week_166) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e+2W"));
}

TEST(delta_u0065_u002b_year_167) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e+2Y"));
}

TEST(delta_u0065_u002d_month_168) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e-2M"));
}

TEST(delta_u0065_u002d_week_169) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e-2W"));
}

TEST(delta_u0065_u002d_year_170) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e-2Y"));
}

TEST(delta_u0065_month_171) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e2M"));
}

TEST(delta_u0065_week_172) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e2W"));
}

TEST(delta_u0065_year_173) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1e2Y"));
}

TEST(delta_u0065_month_174) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1eM"));
}

TEST(delta_u0065_week_175) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1eW"));
}

TEST(delta_u0065_year_176) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1eY"));
}

TEST(delta_year_month_day_177) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1y2M3D"));
}

TEST(delta_u007f_day_178) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\x7f"
                                                     "D"));
}

TEST(delta_u00c2_u00a0_day_179) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\xc3\x82\xc2\xa0"
                                                     "D"));
}

TEST(delta_u00c5_u00bf_180) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\xc3\x85\xc2\xbf"));
}

TEST(delta_u00d9_u00a0_day_181) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\xc3\x99\xc2\xa0"
                                                     "D"));
}

TEST(delta_u00db_u00b6_day_182) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P1\xc3\x9b\xc2\xb6"
                                                     "D"));
}

TEST(delta_u00e0_u00a5_u00a8_day_183) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa0\xc2\xa5\xc2\xa8"
                                            "D"));
}

TEST(delta_u00e0_u00a7_u00ae_day_184) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa0\xc2\xa7\xc2\xae"
                                            "D"));
}

TEST(delta_u00e0_u00ab_u00aa_day_185) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa0\xc2\xab\xc2\xaa"
                                            "D"));
}

TEST(delta_u00e0_u00af_u00a6_day_186) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa0\xc2\xaf\xc2\xa6"
                                            "D"));
}

TEST(delta_u00e0_u00b1_u00ac_day_187) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa0\xc2\xb1\xc2\xac"
                                            "D"));
}

TEST(delta_u00e0_u00b5_u00a8_day_188) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa0\xc2\xb5\xc2\xa8"
                                            "D"));
}

TEST(delta_u00e0_u00b7_u00ae_day_189) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa0\xc2\xb7\xc2\xae"
                                            "D"));
}

TEST(delta_u00e0_u00bb_u0094_day_190) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa0\xc2\xbb\xc2\x94"
                                            "D"));
}

TEST(delta_u00e1_u0081_u0080_day_191) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\x81\xc2\x80"
                                            "D"));
}

TEST(delta_u00e1_u0082_u0096_day_192) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\x82\xc2\x96"
                                            "D"));
}

TEST(delta_u00e1_u00a0_u0092_day_193) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\xa0\xc2\x92"
                                            "D"));
}

TEST(delta_u00e1_u00a5_u008e_day_194) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\xa5\xc2\x8e"
                                            "D"));
}

TEST(delta_u00e1_u00aa_u0084_day_195) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\xaa\xc2\x84"
                                            "D"));
}

TEST(delta_u00e1_u00ad_u0090_day_196) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\xad\xc2\x90"
                                            "D"));
}

TEST(delta_u00e1_u00ae_u00b6_day_197) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\xae\xc2\xb6"
                                            "D"));
}

TEST(delta_u00e1_u00b1_u0092_day_198) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\xb1\xc2\x92"
                                            "D"));
}

TEST(delta_u00e1_u00b5_u0088_199) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa1\xc2\xb5\xc2\x88"));
}

TEST(delta_u00e2_u0080_u008b_day_200) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa2\xc2\x80\xc2\x8b"
                                            "D"));
}

TEST(delta_u00e2_u0080_u00a8_day_201) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa2\xc2\x80\xc2\xa8"
                                            "D"));
}

TEST(delta_u00e2_u0080_u00a9_day_202) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa2\xc2\x80\xc2\xa9"
                                            "D"));
}

TEST(delta_u00e2_u0084_u008d_203) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa2\xc2\x84\xc2\x8d"));
}

TEST(delta_u00e2_u0092_u00bd_204) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa2\xc2\x92\xc2\xbd"));
}

TEST(delta_u00e2_u0093_u00a2_205) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa2\xc2\x93\xc2\xa2"));
}

TEST(delta_u00e3_u0080_u0080_day_206) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xa3\xc2\x80\xc2\x80"
                                            "D"));
}

TEST(delta_u00ea_u0098_u00a8_day_207) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xaa\xc2\x98\xc2\xa8"
                                            "D"));
}

TEST(delta_u00ea_u00a4_u0084_day_208) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xaa\xc2\xa4\xc2\x84"
                                            "D"));
}

TEST(delta_u00ea_u00a7_u00b0_day_209) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xaa\xc2\xa7\xc2\xb0"
                                            "D"));
}

TEST(delta_u00ea_u00a9_u0096_day_210) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xaa\xc2\xa9\xc2\x96"
                                            "D"));
}

TEST(delta_u00ef_u00bb_u00bf_day_211) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xaf\xc2\xbb\xc2\xbf"
                                            "D"));
}

TEST(delta_u00ef_u00bc_u0092_day_212) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xaf\xc2\xbc\xc2\x92"
                                            "D"));
}

TEST(delta_u00ef_u00bc_213) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xaf\xc2\xbc\xc2\xb9"));
}

TEST(delta_u00f0_u0090_u0092_u00a8_day_214) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x90\xc2\x92\xc2\xa8"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0081_u00aa_day_215) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\x81\xc2\xaa"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0084_u00b6_day_216) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\x84\xc2\xb6"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0087_u0096_day_217) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\x87\xc2\x96"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0091_u0092_day_218) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\x91\xc2\x92"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0093_u0098_day_219) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\x93\xc2\x98"
                                            "D"));
}

TEST(delta_u00f0_u0091_u009b_u0084_day_220) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\x9b\xc2\x84"
                                            "D"));
}

TEST(delta_u00f0_u0091_u00a3_u00a0_day_221) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\xa3\xc2\xa0"
                                            "D"));
}

TEST(delta_u00f0_u0091_u00a5_u0096_day_222) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\xa5\xc2\x96"
                                            "D"));
}

TEST(delta_u00f0_u0091_u00b5_u0092_day_223) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\xb5\xc2\x92"
                                            "D"));
}

TEST(delta_u00f0_u0091_u00b6_u00a8_day_224) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x91\xc2\xb6\xc2\xa8"
                                            "D"));
}

TEST(delta_u00f0_u0096_u00ad_u0094_day_225) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x96\xc2\xad\xc2\x94"
                                            "D"));
}

TEST(delta_u00f0_u009d_u0090_u008c_226) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x90\xc2\x8c"));
}

TEST(delta_u00f0_u009d_u0090_u00ad_227) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x90\xc2\xad"));
}

TEST(delta_u00f0_u009d_u0091_u0091_228) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x91\xc2\x91"));
}

TEST(delta_u00f0_u009d_u0091_u00ba_229) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x91\xc2\xba"));
}

TEST(delta_u00f0_u009d_u0092_u009a_230) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x92\xc2\x9a"));
}

TEST(delta_u00f0_u009d_u0093_u0088_231) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x93\xc2\x88"));
}

TEST(delta_u00f0_u009d_u0093_u00a8_232) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x93\xc2\xa8"));
}

TEST(delta_u00f0_u009d_u0094_u00b4_233) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x94\xc2\xb4"));
}

TEST(delta_u00f0_u009d_u0096_u0082_234) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x96\xc2\x82"));
}

TEST(delta_u00f0_u009d_u0096_u00a7_235) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x96\xc2\xa7"));
}

TEST(delta_u00f0_u009d_u0097_u008c_236) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x97\xc2\x8c"));
}

TEST(delta_u00f0_u009d_u0097_u00ac_237) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x97\xc2\xac"));
}

TEST(delta_u00f0_u009d_u0098_u0094_238) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x98\xc2\x94"));
}

TEST(delta_u00f0_u009d_u0098_u00b5_239) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x98\xc2\xb5"));
}

TEST(delta_u00f0_u009d_u0099_u0099_240) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x99\xc2\x99"));
}

TEST(delta_u00f0_u009d_u009a_u00a0_241) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "P1\xc3\xb0\xc2\x9d\xc2\x9a\xc2\xa0"));
}

TEST(delta_u00f0_u009d_u009f_u0098_day_242) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x9d\xc2\x9f\xc2\x98"
                                            "D"));
}

TEST(delta_u00f0_u009d_u009f_u00a8_day_243) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x9d\xc2\x9f\xc2\xa8"
                                            "D"));
}

TEST(delta_u00f0_u009d_u009f_u00b8_day_244) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x9d\xc2\x9f\xc2\xb8"
                                            "D"));
}

TEST(delta_u00f0_u009e_u0085_u0088_day_245) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x9e\xc2\x85\xc2\x88"
                                            "D"));
}

TEST(delta_u00f0_u009e_u00a5_u0094_day_246) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P1\xc3\xb0\xc2\x9e\xc2\xa5\xc2\x94"
                                            "D"));
}

TEST(delta_day_247) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P2147483647D"));
}

TEST(delta_week_248) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P2w"));
}

TEST(delta_day_249) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P4294967295D"));
}

TEST(delta_week_250) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P7W"));
}

TEST(delta_day_251) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P9007199254740991D"));
}

TEST(delta_day_252) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P9007199254740993D"));
}

TEST(delta_day_253) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P9223372036854775807D"));
}

TEST(delta_day_254) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P9223372036854775808D"));
}

TEST(delta_day_255) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration(
      "P99999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "9999999999999999999999D"));
}

TEST(delta_day_256) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration(
      "P99999999999999999999999999999999999999999999999999999999999999999999999"
      "99999999999999999999999999999D"));
}

TEST(delta_day_257) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration(
      "P99999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999D"));
}

TEST(delta_day_258) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration(
      "P99999999999999999999999999999999999999999999999999999999999999999999999"
      "99999999D"));
}

TEST(delta_day_259) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration(
      "P99999999999999999999999999999999999999999999999999999999999999999999999"
      "9999999D"));
}

TEST(delta_day_260) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration(
      "P99999999999999999999999999999999999999999999999999999999999999999999999"
      "999999D"));
}

TEST(delta_day_261) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration(
      "P999999999999999999999999999999999999999D"));
}

TEST(delta_day_262) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P99999999999999999999D"));
}

TEST(delta_year_month_day_t_hour_month_second_263) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration(
      "P99999999999999999999Y99999999999999999999M99999999999999999999DT9999999"
      "9999999999999H99999999999999999999M99999999999999999999S"));
}

TEST(delta_day_264) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P9999999999999999999D"));
}

TEST(delta_day_265) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P9999999999D"));
}

TEST(delta_day_266) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("P999D"));
}

TEST(delta_hour_267) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PH"));
}

TEST(delta_month_268) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PM"));
}

TEST(delta_second_269) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PS"));
}

TEST(delta_t_u0020_hour_270) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT 1H"));
}

TEST(delta_t_u0020_month_271) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT 1M"));
}

TEST(delta_t_u0020_second_272) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT 1S"));
}

TEST(delta_t_u002b_hour_273) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT+1H"));
}

TEST(delta_t_u002b_month_274) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT+1M"));
}

TEST(delta_t_u002b_second_275) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT+1S"));
}

TEST(delta_t_u002c_hour_276) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT,5H"));
}

TEST(delta_t_u002c_month_277) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT,5M"));
}

TEST(delta_t_u002c_second_278) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT,5S"));
}

TEST(delta_t_u002d_hour_279) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT-1H"));
}

TEST(delta_t_u002d_month_280) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT-1M"));
}

TEST(delta_t_u002e_hour_281) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT.5H"));
}

TEST(delta_t_u002e_month_282) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT.5M"));
}

TEST(delta_t_u0078_hour_283) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0x1H"));
}

TEST(delta_t_u0078_month_284) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0x1M"));
}

TEST(delta_t_u0078_second_285) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT0x1S"));
}

TEST(delta_t_u0020_hour_286) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1 H"));
}

TEST(delta_t_u0020_month_287) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1 M"));
}

TEST(delta_t_u0020_second_288) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1 S"));
}

TEST(delta_t_u002c_hour_289) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1,5H"));
}

TEST(delta_t_u002c_month_290) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1,5M"));
}

TEST(delta_t_u002c_hour_291) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1,H"));
}

TEST(delta_t_u002c_month_292) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1,M"));
}

TEST(delta_t_u002c_second_293) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1,S"));
}

TEST(delta_t_u002d_hour_294) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1-H"));
}

TEST(delta_t_u002d_month_295) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1-M"));
}

TEST(delta_t_u002d_second_296) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1-S"));
}

TEST(delta_t_u002e_u002e_hour_297) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1..5H"));
}

TEST(delta_t_u002e_u002e_month_298) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1..5M"));
}

TEST(delta_t_u002e_u002e_second_299) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1..5S"));
}

TEST(delta_t_u002e_second_300) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.5S"));
}

TEST(delta_t_u002e_hour_301) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.H"));
}

TEST(delta_t_u002e_month_302) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1.M"));
}

TEST(delta_t_u0041_303) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1A"));
}

TEST(delta_t_u0042_304) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1B"));
}

TEST(delta_t_u0043_305) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1C"));
}

TEST(delta_t_u0045_306) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1E"));
}

TEST(delta_t_u0045_hour_307) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1E2H"));
}

TEST(delta_t_u0045_month_308) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1E2M"));
}

TEST(delta_t_u0045_second_309) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1E2S"));
}

TEST(delta_t_u0046_310) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1F"));
}

TEST(delta_t_u0047_311) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1G"));
}

TEST(delta_t_hour_312) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H1"));
}

TEST(delta_t_hour_day_313) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H1D"));
}

TEST(delta_t_hour_hour_314) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H1H"));
}

TEST(delta_t_hour_month_315) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1H1M"));
}

TEST(delta_t_hour_month_second_316) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1H1M1S"));
}

TEST(delta_t_hour_week_317) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H1W"));
}

TEST(delta_t_hour_year_318) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1H1Y"));
}

TEST(delta_t_hour_t_319) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1HT"));
}

TEST(delta_t_hour_t_hour_320) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1HT1H"));
}

TEST(delta_t_hour_t_month_321) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1HT1M"));
}

TEST(delta_t_u0049_322) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1I"));
}

TEST(delta_t_u004a_323) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1J"));
}

TEST(delta_t_u004b_324) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1K"));
}

TEST(delta_t_u004c_325) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1L"));
}

TEST(delta_t_month_month_326) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M1M"));
}

TEST(delta_t_month_second_327) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_duration("PT1M1S"));
}

TEST(delta_t_month_week_328) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M1W"));
}

TEST(delta_t_month_year_329) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1M1Y"));
}

TEST(delta_t_month_t_hour_330) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1MT1H"));
}

TEST(delta_t_u004e_331) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1N"));
}

TEST(delta_t_u004f_332) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1O"));
}

TEST(delta_t_u0051_333) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1Q"));
}

TEST(delta_t_u0052_334) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1R"));
}

TEST(delta_t_second_day_335) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S1D"));
}

TEST(delta_t_second_second_336) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S1S"));
}

TEST(delta_t_second_week_337) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S1W"));
}

TEST(delta_t_second_year_338) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1S1Y"));
}

TEST(delta_t_second_t_hour_339) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1ST1H"));
}

TEST(delta_t_u0055_340) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1U"));
}

TEST(delta_t_u0056_341) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1V"));
}

TEST(delta_t_u005a_342) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1Z"));
}

TEST(delta_t_u005f_hour_343) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1_0H"));
}

TEST(delta_t_u005f_month_344) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1_0M"));
}

TEST(delta_t_u005f_second_345) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1_0S"));
}

TEST(delta_t_u0065_u002b_hour_346) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e+2H"));
}

TEST(delta_t_u0065_u002b_month_347) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e+2M"));
}

TEST(delta_t_u0065_u002b_second_348) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e+2S"));
}

TEST(delta_t_u0065_u002d_hour_349) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e-2H"));
}

TEST(delta_t_u0065_u002d_month_350) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e-2M"));
}

TEST(delta_t_u0065_u002d_second_351) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e-2S"));
}

TEST(delta_t_u0065_hour_352) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e2H"));
}

TEST(delta_t_u0065_month_353) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e2M"));
}

TEST(delta_t_u0065_second_354) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1e2S"));
}

TEST(delta_t_u0065_hour_355) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1eH"));
}

TEST(delta_t_u0065_month_356) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1eM"));
}

TEST(delta_t_u0065_second_357) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1eS"));
}

TEST(delta_t_hour_358) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1h"));
}

TEST(delta_t_second_359) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PT1s"));
}

TEST(delta_t_day_360) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTD"));
}

TEST(delta_t_hour_361) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTH"));
}

TEST(delta_t_month_362) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTM"));
}

TEST(delta_t_t_363) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTT"));
}

TEST(delta_t_week_364) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTW"));
}

TEST(delta_t_year_365) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTY"));
}

TEST(delta_t_u0065_hour_366) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTe1H"));
}

TEST(delta_t_u0065_month_367) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTe1M"));
}

TEST(delta_t_u0065_second_368) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("PTe1S"));
}

TEST(delta_u0065_day_369) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("Pe1D"));
}

TEST(delta_u0065_month_370) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("Pe1M"));
}

TEST(delta_u0065_week_371) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("Pe1W"));
}

TEST(delta_u0065_year_372) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("Pe1Y"));
}

TEST(delta_u00d9_u00a0_day_373) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\x99\xc2\xa0"
                                                     "D"));
}

TEST(delta_u00db_u00b6_day_374) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\x9b\xc2\xb6"
                                                     "D"));
}

TEST(delta_u00e0_u00a5_u00a8_day_375) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa0\xc2\xa5\xc2\xa8"
                                                     "D"));
}

TEST(delta_u00e0_u00a7_u00ae_day_376) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa0\xc2\xa7\xc2\xae"
                                                     "D"));
}

TEST(delta_u00e0_u00ab_u00aa_day_377) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa0\xc2\xab\xc2\xaa"
                                                     "D"));
}

TEST(delta_u00e0_u00af_u00a6_day_378) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa0\xc2\xaf\xc2\xa6"
                                                     "D"));
}

TEST(delta_u00e0_u00b1_u00ac_day_379) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa0\xc2\xb1\xc2\xac"
                                                     "D"));
}

TEST(delta_u00e0_u00b5_u00a8_day_380) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa0\xc2\xb5\xc2\xa8"
                                                     "D"));
}

TEST(delta_u00e0_u00b7_u00ae_day_381) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa0\xc2\xb7\xc2\xae"
                                                     "D"));
}

TEST(delta_u00e0_u00bb_u0094_day_382) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa0\xc2\xbb\xc2\x94"
                                                     "D"));
}

TEST(delta_u00e1_u0081_u0080_day_383) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa1\xc2\x81\xc2\x80"
                                                     "D"));
}

TEST(delta_u00e1_u0082_u0096_day_384) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa1\xc2\x82\xc2\x96"
                                                     "D"));
}

TEST(delta_u00e1_u00a0_u0092_day_385) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa1\xc2\xa0\xc2\x92"
                                                     "D"));
}

TEST(delta_u00e1_u00a5_u008e_day_386) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa1\xc2\xa5\xc2\x8e"
                                                     "D"));
}

TEST(delta_u00e1_u00aa_u0084_day_387) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa1\xc2\xaa\xc2\x84"
                                                     "D"));
}

TEST(delta_u00e1_u00ad_u0090_day_388) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa1\xc2\xad\xc2\x90"
                                                     "D"));
}

TEST(delta_u00e1_u00ae_u00b6_day_389) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa1\xc2\xae\xc2\xb6"
                                                     "D"));
}

TEST(delta_u00e1_u00b1_u0092_day_390) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xa1\xc2\xb1\xc2\x92"
                                                     "D"));
}

TEST(delta_u00ea_u0098_u00a8_day_391) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xaa\xc2\x98\xc2\xa8"
                                                     "D"));
}

TEST(delta_u00ea_u00a4_u0084_day_392) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xaa\xc2\xa4\xc2\x84"
                                                     "D"));
}

TEST(delta_u00ea_u00a7_u00b0_day_393) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xaa\xc2\xa7\xc2\xb0"
                                                     "D"));
}

TEST(delta_u00ea_u00a9_u0096_day_394) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xaa\xc2\xa9\xc2\x96"
                                                     "D"));
}

TEST(delta_u00ef_u00bc_u0092_day_395) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("P\xc3\xaf\xc2\xbc\xc2\x92"
                                                     "D"));
}

TEST(delta_u00f0_u0090_u0092_u00a8_day_396) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x90\xc2\x92\xc2\xa8"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0081_u00aa_day_397) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\x81\xc2\xaa"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0084_u00b6_day_398) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\x84\xc2\xb6"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0087_u0096_day_399) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\x87\xc2\x96"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0091_u0092_day_400) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\x91\xc2\x92"
                                            "D"));
}

TEST(delta_u00f0_u0091_u0093_u0098_day_401) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\x93\xc2\x98"
                                            "D"));
}

TEST(delta_u00f0_u0091_u009b_u0084_day_402) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\x9b\xc2\x84"
                                            "D"));
}

TEST(delta_u00f0_u0091_u00a3_u00a0_day_403) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\xa3\xc2\xa0"
                                            "D"));
}

TEST(delta_u00f0_u0091_u00a5_u0096_day_404) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\xa5\xc2\x96"
                                            "D"));
}

TEST(delta_u00f0_u0091_u00b5_u0092_day_405) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\xb5\xc2\x92"
                                            "D"));
}

TEST(delta_u00f0_u0091_u00b6_u00a8_day_406) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x91\xc2\xb6\xc2\xa8"
                                            "D"));
}

TEST(delta_u00f0_u0096_u00ad_u0094_day_407) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x96\xc2\xad\xc2\x94"
                                            "D"));
}

TEST(delta_u00f0_u009d_u009f_u0098_day_408) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x9d\xc2\x9f\xc2\x98"
                                            "D"));
}

TEST(delta_u00f0_u009d_u009f_u00a8_day_409) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x9d\xc2\x9f\xc2\xa8"
                                            "D"));
}

TEST(delta_u00f0_u009d_u009f_u00b8_day_410) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x9d\xc2\x9f\xc2\xb8"
                                            "D"));
}

TEST(delta_u00f0_u009e_u0085_u0088_day_411) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x9e\xc2\x85\xc2\x88"
                                            "D"));
}

TEST(delta_u00f0_u009e_u00a5_u0094_day_412) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("P\xc3\xb0\xc2\x9e\xc2\xa5\xc2\x94"
                                            "D"));
}

TEST(delta_u0052_u002f_u0050_day_413) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("R5/P1D"));
}

TEST(delta_u0070_414) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("p"));
}

TEST(delta_u0070_day_415) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("p1D"));
}

TEST(delta_u0070_day_416) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("p1d"));
}

TEST(delta_u0070_year_month_day_t_hour_month_second_417) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("p1y2m3dt4h5m6s"));
}

TEST(delta_u007f_u0050_day_418) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x7fP1D"));
}

TEST(delta_u007f_u0050_day_u007f_419) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\x7fP1D\x7f"));
}

TEST(delta_u00c2_u00a0_u0050_day_420) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration("\xc3\x82\xc2\xa0P1D"));
}

TEST(delta_u00c2_u00a0_u0050_day_u00c2_u00a0_421) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "\xc3\x82\xc2\xa0P1D\xc3\x82\xc2\xa0"));
}

TEST(delta_u00e2_u0080_u008b_u0050_day_422) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("\xc3\xa2\xc2\x80\xc2\x8bP1D"));
}

TEST(delta_u00e2_u0080_u008b_u0050_day_u00e2_u0080_u008b_423) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "\xc3\xa2\xc2\x80\xc2\x8bP1D\xc3\xa2\xc2\x80\xc2\x8b"));
}

TEST(delta_u00e2_u0080_u00a8_u0050_day_424) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("\xc3\xa2\xc2\x80\xc2\xa8P1D"));
}

TEST(delta_u00e2_u0080_u00a8_u0050_day_u00e2_u0080_u00a8_425) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "\xc3\xa2\xc2\x80\xc2\xa8P1D\xc3\xa2\xc2\x80\xc2\xa8"));
}

TEST(delta_u00e2_u0080_u00a9_u0050_day_426) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("\xc3\xa2\xc2\x80\xc2\xa9P1D"));
}

TEST(delta_u00e2_u0080_u00a9_u0050_day_u00e2_u0080_u00a9_427) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "\xc3\xa2\xc2\x80\xc2\xa9P1D\xc3\xa2\xc2\x80\xc2\xa9"));
}

TEST(delta_u00e3_u0080_u0080_u0050_day_428) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("\xc3\xa3\xc2\x80\xc2\x80P1D"));
}

TEST(delta_u00e3_u0080_u0080_u0050_day_u00e3_u0080_u0080_429) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "\xc3\xa3\xc2\x80\xc2\x80P1D\xc3\xa3\xc2\x80\xc2\x80"));
}

TEST(delta_u00ef_u00bb_u00bf_u0050_day_u00ef_u00bb_u00bf_430) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_duration(
      "\xc3\xaf\xc2\xbb\xc2\xbfP1D\xc3\xaf\xc2\xbb\xc2\xbf"));
}

TEST(delta_u00f0_u009d_u0094_u0093_day_431) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("\xc3\xb0\xc2\x9d\xc2\x94\xc2\x93"
                                            "1D"));
}

TEST(delta_u00f0_u009d_u0095_u00a1_day_432) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("\xc3\xb0\xc2\x9d\xc2\x95\xc2\xa1"
                                            "1D"));
}

TEST(delta_u00f0_u009d_u0099_u00bf_day_433) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_duration("\xc3\xb0\xc2\x9d\xc2\x99\xc2\xbf"
                                            "1D"));
}
