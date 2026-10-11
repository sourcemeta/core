#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/json.h>

#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(protocol_version_constant_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::MCP_PROTOCOL_VERSION_2025_03_26, "2025-03-26");
}

TEST(protocol_version_constant_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::MCP_PROTOCOL_VERSION_2025_06_18, "2025-06-18");
}

TEST(protocol_version_constant_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::MCP_PROTOCOL_VERSION_2025_11_25, "2025-11-25");
}

TEST(protocol_version_constant_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::MCP_PROTOCOL_VERSION_2026_07_28, "2026-07-28");
}

TEST(protocol_version_string_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_protocol_version_string(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26),
            "2025-03-26");
}

TEST(protocol_version_string_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_protocol_version_string(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18),
            "2025-06-18");
}

TEST(protocol_version_string_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_protocol_version_string(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25),
            "2025-11-25");
}

TEST(protocol_version_string_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_protocol_version_string(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28),
            "2026-07-28");
}

TEST(resolve_protocol_version_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_protocol_version("2025-03-26"),
            sourcemeta::core::MCPProtocolVersion::V_2025_03_26);
}

TEST(resolve_protocol_version_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_protocol_version("2025-06-18"),
            sourcemeta::core::MCPProtocolVersion::V_2025_06_18);
}

TEST(resolve_protocol_version_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_protocol_version("2025-11-25"),
            sourcemeta::core::MCPProtocolVersion::V_2025_11_25);
}

TEST(resolve_protocol_version_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_protocol_version("2026-07-28"),
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
}

// Resolution does not guess, so a caller that wants the pre-header default of
// 2025-03-26 opts into it explicitly
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#protocol-version-header
TEST(resolve_protocol_version_rejects_an_empty_string) {
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_protocol_version("").has_value());
}

TEST(resolve_protocol_version_rejects_an_unknown_revision) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("1999-01-01").has_value());
}

TEST(resolve_protocol_version_rejects_a_malformed_revision) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("2026/07/28").has_value());
}

TEST(resolve_protocol_version_rejects_a_padded_revision) {
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_protocol_version(" 2026-07-28 ")
                   .has_value());
}

TEST(resolve_protocol_version_rejects_a_prefix) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("2026-07").has_value());
}

TEST(resolve_protocol_version_rejects_the_draft_revision) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("draft").has_value());
}

TEST(resolve_protocol_version_rejects_the_first_revision) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("2024-11-05").has_value());
}

TEST(resolve_protocol_version_of_a_runtime_string) {
  const std::string value{"2026-07-28"};
  EXPECT_EQ(sourcemeta::core::mcp_resolve_protocol_version(value),
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
}

TEST(protocol_version_is_valid_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_is_valid("2025-03-26"));
}

TEST(protocol_version_is_valid_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_is_valid("2025-06-18"));
}

TEST(protocol_version_is_valid_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_is_valid("2025-11-25"));
}

TEST(protocol_version_is_valid_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_is_valid("2026-07-28"));
}

TEST(protocol_version_is_valid_rejects_an_unknown_revision) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_is_valid("1999-01-01"));
}

TEST(protocol_version_is_valid_rejects_an_empty_string) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_is_valid(""));
}

TEST(protocol_version_2025_03_26_is_at_least_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(protocol_version_2025_03_26_is_at_least_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(protocol_version_2025_03_26_is_at_least_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(protocol_version_2025_03_26_is_at_least_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

TEST(protocol_version_2025_06_18_is_at_least_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(protocol_version_2025_06_18_is_at_least_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(protocol_version_2025_06_18_is_at_least_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(protocol_version_2025_06_18_is_at_least_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

TEST(protocol_version_2025_11_25_is_at_least_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(protocol_version_2025_11_25_is_at_least_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(protocol_version_2025_11_25_is_at_least_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(protocol_version_2025_11_25_is_at_least_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

TEST(protocol_version_2026_07_28_is_at_least_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(protocol_version_2026_07_28_is_at_least_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(protocol_version_2026_07_28_is_at_least_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(protocol_version_2026_07_28_is_at_least_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

TEST(method_constant_method_completion_complete) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_COMPLETION_COMPLETE,
            "completion/complete");
}

TEST(method_constant_method_elicitation_create) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_ELICITATION_CREATE,
            "elicitation/create");
}

TEST(method_constant_method_initialize) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_INITIALIZE, "initialize");
}

TEST(method_constant_method_logging_set_level) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_LOGGING_SET_LEVEL, "logging/setLevel");
}

TEST(method_constant_method_notifications_cancelled) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_CANCELLED,
            "notifications/cancelled");
}

TEST(method_constant_method_notifications_elicitation_complete) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_ELICITATION_COMPLETE,
            "notifications/elicitation/complete");
}

TEST(method_constant_method_notifications_initialized) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_INITIALIZED,
            "notifications/initialized");
}

TEST(method_constant_method_notifications_message) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_MESSAGE,
            "notifications/message");
}

