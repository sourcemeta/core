#ifndef SOURCEMETA_CORE_MCP_H_
#define SOURCEMETA_CORE_MCP_H_

#ifndef SOURCEMETA_CORE_MCP_EXPORT
#include <sourcemeta/core/mcp_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::int64_t, std::uint8_t
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
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#meta
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
///            sourcemeta::core::MCPProtocolVersion::V_2026_07_28) ==
///        "2026-07-28");
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
/// The MCP method name for the `prompts/list` request.
constexpr JSON::StringView MCP_METHOD_PROMPTS_LIST{"prompts/list"};

/// @ingroup mcp
/// The MCP method name for the `completion/complete` request.
constexpr JSON::StringView MCP_METHOD_COMPLETION_COMPLETE{
    "completion/complete"};

/// @ingroup mcp
/// The MCP method name for the `logging/setLevel` request.
constexpr JSON::StringView MCP_METHOD_LOGGING_SET_LEVEL{"logging/setLevel"};

/// @ingroup mcp
/// The MCP method name for the `notifications/cancelled` notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_CANCELLED{
    "notifications/cancelled"};

/// @ingroup mcp
/// The MCP method name for the `notifications/message` notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_MESSAGE{
    "notifications/message"};

/// @ingroup mcp
/// The MCP method name for the `notifications/progress` notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_PROGRESS{
    "notifications/progress"};

/// @ingroup mcp
/// The MCP method name for the `notifications/prompts/list_changed`
/// notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_PROMPTS_LIST_CHANGED{
    "notifications/prompts/list_changed"};

/// @ingroup mcp
/// The MCP method name for the `notifications/resources/list_changed`
/// notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_RESOURCES_LIST_CHANGED{
    "notifications/resources/list_changed"};

/// @ingroup mcp
/// The MCP method name for the `notifications/resources/updated` notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_RESOURCES_UPDATED{
    "notifications/resources/updated"};

/// @ingroup mcp
/// The MCP method name for the `notifications/tools/list_changed` notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_TOOLS_LIST_CHANGED{
    "notifications/tools/list_changed"};

/// @ingroup mcp
/// The MCP method name for the `roots/list` request.
constexpr JSON::StringView MCP_METHOD_ROOTS_LIST{"roots/list"};

/// @ingroup mcp
/// The MCP method name for the `sampling/createMessage` request.
constexpr JSON::StringView MCP_METHOD_SAMPLING_CREATE_MESSAGE{
    "sampling/createMessage"};

/// @ingroup mcp
/// The MCP method name for the `elicitation/create` request.
constexpr JSON::StringView MCP_METHOD_ELICITATION_CREATE{"elicitation/create"};

/// @ingroup mcp
/// Check whether the given method name corresponds to an operation that
/// requires a name or URI identifier (such as `tools/call`, `resources/read`,
/// or `prompts/get`). For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_is_named_request_method("tools/call"));
/// assert(sourcemeta::core::mcp_is_named_request_method("resources/read"));
/// assert(sourcemeta::core::mcp_is_named_request_method("prompts/get"));
/// assert(!sourcemeta::core::mcp_is_named_request_method("tools/list"));
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http/#standard-request-headers
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
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#error-codes
constexpr std::int64_t MCP_CODE_HEADER_MISMATCH{-32020};

/// @ingroup mcp
/// The MCP error code indicating that a client is missing a capability required
/// to process a request.
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#error-codes
constexpr std::int64_t MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY{-32021};

/// @ingroup mcp
/// The MCP error code indicating that the client's requested protocol version
/// is not supported.
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#error-codes
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
/// method for the specified protocol version. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_is_request_method(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
/// assert(sourcemeta::core::mcp_is_request_method(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/get"));
/// assert(!sourcemeta::core::mcp_is_request_method(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
/// ```
constexpr auto mcp_is_request_method(const MCPProtocolVersion version,
                                     const JSON::StringView method) noexcept
    -> bool {
  if (version == MCPProtocolVersion::V_2026_07_28) {
    return method == MCP_METHOD_SERVER_DISCOVER ||
           method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
           method == MCP_METHOD_RESOURCES_LIST ||
           method == MCP_METHOD_RESOURCES_READ ||
           method == MCP_METHOD_RESOURCES_TEMPLATES_LIST ||
           method == MCP_METHOD_PROMPTS_LIST ||
           method == MCP_METHOD_PROMPTS_GET ||
           method == MCP_METHOD_COMPLETION_COMPLETE ||
           method == MCP_METHOD_LOGGING_SET_LEVEL ||
           method == MCP_METHOD_SUBSCRIPTIONS_LISTEN;
  }

  return method == MCP_METHOD_INITIALIZE || method == MCP_METHOD_PING ||
         method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
         method == MCP_METHOD_RESOURCES_LIST ||
         method == MCP_METHOD_RESOURCES_READ ||
         method == MCP_METHOD_RESOURCES_TEMPLATES_LIST ||
         method == MCP_METHOD_PROMPTS_LIST ||
         method == MCP_METHOD_PROMPTS_GET ||
         method == MCP_METHOD_COMPLETION_COMPLETE ||
         method == MCP_METHOD_LOGGING_SET_LEVEL;
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
/// Classify an MCP method or notification into its protocol era. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_classify_method("initialize") ==
///        sourcemeta::core::MCPMethodEra::LegacyOnly);
/// assert(sourcemeta::core::mcp_classify_method("server/discover") ==
///        sourcemeta::core::MCPMethodEra::ModernOnly);
/// assert(sourcemeta::core::mcp_classify_method("tools/list") ==
///        sourcemeta::core::MCPMethodEra::Shared);
/// ```
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
      method == MCP_METHOD_RESOURCES_TEMPLATES_LIST ||
      method == MCP_METHOD_PROMPTS_LIST || method == MCP_METHOD_PROMPTS_GET ||
      method == MCP_METHOD_COMPLETION_COMPLETE ||
      method == MCP_METHOD_LOGGING_SET_LEVEL ||
      method == MCP_METHOD_NOTIFICATIONS_CANCELLED ||
      method == MCP_METHOD_NOTIFICATIONS_MESSAGE ||
      method == MCP_METHOD_NOTIFICATIONS_PROGRESS ||
      method == MCP_METHOD_NOTIFICATIONS_PROMPTS_LIST_CHANGED ||
      method == MCP_METHOD_NOTIFICATIONS_RESOURCES_LIST_CHANGED ||
      method == MCP_METHOD_NOTIFICATIONS_RESOURCES_UPDATED ||
      method == MCP_METHOD_NOTIFICATIONS_TOOLS_LIST_CHANGED ||
      method == MCP_METHOD_ROOTS_LIST ||
      method == MCP_METHOD_SAMPLING_CREATE_MESSAGE ||
      method == MCP_METHOD_ELICITATION_CREATE) {
    return MCPMethodEra::Shared;
  }
  return MCPMethodEra::Unsupported;
}

