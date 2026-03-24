sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME error
  SOURCES error_file_test.cc)

target_link_libraries(sourcemeta_core_error_unit
  PRIVATE sourcemeta::core::error)
