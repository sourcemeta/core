#include <sourcemeta/core/benchmark.h>
#include <sourcemeta/core/mcp.h>

#include <cstddef>   // std::size_t
#include <stdexcept> // std::runtime_error
#include <string>    // std::string, std::to_string
#include <utility>   // std::pair, std::move
#include <vector>    // std::vector

namespace {
using namespace sourcemeta::core;

auto validate_headers(BenchmarkState &state, const std::size_t count) -> void {
  std::vector<std::string> names;
  names.reserve(count);
  auto arguments{JSON::make_object()};
  for (std::size_t index{0}; index < count; ++index) {
    names.push_back("field" + std::to_string(index));
    arguments.assign(names.back(), JSON{index % 2 == 0 ? "plain" : " café "});
  }

  std::vector<MCPHeaderParameter> parameters;
  parameters.reserve(count);
  for (const auto &name : names) {
    parameters.push_back(
        {.name = name, .path = {name}, .type = JSON::Type::String});
  }

  // Put unrelated headers first to exercise the per-parameter linear search.
  std::vector<std::pair<std::string, std::string>> owned;
  owned.reserve(count * 2);
  for (std::size_t index{0}; index < count; ++index) {
    owned.emplace_back("Other-" + std::to_string(index), "ignored");
  }
  for (auto &header : mcp_make_parameter_headers(parameters, arguments)) {
    owned.push_back(std::move(header));
  }
  std::vector<std::pair<JSON::StringView, JSON::StringView>> headers;
  headers.reserve(owned.size());
  for (const auto &header : owned) {
    headers.emplace_back(header.first, header.second);
  }

  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const JSON identifier{1};
  if (mcp_validate_parameter_headers(VERSION, identifier, parameters, arguments,
                                     headers)) {
    throw std::runtime_error{"Invalid MCP header benchmark fixture"};
  }
  for (auto iteration : state) {
    const auto result{mcp_validate_parameter_headers(
        VERSION, identifier, parameters, arguments, headers)};
    benchmark_do_not_optimize(result);
  }
}
} // namespace

BENCHMARK(MCP_Validate_Headers_2) { validate_headers(state, 2); }
BENCHMARK(MCP_Validate_Headers_64) { validate_headers(state, 64); }

BENCHMARK(MCP_Intersect_Disjoint_512) {
  using namespace sourcemeta::core;
  std::vector<std::string> requested;
  std::vector<std::string> supported;
  requested.reserve(512);
  supported.reserve(512);
  for (std::size_t index{0}; index < 512; ++index) {
    requested.push_back("file:///bench/" + std::to_string(index));
    supported.push_back("file:///bench/" + std::to_string(index + 512));
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

  // No matches forces a complete supported-list scan for each requested URI.
  const auto control{mcp_intersect_subscription_filters(left, right)};
  if (!control.resource_subscriptions ||
      !control.resource_subscriptions->empty()) {
    throw std::runtime_error{"Invalid MCP subscription benchmark fixture"};
  }
  for (auto iteration : state) {
    const auto result{mcp_intersect_subscription_filters(left, right)};
    benchmark_do_not_optimize(result);
  }
}
