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
    crypto_rsassa_pkcs1_v15_test.cc)

target_link_libraries(sourcemeta_core_crypto_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::text)

# pyca/cryptography SHA-256 Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography_sha256
  SOURCES pyca_cryptography_sha256_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_sha256_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_sha256_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::io sourcemeta::core::text)

# pyca/cryptography SHA-384 Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography_sha384
  SOURCES pyca_cryptography_sha384_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_sha384_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_sha384_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::io sourcemeta::core::text)

# pyca/cryptography SHA-512 Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography_sha512
  SOURCES pyca_cryptography_sha512_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_sha512_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_sha512_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::io sourcemeta::core::text)

# pyca/cryptography SHA-1 Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography_sha1
  SOURCES pyca_cryptography_sha1_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_sha1_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_sha1_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::io sourcemeta::core::text)

# pyca/cryptography RSASSA-PKCS1-v1_5 Verification Test Vectors
# See https://github.com/pyca/cryptography
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME pyca_cryptography_rsa_sigver15
  SOURCES pyca_cryptography_rsa_sigver15_test.cc)
target_compile_definitions(sourcemeta_core_pyca_cryptography_rsa_sigver15_unit
  PRIVATE PYCA_CRYPTOGRAPHY_PATH="${PROJECT_SOURCE_DIR}/vendor/pyca-cryptography/vectors/cryptography_vectors")
target_link_libraries(sourcemeta_core_pyca_cryptography_rsa_sigver15_unit
  PRIVATE sourcemeta::core::crypto sourcemeta::core::io sourcemeta::core::text)
