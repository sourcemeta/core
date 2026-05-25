sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME unicode
  SOURCES
    codepoint_to_utf8_test.cc
    utf8_to_utf32_test.cc
    utf8_codepoint_length_test.cc
    utf8_lead_byte_size_test.cc
    utf8_codepoint_byte_count_test.cc
    is_utf8_continuation_test.cc
    is_surrogate_test.cc
    is_valid_codepoint_test.cc)

target_link_libraries(sourcemeta_core_unicode_unit
  PRIVATE sourcemeta::core::unicode)
