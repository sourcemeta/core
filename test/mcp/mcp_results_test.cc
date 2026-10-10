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
constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED_VERSIONS{
    {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};

using namespace sourcemeta::core;

constexpr std::array<JSON::StringView, 4> WIRES{
    {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
constexpr auto CURRENT{MCPProtocolVersion::V_2026_07_28};

auto parameters() -> JSON {
  return parse_json(
      R"({
  "_meta": {
    "io.modelcontextprotocol/protocolVersion": "2026-07-28",
    "io.modelcontextprotocol/clientCapabilities": {}
  }
})");
}

auto list_result(const JSON::StringView method,
                 const MCPProtocolVersion version, JSON entries) -> JSON {
  if (method == "resources/list") {
    return mcp_make_resources_list_result(version, std::move(entries),
                                          std::nullopt, {});
  }
  if (method == "resources/templates/list") {
    return mcp_make_resource_templates_list_result(version, std::move(entries),
                                                   std::nullopt, {});
  }
  if (method == "resources/read") {
    return mcp_make_resources_read_result(version, std::move(entries), {});
  }
  if (method == "prompts/list") {
    return mcp_make_prompts_list_result(version, std::move(entries),
                                        std::nullopt, {});
  }
  return mcp_make_tools_list_result(version, std::move(entries), std::nullopt,
                                    {});
}

auto sampling_result(const JSON &messages,
                     const MCPClientCapabilities &capabilities) -> JSON {
  auto requests{parse_json(
      R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":1}}})")};
  requests.at("r").at("params").assign("messages", messages);
  return mcp_make_input_required_result(CURRENT, "tools/call", JSON{1},
                                        requests, std::nullopt, capabilities);
}
} // namespace

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

TEST(make_resource_link_unsupported_revision) {
  bool rejected = false;
  try {
    sourcemeta::core::mcp_make_resource_link(
        sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "a");
  } catch (const std::invalid_argument &) {
    rejected = true;
  }
  EXPECT_TRUE(rejected);
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
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      sourcemeta::core::parse_json(R"([ 1, 2, 3 ])"))};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "abc",
    "result": {
      "resultType": "complete",
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
  try {
    (void)sourcemeta::core::mcp_make_tool_success(
        sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
        sourcemeta::core::JSON{nullptr}, sourcemeta::core::JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
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
  try {
    (void)sourcemeta::core::mcp_make_tool_error(
        sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
        sourcemeta::core::JSON{nullptr}, "Failure");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
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

TEST(make_resource_with_priority_positive_infinity) {
  const auto resource{sourcemeta::core::mcp_make_resource(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", {}, std::nullopt,
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
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", {}, std::nullopt,
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
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///a", "Alpha",
      "text/plain", {}, std::nullopt,
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
  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 30000, .scope = sourcemeta::core::MCPCacheScope::Public};
  const auto result{sourcemeta::core::mcp_make_resources_read_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::move(contents),
      policy)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "contents": [
      { "uri": "file:///a", "mimeType": "text/plain", "text": "Hello" }
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
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "contents": []
  })JSON")};
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
      sourcemeta::core::parse_json(R"({ "type": "object" })"))};
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
  "params": {
    "protocolVersion": "2025-11-25",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
  "params": {
    "protocolVersion": "9999-01-01",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
  "params": {
    "protocolVersion": "2025-11-25",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
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
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
  "params": {
    "protocolVersion": "2025-11-25",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS, "Be careful.")};
  EXPECT_EQ(envelope.at("result").at("instructions").to_string(),
            "Be careful.");
}

TEST(make_initialize_result_includes_title_on_2025_06_18) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "protocolVersion": "2025-06-18",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url = "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
  "params": {
    "protocolVersion": "2025-11-25",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url = "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
  "params": {
    "protocolVersion": "2025-03-26",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = "Server",
                                                   .description = "desc",
                                                   .website_url = "https://x"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("title"));
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("description"));
  EXPECT_FALSE(envelope.at("result").at("serverInfo").defines("websiteUrl"));
}

TEST(make_initialize_result_falls_back_to_2025_11_25_on_unknown_version) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "protocolVersion": "9999-01-01",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
      request, capabilities, server, SUPPORTED_VERSIONS)};
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
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
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

  const auto envelope{
      sourcemeta::core::mcp_make_error_missing_required_capability(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
          std::move(required), "Missing required client capability")};

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
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      "Mcp-Method header does not match body method")};

  EXPECT_EQ(envelope.at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(envelope.at("error").at("message").to_string(),
            "Mcp-Method header does not match body method");
}

TEST(error_header_mismatch_unexpected) {
  const auto identifier{sourcemeta::core::JSON{14}};
  const auto envelope{sourcemeta::core::mcp_make_error_header_mismatch(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
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
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
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

TEST(error_header_mismatch_null_and_missing_identifier) {
  const auto null_identifier{sourcemeta::core::JSON{nullptr}};
  const auto envelope_null_identifier{
      sourcemeta::core::mcp_make_error_header_mismatch(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, null_identifier,
          "Mcp-Method", "tools/call", "resources/read")};
  EXPECT_FALSE(envelope_null_identifier.defines("id"));

  const auto envelope_missing_identifier{
      sourcemeta::core::mcp_make_error_header_mismatch(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::nullopt,
          "Mcp-Method", "tools/call", "resources/read")};
  EXPECT_FALSE(envelope_missing_identifier.defines("id"));
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
      "server/discover", std::nullopt, envelope, SUPPORTED_VERSIONS)};
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
      "tools/list", std::nullopt, envelope, SUPPORTED_VERSIONS)};
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
      "tools/call", "calculator", envelope, SUPPORTED_VERSIONS)};
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
        "resources/read", "file:///path/to/resource.txt", envelope,
        SUPPORTED_VERSIONS)};
    EXPECT_FALSE(error.has_value());
  }

  // Base64 encoded header value
  {
    // "file:///path/to/resource.txt" in base64 is
    // "ZmlsZTovLy9wYXRoL3RvL3Jlc291cmNlLnR4dA=="
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "resources/read", "=?base64?ZmlsZTovLy9wYXRoL3RvL3Jlc291cmNlLnR4dA==?=",
        envelope, SUPPORTED_VERSIONS)};
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
      "prompts/get", "summarize", envelope, SUPPORTED_VERSIONS)};
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
        "tools/call", "calculator", envelope, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
    EXPECT_EQ(error->at("id").to_integer(), 42);
  }

  // Missing Mcp-Method
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        std::nullopt, "calculator", envelope, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
    EXPECT_EQ(error->at("id").to_integer(), 42);
  }

  // Missing Mcp-Name on named method
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", std::nullopt, envelope, SUPPORTED_VERSIONS)};
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
        "tools/call", "calculator", envelope_legacy_meta, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
  }

  // Mcp-Method disagrees with JSON-RPC method
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/list", "calculator", envelope, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
  }

  // Mcp-Name disagrees with request body name
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", "other_tool", envelope, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
  }

  // Unsupported protocol header
  {
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "1900-01-01",
        "tools/call", "calculator", envelope, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32020);
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
        "tools/list", "unexpected_name", envelope_tools_list,
        SUPPORTED_VERSIONS)};
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
        legacy_version, std::nullopt, std::nullopt, std::nullopt, envelope,
        SUPPORTED_VERSIONS)};
    EXPECT_FALSE(no_headers.has_value());

    // An empty method is a string. Dispatch handles unsupported method names.
    auto empty_method{envelope};
    empty_method.assign("method", sourcemeta::core::JSON{""});
    EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                     legacy_version, std::nullopt, std::nullopt, std::nullopt,
                     empty_method, SUPPORTED_VERSIONS)
                     .has_value());

    // If matching headers are supplied, validation passes
    const auto matching{sourcemeta::core::mcp_validate_request_headers(
        legacy_version,
        sourcemeta::core::mcp_protocol_version_string(legacy_version),
        "tools/call", "calculator", envelope, SUPPORTED_VERSIONS)};
    EXPECT_FALSE(matching.has_value());

    // Routing headers are undefined and ignored in legacy revisions
    const auto method_mismatch{sourcemeta::core::mcp_validate_request_headers(
        legacy_version, std::nullopt, "tools/list", std::nullopt, envelope,
        SUPPORTED_VERSIONS)};
    EXPECT_FALSE(method_mismatch.has_value());

    // A mismatched name is also ignored
    const auto name_mismatch{sourcemeta::core::mcp_validate_request_headers(
        legacy_version, std::nullopt, std::nullopt, "other_tool", envelope,
        SUPPORTED_VERSIONS)};
    EXPECT_FALSE(name_mismatch.has_value());

    // Unsupported legacy transport version is a bad request
    const auto unsupported{sourcemeta::core::mcp_validate_request_headers(
        legacy_version, "invalid-protocol", std::nullopt, std::nullopt,
        envelope, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(unsupported.has_value());
    EXPECT_EQ(unsupported->at("error").at("code").to_integer(), -32600);
  }
}

TEST(header_validation_safety_cases) {
  // Envelope is not an object
  {
    const auto not_object{sourcemeta::core::JSON{42}};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/list", std::nullopt, not_object, SUPPORTED_VERSIONS)};
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
        "tools/list", std::nullopt, no_method, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32600);
  }

  // params is missing for a named method
  {
    const auto no_parameters{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 1,
      "method": "tools/call"
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", "calculator", no_parameters, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32602);
  }

  // params is not an object for a named method
  {
    const auto parameters_array{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0",
      "id": 1,
      "method": "tools/call",
      "params": [ 1, 2, 3 ]
    })JSON")};
    const auto error{sourcemeta::core::mcp_validate_request_headers(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
        "tools/call", "calculator", parameters_array, SUPPORTED_VERSIONS)};
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
        "tools/call", "calculator", wrong_name_type, SUPPORTED_VERSIONS)};
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
        "tools/list", std::nullopt, missing_meta, SUPPORTED_VERSIONS)};
    EXPECT_TRUE(error.has_value());
    EXPECT_EQ(error->at("error").at("code").to_integer(), -32602);
  }
}

TEST(header_validation_id_handling) {
  // Explicit id: null is unreadable and omitted from the error response
  const auto null_identifier_envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
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

  const auto error_null_identifier{
      sourcemeta::core::mcp_validate_request_headers(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
          "tools/call", "mismatched_tool", null_identifier_envelope,
          SUPPORTED_VERSIONS)};
  EXPECT_TRUE(error_null_identifier.has_value());
  EXPECT_FALSE(error_null_identifier->defines("id"));

  // Notification (missing id) omits id in error response
  const auto notification_envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
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

  const auto error_notification{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "tools/call", "mismatched_tool", notification_envelope,
      SUPPORTED_VERSIONS)};
  EXPECT_TRUE(error_notification.has_value());
  EXPECT_FALSE(error_notification->defines("id"));

  // Invalid id type sets id: null in error response
  const auto invalid_identifier_envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": true,
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

  const auto error_invalid_identifier{
      sourcemeta::core::mcp_validate_request_headers(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
          "tools/call", "mismatched_tool", invalid_identifier_envelope,
          SUPPORTED_VERSIONS)};
  EXPECT_TRUE(error_invalid_identifier.has_value());
  EXPECT_FALSE(error_invalid_identifier->defines("id"));
}

TEST(header_validation_resources_subscribe_unsubscribe) {
  // Legacy resources/subscribe with params.uri
  const auto envelope_subscribe{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "resources/subscribe",
    "params": {
      "uri": "memo://my-channel"
    }
  })JSON")};

  const auto error_subscribe{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::nullopt,
      "resources/subscribe", "memo://my-channel", envelope_subscribe,
      SUPPORTED_VERSIONS)};
  EXPECT_FALSE(error_subscribe.has_value());

  // Legacy resources/unsubscribe with params.uri
  const auto envelope_unsubscribe{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 2,
    "method": "resources/unsubscribe",
    "params": {
      "uri": "memo://my-channel"
    }
  })JSON")};

  const auto error_unsubscribe{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::nullopt,
      "resources/unsubscribe", "memo://my-channel", envelope_unsubscribe,
      SUPPORTED_VERSIONS)};
  EXPECT_FALSE(error_unsubscribe.has_value());

  // Legacy transport ignores the later routing field
  const auto error_subscribe_mismatch{
      sourcemeta::core::mcp_validate_request_headers(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::nullopt,
          "resources/subscribe", "memo://other-channel", envelope_subscribe,
          SUPPORTED_VERSIONS)};
  EXPECT_FALSE(error_subscribe_mismatch.has_value());
}

TEST(header_validation_obs_text_characters) {
  const auto request{sourcemeta::core::parse_json(
      R"({
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": {
    "name": "café",
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  }
})")};
  const auto raw{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "tools/call", "café", request, SUPPORTED_VERSIONS)};
  EXPECT_TRUE(raw.has_value());
  EXPECT_EQ(raw->at("error").at("code").to_integer(), -32020);
  const auto encoded{sourcemeta::core::mcp_encode_header_value("café")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "tools/call", encoded, request,
                   SUPPORTED_VERSIONS)
                   .has_value());
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

TEST(error_builders_preserve_null_and_omit_missing_identifier) {
  const auto null_identifier{sourcemeta::core::JSON{nullptr}};

  const auto unsupported_null{
      sourcemeta::core::mcp_make_error_unsupported_protocol_version(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, null_identifier,
          "1900-01-01",
          std::array<sourcemeta::core::JSON::StringView, 1>{{"2026-07-28"}})};
  EXPECT_FALSE(unsupported_null.defines("id"));

  const auto unsupported_missing{
      sourcemeta::core::mcp_make_error_unsupported_protocol_version(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::nullopt,
          "1900-01-01",
          std::array<sourcemeta::core::JSON::StringView, 1>{{"2026-07-28"}})};
  EXPECT_FALSE(unsupported_missing.defines("id"));

  const auto not_found_null{sourcemeta::core::mcp_make_error_resource_not_found(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, null_identifier)};
  EXPECT_FALSE(not_found_null.defines("id"));

  const auto not_found_missing{
      sourcemeta::core::mcp_make_error_resource_not_found(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::nullopt)};
  EXPECT_FALSE(not_found_missing.defines("id"));

  auto required_capabilities{sourcemeta::core::JSON::make_object()};
  const auto missing_capability_null{
      sourcemeta::core::mcp_make_error_missing_required_capability(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, null_identifier,
          required_capabilities, "Missing capability")};
  EXPECT_FALSE(missing_capability_null.defines("id"));

  const auto missing_capability_missing{
      sourcemeta::core::mcp_make_error_missing_required_capability(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::nullopt,
          required_capabilities, "Missing capability")};
  EXPECT_FALSE(missing_capability_missing.defines("id"));
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
  auto result_legacy{sourcemeta::core::JSON::make_object()};
  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 60000, .scope = sourcemeta::core::MCPCacheScope::Private};
  const auto decorated_legacy{sourcemeta::core::mcp_decorate_cacheable_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      std::move(result_legacy), policy,
      sourcemeta::core::MCP_METHOD_TOOLS_LIST)};
  EXPECT_FALSE(decorated_legacy.defines("ttlMs"));
  EXPECT_FALSE(decorated_legacy.defines("cacheScope"));

  auto result_modern{sourcemeta::core::JSON::make_object()};
  const auto decorated_modern{sourcemeta::core::mcp_decorate_cacheable_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      std::move(result_modern), policy,
      sourcemeta::core::MCP_METHOD_TOOLS_LIST)};
  EXPECT_EQ(decorated_modern.at("ttlMs").to_integer(), 60000);
  EXPECT_EQ(decorated_modern.at("cacheScope").to_string(), "private");
}

