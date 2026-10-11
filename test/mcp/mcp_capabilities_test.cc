#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/json.h>

#include <sourcemeta/core/test.h>

#include "mcp_helpers.h"

TEST(parse_client_capabilities_empty) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(R"JSON({})JSON"))};
  EXPECT_FALSE(capabilities.roots);
  EXPECT_FALSE(capabilities.roots_list_changed);
  EXPECT_FALSE(capabilities.sampling);
  EXPECT_FALSE(capabilities.sampling_context);
  EXPECT_FALSE(capabilities.sampling_tools);
  EXPECT_FALSE(capabilities.elicitation);
  EXPECT_FALSE(capabilities.elicitation_form);
  EXPECT_FALSE(capabilities.elicitation_url);
  EXPECT_FALSE(capabilities.extensions.has_value());
  EXPECT_FALSE(capabilities.experimental.has_value());
  EXPECT_EQ(capabilities.source.value(),
            sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(parse_client_capabilities_roots_with_list_changed_2025_11_25) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "roots": { "listChanged": true } })JSON"))};
  EXPECT_TRUE(capabilities.roots);
  EXPECT_TRUE(capabilities.roots_list_changed);
  EXPECT_FALSE(capabilities.sampling);
  EXPECT_FALSE(capabilities.sampling_context);
  EXPECT_FALSE(capabilities.sampling_tools);
  EXPECT_FALSE(capabilities.elicitation);
  EXPECT_FALSE(capabilities.elicitation_form);
  EXPECT_FALSE(capabilities.elicitation_url);
  EXPECT_FALSE(capabilities.extensions.has_value());
  EXPECT_FALSE(capabilities.experimental.has_value());
}

// Roots lost its list-changed notification in 2026-07-28, where the capability
// is an empty object
// https://modelcontextprotocol.io/specification/2026-07-28/schema#clientcapabilities
TEST(parse_client_capabilities_roots_list_changed_ignored_on_2026_07_28) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({ "roots": { "listChanged": true } })JSON"))};
  EXPECT_TRUE(capabilities.roots);
  EXPECT_FALSE(capabilities.roots_list_changed);
}

TEST(parse_client_capabilities_sampling_context_and_tools_2025_11_25) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "sampling": { "context": {}, "tools": {} } })JSON"))};
  EXPECT_FALSE(capabilities.roots);
  EXPECT_FALSE(capabilities.roots_list_changed);
  EXPECT_TRUE(capabilities.sampling);
  EXPECT_TRUE(capabilities.sampling_context);
  EXPECT_TRUE(capabilities.sampling_tools);
  EXPECT_FALSE(capabilities.elicitation);
  EXPECT_FALSE(capabilities.elicitation_form);
  EXPECT_FALSE(capabilities.elicitation_url);
}

TEST(parse_client_capabilities_sampling_sub_flags_ignored_on_2025_06_18) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::parse_json(
          R"JSON({ "sampling": { "context": {}, "tools": {} } })JSON"))};
  EXPECT_TRUE(capabilities.sampling);
  EXPECT_FALSE(capabilities.sampling_context);
  EXPECT_FALSE(capabilities.sampling_tools);
}

TEST(parse_client_capabilities_elicitation_form_and_url_2025_11_25) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "elicitation": { "form": {}, "url": {} } })JSON"))};
  EXPECT_TRUE(capabilities.elicitation);
  EXPECT_TRUE(capabilities.elicitation_form);
  EXPECT_TRUE(capabilities.elicitation_url);
}

TEST(parse_client_capabilities_elicitation_sub_flags_ignored_on_2025_06_18) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
      sourcemeta::core::parse_json(
          R"JSON({ "elicitation": { "form": {}, "url": {} } })JSON"))};
  EXPECT_TRUE(capabilities.elicitation);
  EXPECT_FALSE(capabilities.elicitation_form);
  EXPECT_FALSE(capabilities.elicitation_url);
}

