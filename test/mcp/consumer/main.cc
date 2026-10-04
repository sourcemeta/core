#include <sourcemeta/core/mcp.h>

#include <array>
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
  if (envelope.at("id") != JSON{0} || envelope.at("result") != result) {
    return 1;
  }
  const auto capabilities{mcp_parse_server_capabilities(
      version, parse_json(R"({"resources":{},"logging":{}})"))};
  if (!mcp_serialize_server_capabilities(version, capabilities)
           .defines("logging")) {
    return 1;
  }
  constexpr std::array<JSON::StringView, 1> audience{{"user"}};
  const MCPResourceAnnotations annotations{audience, 0.5,
                                           "2026-07-28T00:00:00Z"};
  const auto icons{parse_json(R"([{"src":"https://example.com/icon.png"}])")};
  const auto resource{mcp_make_resource(version, "file:///a", "a", "text/plain",
                                        {}, std::nullopt, std::nullopt,
                                        {"Title", &icons}, annotations)};
  if (!resource.defines("title") || !resource.defines("icons")) {
    return 1;
  }
  const auto block{mcp_make_embedded_resource(
      version,
      mcp_make_resource_blob_content("file:///a", "application/octet-stream",
                                     "YWJj"),
      annotations)};
  if (block.at("type") != JSON{"resource"} ||
      mcp_make_image_block(version, "YWJj", "image/png").at("type") !=
          JSON{"image"} ||
      mcp_make_audio_block(version, "YWJj", "audio/wav").at("type") !=
          JSON{"audio"}) {
    return 1;
  }
  if (mcp_resolve_result_type(version, result) !=
          JSON::StringView{"complete"} ||
      !mcp_log_level_enabled(MCPLogLevel::Error, MCPLogLevel::Warning)) {
    return 1;
  }
  return 0;
}
