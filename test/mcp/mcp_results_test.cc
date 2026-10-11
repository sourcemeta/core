#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <sourcemeta/core/test.h>

#include "mcp_helpers.h"

#include <array>    // std::array
#include <cstddef>  // std::size_t
#include <cstdint>  // std::int64_t
#include <limits>   // std::numeric_limits
#include <optional> // std::optional, std::nullopt
#include <sstream>  // std::ostringstream
#include <utility>  // std::move
#include <vector>   // std::vector

TEST(make_text_block) {
  const auto block{sourcemeta::core::mcp_make_text_block("hello")};
  const auto expected{sourcemeta::core::parse_json(
      R"JSON({ "type": "text", "text": "hello" })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_text_block_empty_string) {
  const auto block{sourcemeta::core::mcp_make_text_block("")};
  const auto expected{sourcemeta::core::parse_json(
      R"JSON({ "type": "text", "text": "" })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_text_block_with_newlines) {
  const auto block{sourcemeta::core::mcp_make_text_block("line1\nline2")};
  const auto expected{sourcemeta::core::parse_json(
      R"JSON({ "type": "text", "text": "line1\nline2" })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_11_25_full) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "file:///foo",
      "My File", "text/plain", "A description")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "file:///foo",
    "name": "My File",
    "description": "A description",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_11_25_without_description) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "file:///foo",
      "My File", "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "file:///foo",
    "name": "My File",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(block, expected);
}

TEST(make_resource_link_2025_06_18_supports_structured) {
  const auto block{sourcemeta::core::mcp_make_resource_link(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "file:///foo",
      "My File", "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "file:///foo",
    "name": "My File",
    "mimeType": "text/plain"
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
      "content": [ { "type": "text", "text": "{\n  \"foo\": 42\n}" } ],
      "structuredContent": { "foo": 42 },
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_success_with_array_result) {
  const auto identifier{sourcemeta::core::JSON{"abc"}};
  const auto envelope{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      sourcemeta::core::parse_json(R"JSON([ 1, 2, 3 ])JSON"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "abc",
    "result": {
      "resultType": "complete",
      "content": [ { "type": "text", "text": "[ 1, 2, 3 ]" } ],
      "structuredContent": [ 1, 2, 3 ],
      "isError": false
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(tool_success_with_null_id) {
  EXPECT_MCP_REJECTS((void)sourcemeta::core::mcp_make_tool_success(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::JSON{nullptr},
                         sourcemeta::core::JSON::make_object()),
                     "MCP identifiers must be strings or integers");
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
      "content": [ { "type": "text", "text": "{\n  \"foo\": 42\n}" } ],
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
  EXPECT_MCP_REJECTS((void)sourcemeta::core::mcp_make_tool_error(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::JSON{nullptr}, "Failure"),
                     "MCP identifiers must be strings or integers");
}

TEST(error_resource_not_found_with_integer_id) {
  const auto identifier{sourcemeta::core::JSON{3}};
  const auto envelope{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 3,
    "error": { "code": -32002, "message": "Resource not found" }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(error_resource_not_found_with_string_id) {
  const auto identifier{sourcemeta::core::JSON{"req-7"}};
  const auto envelope{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "req-7",
    "error": { "code": -32002, "message": "Resource not found" }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_resource_full) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", "First file", std::optional<std::size_t>{1024})};
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
  const auto resource{sourcemeta::core::mcp_make_resource(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain")};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain"
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_without_size) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", "First file")};
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
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", "First file", std::optional<std::size_t>{1024},
      std::optional<double>{0.75})};
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
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", {}, std::nullopt, std::optional<double>{0.25})};
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
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", {}, std::nullopt, std::optional<double>{2.5})};
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
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", {}, std::nullopt, std::optional<double>{-5.0})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "Alpha",
    "mimeType": "text/plain",
    "annotations": { "priority": 0.0 }
  })JSON")};
  EXPECT_EQ(resource, expected);
}

TEST(make_resource_rejects_priority_positive_infinity) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_resource(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a",
          "Alpha", "text/plain", {}, std::nullopt,
          std::optional<double>{std::numeric_limits<double>::infinity()}),
      "Annotation priority must be a finite number");
}

TEST(make_resource_rejects_priority_negative_infinity) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_resource(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a",
          "Alpha", "text/plain", {}, std::nullopt,
          std::optional<double>{-std::numeric_limits<double>::infinity()}),
      "Annotation priority must be a finite number");
}

TEST(make_resource_rejects_priority_nan) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_resource(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a",
          "Alpha", "text/plain", {}, std::nullopt,
          std::optional<double>{std::numeric_limits<double>::quiet_NaN()}),
      "Annotation priority must be a finite number");
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
  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 30000, .scope = sourcemeta::core::MCPCacheScope::Public};
  const auto result{sourcemeta::core::mcp_make_resources_read_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::move(contents),
      policy)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "contents": [
      {
        "uri": "file:///a",
        "mimeType": "text/plain",
        "text": "Hello"
      }
    ]
  })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(make_resources_read_result_empty) {
  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 30000, .scope = sourcemeta::core::MCPCacheScope::Public};
  const auto result{sourcemeta::core::mcp_make_resources_read_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::JSON::make_array(), policy)};
  const auto expected{
      sourcemeta::core::parse_json(R"JSON({ "contents": [] })JSON")};
  EXPECT_EQ(result, expected);
}

TEST(make_resource_template) {
  const auto entry{sourcemeta::core::mcp_make_resource_template(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///{path}",
      "Files", "Resolves a file path", "text/plain")};
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
      sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"))};
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
      sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
      sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "name": "say",
    "description": "Says hello",
    "inputSchema": { "type": "object" },
    "outputSchema": { "type": "object" },
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
      sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
      sourcemeta::core::parse_json(R"JSON({ "type": "string" })JSON"))};
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
      sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
      std::nullopt, annotations)};
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
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED)};
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
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
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
      request, capabilities, server, SUPPORTED)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_initialize_result_non_string_protocol_version_is_invalid_params) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
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
      request, capabilities, server, SUPPORTED)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(make_initialize_result_unsupported_protocol_version_negotiates_latest) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "9999-01-01",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED)};
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
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
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
      request, capabilities, server, SUPPORTED)};
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
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED, "Be careful.")};
  EXPECT_EQ(envelope.at("result").at("instructions").to_string(),
            "Be careful.");
}

// A description and a website URL arrived in 2025-11-25, so an earlier
// handshake carries only the title
// https://modelcontextprotocol.io/specification/2026-07-28/schema#implementation
TEST(make_initialize_result_includes_only_the_title_on_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url =
                                                       "https://example.com"};
  EXPECT_EQ(sourcemeta::core::mcp_make_initialize_result(
                sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-06-18",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON"),
                {}, server, SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "protocolVersion": "2025-06-18",
      "capabilities": {},
      "serverInfo": { "name": "srv", "version": "1.0.0", "title": "Server" }
    }
  })JSON"));
}

TEST(make_initialize_result_includes_full_implementation_on_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url = "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED)};
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
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-03-26",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url = "https://x"};
  EXPECT_EQ(sourcemeta::core::mcp_make_initialize_result(request, capabilities,
                                                         server, SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "protocolVersion": "2025-03-26",
      "capabilities": {},
      "serverInfo": { "name": "srv", "version": "1.0.0" }
    }
  })JSON"));
}

TEST(make_initialize_result_falls_back_to_2025_11_25_on_unknown_version) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "9999-01-01",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED)};
  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
}

TEST(make_initialize_result_returns_invalid_request_when_missing_params) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 1, "method": "initialize" })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED)};
  EXPECT_EQ(envelope.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST);
}

TEST(make_initialize_result_returns_invalid_request_when_id_missing) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(
      R"JSON({
        "jsonrpc": "2.0",
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
      request, capabilities, server, SUPPORTED)};
  EXPECT_EQ(envelope.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST);
}

TEST(tool_call_arguments_of_a_tool_call) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "foo", "arguments": { "x": 1 } }
  })JSON")};
  EXPECT_EQ(*sourcemeta::core::mcp_tool_call_arguments(envelope),
            sourcemeta::core::parse_json(R"JSON({ "x": 1 })JSON"));
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
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 1, "method": "tools/call" })JSON")};
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

TEST(body_extraction_name) {
  const auto tool_call{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "my-tool", "arguments": {} }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(tool_call), "my-tool");

  const auto res_read{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "resources/read",
    "params": { "uri": "file:///path/to/file" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(res_read),
            "file:///path/to/file");

  const auto prompt_get{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "prompts/get",
    "params": { "name": "code-review" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(prompt_get),
            "code-review");

  const auto neither{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
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
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          "1900-01-01", supported)};

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

  EXPECT_EQ(sourcemeta::core::mcp_make_error_missing_required_capability(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                std::move(required), "Missing required client capability"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 11,
    "error": {
      "code": -32021,
      "message": "Missing required client capability",
      "data": { "requiredCapabilities": { "elicitation": {} } }
    }
  })JSON"));
}

TEST(error_header_mismatch) {
  const auto identifier{sourcemeta::core::JSON{12}};
  const auto envelope{sourcemeta::core::mcp_make_error_header_mismatch(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
      "Mcp-Method header does not match body method")};

  EXPECT_EQ(envelope.at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(envelope.at("error").at("message").to_string(),
            "Mcp-Method header does not match body method");
}

TEST(error_header_mismatch_unexpected) {
  const auto identifier{sourcemeta::core::JSON{14}};
  const auto envelope{sourcemeta::core::mcp_make_error_header_mismatch(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
      "Mcp-Name", "unexpected-name")};

  EXPECT_EQ(envelope.at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(envelope.at("error").at("message").to_string(),
            "Header mismatch: unexpected header provided");
  EXPECT_EQ(envelope.at("error").at("data").at("header").to_string(),
            "Mcp-Name");
  EXPECT_EQ(envelope.at("error").at("data").at("headerValue").to_string(),
            "unexpected-name");
}

TEST(error_header_mismatch_detailed) {
  const auto identifier{sourcemeta::core::JSON{15}};
  const auto envelope{sourcemeta::core::mcp_make_error_header_mismatch(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
      "Mcp-Method", "tools/call", "resources/read")};

  EXPECT_EQ(envelope.at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(envelope.at("error").at("message").to_string(), "Header mismatch");
  EXPECT_EQ(envelope.at("error").at("data").at("header").to_string(),
            "Mcp-Method");
  EXPECT_EQ(envelope.at("error").at("data").at("headerValue").to_string(),
            "tools/call");
  EXPECT_EQ(envelope.at("error").at("data").at("bodyValue").to_string(),
            "resources/read");
}

TEST(make_error_header_mismatch_omits_a_null_identifier) {
  const sourcemeta::core::JSON identifier{nullptr};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_header_mismatch(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                "Mcp-Method", "tools/call", "resources/read"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "Mcp-Method",
        "headerValue": "tools/call",
        "bodyValue": "resources/read"
      }
    }
  })JSON"));
}

TEST(make_error_header_mismatch_omits_an_absent_identifier) {
  EXPECT_EQ(sourcemeta::core::mcp_make_error_header_mismatch(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, nullptr,
                "Mcp-Method", "tools/call", "resources/read"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "Mcp-Method",
        "headerValue": "tools/call",
        "bodyValue": "resources/read"
      }
    }
  })JSON"));
}

TEST(error_resource_not_found_version_aware) {
  const auto identifier{sourcemeta::core::JSON{13}};

  const auto modern_err{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier)};
  EXPECT_EQ(modern_err.at("error").at("code").to_integer(), -32602);
  EXPECT_EQ(modern_err.at("error").at("message").to_string(),
            "Resource not found");

  const auto legacy_err{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier)};
  EXPECT_EQ(legacy_err.at("error").at("code").to_integer(), -32002);
  EXPECT_EQ(legacy_err.at("error").at("message").to_string(),
            "Resource not found");
}

TEST(decorate_result_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_decorate_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::parse_json(R"JSON({ "status": "ok" })JSON")),
            sourcemeta::core::parse_json(R"JSON({ "status": "ok" })JSON"));
}

TEST(decorate_result_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_decorate_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON({ "status": "ok" })JSON")),
            sourcemeta::core::parse_json(
                R"JSON({ "status": "ok", "resultType": "complete" })JSON"));
}

TEST(decorate_result_with_full_server_information) {
  const sourcemeta::core::MCPImplementation server{.name = "ExampleServer",
                                                   .version = "1.0.0",
                                                   .title = "Example Title",
                                                   .description = "Description",
                                                   .website_url =
                                                       "https://example.com"};
  EXPECT_EQ(sourcemeta::core::mcp_decorate_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON({ "status": "ok" })JSON"),
                server),
            sourcemeta::core::parse_json(R"JSON({
    "status": "ok",
    "resultType": "complete",
    "_meta": {
      "io.modelcontextprotocol/serverInfo": {
        "name": "ExampleServer",
        "version": "1.0.0",
        "title": "Example Title",
        "description": "Description",
        "websiteUrl": "https://example.com"
      }
    }
  })JSON"));
}

TEST(decorate_result_ignores_server_information_on_2025_11_25) {
  const sourcemeta::core::MCPImplementation server{.name = "ExampleServer",
                                                   .version = "1.0.0"};
  EXPECT_EQ(sourcemeta::core::mcp_decorate_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::parse_json(R"JSON({ "status": "ok" })JSON"),
                server),
            sourcemeta::core::parse_json(R"JSON({ "status": "ok" })JSON"));
}

TEST(server_discover_result) {
  const auto identifier{sourcemeta::core::JSON{1}};
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.tools = true;
  capabilities.tools_list_changed = true;
  capabilities.resources = true;
  capabilities.resources_subscribe = true;

  const sourcemeta::core::MCPImplementation server{.name = "TestServer",
                                                   .version = "2.0.0"};
  const sourcemeta::core::MCPCachePolicy cache{
      .ttl_ms = 3600000, .scope = sourcemeta::core::MCPCacheScope::Public};

  const std::vector<sourcemeta::core::JSON::StringView> discovery_versions{
      "2026-07-28", "2025-11-25", "2025-06-18", "2025-03-26"};
  const auto envelope{sourcemeta::core::mcp_make_server_discover_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      capabilities, server, discovery_versions, "Instructions here", cache)};

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

// The handshake only exists before 2026-07-28, so a client asking for the
// stateless revision is answered with the newest revision that still has one
// https://modelcontextprotocol.io/specification/2026-07-28/basic/versioning#protocol-version-negotiation
TEST(make_initialize_result_negotiates_down_from_2026_07_28) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  EXPECT_EQ(sourcemeta::core::mcp_make_initialize_result(
                sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2026-07-28",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON"),
                {}, server, SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "serverInfo": { "name": "srv", "version": "1.0.0" }
    }
  })JSON"));
}

TEST(initialize_negotiation_with_unknown_version) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "3000-01-01",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON")};

  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED)};

  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
}

TEST(mrtr_input_required_result) {
  const auto identifier{sourcemeta::core::JSON{50}};
  auto input_requests{sourcemeta::core::JSON::make_object()};
  auto input_request{sourcemeta::core::JSON::make_object()};
  input_request.assign("method", sourcemeta::core::JSON{"elicitation/create"});
  input_request.assign("params", sourcemeta::core::parse_json(
                                     R"JSON({
            "message": "Confirm delete?",
            "requestedSchema": { "type": "object", "properties": {} }
          })JSON"));
  input_requests.assign("confirm", std::move(input_request));

  const auto envelope{sourcemeta::core::mcp_make_input_required_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call",
      identifier, std::move(input_requests), "opaque-state-token-123",
      sourcemeta::core::MCPClientCapabilities{.elicitation = true,
                                              .elicitation_form = true})};

  const auto &result{envelope.at("result")};
  EXPECT_EQ(result.at("resultType").to_string(), "input_required");
  EXPECT_EQ(result.at("inputRequests")
                .at("confirm")
                .at("params")
                .at("message")
                .to_string(),
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
      "inputResponses": { "confirm": { "value": true } }
    }
  })JSON")};

  EXPECT_EQ(
      sourcemeta::core::mcp_request_state(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, continuation),
      "token-xyz");
  EXPECT_EQ(
      *sourcemeta::core::mcp_request_input_responses(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, continuation),
      sourcemeta::core::parse_json(
          R"JSON({ "confirm": { "value": true } })JSON"));
}

