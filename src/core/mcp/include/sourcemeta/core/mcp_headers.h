#ifndef SOURCEMETA_CORE_MCP_HEADERS_H_
#define SOURCEMETA_CORE_MCP_HEADERS_H_

#ifndef SOURCEMETA_CORE_MCP_EXPORT
#include <sourcemeta/core/mcp_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/mcp_capabilities.h>
#include <sourcemeta/core/mcp_error.h>
#include <sourcemeta/core/mcp_protocol.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace sourcemeta::core {

/// @ingroup mcp
/// Header name for the protocol version in Streamable HTTP transport.
constexpr JSON::StringView MCP_HEADER_PROTOCOL_VERSION{"MCP-Protocol-Version"};

/// @ingroup mcp
/// Header name for the MCP method in Streamable HTTP transport.
constexpr JSON::StringView MCP_HEADER_METHOD{"Mcp-Method"};

/// @ingroup mcp
/// Header name for the target name/URI in Streamable HTTP transport.
constexpr JSON::StringView MCP_HEADER_NAME{"Mcp-Name"};

/// @ingroup mcp
/// Validation status returned by @ref mcp_validate_request_meta.
enum class MCPRequestMetaStatus : std::uint8_t {
  /// The request metadata is fully valid.
  Valid,
  /// Request envelope lacks a `params` property.
  MissingParams,
  /// The `params` property is not a JSON object.
  ParamsNotObject,
  /// The `params` object lacks a `_meta` property.
  MissingMeta,
  /// The `_meta` property is not a JSON object.
  MetaNotObject,
  /// `_meta` lacks `io.modelcontextprotocol/protocolVersion`.
  MissingProtocolVersion,
  /// `io.modelcontextprotocol/protocolVersion` is not a string.
  ProtocolVersionNotString,
  /// `io.modelcontextprotocol/protocolVersion` is not a recognized version.
  UnsupportedProtocolVersion,
  /// `_meta` lacks `io.modelcontextprotocol/clientCapabilities`.
  MissingClientCapabilities,
  /// `io.modelcontextprotocol/clientCapabilities` is not a JSON object.
  ClientCapabilitiesNotObject,
  /// `io.modelcontextprotocol/clientInfo` is not a JSON object.
  ClientInfoNotObject,
  /// `io.modelcontextprotocol/clientInfo` lacks required `name` property.
  MissingClientInfoName,
  /// `name` property in `io.modelcontextprotocol/clientInfo` is not a string.
  ClientInfoNameNotString,
  /// `io.modelcontextprotocol/clientInfo` lacks required `version` property.
  MissingClientInfoVersion,
  /// `version` property in `io.modelcontextprotocol/clientInfo` is not a
  /// string.
  ClientInfoVersionNotString,
  /// `title` property in `io.modelcontextprotocol/clientInfo` is not a string.
  ClientInfoTitleNotString,
  /// `description` property in `io.modelcontextprotocol/clientInfo` is not a
  /// string.
  ClientInfoDescriptionNotString,
};

/// @ingroup mcp
/// Parsed representation of modern request metadata.
struct MCPRequestMeta {
  /// The declared protocol version.
  MCPProtocolVersion protocol_version = MCPProtocolVersion::V_2026_07_28;
  /// Non-null pointer to client capabilities object within the request.
  const sourcemeta::core::JSON *client_capabilities = nullptr;
  /// Optional parsed client capabilities.
  std::optional<MCPClientCapabilities> parsed_client_capabilities =
      std::nullopt;
  /// Client implementation info (optional per the MCP specification).
  std::optional<MCPClientInfo> client_info = std::nullopt;
  /// Optional requested log level.
  std::optional<JSON::StringView> log_level = std::nullopt;
  /// Non-null pointer to the entire `_meta` object within the request.
  const sourcemeta::core::JSON *meta_object = nullptr;
};

/// @ingroup mcp
/// Validate and extract modern MCP request metadata from a request envelope or
/// params object.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_request_meta(
    const sourcemeta::core::JSON &envelope_or_parameters)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>>;

/// @ingroup mcp
/// Construct an appropriate JSON-RPC error response from an invalid
/// @ref MCPRequestMetaStatus.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_request_meta(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const MCPRequestMetaStatus status, const JSON::StringView requested = "",
    const std::vector<JSON::StringView> &supported = {})
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Read the declared protocol version from a modern MCP request envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_protocol_version(const sourcemeta::core::JSON &envelope)
    -> std::optional<MCPProtocolVersion>;

/// @ingroup mcp
/// Read client implementation information from a modern MCP request envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_client_info(const sourcemeta::core::JSON &envelope)
    -> std::optional<MCPClientInfo>;

/// @ingroup mcp
/// Read client capabilities from a modern MCP request envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_client_capabilities(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

/// @ingroup mcp
/// Read the requested log level from a modern MCP request envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_log_level(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Check whether a modern MCP request contains all required metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_has_required_request_meta(const sourcemeta::core::JSON &envelope)
    -> bool;

/// @ingroup mcp
/// Extract the request method name from a JSON-RPC request body.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_method_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Extract the target tool name or resource URI from a JSON-RPC request body.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_name_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<std::string>;

/// @ingroup mcp
/// Validate Streamable HTTP transport header values against the parsed
/// JSON-RPC request body for the specified protocol version.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_request_headers(
    const MCPProtocolVersion version,
    const std::optional<JSON::StringView> &protocol_version_header,
    const std::optional<JSON::StringView> &method_header,
    const std::optional<JSON::StringView> &name_header,
    const sourcemeta::core::JSON &envelope)
    -> std::optional<sourcemeta::core::JSON>;

} // namespace sourcemeta::core

#endif
