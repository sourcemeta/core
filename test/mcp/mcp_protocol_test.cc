#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <sourcemeta/core/test.h>

#include <array>
#include <stdexcept>
#include <vector>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::int64_t
#include <limits>   // std::numeric_limits
#include <optional> // std::optional, std::nullopt
#include <sstream>
#include <string>
#include <utility> // std::move

namespace {
using namespace sourcemeta::core;

} // namespace

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

TEST(method_initialize) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_INITIALIZE, "initialize");
}

TEST(method_ping) { EXPECT_EQ(sourcemeta::core::MCP_METHOD_PING, "ping"); }

TEST(method_tools_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TOOLS_LIST, "tools/list");
}

TEST(method_tools_call) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TOOLS_CALL, "tools/call");
}

TEST(method_resources_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_LIST, "resources/list");
}

TEST(method_resources_read) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_READ, "resources/read");
}

TEST(method_resources_templates_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST,
            "resources/templates/list");
}

TEST(is_request_method_initialize) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "initialize"));
}

TEST(is_request_method_ping) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "ping"));
}

TEST(is_request_method_tools_list) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list"));
}

TEST(is_request_method_tools_call) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/call"));
}

TEST(is_request_method_resources_list) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/list"));
}

TEST(is_request_method_resources_read) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "resources/read"));
}

TEST(is_request_method_resources_templates_list) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list"));
}

TEST(is_request_method_unknown) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "foo/bar"));
}

TEST(is_request_method_empty) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, ""));
}

TEST(resolve_protocol_version_empty_returns_nullopt) {
  const auto result{sourcemeta::core::mcp_resolve_protocol_version("")};
  EXPECT_FALSE(result.has_value());
}

TEST(resolve_protocol_version_2025_03_26) {
  const auto result{
      sourcemeta::core::mcp_resolve_protocol_version("2025-03-26")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2025_03_26);
}

TEST(resolve_protocol_version_2025_06_18) {
  const auto result{
      sourcemeta::core::mcp_resolve_protocol_version("2025-06-18")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2025_06_18);
}

TEST(resolve_protocol_version_2025_11_25) {
  const auto result{
      sourcemeta::core::mcp_resolve_protocol_version("2025-11-25")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2025_11_25);
}

TEST(resolve_protocol_version_2026_07_28) {
  const auto result{
      sourcemeta::core::mcp_resolve_protocol_version("2026-07-28")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
}

TEST(resolve_protocol_version_unknown) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("9999-01-01").has_value());
}

TEST(resolve_protocol_version_malformed) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("not-a-date").has_value());
}

TEST(resolve_protocol_version_unrecognized_dates) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("2026-07-27").has_value());
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_protocol_version("2026-07-28 ")
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_protocol_version(" 2026-07-28")
                   .has_value());
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("2026-7-28").has_value());
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("2027-01-01").has_value());
}

TEST(protocol_version_at_least_2025_03_26_vs_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(protocol_version_at_least_2025_06_18_vs_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(protocol_version_at_least_2025_11_25_vs_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(protocol_version_at_least_2026_07_28_vs_2026_07_28) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

TEST(protocol_version_at_least_2026_07_28_vs_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(protocol_version_at_least_2026_07_28_vs_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(protocol_version_at_least_2026_07_28_vs_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(protocol_version_at_least_2025_03_26_vs_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

TEST(protocol_version_at_least_2025_06_18_vs_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

TEST(protocol_version_at_least_2025_11_25_vs_2026_07_28) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28));
}

TEST(protocol_version_at_least_2025_06_18_vs_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(protocol_version_at_least_2025_03_26_vs_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(protocol_version_at_least_2025_11_25_vs_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(protocol_version_at_least_2025_06_18_vs_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_at_least(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

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

TEST(is_request_method_of_a_parsed_message) {
  const auto message{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/call"
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      message.at("method").to_string()));
}

TEST(supports_jsonrpc_batching_at_runtime) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_jsonrpc_batching(version_2025_03_26));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_jsonrpc_batching(version_2025_06_18));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_jsonrpc_batching(version_2025_11_25));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_jsonrpc_batching(version_2026_07_28));
}

TEST(protocol_era_predicates_initialization_handshake) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_TRUE(
      sourcemeta::core::mcp_uses_initialization_handshake(version_2025_03_26));
  EXPECT_TRUE(
      sourcemeta::core::mcp_uses_initialization_handshake(version_2025_06_18));
  EXPECT_TRUE(
      sourcemeta::core::mcp_uses_initialization_handshake(version_2025_11_25));
  EXPECT_FALSE(
      sourcemeta::core::mcp_uses_initialization_handshake(version_2026_07_28));
}

TEST(protocol_era_predicates_ping) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_TRUE(sourcemeta::core::mcp_supports_ping(version_2025_03_26));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_ping(version_2025_06_18));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_ping(version_2025_11_25));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_ping(version_2026_07_28));
}

