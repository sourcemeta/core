#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <sourcemeta/core/test.h>

#include "mcp_helpers.h"

#include <array>     // std::array
#include <cstdint>   // std::int64_t
#include <limits>    // std::numeric_limits
#include <optional>  // std::optional, std::nullopt
#include <stdexcept> // std::invalid_argument
#include <string>    // std::string

TEST(code_resource_not_found) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
            static_cast<std::int64_t>(-32002));
}

TEST(code_url_elicitation_required) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_URL_ELICITATION_REQUIRED,
            static_cast<std::int64_t>(-32042));
}

TEST(code_header_mismatch) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_HEADER_MISMATCH,
            static_cast<std::int64_t>(-32020));
}

TEST(code_missing_required_client_capability) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
            static_cast<std::int64_t>(-32021));
}

TEST(code_unsupported_protocol_version) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
            static_cast<std::int64_t>(-32022));
}

TEST(make_error_with_integer_identifier) {
  const sourcemeta::core::JSON identifier{42};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_with_string_identifier) {
  const sourcemeta::core::JSON identifier{"abc"};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "abc",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_with_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::JSONRPC_CODE_INTERNAL, "Internal error",
          sourcemeta::core::parse_json(R"JSON({ "detail": "boom" })JSON")),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32603,
      "message": "Internal error",
      "data": { "detail": "boom" }
    }
  })JSON"));
}

// The 2025-03-26 and 2025-06-18 error schemas have no way to express an
// unreadable identifier, so a request response must carry a readable one
// https://modelcontextprotocol.io/specification/2025-06-18/schema#jsonrpcerror
TEST(make_error_rejects_unreadable_identifier_2025_03_26) {
  const sourcemeta::core::JSON identifier{true};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, &identifier,
          sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST, "Invalid Request"),
      "This MCP revision requires a readable error identifier");
}

TEST(make_error_rejects_unreadable_identifier_2025_06_18) {
  const sourcemeta::core::JSON identifier{true};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, &identifier,
          sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST, "Invalid Request"),
      "This MCP revision requires a readable error identifier");
}

TEST(make_error_rejects_absent_identifier_2025_03_26) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, nullptr,
          sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST, "Invalid Request"),
      "This MCP revision requires a readable error identifier");
}

TEST(make_error_rejects_absent_identifier_2025_06_18) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, nullptr,
          sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST, "Invalid Request"),
      "This MCP revision requires a readable error identifier");
}

TEST(make_error_transport_keeps_null_identifier_2025_03_26) {
  const sourcemeta::core::JSON identifier{true};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request", std::nullopt,
                sourcemeta::core::MCPErrorContext::Transport),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_transport_keeps_null_identifier_2025_06_18) {
  const sourcemeta::core::JSON identifier{true};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request", std::nullopt,
                sourcemeta::core::MCPErrorContext::Transport),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_transport_omits_absent_identifier_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, nullptr,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request", std::nullopt,
                sourcemeta::core::MCPErrorContext::Transport),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_transport_omits_absent_identifier_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, nullptr,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request", std::nullopt,
                sourcemeta::core::MCPErrorContext::Transport),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

// From 2025-11-25 onwards the identifier is optional on an error response, so
// an unreadable one is omitted rather than reported as null
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#error-responses
TEST(make_error_omits_unreadable_identifier_2025_11_25) {
  const sourcemeta::core::JSON identifier{true};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_omits_unreadable_identifier_2026_07_28) {
  const sourcemeta::core::JSON identifier{true};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_omits_absent_identifier_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, nullptr,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_omits_absent_identifier_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, nullptr,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

// A request identifier is a string or a number that is not fractional
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#requests
TEST(make_error_omits_fractional_identifier_2026_07_28) {
  const sourcemeta::core::JSON identifier{1.5};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_omits_object_identifier_2026_07_28) {
  const auto identifier{sourcemeta::core::parse_json(R"JSON({})JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_omits_array_identifier_2026_07_28) {
  const auto identifier{sourcemeta::core::parse_json(R"JSON([])JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_omits_null_identifier_2026_07_28) {
  const sourcemeta::core::JSON identifier{nullptr};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST,
                "Invalid Request"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

// 2026-07-28 retires -32002 in favour of -32602
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#error-codes
TEST(make_error_rejects_resource_not_found_on_2026_07_28) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND, "Resource not found"),
      "Retired MCP error code");
}

// -32042 was defined by 2025-11-25 only
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#error-codes
TEST(make_error_rejects_url_elicitation_required_on_2026_07_28) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_error(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         &identifier,
                         sourcemeta::core::MCP_CODE_URL_ELICITATION_REQUIRED,
                         "URL elicitation required"),
                     "Retired MCP error code");
}

TEST(make_error_accepts_resource_not_found_on_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier,
                sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
                "Resource not found"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32002, "message": "Resource not found" }
  })JSON"));
}

