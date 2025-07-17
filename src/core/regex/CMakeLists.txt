sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME regex)
sourcemeta_clang_tidy_attempt_enable(TARGET_NAME "sourcemeta_core_regex")

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME regex)
endif()

target_link_libraries(sourcemeta_core_regex INTERFACE Boost::regex)
