sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME uritemplate
  PRIVATE_HEADERS error.h token.h
  SOURCES helpers.h uritemplate.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME uritemplate)
endif()