TEST(protocol_era_predicates_protocol_sessions) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_protocol_sessions(version_2025_03_26));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_protocol_sessions(version_2025_06_18));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_protocol_sessions(version_2025_11_25));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_protocol_sessions(version_2026_07_28));
}

TEST(protocol_era_predicates_requires_result_type) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(sourcemeta::core::mcp_requires_result_type(version_2025_03_26));
  EXPECT_FALSE(sourcemeta::core::mcp_requires_result_type(version_2025_06_18));
  EXPECT_FALSE(sourcemeta::core::mcp_requires_result_type(version_2025_11_25));
  EXPECT_TRUE(sourcemeta::core::mcp_requires_result_type(version_2026_07_28));
}

TEST(protocol_era_predicates_requires_cacheable_metadata) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(
      sourcemeta::core::mcp_requires_cacheable_metadata(version_2025_03_26));
  EXPECT_FALSE(
      sourcemeta::core::mcp_requires_cacheable_metadata(version_2025_06_18));
  EXPECT_FALSE(
      sourcemeta::core::mcp_requires_cacheable_metadata(version_2025_11_25));
  EXPECT_TRUE(
      sourcemeta::core::mcp_requires_cacheable_metadata(version_2026_07_28));
}

TEST(protocol_era_predicates_supports_server_discover) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_server_discover(version_2025_03_26));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_server_discover(version_2025_06_18));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_server_discover(version_2025_11_25));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_server_discover(version_2026_07_28));
}

TEST(protocol_era_predicates_supports_mrtr) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(sourcemeta::core::mcp_supports_mrtr(version_2025_03_26));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_mrtr(version_2025_06_18));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_mrtr(version_2025_11_25));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_mrtr(version_2026_07_28));
}

TEST(method_support_predicates_at_runtime) {
  std::string method_init{sourcemeta::core::MCP_METHOD_INITIALIZE};
  std::string method_ping{sourcemeta::core::MCP_METHOD_PING};
  std::string method_init_notif{
      sourcemeta::core::MCP_METHOD_NOTIFICATIONS_INITIALIZED};
  std::string method_logging_set_level{
      sourcemeta::core::MCP_METHOD_LOGGING_SET_LEVEL};
  std::string method_discover{sourcemeta::core::MCP_METHOD_SERVER_DISCOVER};
  std::string method_listen{sourcemeta::core::MCP_METHOD_SUBSCRIPTIONS_LISTEN};
  std::string method_sub_ack{
      sourcemeta::core::MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED};
  std::string method_tools_list{sourcemeta::core::MCP_METHOD_TOOLS_LIST};
  std::string method_tools_call{sourcemeta::core::MCP_METHOD_TOOLS_CALL};
  std::string method_resources_list{
      sourcemeta::core::MCP_METHOD_RESOURCES_LIST};
  std::string method_resources_read{
      sourcemeta::core::MCP_METHOD_RESOURCES_READ};
  std::string method_resources_templates_list{
      sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST};
  std::string method_custom{"custom/unknownMethod"};

  auto version_legacy{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_modern{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};

  // Legacy-only methods (removed in 2026-07-28)
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_init));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_method(version_modern, method_init));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_ping));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_method(version_modern, method_ping));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_init_notif));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_method(version_modern, method_init_notif));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(version_legacy,
                                                    method_logging_set_level));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(version_modern,
                                                     method_logging_set_level));

  // Modern-only methods (introduced in 2026-07-28)
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_discover));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_modern, method_discover));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_listen));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_modern, method_listen));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_sub_ack));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_modern, method_sub_ack));

  // Shared methods across versions
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_tools_list));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_modern, method_tools_list));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_tools_call));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_method(version_modern, method_tools_call));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(version_legacy,
                                                    method_resources_list));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(version_modern,
                                                    method_resources_list));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(version_legacy,
                                                    method_resources_read));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(version_modern,
                                                    method_resources_read));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      version_legacy, method_resources_templates_list));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      version_modern, method_resources_templates_list));

  // Unknown method
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_method(version_legacy, method_custom));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_method(version_modern, method_custom));
}

