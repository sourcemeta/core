sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME html
  SOURCES html_buffer_test.cc html_encoder_test.cc html_entity_test.cc
    html_escape_test.cc)

target_link_libraries(sourcemeta_core_html_unit
  PRIVATE sourcemeta::core::html)
