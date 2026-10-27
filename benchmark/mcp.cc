#include <sourcemeta/core/benchmark.h>
#include <sourcemeta/core/mcp.h>

#include <array>   // std::array
#include <cassert> // assert
#include <cstddef> // std::size_t
#include <string>  // std::string, std::to_string
#include <utility> // std::pair
#include <vector>  // std::vector

BENCHMARK(MCP_Validate_Headers_64) {
  using namespace sourcemeta::core;
  constexpr std::array<JSON::StringView, 64> NAMES{
      {"field0",  "field1",  "field2",  "field3",  "field4",  "field5",
       "field6",  "field7",  "field8",  "field9",  "field10", "field11",
       "field12", "field13", "field14", "field15", "field16", "field17",
       "field18", "field19", "field20", "field21", "field22", "field23",
       "field24", "field25", "field26", "field27", "field28", "field29",
       "field30", "field31", "field32", "field33", "field34", "field35",
       "field36", "field37", "field38", "field39", "field40", "field41",
       "field42", "field43", "field44", "field45", "field46", "field47",
       "field48", "field49", "field50", "field51", "field52", "field53",
       "field54", "field55", "field56", "field57", "field58", "field59",
       "field60", "field61", "field62", "field63"}};
  std::array<MCPHeaderParameter, 64> parameters;
  auto arguments{JSON::make_object()};
  for (std::size_t index{0}; index < parameters.size(); ++index) {
    parameters[index] = {.name = NAMES[index],
                         .path = {NAMES[index]},
                         .type = JSON::Type::String};
    arguments.assign(NAMES[index], JSON{index % 2 == 0 ? "plain" : " café "});
  }

  const auto owned{mcp_make_parameter_headers(parameters, arguments)};
  // Unrelated headers precede the mirrored values to exercise linear searches.
  std::vector<std::pair<JSON::StringView, JSON::StringView>> headers(
      64, {"Other", "ignored"});
  headers.reserve(128);
  for (const auto &header : owned) {
    headers.emplace_back(header.first, header.second);
  }

  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const JSON identifier{1};
  assert(!mcp_validate_parameter_headers(VERSION, identifier, parameters,
                                         arguments, headers)
              .has_value());
  for (auto iteration : state) {
    const auto result{mcp_validate_parameter_headers(
        VERSION, identifier, parameters, arguments, headers)};
    benchmark_do_not_optimize(result);
  }
}

BENCHMARK(MCP_Intersect_Disjoint_512) {
  using namespace sourcemeta::core;
  // Filters borrow the URI strings; fixed backing storage outlives the loop.
  std::array<std::string, 1024> uris;
  MCPSubscriptionFilter left;
  MCPSubscriptionFilter right;
  left.resource_subscriptions.emplace();
  right.resource_subscriptions.emplace();
  left.resource_subscriptions->reserve(512);
  right.resource_subscriptions->reserve(512);
  for (std::size_t index{0}; index < 512; ++index) {
    uris[index] = "file:///bench/" + std::to_string(index);
    uris[index + 512] = "file:///bench/" + std::to_string(index + 512);
    left.resource_subscriptions->push_back(uris[index]);
    right.resource_subscriptions->push_back(uris[index + 512]);
  }

  [[maybe_unused]] const auto control{
      mcp_intersect_subscription_filters(left, right)};
  assert(control.resource_subscriptions &&
         control.resource_subscriptions->empty());
  for (auto iteration : state) {
    const auto result{mcp_intersect_subscription_filters(left, right)};
    benchmark_do_not_optimize(result);
  }
}
