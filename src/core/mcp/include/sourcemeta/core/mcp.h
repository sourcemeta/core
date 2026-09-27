#ifndef SOURCEMETA_CORE_MCP_H_
#define SOURCEMETA_CORE_MCP_H_

#ifndef SOURCEMETA_CORE_MCP_EXPORT
#include <sourcemeta/core/mcp_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::int64_t, std::uint8_t, std::uint64_t
#include <optional> // std::optional, std::nullopt
#include <utility>  // std::pair, std::to_underlying, std::unreachable
#include <vector>   // std::vector

/// @defgroup mcp MCP
/// @brief Helpers for building Model Context Protocol (MCP) envelopes.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// ```

namespace sourcemeta::core {

/// @ingroup mcp
/// The supported MCP protocol revisions.
enum class MCPProtocolVersion : std::uint8_t {
  /// The MCP 2025-03-26 protocol revision.
  V_2025_03_26,
  /// The MCP 2025-06-18 protocol revision.
  V_2025_06_18,
  /// The MCP 2025-11-25 protocol revision.
  V_2025_11_25,
  /// The MCP 2026-07-28 protocol revision.
  V_2026_07_28,
};

/// @ingroup mcp
/// Get the canonical wire-format string for an MCP protocol version. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_protocol_version_string(
///            sourcemeta::core::MCPProtocolVersion::V_2025_11_25) ==
///        "2025-11-25");
/// ```
constexpr auto
mcp_protocol_version_string(const MCPProtocolVersion version) noexcept
    -> JSON::StringView {
  switch (version) {
    case MCPProtocolVersion::V_2025_03_26:
      return "2025-03-26";
    case MCPProtocolVersion::V_2025_06_18:
      return "2025-06-18";
    case MCPProtocolVersion::V_2025_11_25:
      return "2025-11-25";
    case MCPProtocolVersion::V_2026_07_28:
      return "2026-07-28";
  }
  std::unreachable();
}

/// @ingroup mcp
/// Check whether an MCP protocol revision is at least the given minimum
/// version.
///
/// Protocol revisions in @ref MCPProtocolVersion are declared chronologically,
/// so future versions must be appended.
///
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_protocol_version_at_least(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// ```
constexpr auto
mcp_protocol_version_at_least(const MCPProtocolVersion current,
                              const MCPProtocolVersion minimum) noexcept
    -> bool {
  return std::to_underlying(current) >= std::to_underlying(minimum);
}

/// @ingroup mcp
/// The MCP method name for the `initialize` request.
constexpr JSON::StringView MCP_METHOD_INITIALIZE{"initialize"};

/// @ingroup mcp
/// The MCP method name for the `ping` request.
constexpr JSON::StringView MCP_METHOD_PING{"ping"};

/// @ingroup mcp
/// The MCP method name for the `tools/list` request.
constexpr JSON::StringView MCP_METHOD_TOOLS_LIST{"tools/list"};

/// @ingroup mcp
/// The MCP method name for the `tools/call` request.
constexpr JSON::StringView MCP_METHOD_TOOLS_CALL{"tools/call"};

/// @ingroup mcp
/// The MCP method name for the `resources/list` request.
constexpr JSON::StringView MCP_METHOD_RESOURCES_LIST{"resources/list"};

/// @ingroup mcp
/// The MCP method name for the `resources/read` request.
constexpr JSON::StringView MCP_METHOD_RESOURCES_READ{"resources/read"};

/// @ingroup mcp
/// The MCP method name for the `resources/templates/list` request.
constexpr JSON::StringView MCP_METHOD_RESOURCES_TEMPLATES_LIST{
    "resources/templates/list"};

/// @ingroup mcp
/// The MCP method name for the `notifications/initialized` notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_INITIALIZED{
    "notifications/initialized"};

/// @ingroup mcp
/// The MCP method name for the `server/discover` request.
constexpr JSON::StringView MCP_METHOD_SERVER_DISCOVER{"server/discover"};

/// @ingroup mcp
/// The MCP method name for the `subscriptions/listen` request.
constexpr JSON::StringView MCP_METHOD_SUBSCRIPTIONS_LISTEN{
    "subscriptions/listen"};

/// @ingroup mcp
/// The MCP method name for the `notifications/subscriptions/acknowledged`
/// notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED{
    "notifications/subscriptions/acknowledged"};

