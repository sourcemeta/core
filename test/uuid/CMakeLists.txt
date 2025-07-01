sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME uuid
  SOURCES uuid_v4_test.cc)

target_link_libraries(sourcemeta_core_uuid_unit
  PRIVATE sourcemeta::core::uuid)
