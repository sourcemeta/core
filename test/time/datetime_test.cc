#include <gtest/gtest.h>

#include <sourcemeta/core/time.h>

// VALID - RFC 3339 §5.6 compliant inputs

// RFC 3339 §5.8 example: fractional seconds with UTC
TEST(Time_datetime, valid_rfc_example_1) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("1985-04-12T23:20:50.52Z"));
}

// RFC 3339 §5.8 example: negative numeric offset
TEST(Time_datetime, valid_rfc_example_negative) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("1996-12-19T16:39:57-08:00"));
}

// RFC 3339 §5.7: leap second at end of December
TEST(Time_datetime, valid_rfc_leap_second_1) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("1990-12-31T23:59:60Z"));
}

// RFC 3339 §5.7: leap second with negative offset
TEST(Time_datetime, valid_rfc_leap_second_2) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("1990-12-31T15:59:60-08:00"));
}

// RFC 3339 §5.8 example: historical offset +00:20
TEST(Time_datetime, valid_rfc_historical) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("1937-01-01T12:00:27.87+00:20"));
}

// Basic valid date-time with UTC
TEST(Time_datetime, valid_basic_utc) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("2024-01-15T14:30:00Z"));
}

// §5.6 time-secfrac = "." 1*DIGIT — long fractional seconds
TEST(Time_datetime, valid_long_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("2024-01-15T14:30:00.123456Z"));
}

// §5.7: historical leap second (1998-12-31)
TEST(Time_datetime, valid_historical_leap_sec) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("1998-12-31T23:59:60Z"));
}

// §1: year range includes 0000
TEST(Time_datetime, valid_year_zero) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("0000-01-01T00:00:00Z"));
}

// Appendix C: year 0000 is a leap year (0%400==0)
TEST(Time_datetime, valid_year_zero_leap_day) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("0000-02-29T00:00:00Z"));
}

// §1: maximum year 9999
TEST(Time_datetime, valid_max_year) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("9999-12-31T23:59:59Z"));
}

// §5.6 NOTE: T and Z may be lowercase
TEST(Time_datetime, valid_lowercase_t_z) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("2024-01-15t14:30:00z"));
}

// §5.6 time-numoffset: UTC expressed as +00:00
TEST(Time_datetime, valid_utc_offset_zero) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("2024-01-15T00:00:00+00:00"));
}

// §4.3: unknown local offset convention -00:00
TEST(Time_datetime, valid_unknown_offset) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("2024-01-15T00:00:00-00:00"));
}

// Appendix C: year 2000 is a leap year (2000%400==0)
TEST(Time_datetime, valid_year_2000_leap) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("2000-02-29T12:00:00Z"));
}

// Appendix C: year 2004 is a normal leap year
TEST(Time_datetime, valid_normal_leap_year) {
  EXPECT_TRUE(sourcemeta::core::is_datetime("2004-02-29T12:00:00Z"));
}

// §5.6: fractional seconds with positive numeric offset
TEST(Time_datetime, valid_secfrac_with_offset) {
  EXPECT_TRUE(
      sourcemeta::core::is_datetime("1963-06-19T08:30:06.283185+01:00"));
}

// INVALID - RFC 3339 §5.6 violations

// §5.6: full-time requires time-offset
TEST(Time_datetime, invalid_no_timezone) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T14:30:00"));
}

// §5.6: time-numoffset requires colon — +0530 is colonless
TEST(Time_datetime, invalid_colonless_offset) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T14:30:00+0530"));
}

// §5.7/Appendix C: 2023 is not a leap year
TEST(Time_datetime, invalid_non_leap_feb29) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2023-02-29T14:30:00Z"));
}

// Appendix C: 2100%100==0 but 2100%400!=0 — not a leap year
TEST(Time_datetime, invalid_century_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2100-02-29T14:30:00Z"));
}

// Appendix C: 1900%100==0 but 1900%400!=0 — not a leap year
TEST(Time_datetime, invalid_1900_non_leap) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("1900-02-29T14:30:00Z"));
}

// §5.6: date-time = full-date "T" full-time — space is not T
TEST(Time_datetime, invalid_space_separator) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15 14:30:00Z"));
}

// §5.6: date-time = full-date "T" full-time — tab is not T
TEST(Time_datetime, invalid_tab_separator) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15\t14:30:00Z"));
}

// §5.6: date-fullyear = 4DIGIT — five digits invalid
TEST(Time_datetime, invalid_five_digit_year) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("10000-01-01T00:00:00Z"));
}

// §5.6: date-month = 2DIGIT ; 01-12
TEST(Time_datetime, invalid_month_13) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-13-01T14:30:00Z"));
}

// §5.6: date-month = 2DIGIT ; 01-12
TEST(Time_datetime, invalid_month_00) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-00-01T14:30:00Z"));
}

// §5.7: date-mday starts at 01
TEST(Time_datetime, invalid_day_00) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-00T14:30:00Z"));
}

// §5.7: day 32 always invalid
TEST(Time_datetime, invalid_day_32) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-32T14:30:00Z"));
}

// §5.7: hour only allows 00-23 (not 24)
TEST(Time_datetime, invalid_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T24:00:00Z"));
}

// §5.6: time-minute = 2DIGIT ; 00-59
TEST(Time_datetime, invalid_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T14:60:00Z"));
}

// §5.6: time-second = 2DIGIT ; max 60 for leap second
TEST(Time_datetime, invalid_second_61) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T14:30:61Z"));
}

// Must consume entire string — no trailing characters
TEST(Time_datetime, invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T14:30:00Z "));
}

// Must start with date — no leading characters
TEST(Time_datetime, invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_datetime(" 2024-01-15T14:30:00Z"));
}

// Empty string is invalid
TEST(Time_datetime, invalid_empty) {
  EXPECT_FALSE(sourcemeta::core::is_datetime(""));
}

// Completely non-date string
TEST(Time_datetime, invalid_not_a_date) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("not-a-date"));
}

// §5.6: offset time-hour = 00-23
TEST(Time_datetime, invalid_offset_hour_25) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T14:30:00+25:00"));
}

// §5.6: offset time-minute = 00-59
TEST(Time_datetime, invalid_offset_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T14:30:00+00:60"));
}

// §5.6: time-secfrac = "." 1*DIGIT — empty fractional part
TEST(Time_datetime, invalid_empty_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2024-01-15T14:30:00.Z"));
}

// Appendix C: 2300%100==0 but 2300%400!=0 — not a leap year
TEST(Time_datetime, invalid_feb29_year_2300) {
  EXPECT_FALSE(sourcemeta::core::is_datetime("2300-02-29T00:00:00Z"));
}