TEST(make_error_accepts_url_elicitation_required_on_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier,
                sourcemeta::core::MCP_CODE_URL_ELICITATION_REQUIRED,
                "URL elicitation required"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32042, "message": "URL elicitation required" }
  })JSON"));
}

// Codes from -32020 to -32099 belong to the specification, and an
// implementation must not emit one the specification does not define
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#error-codes
TEST(make_error_rejects_undefined_reserved_code_lower_bound) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_error(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         &identifier, -32099, "Undefined"),
                     "Undefined reserved MCP error code");
}

TEST(make_error_rejects_undefined_reserved_code_upper_bound) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_error(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         &identifier, -32023, "Undefined"),
                     "Undefined reserved MCP error code");
}

TEST(make_error_rejects_undefined_reserved_code_midrange) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_error(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         &identifier, -32050, "Undefined"),
                     "Undefined reserved MCP error code");
}

TEST(make_error_accepts_code_below_the_reserved_range) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                -32100, "Application"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32100, "message": "Application" }
  })JSON"));
}

// The -32000 to -32019 sub-range predates the allocation policy, so it stays
// usable even though new code should avoid it
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#error-codes
TEST(make_error_accepts_code_in_the_legacy_sub_range) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                -32019, "Legacy"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32019, "message": "Legacy" }
  })JSON"));
}

TEST(make_error_accepts_parse_error_code) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_PARSE, "Parse error"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32700, "message": "Parse error" }
  })JSON"));
}

TEST(make_error_accepts_method_not_found_code) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND,
                "Method not found"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32601, "message": "Method not found" }
  })JSON"));
}

TEST(make_error_accepts_invalid_params_code) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INVALID_PARAMS,
                "Invalid params"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

TEST(make_error_accepts_internal_error_code) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::JSONRPC_CODE_INTERNAL, "Internal error"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32603, "message": "Internal error" }
  })JSON"));
}

TEST(make_error_accepts_application_code) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                1, "Application"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": 1, "message": "Application" }
  })JSON"));
}

// The reserved sub-range policy was introduced by 2026-07-28, so earlier
// revisions keep whatever their implementations allocated
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#error-codes
TEST(make_error_accepts_undefined_reserved_code_on_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier,
                -32050, "Legacy"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32050, "message": "Legacy" }
  })JSON"));
}

TEST(make_error_accepts_undefined_reserved_code_on_2025_06_18) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, &identifier,
                -32050, "Legacy"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32050, "message": "Legacy" }
  })JSON"));
}

TEST(make_error_accepts_undefined_reserved_code_on_2025_03_26) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, &identifier,
                -32050, "Legacy"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32050, "message": "Legacy" }
  })JSON"));
}

// An unsupported-version error reports what was asked for and what the server
// supports
// https://modelcontextprotocol.io/specification/2026-07-28/schema#unsupportedprotocolversionerror
TEST(make_error_unsupported_protocol_version_requires_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version"),
      "Protocol version error requires data");
}

TEST(make_error_unsupported_protocol_version_rejects_null_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(R"JSON(null)JSON")),
      "Protocol version error requires data");
}

TEST(make_error_unsupported_protocol_version_rejects_boolean_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(R"JSON(false)JSON")),
      "Protocol version error requires data");
}

