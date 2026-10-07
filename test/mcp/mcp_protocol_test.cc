#include <sourcemeta/core/mcp_protocol.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <sourcemeta/core/test.h>

#include <string> // std::string

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

TEST(method_notifications_initialized) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_INITIALIZED,
            "notifications/initialized");
}

TEST(is_request_method_initialize) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("initialize"));
}

TEST(is_request_method_ping) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("ping"));
}

TEST(is_request_method_tools_list) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("tools/list"));
}

TEST(is_request_method_tools_call) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("tools/call"));
}

TEST(is_request_method_resources_list) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("resources/list"));
}

TEST(is_request_method_resources_read) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("resources/read"));
}

TEST(is_request_method_resources_templates_list) {
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_request_method("resources/templates/list"));
}

TEST(is_request_method_notifications_initialized_is_not_request) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_request_method("notifications/initialized"));
}

TEST(is_request_method_unknown) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method("foo/bar"));
}

TEST(is_request_method_empty) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(""));
}

TEST(resolve_protocol_version_empty_defaults_to_2025_03_26) {
  const auto result{sourcemeta::core::mcp_resolve_protocol_version("")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2025_03_26);
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

TEST(resolve_protocol_version_unknown) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("9999-01-01").has_value());
}

TEST(resolve_protocol_version_malformed) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("not-a-date").has_value());
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

TEST(is_request_method_of_a_parsed_message) {
  const auto message{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/call"
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      message.at("method").to_string()));
}

TEST(supports_jsonrpc_batching_at_runtime) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_jsonrpc_batching(version_2025_03_26));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_jsonrpc_batching(version_2025_06_18));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_jsonrpc_batching(version_2025_11_25));
}

TEST(is_request_method_initialize_at_runtime) {
  const std::string method{"initialize"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(method));
}

TEST(is_request_method_ping_at_runtime) {
  const std::string method{"ping"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(method));
}

TEST(is_request_method_tools_list_at_runtime) {
  const std::string method{"tools/list"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(method));
}

TEST(is_request_method_tools_call_at_runtime) {
  const std::string method{"tools/call"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(method));
}

TEST(is_request_method_resources_list_at_runtime) {
  const std::string method{"resources/list"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(method));
}

TEST(is_request_method_resources_read_at_runtime) {
  const std::string method{"resources/read"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(method));
}

TEST(is_request_method_resources_templates_list_at_runtime) {
  const std::string method{"resources/templates/list"};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(method));
}

TEST(is_request_method_unknown_at_runtime) {
  const std::string method{"foo/bar"};
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(method));
}
