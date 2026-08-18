sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME oidc
  SOURCES oidc_error_test.cc oidc_metadata_test.cc oidc_discovery_test.cc
    oidc_hash_test.cc oidc_id_token_test.cc oidc_authentication_test.cc
    oidc_claims_test.cc oidc_claim_request_accepts_multi_valued_test.cc
    oidc_userinfo_test.cc oidc_registration_test.cc
    oidc_subject_test.cc oidc_request_object_test.cc oidc_logout_test.cc
    oidc_encryption_test.cc)

target_link_libraries(sourcemeta_core_oidc_unit
  PRIVATE sourcemeta::core::oidc)
target_link_libraries(sourcemeta_core_oidc_unit
  PRIVATE sourcemeta::core::jose)
target_link_libraries(sourcemeta_core_oidc_unit
  PRIVATE sourcemeta::core::oauth)
