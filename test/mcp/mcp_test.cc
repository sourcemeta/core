#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/http_status.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <sourcemeta/core/test.h>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::int64_t
#include <limits>   // std::numeric_limits
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string
#include <utility>  // std::move

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

TEST(method_notifications_initialized) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_INITIALIZED,
            "notifications/initialized");
}

TEST(code_resource_not_found) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
            static_cast<std::int64_t>(-32002));
}

TEST(code_url_elicitation_required) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_URL_ELICITATION_REQUIRED,
            static_cast<std::int64_t>(-32042));
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

TEST(is_request_method_notifications_initialized_is_not_request) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
}

TEST(is_request_method_unknown) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "foo/bar"));
}

TEST(is_request_method_empty) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, ""));
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

TEST(protocol_version_at_least) {
  using sourcemeta::core::mcp_protocol_version_at_least;
  using sourcemeta::core::MCPProtocolVersion;

  // Every revision is at least itself
  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_03_26,
                                            MCPProtocolVersion::V_2025_03_26));
  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_06_18,
                                            MCPProtocolVersion::V_2025_06_18));
  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_11_25,
                                            MCPProtocolVersion::V_2025_11_25));
  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2026_07_28,
                                            MCPProtocolVersion::V_2026_07_28));

  // 2026-07-28 is at least every older revision
  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2026_07_28,
                                            MCPProtocolVersion::V_2025_03_26));
  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2026_07_28,
                                            MCPProtocolVersion::V_2025_06_18));
  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2026_07_28,
                                            MCPProtocolVersion::V_2025_11_25));

  // No older revision is at least 2026-07-28
  EXPECT_FALSE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_03_26,
                                             MCPProtocolVersion::V_2026_07_28));
  EXPECT_FALSE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_06_18,
                                             MCPProtocolVersion::V_2026_07_28));
  EXPECT_FALSE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_11_25,
                                             MCPProtocolVersion::V_2026_07_28));

  // Existing versions remain chronologically ordered
  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_06_18,
                                            MCPProtocolVersion::V_2025_03_26));
  EXPECT_FALSE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_03_26,
                                             MCPProtocolVersion::V_2025_06_18));

  EXPECT_TRUE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_11_25,
                                            MCPProtocolVersion::V_2025_06_18));
  EXPECT_FALSE(mcp_protocol_version_at_least(MCPProtocolVersion::V_2025_06_18,
                                             MCPProtocolVersion::V_2025_11_25));
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

TEST(make_text_block) {
  const auto block{sourcemeta::core::mcp_make_text_block("hello")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "hello"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_text_block_empty_string) {
  const auto block{sourcemeta::core::mcp_make_text_block("")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": ""
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_text_block_with_newlines) {
  const auto block{sourcemeta::core::mcp_make_text_block("line1\nline2")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "line1\nline2"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_11_25_full) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "file:///foo",
      "text/plain", "My File", "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "file:///foo",
    "name": "My File",
    "description": "A description",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_11_25_without_name_and_description) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "file:///foo",
      "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "file:///foo",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_06_18_supports_structured) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "file:///foo",
      "text/plain", "My File")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "file:///foo",
    "name": "My File",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_03_26_falls_back_to_text_with_name) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///foo",
      "text/plain", "My File", "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "My File\nfile:///foo\nA description"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_03_26_falls_back_to_text_without_name) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///foo",
      "text/plain", {}, "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "file:///foo\nA description"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_03_26_falls_back_to_text_without_description) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///foo",
      "text/plain", "My File")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "My File\nfile:///foo"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_03_26_falls_back_to_text_uri_only) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///foo",
      "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "file:///foo"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_03_26_falls_back_handles_parentheses_in_name) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      "https://example.com/schema", "application/schema+json",
      "RFC 5322 Email Address (Addr-Spec)", "Validates email syntax")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "RFC 5322 Email Address (Addr-Spec)\nhttps://example.com/schema\nValidates email syntax"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_03_26_falls_back_preserves_empty_uri_position) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "", "text/plain",
      "My File", "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "My File\n\nA description"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_03_26_falls_back_empty_uri_with_description_only) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "", "text/plain", {},
      "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "\nA description"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_03_26_falls_back_empty_uri_with_name_only) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "", "text/plain",
      "My File")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "My File\n"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(tool_success_with_object_result) {
  const auto identifier{sourcemeta::core::JSON{1}};
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("foo", sourcemeta::core::JSON{42});
  const auto envelope{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier,
      std::move(result))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "content": [
        { "type": "text", "text": "{\n  \"foo\": 42\n}" }
      ],
      "structuredContent": { "foo": 42 },
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_success_with_array_result) {
  const auto identifier{sourcemeta::core::JSON{"abc"}};
  const auto envelope{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier,
      sourcemeta::core::parse_json(R"([ 1, 2, 3 ])"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "abc",
    "result": {
      "content": [
        { "type": "text", "text": "[ 1, 2, 3 ]" }
      ],
      "structuredContent": [ 1, 2, 3 ],
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_success_with_null_id) {
  const auto envelope{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON{nullptr}, sourcemeta::core::JSON::make_object())};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "result": {
      "content": [
        { "type": "text", "text": "{}" }
      ],
      "structuredContent": {},
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_success_2025_03_26_omits_structured_content) {
  const auto identifier{sourcemeta::core::JSON{1}};
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("foo", sourcemeta::core::JSON{42});
  const auto envelope{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, identifier,
      std::move(result))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "content": [
        { "type": "text", "text": "{\n  \"foo\": 42\n}" }
      ],
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_success_with_explicit_content_blocks) {
  const auto identifier{sourcemeta::core::JSON{1}};
  auto structured{sourcemeta::core::JSON::make_object()};
  structured.assign("ok", sourcemeta::core::JSON{true});
  auto blocks{sourcemeta::core::JSON::make_array()};
  blocks.push_back(sourcemeta::core::mcp_make_text_block("done"));
  const auto envelope{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier,
      std::move(structured), std::move(blocks))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "content": [ { "type": "text", "text": "done" } ],
      "structuredContent": { "ok": true },
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_success_with_explicit_blocks_omits_structured_on_2025_03_26) {
  const auto identifier{sourcemeta::core::JSON{1}};
  auto structured{sourcemeta::core::JSON::make_object()};
  structured.assign("ok", sourcemeta::core::JSON{true});
  auto blocks{sourcemeta::core::JSON::make_array()};
  blocks.push_back(sourcemeta::core::mcp_make_text_block("done"));
  const auto envelope{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, identifier,
      std::move(structured), std::move(blocks))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "content": [ { "type": "text", "text": "done" } ],
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_error_with_message) {
  const auto identifier{sourcemeta::core::JSON{7}};
  const auto envelope{sourcemeta::core::mcp_make_tool_error(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier,
      "Schema not found")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 7,
    "result": {
      "content": [ { "type": "text", "text": "Schema not found" } ],
      "isError": true
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_error_with_string_id) {
  const auto identifier{sourcemeta::core::JSON{"req-1"}};
  const auto envelope{sourcemeta::core::mcp_make_tool_error(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier,
      "Invalid input")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "req-1",
    "result": {
      "content": [ { "type": "text", "text": "Invalid input" } ],
      "isError": true
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_error_with_null_id) {
  const auto envelope{sourcemeta::core::mcp_make_tool_error(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON{nullptr}, "Boom")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "result": {
      "content": [ { "type": "text", "text": "Boom" } ],
      "isError": true
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(error_resource_not_found_with_integer_id) {
  const auto identifier{sourcemeta::core::JSON{3}};
  const auto envelope{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 3,
    "error": {
      "code": -32002,
      "message": "Resource not found"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(error_resource_not_found_with_string_id) {
  const auto identifier{sourcemeta::core::JSON{"req-7"}};
  const auto envelope{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "req-7",
    "error": {
      "code": -32002,
      "message": "Resource not found"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_resource_full) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", "First file",
      std::optional<std::size_t>{1024})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "description": "First file",
    "mimeType": "text/plain",
    "size": 1024
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_without_description) {
  const auto resource{
      sourcemeta::core::mcp_make_resource("file:///a", "Alpha", "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_without_size) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", "First file")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "description": "First file",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_with_priority) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", "First file",
      std::optional<std::size_t>{1024}, std::optional<double>{0.75})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "description": "First file",
    "mimeType": "text/plain",
    "size": 1024,
    "annotations": { "priority": 0.75 }
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_with_priority_without_size) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", {}, std::nullopt,
      std::optional<double>{0.25})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain",
    "annotations": { "priority": 0.25 }
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_with_priority_clamped_high) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", {}, std::nullopt,
      std::optional<double>{2.5})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain",
    "annotations": { "priority": 1.0 }
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_with_priority_clamped_low) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", {}, std::nullopt,
      std::optional<double>{-5.0})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain",
    "annotations": { "priority": 0.0 }
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_with_priority_positive_infinity) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", {}, std::nullopt,
      std::optional<double>{std::numeric_limits<double>::infinity()})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain",
    "annotations": { "priority": 1.0 }
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_with_priority_negative_infinity) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", {}, std::nullopt,
      std::optional<double>{-std::numeric_limits<double>::infinity()})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain",
    "annotations": { "priority": 0.0 }
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_with_priority_nan) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      "file:///a", "Alpha", "text/plain", {}, std::nullopt,
      std::optional<double>{std::numeric_limits<double>::quiet_NaN()})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain",
    "annotations": { "priority": 1.0 }
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_text_content) {
  const auto content{sourcemeta::core::mcp_make_resource_text_content(
      "file:///a", "text/plain", "Hello")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "mimeType": "text/plain",
    "text": "Hello"
  })JSON")};
  EXPECT_EQ(content, expected);
}