/// @ingroup mcp
/// The MCP method name for the `prompts/get` request.
constexpr JSON::StringView MCP_METHOD_PROMPTS_GET{"prompts/get"};

/// @ingroup mcp
/// Check whether the given method name corresponds to an operation that
/// requires a name or URI identifier (such as `tools/call`, `resources/read`,
/// or `prompts/get`).
constexpr auto
mcp_is_named_request_method(const JSON::StringView method) noexcept -> bool {
  return method == MCP_METHOD_TOOLS_CALL ||
         method == MCP_METHOD_RESOURCES_READ ||
         method == MCP_METHOD_PROMPTS_GET;
}

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
/// The MCP error code indicating that a client is missing a capability required
/// to process a request.
constexpr std::int64_t MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY{-32021};

/// @ingroup mcp
/// The MCP error code indicating that the client's requested protocol version
/// is not supported.
constexpr std::int64_t MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION{-32022};

/// @ingroup mcp
/// The key used for request and result metadata objects.
constexpr JSON::StringView MCP_META_KEY{"_meta"};

/// @ingroup mcp
/// The metadata key for the client's declared MCP protocol version.
constexpr JSON::StringView MCP_META_PROTOCOL_VERSION{
    "io.modelcontextprotocol/protocolVersion"};

/// @ingroup mcp
/// The metadata key for client capabilities in modern MCP requests.
constexpr JSON::StringView MCP_META_CLIENT_CAPABILITIES{
    "io.modelcontextprotocol/clientCapabilities"};

/// @ingroup mcp
/// The metadata key for client implementation info in modern MCP requests.
constexpr JSON::StringView MCP_META_CLIENT_INFO{
    "io.modelcontextprotocol/clientInfo"};

/// @ingroup mcp
/// The metadata key for the client's requested log level in modern MCP
/// requests.
constexpr JSON::StringView MCP_META_LOG_LEVEL{
    "io.modelcontextprotocol/logLevel"};

/// @ingroup mcp
/// The metadata key for server implementation info in modern MCP results.
constexpr JSON::StringView MCP_META_SERVER_INFO{
    "io.modelcontextprotocol/serverInfo"};

/// @ingroup mcp
/// The metadata key for subscription identifiers in modern subscription results
/// and notifications.
constexpr JSON::StringView MCP_META_SUBSCRIPTION_ID{
    "io.modelcontextprotocol/subscriptionId"};

/// @ingroup mcp
/// The HTTP header name declaring the client's MCP protocol version.
constexpr JSON::StringView MCP_HEADER_PROTOCOL_VERSION{"MCP-Protocol-Version"};

/// @ingroup mcp
/// The HTTP header name declaring the MCP request method.
constexpr JSON::StringView MCP_HEADER_METHOD{"Mcp-Method"};

/// @ingroup mcp
/// The HTTP header name declaring the target tool or resource name.
constexpr JSON::StringView MCP_HEADER_NAME{"Mcp-Name"};

/// @ingroup mcp
/// Check whether the given method name corresponds to a valid MCP request
/// method for the specified protocol version.
constexpr auto mcp_is_request_method(const MCPProtocolVersion version,
                                     const JSON::StringView method) noexcept
    -> bool {
  if (version == MCPProtocolVersion::V_2026_07_28) {
    return method == MCP_METHOD_SERVER_DISCOVER ||
           method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
           method == MCP_METHOD_RESOURCES_LIST ||
           method == MCP_METHOD_RESOURCES_READ ||
           method == MCP_METHOD_RESOURCES_TEMPLATES_LIST ||
           method == MCP_METHOD_SUBSCRIPTIONS_LISTEN;
  }

  return method == MCP_METHOD_INITIALIZE || method == MCP_METHOD_PING ||
         method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
         method == MCP_METHOD_RESOURCES_LIST ||
         method == MCP_METHOD_RESOURCES_READ ||
         method == MCP_METHOD_RESOURCES_TEMPLATES_LIST;
}

