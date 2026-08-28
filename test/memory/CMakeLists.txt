sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME memory
  SOURCES memory_owned_or_reference_test.cc)

target_link_libraries(sourcemeta_core_memory_unit PRIVATE sourcemeta::core::memory)