TEST(make_error_unsupported_protocol_version_rejects_array_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(R"JSON([])JSON")),
      "Protocol version error requires data");
}

TEST(make_error_unsupported_protocol_version_rejects_empty_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(R"JSON({})JSON")),
      "Invalid protocol version error data");
}

TEST(make_error_unsupported_protocol_version_rejects_non_string_requested) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(
              R"JSON({ "requested": false, "supported": [] })JSON")),
      "Invalid protocol version error data");
}

TEST(make_error_unsupported_protocol_version_rejects_non_array_supported) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(
              R"JSON({ "requested": "x", "supported": false })JSON")),
      "Invalid protocol version error data");
}

TEST(make_error_unsupported_protocol_version_rejects_non_string_supported) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(
              R"JSON({ "requested": "x", "supported": [ 1 ] })JSON")),
      "Supported protocol versions must be strings");
}

// The advertised list is what a client picks its next attempt from, so the
// chokepoint holds it to the same rule as the builders that advertise it
// https://modelcontextprotocol.io/specification/2026-07-28/basic/versioning
TEST(make_error_unsupported_protocol_version_rejects_an_unmodelled_supported) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(
              R"JSON({ "requested": "x", "supported": [ "1999-01-01" ] })JSON")),
      "Unknown server-supported MCP version");
}

TEST(make_error_unsupported_protocol_version_with_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
          "Unsupported protocol version",
          sourcemeta::core::parse_json(
              R"JSON({ "requested": "1999-01-01", "supported": [] })JSON")),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32022,
      "message": "Unsupported protocol version",
      "data": { "requested": "1999-01-01", "supported": [] }
    }
  })JSON"));
}

// A missing-capability error lists the capabilities the server needed
// https://modelcontextprotocol.io/specification/2026-07-28/schema#missingrequiredclientcapabilityerror
TEST(make_error_missing_capability_requires_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
          "Missing required client capability"),
      "Capability error requires data");
}

TEST(make_error_missing_capability_rejects_null_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
          "Missing required client capability",
          sourcemeta::core::parse_json(R"JSON(null)JSON")),
      "Capability error requires data");
}

TEST(make_error_missing_capability_rejects_array_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
          "Missing required client capability",
          sourcemeta::core::parse_json(R"JSON([])JSON")),
      "Capability error requires data");
}

TEST(make_error_missing_capability_rejects_data_without_capabilities) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
          "Missing required client capability",
          sourcemeta::core::parse_json(R"JSON({})JSON")),
      "Invalid required client capabilities");
}

TEST(make_error_missing_capability_rejects_non_object_capabilities) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
          "Missing required client capability",
          sourcemeta::core::parse_json(
              R"JSON({ "requiredCapabilities": false })JSON")),
      "Invalid required client capabilities");
}

TEST(make_error_missing_capability_rejects_malformed_capability) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
          "Missing required client capability",
          sourcemeta::core::parse_json(
              R"JSON({ "requiredCapabilities": { "roots": false } })JSON")),
      "Invalid required client capabilities");
}

TEST(make_error_missing_capability_rejects_malformed_extension_key) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
          "Missing required client capability",
          sourcemeta::core::parse_json(
              R"JSON({
                "requiredCapabilities": { "extensions": { "invalid key": {} } }
              })JSON")),
      "Invalid required client capabilities");
}

TEST(make_error_missing_capability_with_data) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
                "Missing required client capability",
                sourcemeta::core::parse_json(
                    R"JSON({
                      "requiredCapabilities": { "roots": {} }
                    })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32021,
      "message": "Missing required client capability",
      "data": { "requiredCapabilities": { "roots": {} } }
    }
  })JSON"));
}

TEST(make_error_header_mismatch_does_not_check_data_on_2026_07_28) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCP_CODE_HEADER_MISMATCH, "Header mismatch"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32020, "message": "Header mismatch" }
  })JSON"));
}