TEST(method_server_discover) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_SERVER_DISCOVER, "server/discover");
}

TEST(method_support_per_version) {
  using sourcemeta::core::MCPProtocolVersion;

  // Legacy versions support legacy methods but not 2026-07-28 methods
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "initialize"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "notifications/initialized"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "tools/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "resources/templates/list"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "server/discover"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25,
      "notifications/subscriptions/acknowledged"));

  // 2026-07-28 removed initialize, ping, notifications/initialized,
  // logging/setLevel
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "ping"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "notifications/initialized"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));

  // 2026-07-28 added server/discover, subscriptions/listen,
  // notifications/subscriptions/acknowledged
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "server/discover"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "subscriptions/listen"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28,
      "notifications/subscriptions/acknowledged"));

  // Shared methods supported on 2026-07-28
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "tools/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "tools/call"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "resources/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "resources/templates/list"));

  // Unknown method unsupported on all
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2025_11_25, "unknown/method"));
  EXPECT_FALSE(sourcemeta::core::mcp_supports_method(
      MCPProtocolVersion::V_2026_07_28, "unknown/method"));

  // mcp_is_request_method checks for logging/setLevel
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      MCPProtocolVersion::V_2025_11_25, "logging/setLevel"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      MCPProtocolVersion::V_2026_07_28, "logging/setLevel"));
}

TEST(is_request_method_initialize_at_runtime) {
  const std::string method{"initialize"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, method));
}

TEST(is_request_method_ping_at_runtime) {
  const std::string method{"ping"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, method));
}

TEST(is_request_method_tools_list_at_runtime) {
  const std::string method{"tools/list"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, method));
}

TEST(is_request_method_tools_call_at_runtime) {
  const std::string method{"tools/call"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, method));
}

TEST(is_request_method_resources_list_at_runtime) {
  const std::string method{"resources/list"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, method));
}

TEST(is_request_method_resources_read_at_runtime) {
  const std::string method{"resources/read"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, method));
}

TEST(is_request_method_resources_templates_list_at_runtime) {
  const std::string method{"resources/templates/list"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, method));
}

TEST(is_request_method_unknown_at_runtime) {
  const std::string method{"foo/bar"};
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, method));
}

TEST(protocol_version_is_valid_checks) {
  const std::string empty;
  const std::string v_2025_03_26{"2025-03-26"};
  const std::string v_2025_06_18{"2025-06-18"};
  const std::string v_2025_11_25{"2025-11-25"};
  const std::string v_2026_07_28{"2026-07-28"};
  const std::string v_2024_11_05{"2024-11-05"};
  const std::string invalid{"invalid-version"};

  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_is_valid(empty));
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_is_valid(v_2025_03_26));
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_is_valid(v_2025_06_18));
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_is_valid(v_2025_11_25));
  EXPECT_TRUE(sourcemeta::core::mcp_protocol_version_is_valid(v_2026_07_28));
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_is_valid(v_2024_11_05));
  EXPECT_FALSE(sourcemeta::core::mcp_protocol_version_is_valid(invalid));
}