TEST(tool_success_modern_result_type_and_structured_content) {
  const auto identifier{sourcemeta::core::JSON{100}};

  // Structured content with object
  auto structured_object{sourcemeta::core::JSON::make_object()};
  structured_object.assign("foo", sourcemeta::core::JSON{123});
  const auto envelope_object{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      std::move(structured_object))};
  EXPECT_EQ(envelope_object.at("result").at("resultType").to_string(),
            "complete");
  EXPECT_EQ(envelope_object.at("result").at("isError").to_boolean(), false);
  EXPECT_EQ(envelope_object.at("result")
                .at("structuredContent")
                .at("foo")
                .to_integer(),
            123);

  // Structured content with array
  auto structured_array{sourcemeta::core::JSON::make_array()};
  structured_array.push_back(sourcemeta::core::JSON{"item"});
  const auto envelope_array{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      std::move(structured_array))};
  EXPECT_EQ(envelope_array.at("result").at("resultType").to_string(),
            "complete");
  EXPECT_TRUE(envelope_array.at("result").at("structuredContent").is_array());

  // Structured content with scalar string
  const auto envelope_string{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      sourcemeta::core::JSON{"hello scalar"})};
  EXPECT_EQ(envelope_string.at("result").at("resultType").to_string(),
            "complete");
  EXPECT_EQ(envelope_string.at("result").at("structuredContent").to_string(),
            "hello scalar");

  // Structured content with null
  const auto envelope_null{sourcemeta::core::mcp_make_tool_success(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      sourcemeta::core::JSON{nullptr})};
  EXPECT_EQ(envelope_null.at("result").at("resultType").to_string(),
            "complete");
  EXPECT_TRUE(envelope_null.at("result").at("structuredContent").is_null());
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
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, std::move(contents),
      policy)};
  EXPECT_FALSE(legacy_result.defines("resultType"));
  EXPECT_FALSE(legacy_result.defines("ttlMs"));
  EXPECT_FALSE(legacy_result.defines("cacheScope"));
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

TEST(initialize_negotiation_with_modern_request) {
  // If an older client calls initialize with protocolVersion = "2026-07-28",
  // this handshake builder selects a revision from its explicit supported set
  // that still implements initialize (2025-11-25 in this fixture).
  const auto request{sourcemeta::core::parse_json(R"JSON({
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "protocolVersion": "2026-07-28",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};

  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};

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
  "params": {
    "protocolVersion": "3000-01-01",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};

  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};

  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
}

TEST(mrtr_input_required_result) {
  const auto identifier{sourcemeta::core::JSON{50}};
  auto input_requests{sourcemeta::core::JSON::make_object()};
  auto input_request{sourcemeta::core::JSON::make_object()};
  input_request.assign("method", sourcemeta::core::JSON{"elicitation/create"});
  input_request.assign(
      "params",
      sourcemeta::core::parse_json(
          R"({"message":"Confirm delete?","requestedSchema":{"type":"object","properties":{}}})"));
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
      "inputResponses": {
        "confirm": { "value": true }
      }
    }
  })JSON")};

  EXPECT_EQ(
      sourcemeta::core::mcp_request_state(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, continuation),
      "token-xyz");
  const auto *responses{sourcemeta::core::mcp_request_input_responses(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, continuation)};
  EXPECT_NE(responses, nullptr);
  EXPECT_TRUE(responses->is_object());
  EXPECT_EQ(responses->at("confirm").at("value").to_boolean(), true);
}

TEST(client_capabilities_parsing_and_serialization) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.roots_list_changed = true;
  capabilities.sampling = true;
  capabilities.sampling_context = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  capabilities.elicitation_url = true;

  auto extensions{sourcemeta::core::JSON::make_object()};
  extensions.assign("com.example/customCapability",
                    sourcemeta::core::JSON::make_object());
  capabilities.extensions = std::move(extensions);

  const auto serialized_legacy{
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities)};
  EXPECT_TRUE(serialized_legacy.at("roots").at("listChanged").to_boolean());
  EXPECT_TRUE(serialized_legacy.defines("sampling"));
  EXPECT_TRUE(serialized_legacy.at("sampling").defines("context"));
  EXPECT_TRUE(serialized_legacy.at("sampling").defines("tools"));
  EXPECT_TRUE(serialized_legacy.defines("elicitation"));
  EXPECT_TRUE(serialized_legacy.at("elicitation").defines("form"));
  EXPECT_TRUE(serialized_legacy.at("elicitation").defines("url"));
  EXPECT_FALSE(serialized_legacy.defines("extensions"));

  const auto serialized_modern{
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities)};
  EXPECT_FALSE(serialized_modern.at("roots").defines("listChanged"));
  EXPECT_TRUE(serialized_modern.defines("sampling"));
  EXPECT_TRUE(serialized_modern.at("sampling").defines("context"));
  EXPECT_TRUE(serialized_modern.at("sampling").defines("tools"));
  EXPECT_TRUE(serialized_modern.defines("elicitation"));
  EXPECT_TRUE(serialized_modern.at("elicitation").defines("form"));
  EXPECT_TRUE(serialized_modern.at("elicitation").defines("url"));
  EXPECT_TRUE(serialized_modern.at("extensions")
                  .at("com.example/customCapability")
                  .is_object());

  const auto parsed_legacy{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, serialized_legacy)};
  EXPECT_TRUE(parsed_legacy.roots);
  EXPECT_TRUE(parsed_legacy.roots_list_changed);
  EXPECT_TRUE(parsed_legacy.sampling);
  EXPECT_TRUE(parsed_legacy.sampling_context);
  EXPECT_TRUE(parsed_legacy.sampling_tools);
  EXPECT_TRUE(parsed_legacy.elicitation);
  EXPECT_TRUE(parsed_legacy.elicitation_form);
  EXPECT_TRUE(parsed_legacy.elicitation_url);
  EXPECT_FALSE(parsed_legacy.extensions.has_value());

  const auto parsed_modern{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, serialized_modern)};
  EXPECT_TRUE(parsed_modern.roots);
  EXPECT_FALSE(parsed_modern.roots_list_changed);
  EXPECT_TRUE(parsed_modern.extensions.has_value());
  EXPECT_TRUE(
      parsed_modern.extensions->at("com.example/customCapability").is_object());
}

TEST(header_validation_and_mismatch_error) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
  "jsonrpc": "2.0",
  "id": 42,
  "method": "tools/call",
  "params": {
    "name": "my-tool",
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  }
})JSON")};

  const auto version{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};

  // Matching headers
  const auto ok_result{sourcemeta::core::mcp_validate_request_headers(
      version, "2026-07-28", "tools/call", "my-tool", request,
      SUPPORTED_VERSIONS)};
  EXPECT_FALSE(ok_result.has_value());

  // Method mismatch
  const auto method_error{sourcemeta::core::mcp_validate_request_headers(
      version, "2026-07-28", "tools/list", "my-tool", request,
      SUPPORTED_VERSIONS)};
  EXPECT_TRUE(method_error.has_value());
  EXPECT_EQ(method_error->at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(method_error->at("error").at("data").at("header").to_string(),
            "Mcp-Method");
  EXPECT_EQ(method_error->at("error").at("data").at("headerValue").to_string(),
            "tools/list");
  EXPECT_EQ(method_error->at("error").at("data").at("bodyValue").to_string(),
            "tools/call");

  // Name mismatch
  const auto name_error{sourcemeta::core::mcp_validate_request_headers(
      version, "2026-07-28", "tools/call", "other-tool", request,
      SUPPORTED_VERSIONS)};
  EXPECT_TRUE(name_error.has_value());
  EXPECT_EQ(name_error->at("error").at("code").to_integer(), -32020);
  EXPECT_EQ(name_error->at("error").at("data").at("header").to_string(),
            "Mcp-Name");
  EXPECT_EQ(name_error->at("error").at("data").at("headerValue").to_string(),
            "other-tool");
  EXPECT_EQ(name_error->at("error").at("data").at("bodyValue").to_string(),
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
      sourcemeta::core::JSON::make_array(), "cursor-no-policy", policy)};
  EXPECT_FALSE(legacy_no_policy.defines("resultType"));
  EXPECT_EQ(legacy_no_policy.at("nextCursor").to_string(), "cursor-no-policy");
}

TEST(resources_list_result_modern_and_legacy) {
  auto resources{sourcemeta::core::JSON::make_array()};
  resources.push_back(sourcemeta::core::mcp_make_resource(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///doc.txt",
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
      sourcemeta::core::JSON::make_array(), "cursor-no-policy", policy)};
  EXPECT_FALSE(legacy_no_policy.defines("resultType"));
  EXPECT_EQ(legacy_no_policy.at("nextCursor").to_string(), "cursor-no-policy");
}

TEST(resource_templates_list_result_modern_and_legacy) {
  auto templates{sourcemeta::core::JSON::make_array()};
  templates.push_back(sourcemeta::core::mcp_make_resource_template(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26, "file:///{path}",
      "Files", "File access", "text/plain"));

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
          std::move(templates), "cursor-tmpl", tmpl_policy)};
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
      sourcemeta::core::JSON::make_array(), "", policy)};
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
      sourcemeta::core::JSON::make_array(), "", policy)};
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
          sourcemeta::core::JSON::make_array(), "", policy)};
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
      sourcemeta::core::JSON::make_array(), "", policy)};
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
  "params": {
    "protocolVersion": "2024-11-05",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})JSON")};

  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0"};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};

  EXPECT_EQ(envelope.at("result").at("protocolVersion").to_string(),
            "2025-11-25");
  EXPECT_FALSE(envelope.at("result").defines("resultType"));
  EXPECT_FALSE(envelope.at("result").defines("_meta"));
}

TEST(error_code_to_http_status_mapping) {
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSONRPC_CODE_PARSE),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND),
            sourcemeta::core::HTTP_STATUS_NOT_FOUND);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSONRPC_CODE_INVALID_PARAMS),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSONRPC_CODE_INTERNAL),
            sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::MCP_CODE_HEADER_MISMATCH),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, -99999),
            sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR);
}

TEST(error_code_to_http_status_mapping_version_aware) {
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND),
            sourcemeta::core::HTTP_STATUS_NOT_FOUND);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
  EXPECT_EQ(sourcemeta::core::mcp_error_code_to_http_status(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                sourcemeta::core::JSONRPC_CODE_METHOD_NOT_FOUND),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
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

TEST(value_result_decoration) {
  auto legacy_payload{sourcemeta::core::JSON::make_object()};
  legacy_payload.assign("key", sourcemeta::core::JSON{"value"});
  legacy_payload = sourcemeta::core::mcp_decorate_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      std::move(legacy_payload));
  EXPECT_FALSE(legacy_payload.defines("resultType"));
  EXPECT_FALSE(legacy_payload.defines("_meta"));
  EXPECT_EQ(legacy_payload.at("key").to_string(), "value");

  auto modern_payload{sourcemeta::core::JSON::make_object()};
  modern_payload.assign("key", sourcemeta::core::JSON{"value"});
  modern_payload = sourcemeta::core::mcp_decorate_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      std::move(modern_payload));
  EXPECT_EQ(modern_payload.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern_payload.at("key").to_string(), "value");

  // In-place cacheable decoration
  auto modern_cacheable{sourcemeta::core::JSON::make_object()};
  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 5000, .scope = sourcemeta::core::MCPCacheScope::Private};
  modern_cacheable = sourcemeta::core::mcp_decorate_cacheable_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      std::move(modern_cacheable), policy,
      sourcemeta::core::MCP_METHOD_TOOLS_LIST);
  EXPECT_EQ(modern_cacheable.at("ttlMs").to_integer(), 5000);
  EXPECT_EQ(modern_cacheable.at("cacheScope").to_string(), "private");
}

TEST(prompts_list_result_builder) {
  auto prompts{sourcemeta::core::JSON::make_array()};
  auto prompt1{sourcemeta::core::JSON::make_object()};
  prompt1.assign("name", sourcemeta::core::JSON{"test-prompt"});
  prompts.push_back(std::move(prompt1));

  const sourcemeta::core::MCPCachePolicy policy{
      .ttl_ms = 10000, .scope = sourcemeta::core::MCPCacheScope::Public};

  const auto legacy{sourcemeta::core::mcp_make_prompts_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25, prompts,
      "next-cursor", policy)};
  EXPECT_FALSE(legacy.defines("resultType"));
  EXPECT_FALSE(legacy.defines("_meta"));
  EXPECT_EQ(legacy.at("prompts").size(), 1);
  EXPECT_EQ(legacy.at("nextCursor").to_string(), "next-cursor");

  const auto modern{sourcemeta::core::mcp_make_prompts_list_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, prompts, std::nullopt,
      policy)};
  EXPECT_EQ(modern.at("resultType").to_string(), "complete");
  EXPECT_EQ(modern.at("prompts").size(), 1);
  EXPECT_FALSE(modern.defines("nextCursor"));
  EXPECT_EQ(modern.at("ttlMs").to_integer(), 10000);
  EXPECT_EQ(modern.at("cacheScope").to_string(), "public");
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
      "tools/call", "base64:%%%invalid-chars&&&", request, SUPPORTED_VERSIONS)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error->at("error").at("code").to_integer(),
            sourcemeta::core::MCP_CODE_HEADER_MISMATCH);
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

TEST(decorate_result_rejects_non_object_meta) {
  auto result{sourcemeta::core::parse_json(
      R"({"resultType":"complete","_meta":"invalid"})")};
  const auto before{result};
  try {
    result = sourcemeta::core::mcp_decorate_result(
        sourcemeta::core::MCPProtocolVersion::V_2026_07_28, result);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(result, before);
}

TEST(cache_policy_clamps_negative_ttl_ms_to_zero) {
  auto result{sourcemeta::core::JSON::make_object()};
  const sourcemeta::core::MCPCachePolicy negative_policy{
      .ttl_ms = -500, .scope = sourcemeta::core::MCPCacheScope::Public};
  result = sourcemeta::core::mcp_decorate_cacheable_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, std::move(result),
      negative_policy, sourcemeta::core::MCP_METHOD_TOOLS_LIST);

  EXPECT_TRUE(result.defines("ttlMs"));
  EXPECT_EQ(result.at("ttlMs").to_integer(), 0);
  EXPECT_EQ(result.at("cacheScope").to_string(), "public");
}