/// @ingroup mcp
/// Check whether the given method name is supported in the specified protocol
/// version. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_method(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
/// assert(!sourcemeta::core::mcp_supports_method(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
/// ```
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
/// Check whether the given method name is legacy-only. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_is_legacy_only_method("initialize"));
/// assert(!sourcemeta::core::mcp_is_legacy_only_method("tools/list"));
/// ```
constexpr auto mcp_is_legacy_only_method(const JSON::StringView method) noexcept
    -> bool {
  return mcp_classify_method(method) == MCPMethodEra::LegacyOnly;
}

/// @ingroup mcp
/// Check whether the given method name is modern-only. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_is_modern_only_method("server/discover"));
/// assert(!sourcemeta::core::mcp_is_modern_only_method("tools/list"));
/// ```
constexpr auto mcp_is_modern_only_method(const JSON::StringView method) noexcept
    -> bool {
  return mcp_classify_method(method) == MCPMethodEra::ModernOnly;
}

/// @ingroup mcp
/// Check whether the given method name is supported across both legacy and
/// modern revisions. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_is_shared_method("tools/list"));
/// assert(!sourcemeta::core::mcp_is_shared_method("initialize"));
/// ```
constexpr auto mcp_is_shared_method(const JSON::StringView method) noexcept
    -> bool {
  return mcp_classify_method(method) == MCPMethodEra::Shared;
}

