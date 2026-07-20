sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME oauth
  SOURCES oauth_error_test.cc oauth_pkce_test.cc oauth_bearer_test.cc
    oauth_random_test.cc oauth_authorization_test.cc oauth_token_test.cc
    oauth_client_authentication_test.cc oauth_transaction_test.cc
    oauth_metadata_test.cc oauth_metadata_provider_test.cc)

target_link_libraries(sourcemeta_core_oauth_unit
  PRIVATE sourcemeta::core::oauth)
target_link_libraries(sourcemeta_core_oauth_unit
  PRIVATE sourcemeta::core::http)
