sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME md5 SOURCES md5.cc)
sourcemeta_clang_tidy_attempt_enable(TARGET_NAME "sourcemeta_core_md5")

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME md5)
endif()

target_link_libraries(sourcemeta_core_md5 PRIVATE BearSSL::BearSSL)
