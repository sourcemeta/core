sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME jsonrpc)

target_link_libraries(sourcemeta_core_jsonrpc INTERFACE sourcemeta::core::json)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME jsonrpc)
endif()