/// @ingroup mcp
/// Check whether the given method name corresponds to an MCP request method
/// (notifications excluded) for legacy revisions.
constexpr auto mcp_is_request_method(const JSON::StringView method) noexcept
    -> bool {
  return mcp_is_request_method(MCPProtocolVersion::V_2025_11_25, method);
}

/// @ingroup mcp
/// Classification of MCP methods by protocol era.
enum class MCPMethodEra : std::uint8_t {
  /// The method exists only in legacy MCP revisions (e.g. `initialize`, `ping`,
  /// `notifications/initialized`).
  LegacyOnly,
  /// The method exists only in modern MCP revisions (e.g. `server/discover`,
  /// `subscriptions/listen`, `notifications/subscriptions/acknowledged`).
  ModernOnly,
  /// The method is supported across both legacy and modern revisions
  /// (e.g. `tools/list`, `tools/call`, `resources/list`, etc.).
  Shared,
  /// The method is not recognized or supported by MCP.
  Unsupported,
};

/// @ingroup mcp
/// Classify an MCP method or notification into its protocol era.
constexpr auto mcp_classify_method(const JSON::StringView method) noexcept
    -> MCPMethodEra {
  if (method == MCP_METHOD_INITIALIZE || method == MCP_METHOD_PING ||
      method == MCP_METHOD_NOTIFICATIONS_INITIALIZED) {
    return MCPMethodEra::LegacyOnly;
  }
  if (method == MCP_METHOD_SERVER_DISCOVER ||
      method == MCP_METHOD_SUBSCRIPTIONS_LISTEN ||
      method == MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED) {
    return MCPMethodEra::ModernOnly;
  }
  if (method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
      method == MCP_METHOD_RESOURCES_LIST ||
      method == MCP_METHOD_RESOURCES_READ ||
      method == MCP_METHOD_RESOURCES_TEMPLATES_LIST) {
    return MCPMethodEra::Shared;
  }
  return MCPMethodEra::Unsupported;
}

/// @ingroup mcp
/// Check whether the given method name is supported in the specified protocol
/// version.
constexpr auto mcp_supports_method(const MCPProtocolVersion version,
                                   const JSON::StringView method) noexcept
    -> bool {
  const auto era{mcp_classify_method(method)};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    return era == MCPMethodEra::ModernOnly || era == MCPMethodEra::Shared;
  }
  return era == MCPMethodEra::LegacyOnly || era == MCPMethodEra::Shared;
}

/// @ingroup mcp
/// Check whether the given method name is legacy-only.
constexpr auto mcp_is_legacy_only_method(const JSON::StringView method) noexcept
    -> bool {
  return mcp_classify_method(method) == MCPMethodEra::LegacyOnly;
}

/// @ingroup mcp
/// Check whether the given method name is modern-only.
constexpr auto mcp_is_modern_only_method(const JSON::StringView method) noexcept
    -> bool {
  return mcp_classify_method(method) == MCPMethodEra::ModernOnly;
}

/// @ingroup mcp
/// Check whether the given method name is supported across both legacy and
/// modern revisions.
constexpr auto mcp_is_shared_method(const JSON::StringView method) noexcept
    -> bool {
  return mcp_classify_method(method) == MCPMethodEra::Shared;
}

/// @ingroup mcp
/// Resolve an `MCP-Protocol-Version` header value into a known protocol
/// version, or `std::nullopt` when the value is unrecognised. An absent header
/// resolves to the oldest supported version per the Streamable HTTP transport.
constexpr auto
mcp_resolve_protocol_version(const JSON::StringView header) noexcept
    -> std::optional<MCPProtocolVersion> {
  if (header.empty()) {
    return MCPProtocolVersion::V_2025_03_26;
  }
  if (header == "2026-07-28") {
    return MCPProtocolVersion::V_2026_07_28;
  }
  if (header == "2025-11-25") {
    return MCPProtocolVersion::V_2025_11_25;
  }
  if (header == "2025-06-18") {
    return MCPProtocolVersion::V_2025_06_18;
  }
  if (header == "2025-03-26") {
    return MCPProtocolVersion::V_2025_03_26;
  }
  return std::nullopt;
}

