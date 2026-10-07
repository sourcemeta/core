sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME mcp
  SOURCES mcp_protocol_test.cc mcp_error_test.cc mcp_results_test.cc)

target_link_libraries(sourcemeta_core_mcp_unit
  PRIVATE sourcemeta::core::mcp)