// A list result carries its page cursor and, in 2026-07-28, the caching hints
// the operation requires
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching#cacheable-results
TEST(tools_list_result_with_a_cursor_2026_07_28) {
  auto entries{sourcemeta::core::JSON::make_array()};
  entries.push_back(sourcemeta::core::mcp_make_tool_descriptor(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "calc", "Calculator",
      sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON")));
  EXPECT_EQ(
      sourcemeta::core::mcp_make_tools_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{entries}, "cursor-abc",
          {.ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "tools": [
      {
        "name": "calc",
        "description": "Calculator",
        "inputSchema": { "type": "object" },
        "annotations": {
          "readOnlyHint": false,
          "destructiveHint": true,
          "idempotentHint": false,
          "openWorldHint": true
        }
      }
    ],
    "nextCursor": "cursor-abc",
    "ttlMs": 120000,
    "cacheScope": "public"
  })JSON"));
}

TEST(tools_list_result_with_a_cursor_2025_11_25) {
  auto entries{sourcemeta::core::JSON::make_array()};
  entries.push_back(sourcemeta::core::mcp_make_tool_descriptor(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "calc", "Calculator",
      sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON")));
  EXPECT_EQ(
      sourcemeta::core::mcp_make_tools_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON{entries}, "cursor-abc",
          {.ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "tools": [
      {
        "name": "calc",
        "description": "Calculator",
        "inputSchema": { "type": "object" },
        "annotations": {
          "readOnlyHint": false,
          "destructiveHint": true,
          "idempotentHint": false,
          "openWorldHint": true
        }
      }
    ],
    "nextCursor": "cursor-abc"
  })JSON"));
}

TEST(resources_list_result_with_a_cursor_2026_07_28) {
  auto entries{sourcemeta::core::JSON::make_array()};
  entries.push_back(sourcemeta::core::mcp_make_resource(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "file:///a", "a",
      "text/plain"));
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resources_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{entries}, "cursor-abc",
          {.ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resources": [
      {
        "uri": "file:///a",
        "name": "a",
        "mimeType": "text/plain"
      }
    ],
    "nextCursor": "cursor-abc",
    "ttlMs": 120000,
    "cacheScope": "public"
  })JSON"));
}

TEST(resources_list_result_with_a_cursor_2025_11_25) {
  auto entries{sourcemeta::core::JSON::make_array()};
  entries.push_back(sourcemeta::core::mcp_make_resource(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "file:///a", "a",
      "text/plain"));
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resources_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON{entries}, "cursor-abc",
          {.ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resources": [
      {
        "uri": "file:///a",
        "name": "a",
        "mimeType": "text/plain"
      }
    ],
    "nextCursor": "cursor-abc"
  })JSON"));
}

TEST(resource_templates_list_result_with_a_cursor_2026_07_28) {
  auto entries{sourcemeta::core::JSON::make_array()};
  entries.push_back(sourcemeta::core::mcp_make_resource_template(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "file:///{name}",
      "template", "description", "text/plain"));
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{entries}, "cursor-abc",
          {.ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resourceTemplates": [
      {
        "uriTemplate": "file:///{name}",
        "name": "template",
        "description": "description",
        "mimeType": "text/plain"
      }
    ],
    "nextCursor": "cursor-abc",
    "ttlMs": 120000,
    "cacheScope": "public"
  })JSON"));
}

TEST(resource_templates_list_result_with_a_cursor_2025_11_25) {
  auto entries{sourcemeta::core::JSON::make_array()};
  entries.push_back(sourcemeta::core::mcp_make_resource_template(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "file:///{name}",
      "template", "description", "text/plain"));
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON{entries}, "cursor-abc",
          {.ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resourceTemplates": [
      {
        "uriTemplate": "file:///{name}",
        "name": "template",
        "description": "description",
        "mimeType": "text/plain"
      }
    ],
    "nextCursor": "cursor-abc"
  })JSON"));
}

TEST(prompts_list_result_with_a_cursor_2026_07_28) {
  const auto entries{sourcemeta::core::parse_json(
      R"JSON([ { "name": "review", "description": "Review" } ])JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_prompts_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{entries}, "cursor-abc",
          {.ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "prompts": [ { "name": "review", "description": "Review" } ],
    "nextCursor": "cursor-abc",
    "ttlMs": 120000,
    "cacheScope": "public"
  })JSON"));
}

TEST(prompts_list_result_with_a_cursor_2025_11_25) {
  const auto entries{sourcemeta::core::parse_json(
      R"JSON([ { "name": "review", "description": "Review" } ])JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_prompts_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON{entries}, "cursor-abc",
          {.ttl_ms = 120000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "prompts": [ { "name": "review", "description": "Review" } ],
    "nextCursor": "cursor-abc"
  })JSON"));
}

// An empty cursor is a cursor, so a caller that asks for one gets it rather
// than having it treated as absent
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/pagination
TEST(tools_list_result_preserves_an_empty_cursor_2026_07_28) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_tools_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON::make_array(), "",
          {.ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "tools": [],
    "nextCursor": "",
    "ttlMs": 10000,
    "cacheScope": "public"
  })JSON"));
}

TEST(tools_list_result_preserves_an_empty_cursor_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_tools_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON::make_array(), "",
          {.ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(
          R"JSON({ "tools": [], "nextCursor": "" })JSON"));
}

TEST(resources_list_result_preserves_an_empty_cursor_2026_07_28) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resources_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON::make_array(), "",
          {.ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resources": [],
    "nextCursor": "",
    "ttlMs": 10000,
    "cacheScope": "public"
  })JSON"));
}

TEST(resources_list_result_preserves_an_empty_cursor_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resources_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON::make_array(), "",
          {.ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(
          R"JSON({ "resources": [], "nextCursor": "" })JSON"));
}

TEST(resource_templates_list_result_preserves_an_empty_cursor_2026_07_28) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON::make_array(), "",
          {.ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resourceTemplates": [],
    "nextCursor": "",
    "ttlMs": 10000,
    "cacheScope": "public"
  })JSON"));
}

TEST(resource_templates_list_result_preserves_an_empty_cursor_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resource_templates_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON::make_array(), "",
          {.ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(
          R"JSON({ "resourceTemplates": [], "nextCursor": "" })JSON"));
}

TEST(prompts_list_result_preserves_an_empty_cursor_2026_07_28) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_prompts_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON::make_array(), "",
          {.ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "prompts": [],
    "nextCursor": "",
    "ttlMs": 10000,
    "cacheScope": "public"
  })JSON"));
}

TEST(prompts_list_result_preserves_an_empty_cursor_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_prompts_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON::make_array(), "",
          {.ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public}),
      sourcemeta::core::parse_json(
          R"JSON({ "prompts": [], "nextCursor": "" })JSON"));
}

// Schema keywords are carried through verbatim, so a tool may use the whole of
// JSON Schema 2020-12
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#json-schema-usage
TEST(make_tool_descriptor_with_arbitrary_schema_keywords) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "advanced_tool", "Tool using modern JSON Schema",
                sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "$defs": { "item": { "type": "string" } },
    "properties": {
      "tags": {
        "type": "array",
        "prefixItems": [ { "$ref": "#/$defs/item" } ]
      }
    },
    "dependentRequired": { "tags": [ "name" ] },
    "unevaluatedProperties": false
  })JSON"),
                sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "properties": { "status": { "type": "string" } }
  })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "name": "advanced_tool",
    "description": "Tool using modern JSON Schema",
    "inputSchema": {
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "type": "object",
      "$defs": { "item": { "type": "string" } },
      "properties": {
        "tags": {
          "type": "array",
          "prefixItems": [ { "$ref": "#/$defs/item" } ]
        }
      },
      "dependentRequired": { "tags": [ "name" ] },
      "unevaluatedProperties": false
    },
    "outputSchema": {
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "type": "object",
      "properties": { "status": { "type": "string" } }
    },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON"));
}

TEST(make_initialize_result_negotiates_up_from_an_unknown_revision) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  EXPECT_EQ(sourcemeta::core::mcp_make_initialize_result(
                sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {
      "protocolVersion": "2024-11-05",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON"),
                {}, server, SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "serverInfo": { "name": "srv", "version": "1.0.0" }
    }
  })JSON"));
}

// A server identifies itself in the metadata of a result it returns
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(make_empty_result_with_server_information) {
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{100}, server),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 100,
    "result": {
      "resultType": "complete",
      "_meta": {
        "io.modelcontextprotocol/serverInfo": { "name": "srv", "version": "1.0.0" }
      }
    }
  })JSON"));
}

TEST(make_empty_result_ignores_server_information_on_2025_11_25) {
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON{100}, server),
            sourcemeta::core::parse_json(
                R"JSON({ "jsonrpc": "2.0", "id": 100, "result": {} })JSON"));
}

TEST(decorate_cacheable_result_2026_07_28) {
  EXPECT_EQ(
      sourcemeta::core::mcp_decorate_cacheable_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON::make_object(),
          {.ttl_ms = 5000, .scope = sourcemeta::core::MCPCacheScope::Private},
          sourcemeta::core::MCP_METHOD_TOOLS_LIST),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "ttlMs": 5000,
    "cacheScope": "private"
  })JSON"));
}

TEST(decorate_cacheable_result_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_decorate_cacheable_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON::make_object(),
          {.ttl_ms = 5000, .scope = sourcemeta::core::MCPCacheScope::Private},
          sourcemeta::core::MCP_METHOD_TOOLS_LIST),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(base64_header_malformed_rejected_on_2026) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
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
      "tools/call", "base64:%%%invalid-chars&&&", request, SUPPORTED)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error->at("error").at("code").to_integer(),
            sourcemeta::core::MCP_CODE_HEADER_MISMATCH);
}

TEST(request_name_from_body_prefers_uri_for_resources_read) {
  const auto envelope_with_both{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "resources/read",
    "params": {
      "name": "ignored-name",
      "uri": "file:///path/to/resource"
    }
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

TEST(decorate_result_rejects_non_object_meta) {
  auto result{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "complete", "_meta": "invalid" })JSON")};
  const auto before{result};
  EXPECT_MCP_REJECTS(
      result = sourcemeta::core::mcp_decorate_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, result),
      "MCP result _meta must be an object");
  EXPECT_EQ(result, before);
}

TEST(result_type_defaults_to_complete_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_EQ(sourcemeta::core::mcp_resolve_result_type(
                version, sourcemeta::core::JSON::make_object())
                .value(),
            "complete");
}

TEST(result_type_rejects_unnegotiated_extension_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  const auto custom{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "org.example/custom" })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_result_type(version, custom).has_value());
}

TEST(result_type_negotiated_extension_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  const std::array<sourcemeta::core::JSON::StringView, 1> extensions{
      {"org.example/custom"}};
  const auto custom{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "org.example/custom" })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_resolve_result_type(version, custom, extensions)
          .has_value(),
      false);
}

TEST(result_type_input_required_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  const auto interim{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "input_required" })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_resolve_result_type(version, interim).has_value(),
      false);
}

TEST(result_type_rejects_boolean_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_result_type(
                   version, sourcemeta::core::JSON{false})
                   .has_value());
}

TEST(result_type_defaults_to_complete_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_EQ(sourcemeta::core::mcp_resolve_result_type(
                version, sourcemeta::core::JSON::make_object())
                .value(),
            "complete");
}

TEST(result_type_rejects_unnegotiated_extension_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  const auto custom{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "org.example/custom" })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_result_type(version, custom).has_value());
}

TEST(result_type_negotiated_extension_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  const std::array<sourcemeta::core::JSON::StringView, 1> extensions{
      {"org.example/custom"}};
  const auto custom{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "org.example/custom" })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_resolve_result_type(version, custom, extensions)
          .has_value(),
      false);
}

TEST(result_type_input_required_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  const auto interim{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "input_required" })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_resolve_result_type(version, interim).has_value(),
      false);
}

TEST(result_type_rejects_boolean_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_result_type(
                   version, sourcemeta::core::JSON{false})
                   .has_value());
}

TEST(result_type_defaults_to_complete_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_EQ(sourcemeta::core::mcp_resolve_result_type(
                version, sourcemeta::core::JSON::make_object())
                .value(),
            "complete");
}

TEST(result_type_rejects_unnegotiated_extension_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  const auto custom{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "org.example/custom" })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_result_type(version, custom).has_value());
}

TEST(result_type_negotiated_extension_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  const std::array<sourcemeta::core::JSON::StringView, 1> extensions{
      {"org.example/custom"}};
  const auto custom{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "org.example/custom" })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_resolve_result_type(version, custom, extensions)
          .has_value(),
      false);
}

TEST(result_type_input_required_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  const auto interim{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "input_required" })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_resolve_result_type(version, interim).has_value(),
      false);
}

TEST(result_type_rejects_boolean_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_result_type(
                   version, sourcemeta::core::JSON{false})
                   .has_value());
}

TEST(result_type_defaults_to_complete_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_EQ(sourcemeta::core::mcp_resolve_result_type(
                version, sourcemeta::core::JSON::make_object())
                .value(),
            "complete");
}

TEST(result_type_rejects_unnegotiated_extension_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  const auto custom{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "org.example/custom" })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_resolve_result_type(version, custom).has_value());
}

TEST(result_type_negotiated_extension_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  const std::array<sourcemeta::core::JSON::StringView, 1> extensions{
      {"org.example/custom"}};
  const auto custom{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "org.example/custom" })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_resolve_result_type(version, custom, extensions)
          .has_value(),
      true);
}

TEST(result_type_input_required_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  const auto interim{sourcemeta::core::parse_json(
      R"JSON({ "resultType": "input_required" })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_resolve_result_type(version, interim).has_value(),
      true);
}

TEST(result_type_rejects_boolean_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_result_type(
                   version, sourcemeta::core::JSON{false})
                   .has_value());
}

TEST(empty_result_rejects_null_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{nullptr}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_null_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{nullptr}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_boolean_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{true}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_boolean_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{true}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_real_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{1.5}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_real_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{1.5}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_array_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON::make_array()),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_array_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_error(
          version, sourcemeta::core::JSON::make_array(), "error"),
      "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_object_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON::make_object()),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_object_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_error(
          version, sourcemeta::core::JSON::make_object(), "error"),
      "MCP identifiers must be strings or integers");
}

TEST(empty_result_accepts_zero_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(version,
                                                    sourcemeta::core::JSON{0})
                .at("id"),
            sourcemeta::core::JSON{0});
}

TEST(empty_result_accepts_empty_string_id_2025_03_26) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(version,
                                                    sourcemeta::core::JSON{""})
                .at("id"),
            sourcemeta::core::JSON{""});
}

TEST(empty_result_rejects_null_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{nullptr}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_null_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{nullptr}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_boolean_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{true}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_boolean_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{true}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_real_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{1.5}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_real_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{1.5}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_array_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON::make_array()),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_array_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_error(
          version, sourcemeta::core::JSON::make_array(), "error"),
      "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_object_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON::make_object()),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_object_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_error(
          version, sourcemeta::core::JSON::make_object(), "error"),
      "MCP identifiers must be strings or integers");
}

TEST(empty_result_accepts_zero_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(version,
                                                    sourcemeta::core::JSON{0})
                .at("id"),
            sourcemeta::core::JSON{0});
}

TEST(empty_result_accepts_empty_string_id_2025_06_18) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(version,
                                                    sourcemeta::core::JSON{""})
                .at("id"),
            sourcemeta::core::JSON{""});
}

TEST(empty_result_rejects_null_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{nullptr}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_null_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{nullptr}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_boolean_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{true}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_boolean_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{true}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_real_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{1.5}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_real_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{1.5}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_array_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON::make_array()),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_array_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_error(
          version, sourcemeta::core::JSON::make_array(), "error"),
      "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_object_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON::make_object()),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_object_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_error(
          version, sourcemeta::core::JSON::make_object(), "error"),
      "MCP identifiers must be strings or integers");
}

TEST(empty_result_accepts_zero_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(version,
                                                    sourcemeta::core::JSON{0})
                .at("id"),
            sourcemeta::core::JSON{0});
}

TEST(empty_result_accepts_empty_string_id_2025_11_25) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(version,
                                                    sourcemeta::core::JSON{""})
                .at("id"),
            sourcemeta::core::JSON{""});
}

