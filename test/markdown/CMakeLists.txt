sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME markdown
  SOURCES markdown_test.cc)

target_link_libraries(sourcemeta_core_markdown_unit
  PRIVATE sourcemeta::core::markdown)
