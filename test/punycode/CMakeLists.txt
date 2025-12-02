sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME punycode
  SOURCES punycode_encode_test.cc punycode_decode_test.cc)

target_link_libraries(sourcemeta_core_punycode_unit
  PRIVATE sourcemeta::core::punycode)
