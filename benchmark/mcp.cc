#include <sourcemeta/core/benchmark.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/mcp.h>

#include <cstddef>   // std::size_t
#include <sstream>   // std::ostringstream
#include <stdexcept> // std::runtime_error
#include <string>    // std::string, std::to_string
#include <utility>   // std::pair, std::move
#include <vector>    // std::vector

namespace {
using namespace sourcemeta::core;
constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};

// Unlike assert, preflight controls must also run in Release builds. All setup
// and controls precede the state loop, so they are outside its timing window.
auto check(const bool condition) -> void {
  if (!condition) {
    throw std::runtime_error{"Invalid MCP benchmark fixture"};
  }
}

auto metadata(const std::size_t count, const bool invalid) -> JSON {
  auto value{parse_json(R"JSON({"jsonrpc":"2.0","id":1,"method":"tools/call",
    "params":{"_meta":{"io.modelcontextprotocol/protocolVersion":"2026-07-28",
    "io.modelcontextprotocol/clientCapabilities":{}},"name":"bench"}})JSON")};
  auto &meta{value.at("params").at("_meta")};
  for (std::size_t index{0}; index < count; ++index) {
    meta.assign("bench-" + std::to_string(index), JSON{std::string(64, 'x')});
  }
  if (invalid) {
    meta.assign("progressToken", JSON::make_object());
  }
  return value;
}

auto validate_metadata(BenchmarkState &state, const std::size_t count,
                       const bool invalid) -> void {
  const auto input{metadata(count, invalid)};
  const auto expected{invalid ? MCPRequestMetaStatus::InvalidProgressToken
                              : MCPRequestMetaStatus::Valid};
  check(mcp_validate_request_meta(input).first == expected);
  for (auto iteration : state) {
    const auto result{mcp_validate_request_meta(input)};
    benchmark_do_not_optimize(result);
  }
}

auto parse_metadata(BenchmarkState &state, const std::size_t count) -> void {
  const auto input{metadata(count, false)};
  std::ostringstream encoded;
  stringify(input, encoded);
  const auto wire{encoded.str()};
  check(wire.size() == (count == 0 ? 185 : 10187));
  check(parse_json(wire) == input);
  for (auto iteration : state) {
    const auto result{parse_json(wire)};
    benchmark_do_not_optimize(result);
  }
}

// Owning strings are fully populated before constructing views. No owner is
// mutated after its views have been handed to descriptors/headers.
struct HeaderFixture {
  std::vector<std::string> names;
  std::vector<MCPHeaderParameter> descriptors;
  JSON arguments{JSON::make_object()};
  std::vector<std::pair<std::string, std::string>> owned;
  std::vector<std::pair<JSON::StringView, JSON::StringView>> headers;

  explicit HeaderFixture(const std::size_t count, const bool invalid) {
    for (std::size_t index{0}; index < count; ++index) {
      names.push_back("field" + std::to_string(index));
      arguments.assign(names.back(), JSON{index % 2 == 0 ? "plain" : " café "});
    }
    for (const auto &name : names) {
      descriptors.push_back({name, {name}, JSON::Type::String});
    }
    // Unrelated headers force the validator to search, not just match the
    // first element. The invalid case fails at the last mirrored parameter.
    for (std::size_t index{0}; index < count; ++index) {
      owned.emplace_back("Other-" + std::to_string(index), "ignored");
    }
    auto mirrored{mcp_make_parameter_headers(descriptors, arguments)};
    for (auto &entry : mirrored) {
      owned.push_back(std::move(entry));
    }
    if (invalid) {
      owned.back().second = "mismatch";
    }
    for (const auto &entry : owned) {
      headers.emplace_back(entry.first, entry.second);
    }
  }
};

auto validate_headers(BenchmarkState &state, const std::size_t count,
                      const bool invalid) -> void {
  const HeaderFixture input{count, invalid};
  const JSON identifier{1};
  check(mcp_validate_parameter_headers(VERSION, identifier, input.descriptors,
                                       input.arguments, input.headers)
            .has_value() == invalid);
  for (auto iteration : state) {
    const auto result{
        mcp_validate_parameter_headers(VERSION, identifier, input.descriptors,
                                       input.arguments, input.headers)};
    benchmark_do_not_optimize(result);
  }
}

auto make_headers(BenchmarkState &state, const std::size_t count) -> void {
  const HeaderFixture input{count, false};
  check(mcp_make_parameter_headers(input.descriptors, input.arguments).size() ==
        count);
  for (auto iteration : state) {
    const auto result{
        mcp_make_parameter_headers(input.descriptors, input.arguments)};
    benchmark_do_not_optimize(result);
  }
}

auto result_payload(const std::size_t count) -> JSON {
  auto entries{JSON::make_array()};
  for (std::size_t index{0}; index < count; ++index) {
    auto entry{JSON::make_object()};
    entry.assign("name", JSON{"item" + std::to_string(index)});
    entry.assign("uri", JSON{"file:///bench/" + std::to_string(index)});
    entry.assign("description", JSON{std::string(128, 'x')});
    entries.push_back(std::move(entry));
  }
  auto result{JSON::make_object()};
  result.assign("resources", std::move(entries));
  return result;
}

