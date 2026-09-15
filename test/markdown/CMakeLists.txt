sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME markdown
  SOURCES
    markdown_autolinks_test.cc
    markdown_blockquotes_test.cc
    markdown_characters_test.cc
    markdown_code_blocks_test.cc
    markdown_code_spans_test.cc
    markdown_concurrency_test.cc
    markdown_emphasis_test.cc
    markdown_entities_test.cc
    markdown_escapes_test.cc
    markdown_footnotes_test.cc
    markdown_headings_test.cc
    markdown_images_test.cc
    markdown_line_breaks_test.cc
    markdown_links_test.cc
    markdown_lists_test.cc
    markdown_paragraphs_test.cc
    markdown_pathological_test.cc
    markdown_safety_test.cc
    markdown_spec_blocks_test.cc
    markdown_spec_inlines_test.cc
    markdown_strikethrough_test.cc
    markdown_tables_test.cc
    markdown_task_lists_test.cc
    markdown_thematic_breaks_test.cc
    markdown_unsafe_test.cc)

target_link_libraries(sourcemeta_core_markdown_unit
  PRIVATE sourcemeta::core::markdown)
