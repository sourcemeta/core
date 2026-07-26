#include <sourcemeta/core/test.h>
#include <sourcemeta/core/time.h>

TEST(valid_basic_zulu) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00Z"));
}

TEST(valid_min_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("00:00:00Z"));
}

TEST(valid_max_normal_time) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:59Z"));
}

TEST(valid_lowercase_z) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00z"));
}

TEST(valid_positive_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+05:30"));
}

TEST(valid_negative_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00-08:00"));
}

TEST(valid_zero_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+00:00"));
}

TEST(valid_unknown_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00-00:00"));
}

TEST(valid_short_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00.5Z"));
}

TEST(valid_long_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("14:30:00.123456789Z"));
}

TEST(valid_secfrac_with_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.283185+01:00"));
}

TEST(valid_leap_second_zulu) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:60Z"));
}

TEST(valid_leap_second_negative_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("15:59:60-08:00"));
}

TEST(valid_leap_second_positive_offset_wraparound) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("01:29:60+01:30"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("")); }

TEST(invalid_no_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00"));
}

TEST(invalid_no_offset_with_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00.5"));
}

TEST(invalid_empty_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00.Z"));
}

TEST(invalid_one_digit_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("4:30:00Z"));
}

TEST(invalid_one_digit_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:3:00Z"));
}

TEST(invalid_one_digit_second) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:0Z"));
}

TEST(invalid_dot_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14.30.00Z"));
}

TEST(invalid_colonless_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+0530"));
}

TEST(invalid_trailing_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00Z "));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(" 14:30:00Z"));
}

TEST(invalid_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("24:00:00Z"));
}

TEST(invalid_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("00:60:00Z"));
}

TEST(invalid_second_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("00:00:61Z"));
}

TEST(invalid_offset_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+24:00"));
}

TEST(invalid_offset_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("14:30:00+00:60"));
}

TEST(invalid_leap_second_wrong_hour_zulu) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("22:59:60Z"));
}

TEST(invalid_leap_second_wrong_minute_zulu) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:58:60Z"));
}

TEST(invalid_leap_second_wrong_offset_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60+01:00"));
}

TEST(invalid_leap_second_wrong_offset_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60+00:30"));
}

TEST(suite_valid_basic) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z"));
}

TEST(suite_invalid_extra_leading_zeros) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("008:030:006Z"));
}

TEST(suite_invalid_no_leading_zero) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("8:3:6Z"));
}

TEST(suite_invalid_mixed_field_widths) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("8:0030:6Z"));
}

TEST(suite_valid_leap_second_zero_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:60+00:00"));
}

TEST(suite_invalid_leap_wrong_hour_zero_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("22:59:60+00:00"));
}

TEST(suite_invalid_leap_wrong_minute_zero_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:58:60+00:00"));
}

TEST(suite_valid_leap_large_positive_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:29:60+23:30"));
}

TEST(suite_valid_leap_large_negative_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("00:29:60-23:30"));
}

TEST(suite_invalid_leap_wrong_hour_negative_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60-01:00"));
}

TEST(suite_invalid_leap_wrong_minute_negative_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60-00:30"));
}

TEST(suite_valid_short_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:20:50.52Z"));
}

TEST(suite_valid_precise_secfrac) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.283185Z"));
}

TEST(suite_valid_plus_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00:20"));
}

TEST(suite_valid_unknown_local_offset) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("12:34:56-00:00"));
}

TEST(suite_invalid_offset_field_widths) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06-8:000"));
}

TEST(suite_invalid_both_z_and_numoffset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("01:02:03Z+00:30"));
}

TEST(suite_invalid_offset_indicator_pst) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06 PST"));
}

TEST(suite_invalid_iso8601_comma_secfrac) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("01:01:01,1111"));
}

TEST(suite_invalid_bengali_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("1\xe0\xa7\xa8"
                                                     ":00:00Z"));
}

TEST(suite_invalid_hash_offset_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06#00:20"));
}

TEST(suite_invalid_letters) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("ab:cd:ef"));
}

TEST(suite_invalid_datetime_passed_in) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("2020-11-28T23:55:45Z"));
}

TEST(length_01_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0"));
}

TEST(length_02_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08"));
}

TEST(length_03_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:"));
}

TEST(length_04_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:3"));
}

TEST(length_05_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30"));
}

TEST(length_06_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:"));
}

TEST(length_07_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:0"));
}

TEST(length_08_too_short) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06"));
}

TEST(hour_value_00) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("00:30:06Z"));
}

TEST(hour_value_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("01:30:06Z"));
}

TEST(hour_value_09) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("09:30:06Z"));
}

TEST(hour_value_10) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("10:30:06Z"));
}

TEST(hour_value_19) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("19:30:06Z"));
}

TEST(hour_value_20) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("20:30:06Z"));
}

TEST(hour_value_23) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:30:06Z"));
}

TEST(hour_value_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("24:30:06Z"));
}

TEST(hour_value_25) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("25:30:06Z"));
}

TEST(hour_value_29) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("29:30:06Z"));
}

