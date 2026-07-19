sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME oauth
  PRIVATE_HEADERS error.h profile.h pkce.h bearer.h
  SOURCES oauth_error.cc oauth_pkce.cc oauth_bearer.cc oauth_syntax.h)

target_link_libraries(sourcemeta_core_oauth
  PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_core_oauth
  PUBLIC sourcemeta::core::crypto)
target_link_libraries(sourcemeta_core_oauth
  PUBLIC sourcemeta::core::jose)
target_link_libraries(sourcemeta_core_oauth
  PUBLIC sourcemeta::core::http)
target_link_libraries(sourcemeta_core_oauth
  PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_core_oauth
  PRIVATE sourcemeta::core::text)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME oauth)
endif()