TEST(empty_result_rejects_null_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{nullptr}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_null_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{nullptr}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_boolean_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{true}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_boolean_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{true}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_real_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON{1.5}),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_real_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_error(
                         version, sourcemeta::core::JSON{1.5}, "error"),
                     "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_array_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON::make_array()),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_array_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_error(
          version, sourcemeta::core::JSON::make_array(), "error"),
      "MCP identifiers must be strings or integers");
}

TEST(empty_result_rejects_object_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_empty_result(
                         version, sourcemeta::core::JSON::make_object()),
                     "MCP identifiers must be strings or integers");
}

TEST(tool_error_rejects_object_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_error(
          version, sourcemeta::core::JSON::make_object(), "error"),
      "MCP identifiers must be strings or integers");
}

TEST(empty_result_accepts_zero_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(version,
                                                    sourcemeta::core::JSON{0})
                .at("id"),
            sourcemeta::core::JSON{0});
}

TEST(empty_result_accepts_empty_string_id_2026_07_28) {
  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(version,
                                                    sourcemeta::core::JSON{""})
                .at("id"),
            sourcemeta::core::JSON{""});
}

TEST(initialize_supported_subset) {
  const std::array<sourcemeta::core::JSON::StringView, 1> only{{"2025-03-26"}};
  const auto init{sourcemeta::core::parse_json(
      R"JSON({
        "jsonrpc": "2.0",
        "id": 0,
        "method": "initialize",
        "params": {
          "protocolVersion": "unknown",
          "capabilities": {},
          "clientInfo": { "name": "client", "version": "1" }
        }
      })JSON")};
  const auto result{sourcemeta::core::mcp_make_initialize_result(
      init, {}, {.name = "server", .version = "1"}, only)};
  EXPECT_EQ(result.at("result").at("protocolVersion"),
            sourcemeta::core::JSON{"2025-03-26"});
}

TEST(initialize_missing_client_info) {
  const std::array<sourcemeta::core::JSON::StringView, 1> only{{"2025-03-26"}};
  const auto init{sourcemeta::core::parse_json(
      R"JSON({
        "jsonrpc": "2.0",
        "id": 0,
        "method": "initialize",
        "params": {
          "protocolVersion": "unknown",
          "capabilities": {},
          "clientInfo": { "name": "client", "version": "1" }
        }
      })JSON")};
  auto invalid{init};
  invalid.at("params").erase("clientInfo");
  EXPECT_EQ(sourcemeta::core::mcp_make_initialize_result(invalid, {},
                                                         {"server", "1"}, only)
                .at("error")
                .at("code"),
            sourcemeta::core::JSON{-32602});
}

TEST(initialize_invalid_identifier) {
  const std::array<sourcemeta::core::JSON::StringView, 1> only{{"2025-03-26"}};
  const auto init{sourcemeta::core::parse_json(
      R"JSON({
        "jsonrpc": "2.0",
        "id": 0,
        "method": "initialize",
        "params": {
          "protocolVersion": "unknown",
          "capabilities": {},
          "clientInfo": { "name": "client", "version": "1" }
        }
      })JSON")};

  auto invalid{init};
  invalid.at("params").erase("clientInfo");
  invalid.assign("id", sourcemeta::core::JSON{});
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_initialize_result(
                         invalid, {}, {.name = "server", .version = "1"}, only),
                     "This MCP revision requires a readable error identifier");
}

TEST(initialize_rejects_modern_only_support) {
  const auto init{sourcemeta::core::parse_json(
      R"JSON({
        "jsonrpc": "2.0",
        "id": 0,
        "method": "initialize",
        "params": {
          "protocolVersion": "unknown",
          "capabilities": {},
          "clientInfo": { "name": "client", "version": "1" }
        }
      })JSON")};
  const std::array<sourcemeta::core::JSON::StringView, 1> modern_only{
      {"2026-07-28"}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_initialize_result(
          init, {}, {.name = "server", .version = "1"}, modern_only),
      "No initialization-compatible version is supported");
}

// An absent discriminator means the result is complete, which keeps results
// from earlier revisions readable
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#result-responses
TEST(result_type_of_a_result_without_a_discriminator) {
  EXPECT_EQ(
      sourcemeta::core::mcp_result_type(sourcemeta::core::JSON::make_object()),
      "complete");
}

TEST(result_type_of_an_extension_discriminator) {
  EXPECT_EQ(sourcemeta::core::mcp_result_type(sourcemeta::core::parse_json(
                R"JSON({ "resultType": "example/extension" })JSON")),
            "example/extension");
}

TEST(result_type_of_a_non_object) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_result_type(sourcemeta::core::JSON{nullptr})
          .has_value());
}

TEST(result_type_of_a_non_string_discriminator) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_result_type(
          sourcemeta::core::parse_json(R"JSON({ "resultType": false })JSON"))
          .has_value());
}

TEST(decorate_result_rejects_an_interim_result) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_decorate_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(
              R"JSON({
                "resultType": "input_required",
                "requestState": "s"
              })JSON")),
      "The complete-result decorator cannot construct interim or extension "
      "results");
}

TEST(decorate_result_rejects_an_extension_result) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_decorate_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(
              R"JSON({ "resultType": "unknown" })JSON")),
      "The complete-result decorator cannot construct interim or extension "
      "results");
}

TEST(decorate_cacheable_result_rejects_an_interim_result) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_decorate_cacheable_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(
              R"JSON({
                "resultType": "input_required",
                "requestState": "s"
              })JSON"),
          {}, "resources/read"),
      "The complete-result decorator cannot construct interim or extension "
      "results");
}

TEST(decorate_cacheable_result_rejects_an_extension_result) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_decorate_cacheable_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(
              R"JSON({ "resultType": "unknown" })JSON"),
          {}, "resources/read"),
      "The complete-result decorator cannot construct interim or extension "
      "results");
}

// Caching hints belong to the six cacheable operations and nowhere else
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching#cacheable-results
TEST(decorate_cacheable_result_rejects_a_non_cacheable_operation) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_decorate_cacheable_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::JSON::make_object(), {},
                         "tools/call"),
                     "Operation is not cacheable");
}

// A time to live is a count of milliseconds, so a negative hint becomes an
// immediately stale one
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching#time-to-live-ttl-field
TEST(decorate_cacheable_result_clamps_a_negative_time_to_live) {
  EXPECT_EQ(
      sourcemeta::core::mcp_decorate_cacheable_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON::make_object(),
          {.ttl_ms = -4, .scope = sourcemeta::core::MCPCacheScope::Private},
          "resources/read"),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(decorate_result_rejects_incomplete_server_information) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_decorate_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/serverInfo": { "name": "server" }
    }
  })JSON")),
                     "Invalid server information");
}

// A tool's input schema declares an object at its root
// https://modelcontextprotocol.io/specification/2026-07-28/schema#tool
TEST(tool_descriptor_rejects_a_non_object_schema_2025_03_26) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tool", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "string" })JSON")),
      "Invalid MCP tool descriptor");
}

TEST(tool_descriptor_rejects_a_non_object_schema_2025_06_18) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tool", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "string" })JSON")),
      "Invalid MCP tool descriptor");
}

TEST(tool_descriptor_rejects_a_non_object_schema_2025_11_25) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tool", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "string" })JSON")),
      "Invalid MCP tool descriptor");
}

TEST(tool_descriptor_rejects_a_non_object_schema_2026_07_28) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tool", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "string" })JSON")),
      "Invalid MCP tool descriptor");
}

TEST(resources_list_result_rejects_a_nameless_resource_2025_03_26) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_resources_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
          sourcemeta::core::parse_json(R"JSON([ { "uri": "x" } ])JSON"),
          std::nullopt, {}),
      "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_a_nameless_resource_2025_06_18) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_resources_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
          sourcemeta::core::parse_json(R"JSON([ { "uri": "x" } ])JSON"),
          std::nullopt, {}),
      "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_a_nameless_resource_2025_11_25) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_resources_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::parse_json(R"JSON([ { "uri": "x" } ])JSON"),
          std::nullopt, {}),
      "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_a_nameless_resource_2026_07_28) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_resources_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(R"JSON([ { "uri": "x" } ])JSON"),
          std::nullopt, {}),
      "Invalid MCP resource descriptor entry");
}

TEST(prompts_list_result_rejects_a_non_string_argument_name_2025_03_26) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                         sourcemeta::core::parse_json(
                             R"JSON([
                {
                  "name": "prompt",
                  "arguments": [ { "name": 3 } ]
                }
              ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(prompts_list_result_rejects_a_non_string_argument_name_2025_06_18) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                         sourcemeta::core::parse_json(
                             R"JSON([
                {
                  "name": "prompt",
                  "arguments": [ { "name": 3 } ]
                }
              ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(prompts_list_result_rejects_a_non_string_argument_name_2025_11_25) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::parse_json(
                             R"JSON([
                {
                  "name": "prompt",
                  "arguments": [ { "name": 3 } ]
                }
              ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(prompts_list_result_rejects_a_non_string_argument_name_2026_07_28) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                {
                  "name": "prompt",
                  "arguments": [ { "name": 3 } ]
                }
              ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(resources_read_result_rejects_a_non_string_blob_2025_03_26) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_read_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                         sourcemeta::core::parse_json(
                             R"JSON([ { "uri": "x", "blob": false } ])JSON"),
                         {}),
                     "Invalid MCP resource contents entry");
}

TEST(resources_read_result_rejects_a_non_string_blob_2025_06_18) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_read_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                         sourcemeta::core::parse_json(
                             R"JSON([ { "uri": "x", "blob": false } ])JSON"),
                         {}),
                     "Invalid MCP resource contents entry");
}

TEST(resources_read_result_rejects_a_non_string_blob_2025_11_25) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_read_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::parse_json(
                             R"JSON([ { "uri": "x", "blob": false } ])JSON"),
                         {}),
                     "Invalid MCP resource contents entry");
}

TEST(resources_read_result_rejects_a_non_string_blob_2026_07_28) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_read_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([ { "uri": "x", "blob": false } ])JSON"),
                         {}),
                     "Invalid MCP resource contents entry");
}

TEST(tool_success_rejects_a_non_string_text_block_2025_03_26) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_success(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
          sourcemeta::core::JSON{0}, sourcemeta::core::JSON::make_object(),
          sourcemeta::core::parse_json(
              R"JSON([ { "type": "text", "text": false } ])JSON")),
      "Invalid MCP content block");
}

TEST(tool_success_rejects_a_non_string_text_block_2025_06_18) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_success(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
          sourcemeta::core::JSON{0}, sourcemeta::core::JSON::make_object(),
          sourcemeta::core::parse_json(
              R"JSON([ { "type": "text", "text": false } ])JSON")),
      "Invalid MCP content block");
}

TEST(tool_success_rejects_a_non_string_text_block_2025_11_25) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_success(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON{0}, sourcemeta::core::JSON::make_object(),
          sourcemeta::core::parse_json(
              R"JSON([ { "type": "text", "text": false } ])JSON")),
      "Invalid MCP content block");
}

TEST(tool_success_rejects_a_non_string_text_block_2026_07_28) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_success(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{0}, sourcemeta::core::JSON::make_object(),
          sourcemeta::core::parse_json(
              R"JSON([ { "type": "text", "text": false } ])JSON")),
      "Invalid MCP content block");
}

// A prompt message speaks as the user or as the assistant
// https://modelcontextprotocol.io/specification/2026-07-28/schema#promptmessage
TEST(prompts_get_result_rejects_an_unknown_role_2025_03_26) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_get_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "",
                         sourcemeta::core::parse_json(R"JSON([
    {
      "role": "system",
      "content": { "type": "text", "text": "hi" }
    }
  ])JSON")),
                     "Invalid prompt message");
}

TEST(prompts_get_result_rejects_an_unknown_role_2025_06_18) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_get_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "",
                         sourcemeta::core::parse_json(R"JSON([
    {
      "role": "system",
      "content": { "type": "text", "text": "hi" }
    }
  ])JSON")),
                     "Invalid prompt message");
}

TEST(prompts_get_result_rejects_an_unknown_role_2025_11_25) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_get_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "",
                         sourcemeta::core::parse_json(R"JSON([
    {
      "role": "system",
      "content": { "type": "text", "text": "hi" }
    }
  ])JSON")),
                     "Invalid prompt message");
}

TEST(prompts_get_result_rejects_an_unknown_role_2026_07_28) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_get_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "",
                         sourcemeta::core::parse_json(R"JSON([
    {
      "role": "system",
      "content": { "type": "text", "text": "hi" }
    }
  ])JSON")),
                     "Invalid prompt message");
}

// Structured content became an object only from 2025-06-18, and
// 2026-07-28 allows any JSON value again
// https://modelcontextprotocol.io/specification/2026-07-28/schema#calltoolresult
TEST(tool_success_rejects_an_array_structured_content_on_2025_06_18) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_success(
                         sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                         sourcemeta::core::JSON{0},
                         sourcemeta::core::JSON::make_array()),
                     "Legacy structuredContent must be an object");
}

TEST(tool_success_rejects_an_array_structured_content_on_2025_11_25) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_success(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::JSON{0},
                         sourcemeta::core::JSON::make_array()),
                     "Legacy structuredContent must be an object");
}

TEST(tool_descriptor_rejects_a_non_object_output_schema_on_2025_06_18) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tool", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
          sourcemeta::core::parse_json(R"JSON({ "type": "string" })JSON")),
      "Invalid MCP tool descriptor");
}

TEST(tool_descriptor_rejects_a_non_object_output_schema_on_2025_11_25) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tool", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
          sourcemeta::core::parse_json(R"JSON({ "type": "string" })JSON")),
      "Invalid MCP tool descriptor");
}

TEST(tool_success_accepts_null_structured_content_on_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON(null)JSON"))
                .at("result")
                .at("structuredContent"),
            sourcemeta::core::parse_json(R"JSON(null)JSON"));
}

TEST(tool_success_accepts_a_boolean_structured_content_on_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON(true)JSON"))
                .at("result")
                .at("structuredContent"),
            sourcemeta::core::parse_json(R"JSON(true)JSON"));
}

TEST(tool_success_accepts_an_integer_structured_content_on_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON(5)JSON"))
                .at("result")
                .at("structuredContent"),
            sourcemeta::core::parse_json(R"JSON(5)JSON"));
}

TEST(tool_success_accepts_a_string_structured_content_on_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON("text")JSON"))
                .at("result")
                .at("structuredContent"),
            sourcemeta::core::parse_json(R"JSON("text")JSON"));
}

TEST(tool_success_accepts_an_array_structured_content_on_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON([])JSON"))
                .at("result")
                .at("structuredContent"),
            sourcemeta::core::parse_json(R"JSON([])JSON"));
}

// A completion result carries at most 100 values
// https://modelcontextprotocol.io/specification/2026-07-28/schema#completeresult
TEST(completion_result_rejects_more_than_one_hundred_values) {
  auto values{sourcemeta::core::JSON::make_array()};
  for (std::size_t index = 0; index < 101; ++index) {
    values.push_back(sourcemeta::core::JSON{"value"});
  }
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_completion_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         values, std::nullopt, std::nullopt),
                     "At most 100 completion values are permitted");
}

TEST(completion_result_accepts_exactly_one_hundred_values) {
  auto values{sourcemeta::core::JSON::make_array()};
  for (std::size_t index = 0; index < 100; ++index) {
    values.push_back(sourcemeta::core::JSON{"value"});
  }
  auto completion{sourcemeta::core::JSON::make_object()};
  completion.assign("values", values);
  auto expected{sourcemeta::core::JSON::make_object()};
  expected.assign("resultType", sourcemeta::core::JSON{"complete"});
  expected.assign("completion", std::move(completion));
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, values,
                std::nullopt, std::nullopt),
            expected);
}

// A total may exceed the values actually sent, so it may never fall below them
// https://modelcontextprotocol.io/specification/2026-07-28/schema#completeresult
TEST(completion_result_rejects_a_total_below_the_value_count) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_completion_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([ "value" ])JSON"),
                         0, false),
                     "Invalid completion total");
}

TEST(completion_result_rejects_a_negative_total_2025_03_26) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_completion_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                         sourcemeta::core::JSON::make_array(), -1, false),
                     "Invalid completion total");
}

TEST(completion_result_rejects_a_negative_total_2025_06_18) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_completion_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                         sourcemeta::core::JSON::make_array(), -1, false),
                     "Invalid completion total");
}

