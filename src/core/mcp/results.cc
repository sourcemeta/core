#include <sourcemeta/core/mcp_results.h>

#include "helpers.h"

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp_capabilities.h>
#include <sourcemeta/core/mcp_protocol.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

auto serialize_capabilities(
    const sourcemeta::core::MCPProtocolVersion version,
    const sourcemeta::core::MCPServerCapabilities &capabilities)
    -> sourcemeta::core::JSON {
  auto capabilities_object{sourcemeta::core::JSON::make_object()};

  if (capabilities.logging) {
    capabilities_object.assign_assume_new("logging",
                                          sourcemeta::core::JSON::make_object(),
                                          sourcemeta::core::MCP_HASH_LOGGING);
  }

  if (capabilities.completions) {
    capabilities_object.assign_assume_new(
        "completions", sourcemeta::core::JSON::make_object(),
        sourcemeta::core::MCP_HASH_COMPLETIONS);
  }

  if (capabilities.prompts || capabilities.prompts_list_changed) {
    auto prompts{sourcemeta::core::JSON::make_object()};
    if (capabilities.prompts_list_changed) {
      prompts.assign_assume_new("listChanged", sourcemeta::core::JSON{true},
                                sourcemeta::core::MCP_HASH_LIST_CHANGED);
    }
    capabilities_object.assign_assume_new("prompts", std::move(prompts),
                                          sourcemeta::core::MCP_HASH_PROMPTS);
  }

  if (capabilities.resources || capabilities.resources_subscribe ||
      capabilities.resources_list_changed) {
    auto resources{sourcemeta::core::JSON::make_object()};
    if (capabilities.resources_subscribe) {
      resources.assign_assume_new("subscribe", sourcemeta::core::JSON{true},
                                  sourcemeta::core::MCP_HASH_SUBSCRIBE);
    }
    if (capabilities.resources_list_changed) {
      resources.assign_assume_new("listChanged", sourcemeta::core::JSON{true},
                                  sourcemeta::core::MCP_HASH_LIST_CHANGED);
    }
    capabilities_object.assign_assume_new("resources", std::move(resources),
                                          sourcemeta::core::MCP_HASH_RESOURCES);
  }

  if (capabilities.tools || capabilities.tools_list_changed) {
    auto tools{sourcemeta::core::JSON::make_object()};
    if (capabilities.tools_list_changed) {
      tools.assign_assume_new("listChanged", sourcemeta::core::JSON{true},
                              sourcemeta::core::MCP_HASH_LIST_CHANGED);
    }
    capabilities_object.assign_assume_new("tools", std::move(tools),
                                          sourcemeta::core::MCP_HASH_TOOLS);
  }

  if (version == sourcemeta::core::MCPProtocolVersion::V_2026_07_28 &&
      capabilities.extensions.has_value() &&
      capabilities.extensions->is_object()) {
    capabilities_object.assign_assume_new(
        "extensions", sourcemeta::core::JSON{capabilities.extensions.value()},
        sourcemeta::core::MCP_HASH_EXTENSIONS);
  }

  if (capabilities.experimental.has_value() &&
      capabilities.experimental->is_object()) {
    capabilities_object.assign_assume_new(
        "experimental",
        sourcemeta::core::JSON{capabilities.experimental.value()},
        sourcemeta::core::MCP_HASH_EXPERIMENTAL);
  }

  return capabilities_object;
}

} // namespace

