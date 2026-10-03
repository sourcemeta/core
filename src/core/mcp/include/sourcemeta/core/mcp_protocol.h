#ifndef SOURCEMETA_CORE_MCP_PROTOCOL_H_
#define SOURCEMETA_CORE_MCP_PROTOCOL_H_

#ifndef SOURCEMETA_CORE_MCP_EXPORT
#include <sourcemeta/core/mcp_export.h>
#endif

#include <sourcemeta/core/json.h>

#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

namespace sourcemeta::core {

/// @ingroup mcp
/// The supported MCP protocol revisions.
///
/// MCP 2026-07-28 is a major architectural redesign replacing stateful
/// initialization with stateless, per-request capability negotiation, removing
/// `initialize`, `ping`, and `logging/setLevel`, and introducing
/// `server/discover`, `subscriptions/listen`, explicit result types, and
/// caching policies.
///
/// Legacy revisions (2024-11-05 through 2025-11-25) use connection-scoped
/// initialization handshakes and stateful protocol sessions.
///
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/basic/index/#meta
enum class MCPProtocolVersion : std::uint8_t {
  /// Protocol revision 2025-03-26.
  V_2025_03_26,
  /// Protocol revision 2025-06-18.
  V_2025_06_18,
  /// The latest initialization-handshake revision (2025-11-25).
  V_2025_11_25,
  /// The stateless, per-request capability negotiation revision (2026-07-28).
  V_2026_07_28,
};

/// @ingroup mcp
/// Wire string constant for MCP protocol version `2025-03-26`.
constexpr JSON::StringView MCP_PROTOCOL_VERSION_2025_03_26{"2025-03-26"};

/// @ingroup mcp
/// Wire string constant for MCP protocol version `2025-06-18`.
constexpr JSON::StringView MCP_PROTOCOL_VERSION_2025_06_18{"2025-06-18"};

/// @ingroup mcp
/// Wire string constant for MCP protocol version `2025-11-25`.
constexpr JSON::StringView MCP_PROTOCOL_VERSION_2025_11_25{"2025-11-25"};

/// @ingroup mcp
/// Wire string constant for MCP protocol version `2026-07-28`.
constexpr JSON::StringView MCP_PROTOCOL_VERSION_2026_07_28{"2026-07-28"};

/// @ingroup mcp
/// Wire string for the initialization handshake request method.
constexpr JSON::StringView MCP_METHOD_INITIALIZE{"initialize"};

/// @ingroup mcp
/// Wire string for the initialization confirmation notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_INITIALIZED{
    "notifications/initialized"};

/// @ingroup mcp
/// Wire string for the connection liveness check method.
constexpr JSON::StringView MCP_METHOD_PING{"ping"};

/// @ingroup mcp
/// Wire string for the modern server discovery request method.
constexpr JSON::StringView MCP_METHOD_SERVER_DISCOVER{"server/discover"};

/// @ingroup mcp
/// Wire string for the modern subscriptions listen request method.
constexpr JSON::StringView MCP_METHOD_SUBSCRIPTIONS_LISTEN{
    "subscriptions/listen"};

/// @ingroup mcp
/// Wire string for the modern subscriptions acknowledged notification.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED{
    "notifications/subscriptions/acknowledged"};

/// @ingroup mcp
/// Wire string for the tools list request method.
constexpr JSON::StringView MCP_METHOD_TOOLS_LIST{"tools/list"};

/// @ingroup mcp
/// Wire string for the tool call request method.
constexpr JSON::StringView MCP_METHOD_TOOLS_CALL{"tools/call"};

/// @ingroup mcp
/// Wire string for the resources list request method.
constexpr JSON::StringView MCP_METHOD_RESOURCES_LIST{"resources/list"};

/// @ingroup mcp
/// Wire string for the resource read request method.
constexpr JSON::StringView MCP_METHOD_RESOURCES_READ{"resources/read"};

/// @ingroup mcp
/// Wire string for the resource subscribe request method.
constexpr JSON::StringView MCP_METHOD_RESOURCES_SUBSCRIBE{
    "resources/subscribe"};

/// @ingroup mcp
/// Wire string for the resource unsubscribe request method.
constexpr JSON::StringView MCP_METHOD_RESOURCES_UNSUBSCRIBE{
    "resources/unsubscribe"};

/// @ingroup mcp
/// Wire string for the resource templates list request method.
constexpr JSON::StringView MCP_METHOD_RESOURCES_TEMPLATES_LIST{
    "resources/templates/list"};

/// @ingroup mcp
/// Wire string for the prompts list request method.
constexpr JSON::StringView MCP_METHOD_PROMPTS_LIST{"prompts/list"};

/// @ingroup mcp
/// Wire string for the prompt get request method.
constexpr JSON::StringView MCP_METHOD_PROMPTS_GET{"prompts/get"};

/// @ingroup mcp
/// Wire string for the completion request method.
constexpr JSON::StringView MCP_METHOD_COMPLETION_COMPLETE{
    "completion/complete"};

