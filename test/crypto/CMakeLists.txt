sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME crypto
  SOURCES crypto_md5_test.cc)

target_link_libraries(sourcemeta_core_crypto_unit
  PRIVATE sourcemeta::core::crypto)

# pyca/cryptography MD5 Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography_md5
  SOURCES pyca_cryptography_md5_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_md5_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_md5_unit
  PRIVATE sourcemeta::core::crypto)

# SHA-256 Unit Tests
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME crypto_sha256
  SOURCES crypto_sha256_test.cc)
target_link_libraries(sourcemeta_core_crypto_sha256_unit
  PRIVATE sourcemeta::core::crypto)

# pyca/cryptography SHA-256 Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography_sha256
  SOURCES pyca_cryptography_sha256_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_sha256_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_sha256_unit
  PRIVATE sourcemeta::core::crypto)
