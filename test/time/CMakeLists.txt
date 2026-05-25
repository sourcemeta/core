sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME time
  SOURCES gmt_test.cc rfc3339_datetime_test.cc rfc3339_fulldate_test.cc
          is_leap_year_test.cc max_day_in_month_test.cc)

target_link_libraries(sourcemeta_core_time_unit
  PRIVATE sourcemeta::core::time)