TEST(official_method_matrix) {
  struct Entry {
    JSON::StringView method;
    unsigned client_request;
    unsigned server_request;
    unsigned client_notification;
    unsigned server_notification;
    unsigned input_request;
  };
  const std::array<Entry, 35> entries{{
      {.method = "completion/complete",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "elicitation/create",
       .client_request = 0,
       .server_request = 6,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 8},
      {.method = "initialize",
       .client_request = 7,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "logging/setLevel",
       .client_request = 7,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "notifications/cancelled",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 15,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/elicitation/complete",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 4,
       .input_request = 0},
      {.method = "notifications/initialized",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 7,
       .server_notification = 0,
       .input_request = 0},
      {.method = "notifications/message",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/progress",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 7,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/prompts/list_changed",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/resources/list_changed",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/resources/updated",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/roots/list_changed",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 7,
       .server_notification = 0,
       .input_request = 0},
      {.method = "notifications/subscriptions/acknowledged",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 8,
       .input_request = 0},
      {.method = "notifications/tasks/status",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 4,
       .server_notification = 4,
       .input_request = 0},
      {.method = "notifications/tools/list_changed",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "ping",
       .client_request = 7,
       .server_request = 7,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "prompts/get",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "prompts/list",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/list",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/read",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/subscribe",
       .client_request = 7,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/templates/list",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/unsubscribe",
       .client_request = 7,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "roots/list",
       .client_request = 0,
       .server_request = 7,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 8},
      {.method = "sampling/createMessage",
       .client_request = 0,
       .server_request = 7,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 8},
      {.method = "server/discover",
       .client_request = 8,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "subscriptions/listen",
       .client_request = 8,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tasks/cancel",
       .client_request = 4,
       .server_request = 4,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tasks/get",
       .client_request = 4,
       .server_request = 4,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tasks/list",
       .client_request = 4,
       .server_request = 4,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tasks/result",
       .client_request = 4,
       .server_request = 4,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tools/call",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tools/list",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "extension/unknown",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
  }};
  for (std::size_t index = 0; index < 4; ++index) {
    struct Revision {
      MCPProtocolVersion version;
      unsigned bit;
    };
    constexpr std::array<Revision, 4> DATED{
        {{.version = MCPProtocolVersion::V_2025_03_26, .bit = 1},
         {.version = MCPProtocolVersion::V_2025_06_18, .bit = 2},
         {.version = MCPProtocolVersion::V_2025_11_25, .bit = 4},
         {.version = MCPProtocolVersion::V_2026_07_28, .bit = 8}}};
    static_assert(DATED.size() == 4);
    const auto version{DATED[index].version};
    const auto bit{DATED[index].bit};
    EXPECT_EQ(mcp_supports_implementation_website_url(version),
              mcp_protocol_version_at_least(version,
                                            MCPProtocolVersion::V_2025_11_25));
    for (const auto &entry : entries) {
      EXPECT_EQ(mcp_is_request_method(version, entry.method),
                (entry.client_request & bit) != 0);
      EXPECT_EQ(mcp_is_server_request_method(version, entry.method),
                (entry.server_request & bit) != 0);
      EXPECT_EQ(mcp_is_client_notification_method(version, entry.method),
                (entry.client_notification & bit) != 0);
      EXPECT_EQ(mcp_is_server_notification_method(version, entry.method),
                (entry.server_notification & bit) != 0);
      EXPECT_EQ(
          mcp_is_notification_method(version, entry.method),
          ((entry.client_notification | entry.server_notification) & bit) != 0);
      EXPECT_EQ(mcp_is_input_request_method(version, entry.method),
                (entry.input_request & bit) != 0);
      EXPECT_EQ(mcp_supports_method(version, entry.method),
                ((entry.client_request | entry.server_request |
                  entry.client_notification | entry.server_notification |
                  entry.input_request) &
                 bit) != 0);
    }
  }
  for (const auto *const method :
       {"tools/call", "resources/read", "prompts/get", "resources/subscribe",
        "resources/unsubscribe"}) {
    EXPECT_TRUE(mcp_is_named_request_method(method));
  }
  for (const auto *const method :
       {"tools/list", "resources/list", "server/discover",
        "subscriptions/listen", "extension/unknown"}) {
    EXPECT_FALSE(mcp_is_named_request_method(method));
  }
}
