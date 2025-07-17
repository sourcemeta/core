sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME uuid SOURCES uuid.cc)
sourcemeta_clang_tidy_attempt_enable(TARGET_NAME "sourcemeta_core_uuid")

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME uuid)
endif()
