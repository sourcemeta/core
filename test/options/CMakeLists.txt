sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME options
  SOURCES options_test.cc)

target_link_libraries(sourcemeta_core_options_unit
  PRIVATE sourcemeta::core::options)