TEST(make_error_unsupported_protocol_version_builder) {
  const sourcemeta::core::JSON identifier{7};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> SUPPORTED{
      {"2025-11-25", "2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_unsupported_protocol_version(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                "1999-01-01", SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 7,
    "error": {
      "code": -32022,
      "message": "Unsupported protocol version",
      "data": {
        "supported": [ "2025-11-25", "2026-07-28" ],
        "requested": "1999-01-01"
      }
    }
  })JSON"));
}

TEST(make_error_unsupported_protocol_version_builder_without_identifier) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_unsupported_protocol_version(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, nullptr,
                "1999-01-01", SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32022,
      "message": "Unsupported protocol version",
      "data": {
        "supported": [ "2026-07-28" ],
        "requested": "1999-01-01"
      }
    }
  })JSON"));
}

TEST(make_error_unsupported_protocol_version_builder_rejects_empty_supported) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 0> SUPPORTED{};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error_unsupported_protocol_version(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          "1999-01-01", SUPPORTED),
      "Supported protocol versions must not be empty");
}

TEST(make_error_unsupported_protocol_version_builder_rejects_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2025-11-25"}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error_unsupported_protocol_version(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier,
          "1999-01-01", SUPPORTED),
      "This protocol error is defined for MCP 2026-07-28");
}

TEST(make_error_missing_required_capability_builder) {
  const sourcemeta::core::JSON identifier{"x"};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_missing_required_capability(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::parse_json(R"JSON({ "elicitation": {} })JSON"),
          "Elicitation is required"),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "x",
    "error": {
      "code": -32021,
      "message": "Elicitation is required",
      "data": { "requiredCapabilities": { "elicitation": {} } }
    }
  })JSON"));
}

TEST(make_error_missing_required_capability_builder_rejects_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error_missing_required_capability(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier,
          sourcemeta::core::parse_json(R"JSON({ "roots": {} })JSON"),
          "Roots are required"),
      "This protocol error is defined for MCP 2026-07-28");
}

TEST(make_error_missing_required_capability_builder_rejects_malformed) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error_missing_required_capability(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::parse_json(R"JSON({ "roots": false })JSON"),
          "Roots are required"),
      "Invalid required client capabilities");
}

TEST(make_error_header_mismatch_with_message) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_header_mismatch(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                "Duplicate routing header"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32020, "message": "Duplicate routing header" }
  })JSON"));
}

TEST(make_error_header_mismatch_with_message_without_identifier) {
  EXPECT_EQ(sourcemeta::core::mcp_make_error_header_mismatch(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, nullptr,
                "Duplicate routing header"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32020, "message": "Duplicate routing header" }
  })JSON"));
}

TEST(make_error_header_mismatch_with_message_rejects_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_error_header_mismatch(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         &identifier, "Duplicate routing header"),
                     "This protocol error is defined for MCP 2026-07-28");
}

TEST(make_error_header_mismatch_with_header_and_body_value) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_header_mismatch(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                "Mcp-Name", "foo", "bar"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "Mcp-Name",
        "headerValue": "foo",
        "bodyValue": "bar"
      }
    }
  })JSON"));
}

TEST(make_error_header_mismatch_with_header_and_body_value_rejects_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_error_header_mismatch(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         &identifier, "Mcp-Name", "foo", "bar"),
                     "This protocol error is defined for MCP 2026-07-28");
}

TEST(make_error_header_mismatch_with_unexpected_header) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_header_mismatch(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                "Mcp-Name", "foo"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32020,
      "message": "Header mismatch: unexpected header provided",
      "data": { "header": "Mcp-Name", "headerValue": "foo" }
    }
  })JSON"));
}

TEST(make_error_header_mismatch_with_unexpected_header_rejects_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_error_header_mismatch(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         &identifier, "Mcp-Name", "foo"),
                     "This protocol error is defined for MCP 2026-07-28");
}

// Resource-not-found moved from its own code to invalid params in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/server/resources#error-handling
TEST(make_error_resource_not_found_2026_07_28) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_resource_not_found(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Resource not found" }
  })JSON"));
}

TEST(make_error_resource_not_found_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_resource_not_found(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32002, "message": "Resource not found" }
  })JSON"));
}

TEST(make_error_resource_not_found_2025_06_18) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_resource_not_found(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, &identifier),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32002, "message": "Resource not found" }
  })JSON"));
}

