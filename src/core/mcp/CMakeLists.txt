sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME mcp
  PRIVATE_HEADERS capabilities.h error.h headers.h protocol.h results.h
  SOURCES helpers.h capabilities.cc error.cc headers.cc results.cc)

target_link_libraries(sourcemeta_core_mcp PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_core_mcp PUBLIC sourcemeta::core::jsonrpc)
target_link_libraries(sourcemeta_core_mcp PUBLIC sourcemeta::core::http)
target_link_libraries(sourcemeta_core_mcp PRIVATE sourcemeta::core::crypto)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME mcp)
endif()
