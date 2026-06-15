sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jose
  SOURCES jose_algorithm_test.cc jose_jwk_test.cc)

target_link_libraries(sourcemeta_core_jose_unit
  PRIVATE sourcemeta::core::jose)