TEST(make_error_resource_not_found_2025_03_26) {
  const sourcemeta::core::JSON identifier{1};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_resource_not_found(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, &identifier),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32002, "message": "Resource not found" }
  })JSON"));
}

TEST(make_error_resource_not_found_rejects_absent_identifier_2025_03_26) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error_resource_not_found(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, nullptr),
      "This MCP revision requires a readable error identifier");
}

TEST(make_error_resource_not_found_omits_absent_identifier_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_error_resource_not_found(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, nullptr),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32602, "message": "Resource not found" }
  })JSON"));
}

// A method the server does not implement is a 404 in 2026-07-28, which the
// JSON-RPC error body distinguishes from a 404 served by a legacy endpoint
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#protocol-version-header
TEST(error_code_to_http_status_method_not_found_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND),
            sourcemeta::core::HTTP_STATUS_NOT_FOUND);
}

// The three earlier revisions tie a status to a transport condition, such as a
// terminated session, rather than to a JSON-RPC error, which in that era
// travels on a 200, so there is no status to report for any code
// https://modelcontextprotocol.io/specification/2025-11-25/basic/transports
TEST(error_code_to_http_status_method_not_found_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND)
                   .has_value());
}

TEST(error_code_to_http_status_method_not_found_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                   sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND)
                   .has_value());
}

TEST(error_code_to_http_status_method_not_found_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                   sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND)
                   .has_value());
}

// 2026-07-28 names a status for five codes and no others, so a parse failure,
// a malformed envelope and an internal fault leave the choice to the adapter
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#server-validation
TEST(error_code_to_http_status_parse_error) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSONRPC_CODE_PARSE)
                   .has_value());
}

TEST(error_code_to_http_status_invalid_request) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST)
                   .has_value());
}

// A request that omits a required metadata field is answered with invalid
// parameters and a 400, which is the one mandate that attaches to this code
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(error_code_to_http_status_invalid_params) {
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSONRPC_CODE_INVALID_PARAMS),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
}

TEST(error_code_to_http_status_invalid_params_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   sourcemeta::core::JSONRPC_CODE_INVALID_PARAMS)
                   .has_value());
}

TEST(error_code_to_http_status_internal_error) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSONRPC_CODE_INTERNAL)
                   .has_value());
}

// A header mismatch, a missing capability and an unsupported version are all
// rejections of the request as sent
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#server-validation
TEST(error_code_to_http_status_header_mismatch) {
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::MCP_CODE_HEADER_MISMATCH),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
}

TEST(error_code_to_http_status_missing_required_client_capability) {
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
}

TEST(error_code_to_http_status_unsupported_protocol_version) {
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
}

// A resource that cannot be found is the client asking for something absent,
// so the earlier mapping reporting the server at fault was the one row that
// was wrong whichever way the convention went
TEST(error_code_to_http_status_resource_not_found) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND)
                   .has_value());
}

TEST(error_code_to_http_status_unmapped_code) {
  EXPECT_FALSE(sourcemeta::core::mcp_error_code_to_http_status(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28, -99999)
                   .has_value());
}

TEST(mcp_error_is_catchable_as_an_invalid_argument) {
  try {
    sourcemeta::core::mcp_make_resource_link(
        sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "a");
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(),
                 "Resource links require MCP 2025-06-18 or later");
  }
}

TEST(mcp_error_reports_a_contract_violation_before_the_json_module) {
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .priority = std::numeric_limits<double>::infinity()};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_serialize_resource_annotations(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, annotations),
      "Annotation priority must be a finite number");
}

TEST(a_json_non_finite_value_is_not_an_mcp_error) {
  try {
    const sourcemeta::core::JSON value{std::numeric_limits<double>::infinity()};
    EXPECT_TRUE(value.is_real());
    FAIL();
  } catch (const sourcemeta::core::MCPError &) {
    FAIL();
  } catch (const std::invalid_argument &error) {
    EXPECT_STREQ(error.what(), "JSON does not support Infinity or NaN");
  }
}
