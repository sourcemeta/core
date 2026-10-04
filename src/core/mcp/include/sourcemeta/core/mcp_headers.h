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
#include <span>
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
  /// The input is not a valid MCP request envelope.
  InvalidEnvelope,
  /// Metadata contains an invalid key name or malformed tracing value.
  InvalidMetaKey,
  /// Known client capability fields have invalid shapes.
  InvalidClientCapabilities,
  /// Client websiteUrl must be a string.
  ClientInfoWebsiteUrlNotString,
  /// Implementation icons have invalid shapes.
  InvalidClientInfo,
  /// logLevel must be one of the eight specified logging levels.
  InvalidLogLevel,
  /// progressToken must be a string or integer.
  InvalidProgressToken,
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
/// Parsed representation of 2026-07-28 request metadata.
struct MCPRequestMeta {
  /// The declared protocol revision.
  MCPProtocolVersion protocol_version;
  /// Borrowed capabilities; the source request must outlive this view.
  const JSON &client_capabilities;
  /// Optional borrowed implementation description.
  std::optional<MCPImplementation> client_info;
  /// Optional requested log level.
  std::optional<JSON::StringView> log_level;
  /// Borrowed metadata; never null after successful validation.
  const JSON &meta_object;
  /// Optional borrowed progress token.
  const JSON *progress_token = nullptr;
};

/// @ingroup mcp
/// Validate and extract MCP 2026-07-28 request metadata from a request
/// envelope. Use mcp_validate_request_parameters for parameters.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_request_meta(const sourcemeta::core::JSON &envelope)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>>;

/// @ingroup mcp
/// Validate metadata in a parameters object, without guessing envelope shape.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_request_parameters(const JSON &parameters)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>>;

/// @ingroup mcp
/// Construct an appropriate JSON-RPC error response from an invalid
/// @ref MCPRequestMetaStatus.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_request_meta(
    const MCPProtocolVersion version,
    const std::optional<sourcemeta::core::JSON> &identifier,
    const MCPRequestMetaStatus status, const JSON::StringView requested,
    const std::span<const JSON::StringView> supported)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Extract the request method name from a JSON-RPC request body.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_method_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Borrow the target tool name or resource URI from a JSON-RPC request body.
/// The returned view is valid only while the source string remains alive and
/// unchanged.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_name_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Validate Streamable HTTP transport header values against the parsed
/// JSON-RPC request body for the specified protocol version.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_request_headers(
    const MCPProtocolVersion version,
    const std::optional<JSON::StringView> &protocol_version_header,
    const std::optional<JSON::StringView> &method_header,
    const std::optional<JSON::StringView> &name_header,
    const sourcemeta::core::JSON &envelope,
    const std::span<const JSON::StringView> supported_versions)
    -> std::optional<sourcemeta::core::JSON>;

/// @ingroup mcp
/// Encode a Mcp-Name or Mcp-Param value using the MCP 2026-07-28 sentinel
/// encoding when the value cannot be represented literally.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_encode_header_value(JSON::StringView value) -> std::string;

/// @ingroup mcp
/// Validate a borrowed HTTP header collection. Names are case insensitive;
/// duplicate recognized singleton fields are rejected. Unknown fields stay
/// opaque. HTTP framing, Origin and session validation belong to the adapter.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_request_headers(
    MCPProtocolVersion version,
    std::span<const std::pair<JSON::StringView, JSON::StringView>> headers,
    const JSON &envelope, std::span<const JSON::StringView> supported_versions)
    -> std::optional<JSON>;

/// @ingroup mcp
/// A statically reachable x-mcp-header annotation. String views borrow the
/// input schema; keep that schema alive while using the descriptors.
struct MCPHeaderParameter {
  /// The suffix of Mcp-Param-{name}.
  JSON::StringView name;
  /// Instance property path, with no array or reference traversal.
  std::vector<JSON::StringView> path;
  /// One of String, Integer or Boolean.
  JSON::Type type;
};

/// @ingroup mcp
/// Extract checked 2026-07-28 HTTP parameter annotations, or nullopt if the
/// schema has an invalid annotation, type, location or duplicate field name.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_header_parameters(MCPProtocolVersion version, const JSON &input_schema)
    -> std::optional<std::vector<MCPHeaderParameter>>;

/// @ingroup mcp
/// Produce mirrored parameter headers. Missing/null values are omitted.
/// Throws invalid_argument on a type mismatch or an unsafe integer.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_parameter_headers(std::span<const MCPHeaderParameter> parameters,
                                const JSON &arguments)
    -> std::vector<std::pair<std::string, std::string>>;

/// @ingroup mcp
/// Compare annotated parameters with raw HTTP headers case-insensitively.
/// Validates required mirrors, encoding, duplicates and numeric equivalence.
/// Protocol version must be 2026-07-28. Schema extraction should precede this.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_parameter_headers(
    MCPProtocolVersion version, const JSON &identifier,
    std::span<const MCPHeaderParameter> parameters, const JSON &arguments,
    std::span<const std::pair<JSON::StringView, JSON::StringView>> headers)
    -> std::optional<JSON>;

} // namespace sourcemeta::core

#endif