TEST(make_resources_read_result_single) {
  auto contents{sourcemeta::core::JSON::make_array()};
  contents.push_back(sourcemeta::core::mcp_make_resource_text_content(
      "file:///a", "text/plain", "Hello"));
  const auto result{sourcemeta::core::mcp_make_resources_read_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::move(contents))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "contents": [
      { "uri": "file:///a", "mimeType": "text/plain", "text": "Hello" }
    ]
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(make_resources_read_result_empty) {
  const auto result{sourcemeta::core::mcp_make_resources_read_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON::make_array())};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "contents": []
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(make_resource_template) {
  const auto entry{sourcemeta::core::mcp_make_resource_template(
      "file:///{path}", "Files", "Resolves a file path", "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "file:///{path}",
    "name": "Files",
    "description": "Resolves a file path",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(entry, expected);
}

TEST(make_tool_descriptor_default_annotations_2025_11_25) {
  const auto entry{sourcemeta::core::mcp_make_tool_descriptor(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "say", "Says hello",
      sourcemeta::core::parse_json(R"({ "type": "object" })"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "name": "say",
    "description": "Says hello",
    "inputSchema": { "type": "object" },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON")};
  EXPECT_EQ(entry, expected);
}

TEST(make_tool_descriptor_with_output_schema_2025_11_25) {
  const auto entry{sourcemeta::core::mcp_make_tool_descriptor(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "say", "Says hello",
      sourcemeta::core::parse_json(R"({ "type": "object" })"),
      sourcemeta::core::parse_json(R"({ "type": "string" })"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "name": "say",
    "description": "Says hello",
    "inputSchema": { "type": "object" },
    "outputSchema": { "type": "string" },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON")};
  EXPECT_EQ(entry, expected);
}

TEST(make_tool_descriptor_with_output_schema_dropped_on_2025_03_26) {
  const auto entry{sourcemeta::core::mcp_make_tool_descriptor(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "say", "Says hello",
      sourcemeta::core::parse_json(R"({ "type": "object" })"),
      sourcemeta::core::parse_json(R"({ "type": "string" })"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "name": "say",
    "description": "Says hello",
    "inputSchema": { "type": "object" },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON")};
  EXPECT_EQ(entry, expected);
}

TEST(make_tool_descriptor_with_title_and_read_only_hints) {
  sourcemeta::core::MCPToolAnnotations annotations;
  annotations.title = "Say Hello";
  annotations.read_only = true;
  annotations.destructive = false;
  annotations.idempotent = true;
  annotations.open_world = false;
  const auto entry{sourcemeta::core::mcp_make_tool_descriptor(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "say", "Says hello",
      sourcemeta::core::parse_json(R"({ "type": "object" })"), std::nullopt,
      annotations)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "name": "say",
    "description": "Says hello",
    "inputSchema": { "type": "object" },
    "annotations": {
      "title": "Say Hello",
      "readOnlyHint": true,
      "destructiveHint": false,
      "idempotentHint": true,
      "openWorldHint": false
    }
  })JSON")};
  EXPECT_EQ(entry, expected);
}

TEST(make_initialize_result_minimal_2025_11_25) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-11-25" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "serverInfo": { "name": "srv", "version": "1.0.0" }
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_initialize_result_missing_protocol_version_is_invalid_params) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {}
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_initialize_result_non_string_protocol_version_is_invalid_params) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": 123 }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_initialize_result_unsupported_protocol_version_negotiates_latest) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "9999-01-01" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "serverInfo": { "name": "srv", "version": "1.0.0" }
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_initialize_result_with_all_capabilities) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-11-25" }
  })JSON")};
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.prompts = true;
  capabilities.resources = true;
  capabilities.tools = true;
  capabilities.logging = true;
  capabilities.completions = true;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "protocolVersion": "2025-11-25",
      "capabilities": {
        "prompts": {},
        "resources": {},
        "tools": {},
        "logging": {},
        "completions": {}
      },
      "serverInfo": { "name": "srv", "version": "1.0.0" }
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_initialize_result_includes_instructions_when_provided) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-11-25" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, "Be careful.")};
  EXPECT_EQ(envelope.at("result").at("instructions").to_string(),
            "Be careful.");
}

TEST(make_initialize_result_includes_title_on_2025_06_18) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-06-18" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url = "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("result").at("serverInfo").at("title").to_string(),
            "Server");
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("description"));
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("websiteUrl"));
}

TEST(make_initialize_result_includes_full_implementation_on_2025_11_25) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-11-25" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url = "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("result").at("serverInfo").at("title").to_string(),
            "Server");
  EXPECT_EQ(
      envelope.at("result").at("serverInfo").at("description").to_string(),
      "desc");
  EXPECT_EQ(envelope.at("result").at("serverInfo").at("websiteUrl").to_string(),
            "https://x");
}

TEST(
    make_initialize_result_strips_unsupported_implementation_fields_on_2025_03_26) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-03-26" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url = "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("title"));
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("description"));
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("websiteUrl"));
}

TEST(make_initialize_result_falls_back_to_2025_11_25_on_unknown_version) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "9999-01-01" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
}

TEST(make_initialize_result_returns_invalid_request_when_missing_params) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "initialize"
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST);
}

TEST(make_initialize_result_returns_invalid_request_when_id_missing) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "method": "initialize", "params": {}
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST);
}

TEST(tool_call_arguments_present) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": { "x": 1 } }
  })JSON")};
  const auto *arguments{sourcemeta::core::mcp_tool_call_arguments(envelope)};
  EXPECT_NE(arguments, nullptr);
  EXPECT_TRUE(arguments->is_object());
  EXPECT_EQ(arguments->at("x").to_integer(), 1);
}

TEST(tool_call_arguments_missing) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(tool_call_arguments_params_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": [ 1, 2 ]
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(tool_call_arguments_no_params) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call"
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(tool_call_arguments_string_value) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": "not-an-object" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(tool_call_arguments_array_value) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": [ 1, 2, 3 ] }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(tool_call_arguments_null_value) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": null }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
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

TEST(protocol_era_predicates_requires_request_meta) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(version_2025_03_26));
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(version_2025_06_18));
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(version_2025_11_25));
  EXPECT_TRUE(sourcemeta::core::mcp_requires_request_meta(version_2026_07_28));
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

TEST(protocol_era_predicates_supports_subscriptions_listen) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_subscriptions_listen(version_2025_03_26));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_subscriptions_listen(version_2025_06_18));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_subscriptions_listen(version_2025_11_25));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_subscriptions_listen(version_2026_07_28));
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

TEST(latest_initialization_version) {
  auto version{sourcemeta::core::mcp_latest_initialization_version()};
  EXPECT_EQ(version, sourcemeta::core::MCPProtocolVersion::V_2025_11_25);
}

TEST(method_server_discover) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_SERVER_DISCOVER, "server/discover");
}

TEST(method_subscriptions_listen) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_SUBSCRIPTIONS_LISTEN,
            "subscriptions/listen");
}

TEST(method_notifications_subscriptions_acknowledged) {
  EXPECT_EQ(
      sourcemeta::core::MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED,
      "notifications/subscriptions/acknowledged");
}

TEST(version_aware_request_methods_legacy) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(version, "initialize"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(version, "ping"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(version, "tools/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(version, "tools/call"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_request_method(version, "resources/list"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_request_method(version, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      version, "resources/templates/list"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_request_method(version, "server/discover"));
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_request_method(version, "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      version, "notifications/initialized"));
}

TEST(version_aware_request_methods_modern) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(version, "initialize"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(version, "ping"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_request_method(version, "server/discover"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(version, "tools/list"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(version, "tools/call"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_request_method(version, "resources/list"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_request_method(version, "resources/read"));
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method(
      version, "resources/templates/list"));
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_request_method(version, "subscriptions/listen"));
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      version, "notifications/initialized"));
}

TEST(code_header_mismatch) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_HEADER_MISMATCH,
            static_cast<std::int64_t>(-32020));
}

TEST(code_missing_required_client_capability) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY,
            static_cast<std::int64_t>(-32021));
}

TEST(code_unsupported_protocol_version) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
            static_cast<std::int64_t>(-32022));
}