namespace sourcemeta::core {

auto mcp_make_text_block(const JSON::StringView text)
    -> sourcemeta::core::JSON {
  auto block{sourcemeta::core::JSON::make_object()};
  block.assign_assume_new("type", sourcemeta::core::JSON{"text"},
                          MCP_HASH_TYPE);
  block.assign_assume_new("text", sourcemeta::core::JSON{text}, MCP_HASH_TEXT);
  return block;
}

auto mcp_make_resource_link(const MCPProtocolVersion version,
                            const JSON::StringView uri,
                            const JSON::StringView name,
                            const JSON::StringView mime_type,
                            const JSON::StringView description)
    -> sourcemeta::core::JSON {
  if (!mcp_supports_resource_link_content(version)) {
    std::string text;
    if (!name.empty()) {
      text.append(name);
      text.append("\n");
    }
    text.append(uri);
    if (!description.empty()) {
      text.append("\n");
      text.append(description);
    }
    return mcp_make_text_block(text);
  }

  auto block{sourcemeta::core::JSON::make_object()};
  block.assign_assume_new("type", sourcemeta::core::JSON{"resource_link"},
                          MCP_HASH_TYPE);
  block.assign_assume_new("uri", sourcemeta::core::JSON{uri}, MCP_HASH_URI);
  block.assign_assume_new("name", sourcemeta::core::JSON{name}, MCP_HASH_NAME);
  if (!description.empty()) {
    block.assign_assume_new("description", sourcemeta::core::JSON{description},
                            MCP_HASH_DESCRIPTION);
  }
  if (!mime_type.empty()) {
    block.assign_assume_new("mimeType", sourcemeta::core::JSON{mime_type},
                            MCP_HASH_MIME_TYPE);
  }
  return block;
}

void mcp_decorate_result_in_place(
    const MCPProtocolVersion version, sourcemeta::core::JSON &result,
    const std::optional<MCPImplementation> &server_info) {
  if (!mcp_requires_result_type(version) || !result.is_object()) {
    return;
  }

  if (result.try_at("resultType", MCP_HASH_RESULT_TYPE) == nullptr) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }

  if (server_info.has_value()) {
    auto info{sourcemeta::core::JSON::make_object()};
    info.assign_assume_new("name", sourcemeta::core::JSON{server_info->name},
                           MCP_HASH_NAME);
    info.assign_assume_new("version",
                           sourcemeta::core::JSON{server_info->version},
                           MCP_HASH_VERSION);
    if (!server_info->title.empty()) {
      info.assign_assume_new(
          "title", sourcemeta::core::JSON{server_info->title}, MCP_HASH_TITLE);
    }
    if (!server_info->description.empty()) {
      info.assign_assume_new("description",
                             sourcemeta::core::JSON{server_info->description},
                             MCP_HASH_DESCRIPTION);
    }
    if (!server_info->website_url.empty()) {
      info.assign_assume_new("websiteUrl",
                             sourcemeta::core::JSON{server_info->website_url},
                             MCP_HASH_WEBSITE_URL);
    }

    if (auto *meta{result.try_at("_meta", MCP_HASH_META)};
        meta != nullptr && meta->is_object()) {
      meta->assign("io.modelcontextprotocol/serverInfo", std::move(info));
    } else {
      auto meta_object{sourcemeta::core::JSON::make_object()};
      meta_object.assign_assume_new("io.modelcontextprotocol/serverInfo",
                                    std::move(info), MCP_HASH_META_SERVER_INFO);
      result.assign("_meta", std::move(meta_object));
    }
  }
}

auto mcp_decorate_result(const MCPProtocolVersion version,
                         sourcemeta::core::JSON result,
                         const std::optional<MCPImplementation> &server_info)
    -> sourcemeta::core::JSON {
  mcp_decorate_result_in_place(version, result, server_info);
  return result;
}

void mcp_decorate_cacheable_result_in_place(
    const MCPProtocolVersion version, sourcemeta::core::JSON &result,
    const std::optional<MCPCachePolicy> &cache_policy) {
  if (!mcp_requires_cacheable_metadata(version) || !cache_policy.has_value() ||
      !result.is_object()) {
    return;
  }

  if (const auto *result_type{
          result.try_at("resultType", MCP_HASH_RESULT_TYPE)};
      result_type != nullptr && result_type->is_string() &&
      result_type->to_string() != "complete") {
    return;
  }

  const auto clamped_ttl{std::max<std::int64_t>(0, cache_policy->ttl_ms)};
  result.assign("ttlMs", sourcemeta::core::JSON{clamped_ttl});
  result.assign("cacheScope", sourcemeta::core::JSON{
                                  mcp_cache_scope_string(cache_policy->scope)});
}

auto mcp_decorate_cacheable_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON result,
    const std::optional<MCPCachePolicy> &cache_policy)
    -> sourcemeta::core::JSON {
  mcp_decorate_cacheable_result_in_place(version, result, cache_policy);
  return result;
}

