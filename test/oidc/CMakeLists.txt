sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME oidc
  SOURCES oidc_error_test.cc oidc_metadata_test.cc oidc_discovery_test.cc)

target_link_libraries(sourcemeta_core_oidc_unit
  PRIVATE sourcemeta::core::oidc)
target_link_libraries(sourcemeta_core_oidc_unit
  PRIVATE sourcemeta::core::oauth)
