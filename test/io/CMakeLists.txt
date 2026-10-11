sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME io
  SOURCES
    io_binary_test.cc
    io_bytestream_test.cc
    io_canonical_test.cc
    io_error_test.cc
    io_for_each_line_test.cc
    io_hardlink_directory_test.cc
    io_read_file_test.cc
    io_read_file_to_string_test.cc
    io_read_to_string_test.cc
    io_resume_stream_test.cc
    io_is_under_path_test.cc
    io_is_lexically_under_path_test.cc
    io_strip_path_prefix_test.cc
    io_weakly_canonical_test.cc
    io_write_file_test.cc)

target_link_libraries(sourcemeta_core_io_unit
  PRIVATE sourcemeta::core::io)
target_compile_definitions(sourcemeta_core_io_unit
  PRIVATE STUBS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/stubs"
  PRIVATE BUILD_DIRECTORY="${CMAKE_CURRENT_BINARY_DIR}")
