sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME io
  SOURCES io_canonical_test.cc io_weakly_canonical_test.cc io_read_file_test.cc)

target_link_libraries(sourcemeta_core_io_unit
  PRIVATE sourcemeta::core::io)
target_compile_definitions(sourcemeta_core_io_unit
  PRIVATE TEST_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")