// Elicitation was introduced in 2025-06-18
// https://modelcontextprotocol.io/specification/2025-06-18/client/elicitation
TEST(parse_client_capabilities_elicitation_ignored_on_2025_03_26) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
      sourcemeta::core::parse_json(R"JSON({ "elicitation": {} })JSON"))};
  EXPECT_FALSE(capabilities.elicitation);
  EXPECT_FALSE(capabilities.elicitation_form);
  EXPECT_FALSE(capabilities.elicitation_url);
}

TEST(parse_client_capabilities_extensions_2026_07_28) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({ "extensions": { "org.example/thing": {} } })JSON"))};
  EXPECT_TRUE(capabilities.extensions.has_value());
  EXPECT_EQ(
      capabilities.extensions.value(),
      sourcemeta::core::parse_json(R"JSON({ "org.example/thing": {} })JSON"));
}

TEST(parse_client_capabilities_extensions_ignored_on_2025_11_25) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "extensions": { "org.example/thing": {} } })JSON"))};
  EXPECT_FALSE(capabilities.extensions.has_value());
}

TEST(parse_client_capabilities_experimental) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({ "experimental": { "custom": {} } })JSON"))};
  EXPECT_TRUE(capabilities.experimental.has_value());
  EXPECT_EQ(capabilities.experimental.value(),
            sourcemeta::core::parse_json(R"JSON({ "custom": {} })JSON"));
}

TEST(parse_client_capabilities_preserves_opaque_settings_in_source) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "roots": { "settings": { "name": "root" } } })JSON"))};
  EXPECT_EQ(capabilities.source.value(), sourcemeta::core::parse_json(
                                             R"JSON({
                                               "roots": {
                                                 "settings": {
                                                   "name": "root"
                                                 }
                                               }
                                             })JSON"));
}

TEST(parse_client_capabilities_rejects_non_object) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_parse_client_capabilities(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([])JSON")),
                     "Invalid client capabilities");
}

TEST(parse_client_capabilities_rejects_non_object_roots) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(R"JSON({ "roots": true })JSON")),
      "Invalid client capabilities");
}

TEST(parse_client_capabilities_rejects_non_boolean_roots_list_changed) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_parse_client_capabilities(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::parse_json(
                             R"JSON({ "roots": { "listChanged": 1 } })JSON")),
                     "Invalid client capabilities");
}

TEST(parse_client_capabilities_rejects_non_object_tasks) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::parse_json(R"JSON({ "tasks": false })JSON")),
      "Invalid client capabilities");
}

TEST(parse_client_capabilities_rejects_non_object_tasks_list) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_parse_client_capabilities(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::parse_json(
                             R"JSON({ "tasks": { "list": false } })JSON")),
                     "Invalid client capabilities");
}

TEST(parse_client_capabilities_rejects_non_object_tasks_create_message) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_parse_client_capabilities(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "tasks": {
                                 "requests": {
                                   "sampling": { "createMessage": true }
                                 }
                               }
                             })JSON")),
                     "Invalid client capabilities");
}

TEST(parse_client_capabilities_rejects_non_object_experimental_entry) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_parse_client_capabilities(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "experimental": { "custom": 1 }
                             })JSON")),
                     "Invalid client capabilities");
}

// Extension capability keys follow the `_meta` key rules with a mandatory
// prefix
// https://modelcontextprotocol.io/specification/2026-07-28/schema#clientcapabilities
TEST(parse_client_capabilities_rejects_unprefixed_extension_key) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_parse_client_capabilities(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON({ "extensions": { "thing": {} } })JSON")),
                     "Invalid client capabilities");
}

TEST(serialize_client_capabilities_empty) {
  const sourcemeta::core::MCPClientCapabilities capabilities;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(serialize_client_capabilities_roots_2026_07_28) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(R"JSON({ "roots": {} })JSON"));
}

TEST(serialize_client_capabilities_roots_list_changed_2025_11_25) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.roots_list_changed = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "roots": { "listChanged": true } })JSON"));
}

