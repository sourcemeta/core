#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <gtest/gtest.h>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::int64_t
#include <limits>   // std::numeric_limits
#include <optional> // std::optional, std::nullopt
#include <utility>  // std::move

TEST(MCP, protocol_version_string_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_protocol_version_string(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26),
            "2025-03-26");
}

TEST(MCP, protocol_version_string_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_protocol_version_string(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18),
            "2025-06-18");
}

TEST(MCP, protocol_version_string_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_protocol_version_string(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25),
            "2025-11-25");
}

TEST(MCP, method_initialize) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_INITIALIZE, "initialize");
}

TEST(MCP, method_ping) { EXPECT_EQ(sourcemeta::core::MCP_METHOD_PING, "ping"); }

TEST(MCP, method_tools_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TOOLS_LIST, "tools/list");
}

TEST(MCP, method_tools_call) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_TOOLS_CALL, "tools/call");
}

TEST(MCP, method_resources_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_LIST, "resources/list");
}

TEST(MCP, method_resources_read) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_READ, "resources/read");
}

TEST(MCP, method_resources_templates_list) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_RESOURCES_TEMPLATES_LIST,
            "resources/templates/list");
}

TEST(MCP, method_notifications_initialized) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_INITIALIZED,
            "notifications/initialized");
}

TEST(MCP, code_resource_not_found) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
            static_cast<std::int64_t>(-32002));
}

TEST(MCP, code_url_elicitation_required) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_URL_ELICITATION_REQUIRED,
            static_cast<std::int64_t>(-32042));
}

TEST(MCP, is_request_method_initialize) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("initialize"));
}

TEST(MCP, is_request_method_ping) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("ping"));
}

TEST(MCP, is_request_method_tools_list) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("tools/list"));
}

TEST(MCP, is_request_method_tools_call) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("tools/call"));
}

TEST(MCP, is_request_method_resources_list) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("resources/list"));
}

TEST(MCP, is_request_method_resources_read) {
  EXPECT_TRUE(sourcemeta::core::mcp_is_request_method("resources/read"));
}

TEST(MCP, is_request_method_resources_templates_list) {
  EXPECT_TRUE(
      sourcemeta::core::mcp_is_request_method("resources/templates/list"));
}

TEST(MCP, is_request_method_notifications_initialized_is_not_request) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_is_request_method("notifications/initialized"));
}

TEST(MCP, is_request_method_unknown) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method("foo/bar"));
}

TEST(MCP, is_request_method_empty) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(""));
}

TEST(MCP, resolve_protocol_version_empty_defaults_to_2025_03_26) {
  const auto result{sourcemeta::core::mcp_resolve_protocol_version("")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2025_03_26);
}

TEST(MCP, resolve_protocol_version_2025_03_26) {
  const auto result{
      sourcemeta::core::mcp_resolve_protocol_version("2025-03-26")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2025_03_26);
}

TEST(MCP, resolve_protocol_version_2025_06_18) {
  const auto result{
      sourcemeta::core::mcp_resolve_protocol_version("2025-06-18")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2025_06_18);
}

TEST(MCP, resolve_protocol_version_2025_11_25) {
  const auto result{
      sourcemeta::core::mcp_resolve_protocol_version("2025-11-25")};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::MCPProtocolVersion::V_2025_11_25);
}

TEST(MCP, resolve_protocol_version_unknown) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("9999-01-01").has_value());
}

TEST(MCP, resolve_protocol_version_malformed) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_protocol_version("not-a-date").has_value());
}