auto mcp_make_empty_result(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &identifier,
                           const std::optional<MCPImplementation> &server_info)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  return sourcemeta::core::jsonrpc_make_success(
      identifier, mcp_decorate_result(version, std::move(result), server_info));
}

auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &identifier,
                           sourcemeta::core::JSON result)
    -> sourcemeta::core::JSON {
  std::ostringstream payload;
  sourcemeta::core::prettify(result, payload);

  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(mcp_make_text_block(payload.str()));

  auto envelope_result{sourcemeta::core::JSON::make_object()};
  if (mcp_requires_result_type(version)) {
    envelope_result.assign_assume_new(
        "resultType", sourcemeta::core::JSON{"complete"}, MCP_HASH_RESULT_TYPE);
  }
  envelope_result.assign_assume_new("content", std::move(content),
                                    MCP_HASH_CONTENT);
  if (mcp_supports_structured_content(version)) {
    envelope_result.assign_assume_new("structuredContent", std::move(result),
                                      MCP_HASH_STRUCTURED_CONTENT);
  }
  envelope_result.assign_assume_new("isError", sourcemeta::core::JSON{false},
                                    MCP_HASH_IS_ERROR);
  return sourcemeta::core::jsonrpc_make_success(identifier,
                                                std::move(envelope_result));
}

auto mcp_make_tool_success(const MCPProtocolVersion version,
                           const sourcemeta::core::JSON &identifier,
                           sourcemeta::core::JSON structured,
                           sourcemeta::core::JSON content_blocks)
    -> sourcemeta::core::JSON {
  auto envelope_result{sourcemeta::core::JSON::make_object()};
  if (mcp_requires_result_type(version)) {
    envelope_result.assign_assume_new(
        "resultType", sourcemeta::core::JSON{"complete"}, MCP_HASH_RESULT_TYPE);
  }
  envelope_result.assign_assume_new("content", std::move(content_blocks),
                                    MCP_HASH_CONTENT);
  if (mcp_supports_structured_content(version)) {
    envelope_result.assign_assume_new("structuredContent",
                                      std::move(structured),
                                      MCP_HASH_STRUCTURED_CONTENT);
  }
  envelope_result.assign_assume_new("isError", sourcemeta::core::JSON{false},
                                    MCP_HASH_IS_ERROR);
  return sourcemeta::core::jsonrpc_make_success(identifier,
                                                std::move(envelope_result));
}

auto mcp_make_tool_error(const MCPProtocolVersion version,
                         const sourcemeta::core::JSON &identifier,
                         const JSON::StringView message)
    -> sourcemeta::core::JSON {
  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(mcp_make_text_block(message));

  auto envelope_result{sourcemeta::core::JSON::make_object()};
  if (mcp_requires_result_type(version)) {
    envelope_result.assign_assume_new(
        "resultType", sourcemeta::core::JSON{"complete"}, MCP_HASH_RESULT_TYPE);
  }
  envelope_result.assign_assume_new("content", std::move(content),
                                    MCP_HASH_CONTENT);
  envelope_result.assign_assume_new("isError", sourcemeta::core::JSON{true},
                                    MCP_HASH_IS_ERROR);
  return sourcemeta::core::jsonrpc_make_success(identifier,
                                                std::move(envelope_result));
}

auto mcp_make_resource(const JSON::StringView uri, const JSON::StringView name,
                       const JSON::StringView mime_type,
                       const JSON::StringView description,
                       const std::optional<std::size_t> size,
                       const std::optional<double> priority)
    -> sourcemeta::core::JSON {
  auto resource{sourcemeta::core::JSON::make_object()};
  resource.assign_assume_new("uri", sourcemeta::core::JSON{uri}, MCP_HASH_URI);
  resource.assign_assume_new("name", sourcemeta::core::JSON{name},
                             MCP_HASH_NAME);
  if (!description.empty()) {
    resource.assign_assume_new("description",
                               sourcemeta::core::JSON{description},
                               MCP_HASH_DESCRIPTION);
  }
  resource.assign_assume_new("mimeType", sourcemeta::core::JSON{mime_type},
                             MCP_HASH_MIME_TYPE);
  if (size.has_value()) {
    resource.assign_assume_new("size", sourcemeta::core::JSON{size.value()},
                               MCP_HASH_SIZE);
  }
  if (priority.has_value()) {
    const auto priority_value{std::isnan(priority.value())
                                  ? 1.0
                                  : std::clamp(priority.value(), 0.0, 1.0)};
    auto annotations{sourcemeta::core::JSON::make_object()};
    annotations.assign_assume_new(
        "priority", sourcemeta::core::JSON{priority_value}, MCP_HASH_PRIORITY);
    resource.assign_assume_new("annotations", std::move(annotations),
                               MCP_HASH_ANNOTATIONS);
  }
  return resource;
}