TEST(completion_result_rejects_a_negative_total_2025_11_25) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_completion_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::JSON::make_array(), -1, false),
                     "Invalid completion total");
}

TEST(completion_result_rejects_a_negative_total_2026_07_28) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_completion_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::JSON::make_array(), -1, false),
                     "Invalid completion total");
}

TEST(completion_result_with_a_zero_total_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON::make_array(), 0, false),
            sourcemeta::core::parse_json(R"JSON({
    "completion": { "values": [], "total": 0, "hasMore": false }
  })JSON"));
}

TEST(completion_result_with_a_zero_total_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON::make_array(), 0, false),
            sourcemeta::core::parse_json(R"JSON({
    "completion": { "values": [], "total": 0, "hasMore": false }
  })JSON"));
}

TEST(completion_result_with_a_zero_total_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON::make_array(), 0, false),
            sourcemeta::core::parse_json(R"JSON({
    "completion": { "values": [], "total": 0, "hasMore": false }
  })JSON"));
}

TEST(completion_result_with_a_zero_total_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON::make_array(), 0, false),
            sourcemeta::core::parse_json(R"JSON({
    "completion": { "values": [], "total": 0, "hasMore": false },
    "resultType": "complete"
  })JSON"));
}

TEST(completion_result_with_the_maximum_total) {
  constexpr auto MAXIMUM{std::numeric_limits<std::int64_t>::max()};
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON([ "value" ])JSON"), MAXIMUM,
                false)
                .at("completion")
                .at("total"),
            sourcemeta::core::JSON{MAXIMUM});
}

// A server asks for roots, elicitation or sampling, and nothing else
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#inputrequiredresult
TEST(input_required_result_with_a_roots_request) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(
                    R"JSON({ "r": { "method": "roots/list" } })JSON"),
                std::nullopt, capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

TEST(input_required_result_with_a_url_elicitation) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "mode": "url",
        "message": "visit",
        "url": "https://example.com"
      }
    }
  })JSON"),
                std::nullopt, capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

// The user navigates to this address out of band, so a reference that only
// resolves against some base is not one they can be sent to
// https://modelcontextprotocol.io/specification/2026-07-28/client/elicitation#url-mode-elicitation-requests
TEST(input_required_result_rejects_a_relative_elicitation_url) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "mode": "url",
        "message": "visit",
        "url": "/ui/set_api_key"
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_a_malformed_elicitation_url) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "mode": "url",
        "message": "visit",
        "url": "http://[not a host]/x"
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// The specification restricts the scheme no further than requiring a URL, so
// a destination outside the web is still one a user can be sent to
TEST(input_required_result_accepts_a_non_web_elicitation_url) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "mode": "url",
        "message": "visit",
        "url": "ftp://files.example.com/form"
      }
    }
  })JSON"),
                std::nullopt, capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

TEST(input_required_result_with_a_form_elicitation) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": {
          "type": "object",
          "properties": {
            "choice": {
              "type": "array",
              "items": { "type": "string", "enum": [ "a" ] }
            }
          }
        }
      }
    }
  })JSON"),
                std::nullopt, capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

TEST(input_required_result_with_a_sampling_request) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 10,
        "messages": [
          {
            "role": "assistant",
            "content": {
              "type": "tool_use",
              "id": "call",
              "name": "tool",
              "input": {}
            }
          },
          {
            "role": "user",
            "content": {
              "type": "tool_result",
              "toolUseId": "call",
              "content": [ { "type": "text", "text": "ok" } ]
            }
          }
        ],
        "tools": [
          {
            "name": "tool",
            "inputSchema": { "type": "object" }
          }
        ],
        "toolChoice": { "mode": "auto" }
      }
    }
  })JSON"),
                std::nullopt, capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

TEST(input_required_result_rejects_non_object_roots_parameters) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(
                             R"JSON({
                "r": { "method": "roots/list", "params": false }
              })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// An elicitation form describes an object of fields
// https://modelcontextprotocol.io/specification/2026-07-28/client/elicitation
TEST(input_required_result_rejects_a_non_object_elicitation_schema) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": { "type": "array" }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// A form names the fields it asks for, so the property map is mandatory
// https://modelcontextprotocol.io/specification/2026-07-28/schema#elicitrequestformparams
TEST(input_required_result_rejects_an_elicitation_schema_without_properties) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": { "type": "object" }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_non_object_elicitation_properties) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": { "type": "object", "properties": [] }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// What a property contains is a schema, which this module does not interpret,
// so a nested object is accepted here and left for a schema validator
TEST(input_required_result_accepts_an_uninterpreted_property_schema) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  const auto envelope{sourcemeta::core::mcp_make_input_required_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/call",
      sourcemeta::core::JSON{0}, sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": {
          "type": "object",
          "properties": { "nested": { "type": "object" } }
        }
      }
    }
  })JSON"),
      std::nullopt, capabilities)};
  EXPECT_EQ(envelope.at("result").at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

// An optional member the caller leaves empty is absent rather than present and
// empty, consistently across the descriptor and contents builders
// https://modelcontextprotocol.io/specification/2026-07-28/schema#resource
TEST(make_resource_omits_an_empty_mime_type) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "file:///a",
                "a", ""),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "a"
  })JSON"));
}

TEST(make_resource_template_omits_an_empty_description_and_mime_type) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "file:///{path}", "Files", "", ""),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "file:///{path}",
    "name": "Files"
  })JSON"));
}

TEST(make_tool_descriptor_omits_an_empty_description) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "calc", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON")),
      sourcemeta::core::parse_json(R"JSON({
    "name": "calc",
    "inputSchema": { "type": "object" },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON"));
}

TEST(make_resource_text_content_omits_an_empty_mime_type) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_text_content("file:///a", "",
                                                             "Hello"),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "text": "Hello"
  })JSON"));
}

TEST(make_resource_blob_content_omits_an_empty_mime_type) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resource_blob_content("file:///a", "", "YWJj"),
      sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "blob": "YWJj"
  })JSON"));
}

// A server identifies itself on every result it sends, including the interim
// and subscription results whose own result type rules out the complete-result
// decorator
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#responses
TEST(tool_success_carries_server_information_2026_07_28) {
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto identifier{sourcemeta::core::JSON{1}};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
                sourcemeta::core::parse_json(R"JSON({ "ok": true })JSON"),
                server),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "content": [ { "type": "text", "text": "{\n  \"ok\": true\n}" } ],
      "structuredContent": { "ok": true },
      "isError": false,
      "resultType": "complete",
      "_meta": {
        "io.modelcontextprotocol/serverInfo": {
          "name": "srv",
          "version": "1.0.0"
        }
      }
    }
  })JSON"));
}

TEST(tool_error_carries_server_information_2026_07_28) {
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto identifier{sourcemeta::core::JSON{2}};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
                "Nope", server),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 2,
    "result": {
      "content": [ { "type": "text", "text": "Nope" } ],
      "isError": true,
      "resultType": "complete",
      "_meta": {
        "io.modelcontextprotocol/serverInfo": {
          "name": "srv",
          "version": "1.0.0"
        }
      }
    }
  })JSON"));
}

TEST(tool_success_ignores_server_information_on_2025_11_25) {
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto identifier{sourcemeta::core::JSON{1}};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, identifier,
                sourcemeta::core::parse_json(R"JSON({ "ok": true })JSON"),
                server),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "content": [ { "type": "text", "text": "{\n  \"ok\": true\n}" } ],
      "structuredContent": { "ok": true },
      "isError": false
    }
  })JSON"));
}

TEST(input_required_result_carries_server_information) {
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{50}, std::nullopt, "token",
                sourcemeta::core::MCPClientCapabilities{}, server),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 50,
    "result": {
      "resultType": "input_required",
      "requestState": "token",
      "_meta": {
        "io.modelcontextprotocol/serverInfo": {
          "name": "srv",
          "version": "1.0.0"
        }
      }
    }
  })JSON"));
}

TEST(subscription_close_result_carries_server_information) {
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_close_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{"sub-1"}, server),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "sub-1",
    "result": {
      "resultType": "complete",
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": "sub-1",
        "io.modelcontextprotocol/serverInfo": {
          "name": "srv",
          "version": "1.0.0"
        }
      }
    }
  })JSON"));
}

// A sampling message carries text, an image, audio or a tool exchange
// https://modelcontextprotocol.io/specification/2026-07-28/schema#samplingmessage
TEST(input_required_result_rejects_a_resource_link_sampling_message) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 10,
        "messages": [
          {
            "role": "user",
            "content": {
              "type": "resource_link",
              "uri": "x",
              "name": "x"
            }
          }
        ]
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_a_non_numeric_temperature) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 10,
        "messages": [],
        "temperature": false
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// A model preference priority is normalized between zero and one
// https://modelcontextprotocol.io/specification/2026-07-28/schema#modelpreferences
TEST(input_required_result_rejects_an_out_of_range_model_priority) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 10,
        "messages": [],
        "modelPreferences": { "costPriority": 2 }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_an_unknown_tool_choice_mode) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 10,
        "messages": [],
        "toolChoice": { "mode": "bad" }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// Only a tool call, a resource read and a prompt get may answer with an
// interim result
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#supported-requests
TEST(input_required_result_rejects_tools_list) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/list", sourcemeta::core::JSON{0}, std::nullopt,
                         "token", {}),
                     "Operation does not support MRTR");
}

TEST(input_required_result_rejects_prompts_list) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "prompts/list", sourcemeta::core::JSON{0},
                         std::nullopt, "token", {}),
                     "Operation does not support MRTR");
}

TEST(input_required_result_rejects_server_discover) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "server/discover", sourcemeta::core::JSON{0},
                         std::nullopt, "token", {}),
                     "Operation does not support MRTR");
}

TEST(input_required_result_rejects_2025_03_26) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                         "tools/call", sourcemeta::core::JSON{0}, std::nullopt,
                         "token", {}),
                     "MRTR requires MCP 2026-07-28");
}

TEST(input_required_result_rejects_2025_06_18) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                         "tools/call", sourcemeta::core::JSON{0}, std::nullopt,
                         "token", {}),
                     "MRTR requires MCP 2026-07-28");
}

TEST(input_required_result_rejects_2025_11_25) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         "tools/call", sourcemeta::core::JSON{0}, std::nullopt,
                         "token", {}),
                     "MRTR requires MCP 2026-07-28");
}

// An interim result carries input requests, a state token, or both
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#inputrequiredresult
TEST(input_required_result_requires_requests_or_state) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0}, std::nullopt,
                         std::nullopt, {}),
                     "An interim result requires input or state");
}

// The client echoes the state token back on the request that continues the
// operation
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#client-requirements-basic-workflow
TEST(request_state_of_a_continuation) {
  const auto continuation{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": { "requestState": "s", "inputResponses": {} }
  })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_request_state(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, continuation)
          .value(),
      "s");
}

TEST(request_state_on_2025_11_25) {
  const auto continuation{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": { "requestState": "s", "inputResponses": {} }
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_request_state(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, continuation)
          .has_value());
}

TEST(request_input_responses_of_a_non_mrtr_method) {
  const auto continuation{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": { "requestState": "s", "inputResponses": {} }
  })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_request_input_responses(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, continuation),
      nullptr);
}

TEST(make_empty_result_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON{0}),
            sourcemeta::core::parse_json(
                R"JSON({ "jsonrpc": "2.0", "id": 0, "result": {} })JSON"));
}

TEST(make_empty_result_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON{0}),
            sourcemeta::core::parse_json(
                R"JSON({ "jsonrpc": "2.0", "id": 0, "result": {} })JSON"));
}

TEST(make_empty_result_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON{0}),
            sourcemeta::core::parse_json(
                R"JSON({ "jsonrpc": "2.0", "id": 0, "result": {} })JSON"));
}

TEST(make_empty_result_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_empty_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": { "resultType": "complete" }
  })JSON"));
}

TEST(make_tool_success_from_a_structured_payload_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({ "value": 1 })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "{\n  \"value\": 1\n}" } ],
      "isError": false
    }
  })JSON"));
}

TEST(make_tool_success_from_a_structured_payload_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({ "value": 1 })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "{\n  \"value\": 1\n}" } ],
      "structuredContent": { "value": 1 },
      "isError": false
    }
  })JSON"));
}

TEST(make_tool_success_from_a_structured_payload_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({ "value": 1 })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "{\n  \"value\": 1\n}" } ],
      "structuredContent": { "value": 1 },
      "isError": false
    }
  })JSON"));
}

TEST(make_tool_success_from_a_structured_payload_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({ "value": 1 })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "resultType": "complete",
      "content": [ { "type": "text", "text": "{\n  \"value\": 1\n}" } ],
      "structuredContent": { "value": 1 },
      "isError": false
    }
  })JSON"));
}

TEST(make_tool_success_from_content_blocks_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON{0},
                sourcemeta::core::JSON::make_object(),
                sourcemeta::core::parse_json(
                    R"JSON([ { "type": "text", "text": "hello" } ])JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "hello" } ],
      "isError": false
    }
  })JSON"));
}

TEST(make_tool_success_from_content_blocks_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON{0},
                sourcemeta::core::JSON::make_object(),
                sourcemeta::core::parse_json(
                    R"JSON([ { "type": "text", "text": "hello" } ])JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "hello" } ],
      "structuredContent": {},
      "isError": false
    }
  })JSON"));
}

TEST(make_tool_success_from_content_blocks_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON{0},
                sourcemeta::core::JSON::make_object(),
                sourcemeta::core::parse_json(
                    R"JSON([ { "type": "text", "text": "hello" } ])JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "hello" } ],
      "structuredContent": {},
      "isError": false
    }
  })JSON"));
}

TEST(make_tool_success_from_content_blocks_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0},
                sourcemeta::core::JSON::make_object(),
                sourcemeta::core::parse_json(
                    R"JSON([ { "type": "text", "text": "hello" } ])JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "resultType": "complete",
      "content": [ { "type": "text", "text": "hello" } ],
      "structuredContent": {},
      "isError": false
    }
  })JSON"));
}

TEST(make_tool_error_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON{0}, "failure"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "failure" } ],
      "isError": true
    }
  })JSON"));
}

TEST(make_tool_error_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON{0}, "failure"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "failure" } ],
      "isError": true
    }
  })JSON"));
}

TEST(make_tool_error_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_error(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON{0}, "failure"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "content": [ { "type": "text", "text": "failure" } ],
      "isError": true
    }
  })JSON"));
}

TEST(make_tool_error_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_error(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, "failure"),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "resultType": "complete",
      "content": [ { "type": "text", "text": "failure" } ],
      "isError": true
    }
  })JSON"));
}

TEST(make_tool_descriptor_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tool",
                "description",
                sourcemeta::core::parse_json(
                    R"JSON({ "type": "object", "properties": {} })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "name": "tool",
    "description": "description",
    "inputSchema": { "type": "object", "properties": {} },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON"));
}

TEST(make_tool_descriptor_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tool",
                "description",
                sourcemeta::core::parse_json(
                    R"JSON({ "type": "object", "properties": {} })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "name": "tool",
    "description": "description",
    "inputSchema": { "type": "object", "properties": {} },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON"));
}

TEST(make_tool_descriptor_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tool",
                "description",
                sourcemeta::core::parse_json(
                    R"JSON({ "type": "object", "properties": {} })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "name": "tool",
    "description": "description",
    "inputSchema": { "type": "object", "properties": {} },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON"));
}

TEST(make_tool_descriptor_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tool",
                "description",
                sourcemeta::core::parse_json(
                    R"JSON({ "type": "object", "properties": {} })JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "name": "tool",
    "description": "description",
    "inputSchema": { "type": "object", "properties": {} },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON"));
}

TEST(make_resource_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                "https://example.com", "resource", "text/plain", "", 10, 0.5),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "https://example.com",
    "name": "resource",
    "mimeType": "text/plain",
    "size": 10,
    "annotations": { "priority": 0.5 }
  })JSON"));
}

TEST(make_resource_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                "https://example.com", "resource", "text/plain", "", 10, 0.5),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "https://example.com",
    "name": "resource",
    "mimeType": "text/plain",
    "size": 10,
    "annotations": { "priority": 0.5 }
  })JSON"));
}

TEST(make_resource_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                "https://example.com", "resource", "text/plain", "", 10, 0.5),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "https://example.com",
    "name": "resource",
    "mimeType": "text/plain",
    "size": 10,
    "annotations": { "priority": 0.5 }
  })JSON"));
}

