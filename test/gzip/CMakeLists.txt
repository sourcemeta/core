sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME gzip
  SOURCES
    gzip_test.cc
    gzip_streambuf_test.cc)

target_link_libraries(sourcemeta_core_gzip_unit
  PRIVATE sourcemeta::core::gzip)
target_link_libraries(sourcemeta_core_gzip_unit
  PRIVATE sourcemeta::core::io)
