sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME crypto
  SOURCES crypto_md5_test.cc)

target_link_libraries(sourcemeta_core_crypto_unit
  PRIVATE sourcemeta::core::crypto)