TEST(cache_scope_strings) {
  EXPECT_EQ(sourcemeta::core::mcp_cache_scope_string(
                sourcemeta::core::MCPCacheScope::Public),
            "public");
  EXPECT_EQ(sourcemeta::core::mcp_cache_scope_string(
                sourcemeta::core::MCPCacheScope::Private),
            "private");
}

TEST(cache_scope_parsing) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_cache_scope("public"),
            sourcemeta::core::MCPCacheScope::Public);
  EXPECT_EQ(sourcemeta::core::mcp_resolve_cache_scope("private"),
            sourcemeta::core::MCPCacheScope::Private);
  EXPECT_EQ(sourcemeta::core::mcp_resolve_cache_scope("shared"), std::nullopt);
}

TEST(request_meta_validation_valid_minimum) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      }
    }
  })JSON")};

  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_EQ(meta->protocol_version,
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
  EXPECT_NE(meta->client_capabilities, nullptr);
  EXPECT_TRUE(meta->client_capabilities->is_object());
  EXPECT_TRUE(meta->client_info.has_value());
  EXPECT_EQ(meta->client_info->name, "ExampleClient");
  EXPECT_EQ(meta->client_info->version, "1.0.0");
  EXPECT_TRUE(meta->client_info->title.empty());
  EXPECT_TRUE(meta->client_info->description.empty());
  EXPECT_TRUE(sourcemeta::core::mcp_has_required_request_meta(envelope));

  const auto client_info{sourcemeta::core::mcp_request_client_info(envelope)};
  EXPECT_TRUE(client_info.has_value());
  EXPECT_EQ(client_info->name, "ExampleClient");
  EXPECT_EQ(client_info->version, "1.0.0");
  EXPECT_FALSE(meta->log_level.has_value());
}

TEST(request_meta_validation_valid_with_client_info_and_log_level) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": { "roots": { "listChanged": true } },
        "io.modelcontextprotocol/clientInfo": {
          "name": "client-one",
          "version": "1.2.3",
          "title": "Client Title",
          "description": "Client Description"
        },
        "io.modelcontextprotocol/logLevel": "debug",
        "customExtension": 42
      },
      "name": "compute"
    }
  })JSON")};

  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_TRUE(meta->client_info.has_value());
  EXPECT_EQ(meta->client_info->name, "client-one");
  EXPECT_EQ(meta->client_info->version, "1.2.3");
  EXPECT_EQ(meta->client_info->title, "Client Title");
  EXPECT_EQ(meta->client_info->description, "Client Description");
  EXPECT_TRUE(meta->log_level.has_value());
  EXPECT_EQ(meta->log_level.value(), "debug");
  EXPECT_NE(meta->meta_object, nullptr);
  EXPECT_TRUE(meta->meta_object->defines("customExtension"));
}

TEST(request_meta_validation_missing_params) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list"
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::MissingParams);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_params_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list", "params": [ 1, 2 ]
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::ParamsNotObject);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_meta) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list", "params": { "name": "test" }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::MissingMeta);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_meta_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list", "params": { "_meta": "invalid" }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::MetaNotObject);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_protocol_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { "io.modelcontextprotocol/clientCapabilities": {} } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingProtocolVersion);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_protocol_version_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": 2026,
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ProtocolVersionNotString);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_unsupported_protocol_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "1900-01-01",
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_client_capabilities) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28"
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingClientCapabilities);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_client_capabilities_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": "not-an-object"
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(
      status,
      sourcemeta::core::MCPRequestMetaStatus::ClientCapabilitiesNotObject);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_client_info) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_TRUE(sourcemeta::core::mcp_has_required_request_meta(envelope));
  EXPECT_FALSE(sourcemeta::core::mcp_request_client_info(envelope).has_value());
  EXPECT_EQ(sourcemeta::core::mcp_request_protocol_version(envelope).value(),
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
  EXPECT_NE(sourcemeta::core::mcp_request_client_capabilities(envelope),
            nullptr);
}

TEST(request_meta_validation_client_info_not_object) {
  // Test null
  {
    const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": null
      } }
    })JSON")};
    const auto [status,
                meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
    EXPECT_EQ(status,
              sourcemeta::core::MCPRequestMetaStatus::ClientInfoNotObject);
    EXPECT_FALSE(meta.has_value());
    EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
    EXPECT_FALSE(
        sourcemeta::core::mcp_request_client_info(envelope).has_value());
  }
  // Test array
  {
    const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": [ "ExampleClient", "1.0.0" ]
      } }
    })JSON")};
    const auto [status,
                meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
    EXPECT_EQ(status,
              sourcemeta::core::MCPRequestMetaStatus::ClientInfoNotObject);
    EXPECT_FALSE(meta.has_value());
    EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
    EXPECT_FALSE(
        sourcemeta::core::mcp_request_client_info(envelope).has_value());
  }
  // Test string
  {
    const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": "ExampleClient/1.0.0"
      } }
    })JSON")};
    const auto [status,
                meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
    EXPECT_EQ(status,
              sourcemeta::core::MCPRequestMetaStatus::ClientInfoNotObject);
    EXPECT_FALSE(meta.has_value());
    EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
    EXPECT_FALSE(
        sourcemeta::core::mcp_request_client_info(envelope).has_value());
  }
}

TEST(request_meta_validation_missing_client_info_name) {
  // Empty object
  {
    const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {}
      } }
    })JSON")};
    const auto [status,
                meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
    EXPECT_EQ(status,
              sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoName);
    EXPECT_FALSE(meta.has_value());
    EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
    EXPECT_FALSE(
        sourcemeta::core::mcp_request_client_info(envelope).has_value());
  }
  // Version present, name missing
  {
    const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "version": "1.0.0"
        }
      } }
    })JSON")};
    const auto [status,
                meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
    EXPECT_EQ(status,
              sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoName);
    EXPECT_FALSE(meta.has_value());
    EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
    EXPECT_FALSE(
        sourcemeta::core::mcp_request_client_info(envelope).has_value());
  }
}

TEST(request_meta_validation_client_info_name_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": 42,
        "version": "1.0.0"
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoNameNotString);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
  EXPECT_FALSE(sourcemeta::core::mcp_request_client_info(envelope).has_value());
}

TEST(request_meta_validation_missing_client_info_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "ExampleClient"
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoVersion);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
  EXPECT_FALSE(sourcemeta::core::mcp_request_client_info(envelope).has_value());
}

TEST(request_meta_validation_client_info_version_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "ExampleClient",
        "version": false
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoVersionNotString);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
  EXPECT_FALSE(sourcemeta::core::mcp_request_client_info(envelope).has_value());
}

TEST(request_meta_validation_client_info_title_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "ExampleClient",
        "version": "1.0.0",
        "title": 123
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoTitleNotString);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
  EXPECT_FALSE(sourcemeta::core::mcp_request_client_info(envelope).has_value());
}

TEST(request_meta_validation_client_info_description_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "ExampleClient",
        "version": "1.0.0",
        "description": [ "an", "mcp", "client" ]
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(
      status,
      sourcemeta::core::MCPRequestMetaStatus::ClientInfoDescriptionNotString);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(envelope));
  EXPECT_FALSE(sourcemeta::core::mcp_request_client_info(envelope).has_value());
}

