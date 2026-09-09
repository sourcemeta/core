sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME terminal
  SOURCES terminal_test.cc)

target_link_libraries(sourcemeta_core_terminal_unit
  PRIVATE sourcemeta::core::terminal)
