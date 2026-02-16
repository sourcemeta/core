sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME crypto
  SOURCES
    crypto_sha256_test.cc
    crypto_uuid_test.cc)

target_link_libraries(sourcemeta_core_crypto_unit
  PRIVATE sourcemeta::core::crypto)

# pyca/cryptography SHA-256 Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography_sha256
  SOURCES pyca_cryptography_sha256_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_sha256_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_sha256_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::io)
