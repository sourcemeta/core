sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME jsonschema
  SOURCES
    jsonschema_reidentify_test.cc
    jsonschema_vocabulary_test.cc
    jsonschema_walker_2020_12_test.cc
    jsonschema_walker_2019_09_test.cc
    jsonschema_walker_draft7_test.cc
    jsonschema_walker_draft6_test.cc
    jsonschema_walker_draft4_test.cc
    jsonschema_walker_draft3_test.cc
    jsonschema_walker_draft2_test.cc
    jsonschema_walker_draft1_test.cc
    jsonschema_walker_draft0_test.cc
    jsonschema_walker_openapi_test.cc
    jsonschema_error_test.cc
    jsonschema_resolver_test.cc
    jsonschema_frame_test.cc
    jsonschema_frame_error_test.cc
    jsonschema_frame_limit_test.cc)

target_link_libraries(sourcemeta_core_jsonschema_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonschema_unit
  PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_core_jsonschema_unit
  PRIVATE sourcemeta::core::jsonschema)
target_link_libraries(sourcemeta_core_jsonschema_unit
  PRIVATE sourcemeta::core::uri)

if(MSVC)
  target_compile_options(sourcemeta_core_jsonschema_unit PRIVATE /bigobj)
endif()

# JSON Schema Referencing Suite
# See https://github.com/python-jsonschema/referencing-suite
sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME jsonschema_referencing_suite
  SOURCES referencingsuite.cc)
target_compile_definitions(sourcemeta_core_jsonschema_referencing_suite_unit
  PRIVATE REFERENCING_SUITE_PATH="${PROJECT_SOURCE_DIR}/vendor/referencing-suite/tests")
target_link_libraries(sourcemeta_core_jsonschema_referencing_suite_unit PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_core_jsonschema_referencing_suite_unit PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonschema_referencing_suite_unit PRIVATE sourcemeta::core::jsonschema)
target_link_libraries(sourcemeta_core_jsonschema_referencing_suite_unit PRIVATE sourcemeta::core::jsonpointer)

# Complete frame tests, expressed as JSON fixtures
sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME jsonschema_frame_suite
  SOURCES framesuite.cc)
target_compile_definitions(sourcemeta_core_jsonschema_frame_suite_unit
  PRIVATE FRAME_SUITE_PATH="${PROJECT_SOURCE_DIR}/test/jsonschema/frame")
target_link_libraries(sourcemeta_core_jsonschema_frame_suite_unit PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonschema_frame_suite_unit PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_core_jsonschema_frame_suite_unit PRIVATE sourcemeta::core::jsonschema)
