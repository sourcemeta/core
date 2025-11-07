sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME numeric
  SOURCES numeric_decimal_test.cc)

target_link_libraries(sourcemeta_core_numeric_unit
  PRIVATE sourcemeta::core::numeric)