/// @ingroup mcp
/// Resolve an `MCP-Protocol-Version` header value into a known protocol
/// version, or `std::nullopt` when the value is unrecognised. An absent header
/// resolves to the oldest supported version per the Streamable HTTP transport:
/// "A server that supports clients implementing protocol versions earlier than
/// 2025-06-18 (which did not define the MCP-Protocol-Version header) MAY treat
/// a request that omits the header as protocol version 2025-03-26."
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http/#protocol-version-header
///
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_resolve_protocol_version("2026-07-28") ==
///        sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
/// assert(sourcemeta::core::mcp_resolve_protocol_version("") ==
///        sourcemeta::core::MCPProtocolVersion::V_2025_03_26);
/// ```
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
/// `notifications/initialized` handshake. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_uses_initialization_handshake(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// assert(!sourcemeta::core::mcp_uses_initialization_handshake(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// ```
constexpr auto
mcp_uses_initialization_handshake(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version supports the `ping` request. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_ping(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// assert(!sourcemeta::core::mcp_supports_ping(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// ```
constexpr auto mcp_supports_ping(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version uses protocol-level sessions. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_protocol_sessions(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// assert(!sourcemeta::core::mcp_supports_protocol_sessions(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// ```
constexpr auto
mcp_supports_protocol_sessions(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version requires per-request stateless metadata
/// (`_meta`). For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_requires_request_meta(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// assert(!sourcemeta::core::mcp_requires_request_meta(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#meta
constexpr auto
mcp_requires_request_meta(const MCPProtocolVersion version) noexcept -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version requires the `resultType` field on
/// successful MCP results. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_requires_result_type(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// assert(!sourcemeta::core::mcp_requires_result_type(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#result-responses
constexpr auto
mcp_requires_result_type(const MCPProtocolVersion version) noexcept -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version requires or supports cacheable-result
/// metadata (`ttlMs` and `cacheScope`). For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_requires_cacheable_metadata(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// assert(!sourcemeta::core::mcp_requires_cacheable_metadata(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
constexpr auto
mcp_requires_cacheable_metadata(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version supports the `server/discover` request.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_server_discover(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// assert(!sourcemeta::core::mcp_supports_server_discover(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/discover/
constexpr auto
mcp_supports_server_discover(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version supports the `subscriptions/listen`
/// request. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_subscriptions_listen(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// assert(!sourcemeta::core::mcp_supports_subscriptions_listen(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions/
constexpr auto
mcp_supports_subscriptions_listen(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the given protocol version supports Multi Round-Trip Results
/// (MRTR). For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_mrtr(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
/// assert(!sourcemeta::core::mcp_supports_mrtr(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr/
constexpr auto mcp_supports_mrtr(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// The latest protocol version that supports the `initialize` handshake. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_latest_initialization_version() ==
///        sourcemeta::core::MCPProtocolVersion::V_2025_11_25);
/// ```
constexpr auto mcp_latest_initialization_version() noexcept
    -> MCPProtocolVersion {
  return MCPProtocolVersion::V_2025_11_25;
}

/// @ingroup mcp
/// Whether the given protocol version supports per-tool `outputSchema`. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_output_schema(
///     sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
/// assert(!sourcemeta::core::mcp_supports_output_schema(
///     sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
/// ```
constexpr auto
mcp_supports_output_schema(const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the given protocol version supports `structuredContent` in tool
/// results. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_structured_content(
///     sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
/// assert(!sourcemeta::core::mcp_supports_structured_content(
///     sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
/// ```
constexpr auto
mcp_supports_structured_content(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the given protocol version supports `resource_link` content blocks.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_resource_link_content(
///     sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
/// assert(!sourcemeta::core::mcp_supports_resource_link_content(
///     sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
/// ```
constexpr auto
mcp_supports_resource_link_content(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the given protocol version supports the `title` field on the
/// implementation info object. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_implementation_title(
///     sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
/// assert(!sourcemeta::core::mcp_supports_implementation_title(
///     sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
/// ```
constexpr auto
mcp_supports_implementation_title(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the given protocol version supports the `description` field on the
/// implementation info object. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_implementation_description(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// assert(!sourcemeta::core::mcp_supports_implementation_description(
///     sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
/// ```
constexpr auto mcp_supports_implementation_description(
    const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_11_25);
}

/// @ingroup mcp
/// Whether the given protocol version supports the `websiteUrl` field on the
/// implementation info object. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_implementation_website_url(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
/// assert(!sourcemeta::core::mcp_supports_implementation_website_url(
///     sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
/// ```
constexpr auto mcp_supports_implementation_website_url(
    const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_11_25);
}

/// @ingroup mcp
/// Whether the given protocol version supports JSON-RPC 2.0 batching. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_supports_jsonrpc_batching(
///     sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
/// assert(!sourcemeta::core::mcp_supports_jsonrpc_batching(
///     sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
/// ```
constexpr auto
mcp_supports_jsonrpc_batching(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2025_03_26;
}

/// @ingroup mcp
/// Cache scope for modern cacheable MCP results.
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
enum class MCPCacheScope : std::uint8_t {
  /// Public caching is permissible across multiple clients/users.
  Public,
  /// Result is private and must only be cached per-client/user.
  Private,
};

/// @ingroup mcp
/// Canonical string conversion for cache scope. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_cache_scope_string(
///            sourcemeta::core::MCPCacheScope::Public) == "public");
/// assert(sourcemeta::core::mcp_cache_scope_string(
///            sourcemeta::core::MCPCacheScope::Private) == "private");
/// ```
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
/// Parse a canonical cache scope string. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_resolve_cache_scope("public") ==
///        sourcemeta::core::MCPCacheScope::Public);
/// assert(sourcemeta::core::mcp_resolve_cache_scope("private") ==
///        sourcemeta::core::MCPCacheScope::Private);
/// assert(!sourcemeta::core::mcp_resolve_cache_scope("shared").has_value());
/// ```
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
/// results. Both fields are required for cacheable operations in MCP
/// 2026-07-28.
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
struct MCPCachePolicy {
  /// Time-to-live in milliseconds. Must be non-negative.
  std::int64_t ttl_ms;
  /// Cache scope (public or private).
  MCPCacheScope scope;
};

/// @ingroup mcp
/// Client implementation information attached to modern MCP request metadata.
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#meta
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
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#meta
struct MCPClientCapabilities {
  /// Whether the client advertises roots.
  bool roots = false;
  /// Whether roots advertise `listChanged` (legacy revisions only).
  bool roots_list_changed = false;
  /// Whether the client advertises sampling.
  bool sampling = false;
  /// Whether sampling advertises context support.
  bool sampling_context = false;
  /// Whether sampling advertises tools support.
  bool sampling_tools = false;
  /// Whether the client advertises elicitation.
  bool elicitation = false;
  /// Whether elicitation advertises form support.
  bool elicitation_form = false;
  /// Whether elicitation advertises url support.
  bool elicitation_url = false;
  /// Optional extensions map for client capabilities.
  std::optional<sourcemeta::core::JSON> extensions = std::nullopt;
  /// Optional experimental map for client capabilities.
  std::optional<sourcemeta::core::JSON> experimental = std::nullopt;
};

/// @ingroup mcp
/// Parse client capabilities from a JSON object. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto payload{sourcemeta::core::parse_json(
///     R"({ "roots": {}, "sampling": { "tools": {} } })")};
/// const auto caps{sourcemeta::core::mcp_parse_client_capabilities(payload)};
/// assert(caps.roots);
/// assert(caps.sampling);
/// assert(caps.sampling_tools);
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_parse_client_capabilities(const sourcemeta::core::JSON &capabilities)
    -> MCPClientCapabilities;

/// @ingroup mcp
/// Serialize client capabilities to a JSON object appropriate for the given
/// protocol version. For 2026-07-28, omits `roots.listChanged`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// sourcemeta::core::MCPClientCapabilities caps;
/// caps.roots = true;
/// caps.roots_list_changed = true;
/// const auto json{sourcemeta::core::mcp_serialize_client_capabilities(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, caps)};
/// assert(json.at("roots").is_object());
/// assert(!json.at("roots").defines("listChanged"));
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_serialize_client_capabilities(
    const MCPProtocolVersion version, const MCPClientCapabilities &capabilities)
    -> sourcemeta::core::JSON;

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
  /// Client implementation info (optional per spec:basic/index#meta).
  std::optional<MCPClientInfo> client_info = std::nullopt;
  /// Optional requested log level.
  std::optional<JSON::StringView> log_level = std::nullopt;
  /// Non-null pointer to the entire `_meta` object within the request.
  const sourcemeta::core::JSON *meta_object = nullptr;
};

/// @ingroup mcp
/// Validate and extract modern MCP request metadata from a request envelope or
/// params object. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/list",
///   "params": { "_meta": {
///     "io.modelcontextprotocol/protocolVersion": "2026-07-28",
///     "io.modelcontextprotocol/clientCapabilities": {}
///   } }
/// })JSON")};
/// const auto [status, meta]{
///     sourcemeta::core::mcp_validate_request_meta(envelope)};
/// assert(status == sourcemeta::core::MCPRequestMetaStatus::Valid);
/// assert(meta.has_value());
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#meta
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_request_meta(const sourcemeta::core::JSON &envelope_or_params)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>>;

/// @ingroup mcp
/// Read the declared protocol version from a modern MCP request envelope. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/list",
///   "params": { "_meta": {
///     "io.modelcontextprotocol/protocolVersion": "2026-07-28",
///     "io.modelcontextprotocol/clientCapabilities": {}
///   } }
/// })JSON")};
/// assert(sourcemeta::core::mcp_request_protocol_version(envelope) ==
///        sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_protocol_version(const sourcemeta::core::JSON &envelope)
    -> std::optional<MCPProtocolVersion>;

/// @ingroup mcp
/// Read client implementation information from a modern MCP request envelope.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/list",
///   "params": { "_meta": {
///     "io.modelcontextprotocol/protocolVersion": "2026-07-28",
///     "io.modelcontextprotocol/clientCapabilities": {},
///     "io.modelcontextprotocol/clientInfo": {
///       "name": "my-client", "version": "1.0.0"
///     }
///   } }
/// })JSON")};
/// const auto info{sourcemeta::core::mcp_request_client_info(envelope)};
/// assert(info.has_value());
/// assert(info->name == "my-client");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_client_info(const sourcemeta::core::JSON &envelope)
    -> std::optional<MCPClientInfo>;

/// @ingroup mcp
/// Read client capabilities object from a modern MCP request envelope. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/list",
///   "params": { "_meta": {
///     "io.modelcontextprotocol/protocolVersion": "2026-07-28",
///     "io.modelcontextprotocol/clientCapabilities": { "roots": {} }
///   } }
/// })JSON")};
/// const auto *caps{
///     sourcemeta::core::mcp_request_client_capabilities(envelope)};
/// assert(caps != nullptr);
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_client_capabilities(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

/// @ingroup mcp
/// Read the requested log level from a modern MCP request envelope. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/list",
///   "params": { "_meta": {
///     "io.modelcontextprotocol/protocolVersion": "2026-07-28",
///     "io.modelcontextprotocol/clientCapabilities": {},
///     "io.modelcontextprotocol/logLevel": "info"
///   } }
/// })JSON")};
/// assert(sourcemeta::core::mcp_request_log_level(envelope) == "info");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_log_level(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Check whether a modern MCP request contains all required metadata. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/list",
///   "params": { "_meta": {
///     "io.modelcontextprotocol/protocolVersion": "2026-07-28",
///     "io.modelcontextprotocol/clientCapabilities": {}
///   } }
/// })JSON")};
/// assert(sourcemeta::core::mcp_has_required_request_meta(envelope));
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_has_required_request_meta(const sourcemeta::core::JSON &envelope)
    -> bool;

/// @ingroup mcp
/// Extract the request method declared in a JSON-RPC request body. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(
///     R"({ "jsonrpc": "2.0", "id": 1, "method": "tools/list" })")};
/// assert(sourcemeta::core::mcp_request_method_from_body(envelope) ==
///        "tools/list");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_method_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Extract the target tool, resource, or prompt name declared in a request body
/// (used to validate against the `Mcp-Name` header). For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/call",
///   "params": { "name": "my_tool" }
/// })JSON")};
/// assert(sourcemeta::core::mcp_request_name_from_body(envelope) == "my_tool");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_name_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Map an MCP or JSON-RPC error code to the corresponding HTTP status code
/// per the Streamable HTTP transport specification. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <sourcemeta/core/jsonrpc.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::mcp_error_code_to_http_status(
///            sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND) == 404);
/// assert(sourcemeta::core::mcp_error_code_to_http_status(
///            sourcemeta::core::MCP_CODE_HEADER_MISMATCH) == 400);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http/#protocol-version-header
constexpr auto mcp_error_code_to_http_status(const std::int64_t code) noexcept
    -> int {
  if (code == JSONRPC_CODE_METHOD_NOT_FOUND) {
    return 404;
  }
  if (code == JSONRPC_CODE_INVALID_REQUEST ||
      code == JSONRPC_CODE_INVALID_PARAMS || code == JSONRPC_CODE_PARSE ||
      code == MCP_CODE_HEADER_MISMATCH ||
      code == MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY ||
      code == MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION) {
    return 400;
  }
  return 500;
}

/// @ingroup mcp
/// Construct an appropriate JSON-RPC error response from an invalid
/// @ref MCPRequestMetaStatus. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// const auto err{sourcemeta::core::mcp_make_error_request_meta(
///     &id, sourcemeta::core::MCPRequestMetaStatus::MissingMeta)};
/// assert(err.at("error").at("code").to_integer() == -32602);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#meta
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_request_meta(const sourcemeta::core::JSON *identifier,
                                 const MCPRequestMetaStatus status)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for unsupported protocol version (-32022).
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// const auto
/// err{sourcemeta::core::mcp_make_error_unsupported_protocol_version(
///     &id, "1.0", {"2026-07-28"})};
/// assert(err.at("error").at("code").to_integer() == -32022);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#error-codes
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_unsupported_protocol_version(
    const sourcemeta::core::JSON *identifier, const JSON::StringView requested,
    const std::vector<JSON::StringView> &supported) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for missing required client capability
/// (-32021). For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// auto required{sourcemeta::core::JSON::make_object()};
/// const auto err{sourcemeta::core::mcp_make_error_missing_required_capability(
///     &id, std::move(required))};
/// assert(err.at("error").at("code").to_integer() == -32021);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#error-codes
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_missing_required_capability(
    const sourcemeta::core::JSON *identifier,
    sourcemeta::core::JSON required_capabilities,
    const JSON::StringView message = "Missing required client capability")
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020). For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// const auto err{sourcemeta::core::mcp_make_error_header_mismatch(
///     &id, "Header mismatch")};
/// assert(err.at("error").at("code").to_integer() == -32020);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#error-codes
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_header_mismatch(
    const sourcemeta::core::JSON *identifier,
    const JSON::StringView message = "Header mismatch")
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020) with detailed
/// header mismatch payload. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// const auto err{sourcemeta::core::mcp_make_error_header_mismatch(
///     &id, "Mcp-Method", "tools/list", "tools/call")};
/// assert(err.at("error").at("code").to_integer() == -32020);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http/#standard-request-headers
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_header_mismatch(const sourcemeta::core::JSON *identifier,
                                    const JSON::StringView header_name,
                                    const JSON::StringView header_value,
                                    const JSON::StringView body_value)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC error response for header mismatch (-32020) reporting an
/// unexpected header supplied without a corresponding body value. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// const auto err{sourcemeta::core::mcp_make_error_header_mismatch(
///     &id, "Mcp-Name", "unexpected")};
/// assert(err.at("error").at("code").to_integer() == -32020);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http/#standard-request-headers
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
/// `Mcp-Method`, and `Mcp-Name` for named methods) are required.
/// For legacy revisions, routing headers are optional but verified if
/// present.
///
/// Returns a JSON-RPC error response envelope if a required header is missing,
/// malformed, or mismatches the request body, or `std::nullopt` if validation
/// succeeds.
///
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/list",
///   "params": { "_meta": {
///     "io.modelcontextprotocol/protocolVersion": "2026-07-28",
///     "io.modelcontextprotocol/clientCapabilities": {}
///   } }
/// })JSON")};
/// const auto err{sourcemeta::core::mcp_validate_request_headers(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     "2026-07-28", "tools/list", std::nullopt, envelope)};
/// assert(!err.has_value());
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http/#standard-request-headers
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
/// Params) for 2026-07-28. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// const auto err{sourcemeta::core::mcp_make_error_resource_not_found(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, id)};
/// assert(err.at("error").at("code").to_integer() == -32602);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#error-codes
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_error_resource_not_found(const MCPProtocolVersion version,
                                       const sourcemeta::core::JSON &identifier)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `text` content block carrying the given text payload. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto block{sourcemeta::core::mcp_make_text_block("hello")};
/// assert(block.at("type").to_string() == "text");
/// assert(block.at("text").to_string() == "hello");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_text_block(const JSON::StringView text) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP content block referencing a resource by URI. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto block{sourcemeta::core::mcp_make_resource_link(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     "file:///data.txt", "text/plain")};
/// assert(block.at("type").to_string() == "resource_link");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_link(const MCPProtocolVersion version,
                            const JSON::StringView uri,
                            const JSON::StringView mime_type,
                            const JSON::StringView name = {},
                            const JSON::StringView description = {})
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Implementation info advertised by an MCP server.
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#meta
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
/// `_meta.io.modelcontextprotocol/serverInfo` for modern versions). For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto result{sourcemeta::core::JSON::make_object()};
/// const auto decorated{sourcemeta::core::mcp_decorate_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::move(result))};
/// assert(decorated.at("resultType").to_string() == "complete");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#result-responses
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_decorate_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON result,
    const std::optional<MCPImplementation> &server_info = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Decorate an existing MCP result object in-place without copying. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto result{sourcemeta::core::JSON::make_object()};
/// sourcemeta::core::mcp_decorate_result_in_place(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, result);
/// assert(result.at("resultType").to_string() == "complete");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
void mcp_decorate_result_in_place(
    const MCPProtocolVersion version, sourcemeta::core::JSON &result,
    const std::optional<MCPImplementation> &server_info = std::nullopt);

/// @ingroup mcp
/// Decorate an MCP result object with cache metadata (`ttlMs` and
/// `cacheScope`) for modern versions. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto result{sourcemeta::core::JSON::make_object()};
/// sourcemeta::core::MCPCachePolicy policy{.ttl_ms = 5000,
///     .scope = sourcemeta::core::MCPCacheScope::Private};
/// const auto decorated{sourcemeta::core::mcp_decorate_cacheable_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::move(result),
///     policy)};
/// assert(decorated.at("ttlMs").to_integer() == 5000);
/// assert(decorated.at("cacheScope").to_string() == "private");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_decorate_cacheable_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON result,
    const std::optional<MCPCachePolicy> &cache_policy = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Decorate an existing MCP result object with cache metadata in-place.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto result{sourcemeta::core::JSON::make_object()};
/// sourcemeta::core::MCPCachePolicy policy{.ttl_ms = 3000,
///     .scope = sourcemeta::core::MCPCacheScope::Public};
/// sourcemeta::core::mcp_decorate_cacheable_result_in_place(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, result, policy);
/// assert(result.at("ttlMs").to_integer() == 3000);
/// ```
SOURCEMETA_CORE_MCP_EXPORT
void mcp_decorate_cacheable_result_in_place(
    const MCPProtocolVersion version, sourcemeta::core::JSON &result,
    const std::optional<MCPCachePolicy> &cache_policy = std::nullopt);

/// @ingroup mcp
/// Build an empty MCP result response envelope with `resultType: "complete"`.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// const auto resp{sourcemeta::core::mcp_make_empty_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, id)};
/// assert(resp.at("result").at("resultType").to_string() == "complete");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/index/#result-responses
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_empty_result(
    const MCPProtocolVersion version, const sourcemeta::core::JSON &identifier,
    const std::optional<MCPImplementation> &server_info = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC envelope wrapping a successful MCP tool call response from
/// the given result payload. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// auto res{sourcemeta::core::JSON::make_object()};
/// const auto envelope{sourcemeta::core::mcp_make_tool_success(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, id,
///     std::move(res))};
/// assert(envelope.at("result").at("resultType").to_string() == "complete");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &identifier,
                           sourcemeta::core::JSON result)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a JSON-RPC envelope wrapping a successful MCP tool call response from
/// caller-provided content blocks and a structured payload. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// auto structured{sourcemeta::core::JSON::make_object()};
/// auto content{sourcemeta::core::JSON::make_array()};
/// const auto envelope{sourcemeta::core::mcp_make_tool_success(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, id,
///     std::move(structured), std::move(content))};
/// assert(envelope.at("result").at("resultType").to_string() == "complete");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &identifier,
                           sourcemeta::core::JSON structured,
                           sourcemeta::core::JSON content_blocks)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a version-aware JSON-RPC envelope wrapping a failed MCP tool call
/// response with the given error message. For 2026-07-28, adds
/// `resultType: "complete"`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// const auto envelope{sourcemeta::core::mcp_make_tool_error(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28, id, "Failed")};
/// assert(envelope.at("result").at("isError").to_boolean());
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_error(const MCPProtocolVersion version,
                         const sourcemeta::core::JSON &identifier,
                         const JSON::StringView message)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP resource descriptor as used in `resources/list` responses. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto res{sourcemeta::core::mcp_make_resource(
///     "file:///data.txt", "Data", "text/plain")};
/// assert(res.at("uri").to_string() == "file:///data.txt");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource(const JSON::StringView uri, const JSON::StringView name,
                       const JSON::StringView mime_type,
                       const JSON::StringView description = {},
                       const std::optional<std::size_t> size = std::nullopt,
                       const std::optional<double> priority = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/read` content entry of `text` flavour. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto entry{sourcemeta::core::mcp_make_resource_text_content(
///     "file:///data.txt", "text/plain", "contents")};
/// assert(entry.at("text").to_string() == "contents");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_text_content(const JSON::StringView uri,
                                    const JSON::StringView mime_type,
                                    const JSON::StringView text)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Wrap a pre-built array of content entries into the MCP `resources/read`
/// result envelope with a required cache policy for 2026-07-28. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto contents{sourcemeta::core::JSON::make_array()};
/// sourcemeta::core::MCPCachePolicy policy{.ttl_ms = 60000,
///     .scope = sourcemeta::core::MCPCacheScope::Public};
/// const auto res{sourcemeta::core::mcp_make_resources_read_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     std::move(contents), policy)};
/// assert(res.at("resultType").to_string() == "complete");
/// assert(res.at("ttlMs").to_integer() == 60000);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_read_result(const MCPProtocolVersion version,
                                    sourcemeta::core::JSON contents,
                                    const MCPCachePolicy &cache_policy)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Wrap a pre-built array of content entries into the MCP `resources/read`
/// result envelope for legacy revisions where caching metadata is not emitted.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto contents{sourcemeta::core::JSON::make_array()};
/// const auto res{sourcemeta::core::mcp_make_resources_read_result(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
///     std::move(contents))};
/// assert(!res.defines("ttlMs"));
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_read_result(const MCPProtocolVersion version,
                                    sourcemeta::core::JSON contents)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a single entry for an MCP `resources/templates/list` response. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto tmpl{sourcemeta::core::mcp_make_resource_template(
///     "file:///{path}", "File", "A file template", "text/plain")};
/// assert(tmpl.at("uriTemplate").to_string() == "file:///{path}");
/// ```
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
/// Build a single entry for an MCP `tools/list` response. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto schema{sourcemeta::core::JSON::make_object()};
/// schema.assign("type", sourcemeta::core::JSON{"object"});
/// const auto tool{sourcemeta::core::mcp_make_tool_descriptor(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     "calculator", "Math tool", std::move(schema))};
/// assert(tool.at("name").to_string() == "calculator");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_descriptor(
    const MCPProtocolVersion version, const JSON::StringView name,
    const JSON::StringView description, sourcemeta::core::JSON input_schema,
    std::optional<sourcemeta::core::JSON> output_schema = std::nullopt,
    const MCPToolAnnotations &annotations = {}) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `tools/list` result object for 2026-07-28 with required cache
/// metadata. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto tools{sourcemeta::core::JSON::make_array()};
/// sourcemeta::core::MCPCachePolicy policy{.ttl_ms = 30000,
///     .scope = sourcemeta::core::MCPCacheScope::Public};
/// const auto res{sourcemeta::core::mcp_make_tools_list_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     std::move(tools), std::nullopt, policy)};
/// assert(res.at("resultType").to_string() == "complete");
/// assert(res.at("ttlMs").to_integer() == 30000);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tools_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON tools,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `tools/list` result object for legacy protocol revisions.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto tools{sourcemeta::core::JSON::make_array()};
/// const auto res{sourcemeta::core::mcp_make_tools_list_result(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
///     std::move(tools))};
/// assert(!res.defines("ttlMs"));
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tools_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON tools,
    const std::optional<JSON::StringView> next_cursor = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/list` result object for 2026-07-28 with required
/// cache metadata. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto resources{sourcemeta::core::JSON::make_array()};
/// sourcemeta::core::MCPCachePolicy policy{.ttl_ms = 30000,
///     .scope = sourcemeta::core::MCPCacheScope::Public};
/// const auto res{sourcemeta::core::mcp_make_resources_list_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     std::move(resources), std::nullopt, policy)};
/// assert(res.at("resultType").to_string() == "complete");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resources,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/list` result object for legacy protocol revisions.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto resources{sourcemeta::core::JSON::make_array()};
/// const auto res{sourcemeta::core::mcp_make_resources_list_result(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
///     std::move(resources))};
/// assert(!res.defines("ttlMs"));
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resources,
    const std::optional<JSON::StringView> next_cursor = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/templates/list` result object for 2026-07-28 with
/// required cache metadata. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto tmpls{sourcemeta::core::JSON::make_array()};
/// sourcemeta::core::MCPCachePolicy policy{.ttl_ms = 30000,
///     .scope = sourcemeta::core::MCPCacheScope::Public};
/// const auto res{sourcemeta::core::mcp_make_resource_templates_list_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     std::move(tmpls), std::nullopt, policy)};
/// assert(res.at("resultType").to_string() == "complete");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_templates_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resource_templates,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/templates/list` result object for legacy revisions.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto tmpls{sourcemeta::core::JSON::make_array()};
/// const auto res{sourcemeta::core::mcp_make_resource_templates_list_result(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
///     std::move(tmpls))};
/// assert(!res.defines("ttlMs"));
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_templates_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resource_templates,
    const std::optional<JSON::StringView> next_cursor = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `prompts/list` result object for 2026-07-28 with required cache
/// metadata. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto prompts{sourcemeta::core::JSON::make_array()};
/// sourcemeta::core::MCPCachePolicy policy{.ttl_ms = 30000,
///     .scope = sourcemeta::core::MCPCacheScope::Public};
/// const auto res{sourcemeta::core::mcp_make_prompts_list_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     std::move(prompts), std::nullopt, policy)};
/// assert(res.at("resultType").to_string() == "complete");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching/#cacheable-results
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_prompts_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON prompts,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `prompts/list` result object for legacy revisions. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto prompts{sourcemeta::core::JSON::make_array()};
/// const auto res{sourcemeta::core::mcp_make_prompts_list_result(
///     sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
///     std::move(prompts))};
/// assert(!res.defines("ttlMs"));
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_prompts_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON prompts,
    const std::optional<JSON::StringView> next_cursor = std::nullopt)
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
/// order. For example:
///
/// ```cpp
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto versions{sourcemeta::core::mcp_supported_protocol_versions()};
/// assert(versions.front() == "2026-07-28");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_supported_protocol_versions() -> std::vector<JSON::StringView>;

/// @ingroup mcp
/// Build the JSON-RPC envelope returned in response to an MCP `initialize`
/// request. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto req{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "initialize",
///   "params": {
///     "protocolVersion": "2025-11-25",
///     "capabilities": {},
///     "clientInfo": { "name": "c", "version": "1" }
///   }
/// })JSON")};
/// sourcemeta::core::MCPServerCapabilities caps;
/// sourcemeta::core::MCPImplementation server{.name = "srv", .version = "1"};
/// const auto res{sourcemeta::core::mcp_make_initialize_result(
///     req, caps, server)};
/// assert(res.at("result").at("serverInfo").at("name").to_string() == "srv");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2025-11-25/basic/lifecycle/#version-negotiation
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_initialize_result(const sourcemeta::core::JSON &request,
                                const MCPServerCapabilities &capabilities,
                                const MCPImplementation &server,
                                const JSON::StringView instructions = {})
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build the JSON-RPC response envelope for an MCP modern `server/discover`
/// request. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// sourcemeta::core::MCPServerCapabilities caps;
/// sourcemeta::core::MCPImplementation server{.name = "srv", .version = "1"};
/// sourcemeta::core::MCPCachePolicy policy{.ttl_ms = 3600000,
///     .scope = sourcemeta::core::MCPCacheScope::Public};
/// const auto res{sourcemeta::core::mcp_make_server_discover_result(
///     id, caps, server, {"2026-07-28"}, "Welcome", policy)};
/// assert(res.at("result").at("resultType").to_string() == "complete");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/server/discover/
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_server_discover_result(
    const sourcemeta::core::JSON &identifier,
    const MCPServerCapabilities &capabilities, const MCPImplementation &server,
    const std::vector<JSON::StringView> &supported_versions,
    const JSON::StringView instructions, const MCPCachePolicy &cache_policy)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP Multi Round-Trip Result (`input_required`). At least one of
/// `input_requests` or `request_state` must be provided. Only `tools/call`,
/// `resources/read`, and `prompts/get` may carry this result. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{1}};
/// auto input_reqs{sourcemeta::core::JSON::make_object()};
/// const auto res{sourcemeta::core::mcp_make_input_required_result(
///     sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
///     "tools/call", id, std::move(input_reqs), "token-123")};
/// assert(res.at("result").at("resultType").to_string() == "input_required");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr/
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_input_required_result(
    const MCPProtocolVersion version, const JSON::StringView method,
    const sourcemeta::core::JSON &identifier,
    std::optional<sourcemeta::core::JSON> input_requests,
    std::optional<JSON::StringView> request_state) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Access `inputResponses` from an MRTR continuation request envelope. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto req{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/call",
///   "params": { "inputResponses": { "x": 42 } }
/// })JSON")};
/// const auto *resp{sourcemeta::core::mcp_request_input_responses(req)};
/// assert(resp != nullptr);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr/
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_input_responses(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

/// @ingroup mcp
/// Access `requestState` token from an MRTR continuation request envelope. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto req{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/call",
///   "params": { "requestState": "token-123" }
/// })JSON")};
/// assert(sourcemeta::core::mcp_request_state(req) == "token-123");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr/
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_state(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Build an acknowledgement notification for `subscriptions/listen` with a
/// string or integer subscription ID. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto notifs{sourcemeta::core::JSON::make_array()};
/// const auto notif{
///     sourcemeta::core::mcp_make_subscription_acknowledged_notification(
///         "sub-1", std::move(notifs))};
/// assert(notif.at("method").to_string() ==
///        "notifications/subscriptions/acknowledged");
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions/#receiving-notifications
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_subscription_acknowledged_notification(
    const sourcemeta::core::JSON &subscription_id,
    sourcemeta::core::JSON notifications) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an acknowledgement notification for `subscriptions/listen` with a
/// string subscription ID. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// auto notifs{sourcemeta::core::JSON::make_array()};
/// const auto notif{
///     sourcemeta::core::mcp_make_subscription_acknowledged_notification(
///         "sub-42", std::move(notifs))};
/// assert(notif.at("jsonrpc").to_string() == "2.0");
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_subscription_acknowledged_notification(
    const JSON::StringView subscription_id,
    sourcemeta::core::JSON notifications) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a graceful close response for `subscriptions/listen`. The
/// subscription identifier is derived from the request identifier per
/// the specification: "The value matches the JSON-RPC id of the originating
/// subscriptions/listen request."
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions/#graceful-closure
///
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto id{sourcemeta::core::JSON{42}};
/// const auto res{sourcemeta::core::mcp_make_subscription_close_result(id)};
/// assert(res.at("result").at("resultType").to_string() == "complete");
/// assert(res.at("result").at("_meta")
///            .at("io.modelcontextprotocol/subscriptionId").to_integer() ==
///            42);
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_subscription_close_result(
    const sourcemeta::core::JSON &identifier) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Access `io.modelcontextprotocol/subscriptionId` from request or result
/// metadata. Returns a pointer to the subscription identifier (string or
/// number) or `nullptr` when absent or of another type. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "method": "test",
///   "params": { "_meta": {
///     "io.modelcontextprotocol/subscriptionId": 7
///   } }
/// })JSON")};
/// const auto *id{sourcemeta::core::mcp_request_subscription_id(envelope)};
/// assert(id != nullptr);
/// assert(id->to_integer() == 7);
/// ```
///
/// @see
/// https://spec.modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions/#receiving-notifications
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_subscription_id(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

/// @ingroup mcp
/// Borrow the `arguments` object from a JSON-RPC `tools/call` envelope. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/mcp.h>
/// #include <cassert>
///
/// const auto envelope{sourcemeta::core::parse_json(R"JSON({
///   "jsonrpc": "2.0", "id": 1, "method": "tools/call",
///   "params": { "name": "tool", "arguments": { "count": 5 } }
/// })JSON")};
/// const auto *args{sourcemeta::core::mcp_tool_call_arguments(envelope)};
/// assert(args != nullptr);
/// assert(args->at("count").to_integer() == 5);
/// ```
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_tool_call_arguments(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

} // namespace sourcemeta::core

#endif
