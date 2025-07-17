sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME yaml
  PRIVATE_HEADERS error.h
  SOURCES yaml.cc)
sourcemeta_clang_tidy_attempt_enable(TARGET_NAME "sourcemeta_core_yaml")

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME yaml)
endif()

target_link_libraries(sourcemeta_core_yaml PRIVATE yaml)
target_link_libraries(sourcemeta_core_yaml PUBLIC sourcemeta::core::json)