TEST(serialize_client_capabilities_roots_list_changed_dropped_on_2026_07_28) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.roots_list_changed = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(R"JSON({ "roots": {} })JSON"));
}

TEST(serialize_client_capabilities_sampling_2025_11_25) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_context = true;
  capabilities.sampling_tools = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "sampling": { "context": {}, "tools": {} } })JSON"));
}

TEST(serialize_client_capabilities_sampling_sub_flags_dropped_on_2025_06_18) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_context = true;
  capabilities.sampling_tools = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, capabilities),
      sourcemeta::core::parse_json(R"JSON({ "sampling": {} })JSON"));
}

TEST(serialize_client_capabilities_elicitation_2025_11_25) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  capabilities.elicitation_url = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "elicitation": { "form": {}, "url": {} } })JSON"));
}

TEST(serialize_client_capabilities_elicitation_dropped_on_2025_03_26) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_03_26, capabilities),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(serialize_client_capabilities_extensions_2026_07_28) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.extensions =
      sourcemeta::core::parse_json(R"JSON({ "org.example/thing": {} })JSON");
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "extensions": { "org.example/thing": {} } })JSON"));
}

TEST(serialize_client_capabilities_extensions_dropped_on_2025_11_25) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.extensions =
      sourcemeta::core::parse_json(R"JSON({ "org.example/thing": {} })JSON");
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(serialize_client_capabilities_experimental) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.experimental =
      sourcemeta::core::parse_json(R"JSON({ "custom": {} })JSON");
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "experimental": { "custom": {} } })JSON"));
}

TEST(serialize_client_capabilities_preserves_opaque_roots_settings) {
  auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "roots": { "listChanged": true, "custom": {} } })JSON"))};
  capabilities.roots_list_changed = false;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(R"JSON({ "roots": { "custom": {} } })JSON"));
}

TEST(serialize_client_capabilities_clearing_sampling_sub_flags) {
  auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "sampling": { "context": {}, "tools": {} } })JSON"))};
  capabilities.sampling_context = false;
  capabilities.sampling_tools = false;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(R"JSON({ "sampling": {} })JSON"));
}

TEST(serialize_client_capabilities_clearing_elicitation_sub_flags) {
  auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "elicitation": { "form": {}, "url": {} } })JSON"))};
  capabilities.elicitation_form = false;
  capabilities.elicitation_url = false;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(R"JSON({ "elicitation": {} })JSON"));
}

TEST(serialize_client_capabilities_clearing_experimental) {
  auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "experimental": { "custom": {} } })JSON"))};
  capabilities.experimental.reset();
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(serialize_client_capabilities_downgrade_drops_elicitation_sub_flags) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({
            "elicitation": { "form": {}, "url": {}, "custom": {} }
          })JSON"))};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "elicitation": { "custom": {} } })JSON"));
}

TEST(serialize_client_capabilities_roundtrip_2025_11_25) {
  const auto source{sourcemeta::core::parse_json(
      R"JSON({
        "roots": {
          "listChanged": false,
          "settings": { "name": "root" }
        },
        "sampling": { "tools": { "option": "value" } },
        "elicitation": { "form": { "setting": true } },
        "tasks": {
          "requests": { "sampling": { "createMessage": {} } }
        },
        "example.org/custom": { "value": true }
      })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::mcp_parse_client_capabilities(
              sourcemeta::core::MCPProtocolVersion::V_2025_11_25, source)),
      source);
}

// Tasks were a 2025-11-25 capability and moved to an extension in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/deprecated
TEST(serialize_client_capabilities_drops_tasks_on_2026_07_28) {
  const auto capabilities{sourcemeta::core::mcp_parse_client_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({
            "tasks": { "list": {} },
            "org.example/custom": {}
          })JSON"))};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(R"JSON({ "org.example/custom": {} })JSON"));
}

TEST(serialize_client_capabilities_rejects_non_object_source) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.source = sourcemeta::core::JSON{true};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid capability source");
}

TEST(serialize_client_capabilities_rejects_string_extensions) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.extensions = sourcemeta::core::JSON{"invalid"};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid extension capabilities");
}

