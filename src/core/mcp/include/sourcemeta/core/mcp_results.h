#ifndef SOURCEMETA_CORE_MCP_RESULTS_H_
#define SOURCEMETA_CORE_MCP_RESULTS_H_

#ifndef SOURCEMETA_CORE_MCP_EXPORT
#include <sourcemeta/core/mcp_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/mcp_capabilities.h>
#include <sourcemeta/core/mcp_headers.h>
#include <sourcemeta/core/mcp_protocol.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>
#include <span>
#include <utility>
#include <vector>

namespace sourcemeta::core {

/// @ingroup mcp
/// Cache scope for 2026-07-28 cacheable MCP results.
///
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching#cacheable-results
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
constexpr auto mcp_resolve_cache_scope(const JSON::StringView scope) noexcept
    -> std::optional<MCPCacheScope> {
  if (scope == "public") {
    return MCPCacheScope::Public;
  }
  if (scope == "private") {
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
/// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching#cacheable-results
struct MCPCachePolicy {
  /// Time-to-live in milliseconds. Must be non-negative.
  std::int64_t ttl_ms = 0;
  /// Cache scope (public or private).
  MCPCacheScope scope = MCPCacheScope::Private;
};

/// @ingroup mcp
/// Optional hints attached to an MCP tool descriptor.
struct MCPToolAnnotations {
  /// Optional human-readable title for the tool.
  JSON::StringView title = {};
  /// `true` when the tool guarantees no side effects.
  bool read_only = false;
  /// `true` when the tool may mutate or delete state.
  bool destructive = true;
  /// `true` when repeated invocations with the same arguments have no
  /// additional effect on the environment.
  bool idempotent = false;
  /// `true` when the tool interacts with state outside the server's control.
  bool open_world = true;
};

/// @ingroup mcp
/// Build an MCP `text` content block carrying the given text payload.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_text_block(const JSON::StringView text) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP content block referencing a resource by URI.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_link(const MCPProtocolVersion version,
                            const JSON::StringView uri,
                            const JSON::StringView name,
                            const JSON::StringView mime_type = {},
                            const JSON::StringView description = {})
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Decorate a successful MCP result object with version-appropriate envelope
/// fields (`resultType: "complete"` and optional
/// `_meta.io.modelcontextprotocol/serverInfo` for 2026-07-28).
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_decorate_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON result,
    const std::optional<MCPImplementation> &server_info = std::nullopt)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Decorate an existing MCP result object in-place without copying.
SOURCEMETA_CORE_MCP_EXPORT
void mcp_decorate_result_in_place(
    const MCPProtocolVersion version, sourcemeta::core::JSON &result,
    const std::optional<MCPImplementation> &server_info = std::nullopt);

/// @ingroup mcp
/// Decorate an MCP result object with cache metadata (`ttlMs` and
/// `cacheScope`) for 2026-07-28.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_decorate_cacheable_result(const MCPProtocolVersion version,
                                   sourcemeta::core::JSON result,
                                   const MCPCachePolicy &cache_policy,
                                   const JSON::StringView method)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Decorate an existing MCP result object with cache metadata in-place.
SOURCEMETA_CORE_MCP_EXPORT
void mcp_decorate_cacheable_result_in_place(const MCPProtocolVersion version,
                                            sourcemeta::core::JSON &result,
                                            const MCPCachePolicy &cache_policy,
                                            const JSON::StringView method);

/// @ingroup mcp
/// Build an empty MCP result response envelope with `resultType: "complete"`.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_empty_result(
    const MCPProtocolVersion version, const sourcemeta::core::JSON &identifier,
    const std::optional<MCPImplementation> &server_info = std::nullopt)
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
/// result envelope with a required cache policy for 2026-07-28.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_read_result(const MCPProtocolVersion version,
                                    sourcemeta::core::JSON contents,
                                    const MCPCachePolicy &cache_policy)
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
/// Build a single entry for an MCP `tools/list` response.
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/schema#tool
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tool_descriptor(
    const MCPProtocolVersion version, const JSON::StringView name,
    const JSON::StringView description, sourcemeta::core::JSON input_schema,
    std::optional<sourcemeta::core::JSON> output_schema = std::nullopt,
    const MCPToolAnnotations &annotations = {}) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `tools/list` result object for 2026-07-28 with required cache
/// metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_tools_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON tools,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/list` result object for 2026-07-28 with required
/// cache metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resources_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resources,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `resources/templates/list` result object for 2026-07-28 with
/// required cache metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_resource_templates_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resource_templates,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP `prompts/list` result object for 2026-07-28 with required cache
/// metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_prompts_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON prompts,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a prompt result from checked role/content messages.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_prompts_get_result(MCPProtocolVersion version,
                                 JSON::StringView description, JSON messages)
    -> JSON;

/// @ingroup mcp
/// Build a completion result. Values must be strings and contain at most 100
/// entries; a supplied total cannot be smaller than the returned entry count.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_completion_result(MCPProtocolVersion version, JSON values,
                                std::optional<std::int64_t> total,
                                std::optional<bool> has_more) -> JSON;

/// @ingroup mcp
/// Write a complete MCP response while borrowing its precomputed result.
/// Applicable to the six cacheable operations. Large entry arrays are never
/// copied. Existing metadata and the source result remain unchanged.
/// A cache policy is mandatory for 2026-07-28; omit it for legacy responses.
/// This writes JSON only; stdio callers must append a newline themselves.
SOURCEMETA_CORE_MCP_EXPORT
void mcp_write_result(
    std::ostream &stream, MCPProtocolVersion version, JSON::StringView method,
    const JSON &identifier, const JSON &result,
    std::optional<MCPCachePolicy> cache_policy,
    std::optional<MCPImplementation> server_info = std::nullopt);

/// @ingroup mcp
/// Build the JSON-RPC envelope returned in response to an MCP `initialize`
/// request.
/// @see
/// https://modelcontextprotocol.io/specification/2025-11-25/basic/lifecycle#initialization
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_initialize_result(
    const sourcemeta::core::JSON &request,
    const MCPServerCapabilities &capabilities, const MCPImplementation &server,
    const std::span<const JSON::StringView> supported_versions,
    const JSON::StringView instructions = {}) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build the JSON-RPC response envelope for an MCP 2026-07-28 `server/discover`
/// request.
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/schema#discoverresult
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_server_discover_result(
    const MCPProtocolVersion version, const sourcemeta::core::JSON &identifier,
    const MCPServerCapabilities &capabilities, const MCPImplementation &server,
    const std::span<const JSON::StringView> supported_versions,
    const JSON::StringView instructions, const MCPCachePolicy &cache_policy)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an MCP Multi Round-Trip Result (`input_required`). At least one of