/// @ingroup mcp
/// Wire string for the legacy logging set-level request method.
constexpr JSON::StringView MCP_METHOD_LOGGING_SET_LEVEL{"logging/setLevel"};

/// @ingroup mcp
/// Wire string for the cancelled notification method.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_CANCELLED{
    "notifications/cancelled"};

/// @ingroup mcp
/// Wire string for the message notification method.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_MESSAGE{
    "notifications/message"};

/// @ingroup mcp
/// Wire string for the progress notification method.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_PROGRESS{
    "notifications/progress"};

/// @ingroup mcp
/// Wire string for the prompts list changed notification method.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_PROMPTS_LIST_CHANGED{
    "notifications/prompts/list_changed"};

/// @ingroup mcp
/// Wire string for the resources list changed notification method.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_RESOURCES_LIST_CHANGED{
    "notifications/resources/list_changed"};

/// @ingroup mcp
/// Wire string for the resources updated notification method.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_RESOURCES_UPDATED{
    "notifications/resources/updated"};

/// @ingroup mcp
/// Wire string for the tools list changed notification method.
constexpr JSON::StringView MCP_METHOD_NOTIFICATIONS_TOOLS_LIST_CHANGED{
    "notifications/tools/list_changed"};

/// @ingroup mcp
/// Wire string for the roots list request method.
constexpr JSON::StringView MCP_METHOD_ROOTS_LIST{"roots/list"};

/// @ingroup mcp
/// Wire string for the sampling create-message request method.
constexpr JSON::StringView MCP_METHOD_SAMPLING_CREATE_MESSAGE{
    "sampling/createMessage"};

/// @ingroup mcp
/// Wire string for the elicitation create request method.
constexpr JSON::StringView MCP_METHOD_ELICITATION_CREATE{"elicitation/create"};

/// @ingroup mcp
/// Convert an @ref MCPProtocolVersion enum value to its canonical string
/// representation.
constexpr auto
mcp_protocol_version_string(const MCPProtocolVersion version) noexcept
    -> JSON::StringView {
  switch (version) {
    case MCPProtocolVersion::V_2025_03_26:
      return MCP_PROTOCOL_VERSION_2025_03_26;
    case MCPProtocolVersion::V_2025_06_18:
      return MCP_PROTOCOL_VERSION_2025_06_18;
    case MCPProtocolVersion::V_2025_11_25:
      return MCP_PROTOCOL_VERSION_2025_11_25;
    case MCPProtocolVersion::V_2026_07_28:
      return MCP_PROTOCOL_VERSION_2026_07_28;
  }
  std::unreachable();
}

/// @ingroup mcp
/// Resolve an MCP protocol version wire string to its corresponding enum value,
/// or `std::nullopt` when the string does not match any recognized version.
constexpr auto
mcp_resolve_protocol_version(const JSON::StringView value) noexcept
    -> std::optional<MCPProtocolVersion> {
  if (value == MCP_PROTOCOL_VERSION_2026_07_28) {
    return MCPProtocolVersion::V_2026_07_28;
  }
  if (value == MCP_PROTOCOL_VERSION_2025_11_25) {
    return MCPProtocolVersion::V_2025_11_25;
  }
  if (value == MCP_PROTOCOL_VERSION_2025_06_18) {
    return MCPProtocolVersion::V_2025_06_18;
  }
  if (value == MCP_PROTOCOL_VERSION_2025_03_26) {
    return MCPProtocolVersion::V_2025_03_26;
  }
  return std::nullopt;
}

/// @ingroup mcp
/// Check whether an MCP protocol revision is at least the given minimum
/// version.
constexpr auto
mcp_protocol_version_at_least(const MCPProtocolVersion current,
                              const MCPProtocolVersion minimum) noexcept
    -> bool {
  return std::to_underlying(current) >= std::to_underlying(minimum);
}

/// @ingroup mcp
/// Check whether the given wire string is a valid, recognized MCP protocol
/// version.
constexpr auto
mcp_protocol_version_is_valid(const JSON::StringView value) noexcept -> bool {
  return mcp_resolve_protocol_version(value).has_value();
}

