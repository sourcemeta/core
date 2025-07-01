sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME md5
  SOURCES md5_test.cc)

target_link_libraries(sourcemeta_core_md5_unit
  PRIVATE sourcemeta::core::md5)
