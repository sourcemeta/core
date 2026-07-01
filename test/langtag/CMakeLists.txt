sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME langtag
  SOURCES langtag_test.cc)

target_link_libraries(sourcemeta_core_langtag_unit
  PRIVATE sourcemeta::core::langtag)
