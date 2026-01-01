sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME uritemplate
  SOURCES
    uritemplate_helpers.h
    uritemplate_test.cc
    uritemplate_parse_test.cc
    uritemplate_parse_error_test.cc
    uritemplate_expand_test.cc
    uritemplate_router_test.cc
    uritemplate_router_view_test.cc)

target_link_libraries(sourcemeta_core_uritemplate_unit
  PRIVATE sourcemeta::core::uritemplate)

# URI Template Test Suite
# See https://github.com/uri-templates/uritemplate-test
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME uritemplate_suite
  SOURCES uritemplate_suite.cc)
target_compile_definitions(sourcemeta_core_uritemplate_suite_unit
  PRIVATE URITEMPLATE_SUITE_PATH="${PROJECT_SOURCE_DIR}/vendor/uritemplate-test")
target_link_libraries(sourcemeta_core_uritemplate_suite_unit
  PRIVATE sourcemeta::core::uritemplate)
target_link_libraries(sourcemeta_core_uritemplate_suite_unit
  PRIVATE sourcemeta::core::json)