TEST(make_resource_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "https://example.com", "resource", "text/plain", "", 10, 0.5),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "https://example.com",
    "name": "resource",
    "mimeType": "text/plain",
    "size": 10,
    "annotations": { "priority": 0.5 }
  })JSON"));
}

TEST(make_resource_template_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                "https://example.com/{name}", "template", "description",
                "text/plain"),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "https://example.com/{name}",
    "name": "template",
    "description": "description",
    "mimeType": "text/plain"
  })JSON"));
}

TEST(make_resource_template_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                "https://example.com/{name}", "template", "description",
                "text/plain"),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "https://example.com/{name}",
    "name": "template",
    "description": "description",
    "mimeType": "text/plain"
  })JSON"));
}

TEST(make_resource_template_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                "https://example.com/{name}", "template", "description",
                "text/plain"),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "https://example.com/{name}",
    "name": "template",
    "description": "description",
    "mimeType": "text/plain"
  })JSON"));
}

TEST(make_resource_template_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "https://example.com/{name}", "template", "description",
                "text/plain"),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "https://example.com/{name}",
    "name": "template",
    "description": "description",
    "mimeType": "text/plain"
  })JSON"));
}

TEST(make_tools_list_result_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tools_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "tools": [], "nextCursor": "" })JSON"));
}

TEST(make_tools_list_result_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tools_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "tools": [], "nextCursor": "" })JSON"));
}

TEST(make_tools_list_result_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tools_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "tools": [], "nextCursor": "" })JSON"));
}

TEST(make_tools_list_result_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tools_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "tools": [],
    "nextCursor": "",
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(make_resources_list_result_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "resources": [], "nextCursor": "" })JSON"));
}

TEST(make_resources_list_result_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "resources": [], "nextCursor": "" })JSON"));
}

TEST(make_resources_list_result_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "resources": [], "nextCursor": "" })JSON"));
}

TEST(make_resources_list_result_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resources": [],
    "nextCursor": "",
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(make_resource_templates_list_result_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_templates_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "resourceTemplates": [], "nextCursor": "" })JSON"));
}

TEST(make_resource_templates_list_result_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_templates_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "resourceTemplates": [], "nextCursor": "" })JSON"));
}

TEST(make_resource_templates_list_result_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_templates_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "resourceTemplates": [], "nextCursor": "" })JSON"));
}

TEST(make_resource_templates_list_result_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_templates_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resourceTemplates": [],
    "nextCursor": "",
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(make_prompts_list_result_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "prompts": [], "nextCursor": "" })JSON"));
}

TEST(make_prompts_list_result_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "prompts": [], "nextCursor": "" })JSON"));
}

TEST(make_prompts_list_result_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(
                R"JSON({ "prompts": [], "nextCursor": "" })JSON"));
}

TEST(make_prompts_list_result_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON::make_array(), "", {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "prompts": [],
    "nextCursor": "",
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(make_resources_read_result_2025_03_26) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resources_read_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
          sourcemeta::core::parse_json(
              R"JSON([ { "uri": "https://example.com", "text": "hello" } ])JSON"),
          {}),
      sourcemeta::core::parse_json(R"JSON({
    "contents": [ { "uri": "https://example.com", "text": "hello" } ]
  })JSON"));
}

TEST(make_resources_read_result_2025_06_18) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resources_read_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
          sourcemeta::core::parse_json(
              R"JSON([ { "uri": "https://example.com", "text": "hello" } ])JSON"),
          {}),
      sourcemeta::core::parse_json(R"JSON({
    "contents": [ { "uri": "https://example.com", "text": "hello" } ]
  })JSON"));
}

TEST(make_resources_read_result_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resources_read_result(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::parse_json(
              R"JSON([ { "uri": "https://example.com", "text": "hello" } ])JSON"),
          {}),
      sourcemeta::core::parse_json(R"JSON({
    "contents": [ { "uri": "https://example.com", "text": "hello" } ]
  })JSON"));
}

TEST(make_resources_read_result_2026_07_28) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_resources_read_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(
              R"JSON([ { "uri": "https://example.com", "text": "hello" } ])JSON"),
          {}),
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "contents": [ { "uri": "https://example.com", "text": "hello" } ],
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(make_prompts_get_result_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_get_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                "description", sourcemeta::core::parse_json(R"JSON([
    {
      "role": "user",
      "content": { "type": "text", "text": "hello" }
    }
  ])JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "description": "description",
    "messages": [
      {
        "role": "user",
        "content": { "type": "text", "text": "hello" }
      }
    ]
  })JSON"));
}

TEST(make_prompts_get_result_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_get_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                "description", sourcemeta::core::parse_json(R"JSON([
    {
      "role": "user",
      "content": { "type": "text", "text": "hello" }
    }
  ])JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "description": "description",
    "messages": [
      {
        "role": "user",
        "content": { "type": "text", "text": "hello" }
      }
    ]
  })JSON"));
}

TEST(make_prompts_get_result_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_get_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                "description", sourcemeta::core::parse_json(R"JSON([
    {
      "role": "user",
      "content": { "type": "text", "text": "hello" }
    }
  ])JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "description": "description",
    "messages": [
      {
        "role": "user",
        "content": { "type": "text", "text": "hello" }
      }
    ]
  })JSON"));
}

TEST(make_prompts_get_result_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_get_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "description", sourcemeta::core::parse_json(R"JSON([
    {
      "role": "user",
      "content": { "type": "text", "text": "hello" }
    }
  ])JSON")),
            sourcemeta::core::parse_json(R"JSON({
    "description": "description",
    "messages": [
      {
        "role": "user",
        "content": { "type": "text", "text": "hello" }
      }
    ],
    "resultType": "complete"
  })JSON"));
}

TEST(make_completion_result_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::parse_json(R"JSON([ "one", "two" ])JSON"), 3,
                true),
            sourcemeta::core::parse_json(R"JSON({
    "completion": {
      "values": [ "one", "two" ],
      "total": 3,
      "hasMore": true
    }
  })JSON"));
}

TEST(make_completion_result_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::parse_json(R"JSON([ "one", "two" ])JSON"), 3,
                true),
            sourcemeta::core::parse_json(R"JSON({
    "completion": {
      "values": [ "one", "two" ],
      "total": 3,
      "hasMore": true
    }
  })JSON"));
}

TEST(make_completion_result_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::parse_json(R"JSON([ "one", "two" ])JSON"), 3,
                true),
            sourcemeta::core::parse_json(R"JSON({
    "completion": {
      "values": [ "one", "two" ],
      "total": 3,
      "hasMore": true
    }
  })JSON"));
}

TEST(make_completion_result_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_completion_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON([ "one", "two" ])JSON"), 3,
                true),
            sourcemeta::core::parse_json(R"JSON({
    "completion": {
      "values": [ "one", "two" ],
      "total": 3,
      "hasMore": true
    },
    "resultType": "complete"
  })JSON"));
}

TEST(make_resource_link_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_link(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                "https://example.com", "link"),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "https://example.com",
    "name": "link"
  })JSON"));
}

TEST(make_resource_link_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_link(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                "https://example.com", "link"),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "https://example.com",
    "name": "link"
  })JSON"));
}

TEST(make_resource_link_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_link(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "https://example.com", "link"),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource_link",
    "uri": "https://example.com",
    "name": "link"
  })JSON"));
}

// Resource link content blocks arrived in 2025-06-18
// https://modelcontextprotocol.io/specification/2026-07-28/schema#resourcelink
TEST(make_resource_link_rejects_2025_03_26) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resource_link(
                         sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                         "https://example.com", "link"),
                     "Resource links require MCP 2025-06-18 or later");
}

TEST(make_initialize_result_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const sourcemeta::core::MCPImplementation server{.name = "server",
                                                   .version = "1",
                                                   .title = "Title",
                                                   .description = "Description",
                                                   .website_url =
                                                       "https://example.com"};
  EXPECT_EQ(sourcemeta::core::mcp_make_initialize_result(
                sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-03-26",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON"),
                {}, server, SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "protocolVersion": "2025-03-26",
      "capabilities": {},
      "serverInfo": { "name": "server", "version": "1" }
    }
  })JSON"));
}

TEST(make_initialize_result_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const sourcemeta::core::MCPImplementation server{.name = "server",
                                                   .version = "1",
                                                   .title = "Title",
                                                   .description = "Description",
                                                   .website_url =
                                                       "https://example.com"};
  EXPECT_EQ(sourcemeta::core::mcp_make_initialize_result(
                sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-06-18",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON"),
                {}, server, SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "protocolVersion": "2025-06-18",
      "capabilities": {},
      "serverInfo": { "name": "server", "version": "1", "title": "Title" }
    }
  })JSON"));
}

TEST(make_initialize_result_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const sourcemeta::core::MCPImplementation server{.name = "server",
                                                   .version = "1",
                                                   .title = "Title",
                                                   .description = "Description",
                                                   .website_url =
                                                       "https://example.com"};
  EXPECT_EQ(sourcemeta::core::mcp_make_initialize_result(
                sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "initialize",
    "params": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "clientInfo": { "name": "client", "version": "1" }
    }
  })JSON"),
                {}, server, SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "protocolVersion": "2025-11-25",
      "capabilities": {},
      "serverInfo": {
        "name": "server",
        "version": "1",
        "title": "Title",
        "description": "Description",
        "websiteUrl": "https://example.com"
      }
    }
  })JSON"));
}

TEST(make_server_discover_result) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const sourcemeta::core::MCPImplementation server{.name = "server",
                                                   .version = "1",
                                                   .title = "Title",
                                                   .description = "Description",
                                                   .website_url =
                                                       "https://example.com"};
  EXPECT_EQ(sourcemeta::core::mcp_make_server_discover_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, {}, server, SUPPORTED,
                "instructions", {}),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "resultType": "complete",
      "supportedVersions": [
        "2025-03-26",
        "2025-06-18",
        "2025-11-25",
        "2026-07-28"
      ],
      "capabilities": {},
      "_meta": {
        "io.modelcontextprotocol/serverInfo": {
          "name": "server",
          "version": "1",
          "title": "Title",
          "description": "Description",
          "websiteUrl": "https://example.com"
        }
      },
      "instructions": "instructions",
      "ttlMs": 0,
      "cacheScope": "private"
    }
  })JSON"));
}

TEST(make_input_required_result_from_request_state) {
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "resources/read", sourcemeta::core::JSON{0}, std::nullopt,
                "state", {}),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "result": {
      "resultType": "input_required",
      "requestState": "state"
    }
  })JSON"));
}

// The continuation echoes the state token exactly and answers the requests
// the server asked for, while an unrecognized key is ignored
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#client-requirements-basic-workflow
TEST(validate_continuation_with_every_response_kind) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": {
      "roots": { "roots": [ { "uri": "file:///workspace" } ] },
      "form": {
        "action": "accept",
        "content": { "choice": [ "a" ], "accepted": true }
      },
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello" }
      },
      "unknown": { "ignored": true }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" },
    "form": { "method": "elicitation/create" },
    "sample": { "method": "sampling/createMessage" }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      "state"));
}

TEST(validate_continuation_rejects_a_different_state_token) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": {
      "roots": { "roots": [ { "uri": "file:///workspace" } ] },
      "form": {
        "action": "accept",
        "content": { "choice": [ "a" ], "accepted": true }
      },
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello" }
      },
      "unknown": { "ignored": true }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" },
    "form": { "method": "elicitation/create" },
    "sample": { "method": "sampling/createMessage" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      "different"));
}

TEST(validate_continuation_rejects_2025_11_25) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": {
      "roots": { "roots": [ { "uri": "file:///workspace" } ] },
      "form": {
        "action": "accept",
        "content": { "choice": [ "a" ], "accepted": true }
      },
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello" }
      },
      "unknown": { "ignored": true }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" },
    "form": { "method": "elicitation/create" },
    "sample": { "method": "sampling/createMessage" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, parameters, prior,
      "state"));
}

// A root is a file URI
// https://modelcontextprotocol.io/specification/2026-07-28/schema#root
TEST(validate_continuation_rejects_a_non_file_root) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": {
      "roots": { "roots": [ { "uri": "https://example.com" } ] }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "roots": { "method": "roots/list" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      "state"));
}

// An elicitation is accepted, declined or cancelled
// https://modelcontextprotocol.io/specification/2026-07-28/schema#elicitresult
TEST(validate_continuation_rejects_an_unknown_elicitation_action) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": { "form": { "action": "unknown" } }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "form": { "method": "elicitation/create" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      "state"));
}

// A sampling result names the model that produced it
// https://modelcontextprotocol.io/specification/2026-07-28/schema#createmessageresult
TEST(validate_continuation_rejects_a_sampling_response_without_a_model) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "content": { "type": "text", "text": "hello" }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      "state"));
}

TEST(validate_continuation_rejects_non_object_responses) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": false
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" },
    "form": { "method": "elicitation/create" },
    "sample": { "method": "sampling/createMessage" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      "state"));
}

TEST(validate_continuation_rejects_a_non_string_state_token) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": 1,
    "inputResponses": {}
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" },
    "form": { "method": "elicitation/create" },
    "sample": { "method": "sampling/createMessage" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

// A continuation carries responses, a state token, or both
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#client-requirements-basic-workflow
TEST(validate_continuation_rejects_neither_responses_nor_state) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" },
    "form": { "method": "elicitation/create" },
    "sample": { "method": "sampling/createMessage" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(continuation_empty_expected_state) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.assign("inputResponses", sourcemeta::core::JSON::make_object());
  const auto prior{sourcemeta::core::JSON::make_object()};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, value, prior));
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, value, prior, ""));
  value.assign("requestState", sourcemeta::core::JSON{""});
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, value, prior, ""));
  value.assign("requestState", sourcemeta::core::JSON{"different"});
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, value, prior, ""));
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::JSON{false}, prior, ""));
  value.assign("requestState", sourcemeta::core::JSON{false});
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, value, prior, ""));
}

// A tool's input schema is carried through untouched, so neither a nested
// annotation-looking key nor a reference is interpreted
// https://modelcontextprotocol.io/specification/2026-07-28/schema#tool
TEST(tool_schema_content_is_opaque_2025_03_26) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "type": "object",
    "$defs": { "value": { "type": "string" } },
    "properties": { "name": { "$ref": "#/$defs/value" } },
    "examples": [
      {
        "x-mcp-header": "Not an annotation",
        "properties": { "type": "invalid" }
      }
    ]
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tool", "",
                schema)
                .at("inputSchema"),
            schema);
}

TEST(tool_schema_content_is_opaque_2025_06_18) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "type": "object",
    "$defs": { "value": { "type": "string" } },
    "properties": { "name": { "$ref": "#/$defs/value" } },
    "examples": [
      {
        "x-mcp-header": "Not an annotation",
        "properties": { "type": "invalid" }
      }
    ]
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tool", "",
                schema)
                .at("inputSchema"),
            schema);
}

TEST(tool_schema_content_is_opaque_2025_11_25) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "type": "object",
    "$defs": { "value": { "type": "string" } },
    "properties": { "name": { "$ref": "#/$defs/value" } },
    "examples": [
      {
        "x-mcp-header": "Not an annotation",
        "properties": { "type": "invalid" }
      }
    ]
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tool", "",
                schema)
                .at("inputSchema"),
            schema);
}

TEST(tool_schema_content_is_opaque_2026_07_28) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "type": "object",
    "$defs": { "value": { "type": "string" } },
    "properties": { "name": { "$ref": "#/$defs/value" } },
    "examples": [
      {
        "x-mcp-header": "Not an annotation",
        "properties": { "type": "invalid" }
      }
    ]
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tool", "",
                schema)
                .at("inputSchema"),
            schema);
}

TEST(make_tool_descriptor_rejects_a_boolean_schema) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_descriptor(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tool", "",
                         sourcemeta::core::parse_json(R"JSON(false)JSON")),
                     "Invalid MCP tool descriptor");
}

TEST(make_tool_descriptor_rejects_a_null_schema) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_descriptor(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tool", "",
                         sourcemeta::core::parse_json(R"JSON(null)JSON")),
                     "Invalid MCP tool descriptor");
}

TEST(make_tool_descriptor_rejects_an_array_schema) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_descriptor(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tool", "",
                         sourcemeta::core::parse_json(R"JSON([])JSON")),
                     "Invalid MCP tool descriptor");
}

