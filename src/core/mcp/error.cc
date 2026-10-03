#include <sourcemeta/core/mcp_error.h>

#include "helpers.h"

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp_protocol.h>

#include <utility>
#include <vector>

namespace sourcemeta::core {

auto mcp_make_error_unsupported_protocol_version(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const JSON::StringView requested,
    const std::vector<JSON::StringView> &supported) -> sourcemeta::core::JSON {
  auto data{sourcemeta::core::JSON::make_object()};
  auto supported_array{sourcemeta::core::JSON::make_array()};
  for (const auto version_string : supported) {
    supported_array.push_back(sourcemeta::core::JSON{version_string});
  }
  data.assign_assume_new("supported", std::move(supported_array),
                         MCP_HASH_SUPPORTED);
  data.assign_assume_new("requested", sourcemeta::core::JSON{requested},
                         MCP_HASH_REQUESTED);

  const auto *const identifier_pointer{
      identifier.has_value() ? &identifier.value() : nullptr};
  auto envelope{sourcemeta::core::jsonrpc_make_error(
      identifier_pointer, MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
      "Unsupported protocol version", std::move(data))};
  if (!identifier.has_value()) {
    envelope.erase("id", MCP_HASH_ID);
  }
  return envelope;
}

auto mcp_make_error_missing_required_capability(
    const std::optional<sourcemeta::core::JSON> &identifier,
    sourcemeta::core::JSON required_capabilities,
    const JSON::StringView message) -> sourcemeta::core::JSON {
  auto data{sourcemeta::core::JSON::make_object()};
  data.assign_assume_new("requiredCapabilities",
                         std::move(required_capabilities),
                         MCP_HASH_REQUIRED_CAPABILITIES);

  const auto *const identifier_pointer{
      identifier.has_value() ? &identifier.value() : nullptr};
  auto envelope{sourcemeta::core::jsonrpc_make_error(
      identifier_pointer, MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY, message,
      std::move(data))};
  if (!identifier.has_value()) {
    envelope.erase("id", MCP_HASH_ID);
  }
  return envelope;
}

auto mcp_make_error_header_mismatch(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const JSON::StringView message) -> sourcemeta::core::JSON {
  const auto *const identifier_pointer{
      identifier.has_value() ? &identifier.value() : nullptr};
  auto envelope{sourcemeta::core::jsonrpc_make_error(
      identifier_pointer, MCP_CODE_HEADER_MISMATCH, message)};
  if (!identifier.has_value()) {
    envelope.erase("id", MCP_HASH_ID);
  }
  return envelope;
}

auto mcp_make_error_header_mismatch(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const JSON::StringView header_name, const JSON::StringView header_value,
    const JSON::StringView body_value) -> sourcemeta::core::JSON {
  auto data{sourcemeta::core::JSON::make_object()};
  data.assign_assume_new("header", sourcemeta::core::JSON{header_name},
                         MCP_HASH_HEADER);
  data.assign_assume_new("headerValue", sourcemeta::core::JSON{header_value},
                         MCP_HASH_HEADER_VALUE);
  data.assign_assume_new("bodyValue", sourcemeta::core::JSON{body_value},
                         MCP_HASH_BODY_VALUE);

  const auto *const identifier_pointer{
      identifier.has_value() ? &identifier.value() : nullptr};
  auto envelope{sourcemeta::core::jsonrpc_make_error(
      identifier_pointer, MCP_CODE_HEADER_MISMATCH, "Header mismatch",
      std::move(data))};
  if (!identifier.has_value()) {
    envelope.erase("id", MCP_HASH_ID);
  }
  return envelope;
}

auto mcp_make_error_header_mismatch(
    const std::optional<sourcemeta::core::JSON> &identifier,
    const JSON::StringView header_name, const JSON::StringView header_value)
    -> sourcemeta::core::JSON {
  auto data{sourcemeta::core::JSON::make_object()};
  data.assign_assume_new("header", sourcemeta::core::JSON{header_name},
                         MCP_HASH_HEADER);
  data.assign_assume_new("headerValue", sourcemeta::core::JSON{header_value},
                         MCP_HASH_HEADER_VALUE);

  const auto *const identifier_pointer{
      identifier.has_value() ? &identifier.value() : nullptr};
  auto envelope{sourcemeta::core::jsonrpc_make_error(
      identifier_pointer, MCP_CODE_HEADER_MISMATCH,
      "Header mismatch: unexpected header provided", std::move(data))};
  if (!identifier.has_value()) {
    envelope.erase("id", MCP_HASH_ID);
  }
  return envelope;
}

auto mcp_make_error_resource_not_found(
    const MCPProtocolVersion version,
    const std::optional<sourcemeta::core::JSON> &identifier)
    -> sourcemeta::core::JSON {
  const auto *const identifier_pointer{
      identifier.has_value() ? &identifier.value() : nullptr};
  auto envelope{version == MCPProtocolVersion::V_2026_07_28
                    ? sourcemeta::core::jsonrpc_make_error(
                          identifier_pointer, JSONRPC_CODE_INVALID_PARAMS,
                          "Resource not found")
                    : sourcemeta::core::jsonrpc_make_error(
                          identifier_pointer, MCP_CODE_RESOURCE_NOT_FOUND,
                          "Resource not found")};
  if (!identifier.has_value()) {
    envelope.erase("id", MCP_HASH_ID);
  }
  return envelope;
}

} // namespace sourcemeta::core