TEST(hour_value_30) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("30:30:06Z"));
}

TEST(hour_value_59) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("59:30:06Z"));
}

TEST(hour_value_90) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("90:30:06Z"));
}

TEST(hour_value_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("99:30:06Z"));
}

TEST(hour_first_digit_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("a8:30:06Z"));
}

TEST(hour_second_digit_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0a:30:06Z"));
}

TEST(hour_first_digit_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("-8:30:06Z"));
}

TEST(hour_second_digit_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0-:30:06Z"));
}

TEST(hour_first_digit_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(" 8:30:06Z"));
}

TEST(hour_second_digit_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0 :30:06Z"));
}

TEST(hour_first_digit_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("+8:30:06Z"));
}

TEST(hour_second_digit_plus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0+:30:06Z"));
}

TEST(hour_first_digit_nul) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_fulltime(std::string_view("\x00"
                                                             "8:30:06Z",
                                                             9)));
}

TEST(hour_second_digit_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(
      std::string_view("0\x00:30:06Z", 9)));
}

TEST(hour_first_digit_colon) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(":8:30:06Z"));
}

TEST(hour_second_digit_colon) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0::30:06Z"));
}

TEST(hour_second_digit_semicolon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0;:30:06Z"));
}

TEST(hour_second_digit_less_than_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0<:30:06Z"));
}

TEST(first_separator_dot) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08.30:06Z"));
}

TEST(second_separator_dot) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30.06Z"));
}

TEST(first_separator_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08-30:06Z"));
}

TEST(second_separator_hyphen) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30-06Z"));
}

TEST(first_separator_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08 30:06Z"));
}

TEST(second_separator_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30 06Z"));
}

TEST(first_separator_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08_30:06Z"));
}

TEST(second_separator_underscore) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30_06Z"));
}

TEST(first_separator_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08/30:06Z"));
}

TEST(second_separator_slash) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30/06Z"));
}

TEST(first_separator_comma) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08,30:06Z"));
}

TEST(second_separator_comma) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30,06Z"));
}

TEST(first_separator_missing) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("0830:06Z"));
}

TEST(second_separator_missing) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:3006Z"));
}

TEST(first_separator_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(std::string_view("08\x00"
                                                                      "30:06Z",
                                                                      9)));
}

TEST(second_separator_nul) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_fulltime(std::string_view("08:30\x00"
                                                             "06Z",
                                                             9)));
}

TEST(first_separator_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08030:06Z"));
}

TEST(second_separator_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30006Z"));
}

TEST(first_separator_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08\x09"
                                                     "30:06Z"));
}

TEST(second_separator_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30\x09"
                                                     "06Z"));
}

TEST(minute_value_00) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:00:06Z"));
}

TEST(minute_value_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:01:06Z"));
}

TEST(minute_value_59) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:59:06Z"));
}

TEST(minute_value_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:60:06Z"));
}

TEST(minute_value_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:61:06Z"));
}

TEST(minute_value_90) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:90:06Z"));
}

TEST(minute_value_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:99:06Z"));
}

TEST(minute_first_digit_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:a0:06Z"));
}

TEST(minute_second_digit_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:3a:06Z"));
}

TEST(minute_first_digit_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08: 0:06Z"));
}

TEST(minute_second_digit_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:3 :06Z"));
}

TEST(minute_first_digit_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(std::string_view("08:\x00"
                                                                      "0:06Z",
                                                                      9)));
}

TEST(minute_second_digit_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(
      std::string_view("08:3\x00:06Z", 9)));
}

TEST(minute_second_digit_colon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:3::06Z"));
}

TEST(minute_second_digit_semicolon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:3;:06Z"));
}

TEST(second_value_00) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:00Z"));
}

TEST(second_value_01) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:01Z"));
}

TEST(second_value_58) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:58Z"));
}

TEST(second_value_61) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:61Z"));
}

TEST(second_value_90) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:90Z"));
}

TEST(second_value_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:99Z"));
}

TEST(second_first_digit_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:a6Z"));
}

TEST(second_second_digit_alpha) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:0aZ"));
}

TEST(second_first_digit_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30: 6Z"));
}

TEST(second_second_digit_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:0 Z"));
}

TEST(second_first_digit_nul) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_fulltime(std::string_view("08:30:\x00"
                                                             "6Z",
                                                             9)));
}

TEST(second_second_digit_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(
      std::string_view("08:30:0\x00Z", 9)));
}

TEST(second_second_digit_colon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:0:Z"));
}

TEST(second_second_digit_semicolon_lands_in_range) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:0;Z"));
}

TEST(secfrac_single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.1Z"));
}

TEST(secfrac_empty) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.Z"));
}

TEST(secfrac_empty_no_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06."));
}

TEST(secfrac_all_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.000Z"));
}

TEST(secfrac_nine_digits) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.123456789Z"));
}

TEST(secfrac_thirty_digits) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime(
      "08:30:06.111111111111111111111111111111Z"));
}

TEST(secfrac_three_hundred_digits) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime(
      "08:30:06."
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999Z"));
}