TEST(make_tool_descriptor_rejects_an_untyped_object_schema) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_descriptor(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tool", "",
                         sourcemeta::core::parse_json(R"JSON({})JSON")),
                     "Invalid MCP tool descriptor");
}

TEST(make_tool_descriptor_rejects_an_array_type_schema) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tool", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "array" })JSON")),
      "Invalid MCP tool descriptor");
}

// Form content answers a form elicitation, and a URL elicitation carries no
// content to validate
// https://modelcontextprotocol.io/specification/2026-07-28/client/elicitation
TEST(validate_continuation_accepts_fractional_form_content) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": {
      "form": {
        "action": "accept",
        "content": { "value": 1.25, "precise": 1.0 }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "form": {
      "method": "elicitation/create",
      "params": {
        "mode": "form",
        "message": "value",
        "requestedSchema": { "type": "object", "properties": {} }
      }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_content_for_a_url_elicitation) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": {
      "form": { "action": "accept", "content": { "value": 1.25 } }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "form": {
      "method": "elicitation/create",
      "params": {
        "mode": "url",
        "message": "value",
        "requestedSchema": { "type": "object", "properties": {} }
      }
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_accepts_a_url_elicitation_without_content) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": { "form": { "action": "accept" } }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "form": {
      "method": "elicitation/create",
      "params": {
        "mode": "url",
        "message": "value",
        "requestedSchema": { "type": "object", "properties": {} }
      }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_accepts_a_declined_elicitation) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": { "form": { "action": "decline" } }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "form": {
      "method": "elicitation/create",
      "params": {
        "mode": "form",
        "message": "value",
        "requestedSchema": { "type": "object", "properties": {} }
      }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_accepts_a_cancelled_elicitation) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": { "form": { "action": "cancel" } }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "form": {
      "method": "elicitation/create",
      "params": {
        "mode": "form",
        "message": "value",
        "requestedSchema": { "type": "object", "properties": {} }
      }
    }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

// Only an accepted elicitation carries content
// https://modelcontextprotocol.io/specification/2026-07-28/schema#elicitresult
TEST(validate_continuation_rejects_content_on_a_declined_elicitation) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": { "form": { "action": "decline", "content": {} } }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "form": {
      "method": "elicitation/create",
      "params": {
        "mode": "form",
        "message": "value",
        "requestedSchema": { "type": "object", "properties": {} }
      }
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_content_on_a_cancelled_elicitation) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "state",
    "inputResponses": { "form": { "action": "cancel", "content": {} } }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "form": {
      "method": "elicitation/create",
      "params": {
        "mode": "form",
        "message": "value",
        "requestedSchema": { "type": "object", "properties": {} }
      }
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

// A token budget is a number the protocol does not otherwise constrain
// https://modelcontextprotocol.io/specification/2026-07-28/schema#createmessagerequestparams
TEST(input_required_result_accepts_a_fractional_token_budget) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({
    "sample": {
      "method": "sampling/createMessage",
      "params": { "messages": [], "maxTokens": 100.0 }
    }
  })JSON"),
                std::nullopt, capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

TEST(input_required_result_accepts_a_zero_token_budget) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({
    "sample": {
      "method": "sampling/createMessage",
      "params": { "messages": [], "maxTokens": 0 }
    }
  })JSON"),
                std::nullopt, capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

TEST(input_required_result_accepts_a_negative_token_budget) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0},
                sourcemeta::core::parse_json(R"JSON({
    "sample": {
      "method": "sampling/createMessage",
      "params": { "messages": [], "maxTokens": -1 }
    }
  })JSON"),
                std::nullopt, capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

// A title arrived in 2025-06-18 and icons in 2025-11-25, so a descriptor
// built for an earlier revision carries neither
// https://modelcontextprotocol.io/specification/2026-07-28/schema#implementation
TEST(make_resource_with_presentation_and_annotations_2025_03_26) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a",
                "a", "text/plain", "description", std::nullopt, std::nullopt,
                presentation, annotations),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "a",
    "description": "description",
    "mimeType": "text/plain",
    "annotations": { "audience": [ "user", "assistant" ], "priority": 0.5 }
  })JSON"));
}

TEST(make_resource_with_presentation_and_annotations_2025_06_18) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "file:///a",
                "a", "text/plain", "description", std::nullopt, std::nullopt,
                presentation, annotations),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "a",
    "description": "description",
    "mimeType": "text/plain",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    },
    "title": "Title"
  })JSON"));
}

TEST(make_resource_with_presentation_and_annotations_2025_11_25) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "file:///a",
                "a", "text/plain", "description", std::nullopt, std::nullopt,
                presentation, annotations),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "a",
    "description": "description",
    "mimeType": "text/plain",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    },
    "title": "Title",
    "icons": [
      {
        "src": "https://example.com/icon.png",
        "mimeType": "image/png",
        "sizes": [ "32x32" ],
        "theme": "dark"
      }
    ]
  })JSON"));
}

TEST(make_resource_with_presentation_and_annotations_2026_07_28) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_resource(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "file:///a",
                "a", "text/plain", "description", std::nullopt, std::nullopt,
                presentation, annotations),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "name": "a",
    "description": "description",
    "mimeType": "text/plain",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    },
    "title": "Title",
    "icons": [
      {
        "src": "https://example.com/icon.png",
        "mimeType": "image/png",
        "sizes": [ "32x32" ],
        "theme": "dark"
      }
    ]
  })JSON"));
}

TEST(make_resource_template_with_presentation_2025_03_26) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                "file:///{name}", "template", "description", "text/plain",
                presentation, annotations),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "file:///{name}",
    "name": "template",
    "description": "description",
    "mimeType": "text/plain",
    "annotations": { "audience": [ "user", "assistant" ], "priority": 0.5 }
  })JSON"));
}

TEST(make_resource_template_with_presentation_2025_06_18) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                "file:///{name}", "template", "description", "text/plain",
                presentation, annotations),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "file:///{name}",
    "name": "template",
    "description": "description",
    "mimeType": "text/plain",
    "title": "Title",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_resource_template_with_presentation_2025_11_25) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                "file:///{name}", "template", "description", "text/plain",
                presentation, annotations),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "file:///{name}",
    "name": "template",
    "description": "description",
    "mimeType": "text/plain",
    "title": "Title",
    "icons": [
      {
        "src": "https://example.com/icon.png",
        "mimeType": "image/png",
        "sizes": [ "32x32" ],
        "theme": "dark"
      }
    ],
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_resource_template_with_presentation_2026_07_28) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_template(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "file:///{name}", "template", "description", "text/plain",
                presentation, annotations),
            sourcemeta::core::parse_json(R"JSON({
    "uriTemplate": "file:///{name}",
    "name": "template",
    "description": "description",
    "mimeType": "text/plain",
    "title": "Title",
    "icons": [
      {
        "src": "https://example.com/icon.png",
        "mimeType": "image/png",
        "sizes": [ "32x32" ],
        "theme": "dark"
      }
    ],
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_tool_descriptor_with_presentation_2025_03_26) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "tool",
                "description",
                sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
                std::nullopt, {}, presentation),
            sourcemeta::core::parse_json(R"JSON({
    "name": "tool",
    "description": "description",
    "inputSchema": { "type": "object" },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    }
  })JSON"));
}

TEST(make_tool_descriptor_with_presentation_2025_06_18) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "tool",
                "description",
                sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
                std::nullopt, {}, presentation),
            sourcemeta::core::parse_json(R"JSON({
    "name": "tool",
    "description": "description",
    "inputSchema": { "type": "object" },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    },
    "title": "Title"
  })JSON"));
}

TEST(make_tool_descriptor_with_presentation_2025_11_25) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tool",
                "description",
                sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
                std::nullopt, {}, presentation),
            sourcemeta::core::parse_json(R"JSON({
    "name": "tool",
    "description": "description",
    "inputSchema": { "type": "object" },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    },
    "title": "Title",
    "icons": [
      {
        "src": "https://example.com/icon.png",
        "mimeType": "image/png",
        "sizes": [ "32x32" ],
        "theme": "dark"
      }
    ]
  })JSON"));
}

TEST(make_tool_descriptor_with_presentation_2026_07_28) {
  const auto icons{sourcemeta::core::parse_json(R"JSON([
    {
      "src": "https://example.com/icon.png",
      "mimeType": "image/png",
      "sizes": [ "32x32" ],
      "theme": "dark"
    }
  ])JSON")};
  const sourcemeta::core::MCPDescriptorPresentation presentation{
      .title = "Title", .icons = &icons};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_descriptor(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tool",
                "description",
                sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
                std::nullopt, {}, presentation),
            sourcemeta::core::parse_json(R"JSON({
    "name": "tool",
    "description": "description",
    "inputSchema": { "type": "object" },
    "annotations": {
      "readOnlyHint": false,
      "destructiveHint": true,
      "idempotentHint": false,
      "openWorldHint": true
    },
    "title": "Title",
    "icons": [
      {
        "src": "https://example.com/icon.png",
        "mimeType": "image/png",
        "sizes": [ "32x32" ],
        "theme": "dark"
      }
    ]
  })JSON"));
}

TEST(serialize_resource_annotations_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_resource_annotations(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, annotations),
      sourcemeta::core::parse_json(
          R"JSON({
            "audience": [ "user", "assistant" ],
            "priority": 0.5
          })JSON"));
}

TEST(serialize_resource_annotations_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_resource_annotations(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, annotations),
      sourcemeta::core::parse_json(R"JSON({
    "audience": [ "user", "assistant" ],
    "priority": 0.5,
    "lastModified": "2026-07-28T00:00:00Z"
  })JSON"));
}

TEST(serialize_resource_annotations_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_resource_annotations(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, annotations),
      sourcemeta::core::parse_json(R"JSON({
    "audience": [ "user", "assistant" ],
    "priority": 0.5,
    "lastModified": "2026-07-28T00:00:00Z"
  })JSON"));
}

TEST(serialize_resource_annotations_2026_07_28) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_resource_annotations(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, annotations),
      sourcemeta::core::parse_json(R"JSON({
    "audience": [ "user", "assistant" ],
    "priority": 0.5,
    "lastModified": "2026-07-28T00:00:00Z"
  })JSON"));
}

TEST(make_image_block_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_image_block(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "YWJj",
                "image/png", annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "image",
    "data": "YWJj",
    "mimeType": "image/png",
    "annotations": { "audience": [ "user", "assistant" ], "priority": 0.5 }
  })JSON"));
}

TEST(make_image_block_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_image_block(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "YWJj",
                "image/png", annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "image",
    "data": "YWJj",
    "mimeType": "image/png",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_image_block_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_image_block(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "YWJj",
                "image/png", annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "image",
    "data": "YWJj",
    "mimeType": "image/png",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_image_block_2026_07_28) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_image_block(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "YWJj",
                "image/png", annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "image",
    "data": "YWJj",
    "mimeType": "image/png",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_audio_block_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_audio_block(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "YWJj",
                "audio/wav", annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "audio",
    "data": "YWJj",
    "mimeType": "audio/wav",
    "annotations": { "audience": [ "user", "assistant" ], "priority": 0.5 }
  })JSON"));
}

TEST(make_audio_block_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_audio_block(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18, "YWJj",
                "audio/wav", annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "audio",
    "data": "YWJj",
    "mimeType": "audio/wav",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_audio_block_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_audio_block(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "YWJj",
                "audio/wav", annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "audio",
    "data": "YWJj",
    "mimeType": "audio/wav",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_audio_block_2026_07_28) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_audio_block(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "YWJj",
                "audio/wav", annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "audio",
    "data": "YWJj",
    "mimeType": "audio/wav",
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_resource_blob_content) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resource_blob_content(
                "file:///a", "application/octet-stream", "YWJj"),
            sourcemeta::core::parse_json(R"JSON({
    "uri": "file:///a",
    "mimeType": "application/octet-stream",
    "blob": "YWJj"
  })JSON"));
}

TEST(make_embedded_resource_from_a_blob_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_embedded_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::mcp_make_resource_blob_content(
                    "file:///a", "application/octet-stream", "YWJj"),
                annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource",
    "resource": {
      "uri": "file:///a",
      "mimeType": "application/octet-stream",
      "blob": "YWJj"
    },
    "annotations": { "audience": [ "user", "assistant" ], "priority": 0.5 }
  })JSON"));
}

TEST(make_embedded_resource_from_a_blob_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_embedded_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::mcp_make_resource_blob_content(
                    "file:///a", "application/octet-stream", "YWJj"),
                annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource",
    "resource": {
      "uri": "file:///a",
      "mimeType": "application/octet-stream",
      "blob": "YWJj"
    },
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_embedded_resource_from_a_blob_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_embedded_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::mcp_make_resource_blob_content(
                    "file:///a", "application/octet-stream", "YWJj"),
                annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource",
    "resource": {
      "uri": "file:///a",
      "mimeType": "application/octet-stream",
      "blob": "YWJj"
    },
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_embedded_resource_from_a_blob_2026_07_28) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_embedded_resource(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::mcp_make_resource_blob_content(
                    "file:///a", "application/octet-stream", "YWJj"),
                annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource",
    "resource": {
      "uri": "file:///a",
      "mimeType": "application/octet-stream",
      "blob": "YWJj"
    },
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_embedded_resource_from_text_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_embedded_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::mcp_make_resource_text_content(
                    "file:///a", "text/plain", "hello"),
                annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource",
    "resource": {
      "uri": "file:///a",
      "mimeType": "text/plain",
      "text": "hello"
    },
    "annotations": { "audience": [ "user", "assistant" ], "priority": 0.5 }
  })JSON"));
}

TEST(make_embedded_resource_from_text_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_embedded_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::mcp_make_resource_text_content(
                    "file:///a", "text/plain", "hello"),
                annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource",
    "resource": {
      "uri": "file:///a",
      "mimeType": "text/plain",
      "text": "hello"
    },
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_embedded_resource_from_text_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_embedded_resource(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::mcp_make_resource_text_content(
                    "file:///a", "text/plain", "hello"),
                annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource",
    "resource": {
      "uri": "file:///a",
      "mimeType": "text/plain",
      "text": "hello"
    },
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

TEST(make_embedded_resource_from_text_2026_07_28) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> ROLES{
      {"user", "assistant"}};
  const sourcemeta::core::MCPResourceAnnotations annotations{
      .audience = ROLES,
      .priority = 0.5,
      .last_modified = "2026-07-28T00:00:00Z"};
  EXPECT_EQ(sourcemeta::core::mcp_make_embedded_resource(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::mcp_make_resource_text_content(
                    "file:///a", "text/plain", "hello"),
                annotations),
            sourcemeta::core::parse_json(R"JSON({
    "type": "resource",
    "resource": {
      "uri": "file:///a",
      "mimeType": "text/plain",
      "text": "hello"
    },
    "annotations": {
      "audience": [ "user", "assistant" ],
      "priority": 0.5,
      "lastModified": "2026-07-28T00:00:00Z"
    }
  })JSON"));
}

// Only the user and the assistant are audiences
// https://modelcontextprotocol.io/specification/2026-07-28/schema#annotations
TEST(serialize_resource_annotations_rejects_an_unknown_audience) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> ROLES{{"system"}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_serialize_resource_annotations(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          {.audience = ROLES, .priority = std::nullopt, .last_modified = {}}),
      "Invalid annotation audience");
}

// An embedded resource carries either text or a blob
// https://modelcontextprotocol.io/specification/2026-07-28/schema#embeddedresource
TEST(make_embedded_resource_rejects_a_resource_without_contents) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_embedded_resource(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(R"JSON({ "uri": "file:///a" })JSON")),
      "Invalid embedded resource");
}

TEST(make_tool_descriptor_rejects_a_non_string_icon_source) {
  const auto icons{
      sourcemeta::core::parse_json(R"JSON([ { "src": true } ])JSON")};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_tool_descriptor(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tool", "",
          sourcemeta::core::parse_json(R"JSON({ "type": "object" })JSON"),
          std::nullopt, {}, {.title = {}, .icons = &icons}),
      "Invalid descriptor icons");
}

