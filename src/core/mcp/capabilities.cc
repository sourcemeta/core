#include <sourcemeta/core/mcp.h>

#include "helpers.h"
#include "validation.h"

#include <sourcemeta/core/json.h>

#include <utility>

namespace {
auto capability_settings(
    const sourcemeta::core::MCPClientCapabilities &capabilities,
    const sourcemeta::core::JSON::StringView name) -> sourcemeta::core::JSON {
  const auto *value{capabilities.source && capabilities.source->is_object()
                        ? capabilities.source->try_at(name)
                        : nullptr};
  return value && value->is_object() ? *value
                                     : sourcemeta::core::JSON::make_object();
}
} // namespace

namespace sourcemeta::core {

auto mcp_parse_client_capabilities(const MCPProtocolVersion version,
                                   const sourcemeta::core::JSON &capabilities)
    -> MCPClientCapabilities {
  MCPClientCapabilities result;
  result.source = capabilities;
  internal::require(internal::valid_capabilities(version, capabilities, true),
                    "Invalid client capabilities");

  const auto *roots_field{capabilities.try_at("roots", MCP_HASH_ROOTS)};
  if (roots_field != nullptr && roots_field->is_object()) {
    result.roots = true;
    const auto *list_changed_field{
        roots_field->try_at("listChanged", MCP_HASH_LIST_CHANGED)};
    if (version != MCPProtocolVersion::V_2026_07_28 &&
        list_changed_field != nullptr && list_changed_field->is_boolean()) {
      result.roots_list_changed = list_changed_field->to_boolean();
    }
  }

  const auto *sampling_field{
      capabilities.try_at("sampling", MCP_HASH_SAMPLING)};
  if (sampling_field != nullptr && sampling_field->is_object()) {
    result.sampling = true;
    const auto *context_field{
        sampling_field->try_at("context", MCP_HASH_CONTEXT)};
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25) &&
        context_field != nullptr && context_field->is_object()) {
      result.sampling_context = true;
    }
    const auto *tools_field{sampling_field->try_at("tools", MCP_HASH_TOOLS)};
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25) &&
        tools_field != nullptr && tools_field->is_object()) {
      result.sampling_tools = true;
    }
  }

  const auto *elicitation_field{
      capabilities.try_at("elicitation", MCP_HASH_ELICITATION)};
  if (mcp_protocol_version_at_least(version,
                                    MCPProtocolVersion::V_2025_06_18) &&
      elicitation_field != nullptr && elicitation_field->is_object()) {
    result.elicitation = true;
    const auto *form_field{elicitation_field->try_at("form", MCP_HASH_FORM)};
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25) &&
        form_field != nullptr && form_field->is_object()) {
      result.elicitation_form = true;
    }
    const auto *url_field{elicitation_field->try_at("url", MCP_HASH_URL)};
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25) &&
        url_field != nullptr && url_field->is_object()) {
      result.elicitation_url = true;
    }
  }

  const auto *extensions_field{
      capabilities.try_at("extensions", MCP_HASH_EXTENSIONS)};
  if (version == MCPProtocolVersion::V_2026_07_28 &&
      extensions_field != nullptr && extensions_field->is_object()) {
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
  internal::require(!capabilities.experimental ||
                        capabilities.experimental->is_object(),
                    "Invalid experimental capabilities");
  if (version == MCPProtocolVersion::V_2026_07_28 && capabilities.extensions) {
    internal::require(internal::valid_extensions(*capabilities.extensions),
                      "Invalid extension capabilities");
  }
  internal::require(!capabilities.source || capabilities.source->is_object(),
                    "Invalid capability source");
  auto result{capabilities.source.value_or(JSON::make_object())};
  // Preserve opaque settings while removing features absent from this revision.
  if (version != MCPProtocolVersion::V_2025_11_25) {
    result.erase("tasks");
  }
  if (version != MCPProtocolVersion::V_2026_07_28) {
    result.erase("extensions");
  }
  result.erase("roots");
  result.erase("sampling");
  result.erase("elicitation");

  if (capabilities.roots || (capabilities.roots_list_changed &&
                             version != MCPProtocolVersion::V_2026_07_28)) {
    auto roots_object{capability_settings(capabilities, "roots")};
    if (version == MCPProtocolVersion::V_2026_07_28) {
      roots_object.erase("listChanged");
    }
    if (capabilities.roots_list_changed &&
        version != MCPProtocolVersion::V_2026_07_28) {
      roots_object.assign("listChanged", sourcemeta::core::JSON{true});
    }
    result.assign("roots", std::move(roots_object));
  }

  if (capabilities.sampling || capabilities.sampling_context ||
      capabilities.sampling_tools) {
    auto sampling_object{capability_settings(capabilities, "sampling")};
    if (!mcp_protocol_version_at_least(version,
                                       MCPProtocolVersion::V_2025_11_25)) {
      sampling_object.erase("context");
      sampling_object.erase("tools");
    }
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25)) {
      if (capabilities.sampling_context) {
        if (!sampling_object.defines("context")) {
          sampling_object.assign("context", JSON::make_object());
        }
      }
      if (capabilities.sampling_tools) {
        if (!sampling_object.defines("tools")) {
          sampling_object.assign("tools", JSON::make_object());
        }
      }
    }
    result.assign("sampling", std::move(sampling_object));
  }

  if (mcp_protocol_version_at_least(version,
                                    MCPProtocolVersion::V_2025_06_18) &&
      (capabilities.elicitation || capabilities.elicitation_form ||
       capabilities.elicitation_url)) {
    auto elicitation_object{capability_settings(capabilities, "elicitation")};
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25)) {
      if (capabilities.elicitation_form) {
        if (!elicitation_object.defines("form")) {
          elicitation_object.assign("form", JSON::make_object());
        }
      }
      if (capabilities.elicitation_url) {
        if (!elicitation_object.defines("url")) {
          elicitation_object.assign("url", JSON::make_object());
        }
      }
    }
    result.assign("elicitation", std::move(elicitation_object));
  }

  if (version == MCPProtocolVersion::V_2026_07_28 &&
      capabilities.extensions.has_value() &&
      capabilities.extensions->is_object()) {
    result.assign("extensions",
                  sourcemeta::core::JSON{capabilities.extensions.value()});
  }

  if (capabilities.experimental.has_value() &&
      capabilities.experimental->is_object()) {
    result.assign("experimental",
                  sourcemeta::core::JSON{capabilities.experimental.value()});
  }

  internal::require(internal::valid_capabilities(version, result, true),
                    "Invalid capability map");
  return result;
}

} // namespace sourcemeta::core
