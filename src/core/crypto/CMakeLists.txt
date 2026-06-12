sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME crypto
  PRIVATE_HEADERS sha256.h sha384.h sha512.h sha1.h fnv128.h uuid.h crc32.h
    base64.h verify.h
  SOURCES crypto_sha256.cc crypto_sha384.cc crypto_sha512.cc crypto_sha2_64.h
    crypto_sha1.cc crypto_fnv128.cc crypto_uuid.cc crypto_crc32.cc
    crypto_base64.cc)

if(SOURCEMETA_CORE_CRYPTO_USE_SYSTEM_OPENSSL)
  target_sources(sourcemeta_core_crypto PRIVATE crypto_verify_openssl.cc)
  target_compile_definitions(sourcemeta_core_crypto
    PRIVATE SOURCEMETA_CORE_CRYPTO_USE_SYSTEM_OPENSSL)
  target_link_libraries(sourcemeta_core_crypto PRIVATE OpenSSL::Crypto)
elseif(APPLE)
  target_sources(sourcemeta_core_crypto PRIVATE crypto_verify_apple.cc)
  target_link_libraries(sourcemeta_core_crypto PRIVATE "-framework Security")
  target_link_libraries(sourcemeta_core_crypto
    PRIVATE "-framework CoreFoundation")
elseif(WIN32)
  target_sources(sourcemeta_core_crypto PRIVATE crypto_verify_windows.cc)
  target_link_libraries(sourcemeta_core_crypto PRIVATE bcrypt)
else()
  message(WARNING "Building the reference signature verification
    backend, instead of the OpenSSL recommended production one")
  target_sources(sourcemeta_core_crypto PRIVATE
    crypto_verify_other.cc crypto_bignum.h)
endif()

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME crypto)
endif()