auto mcp_make_resource_text_content(const JSON::StringView uri,
                                    const JSON::StringView mime_type,
                                    const JSON::StringView text)
    -> sourcemeta::core::JSON {
  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign_assume_new("uri", sourcemeta::core::JSON{uri}, MCP_HASH_URI);
  entry.assign_assume_new("mimeType", sourcemeta::core::JSON{mime_type},
                          MCP_HASH_MIME_TYPE);
  entry.assign_assume_new("text", sourcemeta::core::JSON{text}, MCP_HASH_TEXT);
  return entry;
}

auto mcp_make_resources_read_result(const MCPProtocolVersion version,
                                    sourcemeta::core::JSON contents,
                                    const MCPCachePolicy &cache_policy)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("contents", std::move(contents), MCP_HASH_CONTENTS);
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_tools_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON tools,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("tools", std::move(tools), MCP_HASH_TOOLS);
  if (next_cursor.has_value()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_resources_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resources,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("resources", std::move(resources),
                           MCP_HASH_RESOURCES);
  if (next_cursor.has_value()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_resource_templates_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resource_templates,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("resourceTemplates", std::move(resource_templates),
                           MCP_HASH_RESOURCE_TEMPLATES);
  if (next_cursor.has_value()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_prompts_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON prompts,
    const std::optional<JSON::StringView> next_cursor,
    const MCPCachePolicy &cache_policy) -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("prompts", std::move(prompts), MCP_HASH_PROMPTS);
  if (next_cursor.has_value()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_resource_template(const JSON::StringView uri_template,
                                const JSON::StringView name,
                                const JSON::StringView description,
                                const JSON::StringView mime_type)
    -> sourcemeta::core::JSON {
  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign_assume_new("uriTemplate", sourcemeta::core::JSON{uri_template},
                          MCP_HASH_URI_TEMPLATE);
  entry.assign_assume_new("name", sourcemeta::core::JSON{name}, MCP_HASH_NAME);
  entry.assign_assume_new("description", sourcemeta::core::JSON{description},
                          MCP_HASH_DESCRIPTION);
  entry.assign_assume_new("mimeType", sourcemeta::core::JSON{mime_type},
                          MCP_HASH_MIME_TYPE);
  return entry;
}

auto mcp_make_tool_descriptor(
    const MCPProtocolVersion version, const JSON::StringView name,
    const JSON::StringView description, sourcemeta::core::JSON input_schema,
    std::optional<sourcemeta::core::JSON> output_schema,
    const MCPToolAnnotations &annotations) -> sourcemeta::core::JSON {

#ifndef NDEBUG
  const auto *type_field{input_schema.is_object()
                             ? input_schema.try_at("type", MCP_HASH_TYPE)
                             : nullptr};
  assert(type_field != nullptr && type_field->is_string() &&
         type_field->to_string() == "object");
#endif

  auto entry{sourcemeta::core::JSON::make_object()};
  entry.assign_assume_new("name", sourcemeta::core::JSON{name}, MCP_HASH_NAME);
  entry.assign_assume_new("description", sourcemeta::core::JSON{description},
                          MCP_HASH_DESCRIPTION);
  entry.assign_assume_new("inputSchema", std::move(input_schema),
                          MCP_HASH_INPUT_SCHEMA);
  if (output_schema.has_value() && mcp_supports_output_schema(version)) {
    entry.assign_assume_new("outputSchema", std::move(output_schema).value(),
                            MCP_HASH_OUTPUT_SCHEMA);
  }

  auto annotations_object{sourcemeta::core::JSON::make_object()};
  if (!annotations.title.empty()) {
    annotations_object.assign_assume_new(
        "title", sourcemeta::core::JSON{annotations.title}, MCP_HASH_TITLE);
  }
  annotations_object.assign_assume_new(
      "readOnlyHint", sourcemeta::core::JSON{annotations.read_only},
      MCP_HASH_READ_ONLY_HINT);
  annotations_object.assign_assume_new(
      "destructiveHint", sourcemeta::core::JSON{annotations.destructive},
      MCP_HASH_DESTRUCTIVE_HINT);
  annotations_object.assign_assume_new(
      "idempotentHint", sourcemeta::core::JSON{annotations.idempotent},
      MCP_HASH_IDEMPOTENT_HINT);
  annotations_object.assign_assume_new(
      "openWorldHint", sourcemeta::core::JSON{annotations.open_world},
      MCP_HASH_OPEN_WORLD_HINT);
  entry.assign_assume_new("annotations", std::move(annotations_object),
                          MCP_HASH_ANNOTATIONS);

  return entry;
}

auto mcp_make_initialize_result(const sourcemeta::core::JSON &request,
                                const MCPServerCapabilities &capabilities,
                                const MCPImplementation &server,
                                const JSON::StringView instructions)
    -> sourcemeta::core::JSON {
  const auto *identifier{sourcemeta::core::jsonrpc_request_id(request)};
  const auto *parameters{sourcemeta::core::jsonrpc_params(request)};
  if (identifier == nullptr || parameters == nullptr ||
      !parameters->is_object()) {
    return sourcemeta::core::jsonrpc_make_error_invalid_request(identifier);
  }

  const auto *protocol_version_field{
      parameters->try_at("protocolVersion", MCP_HASH_PROTOCOL_VERSION)};
  if (protocol_version_field == nullptr ||
      !protocol_version_field->is_string()) {
    return sourcemeta::core::jsonrpc_make_error_invalid_params(*identifier);
  }

  const JSON::StringView requested_version{protocol_version_field->to_string()};
  const auto resolved{mcp_resolve_protocol_version(requested_version)};
  const auto version{(resolved.has_value() &&
                      mcp_uses_initialization_handshake(resolved.value()))
                         ? resolved.value()
                         : mcp_latest_initialization_version()};

  auto capabilities_object{serialize_capabilities(version, capabilities)};

  auto server_info{sourcemeta::core::JSON::make_object()};
  server_info.assign_assume_new("name", sourcemeta::core::JSON{server.name},
                                MCP_HASH_NAME);
  server_info.assign_assume_new(
      "version", sourcemeta::core::JSON{server.version}, MCP_HASH_VERSION);
  if (!server.title.empty() && mcp_supports_implementation_title(version)) {
    server_info.assign_assume_new("title", sourcemeta::core::JSON{server.title},
                                  MCP_HASH_TITLE);
  }
  if (!server.description.empty() &&
      mcp_supports_implementation_description(version)) {
    server_info.assign_assume_new("description",
                                  sourcemeta::core::JSON{server.description},
                                  MCP_HASH_DESCRIPTION);
  }
  if (!server.website_url.empty() &&
      mcp_supports_implementation_website_url(version)) {
    server_info.assign_assume_new("websiteUrl",
                                  sourcemeta::core::JSON{server.website_url},
                                  MCP_HASH_WEBSITE_URL);
  }

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new(
      "protocolVersion",
      sourcemeta::core::JSON{mcp_protocol_version_string(version)},
      MCP_HASH_PROTOCOL_VERSION);
  result.assign_assume_new("capabilities", std::move(capabilities_object),
                           MCP_HASH_CAPABILITIES);
  result.assign_assume_new("serverInfo", std::move(server_info),
                           MCP_HASH_SERVER_INFO);
  if (!instructions.empty()) {
    result.assign_assume_new("instructions",
                             sourcemeta::core::JSON{instructions},
                             MCP_HASH_INSTRUCTIONS);
  }
  return sourcemeta::core::jsonrpc_make_success(*identifier, std::move(result));
}

auto mcp_make_server_discover_result(
    const sourcemeta::core::JSON &identifier,
    const MCPServerCapabilities &capabilities, const MCPImplementation &server,
    const std::vector<JSON::StringView> &supported_versions,
    const JSON::StringView instructions, const MCPCachePolicy &cache_policy)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                           MCP_HASH_RESULT_TYPE);

  auto versions_array{sourcemeta::core::JSON::make_array()};
  for (const auto supported_version : supported_versions) {
    versions_array.push_back(sourcemeta::core::JSON{supported_version});
  }
  result.assign_assume_new("supportedVersions", std::move(versions_array),
                           MCP_HASH_SUPPORTED_VERSIONS);

  result.assign_assume_new(
      "capabilities",
      serialize_capabilities(MCPProtocolVersion::V_2026_07_28, capabilities),
      MCP_HASH_CAPABILITIES);

  auto server_info{sourcemeta::core::JSON::make_object()};
  server_info.assign_assume_new("name", sourcemeta::core::JSON{server.name},
                                MCP_HASH_NAME);
  server_info.assign_assume_new(
      "version", sourcemeta::core::JSON{server.version}, MCP_HASH_VERSION);
  if (!server.title.empty()) {
    server_info.assign_assume_new("title", sourcemeta::core::JSON{server.title},
                                  MCP_HASH_TITLE);
  }
  if (!server.description.empty()) {
    server_info.assign_assume_new("description",
                                  sourcemeta::core::JSON{server.description},
                                  MCP_HASH_DESCRIPTION);
  }
  if (!server.website_url.empty()) {
    server_info.assign_assume_new("websiteUrl",
                                  sourcemeta::core::JSON{server.website_url},
                                  MCP_HASH_WEBSITE_URL);
  }

  auto meta{sourcemeta::core::JSON::make_object()};
  meta.assign_assume_new("io.modelcontextprotocol/serverInfo",
                         std::move(server_info), MCP_HASH_META_SERVER_INFO);
  result.assign_assume_new("_meta", std::move(meta), MCP_HASH_META);

  if (!instructions.empty()) {
    result.assign_assume_new("instructions",
                             sourcemeta::core::JSON{instructions},
                             MCP_HASH_INSTRUCTIONS);
  }

  const auto clamped_ttl{std::max<std::int64_t>(0, cache_policy.ttl_ms)};
  result.assign_assume_new("ttlMs", sourcemeta::core::JSON{clamped_ttl},
                           MCP_HASH_TTL_MS);
  result.assign_assume_new(
      "cacheScope",
      sourcemeta::core::JSON{mcp_cache_scope_string(cache_policy.scope)},
      MCP_HASH_CACHE_SCOPE);

  return sourcemeta::core::jsonrpc_make_success(identifier, std::move(result));
}

auto mcp_make_input_required_result(
    [[maybe_unused]] const MCPProtocolVersion version,
    [[maybe_unused]] const JSON::StringView method,
    const sourcemeta::core::JSON &identifier,
    std::optional<sourcemeta::core::JSON> input_requests,
    std::optional<JSON::StringView> request_state) -> sourcemeta::core::JSON {
  assert(mcp_supports_mrtr(version));
  assert(mcp_is_named_request_method(method));
  assert(input_requests.has_value() || request_state.has_value());

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new("resultType",
                           sourcemeta::core::JSON{"input_required"},
                           MCP_HASH_RESULT_TYPE);

  if (input_requests.has_value()) {
    result.assign_assume_new("inputRequests", std::move(input_requests).value(),
                             MCP_HASH_INPUT_REQUESTS);
  }

  if (request_state.has_value()) {
    result.assign_assume_new("requestState",
                             sourcemeta::core::JSON{request_state.value()},
                             MCP_HASH_REQUEST_STATE);
  }

  return sourcemeta::core::jsonrpc_make_success(identifier, std::move(result));
}

auto mcp_request_input_responses(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON * {
  const auto *parameters{sourcemeta::core::jsonrpc_params(envelope)};
  if (parameters == nullptr || !parameters->is_object()) {
    return nullptr;
  }
  const auto *input_responses_field{
      parameters->try_at("inputResponses", MCP_HASH_INPUT_RESPONSES)};
  if (input_responses_field == nullptr || !input_responses_field->is_object()) {
    return nullptr;
  }
  return input_responses_field;
}

auto mcp_request_state(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView> {
  const auto *parameters{sourcemeta::core::jsonrpc_params(envelope)};
  if (parameters == nullptr || !parameters->is_object()) {
    return std::nullopt;
  }
  const auto *request_state_field{
      parameters->try_at("requestState", MCP_HASH_REQUEST_STATE)};
  if (request_state_field == nullptr || !request_state_field->is_string()) {
    return std::nullopt;
  }
  return request_state_field->to_string();
}

auto mcp_make_subscription_acknowledged_notification(
    const sourcemeta::core::JSON &subscription_id,
    sourcemeta::core::JSON notifications) -> sourcemeta::core::JSON {
  assert(subscription_id.is_string() || subscription_id.is_integer());
  auto meta{sourcemeta::core::JSON::make_object()};
  meta.assign_assume_new("io.modelcontextprotocol/subscriptionId",
                         sourcemeta::core::JSON{subscription_id},
                         MCP_HASH_META_SUBSCRIPTION_ID);

  auto parameters{sourcemeta::core::JSON::make_object()};
  parameters.assign_assume_new("_meta", std::move(meta), MCP_HASH_META);
  parameters.assign_assume_new("notifications", std::move(notifications),
                               MCP_HASH_NOTIFICATIONS);

  auto notification{sourcemeta::core::JSON::make_object()};
  notification.assign_assume_new("jsonrpc", sourcemeta::core::JSON{"2.0"},
                                 MCP_HASH_JSONRPC);
  notification.assign_assume_new(
      "method",
      sourcemeta::core::JSON{
          MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED},
      MCP_HASH_METHOD);
  notification.assign_assume_new("params", std::move(parameters),
                                 MCP_HASH_PARAMS);
  return notification;
}

auto mcp_make_subscription_acknowledged_notification(
    const JSON::StringView subscription_id,
    sourcemeta::core::JSON notifications) -> sourcemeta::core::JSON {
  return mcp_make_subscription_acknowledged_notification(
      sourcemeta::core::JSON{subscription_id}, std::move(notifications));
}

auto mcp_make_subscription_close_result(
    const sourcemeta::core::JSON &identifier) -> sourcemeta::core::JSON {
  auto meta{sourcemeta::core::JSON::make_object()};
  meta.assign_assume_new("io.modelcontextprotocol/subscriptionId",
                         sourcemeta::core::JSON{identifier},
                         MCP_HASH_META_SUBSCRIPTION_ID);

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                           MCP_HASH_RESULT_TYPE);
  result.assign_assume_new("_meta", std::move(meta), MCP_HASH_META);

  return sourcemeta::core::jsonrpc_make_success(identifier, std::move(result));
}

auto mcp_request_subscription_id(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON * {
  if (!envelope.is_object()) {
    return nullptr;
  }
  const sourcemeta::core::JSON *meta = nullptr;
  if (const auto *parameters{envelope.try_at("params", MCP_HASH_PARAMS)};
      parameters != nullptr && parameters->is_object()) {
    meta = parameters->try_at("_meta", MCP_HASH_META);
  } else if (const auto *result{envelope.try_at("result", MCP_HASH_RESULT)};
             result != nullptr && result->is_object()) {
    meta = result->try_at("_meta", MCP_HASH_META);
  } else {
    meta = envelope.try_at("_meta", MCP_HASH_META);
  }

  if (meta == nullptr || !meta->is_object()) {
    return nullptr;
  }

  const auto *sub_id{meta->try_at("io.modelcontextprotocol/subscriptionId",
                                  MCP_HASH_META_SUBSCRIPTION_ID)};
  if (sub_id == nullptr || (!sub_id->is_string() && !sub_id->is_integer())) {
    return nullptr;
  }
  return sub_id;
}

auto mcp_tool_call_arguments(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON * {
  const auto *parameters{sourcemeta::core::jsonrpc_params(envelope)};
  if (parameters == nullptr || !parameters->is_object()) {
    return nullptr;
  }
  const auto *arguments{parameters->try_at("arguments", MCP_HASH_ARGUMENTS)};
  if (arguments == nullptr || !arguments->is_object()) {
    return nullptr;
  }
  return arguments;
}

} // namespace sourcemeta::core
