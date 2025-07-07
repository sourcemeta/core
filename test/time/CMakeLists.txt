sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME time
  SOURCES gmt_test.cc)

target_link_libraries(sourcemeta_core_time_unit
  PRIVATE sourcemeta::core::time)