TEST(serialize_client_capabilities_rejects_boolean_experimental) {
  sourcemeta::core::MCPClientCapabilities capabilities;
  capabilities.experimental = sourcemeta::core::JSON{false};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_serialize_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid experimental capabilities");
}

TEST(parse_server_capabilities_empty) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(R"JSON({})JSON"))};
  EXPECT_FALSE(capabilities.prompts);
  EXPECT_FALSE(capabilities.resources);
  EXPECT_FALSE(capabilities.tools);
  EXPECT_FALSE(capabilities.logging);
  EXPECT_FALSE(capabilities.completions);
  EXPECT_FALSE(capabilities.tools_list_changed);
  EXPECT_FALSE(capabilities.resources_subscribe);
  EXPECT_FALSE(capabilities.resources_list_changed);
  EXPECT_FALSE(capabilities.prompts_list_changed);
  EXPECT_FALSE(capabilities.extensions.has_value());
  EXPECT_FALSE(capabilities.experimental.has_value());
  EXPECT_EQ(capabilities.source.value(),
            sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(parse_server_capabilities_all_features) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({
            "prompts": { "listChanged": true },
            "resources": { "subscribe": true, "listChanged": true },
            "tools": { "listChanged": true },
            "logging": {},
            "completions": {}
          })JSON"))};
  EXPECT_TRUE(capabilities.prompts);
  EXPECT_TRUE(capabilities.resources);
  EXPECT_TRUE(capabilities.tools);
  EXPECT_TRUE(capabilities.logging);
  EXPECT_TRUE(capabilities.completions);
  EXPECT_TRUE(capabilities.tools_list_changed);
  EXPECT_TRUE(capabilities.resources_subscribe);
  EXPECT_TRUE(capabilities.resources_list_changed);
  EXPECT_TRUE(capabilities.prompts_list_changed);
}

TEST(parse_server_capabilities_explicit_false_sub_flags) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({
            "resources": { "subscribe": false, "listChanged": false },
            "tools": { "listChanged": false }
          })JSON"))};
  EXPECT_TRUE(capabilities.resources);
  EXPECT_TRUE(capabilities.tools);
  EXPECT_FALSE(capabilities.resources_subscribe);
  EXPECT_FALSE(capabilities.resources_list_changed);
  EXPECT_FALSE(capabilities.tools_list_changed);
}

TEST(parse_server_capabilities_extensions_2026_07_28) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({ "extensions": { "org.example/thing": {} } })JSON"))};
  EXPECT_TRUE(capabilities.extensions.has_value());
  EXPECT_EQ(
      capabilities.extensions.value(),
      sourcemeta::core::parse_json(R"JSON({ "org.example/thing": {} })JSON"));
}

TEST(parse_server_capabilities_extensions_ignored_on_2025_11_25) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(
          R"JSON({ "extensions": { "org.example/thing": {} } })JSON"))};
  EXPECT_FALSE(capabilities.extensions.has_value());
}

TEST(parse_server_capabilities_rejects_non_object) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_parse_server_capabilities(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(R"JSON([])JSON")),
                     "Invalid server capabilities");
}

TEST(parse_server_capabilities_rejects_non_object_tools) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(R"JSON({ "tools": true })JSON")),
      "Invalid server capabilities");
}

TEST(parse_server_capabilities_rejects_non_boolean_list_changed) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_parse_server_capabilities(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::parse_json(
                             R"JSON({ "tools": { "listChanged": 1 } })JSON")),
                     "Invalid server capabilities");
}

TEST(parse_server_capabilities_rejects_non_boolean_subscribe) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::parse_json(
              R"JSON({ "resources": { "subscribe": "yes" } })JSON")),
      "Invalid server capabilities");
}