TEST(body_extraction_method) {
  const auto req{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/call", "params": {}
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_method_from_body(req), "tools/call");

  const auto invalid{
      sourcemeta::core::parse_json(R"JSON({ "foo": "bar" })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_method_from_body(invalid),
            std::nullopt);
}

TEST(body_extraction_name) {
  const auto tool_call{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/call",
    "params": { "name": "my-tool", "arguments": {} }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(tool_call), "my-tool");

  const auto res_read{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "resources/read",
    "params": { "uri": "file:///path/to/file" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(res_read),
            "file:///path/to/file");

  const auto prompt_get{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "prompts/get",
    "params": { "name": "code-review" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(prompt_get),
            "code-review");

  const auto neither{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": {}
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(neither),
            std::nullopt);
}

TEST(error_unsupported_protocol_version) {
  const auto identifier{sourcemeta::core::JSON{10}};
  const std::vector<sourcemeta::core::JSON::StringView> supported{"2026-07-28",
                                                                  "2025-11-25"};
  const auto envelope{
      sourcemeta::core::mcp_make_error_unsupported_protocol_version(
          identifier, "1900-01-01", supported)};

  EXPECT_EQ(envelope.at("error").at("code").to_integer(), -32022);
  EXPECT_EQ(envelope.at("error").at("message").to_string(),
            "Unsupported protocol version");
  EXPECT_EQ(envelope.at("error").at("data").at("requested").to_string(),
            "1900-01-01");
  EXPECT_EQ(envelope.at("error").at("data").at("supported").size(), 2);
  EXPECT_EQ(envelope.at("error").at("data").at("supported").at(0).to_string(),
            "2026-07-28");
  EXPECT_EQ(envelope.at("error").at("data").at("supported").at(1).to_string(),
            "2025-11-25");
}

TEST(error_missing_required_capability) {
  const auto identifier{sourcemeta::core::JSON{11}};
  auto required{sourcemeta::core::JSON::make_object()};
  required.assign("elicitation", sourcemeta::core::JSON::make_object());

  const auto envelope{
      sourcemeta::core::mcp_make_error_missing_required_capability(
          identifier, std::move(required),
          "Missing required client capability")};

  EXPECT_EQ(envelope.at("error").at("code").to_integer(), -32021);
  EXPECT_EQ(envelope.at("error").at("message").to_string(),
            "Missing required client capability");
  EXPECT_TRUE(envelope.at("error")
                  .at("data")
                  .at("requiredCapabilities")
                  .defines("elicitation"));
}

TEST(error_header_mismatch) {
  const auto identifier{sourcemeta::core::JSON{12}};
  const auto envelope{sourcemeta::core::mcp_make_error_header_mismatch(
      identifier, "Mcp-Method header does not match body method")};

  EXPECT_EQ(envelope.at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(envelope.at("error").at("message").to_string(),
            "Mcp-Method header does not match body method");
}

TEST(error_header_mismatch_unexpected) {
  const auto identifier{sourcemeta::core::JSON{14}};
  const auto envelope{sourcemeta::core::mcp_make_error_header_mismatch(
      identifier, "Mcp-Name", "unexpected-name")};

  EXPECT_EQ(envelope.at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(envelope.at("error").at("message").to_string(),
            "Header mismatch: unexpected header provided");
  EXPECT_EQ(envelope.at("error").at("data").at("header").to_string(),
            "Mcp-Name");
  EXPECT_EQ(envelope.at("error").at("data").at("headerValue").to_string(),
            "unexpected-name");
}

TEST(header_validation_modern_valid_server_discover) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "server/discover",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      }
    }
  })JSON")};

  const auto error{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "server/discover", std::nullopt, envelope)};
  EXPECT_FALSE(error.has_value());
}

TEST(header_validation_modern_valid_tools_list) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      }
    }
  })JSON")};

  const auto error{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "tools/list", std::nullopt, envelope)};
  EXPECT_FALSE(error.has_value());
}

TEST(header_validation_modern_valid_tools_call) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      },
      "name": "calculator"
    }
  })JSON")};

  const auto error{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "tools/call", "calculator", envelope)};
  EXPECT_FALSE(error.has_value());
}

TEST(header_validation_modern_valid_resources_read) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "resources/read",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      },
      "uri": "file:///path/to/resource.txt"
    }
  })JSON")};

  // Plain URI
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "resources/read", "file:///path/to/resource.txt", envelope)};
    EXPECT_FALSE(error.has_value());
  }

  // Base64 encoded header value
  {
    // "file:///path/to/resource.txt" in base64 is
    // "ZmlsZTovLy9wYXRoL3RvL3Jlc291cmNlLnR4dA=="
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "resources/read",
        "=?base64?ZmlsZTovLy9wYXRoL3RvL3Jlc291cmNlLnR4dA==?=", envelope)};
    EXPECT_FALSE(error.has_value());
  }
}

TEST(header_validation_modern_valid_prompts_get) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "prompts/get",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      },
      "name": "summarize"
    }
  })JSON")};

  const auto error{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "prompts/get", "summarize", envelope)};
  EXPECT_FALSE(error.has_value());
}

TEST(header_validation_modern_missing_headers) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      },
      "name": "calculator"
    }
  })JSON")};

  // Missing MCP-Protocol-Version
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::nullopt,
        "tools/call", "calculator", envelope)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
    EXPECT_EQ(error->at("id").to_integer(), 42);
  }

  // Missing Mcp-Method
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        std::nullopt, "calculator", envelope)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
    EXPECT_EQ(error->at("id").to_integer(), 42);
  }

  // Missing Mcp-Name on named method
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", std::nullopt, envelope)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
    EXPECT_EQ(error->at("id").to_integer(), 42);
  }
}

TEST(header_validation_modern_mismatches) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 100,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      },
      "name": "calculator"
    }
  })JSON")};

  // Protocol header disagrees with request metadata
  {
    const auto envelope_legacy_meta{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 100,
      "method": "tools/call",
      "params": {
        "_meta": {
          "io.modelcontextprotocol/protocolVersion": "2025-11-25",
          "io.modelcontextprotocol/clientCapabilities": {},
          "io.modelcontextprotocol/clientInfo": {
            "name": "ExampleClient",
            "version": "1.0.0"
          }
        },
        "name": "calculator"
      }
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", "calculator", envelope_legacy_meta)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
  }

  // Mcp-Method disagrees with JSON-RPC method
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/list", "calculator", envelope)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
  }

  // Mcp-Name disagrees with request body name
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", "other_tool", envelope)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
  }

  // Unsupported protocol header
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "1900-01-01",
        "tools/call", "calculator", envelope)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32022);
  }

  // Header name supplied for an operation with no body name
  {
    const auto envelope_tools_list{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 101,
      "method": "tools/list",
      "params": {
        "_meta": {
          "io.modelcontextprotocol/protocolVersion": "2026-07-28",
          "io.modelcontextprotocol/clientCapabilities": {},
          "io.modelcontextprotocol/clientInfo": {
            "name": "ExampleClient",
            "version": "1.0.0"
          }
        }
      }
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/list", "unexpected_name", envelope_tools_list)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
  }
}

TEST(header_validation_legacy_regressions) {
  const std::vector<sourcemeta::core::MCPProtocolVersion> legacy_versions{
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25};

  for (const auto legacy_version : legacy_versions) {
    const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 1,
      "method": "tools/call",
      "params": {
        "name": "calculator"
      }
    })JSON")};

    // Modern routing headers are not required for legacy versions
    const auto no_headers{sourcemeta::core::mcp_validate_request_headers(
        legacy_version, std::nullopt, std::nullopt, std::nullopt, envelope)};
    EXPECT_FALSE(no_headers.has_value());

    // If matching headers are supplied, validation passes
    const auto matching{sourcemeta::core::mcp_validate_request_headers(
        legacy_version,
        sourcemeta::core::mcp_protocol_version_string(legacy_version),
        "tools/call", "calculator", envelope)};
    EXPECT_FALSE(matching.has_value());

    // Mismatched method header produces -32020
    const auto method_mismatch{sourcemeta::core::mcp_validate_request_headers(
        legacy_version, std::nullopt, "tools/list", std::nullopt, envelope)};
    EXPECT_TRUE(method_mismatch.has_value());
    EXPECT_EQ(method_mismatch->at("error").at("code").to_integer(), -32020);

    // Mismatched name header produces -32020
    const auto name_mismatch{sourcemeta::core::mcp_validate_request_headers(
        legacy_version, std::nullopt, std::nullopt, "other_tool", envelope)};
    EXPECT_TRUE(name_mismatch.has_value());
    EXPECT_EQ(name_mismatch->at("error").at("code").to_integer(), -32020);

    // Unsupported protocol header produces -32022
    const auto unsupported{sourcemeta::core::mcp_validate_request_headers(
        legacy_version, "invalid-protocol", std::nullopt, std::nullopt,
        envelope)};
    EXPECT_TRUE(unsupported.has_value());
    EXPECT_EQ(unsupported->at("error").at("code").to_integer(), -32022);
  }
}