/// @ingroup mcp
/// Whether the given protocol version uses the `initialize` /
/// `notifications/initialized` handshake.
constexpr auto
mcp_uses_initialization_handshake(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version supports the `ping` request.
constexpr auto mcp_supports_ping(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version uses protocol-level sessions.
constexpr auto
mcp_supports_protocol_sessions(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version requires per-request stateless metadata
/// (`_meta`).
constexpr auto
mcp_requires_request_meta(const MCPProtocolVersion version) noexcept -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version requires the `resultType` field on
/// successful MCP results.
constexpr auto
mcp_requires_result_type(const MCPProtocolVersion version) noexcept -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version requires or supports cacheable-result
/// metadata (`ttlMs` and `cacheScope`).
constexpr auto
mcp_requires_cacheable_metadata(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version supports the `server/discover` request.
constexpr auto
mcp_supports_server_discover(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version supports the `subscriptions/listen`
/// request.
constexpr auto
mcp_supports_subscriptions_listen(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version supports Multi Round-Trip Results
/// (MRTR).
constexpr auto mcp_supports_mrtr(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// The latest protocol version that supports the `initialize` handshake.
constexpr auto mcp_latest_initialization_version() noexcept
    -> MCPProtocolVersion {
  return MCPProtocolVersion::V_2025_11_25;
}

/// @ingroup mcp
/// Whether the given protocol version supports per-tool `outputSchema`.
constexpr auto
mcp_supports_output_schema(const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the given protocol version supports `structuredContent` in tool
/// results.
constexpr auto
mcp_supports_structured_content(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the given protocol version supports `resource_link` content blocks.
constexpr auto
mcp_supports_resource_link_content(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the given protocol version supports the `title` field on the
/// implementation info object.
constexpr auto
mcp_supports_implementation_title(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the given protocol version supports the `description` field on the
/// implementation info object.
constexpr auto mcp_supports_implementation_description(
    const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_11_25);
}

/// @ingroup mcp
/// Whether the given protocol version supports the `websiteUrl` field on the
/// implementation info object.
constexpr auto mcp_supports_implementation_website_url(
    const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_11_25);
}

/// @ingroup mcp
/// Whether the given protocol version supports JSON-RPC 2.0 batching.
constexpr auto
mcp_supports_jsonrpc_batching(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2025_03_26;
}

/// @ingroup mcp
/// Cache scope for modern cacheable MCP results.
enum class MCPCacheScope : std::uint8_t {
  /// Public caching is permissible across multiple clients/users.
  Public,
  /// Result is private and must only be cached per-client/user.
  Private,
};

/// @ingroup mcp
/// Canonical string conversion for cache scope.
constexpr auto mcp_cache_scope_string(const MCPCacheScope scope) noexcept
    -> JSON::StringView {
  switch (scope) {
    case MCPCacheScope::Public:
      return "public";
    case MCPCacheScope::Private:
      return "private";
  }
  std::unreachable();
}

/// @ingroup mcp
/// Parse a canonical cache scope string.
constexpr auto mcp_resolve_cache_scope(const JSON::StringView str) noexcept
    -> std::optional<MCPCacheScope> {
  if (str == "public") {
    return MCPCacheScope::Public;
  }
  if (str == "private") {
    return MCPCacheScope::Private;
  }
  return std::nullopt;
}

/// @ingroup mcp
/// Cache policy specifying TTL in milliseconds and scope for cacheable MCP
/// results.
struct MCPCachePolicy {
  /// Time-to-live in milliseconds. Must be non-negative.
  std::uint64_t ttl_ms = 0;
  /// Cache scope (public or private).
  MCPCacheScope scope = MCPCacheScope::Public;
};

/// @ingroup mcp
/// Client implementation information attached to modern MCP request metadata.
struct MCPClientInfo {
  /// Machine-readable client name.
  JSON::StringView name;
  /// Semver-compatible client version.
  JSON::StringView version;
  /// Optional human-readable title.
  JSON::StringView title = {};
  /// Optional human-readable description.
  JSON::StringView description = {};
};

/// @ingroup mcp
/// Capabilities advertised by an MCP client.
struct MCPClientCapabilities {
  /// Whether the client advertises roots.
  bool roots = false;
  /// Whether roots advertise `listChanged`.
  bool roots_list_changed = false;
  /// Whether the client advertises sampling.
  bool sampling = false;
  /// Whether the client advertises elicitation.
  bool elicitation = false;
  /// Optional extensions map for client capabilities.
  std::optional<sourcemeta::core::JSON> extensions = std::nullopt;
  /// Optional experimental map for client capabilities.
  std::optional<sourcemeta::core::JSON> experimental = std::nullopt;
};

/// @ingroup mcp
/// Parse client capabilities from a JSON object.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_parse_client_capabilities(const sourcemeta::core::JSON &capabilities)
    -> MCPClientCapabilities;

/// @ingroup mcp
/// Serialize client capabilities to a JSON object.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_serialize_client_capabilities(
    const MCPClientCapabilities &capabilities) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Status returned by @ref mcp_validate_request_meta.
enum class MCPRequestMetaStatus : std::uint8_t {
  /// Request metadata is fully valid per MCP 2026-07-28.
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
  /// `_meta` lacks `io.modelcontextprotocol/clientInfo`.
  MissingClientInfo,
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
  /// Client implementation info.
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
auto mcp_validate_request_meta(const sourcemeta::core::JSON &envelope_or_params)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>>;

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
/// Read client capabilities object from a modern MCP request envelope.
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
/// Extract the request method declared in a JSON-RPC request body.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_method_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Extract the target tool, resource, or prompt name declared in a request body
/// (used to validate against the `Mcp-Name` header).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_name_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Build a JSON-RPC error response for unsupported protocol version (-32022).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_unsupported_protocol_version(
    const sourcemeta::core::JSON *identifier, const JSON::StringView requested,
    const std::vector<JSON::StringView> &supported) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for missing required client capability
/// (-32021).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_missing_required_capability(
    const sourcemeta::core::JSON *identifier,
    sourcemeta::core::JSON required_capabilities,
    const JSON::StringView message = "Missing required client capability")
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_header_mismatch(
    const sourcemeta::core::JSON *identifier,
    const JSON::StringView message = "Header mismatch")
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020) with detailed
/// header mismatch payload.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_header_mismatch(const sourcemeta::core::JSON *identifier,
                                    const JSON::StringView header_name,
                                    const JSON::StringView header_value,
                                    const JSON::StringView body_value)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020) reporting an
/// unexpected header supplied without a corresponding body value.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_header_mismatch(const sourcemeta::core::JSON *identifier,
                                    const JSON::StringView header_name,
                                    const JSON::StringView header_value)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Validate Streamable HTTP transport header values against the parsed
/// JSON-RPC request body for the specified protocol version.
///
/// This helper is intended exclusively for the Streamable HTTP transport.
/// Transports that do not use HTTP headers (such as stdio) must not call this
/// helper.
///
/// For MCP 2026-07-28, standard routing headers (`MCP-Protocol-Version`,
/// `Mcp-Method`, and optionally `Mcp-Name` for named methods) are required.
/// For legacy revisions, routing headers are optional but verified if
/// present.
///
/// Returns a JSON-RPC error response envelope if a required header is missing,
/// malformed, or mismatches the request body, or `std::nullopt` if validation
/// succeeds.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_request_headers(
    const MCPProtocolVersion version,
    const std::optional<JSON::StringView> &protocol_version_header,
    const std::optional<JSON::StringView> &method_header,
    const std::optional<JSON::StringView> &name_header,
    const sourcemeta::core::JSON &envelope)
    -> std::optional<sourcemeta::core::JSON>;

/// @ingroup mcp
/// Build a version-aware JSON-RPC error response reporting that a resource
/// was not found. Emits -32002 for legacy revisions, and -32602 (Invalid
/// Params) for 2026-07-28.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_resource_not_found(const MCPProtocolVersion version,
                                       const sourcemeta::core::JSON &identifier)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `text` content block carrying the given text payload.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_text_block(const JSON::StringView text) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP content block referencing a resource by URI.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_link(const MCPProtocolVersion version,
                            const JSON::StringView uri,
                            const JSON::StringView mime_type,
                            const JSON::StringView name = {},
                            const JSON::StringView description = {})
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Implementation info advertised by an MCP server.
struct MCPImplementation {
  /// Short machine-readable server name.
  JSON::StringView name;
  /// Semver-compatible server version.
  JSON::StringView version;
  /// Optional human-readable title.
  JSON::StringView title = {};
  /// Optional human-readable description.
  JSON::StringView description = {};
  /// Optional public website URL.
  JSON::StringView website_url = {};
};

/// @ingroup mcp
/// Decorate a successful MCP result object with version-appropriate envelope
/// fields (`resultType: "complete"` and optional
/// `_meta.io.modelcontextprotocol/serverInfo` for modern versions).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_decorate_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON result,
    const std::optional<MCPImplementation> &server_info = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Decorate an MCP result object with cache metadata (`ttlMs` and
/// `cacheScope`) for modern versions.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_decorate_cacheable_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON result,
    const std::optional<MCPCachePolicy> &cache_policy = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC envelope wrapping a successful MCP tool call response from
/// the given result payload.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &identifier,
                           sourcemeta::core::JSON result)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC envelope wrapping a successful MCP tool call response from
/// caller-provided content blocks and a structured payload.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &identifier,
                           sourcemeta::core::JSON structured,
                           sourcemeta::core::JSON content_blocks)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a version-aware JSON-RPC envelope wrapping a failed MCP tool call
