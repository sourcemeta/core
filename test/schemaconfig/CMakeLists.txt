sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME schemaconfig
  SOURCES
    schemaconfig_find_test.cc
    schemaconfig_from_json_test.cc
    schemaconfig_read_json_test.cc)

target_compile_definitions(sourcemeta_core_schemaconfig_unit
  PRIVATE TEST_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(sourcemeta_core_schemaconfig_unit
  PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_core_schemaconfig_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_schemaconfig_unit
  PRIVATE sourcemeta::core::schemaconfig)
