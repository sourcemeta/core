sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME yaml
  SOURCES
    yaml_parse_test.cc
    yaml_parse_callback_test.cc)

target_link_libraries(sourcemeta_core_yaml_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_yaml_unit
  PRIVATE sourcemeta::core::yaml)

target_compile_definitions(sourcemeta_core_yaml_unit
  PRIVATE STUBS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/stubs")