/// response with the given error message. For 2026-07-28, adds
/// `resultType: "complete"`.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_error(const MCPProtocolVersion version,
                         const sourcemeta::core::JSON &identifier,
                         const JSON::StringView message)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC envelope wrapping a failed MCP tool call response with the
/// given error message (legacy compatibility overload).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_error(const sourcemeta::core::JSON &identifier,
                         const JSON::StringView message)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error envelope reporting that an MCP resource URI could
/// not be resolved (legacy compatibility overload emitting -32002).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_resource_not_found(const sourcemeta::core::JSON &identifier)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP resource descriptor as used in `resources/list` responses.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource(const JSON::StringView uri, const JSON::StringView name,
                       const JSON::StringView mime_type,
                       const JSON::StringView description = {},
                       const std::optional<std::size_t> size = std::nullopt,
                       const std::optional<double> priority = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/read` content entry of `text` flavour.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_text_content(const JSON::StringView uri,
                                    const JSON::StringView mime_type,
                                    const JSON::StringView text)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Wrap a pre-built array of content entries into the MCP `resources/read`
/// result envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_read_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON contents,
    const std::optional<MCPCachePolicy> &cache_policy = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Wrap a pre-built array of content entries into the MCP `resources/read`
/// result envelope (legacy compatibility overload).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_read_result(sourcemeta::core::JSON contents)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a single entry for an MCP `resources/templates/list` response.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_template(const JSON::StringView uri_template,
                                const JSON::StringView name,
                                const JSON::StringView description,
                                const JSON::StringView mime_type)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Optional hints attached to an MCP tool descriptor.
struct MCPToolAnnotations {
  /// Optional human-readable title for the tool.
  JSON::StringView title = {};
  /// `true` when the tool guarantees no side effects.
  bool read_only = false;
  /// `true` when the tool may mutate or delete state.
  bool destructive = true;
  /// `true` when repeated invocations with the same input yield the same
  /// result.
  bool idempotent = false;
  /// `true` when the tool interacts with state outside the server's control.
  bool open_world = true;
};

/// @ingroup mcp
/// Build a single entry for an MCP `tools/list` response.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_descriptor(
    const MCPProtocolVersion version, const JSON::StringView name,
    const JSON::StringView description, sourcemeta::core::JSON input_schema,
    std::optional<sourcemeta::core::JSON> output_schema = std::nullopt,
    const MCPToolAnnotations &annotations = {}) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `tools/list` result object. For 2026-07-28, adds
/// `resultType: "complete"` and optional cache metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tools_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON tools,
    const std::optional<JSON::StringView> next_cursor = std::nullopt,
    const std::optional<MCPCachePolicy> &cache_policy = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/list` result object. For 2026-07-28, adds
