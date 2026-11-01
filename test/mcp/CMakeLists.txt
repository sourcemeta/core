sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME mcp
  SOURCES
    mcp_helpers.h
    mcp_capabilities_test.cc
    mcp_error_test.cc
    mcp_headers_test.cc
    mcp_logging_test.cc
    mcp_notifications_test.cc
    mcp_protocol_test.cc
    mcp_results_test.cc)

target_link_libraries(sourcemeta_core_mcp_unit
  PRIVATE sourcemeta::core::mcp)

# Enable with an installed Blaze available through CMAKE_PREFIX_PATH.
# Ordinary MCP tests do not discover or require an external evaluator.
option(SOURCEMETA_CORE_MCP_TESTS_BLAZE
  "Test MCP application schema validation using Blaze" OFF)
if(SOURCEMETA_CORE_MCP_TESTS_BLAZE)
  find_package(Blaze REQUIRED COMPONENTS compiler evaluator)
  target_sources(sourcemeta_core_mcp_unit PRIVATE mcp_schema_validation_test.cc)
  target_link_libraries(sourcemeta_core_mcp_unit PRIVATE
    sourcemeta::core::jsonschema
    sourcemeta::blaze::compiler sourcemeta::blaze::evaluator)
endif()
