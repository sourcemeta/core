sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME metapack
  SOURCES
    metapack_corrupt_test.cc
    metapack_parse_test.cc
    metapack_read_test.cc
    metapack_write_test.cc)

target_link_libraries(sourcemeta_core_metapack_unit
  PRIVATE sourcemeta::core::metapack)
