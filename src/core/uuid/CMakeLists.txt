sourcemeta_library(NAMESPACE sourcemeta
    PROJECT core
    NAME uuid
    SOURCES uuid.cc
    CLANG_TIDY "${SOURCEMETA_CXX_CLANG_TIDY}")

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME uuid)
endif()
