sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME mcp
  SOURCES
    mcp_conformance_test.cc
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