/// `resultType: "complete"` and optional cache metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resources,
    const std::optional<JSON::StringView> next_cursor = std::nullopt,
    const std::optional<MCPCachePolicy> &cache_policy = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/templates/list` result object. For 2026-07-28, adds
/// `resultType: "complete"` and optional cache metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_templates_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resource_templates,
    const std::optional<JSON::StringView> next_cursor = std::nullopt,
    const std::optional<MCPCachePolicy> &cache_policy = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Capabilities advertised by an MCP server.
struct MCPServerCapabilities {
  /// Whether the server advertises prompts.
  bool prompts = false;
  /// Whether the server advertises resources.
  bool resources = false;
  /// Whether the server advertises tools.
  bool tools = false;
  /// Whether the server advertises logging.
  bool logging = false;
  /// Whether the server advertises completions.
  bool completions = false;
  /// Whether tools advertise `listChanged`.
  bool tools_list_changed = false;
  /// Whether resources advertise `subscribe`.
  bool resources_subscribe = false;
  /// Whether resources advertise `listChanged`.
  bool resources_list_changed = false;
  /// Whether prompts advertise `listChanged`.
  bool prompts_list_changed = false;
  /// Optional extensions map for capabilities.
  std::optional<sourcemeta::core::JSON> extensions = std::nullopt;
  /// Optional experimental map for capabilities.
  std::optional<sourcemeta::core::JSON> experimental = std::nullopt;
};

/// @ingroup mcp
/// Canonical list of supported MCP protocol version strings in descending
/// order.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_supported_protocol_versions() -> std::vector<JSON::StringView>;

/// @ingroup mcp
/// Build the JSON-RPC envelope returned in response to an MCP `initialize`
/// request.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_initialize_result(const sourcemeta::core::JSON &request,
                                const MCPServerCapabilities &capabilities,
                                const MCPImplementation &server,
                                const JSON::StringView instructions = {})
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build the JSON-RPC response envelope for an MCP modern `server/discover`
/// request.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_server_discover_result(
    const sourcemeta::core::JSON &identifier,
    const MCPServerCapabilities &capabilities, const MCPImplementation &server,
    const std::vector<JSON::StringView> &supported_versions = {},
    const JSON::StringView instructions = {},
    const MCPCachePolicy &cache_policy = {.ttl_ms = 3600000,
                                          .scope = MCPCacheScope::Public})
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP Multi Round-Trip Result (`input_required`).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_input_required_result(
    const sourcemeta::core::JSON &identifier,
    std::optional<sourcemeta::core::JSON> input_requests = std::nullopt,
    std::optional<JSON::StringView> request_state = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Access `inputResponses` from an MRTR continuation request envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_input_responses(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

/// @ingroup mcp
/// Access `requestState` token from an MRTR continuation request envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_state(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Build an acknowledgement notification for `subscriptions/listen`.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_subscription_acknowledged_notification(
    const JSON::StringView subscription_id,
    sourcemeta::core::JSON notifications) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a graceful close response for `subscriptions/listen`.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_subscription_close_result(
    const sourcemeta::core::JSON &identifier,
    const JSON::StringView subscription_id) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Access `io.modelcontextprotocol/subscriptionId` from request or result
/// metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_subscription_id(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Borrow the `arguments` object from a JSON-RPC `tools/call` envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_tool_call_arguments(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

} // namespace sourcemeta::core

#endif
