sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME semver
  SOURCES semver_parse_test.cc semver_parse_error_test.cc semver_from_test.cc
          semver_compare_test.cc semver_to_string_test.cc)

target_link_libraries(sourcemeta_core_semver_unit
  PRIVATE sourcemeta::core::semver)