TEST(result_type_defaults_to_complete_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  EXPECT_EQ(mcp_resolve_result_type(version, JSON::make_object()).value(),
            "complete");
}

TEST(result_type_rejects_unnegotiated_extension_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  EXPECT_FALSE(mcp_resolve_result_type(version, custom).has_value());
}

TEST(result_type_negotiated_extension_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  const std::array<JSON::StringView, 1> extensions{{"org.example/custom"}};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  EXPECT_EQ(mcp_resolve_result_type(version, custom, extensions).has_value(),
            false);
}

TEST(result_type_input_required_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  const auto interim{parse_json(R"({"resultType":"input_required"})")};
  EXPECT_EQ(mcp_resolve_result_type(version, interim).has_value(), false);
}

TEST(result_type_rejects_boolean_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  EXPECT_FALSE(mcp_resolve_result_type(version, JSON{false}).has_value());
}

TEST(result_type_defaults_to_complete_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  EXPECT_EQ(mcp_resolve_result_type(version, JSON::make_object()).value(),
            "complete");
}

TEST(result_type_rejects_unnegotiated_extension_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  EXPECT_FALSE(mcp_resolve_result_type(version, custom).has_value());
}

TEST(result_type_negotiated_extension_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  const std::array<JSON::StringView, 1> extensions{{"org.example/custom"}};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  EXPECT_EQ(mcp_resolve_result_type(version, custom, extensions).has_value(),
            false);
}

TEST(result_type_input_required_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  const auto interim{parse_json(R"({"resultType":"input_required"})")};
  EXPECT_EQ(mcp_resolve_result_type(version, interim).has_value(), false);
}

TEST(result_type_rejects_boolean_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  EXPECT_FALSE(mcp_resolve_result_type(version, JSON{false}).has_value());
}

TEST(result_type_defaults_to_complete_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  EXPECT_EQ(mcp_resolve_result_type(version, JSON::make_object()).value(),
            "complete");
}

TEST(result_type_rejects_unnegotiated_extension_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  EXPECT_FALSE(mcp_resolve_result_type(version, custom).has_value());
}

TEST(result_type_negotiated_extension_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  const std::array<JSON::StringView, 1> extensions{{"org.example/custom"}};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  EXPECT_EQ(mcp_resolve_result_type(version, custom, extensions).has_value(),
            false);
}

TEST(result_type_input_required_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  const auto interim{parse_json(R"({"resultType":"input_required"})")};
  EXPECT_EQ(mcp_resolve_result_type(version, interim).has_value(), false);
}

TEST(result_type_rejects_boolean_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  EXPECT_FALSE(mcp_resolve_result_type(version, JSON{false}).has_value());
}

TEST(result_type_defaults_to_complete_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  EXPECT_EQ(mcp_resolve_result_type(version, JSON::make_object()).value(),
            "complete");
}

TEST(result_type_rejects_unnegotiated_extension_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  EXPECT_FALSE(mcp_resolve_result_type(version, custom).has_value());
}

TEST(result_type_negotiated_extension_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  const std::array<JSON::StringView, 1> extensions{{"org.example/custom"}};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  EXPECT_EQ(mcp_resolve_result_type(version, custom, extensions).has_value(),
            true);
}

TEST(result_type_input_required_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  const auto interim{parse_json(R"({"resultType":"input_required"})")};
  EXPECT_EQ(mcp_resolve_result_type(version, interim).has_value(), true);
}

TEST(result_type_rejects_boolean_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(mcp_resolve_result_type(version, JSON{false}).has_value());
}

