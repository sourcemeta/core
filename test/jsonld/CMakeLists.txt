sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jsonld
  SOURCES jsonld_expand_test.cc jsonld_expand_error_test.cc)

target_link_libraries(sourcemeta_core_jsonld_unit
  PRIVATE sourcemeta::core::jsonld)
target_link_libraries(sourcemeta_core_jsonld_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonld_unit
  PRIVATE sourcemeta::core::jsonpointer)

# W3C JSON-LD Test Suite
# See https://github.com/w3c/json-ld-api
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jsonld_suite
  SOURCES jsonld_suite.cc)
target_compile_definitions(sourcemeta_core_jsonld_suite_unit
  PRIVATE JSONLD_SUITE_PATH="${PROJECT_SOURCE_DIR}/vendor/w3c-json-ld/tests")
target_link_libraries(sourcemeta_core_jsonld_suite_unit
  PRIVATE sourcemeta::core::jsonld)
target_link_libraries(sourcemeta_core_jsonld_suite_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonld_suite_unit
  PRIVATE sourcemeta::core::text)
