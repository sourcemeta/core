sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME argv SOURCES argv.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME argv)
endif()
