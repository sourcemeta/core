#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/test.h>

#include "mcp_helpers.h"

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <string>  // std::string
#include <utility> // std::pair

namespace {
using namespace sourcemeta::core;

// Explicit expectations, rather than deriving the oracle from feature helpers.
// The 2026 revision introduces request metadata and multi-round-trip responses.
constexpr std::array<std::pair<MCPProtocolVersion, JSON::StringView>, 4>
    VERSIONS{{
        {MCPProtocolVersion::V_2025_03_26, "2025-03-26"},
        {MCPProtocolVersion::V_2025_06_18, "2025-06-18"},
        {MCPProtocolVersion::V_2025_11_25, "2025-11-25"},
        {MCPProtocolVersion::V_2026_07_28, "2026-07-28"},
    }};

// Valid modern parameters are shared only as fixture construction, not an
// oracle.
auto parameters() -> JSON {
  return parse_json(R"JSON({"_meta": {
    "io.modelcontextprotocol/protocolVersion": "2026-07-28",
    "io.modelcontextprotocol/clientCapabilities": {}
  }, "requestState": "state"})JSON");
}
} // namespace

TEST(conformance_request_metadata_version_matrix) {
  for (std::size_t index{0}; index < VERSIONS.size(); ++index) {
    auto input{parameters()};
    input.at("_meta").assign("io.modelcontextprotocol/protocolVersion",
                             JSON{VERSIONS[index].second});
    EXPECT_EQ(mcp_validate_request_parameters(input).first,
              index == 3 ? MCPRequestMetaStatus::Valid
                         : MCPRequestMetaStatus::UnsupportedProtocolVersion);
  }
}

TEST(conformance_continuation_version_matrix) {
  for (std::size_t index{0}; index < VERSIONS.size(); ++index) {
    EXPECT_EQ(mcp_validate_continuation(VERSIONS[index].first, parameters(),
                                        JSON::make_object(), "state"),
              index == 3);
  }
}

TEST(conformance_parameter_headers_reject_legacy_versions) {
  const JSON arguments{JSON::make_object()};
  for (std::size_t index{0}; index < 3; ++index) {
    EXPECT_MCP_REJECTS(mcp_validate_parameter_headers(
                           VERSIONS[index].first, JSON{1}, {}, arguments, {}),
                       "Parameter headers require MCP 2026-07-28");
  }
}

// A byte-exact echo is required: matching tokens pass and every other token in
// the corpus fails, including the distinction between absent and empty state.
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr
TEST(conformance_continuation_state_echo_property) {
  constexpr std::array<JSON::StringView, 5> TOKENS{
      {"", "state", "STATE", " state ", "état"}};
  for (const auto issued : TOKENS) {
    for (const auto returned : TOKENS) {
      auto input{parameters()};
      input.assign("requestState", JSON{returned});
      EXPECT_EQ(mcp_validate_continuation(MCPProtocolVersion::V_2026_07_28,
                                          input, JSON::make_object(), issued),
                issued == returned);
    }
  }
}

TEST(conformance_continuation_rejects_unsolicited_state_property) {
  for (const auto *const token : {"", "state", "STATE"}) {
    auto input{parameters()};
    input.assign("requestState", JSON{token});
    EXPECT_FALSE(mcp_validate_continuation(MCPProtocolVersion::V_2026_07_28,
                                           input, JSON::make_object(),
                                           std::nullopt));
  }
}

// Encoding must preserve every printable ASCII byte, including surrounding
// spaces. A changed body is a negative control for each generated fixture.
TEST(conformance_parameter_header_round_trip_property) {
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "value", .path = {"value"}, .type = JSON::Type::String}}};
  for (int character{32}; character <= 126; ++character) {
    const std::string value(3, static_cast<char>(character));
    auto arguments{JSON::make_object()};
    arguments.assign("value", JSON{value});
    const auto owned{mcp_make_parameter_headers(descriptors, arguments)};
    const std::array<std::pair<JSON::StringView, JSON::StringView>, 1> headers{
        {{"mCp-pArAm-VaLuE", owned.front().second}}};
    EXPECT_FALSE(
        mcp_validate_parameter_headers(MCPProtocolVersion::V_2026_07_28,
                                       JSON{1}, descriptors, arguments, headers)
            .has_value());
    arguments.assign("value", JSON{value + "changed"});
    EXPECT_TRUE(mcp_validate_parameter_headers(MCPProtocolVersion::V_2026_07_28,
                                               JSON{1}, descriptors, arguments,
                                               headers)
                    .has_value());
  }
}

TEST(conformance_parameter_header_unicode_round_trip_property) {
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "value", .path = {"value"}, .type = JSON::Type::String}}};
  for (const auto *const value : {"café", "日本語", "🙂", " café "}) {
    auto arguments{JSON::make_object()};
    arguments.assign("value", JSON{value});
    const auto owned{mcp_make_parameter_headers(descriptors, arguments)};
    const std::array<std::pair<JSON::StringView, JSON::StringView>, 1> headers{
        {{owned.front().first, owned.front().second}}};
    EXPECT_FALSE(
        mcp_validate_parameter_headers(MCPProtocolVersion::V_2026_07_28,
                                       JSON{1}, descriptors, arguments, headers)
            .has_value());
  }
}

TEST(conformance_roots_reject_non_file_schemes) {
  for (const auto *const uri :
       {"https://example.com", "relative/path", "FILE:///tmp"}) {
    auto input{parameters()};
    input.assign("inputResponses", parse_json(R"({"r":{"roots":[{}]}})"));
    input.at("inputResponses")
        .at("r")
        .at("roots")
        .at(0)
        .assign("uri", JSON{uri});
    const auto prior{parse_json(R"({"r":{"method":"roots/list"}})")};
    EXPECT_FALSE(mcp_validate_continuation(MCPProtocolVersion::V_2026_07_28,
                                           input, prior, "state"));
  }
}

// Receive-policy characterization, NOT a claim that these are valid URIs.
// Root.uri has @format uri in schema/2026-07-28/schema.ts. The receiver
// currently checks only the literal file:// prefix. Tightening that policy
// needs agreement.
TEST(conformance_roots_current_receive_policy_preserves_prefixed_identifiers) {
  for (const auto *const uri : {"file:///tmp", "file://bad space", "file://"}) {
    auto input{parameters()};
    input.assign("inputResponses", parse_json(R"({"r":{"roots":[{}]}})"));
    input.at("inputResponses")
        .at("r")
        .at("roots")
        .at(0)
        .assign("uri", JSON{uri});
    const auto prior{parse_json(R"({"r":{"method":"roots/list"}})")};
    EXPECT_TRUE(mcp_validate_continuation(MCPProtocolVersion::V_2026_07_28,
                                          input, prior, "state"));
  }
}
