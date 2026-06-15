sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME time
  SOURCES imf_fixdate_test.cc rfc850_date_test.cc asctime_test.cc
          unix_timestamp_test.cc
          rfc3339_datetime_test.cc rfc3339_fulldate_test.cc
          rfc3339_fulltime_test.cc rfc3339_partialtime_no_secfrac_test.cc
          rfc3339_duration_test.cc is_leap_year_test.cc max_day_in_month_test.cc)

target_link_libraries(sourcemeta_core_time_unit
  PRIVATE sourcemeta::core::time)