TEST(empty_result_rejects_null_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_empty_result(version, JSON{nullptr});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_null_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_tool_error(version, JSON{nullptr}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_boolean_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_empty_result(version, JSON{true});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_boolean_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_tool_error(version, JSON{true}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_real_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_empty_result(version, JSON{1.5});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_real_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_tool_error(version, JSON{1.5}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_array_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_empty_result(version, JSON::make_array());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_array_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_tool_error(version, JSON::make_array(), "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_object_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_empty_result(version, JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_object_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  try {
    mcp_make_tool_error(version, JSON::make_object(), "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_accepts_zero_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  EXPECT_EQ(mcp_make_empty_result(version, JSON{0}).at("id"), JSON{0});
}

TEST(empty_result_accepts_empty_string_id_2025_03_26) {
  const auto version{MCPProtocolVersion::V_2025_03_26};
  EXPECT_EQ(mcp_make_empty_result(version, JSON{""}).at("id"), JSON{""});
}

TEST(empty_result_rejects_null_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_empty_result(version, JSON{nullptr});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_null_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_tool_error(version, JSON{nullptr}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_boolean_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_empty_result(version, JSON{true});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_boolean_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_tool_error(version, JSON{true}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_real_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_empty_result(version, JSON{1.5});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_real_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_tool_error(version, JSON{1.5}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_array_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_empty_result(version, JSON::make_array());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_array_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_tool_error(version, JSON::make_array(), "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_object_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_empty_result(version, JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_object_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  try {
    mcp_make_tool_error(version, JSON::make_object(), "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_accepts_zero_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  EXPECT_EQ(mcp_make_empty_result(version, JSON{0}).at("id"), JSON{0});
}

TEST(empty_result_accepts_empty_string_id_2025_06_18) {
  const auto version{MCPProtocolVersion::V_2025_06_18};
  EXPECT_EQ(mcp_make_empty_result(version, JSON{""}).at("id"), JSON{""});
}

TEST(empty_result_rejects_null_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_empty_result(version, JSON{nullptr});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_null_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_tool_error(version, JSON{nullptr}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_boolean_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_empty_result(version, JSON{true});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_boolean_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_tool_error(version, JSON{true}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_real_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_empty_result(version, JSON{1.5});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_real_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_tool_error(version, JSON{1.5}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_array_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_empty_result(version, JSON::make_array());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_array_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_tool_error(version, JSON::make_array(), "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_object_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_empty_result(version, JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_object_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  try {
    mcp_make_tool_error(version, JSON::make_object(), "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_accepts_zero_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  EXPECT_EQ(mcp_make_empty_result(version, JSON{0}).at("id"), JSON{0});
}

TEST(empty_result_accepts_empty_string_id_2025_11_25) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  EXPECT_EQ(mcp_make_empty_result(version, JSON{""}).at("id"), JSON{""});
}

TEST(empty_result_rejects_null_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_empty_result(version, JSON{nullptr});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_null_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_tool_error(version, JSON{nullptr}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_boolean_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_empty_result(version, JSON{true});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_boolean_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_tool_error(version, JSON{true}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_real_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_empty_result(version, JSON{1.5});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_real_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_tool_error(version, JSON{1.5}, "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_array_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_empty_result(version, JSON::make_array());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_array_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_tool_error(version, JSON::make_array(), "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_rejects_object_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_empty_result(version, JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(tool_error_rejects_object_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  try {
    mcp_make_tool_error(version, JSON::make_object(), "error");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(empty_result_accepts_zero_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  EXPECT_EQ(mcp_make_empty_result(version, JSON{0}).at("id"), JSON{0});
}

TEST(empty_result_accepts_empty_string_id_2026_07_28) {
  const auto version{MCPProtocolVersion::V_2026_07_28};
  EXPECT_EQ(mcp_make_empty_result(version, JSON{""}).at("id"), JSON{""});
}

TEST(initialize_supported_subset) {
  const std::array<JSON::StringView, 1> only{{"2025-03-26"}};
  const auto init{parse_json(
      R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "initialize",
  "params": {
    "protocolVersion": "unknown",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})")};
  const auto result{mcp_make_initialize_result(
      init, {}, {.name = "server", .version = "1"}, only)};
  EXPECT_EQ(result.at("result").at("protocolVersion"), JSON{"2025-03-26"});
}

TEST(initialize_missing_client_info) {
  const std::array<JSON::StringView, 1> only{{"2025-03-26"}};
  const auto init{parse_json(
      R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "initialize",
  "params": {
    "protocolVersion": "unknown",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})")};
  auto invalid{init};
  invalid.at("params").erase("clientInfo");
  EXPECT_EQ(mcp_make_initialize_result(invalid, {}, {"server", "1"}, only)
                .at("error")
                .at("code"),
            JSON{-32602});
}

TEST(initialize_invalid_identifier) {
  const std::array<JSON::StringView, 1> only{{"2025-03-26"}};
  const auto init{parse_json(
      R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "initialize",
  "params": {
    "protocolVersion": "unknown",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})")};

  auto invalid{init};
  invalid.at("params").erase("clientInfo");
  invalid.assign("id", JSON{});
  try {
    mcp_make_initialize_result(invalid, {}, {.name = "server", .version = "1"},
                               only);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(initialize_rejects_modern_only_support) {
  const auto init{parse_json(
      R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "initialize",
  "params": {
    "protocolVersion": "unknown",
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})")};
  const std::array<JSON::StringView, 1> modern_only{{"2026-07-28"}};
  try {
    mcp_make_initialize_result(init, {}, {.name = "server", .version = "1"},
                               modern_only);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(result_types_and_cache_scope) {
  EXPECT_EQ(mcp_result_type(JSON::make_object()), "complete");
  EXPECT_EQ(
      mcp_result_type(parse_json(R"({"resultType":"example/extension"})")),
      "example/extension");
  EXPECT_FALSE(mcp_result_type(JSON{nullptr}).has_value());
  EXPECT_FALSE(
      mcp_result_type(parse_json(R"({"resultType":false})")).has_value());
  for (const auto *const text :
       {R"({"resultType":"input_required","requestState":"state"})",
        R"({"resultType":"unknown"})"}) {
    try {
      mcp_decorate_result(CURRENT, parse_json(text));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_decorate_cacheable_result(CURRENT, parse_json(text), {},
                                    "resources/read");
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  try {
    mcp_decorate_cacheable_result(CURRENT, JSON::make_object(), {},
                                  "tools/call");
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(mcp_decorate_cacheable_result(CURRENT, JSON::make_object(),
                                          {-4, MCPCacheScope::Private},
                                          "resources/read")
                .at("ttlMs"),
            JSON{0});
  const auto invalid_info{parse_json(
      R"({"_meta":{"io.modelcontextprotocol/serverInfo":{"name":"server"}}})")};
  try {
    mcp_decorate_result(CURRENT, invalid_info);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(content_and_schemas) {
  {
    const auto version{MCPProtocolVersion::V_2025_03_26};

    try {
      mcp_make_tool_descriptor(version, "tool", "",
                               parse_json(R"({"type":"string"})"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_resources_list_result(version, parse_json(R"([{"uri":"x"}])"),
                                     std::nullopt, {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_prompts_list_result(
          version,
          parse_json(R"([{"name":"prompt","arguments":[{"name":3}]}])"),
          std::nullopt, {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_resources_read_result(
          version, parse_json(R"([{"uri":"x","blob":false}])"), {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_tool_success(version, JSON{0}, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":false}])"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_prompts_get_result(
          version, "",
          parse_json(
              R"([{"role":"system","content":{"type":"text","text":"hi"}}])"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2025_06_18};

    try {
      mcp_make_tool_descriptor(version, "tool", "",
                               parse_json(R"({"type":"string"})"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_resources_list_result(version, parse_json(R"([{"uri":"x"}])"),
                                     std::nullopt, {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_prompts_list_result(
          version,
          parse_json(R"([{"name":"prompt","arguments":[{"name":3}]}])"),
          std::nullopt, {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_resources_read_result(
          version, parse_json(R"([{"uri":"x","blob":false}])"), {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_tool_success(version, JSON{0}, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":false}])"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_prompts_get_result(
          version, "",
          parse_json(
              R"([{"role":"system","content":{"type":"text","text":"hi"}}])"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2025_11_25};

    try {
      mcp_make_tool_descriptor(version, "tool", "",
                               parse_json(R"({"type":"string"})"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_resources_list_result(version, parse_json(R"([{"uri":"x"}])"),
                                     std::nullopt, {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_prompts_list_result(
          version,
          parse_json(R"([{"name":"prompt","arguments":[{"name":3}]}])"),
          std::nullopt, {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_resources_read_result(
          version, parse_json(R"([{"uri":"x","blob":false}])"), {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_tool_success(version, JSON{0}, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":false}])"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_prompts_get_result(
          version, "",
          parse_json(
              R"([{"role":"system","content":{"type":"text","text":"hi"}}])"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2026_07_28};

    try {
      mcp_make_tool_descriptor(version, "tool", "",
                               parse_json(R"({"type":"string"})"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_resources_list_result(version, parse_json(R"([{"uri":"x"}])"),
                                     std::nullopt, {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_prompts_list_result(
          version,
          parse_json(R"([{"name":"prompt","arguments":[{"name":3}]}])"),
          std::nullopt, {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_resources_read_result(
          version, parse_json(R"([{"uri":"x","blob":false}])"), {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_tool_success(version, JSON{0}, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":false}])"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_prompts_get_result(
          version, "",
          parse_json(
              R"([{"role":"system","content":{"type":"text","text":"hi"}}])"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  for (const auto version :
       {MCPProtocolVersion::V_2025_06_18, MCPProtocolVersion::V_2025_11_25}) {
    try {
      mcp_make_tool_success(version, JSON{0}, JSON::make_array());
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    try {
      mcp_make_tool_descriptor(version, "tool", "",
                               parse_json(R"({"type":"object"})"),
                               parse_json(R"({"type":"string"})"));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  for (const auto &value :
       {JSON{nullptr}, JSON{true}, JSON{5}, JSON{"text"}, JSON::make_array()}) {
    EXPECT_EQ(mcp_make_tool_success(CURRENT, JSON{0}, value)
                  .at("result")
                  .at("structuredContent"),
              value);
  }
  auto many{JSON::make_array()};
  for (std::size_t index = 0; index < 101; ++index) {
    many.push_back(JSON{"value"});
  }
  try {
    mcp_make_completion_result(CURRENT, many, std::nullopt, std::nullopt);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_completion_result(CURRENT, parse_json(R"(["value"] )"), 0, false);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  for (const auto version :
       {MCPProtocolVersion::V_2025_03_26, MCPProtocolVersion::V_2025_06_18,
        MCPProtocolVersion::V_2025_11_25, MCPProtocolVersion::V_2026_07_28}) {
    try {
      mcp_make_completion_result(version, JSON::make_array(), -1, false);
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    EXPECT_EQ(mcp_make_completion_result(version, JSON::make_array(), 0, false)
                  .at("completion")
                  .at("total"),
              JSON{0});
    EXPECT_EQ(mcp_make_completion_result(version, parse_json(R"(["value"])"), 1,
                                         false)
                  .at("completion")
                  .at("total"),
              JSON{1});
    constexpr auto MAXIMUM{std::numeric_limits<std::int64_t>::max()};
    EXPECT_EQ(mcp_make_completion_result(version, parse_json(R"(["value"])"),
                                         MAXIMUM, false)
                  .at("completion")
                  .at("total"),
              JSON{MAXIMUM});
  }
}

TEST(mrtr_nested_requests) {
  MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  for (const auto *const text : {R"({"r":{"method":"roots/list"}})",
                                 R"({
  "r": {
    "method": "elicitation/create",
    "params": {
      "mode": "url",
      "message": "visit",
      "url": "https://example.com"
    }
  }
})",
                                 R"({
  "r": {
    "method": "elicitation/create",
    "params": {
      "message": "choose",
      "requestedSchema": {
        "type": "object",
        "properties": {
          "choice": {
            "type": "array",
            "items": {
              "type": "string",
              "enum": [
                "a"
              ]
            }
          }
        }
      }
    }
  }
})",
                                 R"({
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
            "content": [
              {
                "type": "text",
                "text": "ok"
              }
            ]
          }
        }
      ],
      "tools": [
        {
          "name": "tool",
          "inputSchema": {
            "type": "object"
          }
        }
      ],
      "toolChoice": {
        "mode": "auto"
      }
    }
  }
})"}) {
    const auto result{mcp_make_input_required_result(
        CURRENT, "tools/call", JSON{0}, parse_json(text), std::nullopt,
        capabilities)};
    EXPECT_TRUE(result.at("result").is_object());
  }
  for (const auto *const text :
       {R"({"r":{"method":"roots/list","params":false}})",
        R"({
  "r": {
    "method": "elicitation/create",
    "params": {
      "message": "choose",
      "requestedSchema": {
        "type": "array"
      }
    }
  }
})",
        R"({
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
})",
        R"({
  "r": {
    "method": "sampling/createMessage",
    "params": {
      "maxTokens": 10,
      "messages": [],
      "temperature": false
    }
  }
})",
        R"({
  "r": {
    "method": "sampling/createMessage",
    "params": {
      "maxTokens": 10,
      "messages": [],
      "modelPreferences": {
        "costPriority": 2
      }
    }
  }
})",
        R"({
  "r": {
    "method": "sampling/createMessage",
    "params": {
      "maxTokens": 10,
      "messages": [],
      "toolChoice": {
        "mode": "bad"
      }
    }
  }
})"}) {
    try {
      mcp_make_input_required_result(CURRENT, "tools/call", JSON{0},
                                     parse_json(text), std::nullopt,
                                     capabilities);
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  try {
    mcp_make_input_required_result(CURRENT, "tools/list", JSON{0}, std::nullopt,
                                   "token", capabilities);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  {
    const auto version{MCPProtocolVersion::V_2025_03_26};

    if (version != CURRENT) {
      try {
        mcp_make_input_required_result(version, "tools/call", JSON{0},
                                       std::nullopt, "token", capabilities);
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2025_06_18};

    if (version != CURRENT) {
      try {
        mcp_make_input_required_result(version, "tools/call", JSON{0},
                                       std::nullopt, "token", capabilities);
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2025_11_25};

    if (version != CURRENT) {
      try {
        mcp_make_input_required_result(version, "tools/call", JSON{0},
                                       std::nullopt, "token", capabilities);
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2026_07_28};

    if (version != CURRENT) {
      try {
        mcp_make_input_required_result(version, "tools/call", JSON{0},
                                       std::nullopt, "token", capabilities);
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
    }
  }
  const auto continuation{parse_json(
      R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "tools/call",
  "params": {
    "requestState": "s",
    "inputResponses": {}
  }
})")};
  EXPECT_TRUE(mcp_request_state(CURRENT, continuation).has_value());
  EXPECT_FALSE(mcp_request_state(MCPProtocolVersion::V_2025_11_25, continuation)
                   .has_value());
  auto unrelated{continuation};
  unrelated.assign("method", JSON{"tools/list"});
  EXPECT_EQ(mcp_request_input_responses(CURRENT, unrelated), nullptr);
}

TEST(result_writer_2025_03_26) {
  constexpr auto *FIXTURE{
      R"({
  "resources": [
    {
      "name": "test",
      "uri": "https://example.com/a"
    }
  ],
  "nextCursor": "",
  "_meta": {
    "example.org/value": "preserved"
  }
})"};
  const auto page{parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{parse_json(FIXTURE)};

  const auto version{MCPProtocolVersion::V_2025_03_26};

  std::ostringstream stream;
  mcp_write_result(stream, version, "resources/list", JSON{"quote\"\n"}, page,
                   MCPCachePolicy{});
  const auto parsed{parse_json(stream.str())};
  EXPECT_EQ(parsed.at("id"), JSON{"quote\"\n"});
  EXPECT_EQ(parsed.at("result").at("resources"), page.at("resources"));
  EXPECT_EQ(parsed.at("result").at("nextCursor"), JSON{""});
  EXPECT_EQ(parsed.at("result").at("_meta").at("example.org/value"),
            JSON{"preserved"});
  EXPECT_EQ(page, saved);
  EXPECT_EQ(stream.str().find('\n'), std::string::npos);
  EXPECT_TRUE(parsed.at("result").is_object());
}

TEST(result_writer_2025_06_18) {
  constexpr auto *FIXTURE{
      R"({
  "resources": [
    {
      "name": "test",
      "uri": "https://example.com/a"
    }
  ],
  "nextCursor": "",
  "_meta": {
    "example.org/value": "preserved"
  }
})"};
  const auto page{parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{parse_json(FIXTURE)};

  const auto version{MCPProtocolVersion::V_2025_06_18};

  std::ostringstream stream;
  mcp_write_result(stream, version, "resources/list", JSON{"quote\"\n"}, page,
                   MCPCachePolicy{});
  const auto parsed{parse_json(stream.str())};
  EXPECT_EQ(parsed.at("id"), JSON{"quote\"\n"});
  EXPECT_EQ(parsed.at("result").at("resources"), page.at("resources"));
  EXPECT_EQ(parsed.at("result").at("nextCursor"), JSON{""});
  EXPECT_EQ(parsed.at("result").at("_meta").at("example.org/value"),
            JSON{"preserved"});
  EXPECT_EQ(page, saved);
  EXPECT_EQ(stream.str().find('\n'), std::string::npos);
  EXPECT_TRUE(parsed.at("result").is_object());
}

TEST(result_writer_2025_11_25) {
  constexpr auto *FIXTURE{
      R"({
  "resources": [
    {
      "name": "test",
      "uri": "https://example.com/a"
    }
  ],
  "nextCursor": "",
  "_meta": {
    "example.org/value": "preserved"
  }
})"};
  const auto page{parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{parse_json(FIXTURE)};

  const auto version{MCPProtocolVersion::V_2025_11_25};

  std::ostringstream stream;
  mcp_write_result(stream, version, "resources/list", JSON{"quote\"\n"}, page,
                   MCPCachePolicy{});
  const auto parsed{parse_json(stream.str())};
  EXPECT_EQ(parsed.at("id"), JSON{"quote\"\n"});
  EXPECT_EQ(parsed.at("result").at("resources"), page.at("resources"));
  EXPECT_EQ(parsed.at("result").at("nextCursor"), JSON{""});
  EXPECT_EQ(parsed.at("result").at("_meta").at("example.org/value"),
            JSON{"preserved"});
  EXPECT_EQ(page, saved);
  EXPECT_EQ(stream.str().find('\n'), std::string::npos);
  EXPECT_TRUE(parsed.at("result").is_object());
}

TEST(result_writer_2026_07_28) {
  constexpr auto *FIXTURE{
      R"({
  "resources": [
    {
      "name": "test",
      "uri": "https://example.com/a"
    }
  ],
  "nextCursor": "",
  "_meta": {
    "example.org/value": "preserved"
  }
})"};
  const auto page{parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{parse_json(FIXTURE)};

  const auto version{MCPProtocolVersion::V_2026_07_28};

  std::ostringstream stream;
  mcp_write_result(stream, version, "resources/list", JSON{"quote\"\n"}, page,
                   MCPCachePolicy{});
  const auto parsed{parse_json(stream.str())};
  EXPECT_EQ(parsed.at("id"), JSON{"quote\"\n"});
  EXPECT_EQ(parsed.at("result").at("resources"), page.at("resources"));
  EXPECT_EQ(parsed.at("result").at("nextCursor"), JSON{""});
  EXPECT_EQ(parsed.at("result").at("_meta").at("example.org/value"),
            JSON{"preserved"});
  EXPECT_EQ(page, saved);
  EXPECT_EQ(stream.str().find('\n'), std::string::npos);
  EXPECT_TRUE(parsed.at("result").is_object());
}

TEST(builder_results_2025_03_26) {
  const JSON identifier{0};
  const MCPImplementation server{.name = "server",
                                 .version = "1",
                                 .title = "Title",
                                 .description = "Description",
                                 .website_url = "https://example.com"};

  const auto version{MCPProtocolVersion::V_2025_03_26};

  EXPECT_TRUE(
      mcp_make_empty_result(version, identifier).at("result").is_object());
  EXPECT_TRUE(mcp_make_tool_success(version, identifier, JSON::make_object())
                  .at("result")
                  .is_object());
  EXPECT_TRUE(
      mcp_make_tool_success(version, identifier, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":"hello"}])"))
          .at("result")
          .is_object());
  EXPECT_TRUE(mcp_make_tool_error(version, identifier, "failure")
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_tool_descriptor(
                  version, "tool", "description",
                  parse_json(R"({"type":"object","properties":{}})"))
                  .is_object());
  EXPECT_TRUE(mcp_make_resource(version, "https://example.com", "resource",
                                "text/plain", "", 10, 0.5)
                  .is_object());
  EXPECT_TRUE(mcp_make_resource_template(version, "https://example.com/{name}",
                                         "template", "description",
                                         "text/plain")
                  .is_object());
  EXPECT_TRUE(
      (mcp_supports_resource_link_content(version)
           ? mcp_make_resource_link(version, "https://example.com", "link")
           : mcp_make_text_block("link"))
          .is_object());
  EXPECT_TRUE(mcp_make_tools_list_result(version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(
      mcp_make_resources_list_result(version, JSON::make_array(), "", {})
          .is_object());
  EXPECT_TRUE(mcp_make_resource_templates_list_result(
                  version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(mcp_make_prompts_list_result(version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(
      mcp_make_resources_read_result(
          version,
          parse_json(R"([{"uri":"https://example.com","text":"hello"}])"), {})
          .is_object());
  EXPECT_TRUE(
      mcp_make_prompts_get_result(
          version, "description",
          parse_json(
              R"([{"role":"user","content":{"type":"text","text":"hello"}}])"))
          .is_object());
  EXPECT_TRUE(mcp_make_completion_result(
                  version, parse_json(R"(["one","two"] )"), 3, true)
                  .is_object());
  EXPECT_TRUE(mcp_serialize_client_capabilities(version, {}).is_object());
  const auto error{
      mcp_make_error(version, &identifier, JSONRPC_CODE_INVALID_PARAMS, "bad")};
  EXPECT_TRUE(error.is_object());
  EXPECT_TRUE(
      mcp_make_error_resource_not_found(version, identifier).is_object());
  if (mcp_uses_initialization_handshake(version)) {
    auto init{parse_json(
        R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "initialize",
  "params": {
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})")};
    init.at("params").assign("protocolVersion",
                             JSON{mcp_protocol_version_string(version)});
    EXPECT_TRUE(mcp_make_initialize_result(init, {}, server, WIRES)
                    .at("result")
                    .is_object());
  }

  EXPECT_TRUE(mcp_make_server_discover_result(CURRENT, identifier, {}, server,
                                              WIRES, "instructions", {})
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_subscription_close_result(CURRENT, identifier)
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_input_required_result(CURRENT, "resources/read",
                                             identifier, std::nullopt, "state",
                                             {})
                  .at("result")
                  .is_object());
}

TEST(builder_results_2025_06_18) {
  const JSON identifier{0};
  const MCPImplementation server{.name = "server",
                                 .version = "1",
                                 .title = "Title",
                                 .description = "Description",
                                 .website_url = "https://example.com"};

  const auto version{MCPProtocolVersion::V_2025_06_18};

  EXPECT_TRUE(
      mcp_make_empty_result(version, identifier).at("result").is_object());
  EXPECT_TRUE(mcp_make_tool_success(version, identifier, JSON::make_object())
                  .at("result")
                  .is_object());
  EXPECT_TRUE(
      mcp_make_tool_success(version, identifier, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":"hello"}])"))
          .at("result")
          .is_object());
  EXPECT_TRUE(mcp_make_tool_error(version, identifier, "failure")
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_tool_descriptor(
                  version, "tool", "description",
                  parse_json(R"({"type":"object","properties":{}})"))
                  .is_object());
  EXPECT_TRUE(mcp_make_resource(version, "https://example.com", "resource",
                                "text/plain", "", 10, 0.5)
                  .is_object());
  EXPECT_TRUE(mcp_make_resource_template(version, "https://example.com/{name}",
                                         "template", "description",
                                         "text/plain")
                  .is_object());
  EXPECT_TRUE(
      (mcp_supports_resource_link_content(version)
           ? mcp_make_resource_link(version, "https://example.com", "link")
           : mcp_make_text_block("link"))
          .is_object());
  EXPECT_TRUE(mcp_make_tools_list_result(version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(
      mcp_make_resources_list_result(version, JSON::make_array(), "", {})
          .is_object());
  EXPECT_TRUE(mcp_make_resource_templates_list_result(
                  version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(mcp_make_prompts_list_result(version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(
      mcp_make_resources_read_result(
          version,
          parse_json(R"([{"uri":"https://example.com","text":"hello"}])"), {})
          .is_object());
  EXPECT_TRUE(
      mcp_make_prompts_get_result(
          version, "description",
          parse_json(
              R"([{"role":"user","content":{"type":"text","text":"hello"}}])"))
          .is_object());
  EXPECT_TRUE(mcp_make_completion_result(
                  version, parse_json(R"(["one","two"] )"), 3, true)
                  .is_object());
  EXPECT_TRUE(mcp_serialize_client_capabilities(version, {}).is_object());
  const auto error{
      mcp_make_error(version, &identifier, JSONRPC_CODE_INVALID_PARAMS, "bad")};
  EXPECT_TRUE(error.is_object());
  EXPECT_TRUE(
      mcp_make_error_resource_not_found(version, identifier).is_object());
  if (mcp_uses_initialization_handshake(version)) {
    auto init{parse_json(
        R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "initialize",
  "params": {
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})")};
    init.at("params").assign("protocolVersion",
                             JSON{mcp_protocol_version_string(version)});
    EXPECT_TRUE(mcp_make_initialize_result(init, {}, server, WIRES)
                    .at("result")
                    .is_object());
  }

  EXPECT_TRUE(mcp_make_server_discover_result(CURRENT, identifier, {}, server,
                                              WIRES, "instructions", {})
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_subscription_close_result(CURRENT, identifier)
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_input_required_result(CURRENT, "resources/read",
                                             identifier, std::nullopt, "state",
                                             {})
                  .at("result")
                  .is_object());
}

TEST(builder_results_2025_11_25) {
  const JSON identifier{0};
  const MCPImplementation server{.name = "server",
                                 .version = "1",
                                 .title = "Title",
                                 .description = "Description",
                                 .website_url = "https://example.com"};

  const auto version{MCPProtocolVersion::V_2025_11_25};

  EXPECT_TRUE(
      mcp_make_empty_result(version, identifier).at("result").is_object());
  EXPECT_TRUE(mcp_make_tool_success(version, identifier, JSON::make_object())
                  .at("result")
                  .is_object());
  EXPECT_TRUE(
      mcp_make_tool_success(version, identifier, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":"hello"}])"))
          .at("result")
          .is_object());
  EXPECT_TRUE(mcp_make_tool_error(version, identifier, "failure")
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_tool_descriptor(
                  version, "tool", "description",
                  parse_json(R"({"type":"object","properties":{}})"))
                  .is_object());
  EXPECT_TRUE(mcp_make_resource(version, "https://example.com", "resource",
                                "text/plain", "", 10, 0.5)
                  .is_object());
  EXPECT_TRUE(mcp_make_resource_template(version, "https://example.com/{name}",
                                         "template", "description",
                                         "text/plain")
                  .is_object());
  EXPECT_TRUE(
      (mcp_supports_resource_link_content(version)
           ? mcp_make_resource_link(version, "https://example.com", "link")
           : mcp_make_text_block("link"))
          .is_object());
  EXPECT_TRUE(mcp_make_tools_list_result(version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(
      mcp_make_resources_list_result(version, JSON::make_array(), "", {})
          .is_object());
  EXPECT_TRUE(mcp_make_resource_templates_list_result(
                  version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(mcp_make_prompts_list_result(version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(
      mcp_make_resources_read_result(
          version,
          parse_json(R"([{"uri":"https://example.com","text":"hello"}])"), {})
          .is_object());
  EXPECT_TRUE(
      mcp_make_prompts_get_result(
          version, "description",
          parse_json(
              R"([{"role":"user","content":{"type":"text","text":"hello"}}])"))
          .is_object());
  EXPECT_TRUE(mcp_make_completion_result(
                  version, parse_json(R"(["one","two"] )"), 3, true)
                  .is_object());
  EXPECT_TRUE(mcp_serialize_client_capabilities(version, {}).is_object());
  const auto error{
      mcp_make_error(version, &identifier, JSONRPC_CODE_INVALID_PARAMS, "bad")};
  EXPECT_TRUE(error.is_object());
  EXPECT_TRUE(
      mcp_make_error_resource_not_found(version, identifier).is_object());
  if (mcp_uses_initialization_handshake(version)) {
    auto init{parse_json(
        R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "initialize",
  "params": {
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})")};
    init.at("params").assign("protocolVersion",
                             JSON{mcp_protocol_version_string(version)});
    EXPECT_TRUE(mcp_make_initialize_result(init, {}, server, WIRES)
                    .at("result")
                    .is_object());
  }

  EXPECT_TRUE(mcp_make_server_discover_result(CURRENT, identifier, {}, server,
                                              WIRES, "instructions", {})
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_subscription_close_result(CURRENT, identifier)
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_input_required_result(CURRENT, "resources/read",
                                             identifier, std::nullopt, "state",
                                             {})
                  .at("result")
                  .is_object());
}

TEST(builder_results_2026_07_28) {
  const JSON identifier{0};
  const MCPImplementation server{.name = "server",
                                 .version = "1",
                                 .title = "Title",
                                 .description = "Description",
                                 .website_url = "https://example.com"};

  const auto version{MCPProtocolVersion::V_2026_07_28};

  EXPECT_TRUE(
      mcp_make_empty_result(version, identifier).at("result").is_object());
  EXPECT_TRUE(mcp_make_tool_success(version, identifier, JSON::make_object())
                  .at("result")
                  .is_object());
  EXPECT_TRUE(
      mcp_make_tool_success(version, identifier, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":"hello"}])"))
          .at("result")
          .is_object());
  EXPECT_TRUE(mcp_make_tool_error(version, identifier, "failure")
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_tool_descriptor(
                  version, "tool", "description",
                  parse_json(R"({"type":"object","properties":{}})"))
                  .is_object());
  EXPECT_TRUE(mcp_make_resource(version, "https://example.com", "resource",
                                "text/plain", "", 10, 0.5)
                  .is_object());
  EXPECT_TRUE(mcp_make_resource_template(version, "https://example.com/{name}",
                                         "template", "description",
                                         "text/plain")
                  .is_object());
  EXPECT_TRUE(
      (mcp_supports_resource_link_content(version)
           ? mcp_make_resource_link(version, "https://example.com", "link")
           : mcp_make_text_block("link"))
          .is_object());
  EXPECT_TRUE(mcp_make_tools_list_result(version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(
      mcp_make_resources_list_result(version, JSON::make_array(), "", {})
          .is_object());
  EXPECT_TRUE(mcp_make_resource_templates_list_result(
                  version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(mcp_make_prompts_list_result(version, JSON::make_array(), "", {})
                  .is_object());
  EXPECT_TRUE(
      mcp_make_resources_read_result(
          version,
          parse_json(R"([{"uri":"https://example.com","text":"hello"}])"), {})
          .is_object());
  EXPECT_TRUE(
      mcp_make_prompts_get_result(
          version, "description",
          parse_json(
              R"([{"role":"user","content":{"type":"text","text":"hello"}}])"))
          .is_object());
  EXPECT_TRUE(mcp_make_completion_result(
                  version, parse_json(R"(["one","two"] )"), 3, true)
                  .is_object());
  EXPECT_TRUE(mcp_serialize_client_capabilities(version, {}).is_object());
  const auto error{
      mcp_make_error(version, &identifier, JSONRPC_CODE_INVALID_PARAMS, "bad")};
  EXPECT_TRUE(error.is_object());
  EXPECT_TRUE(
      mcp_make_error_resource_not_found(version, identifier).is_object());
  if (mcp_uses_initialization_handshake(version)) {
    auto init{parse_json(
        R"({
  "jsonrpc": "2.0",
  "id": 0,
  "method": "initialize",
  "params": {
    "capabilities": {},
    "clientInfo": {
      "name": "client",
      "version": "1"
    }
  }
})")};
    init.at("params").assign("protocolVersion",
                             JSON{mcp_protocol_version_string(version)});
    EXPECT_TRUE(mcp_make_initialize_result(init, {}, server, WIRES)
                    .at("result")
                    .is_object());
  }

  EXPECT_TRUE(mcp_make_server_discover_result(CURRENT, identifier, {}, server,
                                              WIRES, "instructions", {})
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_subscription_close_result(CURRENT, identifier)
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_input_required_result(CURRENT, "resources/read",
                                             identifier, std::nullopt, "state",
                                             {})
                  .at("result")
                  .is_object());
}

TEST(continuation_validation) {
  auto value{parameters()};
  value.assign("requestState", JSON{"state"});
  value.assign("inputResponses", parse_json(
                                     R"({
  "roots": {
    "roots": [
      {
        "uri": "file:///workspace"
      }
    ]
  },
  "form": {
    "action": "accept",
    "content": {
      "choice": [
        "a"
      ],
      "accepted": true
    }
  },
  "sample": {
    "role": "assistant",
    "model": "model",
    "content": {
      "type": "text",
      "text": "hello"
    }
  },
  "unknown": {
    "ignored": true
  }
})"));
  const auto prior{parse_json(
      R"({
  "roots": {
    "method": "roots/list"
  },
  "form": {
    "method": "elicitation/create"
  },
  "sample": {
    "method": "sampling/createMessage"
  }
})")};
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, "state"));
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, "different"));
  EXPECT_FALSE(mcp_validate_continuation(MCPProtocolVersion::V_2025_11_25,
                                         value, prior, "state"));
  auto bad{value};
  bad.at("inputResponses")
      .at("roots")
      .at("roots")
      .at(0)
      .assign("uri", JSON{"https://example.com"});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, "state"));
  bad = value;
  bad.at("inputResponses").at("form").assign("action", JSON{"unknown"});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, "state"));
  bad = value;
  bad.at("inputResponses").at("sample").erase("model");
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, "state"));
  bad = value;
  bad.assign("inputResponses", JSON{false});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, "state"));
  bad = value;
  bad.assign("requestState", JSON{1});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior));
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, parameters(), prior));
}

TEST(continuation_empty_expected_state) {
  auto value{parameters()};
  value.assign("inputResponses", JSON::make_object());
  const auto prior{JSON::make_object()};
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior));
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, ""));
  value.assign("requestState", JSON{""});
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, ""));
  value.assign("requestState", JSON{"different"});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, ""));
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, JSON{false}, prior, ""));
  value.assign("requestState", JSON{false});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, ""));
}

TEST(tool_schema_content_is_opaque_2025_03_26) {
  const auto schema{parse_json(
      R"({
  "type": "object",
  "$defs": {
    "value": {
      "type": "string"
    }
  },
  "properties": {
    "name": {
      "$ref": "#/$defs/value"
    }
  },
  "examples": [
    {
      "x-mcp-header": "Not an annotation",
      "properties": {
        "type": "invalid"
      }
    }
  ]
})")};

  const auto version{MCPProtocolVersion::V_2025_03_26};

  EXPECT_EQ(
      mcp_make_tool_descriptor(version, "tool", "", schema).at("inputSchema"),
      schema);
  for (const auto *const text :
       {"false", "null", "[]", "{}", R"({"type":"array"})"}) {
    try {
      mcp_make_tool_descriptor(version, "tool", "", parse_json(text));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
}

TEST(tool_schema_content_is_opaque_2025_06_18) {
  const auto schema{parse_json(
      R"({
  "type": "object",
  "$defs": {
    "value": {
      "type": "string"
    }
  },
  "properties": {
    "name": {
      "$ref": "#/$defs/value"
    }
  },
  "examples": [
    {
      "x-mcp-header": "Not an annotation",
      "properties": {
        "type": "invalid"
      }
    }
  ]
})")};

  const auto version{MCPProtocolVersion::V_2025_06_18};

  EXPECT_EQ(
      mcp_make_tool_descriptor(version, "tool", "", schema).at("inputSchema"),
      schema);
  for (const auto *const text :
       {"false", "null", "[]", "{}", R"({"type":"array"})"}) {
    try {
      mcp_make_tool_descriptor(version, "tool", "", parse_json(text));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
}

TEST(tool_schema_content_is_opaque_2025_11_25) {
  const auto schema{parse_json(
      R"({
  "type": "object",
  "$defs": {
    "value": {
      "type": "string"
    }
  },
  "properties": {
    "name": {
      "$ref": "#/$defs/value"
    }
  },
  "examples": [
    {
      "x-mcp-header": "Not an annotation",
      "properties": {
        "type": "invalid"
      }
    }
  ]
})")};

  const auto version{MCPProtocolVersion::V_2025_11_25};

  EXPECT_EQ(
      mcp_make_tool_descriptor(version, "tool", "", schema).at("inputSchema"),
      schema);
  for (const auto *const text :
       {"false", "null", "[]", "{}", R"({"type":"array"})"}) {
    try {
      mcp_make_tool_descriptor(version, "tool", "", parse_json(text));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
}

TEST(tool_schema_content_is_opaque_2026_07_28) {
  const auto schema{parse_json(
      R"({
  "type": "object",
  "$defs": {
    "value": {
      "type": "string"
    }
  },
  "properties": {
    "name": {
      "$ref": "#/$defs/value"
    }
  },
  "examples": [
    {
      "x-mcp-header": "Not an annotation",
      "properties": {
        "type": "invalid"
      }
    }
  ]
})")};

  const auto version{MCPProtocolVersion::V_2026_07_28};

  EXPECT_EQ(
      mcp_make_tool_descriptor(version, "tool", "", schema).at("inputSchema"),
      schema);
  for (const auto *const text :
       {"false", "null", "[]", "{}", R"({"type":"array"})"}) {
    try {
      mcp_make_tool_descriptor(version, "tool", "", parse_json(text));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
}

TEST(elicitation_numbers_and_mode) {
  auto value{parameters()};
  auto prior{parse_json(
      R"({
  "form": {
    "method": "elicitation/create",
    "params": {
      "mode": "form",
      "message": "value",
      "requestedSchema": {
        "type": "object",
        "properties": {}
      }
    }
  }
})")};
  value.assign("inputResponses", parse_json(
                                     R"({
  "form": {
    "action": "accept",
    "content": {
      "value": 1.25,
      "precise": 1.0
    }
  }
})"));
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior));
  prior.at("form").at("params").assign("mode", JSON{"url"});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior));
  value.at("inputResponses").at("form").erase("content");
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior));
  prior.at("form").at("params").assign("mode", JSON{"form"});
  for (const auto *const action : {"decline", "cancel"}) {
    value.at("inputResponses").at("form").assign("action", JSON{action});
    EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior));
    value.at("inputResponses")
        .at("form")
        .assign("content", JSON::make_object());
    EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior));
    value.at("inputResponses").at("form").erase("content");
  }
  MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  for (const auto *const tokens : {"100.0", "0", "-1"}) {
    auto inputs{parse_json(
        R"({
  "sample": {
    "method": "sampling/createMessage",
    "params": {
      "messages": [],
      "maxTokens": 0
    }
  }
})")};
    inputs.at("sample").at("params").assign("maxTokens", parse_json(tokens));
    EXPECT_TRUE(mcp_make_input_required_result(CURRENT, "tools/call", JSON{0},
                                               inputs, std::nullopt,
                                               capabilities)
                    .at("result")
                    .defines("inputRequests"));
  }
}

TEST(descriptor_and_content_2025_03_26) {
  constexpr std::array<JSON::StringView, 2> ROLES{{"user", "assistant"}};
  const auto icons{parse_json(
      R"([
  {
    "src": "https://example.com/icon.png",
    "mimeType": "image/png",
    "sizes": [
      "32x32"
    ],
    "theme": "dark"
  }
])")};
  const MCPDescriptorPresentation presentation{.title = "Title",
                                               .icons = &icons};
  const MCPResourceAnnotations annotations{.audience = ROLES,
                                           .priority = 0.5,
                                           .last_modified =
                                               "2026-07-28T00:00:00Z"};

  const auto version{MCPProtocolVersion::V_2025_03_26};

  const auto resource{
      mcp_make_resource(version, "file:///a", "a", "text/plain", "description",
                        std::nullopt, std::nullopt, presentation, annotations)};
  EXPECT_EQ(resource.defines("title"),
            version != MCPProtocolVersion::V_2025_03_26);
  EXPECT_EQ(
      resource.defines("icons"),
      mcp_protocol_version_at_least(version, MCPProtocolVersion::V_2025_11_25));
  EXPECT_EQ(resource.at("annotations").defines("lastModified"),
            version != MCPProtocolVersion::V_2025_03_26);
  EXPECT_TRUE(resource.is_object());
  EXPECT_TRUE(mcp_make_resource_template(version, "file:///{name}", "template",
                                         "description", "text/plain",
                                         presentation, annotations)
                  .is_object());
  EXPECT_TRUE(mcp_make_tool_descriptor(version, "tool", "description",
                                       parse_json(R"({"type":"object"})"),
                                       std::nullopt, {}, presentation)
                  .is_object());
  EXPECT_TRUE(
      mcp_serialize_resource_annotations(version, annotations).is_object());
  EXPECT_TRUE(mcp_make_image_block(version, "YWJj", "image/png", annotations)
                  .is_object());
  EXPECT_TRUE(mcp_make_audio_block(version, "YWJj", "audio/wav", annotations)
                  .is_object());
  const auto blob{mcp_make_resource_blob_content(
      "file:///a", "application/octet-stream", "YWJj")};
  EXPECT_TRUE(blob.is_object());
  EXPECT_TRUE(
      mcp_make_embedded_resource(version, blob, annotations).is_object());
  EXPECT_TRUE(
      mcp_make_embedded_resource(
          version,
          mcp_make_resource_text_content("file:///a", "text/plain", "hello"),
          annotations)
          .is_object());
  EXPECT_TRUE(mcp_serialize_server_capabilities(
                  version, mcp_parse_server_capabilities(version, parse_json(
                                                                      R"({
  "resources": {
    "subscribe": true,
    "listChanged": false,
    "custom": {}
  },
  "logging": {},
  "completions": {}
})")))
                  .is_object());

  const std::array<JSON::StringView, 1> invalid{{"system"}};
  try {
    mcp_serialize_resource_annotations(
        CURRENT,
        {.audience = invalid, .priority = std::nullopt, .last_modified = {}});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_embedded_resource(CURRENT, parse_json(R"({"uri":"file:///a"})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  const auto bad_icons{parse_json(R"([{"src":true}])")};
  try {
    mcp_make_tool_descriptor(CURRENT, "tool", "",
                             parse_json(R"({"type":"object"})"), std::nullopt,
                             {}, {.title = {}, .icons = &bad_icons});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_parse_server_capabilities(
        CURRENT,
        parse_json(
            R"({"resources":{"subscribe":false,"listChanged":"invalid"}})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_serialize_server_capabilities(CURRENT, {.extensions = JSON{false}});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(descriptor_and_content_2025_06_18) {
  constexpr std::array<JSON::StringView, 2> ROLES{{"user", "assistant"}};
  const auto icons{parse_json(
      R"([
  {
    "src": "https://example.com/icon.png",
    "mimeType": "image/png",
    "sizes": [
      "32x32"
    ],
    "theme": "dark"
  }
])")};
  const MCPDescriptorPresentation presentation{.title = "Title",
                                               .icons = &icons};
  const MCPResourceAnnotations annotations{.audience = ROLES,
                                           .priority = 0.5,
                                           .last_modified =
                                               "2026-07-28T00:00:00Z"};

  const auto version{MCPProtocolVersion::V_2025_06_18};

  const auto resource{
      mcp_make_resource(version, "file:///a", "a", "text/plain", "description",
                        std::nullopt, std::nullopt, presentation, annotations)};
  EXPECT_EQ(resource.defines("title"),
            version != MCPProtocolVersion::V_2025_03_26);
  EXPECT_EQ(
      resource.defines("icons"),
      mcp_protocol_version_at_least(version, MCPProtocolVersion::V_2025_11_25));
  EXPECT_EQ(resource.at("annotations").defines("lastModified"),
            version != MCPProtocolVersion::V_2025_03_26);
  EXPECT_TRUE(resource.is_object());
  EXPECT_TRUE(mcp_make_resource_template(version, "file:///{name}", "template",
                                         "description", "text/plain",
                                         presentation, annotations)
                  .is_object());
  EXPECT_TRUE(mcp_make_tool_descriptor(version, "tool", "description",
                                       parse_json(R"({"type":"object"})"),
                                       std::nullopt, {}, presentation)
                  .is_object());
  EXPECT_TRUE(
      mcp_serialize_resource_annotations(version, annotations).is_object());
  EXPECT_TRUE(mcp_make_image_block(version, "YWJj", "image/png", annotations)
                  .is_object());
  EXPECT_TRUE(mcp_make_audio_block(version, "YWJj", "audio/wav", annotations)
                  .is_object());
  const auto blob{mcp_make_resource_blob_content(
      "file:///a", "application/octet-stream", "YWJj")};
  EXPECT_TRUE(blob.is_object());
  EXPECT_TRUE(
      mcp_make_embedded_resource(version, blob, annotations).is_object());
  EXPECT_TRUE(
      mcp_make_embedded_resource(
          version,
          mcp_make_resource_text_content("file:///a", "text/plain", "hello"),
          annotations)
          .is_object());
  EXPECT_TRUE(mcp_serialize_server_capabilities(
                  version, mcp_parse_server_capabilities(version, parse_json(
                                                                      R"({
  "resources": {
    "subscribe": true,
    "listChanged": false,
    "custom": {}
  },
  "logging": {},
  "completions": {}
})")))
                  .is_object());

  const std::array<JSON::StringView, 1> invalid{{"system"}};
  try {
    mcp_serialize_resource_annotations(
        CURRENT,
        {.audience = invalid, .priority = std::nullopt, .last_modified = {}});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_embedded_resource(CURRENT, parse_json(R"({"uri":"file:///a"})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  const auto bad_icons{parse_json(R"([{"src":true}])")};
  try {
    mcp_make_tool_descriptor(CURRENT, "tool", "",
                             parse_json(R"({"type":"object"})"), std::nullopt,
                             {}, {.title = {}, .icons = &bad_icons});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_parse_server_capabilities(
        CURRENT,
        parse_json(
            R"({"resources":{"subscribe":false,"listChanged":"invalid"}})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_serialize_server_capabilities(CURRENT, {.extensions = JSON{false}});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(descriptor_and_content_2025_11_25) {
  constexpr std::array<JSON::StringView, 2> ROLES{{"user", "assistant"}};
  const auto icons{parse_json(
      R"([
  {
    "src": "https://example.com/icon.png",
    "mimeType": "image/png",
    "sizes": [
      "32x32"
    ],
    "theme": "dark"
  }
])")};
  const MCPDescriptorPresentation presentation{.title = "Title",
                                               .icons = &icons};
  const MCPResourceAnnotations annotations{.audience = ROLES,
                                           .priority = 0.5,
                                           .last_modified =
                                               "2026-07-28T00:00:00Z"};

  const auto version{MCPProtocolVersion::V_2025_11_25};

  const auto resource{
      mcp_make_resource(version, "file:///a", "a", "text/plain", "description",
                        std::nullopt, std::nullopt, presentation, annotations)};
  EXPECT_EQ(resource.defines("title"),
            version != MCPProtocolVersion::V_2025_03_26);
  EXPECT_EQ(
      resource.defines("icons"),
      mcp_protocol_version_at_least(version, MCPProtocolVersion::V_2025_11_25));
  EXPECT_EQ(resource.at("annotations").defines("lastModified"),
            version != MCPProtocolVersion::V_2025_03_26);
  EXPECT_TRUE(resource.is_object());
  EXPECT_TRUE(mcp_make_resource_template(version, "file:///{name}", "template",
                                         "description", "text/plain",
                                         presentation, annotations)
                  .is_object());
  EXPECT_TRUE(mcp_make_tool_descriptor(version, "tool", "description",
                                       parse_json(R"({"type":"object"})"),
                                       std::nullopt, {}, presentation)
                  .is_object());
  EXPECT_TRUE(
      mcp_serialize_resource_annotations(version, annotations).is_object());
  EXPECT_TRUE(mcp_make_image_block(version, "YWJj", "image/png", annotations)
                  .is_object());
  EXPECT_TRUE(mcp_make_audio_block(version, "YWJj", "audio/wav", annotations)
                  .is_object());
  const auto blob{mcp_make_resource_blob_content(
      "file:///a", "application/octet-stream", "YWJj")};
  EXPECT_TRUE(blob.is_object());
  EXPECT_TRUE(
      mcp_make_embedded_resource(version, blob, annotations).is_object());
  EXPECT_TRUE(
      mcp_make_embedded_resource(
          version,
          mcp_make_resource_text_content("file:///a", "text/plain", "hello"),
          annotations)
          .is_object());
  EXPECT_TRUE(mcp_serialize_server_capabilities(
                  version, mcp_parse_server_capabilities(version, parse_json(
                                                                      R"({
  "resources": {
    "subscribe": true,
    "listChanged": false,
    "custom": {}
  },
  "logging": {},
  "completions": {}
})")))
                  .is_object());

  const std::array<JSON::StringView, 1> invalid{{"system"}};
  try {
    mcp_serialize_resource_annotations(
        CURRENT,
        {.audience = invalid, .priority = std::nullopt, .last_modified = {}});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_embedded_resource(CURRENT, parse_json(R"({"uri":"file:///a"})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  const auto bad_icons{parse_json(R"([{"src":true}])")};
  try {
    mcp_make_tool_descriptor(CURRENT, "tool", "",
                             parse_json(R"({"type":"object"})"), std::nullopt,
                             {}, {.title = {}, .icons = &bad_icons});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_parse_server_capabilities(
        CURRENT,
        parse_json(
            R"({"resources":{"subscribe":false,"listChanged":"invalid"}})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_serialize_server_capabilities(CURRENT, {.extensions = JSON{false}});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(descriptor_and_content_2026_07_28) {
  constexpr std::array<JSON::StringView, 2> ROLES{{"user", "assistant"}};
  const auto icons{parse_json(
      R"([
  {
    "src": "https://example.com/icon.png",
    "mimeType": "image/png",
    "sizes": [
      "32x32"
    ],
    "theme": "dark"
  }
])")};
  const MCPDescriptorPresentation presentation{.title = "Title",
                                               .icons = &icons};
  const MCPResourceAnnotations annotations{.audience = ROLES,
                                           .priority = 0.5,
                                           .last_modified =
                                               "2026-07-28T00:00:00Z"};

  const auto version{MCPProtocolVersion::V_2026_07_28};

  const auto resource{
      mcp_make_resource(version, "file:///a", "a", "text/plain", "description",
                        std::nullopt, std::nullopt, presentation, annotations)};
  EXPECT_EQ(resource.defines("title"),
            version != MCPProtocolVersion::V_2025_03_26);
  EXPECT_EQ(
      resource.defines("icons"),
      mcp_protocol_version_at_least(version, MCPProtocolVersion::V_2025_11_25));
  EXPECT_EQ(resource.at("annotations").defines("lastModified"),
            version != MCPProtocolVersion::V_2025_03_26);
  EXPECT_TRUE(resource.is_object());
  EXPECT_TRUE(mcp_make_resource_template(version, "file:///{name}", "template",
                                         "description", "text/plain",
                                         presentation, annotations)
                  .is_object());
  EXPECT_TRUE(mcp_make_tool_descriptor(version, "tool", "description",
                                       parse_json(R"({"type":"object"})"),
                                       std::nullopt, {}, presentation)
                  .is_object());
  EXPECT_TRUE(
      mcp_serialize_resource_annotations(version, annotations).is_object());
  EXPECT_TRUE(mcp_make_image_block(version, "YWJj", "image/png", annotations)
                  .is_object());
  EXPECT_TRUE(mcp_make_audio_block(version, "YWJj", "audio/wav", annotations)
                  .is_object());
  const auto blob{mcp_make_resource_blob_content(
      "file:///a", "application/octet-stream", "YWJj")};
  EXPECT_TRUE(blob.is_object());
  EXPECT_TRUE(
      mcp_make_embedded_resource(version, blob, annotations).is_object());
  EXPECT_TRUE(
      mcp_make_embedded_resource(
          version,
          mcp_make_resource_text_content("file:///a", "text/plain", "hello"),
          annotations)
          .is_object());
  EXPECT_TRUE(mcp_serialize_server_capabilities(
                  version, mcp_parse_server_capabilities(version, parse_json(
                                                                      R"({
  "resources": {
    "subscribe": true,
    "listChanged": false,
    "custom": {}
  },
  "logging": {},
  "completions": {}
})")))
                  .is_object());

  const std::array<JSON::StringView, 1> invalid{{"system"}};
  try {
    mcp_serialize_resource_annotations(
        CURRENT,
        {.audience = invalid, .priority = std::nullopt, .last_modified = {}});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_embedded_resource(CURRENT, parse_json(R"({"uri":"file:///a"})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  const auto bad_icons{parse_json(R"([{"src":true}])")};
  try {
    mcp_make_tool_descriptor(CURRENT, "tool", "",
                             parse_json(R"({"type":"object"})"), std::nullopt,
                             {}, {.title = {}, .icons = &bad_icons});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_parse_server_capabilities(
        CURRENT,
        parse_json(
            R"({"resources":{"subscribe":false,"listChanged":"invalid"}})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_serialize_server_capabilities(CURRENT, {.extensions = JSON{false}});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(result_writer_full_parity) {
  const std::array<std::pair<JSON::StringView, JSON::StringView>, 6> cases{
      {{"tools/list", R"({"tools":[],"nextCursor":""})"},
       {"resources/list", R"({"resources":[],"nextCursor":""})"},
       {"resources/templates/list",
        R"({"resourceTemplates":[],"nextCursor":""})"},
       {"prompts/list", R"({"prompts":[],"nextCursor":""})"},
       {"resources/read",
        R"({"contents":[{"uri":"file:///a","text":"hello","blob":"YWJj"}]})"},
       {"server/discover",
        R"({
  "supportedVersions": [
    "2026-07-28"
  ],
  "capabilities": {},
  "serverInfo": {
    "name": "server",
    "version": "1"
  }
})"}}};
  const JSON identifier{"escaped\"\\\n"};
  const MCPImplementation server{.name = "server", .version = "1"};
  {
    const auto version{MCPProtocolVersion::V_2025_03_26};

    const auto plain{parse_json(R"({"resources":[]})")};
    std::ostringstream plain_stream;
    mcp_write_result(plain_stream, version, "resources/list", identifier, plain,
                     MCPCachePolicy{}, server);
    EXPECT_EQ(parse_json(plain_stream.str()),
              jsonrpc_make_success(
                  identifier,
                  mcp_decorate_cacheable_result(
                      version, mcp_decorate_result(version, plain, server),
                      MCPCachePolicy{}, "resources/list")));
    EXPECT_EQ(plain, parse_json(R"({"resources":[]})"));
    for (const auto &entry : cases) {
      if (entry.first == "server/discover" &&
          !mcp_supports_server_discover(version)) {
        continue;
      }
      for (const auto decorated : {false, true}) {
        auto source{parse_json(entry.second)};
        source.assign("_meta",
                      parse_json(R"({"org.example/custom":{"value":true}})"));
        source.assign("org.example/extension",
                      parse_json(R"({"escaped\"key":[null,true,"line\n\\"]})"));
        if (decorated && version == CURRENT) {
          source = mcp_decorate_result(version, std::move(source), server);
          source = mcp_decorate_cacheable_result(
              version, std::move(source),
              {.ttl_ms = 5, .scope = MCPCacheScope::Public}, entry.first);
        }
        const auto before{source};
        for (const auto include_server : {false, true}) {
          const auto server_info{include_server
                                     ? std::optional<MCPImplementation>{server}
                                     : std::nullopt};
          const auto copied{jsonrpc_make_success(
              identifier,
              mcp_decorate_cacheable_result(
                  version, mcp_decorate_result(version, source, server_info),
                  {.ttl_ms = 123, .scope = MCPCacheScope::Private},
                  entry.first))};
          std::ostringstream stream;
          mcp_write_result(
              stream, version, entry.first, identifier, source,
              version == CURRENT
                  ? std::optional<MCPCachePolicy>{{.ttl_ms = 123,
                                                   .scope =
                                                       MCPCacheScope::Private}}
                  : std::nullopt,
              server_info);
          EXPECT_EQ(parse_json(stream.str()), copied);
          std::ostringstream expected_stream;
          stringify(copied, expected_stream);
          EXPECT_EQ(stream.str(), expected_stream.str());
          EXPECT_EQ(source, before);
        }
      }
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2025_06_18};

    const auto plain{parse_json(R"({"resources":[]})")};
    std::ostringstream plain_stream;
    mcp_write_result(plain_stream, version, "resources/list", identifier, plain,
                     MCPCachePolicy{}, server);
    EXPECT_EQ(parse_json(plain_stream.str()),
              jsonrpc_make_success(
                  identifier,
                  mcp_decorate_cacheable_result(
                      version, mcp_decorate_result(version, plain, server),
                      MCPCachePolicy{}, "resources/list")));
    EXPECT_EQ(plain, parse_json(R"({"resources":[]})"));
    for (const auto &entry : cases) {
      if (entry.first == "server/discover" &&
          !mcp_supports_server_discover(version)) {
        continue;
      }
      for (const auto decorated : {false, true}) {
        auto source{parse_json(entry.second)};
        source.assign("_meta",
                      parse_json(R"({"org.example/custom":{"value":true}})"));
        source.assign("org.example/extension",
                      parse_json(R"({"escaped\"key":[null,true,"line\n\\"]})"));
        if (decorated && version == CURRENT) {
          source = mcp_decorate_result(version, std::move(source), server);
          source = mcp_decorate_cacheable_result(
              version, std::move(source),
              {.ttl_ms = 5, .scope = MCPCacheScope::Public}, entry.first);
        }
        const auto before{source};
        for (const auto include_server : {false, true}) {
          const auto server_info{include_server
                                     ? std::optional<MCPImplementation>{server}
                                     : std::nullopt};
          const auto copied{jsonrpc_make_success(
              identifier,
              mcp_decorate_cacheable_result(
                  version, mcp_decorate_result(version, source, server_info),
                  {.ttl_ms = 123, .scope = MCPCacheScope::Private},
                  entry.first))};
          std::ostringstream stream;
          mcp_write_result(
              stream, version, entry.first, identifier, source,
              version == CURRENT
                  ? std::optional<MCPCachePolicy>{{.ttl_ms = 123,
                                                   .scope =
                                                       MCPCacheScope::Private}}
                  : std::nullopt,
              server_info);
          EXPECT_EQ(parse_json(stream.str()), copied);
          std::ostringstream expected_stream;
          stringify(copied, expected_stream);
          EXPECT_EQ(stream.str(), expected_stream.str());
          EXPECT_EQ(source, before);
        }
      }
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2025_11_25};

    const auto plain{parse_json(R"({"resources":[]})")};
    std::ostringstream plain_stream;
    mcp_write_result(plain_stream, version, "resources/list", identifier, plain,
                     MCPCachePolicy{}, server);
    EXPECT_EQ(parse_json(plain_stream.str()),
              jsonrpc_make_success(
                  identifier,
                  mcp_decorate_cacheable_result(
                      version, mcp_decorate_result(version, plain, server),
                      MCPCachePolicy{}, "resources/list")));
    EXPECT_EQ(plain, parse_json(R"({"resources":[]})"));
    for (const auto &entry : cases) {
      if (entry.first == "server/discover" &&
          !mcp_supports_server_discover(version)) {
        continue;
      }
      for (const auto decorated : {false, true}) {
        auto source{parse_json(entry.second)};
        source.assign("_meta",
                      parse_json(R"({"org.example/custom":{"value":true}})"));
        source.assign("org.example/extension",
                      parse_json(R"({"escaped\"key":[null,true,"line\n\\"]})"));
        if (decorated && version == CURRENT) {
          source = mcp_decorate_result(version, std::move(source), server);
          source = mcp_decorate_cacheable_result(
              version, std::move(source),
              {.ttl_ms = 5, .scope = MCPCacheScope::Public}, entry.first);
        }
        const auto before{source};
        for (const auto include_server : {false, true}) {
          const auto server_info{include_server
                                     ? std::optional<MCPImplementation>{server}
                                     : std::nullopt};
          const auto copied{jsonrpc_make_success(
              identifier,
              mcp_decorate_cacheable_result(
                  version, mcp_decorate_result(version, source, server_info),
                  {.ttl_ms = 123, .scope = MCPCacheScope::Private},
                  entry.first))};
          std::ostringstream stream;
          mcp_write_result(
              stream, version, entry.first, identifier, source,
              version == CURRENT
                  ? std::optional<MCPCachePolicy>{{.ttl_ms = 123,
                                                   .scope =
                                                       MCPCacheScope::Private}}
                  : std::nullopt,
              server_info);
          EXPECT_EQ(parse_json(stream.str()), copied);
          std::ostringstream expected_stream;
          stringify(copied, expected_stream);
          EXPECT_EQ(stream.str(), expected_stream.str());
          EXPECT_EQ(source, before);
        }
      }
    }
  }
  {
    const auto version{MCPProtocolVersion::V_2026_07_28};

    const auto plain{parse_json(R"({"resources":[]})")};
    std::ostringstream plain_stream;
    mcp_write_result(plain_stream, version, "resources/list", identifier, plain,
                     MCPCachePolicy{}, server);
    EXPECT_EQ(parse_json(plain_stream.str()),
              jsonrpc_make_success(
                  identifier,
                  mcp_decorate_cacheable_result(
                      version, mcp_decorate_result(version, plain, server),
                      MCPCachePolicy{}, "resources/list")));
    EXPECT_EQ(plain, parse_json(R"({"resources":[]})"));
    for (const auto &entry : cases) {
      if (entry.first == "server/discover" &&
          !mcp_supports_server_discover(version)) {
        continue;
      }
      for (const auto decorated : {false, true}) {
        auto source{parse_json(entry.second)};
        source.assign("_meta",
                      parse_json(R"({"org.example/custom":{"value":true}})"));
        source.assign("org.example/extension",
                      parse_json(R"({"escaped\"key":[null,true,"line\n\\"]})"));
        if (decorated && version == CURRENT) {
          source = mcp_decorate_result(version, std::move(source), server);
          source = mcp_decorate_cacheable_result(
              version, std::move(source),
              {.ttl_ms = 5, .scope = MCPCacheScope::Public}, entry.first);
        }
        const auto before{source};
        for (const auto include_server : {false, true}) {
          const auto server_info{include_server
                                     ? std::optional<MCPImplementation>{server}
                                     : std::nullopt};
          const auto copied{jsonrpc_make_success(
              identifier,
              mcp_decorate_cacheable_result(
                  version, mcp_decorate_result(version, source, server_info),
                  {.ttl_ms = 123, .scope = MCPCacheScope::Private},
                  entry.first))};
          std::ostringstream stream;
          mcp_write_result(
              stream, version, entry.first, identifier, source,
              version == CURRENT
                  ? std::optional<MCPCachePolicy>{{.ttl_ms = 123,
                                                   .scope =
                                                       MCPCacheScope::Private}}
                  : std::nullopt,
              server_info);
          EXPECT_EQ(parse_json(stream.str()), copied);
          std::ostringstream expected_stream;
          stringify(copied, expected_stream);
          EXPECT_EQ(stream.str(), expected_stream.str());
          EXPECT_EQ(source, before);
        }
      }
    }
  }
  for (const auto *const text :
       {R"({"resources":false})", R"({"resources":[],"nextCursor":false})",
        R"({"resources":[],"_meta":false})",
        R"({"resources":[],"resultType":"input_required"})"}) {
    std::ostringstream stream;
    try {
      mcp_write_result(stream, CURRENT, "resources/list", identifier,
                       parse_json(text), MCPCachePolicy{});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    EXPECT_TRUE(stream.str().empty());
  }
}

TEST(disputed_valid_inputs) {
  auto value{parameters()};
  for (const auto *const text : {"", " , ", "vendor=value,,"}) {
    value.at("_meta").assign("tracestate", JSON{text});
    EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
  }
  value.at("_meta").erase("tracestate");
  value.at("_meta").assign("baggage", JSON{"key=value;flag"});
  EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           parse_json(R"({"extensions":{"org.example/":{}}})"));
  EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(result_writer_metadata_rejections) {
  std::ostringstream stream;
  const auto bad{parse_json(
      R"({
  "resources": [],
  "_meta": {
    "io.modelcontextprotocol/serverInfo": {
      "name": "missing-version"
    }
  }
})")};
  try {
    mcp_write_result(stream, CURRENT, "resources/list", JSON{0}, bad,
                     MCPCachePolicy{});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_TRUE(stream.str().empty());
  const auto discovery{
      parse_json(R"({"supportedVersions":["2025-11-25"],"capabilities":{}})")};
  try {
    mcp_write_result(stream, CURRENT, "server/discover", JSON{0}, discovery,
                     MCPCachePolicy{});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(enum_forms_and_sampling_continuations) {
  MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  const auto inputs{parse_json(
      R"({
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
              {
                "const": "a",
                "title": "Alpha"
              },
              {
                "const": "b",
                "title": "Beta"
              }
            ]
          },
          "multiple": {
            "type": "array",
            "items": {
              "anyOf": [
                {
                  "const": "a",
                  "title": "Alpha"
                },
                {
                  "const": "b",
                  "title": "Beta"
                }
              ]
            }
          }
        }
      }
    }
  }
})")};
  EXPECT_EQ(mcp_make_input_required_result(CURRENT, "tools/call", JSON{0},
                                           inputs, std::nullopt, capabilities)
                .at("result")
                .at("inputRequests"),
            inputs);
  const auto prior{
      parse_json(R"({"sample":{"method":"sampling/createMessage"}})")};
  auto value{parameters()};
  value.assign("inputResponses", parse_json(
                                     R"({
  "sample": {
    "role": "assistant",
    "model": "model",
    "content": [
      {
        "type": "text",
        "text": "answer"
      },
      {
        "type": "tool_use",
        "id": "call-1",
        "name": "tool",
        "input": {}
      }
    ]
  }
})"));
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior));
  value.at("inputResponses")
      .at("sample")
      .at("content")
      .at(1)
      .assign("input", JSON{false});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior));
}

TEST(nested_descriptor_metadata) {
  const std::array<std::pair<JSON::StringView, JSON::StringView>, 5> cases{{
      {"resources/list", R"({"resources":[{"name":"r","uri":"file:///r"}]})"},
      {"resources/templates/list",
       R"({"resourceTemplates":[{"name":"r","uriTemplate":"file:///{name}"}]})"},
      {"resources/read",
       R"({"contents":[{"uri":"file:///r","text":"value"}]})"},
      {"prompts/list", R"({"prompts":[{"name":"p"}]})"},
      {"tools/list",
       R"({
  "tools": [
    {
      "name": "t",
      "inputSchema": {
        "type": "object",
        "properties": {
          "_meta": {
            "const": false
          }
        }
      }
    }
  ]
})"},
  }};
  const std::array<JSON::StringView, 5> keys{
      {"resources", "resourceTemplates", "contents", "prompts", "tools"}};
  for (std::size_t index = 0; index < cases.size(); ++index) {
    const auto &[method, fixture]{cases[index]};
    for (const auto *const text :
         {"false", "null", "[]", R"({"invalid key":true})",
          R"({"traceparent":false})", R"({"traceparent":"bad"})",
          R"({"tracestate":[]})", R"({"baggage":null})"}) {
      auto source{parse_json(fixture)};
      const auto meta{parse_json(text)};
      source.at(keys[index]).at(0).assign("_meta", meta);
      try {
        list_result(method, CURRENT, source.at(keys[index]));
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
      std::ostringstream stream;
      try {
        mcp_write_result(stream, CURRENT, method, JSON{1}, source,
                         MCPCachePolicy{});
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
      EXPECT_TRUE(stream.str().empty());
      if (meta.is_object()) {
        {
          const auto version{MCPProtocolVersion::V_2025_03_26};

          if (version != CURRENT) {
            EXPECT_EQ(list_result(method, version, source.at(keys[index]))
                          .at(keys[index]),
                      source.at(keys[index]));
            std::ostringstream legacy;
            mcp_write_result(legacy, version, method, JSON{1}, source,
                             std::nullopt);
            EXPECT_EQ(parse_json(legacy.str()).at("result").at(keys[index]),
                      source.at(keys[index]));
          }
        }
        {
          const auto version{MCPProtocolVersion::V_2025_06_18};

          if (version != CURRENT) {
            EXPECT_EQ(list_result(method, version, source.at(keys[index]))
                          .at(keys[index]),
                      source.at(keys[index]));
            std::ostringstream legacy;
            mcp_write_result(legacy, version, method, JSON{1}, source,
                             std::nullopt);
            EXPECT_EQ(parse_json(legacy.str()).at("result").at(keys[index]),
                      source.at(keys[index]));
          }
        }
        {
          const auto version{MCPProtocolVersion::V_2025_11_25};

          if (version != CURRENT) {
            EXPECT_EQ(list_result(method, version, source.at(keys[index]))
                          .at(keys[index]),
                      source.at(keys[index]));
            std::ostringstream legacy;
            mcp_write_result(legacy, version, method, JSON{1}, source,
                             std::nullopt);
            EXPECT_EQ(parse_json(legacy.str()).at("result").at(keys[index]),
                      source.at(keys[index]));
          }
        }
        {
          const auto version{MCPProtocolVersion::V_2026_07_28};

          if (version != CURRENT) {
            EXPECT_EQ(list_result(method, version, source.at(keys[index]))
                          .at(keys[index]),
                      source.at(keys[index]));
            std::ostringstream legacy;
            mcp_write_result(legacy, version, method, JSON{1}, source,
                             std::nullopt);
            EXPECT_EQ(parse_json(legacy.str()).at("result").at(keys[index]),
                      source.at(keys[index]));
          }
        }
      }
    }
    for (const auto *const text :
         {"{}", R"({"org.example/data":{"_meta":false}})",
          R"({
  "traceparent": "00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01",
  "tracestate": "vendor=value",
  "baggage": "key=value"
})"}) {
      auto source{parse_json(fixture)};
      source.at(keys[index]).at(0).assign("_meta", parse_json(text));
      const auto built{list_result(method, CURRENT, source.at(keys[index]))};
      std::ostringstream stream;
      mcp_write_result(stream, CURRENT, method, JSON{1}, source,
                       MCPCachePolicy{});
      EXPECT_EQ(parse_json(stream.str()).at("result"), built);
    }
  }
  const auto embedded{parse_json(
      R"([
  {
    "type": "resource",
    "resource": {
      "uri": "file:///r",
      "text": "value",
      "_meta": {
        "invalid key": true
      }
    }
  }
])")};
  try {
    mcp_make_tool_success(CURRENT, JSON{1}, JSON::make_object(), embedded);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_prompts_get_result(
        CURRENT, "",
        parse_json(
            R"([{"role":"user","content":{"type":"text","text":"value","_meta":false}}])"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  const auto business{
      parse_json(R"({"_meta":false,"nested":{"_meta":{"invalid key":true}}})")};
  EXPECT_EQ(mcp_make_tool_success(CURRENT, JSON{1}, business)
                .at("result")
                .at("structuredContent"),
            business);
}

TEST(nested_input_metadata) {
  MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.elicitation_form = true;
  capabilities.sampling = true;
  const std::array<JSON::StringView, 3> cases{{
      R"({"r":{"method":"roots/list","params":{}}})",
      R"({
  "r": {
    "method": "elicitation/create",
    "params": {
      "message": "choose",
      "requestedSchema": {
        "type": "object",
        "properties": {}
      }
    }
  }
})",
      R"({
  "r": {
    "method": "sampling/createMessage",
    "params": {
      "maxTokens": 1,
      "messages": [
        {
          "role": "user",
          "content": {
            "type": "text",
            "text": "hello"
          }
        }
      ]
    }
  }
})",
  }};
  for (const auto fixture : cases) {
    for (const auto *const text :
         {"false", "null", "[]", R"({"invalid key":true})",
          R"({"traceparent":"bad"})"}) {
      auto request{parse_json(fixture)};
      request.at("r").at("params").assign("_meta", parse_json(text));
      try {
        mcp_make_input_required_result(CURRENT, "tools/call", JSON{1}, request,
                                       std::nullopt, capabilities);
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
    }
    for (const auto *const text : {"{}", R"({"org.example/value":true})"}) {
      auto request{parse_json(fixture)};
      request.at("r").at("params").assign("_meta", parse_json(text));
      const auto result{mcp_make_input_required_result(
          CURRENT, "tools/call", JSON{1}, request, std::nullopt, capabilities)};
      EXPECT_TRUE(result.at("result").is_object());
    }
  }
  auto sampling{parse_json(cases[2])};
  auto &message{sampling.at("r").at("params").at("messages").at(0)};
  message.assign("_meta", JSON{false});
  try {
    mcp_make_input_required_result(CURRENT, "tools/call", JSON{1}, sampling,
                                   std::nullopt, capabilities);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  message.erase("_meta");
  message.at("content").assign("_meta", parse_json(R"({"invalid key":true})"));
  try {
    mcp_make_input_required_result(CURRENT, "tools/call", JSON{1}, sampling,
                                   std::nullopt, capabilities);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  message.at("content") = parse_json(
      R"({
  "type": "tool_use",
  "id": "call",
  "name": "tool",
  "input": {
    "_meta": false
  },
  "_meta": {
    "org.example/value": true
  }
})");
  message.assign("role", JSON{"assistant"});
  capabilities.sampling_tools = true;
  sampling.at("r")
      .at("params")
      .at("messages")
      .push_back(parse_json(
          R"({
  "role": "user",
  "content": {
    "type": "tool_result",
    "toolUseId": "call",
    "content": [
      {
        "type": "text",
        "text": "done"
      }
    ]
  }
})"));
  const auto result{mcp_make_input_required_result(
      CURRENT, "tools/call", JSON{1}, sampling, std::nullopt, capabilities)};
  EXPECT_TRUE(result.at("result").is_object());
}

TEST(prompt_argument_metadata) {
  for (const auto *const text :
       {"false", "null", "[]", R"({"invalid key":true})",
        R"({"traceparent":"bad"})"}) {
    auto source{
        parse_json(R"({"prompts":[{"name":"p","arguments":[{"name":"a"}]}]})")};
    const auto meta{parse_json(text)};
    source.at("prompts").at(0).at("arguments").at(0).assign("_meta", meta);
    try {
      mcp_make_prompts_list_result(CURRENT, source.at("prompts"), std::nullopt,
                                   {});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    std::ostringstream stream;
    try {
      mcp_write_result(stream, CURRENT, "prompts/list", JSON{1}, source,
                       MCPCachePolicy{});
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    EXPECT_TRUE(stream.str().empty());
    if (meta.is_object()) {
      {
        const auto version{MCPProtocolVersion::V_2025_03_26};

        if (version == CURRENT) {
          continue;
        }
        EXPECT_EQ(mcp_make_prompts_list_result(version, source.at("prompts"),
                                               std::nullopt, {})
                      .at("prompts"),
                  source.at("prompts"));
        std::ostringstream legacy;
        mcp_write_result(legacy, version, "prompts/list", JSON{1}, source,
                         std::nullopt);
        EXPECT_EQ(parse_json(legacy.str()).at("result").at("prompts"),
                  source.at("prompts"));
      }
      {
        const auto version{MCPProtocolVersion::V_2025_06_18};

        if (version == CURRENT) {
          continue;
        }
        EXPECT_EQ(mcp_make_prompts_list_result(version, source.at("prompts"),
                                               std::nullopt, {})
                      .at("prompts"),
                  source.at("prompts"));
        std::ostringstream legacy;
        mcp_write_result(legacy, version, "prompts/list", JSON{1}, source,
                         std::nullopt);
        EXPECT_EQ(parse_json(legacy.str()).at("result").at("prompts"),
                  source.at("prompts"));
      }
      {
        const auto version{MCPProtocolVersion::V_2025_11_25};

        if (version == CURRENT) {
          continue;
        }
        EXPECT_EQ(mcp_make_prompts_list_result(version, source.at("prompts"),
                                               std::nullopt, {})
                      .at("prompts"),
                  source.at("prompts"));
        std::ostringstream legacy;
        mcp_write_result(legacy, version, "prompts/list", JSON{1}, source,
                         std::nullopt);
        EXPECT_EQ(parse_json(legacy.str()).at("result").at("prompts"),
                  source.at("prompts"));
      }
      {
        const auto version{MCPProtocolVersion::V_2026_07_28};

        if (version == CURRENT) {
          continue;
        }
        EXPECT_EQ(mcp_make_prompts_list_result(version, source.at("prompts"),
                                               std::nullopt, {})
                      .at("prompts"),
                  source.at("prompts"));
        std::ostringstream legacy;
        mcp_write_result(legacy, version, "prompts/list", JSON{1}, source,
                         std::nullopt);
        EXPECT_EQ(parse_json(legacy.str()).at("result").at("prompts"),
                  source.at("prompts"));
      }
    }
  }
  const auto source{parse_json(
      R"({
  "prompts": [
    {
      "name": "p",
      "arguments": [
        {
          "name": "a",
          "_meta": {
            "org.example/value": {
              "_meta": false
            }
          }
        }
      ]
    }
  ]
})")};
  const auto result{mcp_make_prompts_list_result(CURRENT, source.at("prompts"),
                                                 std::nullopt, {})};
  std::ostringstream stream;
  mcp_write_result(stream, CURRENT, "prompts/list", JSON{1}, source,
                   MCPCachePolicy{});
  EXPECT_EQ(parse_json(stream.str()).at("result"), result);
  EXPECT_TRUE(result.is_object());
}

TEST(sampling_tool_history) {
  MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto call{parse_json(
      R"({"type":"tool_use","id":"call","name":"tool","input":{"_meta":false}})")};
  const auto answer{parse_json(
      R"({"type":"tool_result","toolUseId":"call","content":[{"type":"text","text":"done"}]})")};
  const auto valid{parse_json(
      R"([
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
    "content": {
      "type": "text",
      "text": "done"
    }
  }
])")};
  const auto result{sampling_result(valid, capabilities)};
  EXPECT_TRUE(result.at("result").is_object());
  capabilities.sampling_tools = false;
  try {
    sampling_result(valid, capabilities);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  capabilities.sampling_tools = true;
  for (
      const auto *const text :
      {R"([{"role":"user","content":{}}])",
       R"([{"role":"assistant","content":{}}])",
       R"([
  {
    "role": "assistant",
    "content": {}
  },
  {
    "role": "user",
    "content": [
      {},
      {
        "type": "text",
        "text": "mixed"
      }
    ]
  }
])",
       R"([{"role":"assistant","content":{}},{"role":"user","content":[{}, {}]}])"}) {
    auto messages{parse_json(text)};
    messages.at(0).assign("content", call);
    if (messages.size() > 1) {
      auto &content{messages.at(1).at("content")};
      if (content.is_array()) {
        content.at(0) = answer;
        if (content.at(1).empty()) {
          content.at(1) = answer;
        }
      } else {
        content = answer;
      }
    }
    try {
      sampling_result(messages, capabilities);
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  auto incomplete{valid};
  incomplete.at(1).at("content").at(1).assign("toolUseId", JSON{"unknown"});
  try {
    sampling_result(incomplete, capabilities);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  auto duplicate{valid};
  duplicate.at(0).at("content").at(1).assign("id", JSON{"a"});
  duplicate.at(1).assign(
      "content",
      parse_json(R"([{ "type":"tool_result","toolUseId":"a","content":[]}])"));
  try {
    sampling_result(duplicate, capabilities);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  // Every block is otherwise valid: only the intervening text message violates
  // the immediate tool-result requirement.
  const auto interrupted{parse_json(
      R"([
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
      "type": "text",
      "text": "interrupted"
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
])")};
  try {
    sampling_result(interrupted, capabilities);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  auto orphan{JSON::make_array()};
  auto message{JSON::make_object()};
  message.assign("role", JSON{"user"});
  message.assign("content", answer);
  orphan.push_back(std::move(message));
  try {
    sampling_result(orphan, capabilities);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(continuation_nested_metadata) {
  const auto prior{parse_json(
      R"({"roots":{"method":"roots/list"},"sample":{"method":"sampling/createMessage"}})")};
  for (const auto *const text :
       {"false", "null", "[]", R"({"invalid key":true})",
        R"({"traceparent":"bad"})"}) {
    auto value{parameters()};
    value.assign("inputResponses", parse_json(
                                       R"({
  "roots": {
    "roots": [
      {
        "uri": "file:///r"
      }
    ]
  },
  "sample": {
    "role": "assistant",
    "model": "model",
    "content": {
      "type": "text",
      "text": "hello"
    }
  }
})"));
    auto &root{value.at("inputResponses").at("roots").at("roots").at(0)};
    root.assign("_meta", parse_json(text));
    EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior));
    root.erase("_meta");
    auto &sample{value.at("inputResponses").at("sample")};
    sample.assign("_meta", parse_json(text));
    EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior));
    sample.erase("_meta");
    sample.at("content").assign("_meta", parse_json(text));
    EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior));
    sample.at("content").assign(
        "_meta", parse_json(R"({"org.example/value":{"_meta":false}})"));
    root.assign("_meta", JSON::make_object());
    EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior));
  }
}

TEST(result_writer_requires_cache_policy) {
  constexpr auto *FIXTURE{
      R"({
  "resources": [
    {
      "name": "test",
      "uri": "https://example.com/a"
    }
  ],
  "nextCursor": "",
  "_meta": {
    "example.org/value": "preserved"
  }
})"};
  const auto page{parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{parse_json(FIXTURE)};

  std::ostringstream stream;
  try {
    mcp_write_result(stream, CURRENT, "resources/list", JSON{0}, page,
                     std::nullopt);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(result_writer_rejects_non_cacheable_method) {
  constexpr auto *FIXTURE{
      R"({
  "resources": [
    {
      "name": "test",
      "uri": "https://example.com/a"
    }
  ],
  "nextCursor": "",
  "_meta": {
    "example.org/value": "preserved"
  }
})"};
  const auto page{parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{parse_json(FIXTURE)};

  std::ostringstream stream;
  try {
    mcp_write_result(stream, CURRENT, "tools/call", JSON{0}, page,
                     MCPCachePolicy{});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

// The initialize parameters are an Object, so a request carrying any other
// JSON value there is as malformed as one carrying none
TEST(make_initialize_result_returns_invalid_request_when_params_is_an_array) {
  const auto request{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "initialize", "params": []
  })JSON")};
  const sourcemeta::core::MCPServerCapabilities capabilities;
  const sourcemeta::core::MCPImplementation server{.name = "srv",
                                                   .version = "1.0.0",
                                                   .title = {},
                                                   .description = {},
                                                   .website_url = {}};
  const auto envelope{sourcemeta::core::mcp_make_initialize_result(
      request, capabilities, server, SUPPORTED_VERSIONS)};
  EXPECT_EQ(envelope.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_REQUEST);
}
