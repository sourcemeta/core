sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME parallel
  SOURCES parallel_for_each_test.cc)

target_link_libraries(sourcemeta_core_parallel_unit PRIVATE sourcemeta::core::parallel)
