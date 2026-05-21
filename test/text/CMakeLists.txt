sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME text
  SOURCES text_to_title_case_test.cc)

target_link_libraries(sourcemeta_core_text_unit
  PRIVATE sourcemeta::core::text)
