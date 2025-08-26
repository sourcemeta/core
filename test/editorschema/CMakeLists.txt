sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME editorschema
  SOURCES editorschema_test.cc)

target_link_libraries(sourcemeta_core_editorschema_unit
  PRIVATE sourcemeta::core::editorschema)
target_link_libraries(sourcemeta_core_editorschema_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_editorschema_unit
  PRIVATE sourcemeta::core::jsonschema)