TEST(make_resource_rejects_an_unknown_icon_theme) {
  const auto icons{sourcemeta::core::parse_json(
      R"JSON([
        {
          "src": "https://example.com/i.png",
          "theme": "sepia"
        }
      ])JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resource(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "file:///a", "a", "text/plain", "", std::nullopt,
                         std::nullopt, {.title = {}, .icons = &icons}),
                     "Invalid descriptor icons");
}

// The writer emits the JSON-RPC envelope for one of the six cacheable
// operations, decorated for the revision it serves
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/caching#cacheable-results
TEST(write_result_tools_list_2026_07_28) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list",
      sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "tools": [] })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 123, .scope = sourcemeta::core::MCPCacheScope::Public});
  EXPECT_EQ(
      stream.str(),
      "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"tools\":[],\"resultType\":"
      "\"complete\",\"ttlMs\":123,\"cacheScope\":\"public\"}}");
}

TEST(write_result_resources_list_2026_07_28) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "resources": [] })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 123, .scope = sourcemeta::core::MCPCacheScope::Public});
  EXPECT_EQ(
      stream.str(),
      "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"resources\":[],"
      "\"resultType\":\"complete\",\"ttlMs\":123,\"cacheScope\":\"public\"}}");
}

TEST(write_result_resource_templates_list_2026_07_28) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/templates/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "resourceTemplates": [] })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 123, .scope = sourcemeta::core::MCPCacheScope::Public});
  EXPECT_EQ(
      stream.str(),
      "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"resourceTemplates\":[],"
      "\"resultType\":\"complete\",\"ttlMs\":123,\"cacheScope\":\"public\"}}");
}

TEST(write_result_prompts_list_2026_07_28) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "prompts/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "prompts": [] })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 123, .scope = sourcemeta::core::MCPCacheScope::Public});
  EXPECT_EQ(
      stream.str(),
      "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"prompts\":[],\"resultType\":"
      "\"complete\",\"ttlMs\":123,\"cacheScope\":\"public\"}}");
}

TEST(write_result_resources_read_2026_07_28) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/read", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(
          R"JSON({
            "contents": [ { "uri": "file:///a", "text": "hello" } ]
          })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 123, .scope = sourcemeta::core::MCPCacheScope::Public});
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"contents\":[{\"uri\":"
            "\"file:///"
            "a\",\"text\":\"hello\"}],\"resultType\":\"complete\",\"ttlMs\":"
            "123,\"cacheScope\":\"public\"}}");
}

TEST(write_result_tools_list_2025_11_25) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2025_11_25, "tools/list",
      sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "tools": [] })JSON"), std::nullopt);
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"tools\":[]}}");
}

TEST(write_result_resources_list_2025_11_25) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "resources": [] })JSON"),
      std::nullopt);
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"resources\":[]}}");
}

TEST(write_result_resource_templates_list_2025_11_25) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/templates/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "resourceTemplates": [] })JSON"),
      std::nullopt);
  EXPECT_EQ(
      stream.str(),
      "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"resourceTemplates\":[]}}");
}

TEST(write_result_prompts_list_2025_11_25) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "prompts/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "prompts": [] })JSON"),
      std::nullopt);
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"prompts\":[]}}");
}

TEST(write_result_resources_read_2025_11_25) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "resources/read", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(
          R"JSON({
            "contents": [ { "uri": "file:///a", "text": "hello" } ]
          })JSON"),
      std::nullopt);
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"contents\":[{\"uri\":"
            "\"file:///a\",\"text\":\"hello\"}]}}");
}

TEST(write_result_server_discover_2026_07_28) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "server/discover", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({
    "supportedVersions": [ "2026-07-28" ],
    "capabilities": {}
  })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 0, .scope = sourcemeta::core::MCPCacheScope::Private});
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"supportedVersions\":["
            "\"2026-07-28\"],\"capabilities\":{},\"resultType\":\"complete\","
            "\"ttlMs\":0,\"cacheScope\":\"private\"}}");
}

TEST(write_result_with_server_information) {
  const sourcemeta::core::MCPImplementation server{.name = "server",
                                                   .version = "1"};
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({ "resources": [] })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 0, .scope = sourcemeta::core::MCPCacheScope::Private},
      server);
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"resources\":[],"
            "\"resultType\":\"complete\",\"_meta\":{\"io.modelcontextprotocol/"
            "serverInfo\":{\"name\":\"server\",\"version\":\"1\"}},\"ttlMs\":0,"
            "\"cacheScope\":\"private\"}}");
}

TEST(write_result_preserves_a_pagination_cursor) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "tools/list",
      sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(
          R"JSON({ "tools": [], "nextCursor": "" })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 0, .scope = sourcemeta::core::MCPCacheScope::Private});
  EXPECT_EQ(stream.str(), "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"tools\":"
                          "[],\"nextCursor\":\"\",\"resultType\":\"complete\","
                          "\"ttlMs\":0,\"cacheScope\":\"private\"}}");
}

TEST(write_result_preserves_vendor_metadata_and_extension_keys) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({
    "resources": [],
    "_meta": { "org.example/custom": { "value": true } },
    "org.example/extension": [ null, true, "line" ]
  })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 0, .scope = sourcemeta::core::MCPCacheScope::Private});
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"resources\":[],\"_"
            "meta\":{\"org.example/custom\":{\"value\":true}},\"org.example/"
            "extension\":[null,true,\"line\"],\"resultType\":\"complete\","
            "\"ttlMs\":0,\"cacheScope\":\"private\"}}");
}

TEST(write_result_escapes_the_identifier) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/list", sourcemeta::core::JSON{"escaped\"\\\n"},
      sourcemeta::core::parse_json(R"JSON({ "resources": [] })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 0, .scope = sourcemeta::core::MCPCacheScope::Private});
  EXPECT_EQ(stream.str(),
            "{\"jsonrpc\":\"2.0\",\"id\":\"escaped\\\"\\\\\\n\",\"result\":{"
            "\"resources\":[],\"resultType\":\"complete\",\"ttlMs\":0,"
            "\"cacheScope\":\"private\"}}");
}

TEST(write_result_accepts_an_already_decorated_result) {
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/list", sourcemeta::core::JSON{1},
      sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resources": [],
    "ttlMs": 5,
    "cacheScope": "public"
  })JSON"),
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 123, .scope = sourcemeta::core::MCPCacheScope::Private});
  EXPECT_EQ(
      stream.str(),
      "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"resultType\":\"complete\","
      "\"resources\":[],\"ttlMs\":123,\"cacheScope\":\"private\"}}");
}

// The writer takes the result by reference and leaves it untouched
TEST(write_result_does_not_mutate_the_result) {
  const auto result{
      sourcemeta::core::parse_json(R"JSON({ "resources": [] })JSON")};
  std::ostringstream stream;
  sourcemeta::core::mcp_write_result(
      stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      "resources/list", sourcemeta::core::JSON{1}, result,
      sourcemeta::core::MCPCachePolicy{
          .ttl_ms = 0, .scope = sourcemeta::core::MCPCacheScope::Private});
  EXPECT_EQ(result,
            sourcemeta::core::parse_json(R"JSON({ "resources": [] })JSON"));
}

TEST(write_result_rejects_a_non_array_entry_collection) {
  std::ostringstream stream;
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_write_result(
          stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          "resources/list", sourcemeta::core::JSON{1},
          sourcemeta::core::parse_json(R"JSON({ "resources": false })JSON"),
          sourcemeta::core::MCPCachePolicy{}),
      "MCP result entries must be an array");
  EXPECT_TRUE(stream.str().empty());
}

TEST(write_result_rejects_a_non_string_cursor) {
  std::ostringstream stream;
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_write_result(
          stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          "resources/list", sourcemeta::core::JSON{1},
          sourcemeta::core::parse_json(
              R"JSON({ "resources": [], "nextCursor": false })JSON"),
          sourcemeta::core::MCPCachePolicy{}),
      "Invalid result cursor");
  EXPECT_TRUE(stream.str().empty());
}

TEST(write_result_rejects_non_object_metadata) {
  std::ostringstream stream;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_write_result(
                         stream,
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "resources/list", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "resources": [],
                               "_meta": false
                             })JSON"),
                         sourcemeta::core::MCPCachePolicy{}),
                     "Invalid MCP result object");
  EXPECT_TRUE(stream.str().empty());
}

// Only a complete result is cacheable, so an interim one cannot be written
// through the cacheable writer
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr
TEST(write_result_rejects_an_interim_result) {
  std::ostringstream stream;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_write_result(
                         stream,
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "resources/list", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(
                             R"JSON({
                "resources": [],
                "resultType": "input_required"
              })JSON"),
                         sourcemeta::core::MCPCachePolicy{}),
                     "Writer requires a complete result");
  EXPECT_TRUE(stream.str().empty());
}

// A tracing header may be empty or carry empty list members, which W3C
// permits and a receiver simply has nothing to read from
// https://www.w3.org/TR/trace-context/
TEST(request_parameters_accept_an_empty_tracestate) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "tracestate": ""
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(parameters).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(request_parameters_accept_a_tracestate_of_empty_members) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "tracestate": " , "
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(parameters).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(request_parameters_accept_a_tracestate_with_a_trailing_comma) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "tracestate": "vendor=value,,"
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(parameters).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

// A baggage member may carry a property without a value
// https://www.w3.org/TR/baggage/
TEST(request_parameters_accept_a_valueless_baggage_property) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "baggage": "key=value;flag"
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(parameters).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

// An extension key is a prefix with an empty name, which the key rules allow
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(request_parameters_accept_an_extension_key_without_a_name) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": { "extensions": { "org.example/": {} } }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(parameters).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(result_writer_metadata_rejections) {
  std::ostringstream stream;
  const auto bad{sourcemeta::core::parse_json(
      R"JSON({
        "resources": [],
        "_meta": {
          "io.modelcontextprotocol/serverInfo": { "name": "missing-version" }
        }
      })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_write_result(
                         stream,
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "resources/list", sourcemeta::core::JSON{0}, bad,
                         sourcemeta::core::MCPCachePolicy{}),
                     "Invalid precomputed server information");
  EXPECT_TRUE(stream.str().empty());
  const auto discovery{sourcemeta::core::parse_json(
      R"JSON({
        "supportedVersions": [ "2025-11-25" ],
        "capabilities": {}
      })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_write_result(
                         stream,
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "server/discover", sourcemeta::core::JSON{0},
                         discovery, sourcemeta::core::MCPCachePolicy{}),
                     "Discovery must include its serving revision");
}

// An elicitation schema may describe a single or a multiple choice, and the
// builder carries the schema through untouched
// https://modelcontextprotocol.io/specification/2026-07-28/schema#titledsingleselectenumschema
TEST(input_required_result_carries_enum_form_schemas_through) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "form": {
      "method": "elicitation/create",
      "params": {
        "mode": "form",
        "message": "Choose options",
        "requestedSchema": {
          "type": "object",
          "properties": {
            "single": {
              "type": "string",
              "oneOf": [
                { "const": "a", "title": "Alpha" },
                { "const": "b", "title": "Beta" }
              ]
            },
            "multiple": {
              "type": "array",
              "items": {
                "anyOf": [
                  { "const": "a", "title": "Alpha" },
                  { "const": "b", "title": "Beta" }
                ]
              }
            }
          }
        }
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{0}, requests, std::nullopt,
                capabilities)
                .at("result")
                .at("inputRequests"),
            requests);
}

// A sampling result may answer with several content blocks, including a tool
// call of its own
// https://modelcontextprotocol.io/specification/2026-07-28/schema#createmessageresult
TEST(validate_continuation_accepts_a_multi_block_sampling_response) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": [
          { "type": "text", "text": "answer" },
          {
            "type": "tool_use",
            "id": "call-1",
            "name": "tool",
            "input": {}
          }
        ]
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_non_object_tool_use_input) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": [
          {
            "type": "tool_use",
            "id": "call-1",
            "name": "tool",
            "input": false
          }
        ]
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

// A descriptor's own metadata must follow the 2026-07-28 key naming rules and
// the tracing formats
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(resources_list_result_rejects_a_boolean_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uri": "file:///r",
                                 "_meta": false
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_a_null_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uri": "file:///r",
                                 "_meta": null
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_an_array_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uri": "file:///r",
                                 "_meta": []
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_a_malformed_key_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uri": "file:///r",
                                 "_meta": { "invalid key": true }
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_a_boolean_traceparent_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uri": "file:///r",
                                 "_meta": { "traceparent": false }
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_a_malformed_traceparent_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uri": "file:///r",
                                 "_meta": { "traceparent": "bad" }
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource descriptor entry");
}

// The grammar protects a receiver reading what someone else sent, so a sender
// gets no such latitude: a value this module builds is held to the format MCP
// requires of whoever emits it, even where a valid traceparent accompanies it
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(decorate_result_rejects_a_malformed_tracestate_it_would_send) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_decorate_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON({
            "_meta": {
              "traceparent":
                  "00-0123456789abcdef0123456789abcdef-0123456789abcdef-01",
              "tracestate": "UPPER=value"
            }
          })JSON")),
                     "Invalid MCP 2026-07-28 metadata");
}

TEST(decorate_result_rejects_a_non_string_tracestate_it_would_send) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_decorate_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON({
            "_meta": {
              "traceparent":
                  "00-0123456789abcdef0123456789abcdef-0123456789abcdef-01",
              "tracestate": false
            }
          })JSON")),
                     "Invalid MCP 2026-07-28 metadata");
}

TEST(resources_list_result_rejects_an_array_tracestate_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uri": "file:///r",
                                 "_meta": { "tracestate": [] }
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource descriptor entry");
}

TEST(resources_list_result_rejects_a_null_baggage_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uri": "file:///r",
                                 "_meta": { "baggage": null }
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource descriptor entry");
}

TEST(resources_templates_list_result_rejects_malformed_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resource_templates_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "name": "r",
                                 "uriTemplate": "file:///{name}",
                                 "_meta": false
                               }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP resource template entry");
}

TEST(resources_read_result_rejects_malformed_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_resources_read_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               {
                                 "uri": "file:///r",
                                 "text": "value",
                                 "_meta": false
                               }
                             ])JSON"),
                         {}),
                     "Invalid MCP resource contents entry");
}