TEST(method_constant_method_notifications_progress) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_PROGRESS,
            "notifications/progress");
}

TEST(method_constant_method_notifications_prompts_list_changed) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_PROMPTS_LIST_CHANGED,
            "notifications/prompts/list_changed");
}

TEST(method_constant_method_notifications_resources_list_changed) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_RESOURCES_LIST_CHANGED,
            "notifications/resources/list_changed");
}

TEST(method_constant_method_notifications_resources_updated) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_RESOURCES_UPDATED,
            "notifications/resources/updated");
}

TEST(method_constant_method_notifications_roots_list_changed) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_ROOTS_LIST_CHANGED,
            "notifications/roots/list_changed");
}

TEST(method_constant_method_notifications_subscriptions_acknowledged) {
  EXPECT_EQ(
      sourcemeta::core::MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED,
      "notifications/subscriptions/acknowledged");
}

TEST(method_constant_method_notifications_tasks_status) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_TASKS_STATUS,
            "notifications/tasks/status");
}

TEST(method_constant_method_notifications_tools_list_changed) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_TOOLS_LIST_CHANGED,
            "notifications/tools/list_changed");
}

TEST(method_constant_method_ping) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_PING, "ping");
}

TEST(method_constant_method_prompts_get) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_PROMPTS_GET, "prompts/get");
}

TEST(method_constant_method_prompts_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_PROMPTS_LIST, "prompts/list");
}

TEST(method_constant_method_resources_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_LIST, "resources/list");
}

TEST(method_constant_method_resources_read) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_READ, "resources/read");
}

TEST(method_constant_method_resources_subscribe) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_SUBSCRIBE,
            "resources/subscribe");
}

TEST(method_constant_method_resources_templates_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST,
            "resources/templates/list");
}

TEST(method_constant_method_resources_unsubscribe) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_UNSUBSCRIBE,
            "resources/unsubscribe");
}

TEST(method_constant_method_roots_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_ROOTS_LIST, "roots/list");
}

TEST(method_constant_method_sampling_create_message) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_SAMPLING_CREATE_MESSAGE,
            "sampling/createMessage");
}

TEST(method_constant_method_server_discover) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_SERVER_DISCOVER, "server/discover");
}

TEST(method_constant_method_subscriptions_listen) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_SUBSCRIPTIONS_LISTEN,
            "subscriptions/listen");
}

TEST(method_constant_method_tasks_cancel) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TASKS_CANCEL, "tasks/cancel");
}

TEST(method_constant_method_tasks_get) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TASKS_GET, "tasks/get");
}

TEST(method_constant_method_tasks_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TASKS_LIST, "tasks/list");
}

TEST(method_constant_method_tasks_result) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TASKS_RESULT, "tasks/result");
}

TEST(method_constant_method_tools_call) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TOOLS_CALL, "tools/call");
}

TEST(method_constant_method_tools_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TOOLS_LIST, "tools/list");
}