TEST(header_validation_safety_cases) {
  // Envelope is not an object
  {
    const auto not_object{sourcemeta::core::JSON{42}};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/list", std::nullopt, not_object)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32600);
  }

  // Missing JSON-RPC method
  {
    const auto no_method{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 1,
      "params": {}
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/list", std::nullopt, no_method)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32600);
  }

  // params is missing for a named method
  {
    const auto no_params{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 1,
      "method": "tools/call"
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", "calculator", no_params)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32602);
  }

  // params is not an object for a named method
  {
    const auto params_array{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 1,
      "method": "tools/call",
      "params": [ 1, 2, 3 ]
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", "calculator", params_array)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32602);
  }

  // Name property has the wrong type
  {
    const auto wrong_name_type{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 1,
      "method": "tools/call",
      "params": {
        "_meta": {
          "io.modelcontextprotocol/protocolVersion": "2026-07-28"
        },
        "name": 42
      }
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", "calculator", wrong_name_type)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32602);
  }

  // Metadata has already failed validation (missing _meta)
  {
    const auto missing_meta{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 1,
      "method": "tools/list",
      "params": {}
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/list", std::nullopt, missing_meta)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
  }
}

TEST(error_resource_not_found_version_aware) {
  const auto identifier{sourcemeta::core::JSON{13}};

  const auto modern_err{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier)};
  EXPECT_EQ(modern_err.at("error").at("code").to_integer(), -32602);
  EXPECT_EQ(modern_err.at("error").at("message").to_string(),
            "Resource not found");

  const auto legacy_err{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier)};
  EXPECT_EQ(legacy_err.at("error").at("code").to_integer(), -32002);
  EXPECT_EQ(legacy_err.at("error").at("message").to_string(),
            "Resource not found");
}

TEST(decorate_result_modern_and_legacy) {
  auto result_legacy{sourcemeta::core::JSON::make_object()};
  result_legacy.assign("status", sourcemeta::core::JSON{"ok"});
  const auto decorated_legacy{sourcemeta::core::mcp_decorate_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      std::move(result_legacy))};
  EXPECT_FALSE(decorated_legacy.defines("resultType"));
  EXPECT_FALSE(decorated_legacy.defines("_meta"));

  auto result_modern{sourcemeta::core::JSON::make_object()};
  result_modern.assign("status", sourcemeta::core::JSON{"ok"});
  const sourcemeta::core::MCPImplementation server{.name = "ExampleServer",
                                                   .version = "1.0.0",
                                                   .title = "Example Title",
                                                   .description = "Description",
                                                   .website_url =
                                                       "https://ex.com"};
  const auto decorated_modern{sourcemeta::core::mcp_decorate_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      std::move(result_modern), server)};
  EXPECT_EQ(decorated_modern.at("resultType").to_string(), "complete");
  EXPECT_TRUE(decorated_modern.defines("_meta"));
  const auto &info{
      decorated_modern.at("_meta").at("io.modelcontextprotocol/serverInfo")};
  EXPECT_EQ(info.at("name").to_string(), "ExampleServer");
  EXPECT_EQ(info.at("version").to_string(), "1.0.0");
  EXPECT_EQ(info.at("title").to_string(), "Example Title");
  EXPECT_EQ(info.at("description").to_string(), "Description");
  EXPECT_EQ(info.at("websiteUrl").to_string(), "https://ex.com");
}

TEST(decorate_cacheable_result_modern_and_legacy) {
  auto res_legacy{sourcemeta::core::JSON::make_object()};
  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 60000, .scope = sourcemeta::core::MCPCacheScope::Private};
  const auto decorated_legacy{sourcemeta::core::mcp_decorate_cacheable_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::move(res_legacy),
      policy)};
  EXPECT_FALSE(decorated_legacy.defines("ttlMs"));
  EXPECT_FALSE(decorated_legacy.defines("cacheScope"));

  auto res_modern{sourcemeta::core::JSON::make_object()};
  const auto decorated_modern{sourcemeta::core::mcp_decorate_cacheable_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::move(res_modern),
      policy)};
  EXPECT_EQ(decorated_modern.at("ttlMs").to_integer(), 60000);
  EXPECT_EQ(decorated_modern.at("cacheScope").to_string(), "private");
}

TEST(tool_success_modern_result_type_and_structured_content) {
  const auto identifier{sourcemeta::core::JSON{100}};

  // Structured content with object
  auto struct_obj{sourcemeta::core::JSON::make_object()};
  struct_obj.assign("foo", sourcemeta::core::JSON{123});
  const auto env_obj{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      std::move(struct_obj))};
  EXPECT_EQ(env_obj.at("result").at("resultType").to_string(), "complete");
  EXPECT_EQ(env_obj.at("result").at("isError").to_boolean(), false);
  EXPECT_EQ(env_obj.at("result").at("structuredContent").at("foo").to_integer(),
            123);

  // Structured content with array
  auto struct_arr{sourcemeta::core::JSON::make_array()};
  struct_arr.push_back(sourcemeta::core::JSON{"item"});
  const auto env_arr{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      std::move(struct_arr))};
  EXPECT_EQ(env_arr.at("result").at("resultType").to_string(), "complete");
  EXPECT_TRUE(env_arr.at("result").at("structuredContent").is_array());

  // Structured content with scalar string
  const auto env_str{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      sourcemeta::core::JSON{"hello scalar"})};
  EXPECT_EQ(env_str.at("result").at("resultType").to_string(), "complete");
  EXPECT_EQ(env_str.at("result").at("structuredContent").to_string(),
            "hello scalar");

  // Structured content with null
  const auto env_null{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      sourcemeta::core::JSON{nullptr})};
  EXPECT_EQ(env_null.at("result").at("resultType").to_string(), "complete");
  EXPECT_TRUE(env_null.at("result").at("structuredContent").is_null());
}

TEST(tool_error_modern_result_type) {
  const auto identifier{sourcemeta::core::JSON{101}};
  const auto modern_error{sourcemeta::core::mcp_make_tool_error(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      "Something failed")};
  EXPECT_EQ(modern_error.at("result").at("resultType").to_string(), "complete");
  EXPECT_EQ(modern_error.at("result").at("isError").to_boolean(), true);
  EXPECT_EQ(
      modern_error.at("result").at("content").at(0).at("text").to_string(),
      "Something failed");

  const auto legacy_error{sourcemeta::core::mcp_make_tool_error(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier,
      "Something failed")};
  EXPECT_FALSE(legacy_error.at("result").defines("resultType"));
  EXPECT_EQ(legacy_error.at("result").at("isError").to_boolean(), true);
}

TEST(resources_read_result_modern_and_legacy) {
  auto contents{sourcemeta::core::JSON::make_array()};
  contents.push_back(sourcemeta::core::mcp_make_resource_text_content(
      "file:///a", "text/plain", "data"));

  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 30000, .scope = sourcemeta::core::MCPCacheScope::Public};
  const auto modern_result{sourcemeta::core::mcp_make_resources_read_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::JSON{contents}, policy)};
  EXPECT_EQ(modern_result.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern_result.at("ttlMs").to_integer(), 30000);
  EXPECT_EQ(modern_result.at("cacheScope").to_string(), "public");
  EXPECT_EQ(modern_result.at("contents").size(), 1);

  const auto legacy_result{sourcemeta::core::mcp_make_resources_read_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::move(contents))};
  EXPECT_FALSE(legacy_result.defines("resultType"));
  EXPECT_FALSE(legacy_result.defines("ttlMs"));
  EXPECT_FALSE(legacy_result.defines("cacheScope"));
}

TEST(server_discover_result) {
  const auto identifier{sourcemeta::core::JSON{1}};
  sourcemeta::core::MCPServerCapabilities caps;
  caps.tools = true;
  caps.tools_list_changed = true;
  caps.resources = true;
  caps.resources_subscribe = true;

  const sourcemeta::core::MCPImplementation server{.name = "TestServer",
                                                   .version = "2.0.0"};
  const sourcemeta::core::MCPCachePolicy cache{
      .ttl_ms = 3600000, .scope = sourcemeta::core::MCPCacheScope::Public};

  const std::vector<sourcemeta::core::JSON::StringView> supported_versions{
      "2026-07-28", "2025-11-25", "2025-06-18", "2025-03-26"};
  const auto envelope{sourcemeta::core::mcp_make_server_discover_result(
      identifier, caps, server, supported_versions, "Instructions here",
      cache)};

  const auto &result{envelope.at("result")};
  EXPECT_EQ(result.at("resultType").to_string(), "complete");
  EXPECT_EQ(result.at("supportedVersions").size(), 4);
  EXPECT_EQ(result.at("supportedVersions").at(0).to_string(), "2026-07-28");
  EXPECT_TRUE(
      result.at("capabilities").at("tools").at("listChanged").to_boolean());
  EXPECT_TRUE(
      result.at("capabilities").at("resources").at("subscribe").to_boolean());
  EXPECT_EQ(result.at("_meta")
                .at("io.modelcontextprotocol/serverInfo")
                .at("name")
                .to_string(),
            "TestServer");
  EXPECT_EQ(result.at("_meta")
                .at("io.modelcontextprotocol/serverInfo")
                .at("version")
                .to_string(),
            "2.0.0");
  EXPECT_EQ(result.at("instructions").to_string(), "Instructions here");
  EXPECT_EQ(result.at("ttlMs").to_integer(), 3600000);
  EXPECT_EQ(result.at("cacheScope").to_string(), "public");
}

TEST(initialize_negotiation_with_modern_request) {
  // If an older client calls initialize with protocolVersion = "2026-07-28",
  // the server MUST NOT negotiate 2026-07-28 because initialize was removed.
  // It must negotiate the latest handshake-compatible version (2025-11-25).
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2026-07-28" }
  })JSON")};

  const sourcemeta::core::MCPServerCapabilities caps;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto envelope{
      sourcemeta::core::mcp_make_initialize_result(request, caps, server)};

  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
  EXPECT_FALSE(envelope.at("result").defines("resultType"));
  EXPECT_FALSE(envelope.at("result").defines("_meta"));
}

TEST(initialize_negotiation_with_unknown_version) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "3000-01-01" }
  })JSON")};

  const sourcemeta::core::MCPServerCapabilities caps;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto envelope{
      sourcemeta::core::mcp_make_initialize_result(request, caps, server)};

  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
}

