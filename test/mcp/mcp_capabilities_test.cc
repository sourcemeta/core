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

TEST(capability_overrides) {
  using namespace sourcemeta::core;
  constexpr auto LEGACY{MCPProtocolVersion::V_2025_11_25};
  auto client{mcp_parse_client_capabilities(LEGACY, parse_json(
                                                        R"({
  "roots": {
    "listChanged": true,
    "custom": {}
  },
  "sampling": {
    "context": {
      "custom": true
    },
    "tools": {}
  },
  "elicitation": {
    "form": {},
    "url": {}
  },
  "experimental": {
    "custom": {}
  }
})"))};
  client.roots_list_changed = false;
  client.sampling_context = false;
  client.sampling_tools = false;
  client.elicitation_form = false;
  client.elicitation_url = false;
  client.experimental.reset();
  const auto edited{mcp_serialize_client_capabilities(LEGACY, client)};
  EXPECT_FALSE(edited.at("roots").defines("listChanged"));
  EXPECT_TRUE(edited.at("roots").defines("custom"));
  EXPECT_TRUE(edited.at("sampling").empty());
  EXPECT_TRUE(edited.at("elicitation").empty());
  EXPECT_FALSE(edited.defines("experimental"));
  const auto downgraded{mcp_serialize_client_capabilities(
      MCPProtocolVersion::V_2025_06_18,
      mcp_parse_client_capabilities(
          LEGACY,
          parse_json(R"({"elicitation":{"form":{},"url":{},"custom":{}}})")))};
  EXPECT_EQ(downgraded.at("elicitation"), parse_json(R"({"custom":{}})"));
  auto modern{mcp_parse_client_capabilities(
      MCPProtocolVersion::V_2026_07_28,
      parse_json(
          R"({"extensions":{"org.example/":{}},"experimental":{"custom":{}}})"))};
  modern.extensions.reset();
  modern.experimental.reset();
  EXPECT_TRUE(mcp_serialize_client_capabilities(
                  MCPProtocolVersion::V_2026_07_28, modern)
                  .empty());
}

TEST(server_capability_roundtrip_and_overrides) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2025_11_25};
  const auto source{parse_json(
      R"({
  "resources": {
    "subscribe": true,
    "listChanged": false,
    "custom": {}
  },
  "tools": {
    "listChanged": true
  },
  "prompts": {},
  "logging": {
    "custom": {}
  },
  "completions": {},
  "experimental": {
    "custom": {}
  },
  "tasks": {
    "list": {}
  },
  "org.example/custom": {}
})")};
  auto parsed{mcp_parse_server_capabilities(VERSION, source)};
  EXPECT_EQ(mcp_serialize_server_capabilities(VERSION, parsed), source);
  parsed.resources_subscribe = false;
  parsed.tools = false;
  parsed.tools_list_changed = false;
  parsed.experimental.reset();
  const auto result{mcp_serialize_server_capabilities(VERSION, parsed)};
  EXPECT_FALSE(result.at("resources").defines("subscribe"));
  EXPECT_TRUE(result.at("resources").defines("custom"));
  EXPECT_FALSE(result.defines("tools"));
  EXPECT_FALSE(result.defines("experimental"));
  EXPECT_TRUE(result.defines("tasks"));
  const auto modern{mcp_serialize_server_capabilities(
      MCPProtocolVersion::V_2026_07_28, parsed)};
  EXPECT_FALSE(modern.defines("tasks"));
  EXPECT_TRUE(modern.defines("org.example/custom"));
}

TEST(capability_settings_roundtrip) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  const auto source{parse_json(
      R"({
  "roots": {
    "listChanged": false,
    "settings": {
      "name": "root"
    }
  },
  "sampling": {
    "tools": {
      "option": "value"
    }
  },
  "elicitation": {
    "form": {
      "setting": true
    }
  },
  "tasks": {
    "requests": {
      "sampling": {
        "createMessage": {}
      }
    }
  },
  "example.org/custom": {
    "value": true
  }
})")};
  EXPECT_EQ(mcp_serialize_client_capabilities(
                version, mcp_parse_client_capabilities(version, source)),
            source);
  EXPECT_TRUE(source.is_object());
  for (const auto *const text :
       {R"({"tasks":false})", R"({"tasks":{"list":false}})",
        R"({"tasks":{"requests":{"sampling":{"createMessage":true}}}})"}) {
    try {
      mcp_parse_client_capabilities(version, parse_json(text));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
}

TEST(capabilities_serializer_rejects_extensions_string) {
  MCPClientCapabilities capabilities;
  capabilities.extensions = JSON{"invalid"};
  try {
    mcp_serialize_client_capabilities(MCPProtocolVersion::V_2026_07_28,
                                      capabilities);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(capabilities_serializer_rejects_experimental_boolean) {
  MCPClientCapabilities capabilities;
  capabilities.experimental = JSON{false};
  try {
    mcp_serialize_client_capabilities(MCPProtocolVersion::V_2026_07_28,
                                      capabilities);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}
