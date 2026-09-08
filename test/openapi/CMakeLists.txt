sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME openapi
  SOURCES openapi_frame_test.cc openapi_version_test.cc)

target_link_libraries(sourcemeta_core_openapi_unit
  PRIVATE sourcemeta::core::openapi)
target_link_libraries(sourcemeta_core_openapi_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_openapi_unit
  PRIVATE sourcemeta::core::jsonpointer)

# Complete frame tests, expressed as JSON fixtures
sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME openapi_frame_suite
  SOURCES framesuite.cc)
target_compile_definitions(sourcemeta_core_openapi_frame_suite_unit
  PRIVATE FRAME_SUITE_PATH="${PROJECT_SOURCE_DIR}/test/openapi/frame")
target_link_libraries(sourcemeta_core_openapi_frame_suite_unit
  PRIVATE sourcemeta::core::openapi)
target_link_libraries(sourcemeta_core_openapi_frame_suite_unit
  PRIVATE sourcemeta::core::json)

# The OpenAPI Initiative's own meta-schema fixtures
# See https://github.com/OAI/OpenAPI-Specification/tree/v3.1-dev/tests/schema
sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME openapi_suite
  SOURCES oaisuite.cc)
target_compile_definitions(sourcemeta_core_openapi_suite_unit
  PRIVATE OPENAPI_SUITE_PATH="${PROJECT_SOURCE_DIR}/vendor/openapi-test-suite/tests/schema")
target_link_libraries(sourcemeta_core_openapi_suite_unit
  PRIVATE sourcemeta::core::openapi)
target_link_libraries(sourcemeta_core_openapi_suite_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_openapi_suite_unit
  PRIVATE sourcemeta::core::yaml)
