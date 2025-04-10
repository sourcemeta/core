sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jsonschema
  SOURCES
    jsonschema_test_utils.h
    jsonschema_identify_2020_12_test.cc
    jsonschema_identify_2019_09_test.cc
    jsonschema_identify_draft7_test.cc
    jsonschema_identify_draft6_test.cc
    jsonschema_identify_draft4_test.cc
    jsonschema_identify_draft3_test.cc
    jsonschema_identify_draft2_test.cc
    jsonschema_identify_draft1_test.cc
    jsonschema_identify_draft0_test.cc
    jsonschema_identify_test.cc
    jsonschema_is_schema_test.cc
    jsonschema_frame_2020_12_test.cc
    jsonschema_frame_2019_09_test.cc
    jsonschema_frame_draft7_test.cc
    jsonschema_frame_draft6_test.cc
    jsonschema_frame_draft4_test.cc
    jsonschema_frame_draft3_test.cc
    jsonschema_frame_draft2_test.cc
    jsonschema_frame_draft1_test.cc
    jsonschema_frame_draft0_test.cc
    jsonschema_frame_test.cc
    jsonschema_unevaluated_2020_12_test.cc
    jsonschema_unevaluated_2019_09_test.cc
    jsonschema_bundle_2020_12_test.cc
    jsonschema_bundle_2019_09_test.cc
    jsonschema_bundle_draft7_test.cc
    jsonschema_bundle_draft6_test.cc
    jsonschema_bundle_draft4_test.cc
    jsonschema_bundle_draft3_test.cc
    jsonschema_bundle_draft2_test.cc
    jsonschema_bundle_draft1_test.cc
    jsonschema_bundle_draft0_test.cc
    jsonschema_bundle_test.cc
    jsonschema_unidentify_test.cc
    jsonschema_metaschema_test.cc
    jsonschema_transform_rules.h
    jsonschema_transform_rule_test.cc
    jsonschema_transformer_test.cc
    jsonschema_dialect_test.cc
    jsonschema_dialect_2020_12_test.cc
    jsonschema_dialect_2019_09_test.cc
    jsonschema_dialect_draft7_test.cc
    jsonschema_dialect_draft6_test.cc
    jsonschema_dialect_draft4_test.cc
    jsonschema_dialect_draft3_test.cc
    jsonschema_dialect_draft2_test.cc
    jsonschema_dialect_draft1_test.cc
    jsonschema_dialect_draft0_test.cc
    jsonschema_base_dialect_test.cc
    jsonschema_base_dialect_2020_12_test.cc
    jsonschema_base_dialect_2019_09_test.cc
    jsonschema_base_dialect_draft7_test.cc
    jsonschema_base_dialect_draft6_test.cc
    jsonschema_base_dialect_draft4_test.cc
    jsonschema_base_dialect_draft3_test.cc
    jsonschema_base_dialect_draft2_test.cc
    jsonschema_base_dialect_draft1_test.cc
    jsonschema_base_dialect_draft0_test.cc
    jsonschema_vocabulary_2020_12_test.cc
    jsonschema_vocabulary_2019_09_test.cc
    jsonschema_vocabulary_draft7_test.cc
    jsonschema_vocabulary_draft6_test.cc
    jsonschema_vocabulary_draft4_test.cc
    jsonschema_vocabulary_draft3_test.cc
    jsonschema_vocabulary_draft2_test.cc
    jsonschema_vocabulary_draft1_test.cc
    jsonschema_vocabulary_draft0_test.cc
    jsonschema_vocabulary_test.cc
    jsonschema_official_walker_2020_12_test.cc
    jsonschema_official_walker_2019_09_test.cc
    jsonschema_official_walker_draft7_test.cc
    jsonschema_official_walker_draft6_test.cc
    jsonschema_official_walker_draft4_test.cc
    jsonschema_official_walker_draft3_test.cc
    jsonschema_official_walker_draft2_test.cc
    jsonschema_official_walker_draft1_test.cc
    jsonschema_official_walker_draft0_test.cc
    jsonschema_walker_test.cc
    jsonschema_error_test.cc
    jsonschema_keyword_iterator_test.cc
    jsonschema_official_resolver_test.cc
    jsonschema_relativize_test.cc
    jsonschema_map_resolver_test.cc
    jsonschema_flat_file_resolver_test.cc
    jsonschema_format_test.cc
    jsonschema_wrap_test.cc)

target_link_libraries(sourcemeta_core_jsonschema_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonschema_unit
  PRIVATE sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_core_jsonschema_unit
  PRIVATE sourcemeta::core::jsonschema)
target_link_libraries(sourcemeta_core_jsonschema_unit
  PRIVATE sourcemeta::core::uri)
target_compile_definitions(sourcemeta_core_jsonschema_unit
  PRIVATE SCHEMAS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/schemas")

# JSON Schema Referencing Suite
# See https://github.com/python-jsonschema/referencing-suite
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jsonschema_referencing_suite
  SOURCES referencingsuite.cc)
target_compile_definitions(sourcemeta_core_jsonschema_referencing_suite_unit
  PRIVATE REFERENCING_SUITE_PATH="${PROJECT_SOURCE_DIR}/vendor/referencing-suite/tests")
target_link_libraries(sourcemeta_core_jsonschema_referencing_suite_unit PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_core_jsonschema_referencing_suite_unit PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonschema_referencing_suite_unit PRIVATE sourcemeta::core::jsonschema)
target_link_libraries(sourcemeta_core_jsonschema_referencing_suite_unit PRIVATE sourcemeta::core::jsonpointer)
