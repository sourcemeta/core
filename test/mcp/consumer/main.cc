#include <sourcemeta/core/mcp.h>

#include <sstream>

int main() {
  using namespace sourcemeta::core;
  constexpr auto version{MCPProtocolVersion::V_2026_07_28};
  const auto result{
      mcp_make_resources_list_result(version, JSON::make_array(), "", {})};
  std::ostringstream stream;
  mcp_write_result(stream, version, "resources/list", JSON{0}, result,
                   MCPCachePolicy{});
  const auto envelope{parse_json(stream.str())};
  return envelope.at("id") == JSON{0} && envelope.at("result") == result ? 0
                                                                         : 1;
}