// 2026-07-28 dropped the initialize handshake for per-request metadata
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#statelessness
TEST(uses_initialization_handshake_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_uses_initialization_handshake(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(uses_initialization_handshake_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_uses_initialization_handshake(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(uses_initialization_handshake_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_uses_initialization_handshake(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(uses_initialization_handshake_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_uses_initialization_handshake(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// The liveness check is gone from 2026-07-28, which has no session to keep
// alive
// https://modelcontextprotocol.io/specification/2026-07-28/changelog
TEST(supports_ping_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_ping(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_ping_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_ping(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_ping_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_ping(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_ping_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_ping(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// 2026-07-28 is a stateless protocol, so a connection is not a session
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#statelessness
TEST(supports_protocol_sessions_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_protocol_sessions(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_protocol_sessions_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_protocol_sessions(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_protocol_sessions_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_protocol_sessions(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_protocol_sessions_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_protocol_sessions(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// 2026-07-28 carries the protocol version and client capabilities on every
// request instead of negotiating them once
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(requires_request_meta_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(requires_request_meta_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(requires_request_meta_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(requires_request_meta_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_requires_request_meta(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// A 2026-07-28 result says what kind of result it is
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#resulttype
TEST(requires_result_type_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_result_type(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(requires_result_type_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_result_type(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(requires_result_type_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_result_type(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(requires_result_type_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_requires_result_type(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// 2026-07-28 added caching hints to the six cacheable operations
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching#cacheable-results
TEST(requires_cacheable_metadata_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_cacheable_metadata(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(requires_cacheable_metadata_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_cacheable_metadata(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(requires_cacheable_metadata_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_requires_cacheable_metadata(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(requires_cacheable_metadata_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_requires_cacheable_metadata(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Discovery replaced initialize as the way to learn what a server offers
// https://modelcontextprotocol.io/specification/2026-07-28/server/discover
TEST(supports_server_discover_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_server_discover(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_server_discover_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_server_discover(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_server_discover_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_server_discover(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_server_discover_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_server_discover(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Listening replaced the resource subscribe request and the HTTP GET stream
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
TEST(supports_subscriptions_listen_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_subscriptions_listen(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_subscriptions_listen_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_subscriptions_listen(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_subscriptions_listen_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_subscriptions_listen(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_subscriptions_listen_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_subscriptions_listen(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Multi round-trip requests replaced server-initiated JSON-RPC requests
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr
TEST(supports_mrtr_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_mrtr(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_mrtr_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_mrtr(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_mrtr_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_mrtr(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_mrtr_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_mrtr(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Tools gained an output schema in 2025-06-18
// https://modelcontextprotocol.io/specification/2026-07-28/schema#tool
TEST(supports_output_schema_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_output_schema(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_output_schema_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_output_schema(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_output_schema_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_output_schema(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_output_schema_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_output_schema(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Tool results gained structured content in 2025-06-18
// https://modelcontextprotocol.io/specification/2026-07-28/schema#calltoolresult
TEST(supports_structured_content_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_structured_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_structured_content_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_structured_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_structured_content_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_structured_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_structured_content_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_structured_content(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Resource links became a content block in 2025-06-18
// https://modelcontextprotocol.io/specification/2026-07-28/schema#resourcelink
TEST(supports_resource_link_content_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_resource_link_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_resource_link_content_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_resource_link_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_resource_link_content_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_resource_link_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_resource_link_content_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_resource_link_content(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Implementations gained a display title in 2025-06-18
// https://modelcontextprotocol.io/specification/2026-07-28/schema#implementation
TEST(supports_implementation_title_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_title(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_implementation_title_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_title(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_implementation_title_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_title(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_implementation_title_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_title(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Implementations gained a description in 2025-11-25
// https://modelcontextprotocol.io/specification/2026-07-28/schema#implementation
TEST(supports_implementation_description_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_description(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_implementation_description_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_description(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_implementation_description_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_description(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_implementation_description_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_description(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// Implementations gained a website URL in 2025-11-25
// https://modelcontextprotocol.io/specification/2026-07-28/schema#implementation
TEST(supports_implementation_website_url_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_website_url(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_implementation_website_url_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_website_url(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_implementation_website_url_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_website_url(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_implementation_website_url_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_website_url(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// JSON-RPC batching existed only in 2025-03-26
// https://modelcontextprotocol.io/specification/2026-07-28/changelog
TEST(supports_jsonrpc_batching_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_jsonrpc_batching(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(supports_jsonrpc_batching_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_jsonrpc_batching(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(supports_jsonrpc_batching_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_jsonrpc_batching(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(supports_jsonrpc_batching_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_jsonrpc_batching(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

// The client-to-server requests the revision defines in 2025-03-26
// https://modelcontextprotocol.io/specification/2025-03-26/schema
TEST(is_request_method_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "elicitation/create"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "initialize"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/subscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/templates/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "custom/unknownMethod"));
}

// The client-to-server requests the revision defines in 2025-06-18
// https://modelcontextprotocol.io/specification/2025-06-18/schema
TEST(is_request_method_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "elicitation/create"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "initialize"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/subscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/templates/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "custom/unknownMethod"));
}

// The client-to-server requests the revision defines in 2025-11-25
// https://modelcontextprotocol.io/specification/2025-11-25/schema
TEST(is_request_method_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "elicitation/create"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "initialize"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/subscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "subscriptions/listen"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/cancel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "custom/unknownMethod"));
}

// The client-to-server requests the revision defines in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/schema
TEST(is_request_method_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/subscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "sampling/createMessage"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "custom/unknownMethod"));
}

// The standalone server-to-client requests the revision defines in 2025-03-26
// https://modelcontextprotocol.io/specification/2025-03-26/schema
TEST(is_server_request_method_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/unsubscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "roots/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "custom/unknownMethod"));
}

// The standalone server-to-client requests the revision defines in 2025-06-18
// https://modelcontextprotocol.io/specification/2025-06-18/schema
TEST(is_server_request_method_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "completion/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/unsubscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "roots/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "custom/unknownMethod"));
}

// The standalone server-to-client requests the revision defines in 2025-11-25
// https://modelcontextprotocol.io/specification/2025-11-25/schema
TEST(is_server_request_method_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "completion/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/unsubscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "roots/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "subscriptions/listen"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/cancel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "custom/unknownMethod"));
}

// The standalone server-to-client requests the revision defines in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/schema
TEST(is_server_request_method_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "custom/unknownMethod"));
}

// The notifications a client may send in 2025-03-26
// https://modelcontextprotocol.io/specification/2025-03-26/schema
TEST(is_client_notification_method_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "custom/unknownMethod"));
}

// The notifications a client may send in 2025-06-18
// https://modelcontextprotocol.io/specification/2025-06-18/schema
TEST(is_client_notification_method_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "custom/unknownMethod"));
}

// The notifications a client may send in 2025-11-25
// https://modelcontextprotocol.io/specification/2025-11-25/schema
TEST(is_client_notification_method_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/subscriptions/acknowledged"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "custom/unknownMethod"));
}

// The notifications a client may send in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/schema
TEST(is_client_notification_method_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_client_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "custom/unknownMethod"));
}

// The notifications a server may send in 2025-03-26
// https://modelcontextprotocol.io/specification/2025-03-26/schema
TEST(is_server_notification_method_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "custom/unknownMethod"));
}

// The notifications a server may send in 2025-06-18
// https://modelcontextprotocol.io/specification/2025-06-18/schema
TEST(is_server_notification_method_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "custom/unknownMethod"));
}

// The notifications a server may send in 2025-11-25
// https://modelcontextprotocol.io/specification/2025-11-25/schema
TEST(is_server_notification_method_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/cancelled"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/subscriptions/acknowledged"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "custom/unknownMethod"));
}

// The notifications a server may send in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/schema
TEST(is_server_notification_method_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/roots/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_server_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "custom/unknownMethod"));
}

// The notifications the revision defines in either direction in 2025-03-26
// https://modelcontextprotocol.io/specification/2025-03-26/schema
TEST(is_notification_method_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "custom/unknownMethod"));
}

// The notifications the revision defines in either direction in 2025-06-18
// https://modelcontextprotocol.io/specification/2025-06-18/schema
TEST(is_notification_method_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "custom/unknownMethod"));
}

// The notifications the revision defines in either direction in 2025-11-25
// https://modelcontextprotocol.io/specification/2025-11-25/schema
TEST(is_notification_method_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/cancelled"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/subscriptions/acknowledged"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "custom/unknownMethod"));
}

// The notifications the revision defines in either direction in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/schema
TEST(is_notification_method_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/roots/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_notification_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "custom/unknownMethod"));
}

// The nested requests a server may ask for mid-request in 2025-03-26
// https://modelcontextprotocol.io/specification/2025-03-26/schema
TEST(is_input_request_method_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "custom/unknownMethod"));
}

// The nested requests a server may ask for mid-request in 2025-06-18
// https://modelcontextprotocol.io/specification/2025-06-18/schema
TEST(is_input_request_method_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "custom/unknownMethod"));
}

// The nested requests a server may ask for mid-request in 2025-11-25
// https://modelcontextprotocol.io/specification/2025-11-25/schema
TEST(is_input_request_method_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "custom/unknownMethod"));
}

// The nested requests a server may ask for mid-request in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/schema
TEST(is_input_request_method_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "completion/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/unsubscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "roots/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_input_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "custom/unknownMethod"));
}

// The methods the base protocol defines, nested requests included in 2025-03-26
// https://modelcontextprotocol.io/specification/2025-03-26/schema
TEST(supports_method_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "elicitation/create"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "initialize"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/subscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/templates/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "resources/unsubscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "roots/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "custom/unknownMethod"));
}

// The methods the base protocol defines, nested requests included in 2025-06-18
// https://modelcontextprotocol.io/specification/2025-06-18/schema
TEST(supports_method_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "completion/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "elicitation/create"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "initialize"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/subscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/templates/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "resources/unsubscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "roots/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      "custom/unknownMethod"));
}

// The methods the base protocol defines, nested requests included in 2025-11-25
// https://modelcontextprotocol.io/specification/2025-11-25/schema
TEST(supports_method_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "completion/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "elicitation/create"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "initialize"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/cancelled"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/elicitation/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/resources/updated"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/subscriptions/acknowledged"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/tools/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/subscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/unsubscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "roots/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "subscriptions/listen"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/cancel"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "custom/unknownMethod"));
}

// The methods the base protocol defines, nested requests included in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/schema
TEST(supports_method_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "completion/complete"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/message"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/progress"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/prompts/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/roots/list_changed"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tasks/status"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/subscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/unsubscribe"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "roots/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "sampling/createMessage"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "server/discover"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "custom/unknownMethod"));
}

// The operations that name a target, which in 2026-07-28 are the ones
// required to mirror it into Mcp-Name
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#standard-request-headers
TEST(is_named_request_method) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("completion/complete"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("initialize"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("logging/setLevel"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/elicitation/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/initialized"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("notifications/message"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_named_request_method("prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_named_request_method("resources/read"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_named_request_method("resources/subscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method(
      "resources/templates/list"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_named_request_method("resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("roots/list"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("sampling/createMessage"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("server/discover"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_named_request_method("tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_named_request_method("tools/list"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_named_request_method("custom/unknownMethod"));
}

// Only these three operations may answer with an interim result
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#supported-requests
TEST(is_mrtr_method) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method(
      "notifications/elicitation/complete"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_mrtr_method("notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("notifications/message"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method(
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method(
      "notifications/resources/list_changed"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_mrtr_method("notifications/resources/updated"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_mrtr_method("notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method(
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_mrtr_method("notifications/tasks/status"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_mrtr_method("notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_mrtr_method("prompts/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("prompts/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_mrtr_method("resources/read"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("resources/subscribe"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_mrtr_method("resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("roots/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("sampling/createMessage"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("tasks/result"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_mrtr_method("tools/call"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("tools/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_mrtr_method("custom/unknownMethod"));
}

// The operations that must carry caching hints on a complete result
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching#cacheable-results
TEST(is_cacheable_method) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("completion/complete"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("elicitation/create"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("logging/setLevel"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("notifications/cancelled"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method(
      "notifications/elicitation/complete"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("notifications/initialized"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("notifications/message"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("notifications/progress"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method(
      "notifications/prompts/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method(
      "notifications/resources/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method(
      "notifications/resources/updated"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method(
      "notifications/roots/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method(
      "notifications/subscriptions/acknowledged"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("notifications/tasks/status"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method(
      "notifications/tools/list_changed"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("prompts/get"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_cacheable_method("prompts/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_cacheable_method("resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_cacheable_method("resources/read"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("resources/subscribe"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_cacheable_method("resources/templates/list"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("resources/unsubscribe"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("roots/list"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("sampling/createMessage"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_cacheable_method("server/discover"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("tasks/cancel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("tasks/get"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("tasks/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("tasks/result"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_cacheable_method("tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_cacheable_method("tools/list"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_cacheable_method("custom/unknownMethod"));
}
