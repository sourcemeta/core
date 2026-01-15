sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME html
  SOURCES html_encoder_test.cc html_escape_test.cc)

target_link_libraries(sourcemeta_core_html_unit
  PRIVATE sourcemeta::core::html)
