sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jose
  SOURCES jose_algorithm_test.cc jose_jwk_test.cc jose_jwks_test.cc
          jose_jwt_test.cc jose_jwt_check_claims_test.cc
          jose_jws_verify_signature_test.cc jose_jwt_verify_signature_test.cc
          jose_jwt_verify_test.cc)

target_link_libraries(sourcemeta_core_jose_unit
  PRIVATE sourcemeta::core::jose)
target_link_libraries(sourcemeta_core_jose_unit
  PRIVATE sourcemeta::core::crypto)

# IETF JOSE Cookbook (RFC 7520) JWS Examples
# See https://github.com/ietf-jose/cookbook
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jose_cookbook
  SOURCES jose_cookbook_test.cc)
target_compile_definitions(sourcemeta_core_jose_cookbook_unit
  PRIVATE JOSE_COOKBOOK_PATH="${PROJECT_SOURCE_DIR}/vendor/jose-cookbook")
target_link_libraries(sourcemeta_core_jose_cookbook_unit
  PRIVATE sourcemeta::core::jose)
target_link_libraries(sourcemeta_core_jose_cookbook_unit
  PRIVATE sourcemeta::core::crypto)
target_link_libraries(sourcemeta_core_jose_cookbook_unit
  PRIVATE sourcemeta::core::io)
target_link_libraries(sourcemeta_core_jose_cookbook_unit
  PRIVATE sourcemeta::core::json)
