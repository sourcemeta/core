sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME crypto
  SOURCES
    crypto_sha256_test.cc
    crypto_sha384_test.cc
    crypto_sha512_test.cc
    crypto_sha1_test.cc
    crypto_fnv128_test.cc
    crypto_uuid_test.cc
    crypto_crc32_test.cc
    crypto_base64_test.cc
    crypto_base64url_test.cc
    crypto_rsassa_pkcs1_v15_test.cc
    crypto_rsassa_pss_test.cc
    crypto_ecdsa_test.cc)

target_link_libraries(sourcemeta_core_crypto_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::text)

# pyca/cryptography Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography
  SOURCES pyca_cryptography_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::io sourcemeta::core::text)
