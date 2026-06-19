sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME io
  SOURCES
    io_atomic_directory_swap_test.cc
    io_atomic_write_file_test.cc
    io_binary_test.cc
    io_bytestream_test.cc
    io_canonical_test.cc
    io_fileview_test.cc
    io_flush_test.cc
    io_for_each_line_test.cc
    io_hardlink_directory_test.cc
    io_read_file_test.cc
    io_read_to_string_test.cc
    io_is_under_path_test.cc
    io_is_lexically_under_path_test.cc
    io_strip_path_prefix_test.cc
    io_temporary_test.cc
    io_weakly_canonical_test.cc
    io_write_file_test.cc)

target_link_libraries(sourcemeta_core_io_unit
  PRIVATE sourcemeta::core::io)
target_compile_definitions(sourcemeta_core_io_unit
  PRIVATE STUBS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/stubs"
  PRIVATE BUILD_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