TEST(mrtr_input_required_result) {
  const auto identifier{sourcemeta::core::JSON{50}};
  auto input_requests{sourcemeta::core::JSON::make_object()};
  auto req{sourcemeta::core::JSON::make_object()};
  req.assign("type", sourcemeta::core::JSON{"elicitation"});
  req.assign("message", sourcemeta::core::JSON{"Confirm delete?"});
  input_requests.assign("confirm", std::move(req));

  const auto envelope{sourcemeta::core::mcp_make_input_required_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call",
      identifier, std::move(input_requests), "opaque-state-token-123")};

  const auto &result{envelope.at("result")};
  EXPECT_EQ(result.at("resultType").to_string(), "input_required");
  EXPECT_EQ(result.at("inputRequests").at("confirm").at("message").to_string(),
            "Confirm delete?");
  EXPECT_EQ(result.at("requestState").to_string(), "opaque-state-token-123");
}

TEST(mrtr_request_accessors) {
  const auto continuation{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 51,
    "method": "tools/call",
    "params": {
      "name": "my-tool",
      "requestState": "token-xyz",
      "inputResponses": {
        "confirm": { "value": true }
      }
    }
  })JSON")};

  EXPECT_EQ(sourcemeta::core::mcp_request_state(continuation), "token-xyz");
  const auto *responses{
      sourcemeta::core::mcp_request_input_responses(continuation)};
  EXPECT_NE(responses, nullptr);
  EXPECT_TRUE(responses->is_object());
  EXPECT_EQ(responses->at("confirm").at("value").to_boolean(), true);
}

TEST(subscriptions_acknowledged_notification) {
  auto notifs{sourcemeta::core::JSON::make_object()};
  notifs.assign("toolsListChanged", sourcemeta::core::JSON{true});

  const auto envelope{
      sourcemeta::core::mcp_make_subscription_acknowledged_notification(
          "sub-42", std::move(notifs))};

  EXPECT_EQ(envelope.at("jsonrpc").to_string(), "2.0");
  EXPECT_EQ(envelope.at("method").to_string(),
            "notifications/subscriptions/acknowledged");
  EXPECT_EQ(envelope.at("params")
                .at("_meta")
                .at("io.modelcontextprotocol/subscriptionId")
                .to_string(),
            "sub-42");
  EXPECT_TRUE(envelope.at("params")
                  .at("notifications")
                  .at("toolsListChanged")
                  .to_boolean());

  // Also test integer subscription ID
  auto notifs_int{sourcemeta::core::JSON::make_object()};
  const auto envelope_int{
      sourcemeta::core::mcp_make_subscription_acknowledged_notification(
          sourcemeta::core::JSON{123}, std::move(notifs_int))};
  EXPECT_EQ(envelope_int.at("params")
                .at("_meta")
                .at("io.modelcontextprotocol/subscriptionId")
                .to_integer(),
            123);
}

TEST(subscriptions_close_result) {
  const auto identifier{sourcemeta::core::JSON{60}};
  const auto envelope{
      sourcemeta::core::mcp_make_subscription_close_result(identifier)};

  EXPECT_EQ(envelope.at("result").at("resultType").to_string(), "complete");
  EXPECT_EQ(envelope.at("result")
                .at("_meta")
                .at("io.modelcontextprotocol/subscriptionId")
                .to_integer(),
            60);
}

TEST(subscriptions_id_accessor) {
  const auto notification{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": "stream-99"
      }
    }
  })JSON")};
  const auto *sub_id{
      sourcemeta::core::mcp_request_subscription_id(notification)};
  EXPECT_NE(sub_id, nullptr);
  EXPECT_EQ(sub_id->to_string(), "stream-99");

  const auto close_result{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "resultType": "complete",
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": "stream-99"
      }
    }
  })JSON")};
  const auto *close_sub_id{
      sourcemeta::core::mcp_request_subscription_id(close_result)};
  EXPECT_NE(close_sub_id, nullptr);
  EXPECT_EQ(close_sub_id->to_string(), "stream-99");

  // Number subscription ID
  const auto num_envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": 42
      }
    }
  })JSON")};
  const auto *num_sub_id{
      sourcemeta::core::mcp_request_subscription_id(num_envelope)};
  EXPECT_NE(num_sub_id, nullptr);
  EXPECT_EQ(num_sub_id->to_integer(), 42);
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

TEST(client_capabilities_parsing_and_serialization) {
  sourcemeta::core::MCPClientCapabilities caps;
  caps.roots = true;
  caps.roots_list_changed = true;
  caps.sampling = true;
  caps.sampling_context = true;
  caps.sampling_tools = true;
  caps.elicitation = true;
  caps.elicitation_form = true;
  caps.elicitation_url = true;

  auto ext{sourcemeta::core::JSON::make_object()};
  ext.assign("customCapability", sourcemeta::core::JSON{true});
  caps.extensions = std::move(ext);

  const auto serialized_legacy{
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, caps)};
  EXPECT_TRUE(serialized_legacy.at("roots").at("listChanged").to_boolean());
  EXPECT_TRUE(serialized_legacy.defines("sampling"));
  EXPECT_TRUE(serialized_legacy.at("sampling").defines("context"));
  EXPECT_TRUE(serialized_legacy.at("sampling").defines("tools"));
  EXPECT_TRUE(serialized_legacy.defines("elicitation"));
  EXPECT_TRUE(serialized_legacy.at("elicitation").defines("form"));
  EXPECT_TRUE(serialized_legacy.at("elicitation").defines("url"));
  EXPECT_TRUE(
      serialized_legacy.at("extensions").at("customCapability").to_boolean());

  const auto serialized_modern{
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, caps)};
  EXPECT_FALSE(serialized_modern.at("roots").defines("listChanged"));
  EXPECT_TRUE(serialized_modern.defines("sampling"));
  EXPECT_TRUE(serialized_modern.at("sampling").defines("context"));
  EXPECT_TRUE(serialized_modern.at("sampling").defines("tools"));
  EXPECT_TRUE(serialized_modern.defines("elicitation"));
  EXPECT_TRUE(serialized_modern.at("elicitation").defines("form"));
  EXPECT_TRUE(serialized_modern.at("elicitation").defines("url"));

  const auto parsed{
      sourcemeta::core::mcp_parse_client_capabilities(serialized_legacy)};
  EXPECT_TRUE(parsed.roots);
  EXPECT_TRUE(parsed.roots_list_changed);
  EXPECT_TRUE(parsed.sampling);
  EXPECT_TRUE(parsed.sampling_context);
  EXPECT_TRUE(parsed.sampling_tools);
  EXPECT_TRUE(parsed.elicitation);
  EXPECT_TRUE(parsed.elicitation_form);
  EXPECT_TRUE(parsed.elicitation_url);
  EXPECT_TRUE(parsed.extensions.has_value());
  EXPECT_TRUE(parsed.extensions->at("customCapability").to_boolean());
}

TEST(request_meta_accessors) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 10,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": { "roots": { "listChanged": true } },
        "io.modelcontextprotocol/clientInfo": {
          "name": "tester",
          "version": "1.0.0"
        },
        "io.modelcontextprotocol/logLevel": "info"
      },
      "name": "calc"
    }
  })JSON")};

  EXPECT_TRUE(sourcemeta::core::mcp_has_required_request_meta(envelope));

  const auto protocol_version{
      sourcemeta::core::mcp_request_protocol_version(envelope)};
  EXPECT_TRUE(protocol_version.has_value());
  EXPECT_EQ(protocol_version.value(),
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);

  const auto client_info{sourcemeta::core::mcp_request_client_info(envelope)};
  EXPECT_TRUE(client_info.has_value());
  EXPECT_EQ(client_info->name, "tester");
  EXPECT_EQ(client_info->version, "1.0.0");

  const auto *caps{sourcemeta::core::mcp_request_client_capabilities(envelope)};
  EXPECT_NE(caps, nullptr);
  EXPECT_TRUE(caps->at("roots").at("listChanged").to_boolean());

  const auto log_level{sourcemeta::core::mcp_request_log_level(envelope)};
  EXPECT_TRUE(log_level.has_value());
  EXPECT_EQ(log_level.value(), "info");

  // Invalid request
  const auto invalid_req{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 11, "method": "tools/call"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_has_required_request_meta(invalid_req));
  EXPECT_EQ(sourcemeta::core::mcp_request_protocol_version(invalid_req),
            std::nullopt);
  EXPECT_EQ(sourcemeta::core::mcp_request_client_info(invalid_req),
            std::nullopt);
  EXPECT_EQ(sourcemeta::core::mcp_request_client_capabilities(invalid_req),
            nullptr);
  EXPECT_EQ(sourcemeta::core::mcp_request_log_level(invalid_req), std::nullopt);
}

