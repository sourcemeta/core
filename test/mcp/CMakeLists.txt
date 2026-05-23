sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME mcp
  SOURCES mcp_test.cc)

target_link_libraries(sourcemeta_core_mcp_unit
  PRIVATE sourcemeta::core::mcp)
