sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME gzip
  SOURCES
    gzip_compress_error_test.cc
    gzip_compress_format_test.cc
    gzip_compress_size_test.cc
    gzip_compress_test.cc
    gzip_decompress_error_test.cc
    gzip_decompress_limits_error_test.cc
    gzip_decompress_limits_test.cc
    gzip_decompress_test.cc
    gzip_streambuf_test.cc)

target_link_libraries(sourcemeta_core_gzip_unit
  PRIVATE sourcemeta::core::gzip)
