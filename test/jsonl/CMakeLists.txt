sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jsonl
  SOURCES
    jsonl_parse_test.cc
    jsonl_parse_error_test.cc)

target_link_libraries(sourcemeta_core_jsonl_unit
  PRIVATE sourcemeta::core::jsonl)
