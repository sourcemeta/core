sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME error
  SOURCES error_base_test.cc error_file_test.cc error_static_string_test.cc)

target_link_libraries(sourcemeta_core_error_unit
  PRIVATE sourcemeta::core::error)