TEST(secfrac_double_dot) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06..1Z"));
}

TEST(secfrac_two_fractions) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.1.2Z"));
}

TEST(secfrac_comma_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06,1Z"));
}

TEST(secfrac_non_ascii_digit) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.\xd9\xa1Z"));
}

TEST(secfrac_sign_in_fraction) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.+1Z"));
}

TEST(secfrac_then_nothing) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06.1"));
}

TEST(offset_lower_z) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06z"));
}

TEST(offset_char_letter_y) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Y"));
}

TEST(offset_char_letter_u) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06U"));
}

TEST(offset_char_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06 "));
}

TEST(offset_char_asterisk) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06*"));
}

TEST(offset_char_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(
      std::string_view("08:30:06\x00", 9)));
}

TEST(offset_char_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06\x0a"));
}

TEST(offset_z_then_trailing) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Zx"));
}

TEST(offset_z_then_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z\x0a"));
}

TEST(offset_z_then_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z "));
}

TEST(offset_double_z) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06ZZ"));
}

TEST(numoffset_plus_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00:00"));
}

TEST(numoffset_minus_zero) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06-00:00"));
}

TEST(numoffset_max_hour) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+23:59"));
}

TEST(numoffset_hour_24) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+24:00"));
}

TEST(numoffset_hour_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+99:00"));
}

TEST(numoffset_minute_60) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00:60"));
}

TEST(numoffset_minute_99) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00:99"));
}

TEST(numoffset_no_colon) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+0000"));
}

TEST(numoffset_one_digit_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+0:00"));
}

TEST(numoffset_one_digit_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00:0"));
}

TEST(numoffset_hour_only) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00"));
}

TEST(numoffset_truncated) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+0"));
}

TEST(numoffset_double_sign) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06++00:00"));
}

TEST(numoffset_sign_then_letter) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+ab:00"));
}

TEST(numoffset_dot_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00.00"));
}

TEST(numoffset_trailing_content) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00:00x"));
}

TEST(numoffset_then_z) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+00:00Z"));
}

TEST(numoffset_unicode_minus) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06\xe2\x88\x92"
                                                     "00:00"));
}

TEST(numoffset_beyond_18_hours) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("08:30:06+19:00"));
}

TEST(leading_newline) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\x0a"
                                                     "08:30:06Z"));
}

TEST(trailing_cr) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z\x0d"));
}

TEST(leading_cr) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\x0d"
                                                     "08:30:06Z"));
}

TEST(trailing_crlf) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z\x0d\x0a"));
}

TEST(leading_crlf) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\x0d\x0a"
                                                     "08:30:06Z"));
}

TEST(trailing_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z\x09"));
}

TEST(leading_tab) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\x09"
                                                     "08:30:06Z"));
}

TEST(leading_space) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(" 08:30:06Z"));
}

TEST(trailing_nul) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(
      std::string_view("08:30:06Z\x00", 10)));
}

TEST(leading_nul) {
  EXPECT_FALSE(
      sourcemeta::core::is_rfc3339_fulltime(std::string_view("\x00"
                                                             "08:30:06Z",
                                                             10)));
}

TEST(trailing_nbsp) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z\xc2\xa0"));
}

TEST(leading_nbsp) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\xc2\xa0"
                                                     "08:30:06Z"));
}

TEST(trailing_line_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z\xe2\x80\xa8"));
}

TEST(leading_line_separator) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\xe2\x80\xa8"
                                                     "08:30:06Z"));
}

TEST(trailing_bom) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:06Z\xef\xbb\xbf"));
}

TEST(leading_bom) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\xef\xbb\xbf"
                                                     "08:30:06Z"));
}

TEST(leap_utc_2359_lower_z) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:60z"));
}

TEST(leap_not_2359_z) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:30:60Z"));
}

TEST(leap_0059_plus_0100) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("00:59:60+01:00"));
}

TEST(leap_2259_minus_0100) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("22:59:60-01:00"));
}

TEST(leap_offset_wraps_backward) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("00:29:60+00:30"));
}

TEST(leap_offset_wraps_forward) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:29:60-00:30"));
}

TEST(leap_wrong_minute_with_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("00:58:60+01:00"));
}

TEST(leap_with_fraction) {
  EXPECT_TRUE(sourcemeta::core::is_rfc3339_fulltime("23:59:60.5Z"));
}

TEST(leap_max_offset) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("23:59:60+23:59"));
}

TEST(non_ascii_digit_in_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\xd9\xa0"
                                                     "8:30:06Z"));
}

TEST(non_ascii_digit_in_minute) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("08:\xd9\xa3"
                                                     "0:06Z"));
}

TEST(fullwidth_all) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime(
      "\xef\xbc\x90\xef\xbc\x98:\xef\xbc\x93\xef\xbc\x90:"
      "\xef\xbc\x90\xef\xbc\x96Z"));
}

TEST(astral_digit_in_hour) {
  EXPECT_FALSE(sourcemeta::core::is_rfc3339_fulltime("\xf0\x9d\x9f\x98"
                                                     "0:30:06Z"));
}