TEST(serialize_server_capabilities_empty) {
  const sourcemeta::core::MCPServerCapabilities capabilities;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(serialize_server_capabilities_all_features) {
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.prompts = true;
  capabilities.prompts_list_changed = true;
  capabilities.resources = true;
  capabilities.resources_subscribe = true;
  capabilities.resources_list_changed = true;
  capabilities.tools = true;
  capabilities.tools_list_changed = true;
  capabilities.logging = true;
  capabilities.completions = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({
            "prompts": { "listChanged": true },
            "resources": { "listChanged": true, "subscribe": true },
            "tools": { "listChanged": true },
            "logging": {},
            "completions": {}
          })JSON"));
}

TEST(serialize_server_capabilities_without_sub_flags) {
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.prompts = true;
  capabilities.resources = true;
  capabilities.tools = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "prompts": {}, "resources": {}, "tools": {} })JSON"));
}

TEST(serialize_server_capabilities_list_changed_without_the_feature) {
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.tools_list_changed = true;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "tools": { "listChanged": true } })JSON"));
}

TEST(serialize_server_capabilities_preserves_explicit_false_list_changed) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({ "tools": { "listChanged": false } })JSON"))};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "tools": { "listChanged": false } })JSON"));
}

TEST(serialize_server_capabilities_clearing_subscribe_preserves_settings) {
  auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({ "resources": { "subscribe": true, "custom": {} } })JSON"))};
  capabilities.resources_subscribe = false;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "resources": { "custom": {} } })JSON"));
}

TEST(serialize_server_capabilities_clearing_a_feature_drops_it) {
  auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({ "tools": { "listChanged": true } })JSON"))};
  capabilities.tools = false;
  capabilities.tools_list_changed = false;
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(serialize_server_capabilities_preserves_unknown_keys) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(
          R"JSON({ "org.example/custom": { "value": true } })JSON"))};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "org.example/custom": { "value": true } })JSON"));
}

TEST(serialize_server_capabilities_keeps_tasks_on_2025_11_25) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(R"JSON({ "tasks": { "list": {} } })JSON"))};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(R"JSON({ "tasks": { "list": {} } })JSON"));
}

TEST(serialize_server_capabilities_drops_tasks_on_2026_07_28) {
  const auto capabilities{sourcemeta::core::mcp_parse_server_capabilities(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      sourcemeta::core::parse_json(R"JSON({ "tasks": { "list": {} } })JSON"))};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(serialize_server_capabilities_extensions_2026_07_28) {
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.extensions =
      sourcemeta::core::parse_json(R"JSON({ "org.example/thing": {} })JSON");
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "extensions": { "org.example/thing": {} } })JSON"));
}

TEST(serialize_server_capabilities_extensions_dropped_on_2025_11_25) {
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.extensions =
      sourcemeta::core::parse_json(R"JSON({ "org.example/thing": {} })JSON");
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, capabilities),
      sourcemeta::core::parse_json(R"JSON({})JSON"));
}

TEST(serialize_server_capabilities_experimental) {
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.experimental =
      sourcemeta::core::parse_json(R"JSON({ "custom": {} })JSON");
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      sourcemeta::core::parse_json(
          R"JSON({ "experimental": { "custom": {} } })JSON"));
}

TEST(serialize_server_capabilities_roundtrip_2025_11_25) {
  const auto source{sourcemeta::core::parse_json(
      R"JSON({
        "resources": {
          "subscribe": true,
          "listChanged": false,
          "custom": {}
        },
        "tools": { "listChanged": true },
        "prompts": {},
        "logging": { "custom": {} },
        "completions": {},
        "experimental": { "custom": {} },
        "tasks": { "list": {} },
        "org.example/custom": {}
      })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::mcp_parse_server_capabilities(
              sourcemeta::core::MCPProtocolVersion::V_2025_11_25, source)),
      source);
}

TEST(serialize_server_capabilities_rejects_non_object_source) {
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.source = sourcemeta::core::JSON{true};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid capability source");
}

TEST(serialize_server_capabilities_rejects_string_experimental) {
  sourcemeta::core::MCPServerCapabilities capabilities;
  capabilities.experimental = sourcemeta::core::JSON{"invalid"};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_serialize_server_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid capability map");
}
