#ifndef SOURCEMETA_CORE_MCP_CAPABILITIES_H_
#define SOURCEMETA_CORE_MCP_CAPABILITIES_H_

#ifndef SOURCEMETA_CORE_MCP_EXPORT
#include <sourcemeta/core/mcp_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/mcp_protocol.h>

#include <optional>

namespace sourcemeta::core {

/// @ingroup mcp
/// Information about an MCP implementation (client or server).
/// @see
/// https://modelcontextprotocol.io/specification/2025-11-25/schema#implementation
struct MCPImplementation {
  /// Implementation identifier name.
  JSON::StringView name;
  /// Implementation version (not required to be SemVer).
  JSON::StringView version;
  /// Optional human-readable title.
  JSON::StringView title = {};
  /// Optional human-readable description.
  JSON::StringView description = {};
  /// Optional public website URL.
  JSON::StringView website_url = {};
  /// Optional borrowed array of icons (2025-11-25 and later).
  const JSON *icons = nullptr;
};

/// @ingroup mcp
/// Capabilities advertised by an MCP client.
///
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/schema#clientcapabilities
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
  /// Optional owned source capabilities for lossless parsing/serialization.
  /// Keeps extension settings, unknown capabilities and 2025-11-25 task data.
  /// Standard flags are applied over this snapshot when serializing; the
  /// per-request metadata view does not allocate or populate this snapshot.
  std::optional<JSON> source = std::nullopt;
};

/// @ingroup mcp
/// Capabilities advertised by an MCP server.
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/schema#servercapabilities
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
/// Parse client capabilities from a JSON object.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_parse_client_capabilities(const MCPProtocolVersion version,
                                   const sourcemeta::core::JSON &capabilities)
    -> MCPClientCapabilities;

/// @ingroup mcp
/// Serialize client capabilities into a JSON object for the specified protocol
/// version.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_serialize_client_capabilities(
    const MCPProtocolVersion version, const MCPClientCapabilities &capabilities)
    -> sourcemeta::core::JSON;

} // namespace sourcemeta::core

#endif
