#include <sourcemeta/core/mcp_capabilities.h>

#include "helpers.h"

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/mcp_protocol.h>

#include <utility>

namespace sourcemeta::core {

auto mcp_parse_client_capabilities(const sourcemeta::core::JSON &capabilities)
    -> MCPClientCapabilities {
  MCPClientCapabilities result;
  if (!capabilities.is_object()) {
    return result;
  }

  const auto *roots_field{capabilities.try_at("roots", MCP_HASH_ROOTS)};
  if (roots_field != nullptr && roots_field->is_object()) {
    result.roots = true;
    const auto *list_changed_field{
        roots_field->try_at("listChanged", MCP_HASH_LIST_CHANGED)};
    if (list_changed_field != nullptr && list_changed_field->is_boolean()) {
      result.roots_list_changed = list_changed_field->to_boolean();
    }
  }

  const auto *sampling_field{
      capabilities.try_at("sampling", MCP_HASH_SAMPLING)};
  if (sampling_field != nullptr && sampling_field->is_object()) {
    result.sampling = true;
    const auto *context_field{
        sampling_field->try_at("context", MCP_HASH_CONTEXT)};
    if (context_field != nullptr && context_field->is_object()) {
      result.sampling_context = true;
    }
    const auto *tools_field{sampling_field->try_at("tools", MCP_HASH_TOOLS)};
    if (tools_field != nullptr && tools_field->is_object()) {
      result.sampling_tools = true;
    }
  }

  const auto *elicitation_field{
      capabilities.try_at("elicitation", MCP_HASH_ELICITATION)};
  if (elicitation_field != nullptr && elicitation_field->is_object()) {
    result.elicitation = true;
    const auto *form_field{elicitation_field->try_at("form", MCP_HASH_FORM)};
    if (form_field != nullptr && form_field->is_object()) {
      result.elicitation_form = true;
    }
    const auto *url_field{elicitation_field->try_at("url", MCP_HASH_URL)};
    if (url_field != nullptr && url_field->is_object()) {
      result.elicitation_url = true;
    }
  }

  const auto *extensions_field{
      capabilities.try_at("extensions", MCP_HASH_EXTENSIONS)};
  if (extensions_field != nullptr && extensions_field->is_object()) {
    result.extensions = *extensions_field;
  }

  const auto *experimental_field{
      capabilities.try_at("experimental", MCP_HASH_EXPERIMENTAL)};
  if (experimental_field != nullptr && experimental_field->is_object()) {
    result.experimental = *experimental_field;
  }

  return result;
}

auto mcp_serialize_client_capabilities(
    const MCPProtocolVersion version, const MCPClientCapabilities &capabilities)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};

  if (capabilities.roots || capabilities.roots_list_changed) {
    auto roots_object{sourcemeta::core::JSON::make_object()};
    if (capabilities.roots_list_changed &&
        version != MCPProtocolVersion::V_2026_07_28) {
      roots_object.assign_assume_new(
          "listChanged", sourcemeta::core::JSON{true}, MCP_HASH_LIST_CHANGED);
    }
    result.assign_assume_new("roots", std::move(roots_object), MCP_HASH_ROOTS);
  }

  if (capabilities.sampling || capabilities.sampling_context ||
      capabilities.sampling_tools) {
    auto sampling_object{sourcemeta::core::JSON::make_object()};
    if (capabilities.sampling_context) {
      sampling_object.assign_assume_new(
          "context", sourcemeta::core::JSON::make_object(), MCP_HASH_CONTEXT);
    }
    if (capabilities.sampling_tools) {
      sampling_object.assign_assume_new(
          "tools", sourcemeta::core::JSON::make_object(), MCP_HASH_TOOLS);
    }
    result.assign_assume_new("sampling", std::move(sampling_object),
                             MCP_HASH_SAMPLING);
  }

  if (capabilities.elicitation || capabilities.elicitation_form ||
      capabilities.elicitation_url) {
    auto elicitation_object{sourcemeta::core::JSON::make_object()};
    if (capabilities.elicitation_form) {
      elicitation_object.assign_assume_new(
          "form", sourcemeta::core::JSON::make_object(), MCP_HASH_FORM);
    }
    if (capabilities.elicitation_url) {
      elicitation_object.assign_assume_new(
          "url", sourcemeta::core::JSON::make_object(), MCP_HASH_URL);
    }
    result.assign_assume_new("elicitation", std::move(elicitation_object),
                             MCP_HASH_ELICITATION);
  }

  if (capabilities.extensions.has_value() &&
      capabilities.extensions->is_object()) {
    result.assign_assume_new(
        "extensions", sourcemeta::core::JSON{capabilities.extensions.value()},
        MCP_HASH_EXTENSIONS);
  }

  if (capabilities.experimental.has_value() &&
      capabilities.experimental->is_object()) {
    result.assign_assume_new(
        "experimental",
        sourcemeta::core::JSON{capabilities.experimental.value()},
        MCP_HASH_EXPERIMENTAL);
  }

  return result;
}

} // namespace sourcemeta::core