/// `input_requests` or `request_state` must be provided. Only `tools/call`,
/// `resources/read`, and `prompts/get` may carry this result.
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#supported-requests
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_input_required_result(
    const MCPProtocolVersion version, const JSON::StringView method,
    const sourcemeta::core::JSON &identifier,
    std::optional<sourcemeta::core::JSON> input_requests,
    std::optional<JSON::StringView> request_state,
    const MCPClientCapabilities &client_capabilities) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Access `inputResponses` from an MRTR continuation request envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_input_responses(MCPProtocolVersion version,
                                 const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

/// @ingroup mcp
/// Access `requestState` token from an MRTR continuation request envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_state(MCPProtocolVersion version,
                       const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Opt-in filters for a 2026-07-28 subscription. Explicit false values are
/// preserved; an absent value is not requested. Resource URI views are
/// borrowed.
struct MCPSubscriptionFilter {
  /// Opt in to notifications that the tool list changed.
  std::optional<bool> tools_list_changed = std::nullopt;
  /// Opt in to notifications that the prompt list changed.
  std::optional<bool> prompts_list_changed = std::nullopt;
  /// Opt in to notifications that the resource list changed.
  std::optional<bool> resources_list_changed = std::nullopt;
  /// Borrowed resource URIs whose updates are requested. An absent list
  /// requests no subscriptions, while an explicit empty list stays explicit.
  std::optional<std::vector<JSON::StringView>> resource_subscriptions =
      std::nullopt;
};

/// @ingroup mcp
/// Parse the filter in checked 2026-07-28 listen parameters. String views
/// borrow the parameters, while the optional URI vector owns only those views.
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_parse_subscription_filter(MCPProtocolVersion version,
                                   const JSON &parameters)
    -> std::optional<MCPSubscriptionFilter>;

/// @ingroup mcp
/// Restrict requested events and URIs to those the server supports.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_intersect_subscription_filters(const MCPSubscriptionFilter &requested,
                                        const MCPSubscriptionFilter &supported)
    -> MCPSubscriptionFilter;

/// @ingroup mcp
/// Read a result discriminator. Absent resultType means complete on incoming
/// legacy results; unknown string discriminators remain available to
/// extensions.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_result_type(const JSON &result) -> std::optional<JSON::StringView>;

/// @ingroup mcp
/// Check continuation field shapes, exact state echo when expected, and known
/// response kinds against the prior input requests. Unknown response keys are
/// ignored. Core does not validate the user's form values against arbitrary
/// JSON Schema, protect state integrity, or enforce new request IDs/history.
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#client-requirements-basic-workflow
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_validate_continuation(MCPProtocolVersion version,
                               const JSON &parameters,
                               const JSON &input_requests,
                               std::optional<JSON::StringView> expected_state)
    -> bool;

/// @ingroup mcp
/// Construct a checked notification. The direction and parameters are explicit.
/// 2026-07-28 logging/progress require the current validated request context.
/// For 2026-07-28 server list/resource updates, subscription_id and the already
/// acknowledged filter are mandatory. A server cancellation must name that
/// subscription. Runtime code owns acknowledgment ordering, token activity,
/// increasing progress, authorization and stream lifetime.
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_notification(MCPProtocolVersion version, bool from_server,
                           JSON::StringView method, JSON parameters,
                           std::optional<JSON> subscription_id,
                           const MCPSubscriptionFilter *acknowledged = nullptr,
                           const MCPRequestMeta *request_context = nullptr)
    -> JSON;

/// @ingroup mcp
/// Build an acknowledgement notification for `subscriptions/listen` with a
/// string or integer subscription ID. The filter is restricted to the
/// intersection of requested and server-supported events and URIs.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_subscription_acknowledged_notification(
    const MCPProtocolVersion version,
    const sourcemeta::core::JSON &subscription_id,
    const MCPSubscriptionFilter &requested,
    const MCPSubscriptionFilter &supported) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build an acknowledgement notification for `subscriptions/listen` with a
/// string subscription ID.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_subscription_acknowledged_notification(
    const MCPProtocolVersion version, const JSON::StringView subscription_id,
    const MCPSubscriptionFilter &requested,
    const MCPSubscriptionFilter &supported) -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Build a graceful close response for `subscriptions/listen`.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_make_subscription_close_result(
    const MCPProtocolVersion version, const sourcemeta::core::JSON &identifier)
    -> sourcemeta::core::JSON;

/// @ingroup mcp
/// Access `io.modelcontextprotocol/subscriptionId` from request or result
/// metadata.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_request_subscription_id(MCPProtocolVersion version,
                                 const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

/// @ingroup mcp
/// Borrow the `arguments` object from a JSON-RPC `tools/call` envelope.
SOURCEMETA_CORE_MCP_EXPORT
auto mcp_tool_call_arguments(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON *;

} // namespace sourcemeta::core

#endif
