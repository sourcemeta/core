sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME json
  SOURCES
    json_array_test.cc
    json_boolean_test.cc
    json_error_test.cc
    json_hash_test.cc
    json_integer_test.cc
    json_null_test.cc
    json_number_test.cc
    json_object_test.cc
    json_parse_callback_test.cc
    json_parse_error_test.cc
    json_parse_test.cc
    json_prettify_test.cc
    json_real_test.cc
    json_string_test.cc
    json_stringify_test.cc
    json_value_test.cc
    json_type_test.cc
    json_auto_test.cc)

target_link_libraries(sourcemeta_core_json_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_json_unit
  PRIVATE sourcemeta::core::io)
target_compile_definitions(sourcemeta_core_json_unit
  PRIVATE TEST_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

# JSON Test Suite
# See https://github.com/nst/JSONTestSuite
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jsontestsuite
  SOURCES jsontestsuite.cc)
target_compile_definitions(sourcemeta_core_jsontestsuite_unit
  PRIVATE JSONTESTSUITE_PATH="${PROJECT_SOURCE_DIR}/vendor/jsontestsuite")
target_link_libraries(sourcemeta_core_jsontestsuite_unit PRIVATE sourcemeta::core::json)
