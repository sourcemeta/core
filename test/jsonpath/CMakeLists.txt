sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME jsonpath
  SOURCES
    jsonpath_parse_test.cc
    jsonpath_evaluate_test.cc
    jsonpath_filter_test.cc
    jsonpath_normalize_test.cc
    jsonpath_json_auto_test.cc)

target_link_libraries(sourcemeta_core_jsonpath_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonpath_unit
  PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_core_jsonpath_unit
  PRIVATE sourcemeta::core::jsonpath)

if(MSVC)
  target_compile_options(sourcemeta_core_jsonpath_unit PRIVATE /utf-8)
endif()

# JSONPath Compliance Test Suite
# See https://github.com/jsonpath-standard/jsonpath-compliance-test-suite
sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME jsonpath_suite
  SOURCES jsonpath_suite.cc)
target_compile_definitions(sourcemeta_core_jsonpath_suite_unit
  PRIVATE JSONPATH_SUITE_PATH="${PROJECT_SOURCE_DIR}/vendor/jsonpath-compliance-test-suite")
target_link_libraries(sourcemeta_core_jsonpath_suite_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonpath_suite_unit
  PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_core_jsonpath_suite_unit
  PRIVATE sourcemeta::core::jsonpath)