auto serialize_result(BenchmarkState &state, const std::size_t count,
                      const bool move, const bool invalid) -> void {
  auto input{result_payload(count)};
  if (invalid) {
    input.at("resources").at(count - 1).assign("uri", JSON{false});
  }
  const JSON identifier{1};
  const MCPCachePolicy policy{.ttl_ms = 0, .scope = MCPCacheScope::Private};
  std::ostringstream control;
  bool rejected{false};
  try {
    mcp_write_result(control, VERSION, "resources/list", identifier, input,
                     policy);
  } catch (const MCPError &) {
    rejected = true;
  }
  check(rejected == invalid);
  if (!invalid) {
    std::ostringstream payload;
    stringify(input, payload);
    check(payload.str().size() == (count == 1 ? 201 : 48435));
    const auto output{parse_json(control.str())};
    check(output.at("result").at("resources") == input.at("resources"));
    if (move) {
      auto fresh{input};
      std::ostringstream moved;
      mcp_write_result(moved, VERSION, "resources/list", identifier,
                       std::move(fresh), policy);
      check(moved.str() == control.str());
    }
  }
  for (auto iteration : state) {
    std::ostringstream stream;
    if (invalid) {
      try {
        mcp_write_result(stream, VERSION, "resources/list", identifier, input,
                         policy);
      } catch (const MCPError &error) {
        benchmark_do_not_optimize(error);
      }
    } else if (move) {
      // Each repetition needs an intact payload. Include this copy explicitly;
      // this is copy + move-write, not a fictitious copy-free move comparison.
      auto fresh{input};
      mcp_write_result(stream, VERSION, "resources/list", identifier,
                       std::move(fresh), policy);
    } else {
      mcp_write_result(stream, VERSION, "resources/list", identifier, input,
                       policy);
    }
    benchmark_do_not_optimize(stream);
  }
}

auto intersect(BenchmarkState &state, const std::size_t count,
               const bool overlap) -> void {
  std::vector<std::string> requested;
  std::vector<std::string> supported;
  for (std::size_t index{0}; index < count; ++index) {
    requested.push_back("file:///bench/" + std::to_string(index));
    supported.push_back("file:///bench/" +
                        std::to_string(index + (overlap ? count / 2 : count)));
  }
  MCPSubscriptionFilter left;
  MCPSubscriptionFilter right;
  left.resource_subscriptions.emplace();
  right.resource_subscriptions.emplace();
  for (const auto &uri : requested) {
    left.resource_subscriptions->push_back(uri);
  }
  for (const auto &uri : supported) {
    right.resource_subscriptions->push_back(uri);
  }
  const auto control{mcp_intersect_subscription_filters(left, right)};
  check(control.resource_subscriptions &&
        control.resource_subscriptions->size() == (overlap ? count / 2 : 0));
  if (overlap) {
    for (std::size_t index{0}; index < count / 2; ++index) {
      check(control.resource_subscriptions->at(index) ==
            requested.at(index + count / 2));
    }
  }
  for (auto iteration : state) {
    const auto result{mcp_intersect_subscription_filters(left, right)};
    benchmark_do_not_optimize(result);
  }
}
} // namespace

BENCHMARK(MCP_Parse_Metadata_0) { parse_metadata(state, 0); }
BENCHMARK(MCP_Parse_Metadata_128) { parse_metadata(state, 128); }
BENCHMARK(MCP_Validate_Metadata_0) { validate_metadata(state, 0, false); }
BENCHMARK(MCP_Validate_Metadata_128) { validate_metadata(state, 128, false); }
BENCHMARK(MCP_Reject_Metadata_0) { validate_metadata(state, 0, true); }
BENCHMARK(MCP_Reject_Metadata_128) { validate_metadata(state, 128, true); }
BENCHMARK(MCP_Validate_Headers_2) { validate_headers(state, 2, false); }
BENCHMARK(MCP_Validate_Headers_64) { validate_headers(state, 64, false); }
BENCHMARK(MCP_Reject_Headers_2) { validate_headers(state, 2, true); }
BENCHMARK(MCP_Reject_Headers_64) { validate_headers(state, 64, true); }
BENCHMARK(MCP_Make_Headers_2) { make_headers(state, 2); }
BENCHMARK(MCP_Make_Headers_64) { make_headers(state, 64); }
BENCHMARK(MCP_Write_Result_1) { serialize_result(state, 1, false, false); }
BENCHMARK(MCP_Write_Result_256) { serialize_result(state, 256, false, false); }
BENCHMARK(MCP_Copy_Move_Write_Result_1) {
  serialize_result(state, 1, true, false);
}
BENCHMARK(MCP_Copy_Move_Write_Result_256) {
  serialize_result(state, 256, true, false);
}
BENCHMARK(MCP_Reject_Result_1) { serialize_result(state, 1, false, true); }
BENCHMARK(MCP_Reject_Result_256) { serialize_result(state, 256, false, true); }
BENCHMARK(MCP_Intersect_Overlap_8) { intersect(state, 8, true); }
BENCHMARK(MCP_Intersect_Overlap_512) { intersect(state, 512, true); }
BENCHMARK(MCP_Intersect_Disjoint_8) { intersect(state, 8, false); }
BENCHMARK(MCP_Intersect_Disjoint_512) { intersect(state, 512, false); }
