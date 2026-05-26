sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME unicode
  SOURCES
    unicode_codepoint_to_utf8_test.cc
    unicode_utf8_to_utf32_test.cc
    unicode_utf8_codepoint_length_test.cc
    unicode_utf8_lead_byte_size_test.cc
    unicode_utf8_codepoint_byte_count_test.cc
    unicode_is_utf8_continuation_test.cc
    unicode_is_surrogate_test.cc
    unicode_is_valid_codepoint_test.cc
    unicode_combining_class_test.cc
    unicode_joining_type_test.cc
    unicode_bidi_class_test.cc)

target_link_libraries(sourcemeta_core_unicode_unit
  PRIVATE sourcemeta::core::unicode)
