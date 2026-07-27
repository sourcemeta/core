sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME unicode
  SOURCES
    unicode_codepoint_to_utf8_test.cc
    unicode_utf8_to_utf32_test.cc
    unicode_utf32_to_utf8_test.cc
    unicode_utf8_to_wide_test.cc
    unicode_wide_to_utf8_test.cc
    unicode_utf8_codepoint_length_test.cc
    unicode_utf8_decode_test.cc
    unicode_utf8_lead_byte_size_test.cc
    unicode_utf8_codepoint_byte_count_test.cc
    unicode_utf8_codepoint_count_test.cc
    unicode_utf8_codepoint_within_test.cc
    unicode_is_utf8_continuation_test.cc
    unicode_is_surrogate_test.cc
    unicode_is_valid_codepoint_test.cc
    unicode_combining_class_test.cc
    unicode_joining_type_test.cc
    unicode_bidi_class_test.cc
    unicode_script_test.cc
    unicode_is_combining_mark_test.cc
    unicode_nfc_quick_check_test.cc
    unicode_canonical_decomposition_test.cc
    unicode_canonical_composition_test.cc
    unicode_nfc_test.cc
    unicode_is_nfc_test.cc
    unicode_is_ucschar_test.cc
    unicode_is_iprivate_test.cc)

target_link_libraries(sourcemeta_core_unicode_unit
  PRIVATE sourcemeta::core::unicode)

# UAX #15 NormalizationTest.txt conformance pass
sourcemeta_test(NAMESPACE sourcemeta PROJECT core
  NAME unicode_normalization_suite
  SOURCES unicode_normalization_suite.cc)
target_compile_definitions(sourcemeta_core_unicode_normalization_suite_unit
  PRIVATE NORMALIZATIONTEST_PATH="${PROJECT_SOURCE_DIR}/vendor/unicodetools/unicodetools/data/ucd/dev/NormalizationTest.txt")
target_link_libraries(sourcemeta_core_unicode_normalization_suite_unit
  PRIVATE sourcemeta::core::unicode)
target_link_libraries(sourcemeta_core_unicode_normalization_suite_unit
  PRIVATE sourcemeta::core::io)
