#ifndef SOURCEMETA_CORE_MCP_VALIDATION_H_
#define SOURCEMETA_CORE_MCP_VALIDATION_H_

#include <sourcemeta/core/mcp.h>

#include <stdexcept>
#include <string_view>

namespace sourcemeta::core::internal {

inline void require(const bool condition, const char *message) {
  if (!condition) {
    throw std::invalid_argument{message};
  }
}

inline auto valid_id(const JSON &identifier) noexcept -> bool {
  return identifier.is_string() || identifier.is_integer();
}

auto valid_implementation(MCPProtocolVersion version, const JSON &value)
    -> bool;
auto make_implementation(MCPProtocolVersion version,
                         const MCPImplementation &value) -> JSON;
auto valid_capabilities(MCPProtocolVersion version, const JSON &value,
                        bool client) -> bool;
auto valid_extensions(const JSON &value) -> bool;
auto valid_tool(MCPProtocolVersion version, const JSON &value) -> bool;
auto valid_content(MCPProtocolVersion version, const JSON &value) -> bool;
auto valid_resource(const JSON &value, bool contents, bool is_template) -> bool;
auto valid_prompt(const JSON &value) -> bool;
auto valid_input_requests(const JSON &value,
                          const MCPClientCapabilities &capabilities) -> bool;
auto valid_input_response(JSON::StringView method, const JSON &request,
                          const JSON &value) -> bool;
auto valid_meta(const JSON &value) -> bool;
auto valid_metadata_object(const JSON &meta) -> bool;
auto valid_log_level(JSON::StringView value) noexcept -> bool;

} // namespace sourcemeta::core::internal

#endif
