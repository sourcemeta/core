sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME crypto
  PRIVATE_HEADERS md5.h
  SOURCES crypto_md5.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME crypto)
endif()
