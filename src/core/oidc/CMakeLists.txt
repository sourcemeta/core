sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME oidc
  PRIVATE_HEADERS error.h metadata.h discovery.h
  SOURCES oidc_error.cc oidc_metadata.cc oidc_discovery.cc)

target_link_libraries(sourcemeta_core_oidc
  PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_core_oidc
  PUBLIC sourcemeta::core::oauth)
target_link_libraries(sourcemeta_core_oidc
  PRIVATE sourcemeta::core::uri)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME oidc)
endif()