/// @ingroup mcp
/// Whether the specified MCP protocol version uses an initialization handshake
/// (`initialize` / `notifications/initialized`).
constexpr auto
mcp_uses_initialization_handshake(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports the `ping` method.
constexpr auto mcp_supports_ping(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether requests in the specified protocol version must carry metadata
/// inside `params._meta`.
constexpr auto
mcp_requires_request_meta(const MCPProtocolVersion version) noexcept -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether result responses in the specified protocol version must carry an
/// explicit `resultType` field.
constexpr auto
mcp_requires_result_type(const MCPProtocolVersion version) noexcept -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether cacheable responses in the specified protocol version must include
/// `ttlMs` and `cacheScope`.
constexpr auto
mcp_requires_cacheable_metadata(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports the modern
/// `server/discover` method.
constexpr auto
mcp_supports_server_discover(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports modern server-side
/// subscriptions via `subscriptions/listen`.
constexpr auto
mcp_supports_subscriptions_listen(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports Multi Round-Trip
/// Results (`input_required`).
constexpr auto mcp_supports_mrtr(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// The latest protocol version that uses initialization handshakes.
constexpr auto mcp_latest_initialization_version() noexcept
    -> MCPProtocolVersion {
  return MCPProtocolVersion::V_2025_11_25;
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports stateful protocol
/// sessions.
constexpr auto
mcp_supports_protocol_sessions(const MCPProtocolVersion version) noexcept
    -> bool {
  return version != MCPProtocolVersion::V_2026_07_28;
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports output schema on tool
/// definitions.
constexpr auto
mcp_supports_output_schema(const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports structured content.
constexpr auto
mcp_supports_structured_content(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports resource link content.
constexpr auto
mcp_supports_resource_link_content(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports implementation title.
constexpr auto
mcp_supports_implementation_title(const MCPProtocolVersion version) noexcept
    -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_06_18);
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports implementation
/// description.
constexpr auto mcp_supports_implementation_description(
    const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_11_25);
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports implementation
/// website URL.
constexpr auto mcp_supports_implementation_website_url(
    const MCPProtocolVersion version) noexcept -> bool {
  return mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_11_25);
}

/// @ingroup mcp
/// Whether the specified MCP protocol version supports JSON-RPC batching.
constexpr auto
mcp_supports_jsonrpc_batching(const MCPProtocolVersion version) noexcept
    -> bool {
  return version == MCPProtocolVersion::V_2025_03_26;
}

/// @ingroup mcp
/// Whether the given string is a valid, version-appropriate MCP request method
/// for the specified protocol version.
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
         method == MCP_METHOD_LOGGING_SET_LEVEL ||
         method == MCP_METHOD_RESOURCES_SUBSCRIBE ||
         method == MCP_METHOD_RESOURCES_UNSUBSCRIBE;
}

/// @ingroup mcp
/// Check whether the given method name is supported in the specified protocol
/// version.
constexpr auto mcp_supports_method(const MCPProtocolVersion version,
                                   const JSON::StringView method) noexcept
    -> bool {
  if (version == MCPProtocolVersion::V_2026_07_28) {
    return method == MCP_METHOD_SERVER_DISCOVER ||
           method == MCP_METHOD_SUBSCRIPTIONS_LISTEN ||
           method == MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED ||
           method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
           method == MCP_METHOD_RESOURCES_LIST ||
           method == MCP_METHOD_RESOURCES_READ ||
           method == MCP_METHOD_RESOURCES_TEMPLATES_LIST ||
           method == MCP_METHOD_PROMPTS_LIST ||
           method == MCP_METHOD_PROMPTS_GET ||
           method == MCP_METHOD_COMPLETION_COMPLETE ||
           method == MCP_METHOD_NOTIFICATIONS_CANCELLED ||
           method == MCP_METHOD_NOTIFICATIONS_MESSAGE ||
           method == MCP_METHOD_NOTIFICATIONS_PROGRESS ||
           method == MCP_METHOD_NOTIFICATIONS_PROMPTS_LIST_CHANGED ||
           method == MCP_METHOD_NOTIFICATIONS_RESOURCES_LIST_CHANGED ||
           method == MCP_METHOD_NOTIFICATIONS_RESOURCES_UPDATED ||
           method == MCP_METHOD_NOTIFICATIONS_TOOLS_LIST_CHANGED ||
           method == MCP_METHOD_ROOTS_LIST ||
           method == MCP_METHOD_SAMPLING_CREATE_MESSAGE ||
           method == MCP_METHOD_ELICITATION_CREATE;
  }

  return method == MCP_METHOD_INITIALIZE || method == MCP_METHOD_PING ||
         method == MCP_METHOD_NOTIFICATIONS_INITIALIZED ||
         method == MCP_METHOD_TOOLS_LIST || method == MCP_METHOD_TOOLS_CALL ||
         method == MCP_METHOD_RESOURCES_LIST ||
         method == MCP_METHOD_RESOURCES_READ ||
         method == MCP_METHOD_RESOURCES_TEMPLATES_LIST ||
         method == MCP_METHOD_PROMPTS_LIST ||
         method == MCP_METHOD_PROMPTS_GET ||
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
         method == MCP_METHOD_ELICITATION_CREATE ||
         method == MCP_METHOD_RESOURCES_SUBSCRIBE ||
         method == MCP_METHOD_RESOURCES_UNSUBSCRIBE;
}

/// @ingroup mcp
/// Check whether an MCP request method expects a target name/URI identifier.
constexpr auto
mcp_is_named_request_method(const JSON::StringView method) noexcept -> bool {
  return method == MCP_METHOD_TOOLS_CALL ||
         method == MCP_METHOD_RESOURCES_READ ||
         method == MCP_METHOD_PROMPTS_GET ||
         method == MCP_METHOD_RESOURCES_SUBSCRIBE ||
         method == MCP_METHOD_RESOURCES_UNSUBSCRIBE;
}

} // namespace sourcemeta::core

#endif
