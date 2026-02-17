sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME numeric
  SOURCES numeric_decimal_test.cc numeric_parse_test.cc)

target_link_libraries(sourcemeta_core_numeric_unit
  PRIVATE sourcemeta::core::numeric)

# IBM General Decimal Arithmetic Test Suite
# See https://speleotrove.com/decimal/
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME ibm_dectest
  SOURCES decimal_ibm_dectest.cc)
target_compile_definitions(sourcemeta_core_ibm_dectest_unit
  PRIVATE DECTEST_PATH="${CMAKE_CURRENT_SOURCE_DIR}/dectest")
target_link_libraries(sourcemeta_core_ibm_dectest_unit
  PRIVATE sourcemeta::core::numeric)