TEST(MCP, supports_output_schema_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_output_schema(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(MCP, supports_output_schema_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_output_schema(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(MCP, supports_output_schema_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_output_schema(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(MCP, supports_structured_content_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_structured_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(MCP, supports_structured_content_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_structured_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(MCP, supports_structured_content_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_structured_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(MCP, supports_resource_link_content_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_resource_link_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(MCP, supports_resource_link_content_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_resource_link_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(MCP, supports_resource_link_content_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_resource_link_content(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(MCP, supports_implementation_title_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_title(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(MCP, supports_implementation_title_2025_06_18) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_title(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(MCP, supports_implementation_title_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_title(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(MCP, supports_implementation_description_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_description(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(MCP, supports_implementation_description_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_description(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(MCP, supports_implementation_description_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_description(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(MCP, supports_implementation_website_url_2025_03_26) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_website_url(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(MCP, supports_implementation_website_url_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_implementation_website_url(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(MCP, supports_implementation_website_url_2025_11_25) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_implementation_website_url(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(MCP, supports_jsonrpc_batching_2025_03_26) {
  EXPECT_TRUE(sourcemeta::core::mcp_supports_jsonrpc_batching(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26));
}

TEST(MCP, supports_jsonrpc_batching_2025_06_18) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_jsonrpc_batching(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18));
}

TEST(MCP, supports_jsonrpc_batching_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_supports_jsonrpc_batching(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25));
}

TEST(MCP, make_text_block) {
  const auto block{sourcemeta::core::mcp_make_text_block("hello")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "hello"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP, make_text_block_empty_string) {
  const auto block{sourcemeta::core::mcp_make_text_block("")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": ""
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP, make_text_block_with_newlines) {
  const auto block{sourcemeta::core::mcp_make_text_block("line1\nline2")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "line1\nline2"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP, make_resource_link_2025_11_25_full) {
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

TEST(MCP, make_resource_link_2025_11_25_without_name_and_description) {
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

TEST(MCP, make_resource_link_2025_06_18_supports_structured) {
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

TEST(MCP, make_resource_link_2025_03_26_falls_back_to_text_with_name) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///foo",
      "text/plain", "My File", "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "My File\nfile:///foo\nA description"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP, make_resource_link_2025_03_26_falls_back_to_text_without_name) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///foo",
      "text/plain", {}, "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "file:///foo\nA description"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP,
     make_resource_link_2025_03_26_falls_back_to_text_without_description) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///foo",
      "text/plain", "My File")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "My File\nfile:///foo"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP, make_resource_link_2025_03_26_falls_back_to_text_uri_only) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///foo",
      "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "file:///foo"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP,
     make_resource_link_2025_03_26_falls_back_handles_parentheses_in_name) {
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

TEST(MCP,
     make_resource_link_2025_03_26_falls_back_preserves_empty_uri_position) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "", "text/plain",
      "My File", "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "My File\n\nA description"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP,
     make_resource_link_2025_03_26_falls_back_empty_uri_with_description_only) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "", "text/plain", {},
      "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "\nA description"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP, make_resource_link_2025_03_26_falls_back_empty_uri_with_name_only) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "", "text/plain",
      "My File")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "text",
    "text": "My File\n"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(MCP, tool_success_with_object_result) {
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

TEST(MCP, tool_success_with_array_result) {
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

TEST(MCP, tool_success_with_null_id) {
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

TEST(MCP, tool_success_2025_03_26_omits_structured_content) {
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

TEST(MCP, tool_success_with_explicit_content_blocks) {
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

TEST(MCP, tool_success_with_explicit_blocks_omits_structured_on_2025_03_26) {
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

TEST(MCP, tool_error_with_message) {
  const auto identifier{sourcemeta::core::JSON{7}};
  const auto envelope{
      sourcemeta::core::mcp_make_tool_error(identifier, "Schema not found")};
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

TEST(MCP, tool_error_with_string_id) {
  const auto identifier{sourcemeta::core::JSON{"req-1"}};
  const auto envelope{
      sourcemeta::core::mcp_make_tool_error(identifier, "Invalid input")};
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

TEST(MCP, tool_error_with_null_id) {
  const auto envelope{sourcemeta::core::mcp_make_tool_error(
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

TEST(MCP, error_resource_not_found_with_integer_id) {
  const auto identifier{sourcemeta::core::JSON{3}};
  const auto envelope{
      sourcemeta::core::mcp_make_error_resource_not_found(identifier)};
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

TEST(MCP, error_resource_not_found_with_string_id) {
  const auto identifier{sourcemeta::core::JSON{"req-7"}};
  const auto envelope{
      sourcemeta::core::mcp_make_error_resource_not_found(identifier)};
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

TEST(MCP, make_resource_full) {
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

TEST(MCP, make_resource_without_description) {
  const auto resource{
      sourcemeta::core::mcp_make_resource("file:///a", "Alpha", "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(MCP, make_resource_without_size) {
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

TEST(MCP, make_resource_with_priority) {
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

TEST(MCP, make_resource_with_priority_without_size) {
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

TEST(MCP, make_resource_with_priority_clamped_high) {
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

TEST(MCP, make_resource_with_priority_clamped_low) {
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

TEST(MCP, make_resource_with_priority_positive_infinity) {
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

TEST(MCP, make_resource_with_priority_negative_infinity) {
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

TEST(MCP, make_resource_with_priority_nan) {
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

TEST(MCP, make_resource_text_content) {
  const auto content{sourcemeta::core::mcp_make_resource_text_content(
      "file:///a", "text/plain", "Hello")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "mimeType": "text/plain",
    "text": "Hello"
  })JSON")};
  EXPECT_EQ(content, expected);
}

TEST(MCP, make_resources_read_result_single) {
  auto contents{sourcemeta::core::JSON::make_array()};
  contents.push_back(sourcemeta::core::mcp_make_resource_text_content(
      "file:///a", "text/plain", "Hello"));
  const auto result{
      sourcemeta::core::mcp_make_resources_read_result(std::move(contents))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "contents": [
      { "uri": "file:///a", "mimeType": "text/plain", "text": "Hello" }
    ]
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(MCP, make_resources_read_result_empty) {
  const auto result{sourcemeta::core::mcp_make_resources_read_result(
      sourcemeta::core::JSON::make_array())};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "contents": []
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(MCP, make_resource_template) {
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

TEST(MCP, make_tool_descriptor_default_annotations_2025_11_25) {
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

TEST(MCP, make_tool_descriptor_with_output_schema_2025_11_25) {
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

TEST(MCP, make_tool_descriptor_with_output_schema_dropped_on_2025_03_26) {
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

TEST(MCP, make_tool_descriptor_with_title_and_read_only_hints) {
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

TEST(MCP, make_initialize_result_minimal_2025_11_25) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-11-25" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", {}, {}, {}};
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

TEST(MCP, make_initialize_result_with_all_capabilities) {
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
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", {}, {}, {}};
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

TEST(MCP, make_initialize_result_includes_instructions_when_provided) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-11-25" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", {}, {}, {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, "Be careful.")};
  EXPECT_EQ(envelope.at("result").at("instructions").to_string(),
            "Be careful.");
}

TEST(MCP, make_initialize_result_includes_title_on_2025_06_18) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-06-18" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", "Server",
                                                   "desc", "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("result").at("serverInfo").at("title").to_string(),
            "Server");
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("description"));
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("websiteUrl"));
}

TEST(MCP, make_initialize_result_includes_full_implementation_on_2025_11_25) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-11-25" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", "Server",
                                                   "desc", "https://x"};
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
    MCP,
    make_initialize_result_strips_unsupported_implementation_fields_on_2025_03_26) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "2025-03-26" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", "Server",
                                                   "desc", "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("title"));
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("description"));
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("websiteUrl"));
}

TEST(MCP, make_initialize_result_falls_back_to_2025_11_25_on_unknown_version) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": { "protocolVersion": "9999-01-01" }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", {}, {}, {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
}

TEST(MCP, make_initialize_result_returns_invalid_request_when_missing_params) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "initialize"
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", {}, {}, {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST);
}

TEST(MCP, make_initialize_result_returns_invalid_request_when_id_missing) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "method": "initialize", "params": {}
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{"srv", "1.0.0", {}, {}, {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server)};
  EXPECT_EQ(envelope.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST);
}

TEST(MCP, tool_call_arguments_present) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": { "x": 1 } }
  })JSON")};
  const auto *arguments{sourcemeta::core::mcp_tool_call_arguments(envelope)};
  ASSERT_NE(arguments, nullptr);
  EXPECT_TRUE(arguments->is_object());
  EXPECT_EQ(arguments->at("x").to_integer(), 1);
}

TEST(MCP, tool_call_arguments_missing) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(MCP, tool_call_arguments_params_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": [ 1, 2 ]
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(MCP, tool_call_arguments_no_params) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call"
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(MCP, tool_call_arguments_string_value) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": "not-an-object" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(MCP, tool_call_arguments_array_value) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": [ 1, 2, 3 ] }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}

TEST(MCP, tool_call_arguments_null_value) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": null }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_tool_call_arguments(envelope), nullptr);
}
