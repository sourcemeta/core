sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME oauth
  SOURCES oauth_error_test.cc oauth_pkce_test.cc oauth_bearer_test.cc)

target_link_libraries(sourcemeta_core_oauth_unit
  PRIVATE sourcemeta::core::oauth)
target_link_libraries(sourcemeta_core_oauth_unit
  PRIVATE sourcemeta::core::http)