TEST(header_validation_and_mismatch_error) {
  const auto req{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "method": "tools/call",
    "params": {
      "name": "my-tool"
    }
  })JSON")};

  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};

  // Matching headers
  const auto ok_res{sourcemeta::core::mcp_validate_request_headers(
      version, std::nullopt, "tools/call", "my-tool", req)};
  EXPECT_FALSE(ok_res.has_value());

  // Method mismatch
  const auto method_err{sourcemeta::core::mcp_validate_request_headers(
      version, std::nullopt, "tools/list", "my-tool", req)};
  EXPECT_TRUE(method_err.has_value());
  EXPECT_EQ(method_err->at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(method_err->at("error").at("data").at("header").to_string(),
            "Mcp-Method");
  EXPECT_EQ(method_err->at("error").at("data").at("headerValue").to_string(),
            "tools/list");
  EXPECT_EQ(method_err->at("error").at("data").at("bodyValue").to_string(),
            "tools/call");

  // Name mismatch
  const auto name_err{sourcemeta::core::mcp_validate_request_headers(
      version, std::nullopt, "tools/call", "other-tool", req)};
  EXPECT_TRUE(name_err.has_value());
  EXPECT_EQ(name_err->at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(name_err->at("error").at("data").at("header").to_string(),
            "Mcp-Name");
  EXPECT_EQ(name_err->at("error").at("data").at("headerValue").to_string(),
            "other-tool");
  EXPECT_EQ(name_err->at("error").at("data").at("bodyValue").to_string(),
            "my-tool");
}

TEST(tools_list_result_modern_and_legacy) {
  auto tools{sourcemeta::core::JSON::make_array()};
  tools.push_back(sourcemeta::core::mcp_make_tool_descriptor(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "calc", "Calculator",
      sourcemeta::core::parse_json(R"({ "type": "object" })")));

  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public};

  // Modern: has resultType, nextCursor, ttlMs, cacheScope
  const auto modern_result{sourcemeta::core::mcp_make_tools_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::JSON{tools}, "cursor-abc", policy)};
  EXPECT_EQ(modern_result.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern_result.at("nextCursor").to_string(), "cursor-abc");
  EXPECT_EQ(modern_result.at("ttlMs").to_integer(), 120000);
  EXPECT_EQ(modern_result.at("cacheScope").to_string(), "public");
  EXPECT_EQ(modern_result.at("tools").size(), 1);

  // Legacy: omits resultType, ttlMs, cacheScope
  const auto legacy_result{sourcemeta::core::mcp_make_tools_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::move(tools),
      "cursor-abc", policy)};
  EXPECT_FALSE(legacy_result.defines("resultType"));
  EXPECT_FALSE(legacy_result.defines("ttlMs"));
  EXPECT_FALSE(legacy_result.defines("cacheScope"));
  EXPECT_EQ(legacy_result.at("nextCursor").to_string(), "cursor-abc");

  const auto legacy_no_policy{sourcemeta::core::mcp_make_tools_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON::make_array(), "cursor-no-policy")};
  EXPECT_FALSE(legacy_no_policy.defines("resultType"));
  EXPECT_EQ(legacy_no_policy.at("nextCursor").to_string(), "cursor-no-policy");
}

TEST(resources_list_result_modern_and_legacy) {
  auto resources{sourcemeta::core::JSON::make_array()};
  resources.push_back(sourcemeta::core::mcp_make_resource("file:///doc.txt",
                                                          "doc", "text/plain"));

  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 60000, .scope = sourcemeta::core::MCPCacheScope::Private};

  const auto modern_result{sourcemeta::core::mcp_make_resources_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::JSON{resources}, "cursor-123", policy)};
  EXPECT_EQ(modern_result.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern_result.at("nextCursor").to_string(), "cursor-123");
  EXPECT_EQ(modern_result.at("ttlMs").to_integer(), 60000);
  EXPECT_EQ(modern_result.at("cacheScope").to_string(), "private");

  const auto legacy_result{sourcemeta::core::mcp_make_resources_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::move(resources),
      "cursor-123", policy)};
  EXPECT_FALSE(legacy_result.defines("resultType"));
  EXPECT_FALSE(legacy_result.defines("ttlMs"));
  EXPECT_FALSE(legacy_result.defines("cacheScope"));
  EXPECT_EQ(legacy_result.at("nextCursor").to_string(), "cursor-123");

  const auto legacy_no_policy{sourcemeta::core::mcp_make_resources_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON::make_array(), "cursor-no-policy")};
  EXPECT_FALSE(legacy_no_policy.defines("resultType"));
  EXPECT_EQ(legacy_no_policy.at("nextCursor").to_string(), "cursor-no-policy");
}

TEST(resource_templates_list_result_modern_and_legacy) {
  auto templates{sourcemeta::core::JSON::make_array()};
  templates.push_back(sourcemeta::core::mcp_make_resource_template(
      "file:///{path}", "Files", "File access", "text/plain"));

  const sourcemeta::core::MCPCachePolicy tmpl_policy{
      .ttl_ms = 45000, .scope = sourcemeta::core::MCPCacheScope::Public};
  const auto modern_result{
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{templates}, "cursor-tmpl", tmpl_policy)};
  EXPECT_EQ(modern_result.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern_result.at("nextCursor").to_string(), "cursor-tmpl");
  EXPECT_EQ(modern_result.at("ttlMs").to_integer(), 45000);
  EXPECT_EQ(modern_result.at("cacheScope").to_string(), "public");

  const auto legacy_result{
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          std::move(templates), "cursor-tmpl")};
  EXPECT_FALSE(legacy_result.defines("resultType"));
  EXPECT_EQ(legacy_result.at("nextCursor").to_string(), "cursor-tmpl");

  const auto modern_with_policy{
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON::make_array(), "cursor-tmpl-policy",
          tmpl_policy)};
  EXPECT_EQ(modern_with_policy.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern_with_policy.at("ttlMs").to_integer(), 45000);
  EXPECT_EQ(modern_with_policy.at("cacheScope").to_string(), "public");
}

TEST(list_results_preserve_empty_next_cursor) {
  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public};

  // Tools list
  const auto tools_modern{sourcemeta::core::mcp_make_tools_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::JSON::make_array(), "", policy)};
  EXPECT_TRUE(tools_modern.defines("nextCursor"));
  EXPECT_EQ(tools_modern.at("nextCursor").to_string(), "");

  const auto tools_legacy{sourcemeta::core::mcp_make_tools_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON::make_array(), "")};
  EXPECT_TRUE(tools_legacy.defines("nextCursor"));
  EXPECT_EQ(tools_legacy.at("nextCursor").to_string(), "");

  // Resources list
  const auto resources_modern{sourcemeta::core::mcp_make_resources_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::JSON::make_array(), "", policy)};
  EXPECT_TRUE(resources_modern.defines("nextCursor"));
  EXPECT_EQ(resources_modern.at("nextCursor").to_string(), "");

  const auto resources_legacy{sourcemeta::core::mcp_make_resources_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON::make_array(), "")};
  EXPECT_TRUE(resources_legacy.defines("nextCursor"));
  EXPECT_EQ(resources_legacy.at("nextCursor").to_string(), "");

  // Resource templates list
  const auto tmpls_modern{
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON::make_array(), "", policy)};
  EXPECT_TRUE(tmpls_modern.defines("nextCursor"));
  EXPECT_EQ(tmpls_modern.at("nextCursor").to_string(), "");

  const auto tmpls_legacy{
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON::make_array(), "")};
  EXPECT_TRUE(tmpls_legacy.defines("nextCursor"));
  EXPECT_EQ(tmpls_legacy.at("nextCursor").to_string(), "");

  // Prompts list
  const auto prompts_modern{sourcemeta::core::mcp_make_prompts_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::JSON::make_array(), "", policy)};
  EXPECT_TRUE(prompts_modern.defines("nextCursor"));
  EXPECT_EQ(prompts_modern.at("nextCursor").to_string(), "");

  const auto prompts_legacy{sourcemeta::core::mcp_make_prompts_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON::make_array(), "")};
  EXPECT_TRUE(prompts_legacy.defines("nextCursor"));
  EXPECT_EQ(prompts_legacy.at("nextCursor").to_string(), "");
}

TEST(make_tool_descriptor_2026_07_28_arbitrary_json_schema) {
  // Arbitrary JSON Schema 2020-12 keywords
  const auto input_schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "$defs": {
      "item": { "type": "string" }
    },
    "properties": {
      "tags": {
        "type": "array",
        "prefixItems": [ { "$ref": "#/$defs/item" } ]
      }
    },
    "dependentRequired": {
      "tags": [ "name" ]
    },
    "unevaluatedProperties": false
  })JSON")};

  const auto output_schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "properties": {
      "status": { "type": "string" }
    }
  })JSON")};

  const auto descriptor{sourcemeta::core::mcp_make_tool_descriptor(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "advanced_tool",
      "Tool using modern JSON Schema", input_schema, output_schema)};

  EXPECT_EQ(descriptor.at("name").to_string(), "advanced_tool");
  EXPECT_TRUE(descriptor.at("inputSchema").defines("$defs"));
  EXPECT_TRUE(descriptor.at("inputSchema").defines("dependentRequired"));
  EXPECT_TRUE(descriptor.at("inputSchema").defines("unevaluatedProperties"));
  EXPECT_TRUE(descriptor.defines("outputSchema"));
  EXPECT_EQ(descriptor.at("outputSchema").at("type").to_string(), "object");
}