TEST(prompts_list_result_rejects_malformed_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON([
                               { "name": "p", "_meta": false }
                             ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(tools_list_result_rejects_malformed_entry_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tools_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([
    {
      "name": "t",
      "inputSchema": { "type": "object" },
      "_meta": false
    }
  ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP tool descriptor entry");
}

// A tool's input schema is opaque to the protocol, so a property named like a
// metadata field is just a property
// https://modelcontextprotocol.io/specification/2026-07-28/schema#tool
TEST(tools_list_result_accepts_a_metadata_shaped_schema_property) {
  EXPECT_EQ(sourcemeta::core::mcp_make_tools_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON([
    {
      "name": "t",
      "inputSchema": {
        "type": "object",
        "properties": { "_meta": { "const": false } }
      }
    }
  ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "tools": [
      {
        "name": "t",
        "inputSchema": {
          "type": "object",
          "properties": { "_meta": { "const": false } }
        }
      }
    ],
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

// Earlier revisions only require the metadata to be an object, so a key that
// 2026-07-28 rejects still passes there
// https://modelcontextprotocol.io/specification/2025-11-25/basic/index#_meta
TEST(resources_list_result_accepts_a_malformed_key_on_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::parse_json(R"JSON([
    {
      "name": "r",
      "uri": "file:///r",
      "_meta": { "invalid key": true }
    }
  ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "resources": [
      {
        "name": "r",
        "uri": "file:///r",
        "_meta": { "invalid key": true }
      }
    ]
  })JSON"));
}

TEST(resources_list_result_accepts_a_malformed_key_on_2025_06_18) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::parse_json(R"JSON([
    {
      "name": "r",
      "uri": "file:///r",
      "_meta": { "invalid key": true }
    }
  ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "resources": [
      {
        "name": "r",
        "uri": "file:///r",
        "_meta": { "invalid key": true }
      }
    ]
  })JSON"));
}

TEST(resources_list_result_accepts_a_malformed_key_on_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::parse_json(R"JSON([
    {
      "name": "r",
      "uri": "file:///r",
      "_meta": { "invalid key": true }
    }
  ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "resources": [
      {
        "name": "r",
        "uri": "file:///r",
        "_meta": { "invalid key": true }
      }
    ]
  })JSON"));
}

TEST(resources_list_result_accepts_empty_entry_metadata) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(
                    R"JSON([
                {
                  "name": "r",
                  "uri": "file:///r",
                  "_meta": {}
                }
              ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resources": [ { "name": "r", "uri": "file:///r", "_meta": {} } ],
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

// A prefixed vendor key may hold anything, including a nested object that
// happens to use a reserved name
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(resources_list_result_accepts_a_vendor_prefixed_entry_key) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON([
    {
      "name": "r",
      "uri": "file:///r",
      "_meta": { "org.example/data": { "_meta": false } }
    }
  ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resources": [
      {
        "name": "r",
        "uri": "file:///r",
        "_meta": { "org.example/data": { "_meta": false } }
      }
    ],
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(resources_list_result_accepts_well_formed_tracing_metadata) {
  EXPECT_EQ(sourcemeta::core::mcp_make_resources_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON([
    {
      "name": "r",
      "uri": "file:///r",
      "_meta": {
        "traceparent": "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01",
        "tracestate": "vendor=value",
        "baggage": "key=value"
      }
    }
  ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "resources": [
      {
        "name": "r",
        "uri": "file:///r",
        "_meta": {
          "traceparent": "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01",
          "tracestate": "vendor=value",
          "baggage": "key=value"
        }
      }
    ],
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(embedded_resource_metadata) {
  const auto embedded{sourcemeta::core::parse_json(
      R"JSON([
        {
          "type": "resource",
          "resource": {
            "uri": "file:///r",
            "text": "value",
            "_meta": { "invalid key": true }
          }
        }
      ])JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_tool_success(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::JSON{1},
                         sourcemeta::core::JSON::make_object(), embedded),
                     "Invalid MCP content block");
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_get_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "",
                         sourcemeta::core::parse_json(
                             R"JSON([
                {
                  "role": "user",
                  "content": {
                    "type": "text",
                    "text": "value",
                    "_meta": false
                  }
                }
              ])JSON")),
                     "Invalid prompt message");
  const auto business{sourcemeta::core::parse_json(
      R"JSON({
        "_meta": false,
        "nested": { "_meta": { "invalid key": true } }
      })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_tool_success(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{1}, business)
                .at("result")
                .at("structuredContent"),
            business);
}

// A nested request does not carry the outer stateless metadata, and whatever
// metadata it does carry must still be well formed
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(input_required_result_rejects_a_boolean_in_roots_list_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": { "method": "roots/list", "params": { "_meta": false } }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_a_null_in_roots_list_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": { "method": "roots/list", "params": { "_meta": null } }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_an_array_in_roots_list_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": { "method": "roots/list", "params": { "_meta": [] } }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_a_malformed_key_in_roots_list_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "roots/list",
      "params": { "_meta": { "invalid key": true } }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(
    input_required_result_rejects_a_malformed_traceparent_in_roots_list_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "roots/list",
      "params": { "_meta": { "traceparent": "bad" } }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_a_boolean_in_elicitation_create_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": { "type": "object", "properties": {} },
        "_meta": false
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_a_null_in_elicitation_create_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": { "type": "object", "properties": {} },
        "_meta": null
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_an_array_in_elicitation_create_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": { "type": "object", "properties": {} },
        "_meta": []
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(
    input_required_result_rejects_a_malformed_key_in_elicitation_create_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": { "type": "object", "properties": {} },
        "_meta": { "invalid key": true }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(
    input_required_result_rejects_a_malformed_traceparent_in_elicitation_create_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "elicitation/create",
      "params": {
        "message": "choose",
        "requestedSchema": { "type": "object", "properties": {} },
        "_meta": { "traceparent": "bad" }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(
    input_required_result_rejects_a_boolean_in_sampling_create_message_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "user",
            "content": { "type": "text", "text": "hello" }
          }
        ],
        "_meta": false
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(input_required_result_rejects_a_null_in_sampling_create_message_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "user",
            "content": { "type": "text", "text": "hello" }
          }
        ],
        "_meta": null
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(
    input_required_result_rejects_an_array_in_sampling_create_message_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "user",
            "content": { "type": "text", "text": "hello" }
          }
        ],
        "_meta": []
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(
    input_required_result_rejects_a_malformed_key_in_sampling_create_message_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "user",
            "content": { "type": "text", "text": "hello" }
          }
        ],
        "_meta": { "invalid key": true }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(
    input_required_result_rejects_a_malformed_traceparent_in_sampling_create_message_metadata) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "user",
            "content": { "type": "text", "text": "hello" }
          }
        ],
        "_meta": { "traceparent": "bad" }
      }
    }
  })JSON"),
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// A prompt argument carries its own metadata, which follows the same rules as
// everywhere else
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(prompts_list_result_rejects_a_boolean_argument_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([
    {
      "name": "p",
      "arguments": [ { "name": "a", "_meta": false } ]
    }
  ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(prompts_list_result_rejects_a_null_argument_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([
    {
      "name": "p",
      "arguments": [ { "name": "a", "_meta": null } ]
    }
  ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(prompts_list_result_rejects_an_array_argument_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([
    {
      "name": "p",
      "arguments": [ { "name": "a", "_meta": [] } ]
    }
  ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(prompts_list_result_rejects_a_malformed_key_argument_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([
    {
      "name": "p",
      "arguments": [ { "name": "a", "_meta": { "invalid key": true } } ]
    }
  ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(prompts_list_result_rejects_a_malformed_traceparent_argument_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([
    {
      "name": "p",
      "arguments": [ { "name": "a", "_meta": { "traceparent": "bad" } } ]
    }
  ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(write_result_rejects_malformed_prompt_argument_metadata) {
  std::ostringstream stream;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_write_result(
                         stream,
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "prompts/list", sourcemeta::core::JSON{1},
                         sourcemeta::core::parse_json(R"JSON({
    "prompts": [
      {
        "name": "p",
        "arguments": [ { "name": "a", "_meta": false } ]
      }
    ]
  })JSON"),
                         sourcemeta::core::MCPCachePolicy{}),
                     "Invalid precomputed MCP result entry");
  EXPECT_TRUE(stream.str().empty());
}

// Earlier revisions only require the metadata to be an object
// https://modelcontextprotocol.io/specification/2025-11-25/basic/index#_meta
TEST(prompts_list_result_accepts_a_malformed_argument_key_on_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::parse_json(R"JSON([
    {
      "name": "p",
      "arguments": [ { "name": "a", "_meta": { "invalid key": true } } ]
    }
  ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "prompts": [
      {
        "name": "p",
        "arguments": [ { "name": "a", "_meta": { "invalid key": true } } ]
      }
    ]
  })JSON"));
}

TEST(prompts_list_result_accepts_well_formed_argument_metadata) {
  EXPECT_EQ(sourcemeta::core::mcp_make_prompts_list_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON([
    {
      "name": "p",
      "arguments": [
        {
          "name": "a",
          "_meta": { "org.example/data": { "value": 1 } }
        }
      ]
    }
  ])JSON"),
                std::nullopt, {}),
            sourcemeta::core::parse_json(R"JSON({
    "resultType": "complete",
    "prompts": [
      {
        "name": "p",
        "arguments": [
          {
            "name": "a",
            "_meta": { "org.example/data": { "value": 1 } }
          }
        ]
      }
    ],
    "ttlMs": 0,
    "cacheScope": "private"
  })JSON"));
}

TEST(prompts_list_result_rejects_a_non_boolean_required_argument) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_prompts_list_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([
    {
      "name": "p",
      "arguments": [ { "name": "a", "required": 1 } ]
    }
  ])JSON"),
                         std::nullopt, {}),
                     "Invalid MCP prompt descriptor entry");
}

TEST(prompts_list_result_rejects_non_array_arguments) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_prompts_list_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(
              R"JSON([ { "name": "p", "arguments": false } ])JSON"),
          std::nullopt, {}),
      "Invalid MCP prompt descriptor entry");
}

// A tool call is resolved by the immediately following user message, which
// may answer the outstanding calls in any order
// https://modelcontextprotocol.io/specification/2026-07-28/client/sampling
TEST(sampling_history_accepts_tool_calls_answered_in_any_order) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": [
              {
                "type": "tool_use",
                "id": "a",
                "name": "tool",
                "input": {}
              },
              {
                "type": "tool_use",
                "id": "b",
                "name": "tool",
                "input": {}
              }
            ]
          },
          {
            "role": "user",
            "content": [
              {
                "type": "tool_result",
                "toolUseId": "b",
                "content": []
              },
              {
                "type": "tool_result",
                "toolUseId": "a",
                "content": []
              }
            ]
          },
          {
            "role": "assistant",
            "content": { "type": "text", "text": "ok" }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_make_input_required_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "tools/call", sourcemeta::core::JSON{1}, requests, std::nullopt,
                capabilities)
                .at("result")
                .at("resultType"),
            sourcemeta::core::JSON{"input_required"});
}

TEST(sampling_history_requires_the_tool_use_capability) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": [
              {
                "type": "tool_use",
                "id": "a",
                "name": "tool",
                "input": {}
              },
              {
                "type": "tool_use",
                "id": "b",
                "name": "tool",
                "input": {}
              }
            ]
          },
          {
            "role": "user",
            "content": [
              {
                "type": "tool_result",
                "toolUseId": "b",
                "content": []
              },
              {
                "type": "tool_result",
                "toolUseId": "a",
                "content": []
              }
            ]
          },
          {
            "role": "assistant",
            "content": { "type": "text", "text": "ok" }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// Only an assistant message carries a tool call
TEST(sampling_history_rejects_a_tool_call_from_a_user) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "user",
            "content": {
              "type": "tool_use",
              "id": "call",
              "name": "tool",
              "input": {}
            }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(sampling_history_rejects_an_unanswered_tool_call) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": {
              "type": "tool_use",
              "id": "call",
              "name": "tool",
              "input": {}
            }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(sampling_history_rejects_a_tool_result_from_an_assistant) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": {
              "type": "tool_use",
              "id": "call",
              "name": "tool",
              "input": {}
            }
          },
          {
            "role": "assistant",
            "content": {
              "type": "tool_result",
              "toolUseId": "call",
              "content": []
            }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// The answering user message may contain only the corresponding tool
// results
TEST(sampling_history_rejects_a_tool_result_mixed_with_other_content) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": {
              "type": "tool_use",
              "id": "call",
              "name": "tool",
              "input": {}
            }
          },
          {
            "role": "user",
            "content": [
              {
                "type": "tool_result",
                "toolUseId": "call",
                "content": []
              },
              { "type": "text", "text": "mixed" }
            ]
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(sampling_history_rejects_an_unknown_tool_use_identifier) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": {
              "type": "tool_use",
              "id": "call",
              "name": "tool",
              "input": {}
            }
          },
          {
            "role": "user",
            "content": {
              "type": "tool_result",
              "toolUseId": "unknown",
              "content": []
            }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(sampling_history_rejects_duplicate_tool_use_identifiers) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": [
              {
                "type": "tool_use",
                "id": "a",
                "name": "tool",
                "input": {}
              },
              {
                "type": "tool_use",
                "id": "a",
                "name": "tool",
                "input": {}
              }
            ]
          },
          {
            "role": "user",
            "content": [
              {
                "type": "tool_result",
                "toolUseId": "a",
                "content": []
              }
            ]
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// Every block here is well formed on its own, and only the intervening text
// message breaks the immediate-answer requirement
TEST(sampling_history_rejects_an_interrupted_tool_call) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": {
              "type": "tool_use",
              "id": "call",
              "name": "tool",
              "input": {}
            }
          },
          {
            "role": "assistant",
            "content": { "type": "text", "text": "interrupted" }
          },
          {
            "role": "user",
            "content": {
              "type": "tool_result",
              "toolUseId": "call",
              "content": []
            }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(sampling_history_rejects_an_orphan_tool_result) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "user",
            "content": {
              "type": "tool_result",
              "toolUseId": "call",
              "content": []
            }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

TEST(sampling_history_rejects_a_non_object_tool_input) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto requests{sourcemeta::core::parse_json(R"JSON({
    "r": {
      "method": "sampling/createMessage",
      "params": {
        "maxTokens": 1,
        "messages": [
          {
            "role": "assistant",
            "content": {
              "type": "tool_use",
              "id": "call",
              "name": "tool",
              "input": false
            }
          },
          {
            "role": "user",
            "content": {
              "type": "tool_result",
              "toolUseId": "call",
              "content": []
            }
          }
        ]
      }
    }
  })JSON")};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_input_required_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{1}, requests,
                         std::nullopt, capabilities),
                     "Invalid or unsupported nested input requests");
}

// Metadata nested inside a continuation response follows the same key and
// tracing rules as metadata anywhere else
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(validate_continuation_rejects_a_boolean_in_root_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "roots": { "roots": [ { "uri": "file:///r", "_meta": false } ] }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "roots": { "method": "roots/list" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_boolean_in_sampling_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello" },
        "_meta": false
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_boolean_in_content_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello", "_meta": false }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_null_in_root_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "roots": { "roots": [ { "uri": "file:///r", "_meta": null } ] }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "roots": { "method": "roots/list" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_null_in_sampling_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello" },
        "_meta": null
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_null_in_content_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello", "_meta": null }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_an_array_in_root_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "roots": { "roots": [ { "uri": "file:///r", "_meta": [] } ] }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "roots": { "method": "roots/list" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_an_array_in_sampling_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello" },
        "_meta": []
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_an_array_in_content_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello", "_meta": [] }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_malformed_key_in_root_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "roots": {
        "roots": [
          {
            "uri": "file:///r",
            "_meta": { "invalid key": true }
          }
        ]
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "roots": { "method": "roots/list" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_malformed_key_in_sampling_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello" },
        "_meta": { "invalid key": true }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_malformed_key_in_content_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": {
          "type": "text",
          "text": "hello",
          "_meta": { "invalid key": true }
        }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_rejects_a_malformed_traceparent_in_root_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "roots": {
        "roots": [
          {
            "uri": "file:///r",
            "_meta": { "traceparent": "bad" }
          }
        ]
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "roots": { "method": "roots/list" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(
    validate_continuation_rejects_a_malformed_traceparent_in_sampling_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": { "type": "text", "text": "hello" },
        "_meta": { "traceparent": "bad" }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(
    validate_continuation_rejects_a_malformed_traceparent_in_content_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": {
          "type": "text",
          "text": "hello",
          "_meta": { "traceparent": "bad" }
        }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(
      R"JSON({ "sample": { "method": "sampling/createMessage" } })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(validate_continuation_accepts_well_formed_nested_metadata) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": {
      "roots": { "roots": [ { "uri": "file:///r", "_meta": {} } ] },
      "sample": {
        "role": "assistant",
        "model": "model",
        "content": {
          "type": "text",
          "text": "hello",
          "_meta": { "org.example/value": { "_meta": false } }
        }
      }
    }
  })JSON")};
  const auto prior{sourcemeta::core::parse_json(R"JSON({
    "roots": { "method": "roots/list" },
    "sample": { "method": "sampling/createMessage" }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior));
}

TEST(result_writer_requires_cache_policy) {
  constexpr auto *FIXTURE{
      R"JSON({
        "resources": [ { "name": "test", "uri": "https://example.com/a" } ],
        "nextCursor": "",
        "_meta": { "example.org/value": "preserved" }
      })JSON"};
  const auto page{sourcemeta::core::parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{sourcemeta::core::parse_json(FIXTURE)};

  std::ostringstream stream;
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_write_result(
          stream, sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          "resources/list", sourcemeta::core::JSON{0}, page, std::nullopt),
      "Cacheable results require a policy");
}

TEST(result_writer_rejects_non_cacheable_method) {
  constexpr auto *FIXTURE{
      R"JSON({
        "resources": [ { "name": "test", "uri": "https://example.com/a" } ],
        "nextCursor": "",
        "_meta": { "example.org/value": "preserved" }
      })JSON"};
  const auto page{sourcemeta::core::parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{sourcemeta::core::parse_json(FIXTURE)};

  std::ostringstream stream;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_write_result(
                         stream,
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         "tools/call", sourcemeta::core::JSON{0}, page,
                         sourcemeta::core::MCPCachePolicy{}),
                     "Writer requires a cacheable operation");
}

// The initialize parameters are an Object, so a request carrying any other
// JSON value there is as malformed as one carrying none
TEST(make_initialize_result_returns_invalid_request_when_params_is_an_array) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": []
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED)};
  EXPECT_EQ(envelope.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST);
}
