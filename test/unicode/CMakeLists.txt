sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME unicode
  SOURCES
    codepoint_to_utf8_test.cc
    utf8_to_utf32_test.cc
    utf8_codepoint_length_test.cc)

target_link_libraries(sourcemeta_core_unicode_unit
  PRIVATE sourcemeta::core::unicode)
