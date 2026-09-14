sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME markdown
  SOURCES
    markdown_blocks_test.cc
    markdown_concurrency_test.cc
    markdown_extensions_test.cc
    markdown_inlines_test.cc
    markdown_pathological_test.cc
    markdown_safety_test.cc
    markdown_test.cc
    markdown_unsafe_test.cc)

target_link_libraries(sourcemeta_core_markdown_unit
  PRIVATE sourcemeta::core::markdown)