TEST(initialize_negotiation_with_2024_11_05_legacy_date) {
  // A legacy client from 2024-11-05 requesting initialize falls back to
  // the latest handshake-compatible version (2025-11-25).
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2024-11-05" }
  })JSON")};

  const sourcemeta::core::MCPServerCapabilities caps;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto envelope{
      sourcemeta::core::mcp_make_initialize_result(request, caps, server)};

  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
  EXPECT_FALSE(envelope.at("result").defines("resultType"));
  EXPECT_FALSE(envelope.at("result").defines("_meta"));
}

TEST(error_request_meta_generation) {
  const auto identifier{sourcemeta::core::JSON{1}};
  const auto error{sourcemeta::core::mcp_make_error_request_meta(
      identifier, sourcemeta::core::MCPRequestMetaStatus::MissingMeta)};
  EXPECT_EQ(error.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_PARAMS);
  EXPECT_EQ(error.at("id").to_integer(), 1);

  const auto unsupported_error{sourcemeta::core::mcp_make_error_request_meta(
      identifier,
      sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion,
      "2024-01-01", {"2026-07-28"})};
  EXPECT_EQ(unsupported_error.at("error").at("code").to_integer(), -32022);
  EXPECT_EQ(
      unsupported_error.at("error").at("data").at("requested").to_string(),
      "2024-01-01");
  EXPECT_EQ(unsupported_error.at("error")
                .at("data")
                .at("supported")
                .at(0)
                .to_string(),
            "2026-07-28");
}

TEST(error_code_to_http_status_mapping) {
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::JSONRPC_CODE_PARSE),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND),
            sourcemeta::core::HTTP_STATUS_NOT_FOUND);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::JSONRPC_CODE_INVALID_PARAMS),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::JSONRPC_CODE_INTERNAL),
            sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCP_CODE_HEADER_MISMATCH),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(-99999),
            sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR);
}

TEST(empty_result_builder) {
  const auto identifier{sourcemeta::core::JSON{100}};
  const auto legacy{sourcemeta::core::mcp_make_empty_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier)};
  EXPECT_TRUE(legacy.at("result").is_object());
  EXPECT_FALSE(legacy.at("result").defines("resultType"));
  EXPECT_FALSE(legacy.at("result").defines("_meta"));
  EXPECT_EQ(legacy.at("id").to_integer(), 100);

  const auto modern{sourcemeta::core::mcp_make_empty_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier)};
  EXPECT_TRUE(modern.at("result").is_object());
  EXPECT_EQ(modern.at("result").at("resultType").to_string(), "complete");
  EXPECT_FALSE(modern.at("result").defines("_meta"));
  EXPECT_EQ(modern.at("id").to_integer(), 100);

  const sourcemeta::core::MCPImplementation srv{.name = "srv",
                                                .version = "1.0.0"};
  const auto modern_with_info{sourcemeta::core::mcp_make_empty_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier, srv)};
  EXPECT_TRUE(modern_with_info.at("result").defines("_meta"));
  EXPECT_EQ(modern_with_info.at("result")
                .at("_meta")
                .at("io.modelcontextprotocol/serverInfo")
                .at("name")
                .to_string(),
            "srv");
}

TEST(in_place_result_decoration) {
  auto legacy_payload{sourcemeta::core::JSON::make_object()};
  legacy_payload.assign("key", sourcemeta::core::JSON{"value"});
  sourcemeta::core::mcp_decorate_result_in_place(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, legacy_payload);
  EXPECT_FALSE(legacy_payload.defines("resultType"));
  EXPECT_FALSE(legacy_payload.defines("_meta"));
  EXPECT_EQ(legacy_payload.at("key").to_string(), "value");

  auto modern_payload{sourcemeta::core::JSON::make_object()};
  modern_payload.assign("key", sourcemeta::core::JSON{"value"});
  sourcemeta::core::mcp_decorate_result_in_place(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, modern_payload);
  EXPECT_EQ(modern_payload.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern_payload.at("key").to_string(), "value");

  // In-place cacheable decoration
  auto modern_cacheable{sourcemeta::core::JSON::make_object()};
  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 5000, .scope = sourcemeta::core::MCPCacheScope::Private};
  sourcemeta::core::mcp_decorate_cacheable_result_in_place(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, modern_cacheable,
      policy);
  EXPECT_EQ(modern_cacheable.at("ttlMs").to_integer(), 5000);
  EXPECT_EQ(modern_cacheable.at("cacheScope").to_string(), "private");
}

TEST(prompts_list_result_builder) {
  auto prompts{sourcemeta::core::JSON::make_array()};
  auto prompt1{sourcemeta::core::JSON::make_object()};
  prompt1.assign("name", sourcemeta::core::JSON{"test-prompt"});
  prompts.push_back(std::move(prompt1));

  const auto legacy{sourcemeta::core::mcp_make_prompts_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, prompts,
      "next-cursor")};
  EXPECT_FALSE(legacy.defines("resultType"));
  EXPECT_FALSE(legacy.defines("_meta"));
  EXPECT_EQ(legacy.at("prompts").size(), 1);
  EXPECT_EQ(legacy.at("nextCursor").to_string(), "next-cursor");

  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public};
  const auto modern{sourcemeta::core::mcp_make_prompts_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, prompts, std::nullopt,
      policy)};
  EXPECT_EQ(modern.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern.at("prompts").size(), 1);
  EXPECT_FALSE(modern.defines("nextCursor"));
  EXPECT_EQ(modern.at("ttlMs").to_integer(), 10000);
  EXPECT_EQ(modern.at("cacheScope").to_string(), "public");
}

TEST(request_meta_validation_empty_protocol_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};

  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion);
}

TEST(base64_header_malformed_rejected_on_2026) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calc"
    }
  })JSON")};

  // Malformed base64 in Mcp-Name header on 2026-07-28
  const auto error{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "tools/call", "base64:%%%invalid-chars&&&", request)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error->at("error").at("code").to_integer(),
            sourcemeta::core::MCP_CODE_HEADER_MISMATCH);
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

TEST(request_name_from_body_prefers_uri_for_resources_read) {
  const auto envelope_with_both{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "resources/read",
    "params": { "name": "ignored-name", "uri": "file:///path/to/resource" }
  })JSON")};
  const auto extracted_uri{
      sourcemeta::core::mcp_request_name_from_body(envelope_with_both)};
  EXPECT_TRUE(extracted_uri.has_value());
  EXPECT_EQ(extracted_uri.value(), "file:///path/to/resource");

  const auto envelope_uri_only{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 2,
    "method": "resources/read",
    "params": { "uri": "file:///path/only" }
  })JSON")};
  const auto extracted_uri_only{
      sourcemeta::core::mcp_request_name_from_body(envelope_uri_only)};
  EXPECT_TRUE(extracted_uri_only.has_value());
  EXPECT_EQ(extracted_uri_only.value(), "file:///path/only");
}

TEST(request_meta_validation_accepts_params_with_jsonrpc_property) {
  const auto params{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "custom-param-value",
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(params)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_EQ(meta->protocol_version,
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
}

TEST(decorate_result_replaces_non_object_meta) {
  auto result{sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "_meta": "string-not-object"
  })JSON")};
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  sourcemeta::core::mcp_decorate_result_in_place(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, result, server);

  EXPECT_TRUE(result.at("_meta").is_object());
  EXPECT_TRUE(result.at("_meta").defines("io.modelcontextprotocol/serverInfo"));
}

TEST(cache_policy_rejects_negative_ttl_ms) {
  auto result{sourcemeta::core::JSON::make_object()};
  const sourcemeta::core::MCPCachePolicy negative_policy{
      .ttl_ms = -500, .scope = sourcemeta::core::MCPCacheScope::Public};
  sourcemeta::core::mcp_decorate_cacheable_result_in_place(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, result,
      negative_policy);

  EXPECT_FALSE(result.defines("ttlMs"));
  EXPECT_FALSE(result.defines("cacheScope"));
}

TEST(capabilities_serializer_ignores_non_object_extensions) {
  sourcemeta::core::MCPClientCapabilities client_caps;
  client_caps.extensions = sourcemeta::core::JSON{"not-an-object"};
  client_caps.experimental = sourcemeta::core::JSON::make_array();

  const auto serialized_client{
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, client_caps)};
  EXPECT_FALSE(serialized_client.defines("extensions"));
  EXPECT_FALSE(serialized_client.defines("experimental"));

  sourcemeta::core::MCPServerCapabilities server_caps;
  server_caps.extensions = sourcemeta::core::JSON{123};
  server_caps.experimental = sourcemeta::core::JSON{false};

  const auto serialized_server{sourcemeta::core::mcp_make_tools_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON::make_array())};
  EXPECT_FALSE(serialized_server.defines("extensions"));
}
