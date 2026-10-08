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

constexpr auto CURRENT{MCPProtocolVersion::V_2026_07_28};

} // namespace

TEST(code_resource_not_found) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
            static_cast<std::int64_t>(-32002));
}

TEST(code_url_elicitation_required) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_URL_ELICITATION_REQUIRED,
            static_cast<std::int64_t>(-32042));
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

TEST(errors_and_transport_context_2025_03_26) {
  const JSON bad{true};

  const auto version{MCPProtocolVersion::V_2025_03_26};

  const auto transport{
      mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad",
                     std::nullopt, MCPErrorContext::Transport)};
  if (mcp_protocol_version_at_least(version,
                                    MCPProtocolVersion::V_2025_11_25)) {
    EXPECT_FALSE(transport.defines("id"));
  } else {
    EXPECT_TRUE(transport.at("id").is_null());
    try {
      mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad");
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  const auto idless{mcp_make_error(version, nullptr,
                                   JSONRPC_CODE_INVALID_REQUEST, "bad",
                                   std::nullopt, MCPErrorContext::Transport)};
  EXPECT_FALSE(idless.defines("id"));

  try {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_RESOURCE_NOT_FOUND, "retired");
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_URL_ELICITATION_REQUIRED,
                   "retired");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(errors_and_transport_context_2025_06_18) {
  const JSON bad{true};

  const auto version{MCPProtocolVersion::V_2025_06_18};

  const auto transport{
      mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad",
                     std::nullopt, MCPErrorContext::Transport)};
  if (mcp_protocol_version_at_least(version,
                                    MCPProtocolVersion::V_2025_11_25)) {
    EXPECT_FALSE(transport.defines("id"));
  } else {
    EXPECT_TRUE(transport.at("id").is_null());
    try {
      mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad");
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  const auto idless{mcp_make_error(version, nullptr,
                                   JSONRPC_CODE_INVALID_REQUEST, "bad",
                                   std::nullopt, MCPErrorContext::Transport)};
  EXPECT_FALSE(idless.defines("id"));

  try {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_RESOURCE_NOT_FOUND, "retired");
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_URL_ELICITATION_REQUIRED,
                   "retired");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(errors_and_transport_context_2025_11_25) {
  const JSON bad{true};

  const auto version{MCPProtocolVersion::V_2025_11_25};

  const auto transport{
      mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad",
                     std::nullopt, MCPErrorContext::Transport)};
  if (mcp_protocol_version_at_least(version,
                                    MCPProtocolVersion::V_2025_11_25)) {
    EXPECT_FALSE(transport.defines("id"));
  } else {
    EXPECT_TRUE(transport.at("id").is_null());
    try {
      mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad");
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  const auto idless{mcp_make_error(version, nullptr,
                                   JSONRPC_CODE_INVALID_REQUEST, "bad",
                                   std::nullopt, MCPErrorContext::Transport)};
  EXPECT_FALSE(idless.defines("id"));

  try {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_RESOURCE_NOT_FOUND, "retired");
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_URL_ELICITATION_REQUIRED,
                   "retired");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(errors_and_transport_context_2026_07_28) {
  const JSON bad{true};

  const auto version{MCPProtocolVersion::V_2026_07_28};

  const auto transport{
      mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad",
                     std::nullopt, MCPErrorContext::Transport)};
  if (mcp_protocol_version_at_least(version,
                                    MCPProtocolVersion::V_2025_11_25)) {
    EXPECT_FALSE(transport.defines("id"));
  } else {
    EXPECT_TRUE(transport.at("id").is_null());
    try {
      mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad");
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  const auto idless{mcp_make_error(version, nullptr,
                                   JSONRPC_CODE_INVALID_REQUEST, "bad",
                                   std::nullopt, MCPErrorContext::Transport)};
  EXPECT_FALSE(idless.defines("id"));

  try {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_RESOURCE_NOT_FOUND, "retired");
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_URL_ELICITATION_REQUIRED,
                   "retired");
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(reserved_error_contracts) {
  const JSON identifier{1};
  for (std::int64_t code = -32099; code <= -32020; ++code) {
    if (code != MCP_CODE_HEADER_MISMATCH &&
        code != MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY &&
        code != MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION) {
      try {
        mcp_make_error(CURRENT, &identifier, code, "undefined");
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
    }
    {
      const auto version{MCPProtocolVersion::V_2025_03_26};

      if (version != CURRENT) {
        EXPECT_EQ(mcp_make_error(version, &identifier, code, "legacy")
                      .at("error")
                      .at("code"),
                  JSON{code});
      }
    }
    {
      const auto version{MCPProtocolVersion::V_2025_06_18};

      if (version != CURRENT) {
        EXPECT_EQ(mcp_make_error(version, &identifier, code, "legacy")
                      .at("error")
                      .at("code"),
                  JSON{code});
      }
    }
    {
      const auto version{MCPProtocolVersion::V_2025_11_25};

      if (version != CURRENT) {
        EXPECT_EQ(mcp_make_error(version, &identifier, code, "legacy")
                      .at("error")
                      .at("code"),
                  JSON{code});
      }
    }
    {
      const auto version{MCPProtocolVersion::V_2026_07_28};

      if (version != CURRENT) {
        EXPECT_EQ(mcp_make_error(version, &identifier, code, "legacy")
                      .at("error")
                      .at("code"),
                  JSON{code});
      }
    }
  }
  for (const auto code :
       {-32100, -32019, -32600, -32601, -32602, -32603, -32700, 1}) {
    EXPECT_EQ(
        mcp_make_error(CURRENT, &identifier, code, "application or JSON-RPC")
            .at("error")
            .at("code"),
        JSON{code});
  }
  for (const auto code : {MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
                          MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY}) {
    try {
      mcp_make_error(CURRENT, &identifier, code, "missing");
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
    for (const auto *const text : {"null", "false", "[]", "{}"}) {
      try {
        mcp_make_error(CURRENT, &identifier, code, "bad", parse_json(text));
        FAIL();
      } catch (const std::invalid_argument &) {
        // Invalid input is expected to be rejected
      }
    }
  }
  for (const auto *const text : {R"({"requested":false,"supported":[]})",
                                 R"({"requested":"x","supported":false})",
                                 R"({"requested":"x","supported":[1]})"}) {
    try {
      mcp_make_error(CURRENT, &identifier,
                     MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION, "bad",
                     parse_json(text));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  for (const auto *const text :
       {R"({"requiredCapabilities":false})",
        R"({"requiredCapabilities":{"roots":false}})",
        R"({"requiredCapabilities":{"extensions":{"invalid key":{}}}})"}) {
    try {
      mcp_make_error(CURRENT, &identifier,
                     MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY, "bad",
                     parse_json(text));
      FAIL();
    } catch (const std::invalid_argument &) {
      // Invalid input is expected to be rejected
    }
  }
  EXPECT_TRUE(
      mcp_make_error(CURRENT, &identifier,
                     MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION, "Unsupported",
                     parse_json(R"({"requested":"future","supported":[]})"))
          .is_object());
  EXPECT_TRUE(
      mcp_make_error(CURRENT, &identifier,
                     MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY, "Missing",
                     parse_json(R"({"requiredCapabilities":{"roots":{}}})"))
          .is_object());
  EXPECT_TRUE(
      mcp_make_error(CURRENT, &identifier, MCP_CODE_HEADER_MISMATCH, "Mismatch")
          .is_object());
}
