sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME error
  PRIVATE_HEADERS base.h file.h static_string.h
  SOURCES error.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME error)
endif()
