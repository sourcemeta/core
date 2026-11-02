sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME diff
  SOURCES diff_line_myers_test.cc)

target_link_libraries(sourcemeta_core_diff_unit
  PRIVATE sourcemeta::core::diff)
