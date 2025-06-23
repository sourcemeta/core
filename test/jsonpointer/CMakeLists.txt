sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jsonpointer
  SOURCES
    jsonpointer_empty_pointer_test.cc
    jsonpointer_get_test.cc
    jsonpointer_try_get_test.cc
    jsonpointer_error_test.cc
    jsonpointer_parse_error_test.cc
    jsonpointer_parse_test.cc
    jsonpointer_pointer_test.cc
    jsonpointer_position_test.cc
    jsonpointer_template_test.cc
    jsonpointer_weakpointer_test.cc
    jsonpointer_resolve_from_test.cc
    jsonpointer_set_test.cc
    jsonpointer_stringify_test.cc
    jsonpointer_token_test.cc
    jsonpointer_walker_test.cc
    jsonpointer_subpointer_walker_test.cc
    jsonpointer_to_string_test.cc
    jsonpointer_to_uri_test.cc
    jsonpointer_concat_test.cc
    jsonpointer_starts_with_test.cc
    jsonpointer_starts_with_initial_test.cc
    jsonpointer_to_json_test.cc
    jsonpointer_rebase_test.cc)

target_link_libraries(sourcemeta_core_jsonpointer_unit
  PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_core_jsonpointer_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonpointer_unit
  PRIVATE sourcemeta::core::uri)
