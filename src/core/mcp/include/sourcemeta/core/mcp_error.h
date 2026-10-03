#ifndef SOURCEMETA_CORE_MCP_ERROR_H_
#define SOURCEMETA_CORE_MCP_ERROR_H_

#ifndef SOURCEMETA_CORE_MCP_EXPORT
#include <sourcemeta/core/mcp_export.h>
#endif

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp_protocol.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace sourcemeta::core {

/// @ingroup mcp
/// The legacy MCP error code returned when a requested resource cannot be
/// found.
constexpr std::int64_t MCP_CODE_RESOURCE_NOT_FOUND{-32002};

/// @ingroup mcp
/// The MCP error code indicating that the client must complete a URL
/// elicitation flow before retrying.
constexpr std::int64_t MCP_CODE_URL_ELICITATION_REQUIRED{-32042};

/// @ingroup mcp
/// The MCP error code indicating that a transport header disagreed with the
/// request body.
constexpr std::int64_t MCP_CODE_HEADER_MISMATCH{-32020};

/// @ingroup mcp
/// The MCP error code indicating that a required capability was not
/// declared in request metadata.
constexpr std::int64_t MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY{-32021};

/// @ingroup mcp
/// The MCP error code indicating that the requested protocol version is not
/// supported by the receiver.
constexpr std::int64_t MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION{-32022};

/// @ingroup mcp
/// Map an MCP or JSON-RPC error code to an appropriate HTTP status code
/// for the Streamable HTTP transport.
///
/// For MCP 2026-07-28, returns @ref HTTP_STATUS_NOT_FOUND for method not found
/// (-32601). For earlier protocol versions, returns @ref
/// HTTP_STATUS_BAD_REQUEST for method not found. Returns @ref
/// HTTP_STATUS_BAD_REQUEST for client errors
/// (-32600, -32602, -32700, -32020, -32021, -32022), and @ref
/// HTTP_STATUS_INTERNAL_SERVER_ERROR for internal or unmapped server errors.
constexpr auto mcp_error_code_to_http_status(const MCPProtocolVersion version,
                                             const std::int64_t code) noexcept
    -> HTTPStatus {
  if (version == MCPProtocolVersion::V_2026_07_28) {
    if (code == JSONRPC_CODE_METHOD_NOT_FOUND) {
      return HTTP_STATUS_NOT_FOUND;
    }
  } else {
    if (code == JSONRPC_CODE_METHOD_NOT_FOUND) {
      return HTTP_STATUS_BAD_REQUEST;
    }
  }
  if (code == JSONRPC_CODE_INVALID_REQUEST ||
      code == JSONRPC_CODE_INVALID_PARAMS || code == JSONRPC_CODE_PARSE ||
      code == MCP_CODE_HEADER_MISMATCH ||
      code == MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY ||
      code == MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION) {
    return HTTP_STATUS_BAD_REQUEST;
  }
  return HTTP_STATUS_INTERNAL_SERVER_ERROR;
}

/// @ingroup mcp
/// Map an error code to HTTP status code defaulting to modern 2026-07-28
/// version.
constexpr auto mcp_error_code_to_http_status(const std::int64_t code) noexcept
    -> HTTPStatus {
  return mcp_error_code_to_http_status(MCPProtocolVersion::V_2026_07_28, code);
}

/// @ingroup mcp
/// Build a JSON-RPC error response for unsupported protocol version (-32022).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_unsupported_protocol_version(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const JSON::StringView requested,
    const std::vector<JSON::StringView> &supported) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for missing required client capability
/// (-32021).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_missing_required_capability(
    const std::optional<sourcemeta::core::JSON> &identifier,
    sourcemeta::core::JSON required_capabilities,
    const JSON::StringView message) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_header_mismatch(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const JSON::StringView message) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020) with detailed
/// header mismatch payload.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_header_mismatch(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const JSON::StringView header_name, const JSON::StringView header_value,
    const JSON::StringView body_value) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020) reporting an
/// unexpected header supplied without a corresponding body value.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_header_mismatch(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const JSON::StringView header_name, const JSON::StringView header_value)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a version-aware JSON-RPC error response reporting that a resource
/// was not found. Emits -32002 for legacy revisions, and -32602 (Invalid
/// Params) for 2026-07-28.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_resource_not_found(
    const MCPProtocolVersion version,
    const std::optional<sourcemeta::core::JSON> &identifier)
    -> sourcemeta::core::JSON;

} // namespace sourcemeta::core

#endif
