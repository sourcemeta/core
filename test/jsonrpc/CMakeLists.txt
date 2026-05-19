sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME jsonrpc
  SOURCES jsonrpc_test.cc)

target_link_libraries(sourcemeta_core_jsonrpc_unit
  PRIVATE sourcemeta::core::jsonrpc)
