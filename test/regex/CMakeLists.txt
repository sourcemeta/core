sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME regex
  SOURCES
    regex_matches_if_valid_test.cc
    regex_matches_ecma262_test.cc
    regex_matches_rfc9485_test.cc
    regex_to_regex_test.cc)

target_link_libraries(sourcemeta_core_regex_unit
  PRIVATE sourcemeta::core::regex)
