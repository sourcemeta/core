sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME unicode
  SOURCES unicode_test.cc)

target_link_libraries(sourcemeta_core_unicode_unit
  PRIVATE sourcemeta::core::unicode)
