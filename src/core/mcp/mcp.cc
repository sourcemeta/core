#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <algorithm>   // std::clamp
#include <cassert>     // assert
#include <cmath>       // std::isnan
#include <cstddef>     // std::size_t
#include <cstdint>     // std::int64_t, std::uint32_t
#include <optional>    // std::optional, std::nullopt
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view_literals
#include <utility>     // std::move, std::pair
#include <vector>      // std::vector

namespace {
using namespace std::string_view_literals;

constexpr auto MCP_HASH_ANNOTATIONS{
    sourcemeta::core::JSON::Object::hash("annotations"sv)};
constexpr auto MCP_HASH_ARGUMENTS{
    sourcemeta::core::JSON::Object::hash("arguments"sv)};
constexpr auto MCP_HASH_BODY_VALUE{
    sourcemeta::core::JSON::Object::hash("bodyValue"sv)};
constexpr auto MCP_HASH_CACHE_SCOPE{
    sourcemeta::core::JSON::Object::hash("cacheScope"sv)};
constexpr auto MCP_HASH_CAPABILITIES{
    sourcemeta::core::JSON::Object::hash("capabilities"sv)};
constexpr auto MCP_HASH_COMPLETIONS{
    sourcemeta::core::JSON::Object::hash("completions"sv)};
constexpr auto MCP_HASH_CONTENT{
    sourcemeta::core::JSON::Object::hash("content"sv)};
constexpr auto MCP_HASH_CONTENTS{
    sourcemeta::core::JSON::Object::hash("contents"sv)};
constexpr auto MCP_HASH_CONTEXT{
    sourcemeta::core::JSON::Object::hash("context"sv)};
constexpr auto MCP_HASH_DESCRIPTION{
    sourcemeta::core::JSON::Object::hash("description"sv)};
constexpr auto MCP_HASH_DESTRUCTIVE_HINT{
    sourcemeta::core::JSON::Object::hash("destructiveHint"sv)};
constexpr auto MCP_HASH_ELICITATION{
    sourcemeta::core::JSON::Object::hash("elicitation"sv)};
constexpr auto MCP_HASH_EXPERIMENTAL{
    sourcemeta::core::JSON::Object::hash("experimental"sv)};
constexpr auto MCP_HASH_EXTENSIONS{
    sourcemeta::core::JSON::Object::hash("extensions"sv)};
constexpr auto MCP_HASH_FORM{sourcemeta::core::JSON::Object::hash("form"sv)};
constexpr auto MCP_HASH_HEADER{
    sourcemeta::core::JSON::Object::hash("header"sv)};
constexpr auto MCP_HASH_HEADER_VALUE{
    sourcemeta::core::JSON::Object::hash("headerValue"sv)};
constexpr auto MCP_HASH_IDEMPOTENT_HINT{
    sourcemeta::core::JSON::Object::hash("idempotentHint"sv)};
constexpr auto MCP_HASH_INPUT_REQUESTS{
    sourcemeta::core::JSON::Object::hash("inputRequests"sv)};
constexpr auto MCP_HASH_INPUT_RESPONSES{
    sourcemeta::core::JSON::Object::hash("inputResponses"sv)};
constexpr auto MCP_HASH_INPUT_SCHEMA{
    sourcemeta::core::JSON::Object::hash("inputSchema"sv)};
constexpr auto MCP_HASH_INSTRUCTIONS{
    sourcemeta::core::JSON::Object::hash("instructions"sv)};
constexpr auto MCP_HASH_IS_ERROR{
    sourcemeta::core::JSON::Object::hash("isError"sv)};
constexpr auto MCP_HASH_JSONRPC{
    sourcemeta::core::JSON::Object::hash("jsonrpc"sv)};
constexpr auto MCP_HASH_LIST_CHANGED{
    sourcemeta::core::JSON::Object::hash("listChanged"sv)};
constexpr auto MCP_HASH_LOGGING{
    sourcemeta::core::JSON::Object::hash("logging"sv)};
constexpr auto MCP_HASH_META{sourcemeta::core::JSON::Object::hash("_meta"sv)};
constexpr auto MCP_HASH_META_CLIENT_CAPABILITIES{
    sourcemeta::core::JSON::Object::hash(
        "io.modelcontextprotocol/clientCapabilities"sv)};
constexpr auto MCP_HASH_META_CLIENT_INFO{sourcemeta::core::JSON::Object::hash(
    "io.modelcontextprotocol/clientInfo"sv)};
constexpr auto MCP_HASH_META_LOG_LEVEL{
    sourcemeta::core::JSON::Object::hash("io.modelcontextprotocol/logLevel"sv)};
constexpr auto MCP_HASH_META_PROTOCOL_VERSION{
    sourcemeta::core::JSON::Object::hash(
        "io.modelcontextprotocol/protocolVersion"sv)};
constexpr auto MCP_HASH_META_SERVER_INFO{sourcemeta::core::JSON::Object::hash(
    "io.modelcontextprotocol/serverInfo"sv)};
constexpr auto MCP_HASH_META_SUBSCRIPTION_ID{
    sourcemeta::core::JSON::Object::hash(
        "io.modelcontextprotocol/subscriptionId"sv)};
constexpr auto MCP_HASH_METHOD{
    sourcemeta::core::JSON::Object::hash("method"sv)};
constexpr auto MCP_HASH_MIME_TYPE{
    sourcemeta::core::JSON::Object::hash("mimeType"sv)};
constexpr auto MCP_HASH_NAME{sourcemeta::core::JSON::Object::hash("name"sv)};
constexpr auto MCP_HASH_NEXT_CURSOR{
    sourcemeta::core::JSON::Object::hash("nextCursor"sv)};
constexpr auto MCP_HASH_NOTIFICATIONS{
    sourcemeta::core::JSON::Object::hash("notifications"sv)};
constexpr auto MCP_HASH_OPEN_WORLD_HINT{
    sourcemeta::core::JSON::Object::hash("openWorldHint"sv)};
constexpr auto MCP_HASH_OUTPUT_SCHEMA{
    sourcemeta::core::JSON::Object::hash("outputSchema"sv)};
constexpr auto MCP_HASH_PARAMS{
    sourcemeta::core::JSON::Object::hash("params"sv)};
constexpr auto MCP_HASH_PRIORITY{
    sourcemeta::core::JSON::Object::hash("priority"sv)};
constexpr auto MCP_HASH_PROMPTS{
    sourcemeta::core::JSON::Object::hash("prompts"sv)};
constexpr auto MCP_HASH_PROTOCOL_VERSION{
    sourcemeta::core::JSON::Object::hash("protocolVersion"sv)};
constexpr auto MCP_HASH_READ_ONLY_HINT{
    sourcemeta::core::JSON::Object::hash("readOnlyHint"sv)};
constexpr auto MCP_HASH_REQUEST_STATE{
    sourcemeta::core::JSON::Object::hash("requestState"sv)};
constexpr auto MCP_HASH_REQUESTED{
    sourcemeta::core::JSON::Object::hash("requested"sv)};
constexpr auto MCP_HASH_REQUIRED_CAPABILITIES{
    sourcemeta::core::JSON::Object::hash("requiredCapabilities"sv)};
constexpr auto MCP_HASH_RESOURCE_TEMPLATES{
    sourcemeta::core::JSON::Object::hash("resourceTemplates"sv)};
constexpr auto MCP_HASH_RESOURCES{
    sourcemeta::core::JSON::Object::hash("resources"sv)};
constexpr auto MCP_HASH_RESULT{
    sourcemeta::core::JSON::Object::hash("result"sv)};
constexpr auto MCP_HASH_RESULT_TYPE{
    sourcemeta::core::JSON::Object::hash("resultType"sv)};
constexpr auto MCP_HASH_ROOTS{sourcemeta::core::JSON::Object::hash("roots"sv)};
constexpr auto MCP_HASH_SAMPLING{
    sourcemeta::core::JSON::Object::hash("sampling"sv)};
constexpr auto MCP_HASH_SERVER_INFO{
    sourcemeta::core::JSON::Object::hash("serverInfo"sv)};
constexpr auto MCP_HASH_SIZE{sourcemeta::core::JSON::Object::hash("size"sv)};
constexpr auto MCP_HASH_STRUCTURED_CONTENT{
    sourcemeta::core::JSON::Object::hash("structuredContent"sv)};
constexpr auto MCP_HASH_SUBSCRIBE{
    sourcemeta::core::JSON::Object::hash("subscribe"sv)};
constexpr auto MCP_HASH_SUPPORTED{
    sourcemeta::core::JSON::Object::hash("supported"sv)};
constexpr auto MCP_HASH_SUPPORTED_VERSIONS{
    sourcemeta::core::JSON::Object::hash("supportedVersions"sv)};
constexpr auto MCP_HASH_TEXT{sourcemeta::core::JSON::Object::hash("text"sv)};
constexpr auto MCP_HASH_TITLE{sourcemeta::core::JSON::Object::hash("title"sv)};
constexpr auto MCP_HASH_TOOLS{sourcemeta::core::JSON::Object::hash("tools"sv)};
constexpr auto MCP_HASH_TTL_MS{sourcemeta::core::JSON::Object::hash("ttlMs"sv)};
constexpr auto MCP_HASH_TYPE{sourcemeta::core::JSON::Object::hash("type"sv)};
constexpr auto MCP_HASH_URI{sourcemeta::core::JSON::Object::hash("uri"sv)};
constexpr auto MCP_HASH_URI_TEMPLATE{
    sourcemeta::core::JSON::Object::hash("uriTemplate"sv)};
constexpr auto MCP_HASH_URL{sourcemeta::core::JSON::Object::hash("url"sv)};
constexpr auto MCP_HASH_VERSION{
    sourcemeta::core::JSON::Object::hash("version"sv)};
constexpr auto MCP_HASH_WEBSITE_URL{
    sourcemeta::core::JSON::Object::hash("websiteUrl"sv)};

auto serialize_capabilities(
    const sourcemeta::core::MCPServerCapabilities &capabilities)
    -> sourcemeta::core::JSON {
  auto capabilities_object{sourcemeta::core::JSON::make_object()};

  if (capabilities.logging) {
    capabilities_object.assign_assume_new(
        "logging", sourcemeta::core::JSON::make_object(), MCP_HASH_LOGGING);
  }

  if (capabilities.completions) {
    capabilities_object.assign_assume_new("completions",
                                          sourcemeta::core::JSON::make_object(),
                                          MCP_HASH_COMPLETIONS);
  }

  if (capabilities.prompts || capabilities.prompts_list_changed) {
    auto prompts{sourcemeta::core::JSON::make_object()};
    if (capabilities.prompts_list_changed) {
      prompts.assign_assume_new("listChanged", sourcemeta::core::JSON{true},
                                MCP_HASH_LIST_CHANGED);
    }
    capabilities_object.assign_assume_new("prompts", std::move(prompts),
                                          MCP_HASH_PROMPTS);
  }

  if (capabilities.resources || capabilities.resources_subscribe ||
      capabilities.resources_list_changed) {
    auto resources{sourcemeta::core::JSON::make_object()};
    if (capabilities.resources_subscribe) {
      resources.assign_assume_new("subscribe", sourcemeta::core::JSON{true},
                                  MCP_HASH_SUBSCRIBE);
    }
    if (capabilities.resources_list_changed) {
      resources.assign_assume_new("listChanged", sourcemeta::core::JSON{true},
                                  MCP_HASH_LIST_CHANGED);
    }
    capabilities_object.assign_assume_new("resources", std::move(resources),
                                          MCP_HASH_RESOURCES);
  }

  if (capabilities.tools || capabilities.tools_list_changed) {
    auto tools{sourcemeta::core::JSON::make_object()};
    if (capabilities.tools_list_changed) {
      tools.assign_assume_new("listChanged", sourcemeta::core::JSON{true},
                              MCP_HASH_LIST_CHANGED);
    }
    capabilities_object.assign_assume_new("tools", std::move(tools),
                                          MCP_HASH_TOOLS);
  }

  if (capabilities.extensions.has_value()) {
    capabilities_object.assign_assume_new(
        "extensions", sourcemeta::core::JSON{capabilities.extensions.value()},
        MCP_HASH_EXTENSIONS);
  }

  if (capabilities.experimental.has_value()) {
    capabilities_object.assign_assume_new(
        "experimental",
        sourcemeta::core::JSON{capabilities.experimental.value()},
        MCP_HASH_EXPERIMENTAL);
  }

  return capabilities_object;
}

} // namespace

namespace sourcemeta::core {

auto mcp_supported_protocol_versions() -> std::vector<JSON::StringView> {
  return {"2026-07-28", "2025-11-25", "2025-06-18", "2025-03-26"};
}

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
    auto roots_obj{sourcemeta::core::JSON::make_object()};
    if (capabilities.roots_list_changed &&
        version != MCPProtocolVersion::V_2026_07_28) {
      roots_obj.assign_assume_new("listChanged", sourcemeta::core::JSON{true},
                                  MCP_HASH_LIST_CHANGED);
    }
    result.assign_assume_new("roots", std::move(roots_obj), MCP_HASH_ROOTS);
  }

  if (capabilities.sampling || capabilities.sampling_context ||
      capabilities.sampling_tools) {
    auto sampling_obj{sourcemeta::core::JSON::make_object()};
    if (capabilities.sampling_context) {
      sampling_obj.assign_assume_new(
          "context", sourcemeta::core::JSON::make_object(), MCP_HASH_CONTEXT);
    }
    if (capabilities.sampling_tools) {
      sampling_obj.assign_assume_new(
          "tools", sourcemeta::core::JSON::make_object(), MCP_HASH_TOOLS);
    }
    result.assign_assume_new("sampling", std::move(sampling_obj),
                             MCP_HASH_SAMPLING);
  }

  if (capabilities.elicitation || capabilities.elicitation_form ||
      capabilities.elicitation_url) {
    auto elicitation_obj{sourcemeta::core::JSON::make_object()};
    if (capabilities.elicitation_form) {
      elicitation_obj.assign_assume_new(
          "form", sourcemeta::core::JSON::make_object(), MCP_HASH_FORM);
    }
    if (capabilities.elicitation_url) {
      elicitation_obj.assign_assume_new(
          "url", sourcemeta::core::JSON::make_object(), MCP_HASH_URL);
    }
    result.assign_assume_new("elicitation", std::move(elicitation_obj),
                             MCP_HASH_ELICITATION);
  }

  if (capabilities.extensions.has_value()) {
    result.assign_assume_new(
        "extensions", sourcemeta::core::JSON{capabilities.extensions.value()},
        MCP_HASH_EXTENSIONS);
  }

  if (capabilities.experimental.has_value()) {
    result.assign_assume_new(
        "experimental",
        sourcemeta::core::JSON{capabilities.experimental.value()},
        MCP_HASH_EXPERIMENTAL);
  }

  return result;
}

auto mcp_validate_request_meta(const sourcemeta::core::JSON &envelope_or_params)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>> {
  const sourcemeta::core::JSON *parameters = nullptr;
  const auto *jsonrpc_field{
      envelope_or_params.is_object()
          ? envelope_or_params.try_at("jsonrpc", MCP_HASH_JSONRPC)
          : nullptr};
  if (jsonrpc_field != nullptr && jsonrpc_field->is_string() &&
      jsonrpc_field->to_string() == "2.0") {
    parameters = sourcemeta::core::jsonrpc_params(envelope_or_params);
    if (parameters == nullptr) {
      return {MCPRequestMetaStatus::MissingParams, std::nullopt};
    }
  } else {
    parameters = &envelope_or_params;
  }

  if (!parameters->is_object()) {
    return {MCPRequestMetaStatus::ParamsNotObject, std::nullopt};
  }

  const auto *meta{parameters->try_at("_meta", MCP_HASH_META)};
  if (meta == nullptr) {
    return {MCPRequestMetaStatus::MissingMeta, std::nullopt};
  }
  if (!meta->is_object()) {
    return {MCPRequestMetaStatus::MetaNotObject, std::nullopt};
  }

  const auto *protocol_version_field{
      meta->try_at("io.modelcontextprotocol/protocolVersion",
                   MCP_HASH_META_PROTOCOL_VERSION)};
  if (protocol_version_field == nullptr) {
    return {MCPRequestMetaStatus::MissingProtocolVersion, std::nullopt};
  }
  if (!protocol_version_field->is_string()) {
    return {MCPRequestMetaStatus::ProtocolVersionNotString, std::nullopt};
  }

  const auto protocol_str{protocol_version_field->to_string()};
  if (protocol_str.empty()) {
    return {MCPRequestMetaStatus::UnsupportedProtocolVersion, std::nullopt};
  }

  const auto resolved{mcp_resolve_protocol_version(protocol_str)};
  if (!resolved.has_value()) {
    return {MCPRequestMetaStatus::UnsupportedProtocolVersion, std::nullopt};
  }

  const auto *caps{meta->try_at("io.modelcontextprotocol/clientCapabilities",
                                MCP_HASH_META_CLIENT_CAPABILITIES)};
  if (caps == nullptr) {
    return {MCPRequestMetaStatus::MissingClientCapabilities, std::nullopt};
  }
  if (!caps->is_object()) {
    return {MCPRequestMetaStatus::ClientCapabilitiesNotObject, std::nullopt};
  }

  std::optional<MCPClientInfo> client_info;
  const auto *client_info_field{meta->try_at(
      "io.modelcontextprotocol/clientInfo", MCP_HASH_META_CLIENT_INFO)};
  if (client_info_field != nullptr) {
    if (!client_info_field->is_object()) {
      return {MCPRequestMetaStatus::ClientInfoNotObject, std::nullopt};
    }

    const auto *name_field{client_info_field->try_at("name", MCP_HASH_NAME)};
    if (name_field == nullptr) {
      return {MCPRequestMetaStatus::MissingClientInfoName, std::nullopt};
    }
    if (!name_field->is_string()) {
      return {MCPRequestMetaStatus::ClientInfoNameNotString, std::nullopt};
    }

    const auto *version_field{
        client_info_field->try_at("version", MCP_HASH_VERSION)};
    if (version_field == nullptr) {
      return {MCPRequestMetaStatus::MissingClientInfoVersion, std::nullopt};
    }
    if (!version_field->is_string()) {
      return {MCPRequestMetaStatus::ClientInfoVersionNotString, std::nullopt};
    }

    const auto *title_field{client_info_field->try_at("title", MCP_HASH_TITLE)};
    if (title_field != nullptr && !title_field->is_string()) {
      return {MCPRequestMetaStatus::ClientInfoTitleNotString, std::nullopt};
    }

    const auto *description_field{
        client_info_field->try_at("description", MCP_HASH_DESCRIPTION)};
    if (description_field != nullptr && !description_field->is_string()) {
      return {MCPRequestMetaStatus::ClientInfoDescriptionNotString,
              std::nullopt};
    }

    client_info =
        MCPClientInfo{.name = name_field->to_string(),
                      .version = version_field->to_string(),
                      .title = title_field != nullptr ? title_field->to_string()
                                                      : JSON::StringView{},
                      .description = description_field != nullptr
                                         ? description_field->to_string()
                                         : JSON::StringView{}};
  }

  MCPRequestMeta result_meta;
  result_meta.protocol_version = resolved.value();
  result_meta.client_capabilities = caps;
  result_meta.parsed_client_capabilities = mcp_parse_client_capabilities(*caps);
  result_meta.meta_object = meta;
  result_meta.client_info = client_info;

  const auto *log_level_field{meta->try_at("io.modelcontextprotocol/logLevel",
                                           MCP_HASH_META_LOG_LEVEL)};
  if (log_level_field != nullptr && log_level_field->is_string()) {
    result_meta.log_level = log_level_field->to_string();
  }

  return {MCPRequestMetaStatus::Valid, result_meta};
}

auto mcp_request_protocol_version(const sourcemeta::core::JSON &envelope)
    -> std::optional<MCPProtocolVersion> {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  if (meta.has_value()) {
    return meta->protocol_version;
  }
  return std::nullopt;
}

auto mcp_request_client_info(const sourcemeta::core::JSON &envelope)
    -> std::optional<MCPClientInfo> {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  if (meta.has_value()) {
    return meta->client_info;
  }
  return std::nullopt;
}

auto mcp_request_client_capabilities(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON * {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  if (meta.has_value()) {
    return meta->client_capabilities;
  }
  return nullptr;
}

auto mcp_request_log_level(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView> {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  if (meta.has_value()) {
    return meta->log_level;
  }
  return std::nullopt;
}

auto mcp_has_required_request_meta(const sourcemeta::core::JSON &envelope)
    -> bool {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  return status == MCPRequestMetaStatus::Valid;
}

auto mcp_request_method_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView> {
  const auto method{sourcemeta::core::jsonrpc_method(envelope)};
  if (method.empty()) {
    return std::nullopt;
  }
  return method;
}

auto mcp_request_name_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView> {
  const auto *params{sourcemeta::core::jsonrpc_params(envelope)};
  if (params == nullptr || !params->is_object()) {
    return std::nullopt;
  }

  const auto method{sourcemeta::core::jsonrpc_method(envelope)};
  if (method == MCP_METHOD_RESOURCES_READ) {
    const auto *uri_field{params->try_at("uri", MCP_HASH_URI)};
    if (uri_field != nullptr && uri_field->is_string()) {
      return uri_field->to_string();
    }
    return std::nullopt;
  }

  if (method == MCP_METHOD_TOOLS_CALL || method == MCP_METHOD_PROMPTS_GET) {
    const auto *name_field{params->try_at("name", MCP_HASH_NAME)};
    if (name_field != nullptr && name_field->is_string()) {
      return name_field->to_string();
    }
    return std::nullopt;
  }

  const auto *name_field{params->try_at("name", MCP_HASH_NAME)};
  if (name_field != nullptr && name_field->is_string()) {
    return name_field->to_string();
  }

  const auto *uri_field{params->try_at("uri", MCP_HASH_URI)};
  if (uri_field != nullptr && uri_field->is_string()) {
    return uri_field->to_string();
  }

  return std::nullopt;
}

auto mcp_make_error_request_meta(const sourcemeta::core::JSON *identifier,
                                 const MCPRequestMetaStatus status)
    -> sourcemeta::core::JSON {
  switch (status) {
    case MCPRequestMetaStatus::Valid:
      assert(status != MCPRequestMetaStatus::Valid);
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INTERNAL, "Internal error");
    case MCPRequestMetaStatus::MissingParams:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: missing params object");
    case MCPRequestMetaStatus::ParamsNotObject:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: params must be an object");
    case MCPRequestMetaStatus::MissingMeta:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: missing _meta object");
    case MCPRequestMetaStatus::MetaNotObject:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: _meta must be an object");
    case MCPRequestMetaStatus::MissingProtocolVersion:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: missing protocolVersion in _meta");
    case MCPRequestMetaStatus::ProtocolVersionNotString:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: protocolVersion must be a string");
    case MCPRequestMetaStatus::UnsupportedProtocolVersion:
      return mcp_make_error_unsupported_protocol_version(
          identifier, "", mcp_supported_protocol_versions());
    case MCPRequestMetaStatus::MissingClientCapabilities:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: missing clientCapabilities in _meta");
    case MCPRequestMetaStatus::ClientCapabilitiesNotObject:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientCapabilities must be an object");
    case MCPRequestMetaStatus::ClientInfoNotObject:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientInfo must be an object");
    case MCPRequestMetaStatus::MissingClientInfoName:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: missing name in clientInfo");
    case MCPRequestMetaStatus::ClientInfoNameNotString:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientInfo name must be a string");
    case MCPRequestMetaStatus::MissingClientInfoVersion:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: missing version in clientInfo");
    case MCPRequestMetaStatus::ClientInfoVersionNotString:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientInfo version must be a string");
    case MCPRequestMetaStatus::ClientInfoTitleNotString:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientInfo title must be a string");
    case MCPRequestMetaStatus::ClientInfoDescriptionNotString:
      return sourcemeta::core::jsonrpc_make_error(
          identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientInfo description must be a string");
  }
  std::unreachable();
}

namespace {

constexpr auto mcp_base64_decode_char(const char character) noexcept -> int {
  if (character >= 'A' && character <= 'Z') {
    return character - 'A';
  }
  if (character >= 'a' && character <= 'z') {
    return character - 'a' + 26;
  }
  if (character >= '0' && character <= '9') {
    return character - '0' + 52;
  }
  if (character == '+') {
    return 62;
  }
  if (character == '/') {
    return 63;
  }
  return -1;
}

auto mcp_decode_base64(const std::string_view encoded)
    -> std::optional<std::string> {
  std::string decoded;
  decoded.reserve(encoded.size() * 3 / 4);
  std::uint32_t accumulator = 0;
  int bit_count = -8;
  for (const char character : encoded) {
    if (character == '=') {
      break;
    }
    const int value{mcp_base64_decode_char(character)};
    if (value < 0) {
      return std::nullopt;
    }
    accumulator = (accumulator << 6) | static_cast<std::uint32_t>(value);
    bit_count += 6;
    if (bit_count >= 0) {
      decoded.push_back(static_cast<char>((accumulator >> bit_count) & 0xFF));
      bit_count -= 8;
    }
  }
  return decoded;
}

auto mcp_decode_header_value(const JSON::StringView raw)
    -> std::optional<std::string> {
  if (raw.starts_with("=?base64?") && raw.ends_with("?=") && raw.size() >= 11) {
    const auto payload{raw.substr(9, raw.size() - 11)};
    return mcp_decode_base64(payload);
  }
  return std::string{raw};
}

} // namespace

auto mcp_make_error_unsupported_protocol_version(
    const sourcemeta::core::JSON *identifier, const JSON::StringView requested,
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

  return sourcemeta::core::jsonrpc_make_error(
      identifier, MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
      "Unsupported protocol version", std::move(data));
}

auto mcp_make_error_missing_required_capability(
    const sourcemeta::core::JSON *identifier,
    sourcemeta::core::JSON required_capabilities,
    const JSON::StringView message) -> sourcemeta::core::JSON {
  auto data{sourcemeta::core::JSON::make_object()};
  data.assign_assume_new("requiredCapabilities",
                         std::move(required_capabilities),
                         MCP_HASH_REQUIRED_CAPABILITIES);

  return sourcemeta::core::jsonrpc_make_error(
      identifier, MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY, message,
      std::move(data));
}

auto mcp_make_error_header_mismatch(const sourcemeta::core::JSON *identifier,
                                    const JSON::StringView message)
    -> sourcemeta::core::JSON {
  return sourcemeta::core::jsonrpc_make_error(
      identifier, MCP_CODE_HEADER_MISMATCH, message);
}

auto mcp_make_error_header_mismatch(const sourcemeta::core::JSON *identifier,
                                    const JSON::StringView header_name,
                                    const JSON::StringView header_value,
                                    const JSON::StringView body_value)
    -> sourcemeta::core::JSON {
  auto data{sourcemeta::core::JSON::make_object()};
  data.assign_assume_new("header", sourcemeta::core::JSON{header_name},
                         MCP_HASH_HEADER);
  data.assign_assume_new("headerValue", sourcemeta::core::JSON{header_value},
                         MCP_HASH_HEADER_VALUE);
  data.assign_assume_new("bodyValue", sourcemeta::core::JSON{body_value},
                         MCP_HASH_BODY_VALUE);

  return sourcemeta::core::jsonrpc_make_error(
      identifier, MCP_CODE_HEADER_MISMATCH, "Header mismatch", std::move(data));
}

auto mcp_make_error_header_mismatch(const sourcemeta::core::JSON *identifier,
                                    const JSON::StringView header_name,
                                    const JSON::StringView header_value)
    -> sourcemeta::core::JSON {
  auto data{sourcemeta::core::JSON::make_object()};
  data.assign_assume_new("header", sourcemeta::core::JSON{header_name},
                         MCP_HASH_HEADER);
  data.assign_assume_new("headerValue", sourcemeta::core::JSON{header_value},
                         MCP_HASH_HEADER_VALUE);

  return sourcemeta::core::jsonrpc_make_error(
      identifier, MCP_CODE_HEADER_MISMATCH,
      "Header mismatch: unexpected header provided", std::move(data));
}

auto mcp_validate_request_headers(
    const MCPProtocolVersion version,
    const std::optional<JSON::StringView> &protocol_version_header,
    const std::optional<JSON::StringView> &method_header,
    const std::optional<JSON::StringView> &name_header,
    const sourcemeta::core::JSON &envelope)
    -> std::optional<sourcemeta::core::JSON> {
  if (!envelope.is_object()) {
    return sourcemeta::core::jsonrpc_make_error(
        nullptr, JSONRPC_CODE_INVALID_REQUEST, "Invalid Request");
  }

  const auto *request_id{sourcemeta::core::jsonrpc_request_id(envelope)};
  const auto body_method{mcp_request_method_from_body(envelope)};
  if (!body_method.has_value()) {
    return sourcemeta::core::jsonrpc_make_error(
        request_id, JSONRPC_CODE_INVALID_REQUEST, "Invalid Request");
  }

  const auto *parameters{sourcemeta::core::jsonrpc_params(envelope)};
  const auto is_named{mcp_is_named_request_method(body_method.value())};
  const sourcemeta::core::JSON *name_field{nullptr};
  if (is_named) {
    if (parameters == nullptr || !parameters->is_object()) {
      return sourcemeta::core::jsonrpc_make_error(
          request_id, JSONRPC_CODE_INVALID_PARAMS, "Invalid params");
    }

    if (body_method.value() == MCP_METHOD_RESOURCES_READ) {
      name_field = parameters->try_at("uri", MCP_HASH_URI);
    } else {
      name_field = parameters->try_at("name", MCP_HASH_NAME);
    }

    if (name_field == nullptr || !name_field->is_string()) {
      return sourcemeta::core::jsonrpc_make_error(
          request_id, JSONRPC_CODE_INVALID_PARAMS, "Invalid params");
    }
  }

  if (version == MCPProtocolVersion::V_2026_07_28) {
    if (!protocol_version_header.has_value()) {
      return mcp_make_error_header_mismatch(
          request_id, "Missing required header: MCP-Protocol-Version");
    }

    const auto resolved_protocol{
        mcp_resolve_protocol_version(protocol_version_header.value())};
    if (!resolved_protocol.has_value() ||
        resolved_protocol.value() != version) {
      return mcp_make_error_unsupported_protocol_version(
          request_id, protocol_version_header.value(),
          mcp_supported_protocol_versions());
    }

    std::optional<JSON::StringView> body_metadata_protocol;
    if (parameters != nullptr && parameters->is_object()) {
      const auto *meta{parameters->try_at("_meta", MCP_HASH_META)};
      if (meta != nullptr && meta->is_object()) {
        const auto *protocol_field{
            meta->try_at("io.modelcontextprotocol/protocolVersion",
                         MCP_HASH_META_PROTOCOL_VERSION)};
        if (protocol_field != nullptr && protocol_field->is_string()) {
          body_metadata_protocol = protocol_field->to_string();
        }
      }
    }

    if (!body_metadata_protocol.has_value() ||
        protocol_version_header.value() != body_metadata_protocol.value()) {
      return mcp_make_error_header_mismatch(
          request_id, MCP_HEADER_PROTOCOL_VERSION,
          protocol_version_header.value(), body_metadata_protocol.value_or(""));
    }

    if (!method_header.has_value()) {
      return mcp_make_error_header_mismatch(
          request_id, "Missing required header: Mcp-Method");
    }

    if (method_header.value() != body_method.value()) {
      return mcp_make_error_header_mismatch(request_id, MCP_HEADER_METHOD,
                                            method_header.value(),
                                            body_method.value());
    }

    if (is_named) {
      if (!name_header.has_value()) {
        return mcp_make_error_header_mismatch(
            request_id, "Missing required header: Mcp-Name");
      }

      const auto decoded_name{mcp_decode_header_value(name_header.value())};
      if (!decoded_name.has_value() ||
          decoded_name.value() != name_field->to_string()) {
        return mcp_make_error_header_mismatch(request_id, MCP_HEADER_NAME,
                                              name_header.value(),
                                              name_field->to_string());
      }
    } else {
      if (name_header.has_value()) {
        return mcp_make_error_header_mismatch(request_id, MCP_HEADER_NAME,
                                              name_header.value());
      }
    }

    return std::nullopt;
  }

  // Legacy protocol versions
  if (protocol_version_header.has_value()) {
    const auto resolved_protocol{
        mcp_resolve_protocol_version(protocol_version_header.value())};
    if (!resolved_protocol.has_value()) {
      return mcp_make_error_unsupported_protocol_version(
          request_id, protocol_version_header.value(),
          mcp_supported_protocol_versions());
    }
    if (resolved_protocol.value() != version) {
      return mcp_make_error_header_mismatch(
          request_id, MCP_HEADER_PROTOCOL_VERSION,
          protocol_version_header.value(),
          mcp_protocol_version_string(version));
    }
  }

  if (method_header.has_value()) {
    if (method_header.value() != body_method.value()) {
      return mcp_make_error_header_mismatch(request_id, MCP_HEADER_METHOD,
                                            method_header.value(),
                                            body_method.value());
    }
  }

  if (name_header.has_value()) {
    const auto body_name{mcp_request_name_from_body(envelope)};
    if (body_name.has_value()) {
      if (name_header.value() != body_name.value()) {
        return mcp_make_error_header_mismatch(request_id, MCP_HEADER_NAME,
                                              name_header.value(),
                                              body_name.value());
      }
    }
  }

  return std::nullopt;
}

auto mcp_make_error_resource_not_found(const MCPProtocolVersion version,
                                       const sourcemeta::core::JSON &identifier)
    -> sourcemeta::core::JSON {
  if (version == MCPProtocolVersion::V_2026_07_28) {
    return sourcemeta::core::jsonrpc_make_error(
        &identifier, JSONRPC_CODE_INVALID_PARAMS, "Resource not found");
  }
  return sourcemeta::core::jsonrpc_make_error(
      &identifier, MCP_CODE_RESOURCE_NOT_FOUND, "Resource not found");
}

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
                            const JSON::StringView mime_type,
                            const JSON::StringView name,
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
  if (!name.empty()) {
    block.assign_assume_new("name", sourcemeta::core::JSON{name},
                            MCP_HASH_NAME);
  }
  if (!description.empty()) {
    block.assign_assume_new("description", sourcemeta::core::JSON{description},
                            MCP_HASH_DESCRIPTION);
  }
  block.assign_assume_new("mimeType", sourcemeta::core::JSON{mime_type},
                          MCP_HASH_MIME_TYPE);
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
      auto meta_obj{sourcemeta::core::JSON::make_object()};
      meta_obj.assign_assume_new("io.modelcontextprotocol/serverInfo",
                                 std::move(info), MCP_HASH_META_SERVER_INFO);
      result.assign_assume_new("_meta", std::move(meta_obj), MCP_HASH_META);
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

  result.assign("ttlMs", sourcemeta::core::JSON{cache_policy->ttl_ms});
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

auto mcp_make_resources_read_result(const MCPProtocolVersion version,
                                    sourcemeta::core::JSON contents)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("contents", std::move(contents), MCP_HASH_CONTENTS);
  return result;
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
  if (next_cursor.has_value() && !next_cursor->empty()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_tools_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON tools,
    const std::optional<JSON::StringView> next_cursor)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("tools", std::move(tools), MCP_HASH_TOOLS);
  if (next_cursor.has_value() && !next_cursor->empty()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return result;
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
  if (next_cursor.has_value() && !next_cursor->empty()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_resources_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resources,
    const std::optional<JSON::StringView> next_cursor)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("resources", std::move(resources),
                           MCP_HASH_RESOURCES);
  if (next_cursor.has_value() && !next_cursor->empty()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return result;
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
  if (next_cursor.has_value() && !next_cursor->empty()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_resource_templates_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON resource_templates,
    const std::optional<JSON::StringView> next_cursor)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("resourceTemplates", std::move(resource_templates),
                           MCP_HASH_RESOURCE_TEMPLATES);
  if (next_cursor.has_value() && !next_cursor->empty()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return result;
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
  if (next_cursor.has_value() && !next_cursor->empty()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return mcp_decorate_cacheable_result(version, std::move(result),
                                       cache_policy);
}

auto mcp_make_prompts_list_result(
    const MCPProtocolVersion version, sourcemeta::core::JSON prompts,
    const std::optional<JSON::StringView> next_cursor)
    -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  if (version == MCPProtocolVersion::V_2026_07_28) {
    result.assign_assume_new("resultType", sourcemeta::core::JSON{"complete"},
                             MCP_HASH_RESULT_TYPE);
  }
  result.assign_assume_new("prompts", std::move(prompts), MCP_HASH_PROMPTS);
  if (next_cursor.has_value() && !next_cursor->empty()) {
    result.assign_assume_new("nextCursor",
                             sourcemeta::core::JSON{next_cursor.value()},
                             MCP_HASH_NEXT_CURSOR);
  }
  return result;
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
  assert(!annotations.read_only || !annotations.destructive);
  assert(!annotations.read_only || annotations.idempotent);
  // The MCP spec requires `type: "object"` on tool input schemas.
  const auto *type_field{input_schema.is_object()
                             ? input_schema.try_at("type", MCP_HASH_TYPE)
                             : nullptr};
  assert(type_field != nullptr && type_field->is_string() &&
         type_field->to_string() == "object");

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
  // MCP 2026-07-28 removed initialize handshake.
  // For initialize negotiation, fallback must only negotiate
  // handshake-compatible revisions.
  const auto version{(resolved.has_value() &&
                      mcp_uses_initialization_handshake(resolved.value()))
                         ? resolved.value()
                         : mcp_latest_initialization_version()};

  auto capabilities_object{serialize_capabilities(capabilities)};

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

  result.assign_assume_new("capabilities", serialize_capabilities(capabilities),
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

  result.assign_assume_new("ttlMs", sourcemeta::core::JSON{cache_policy.ttl_ms},
                           MCP_HASH_TTL_MS);
  result.assign_assume_new(
      "cacheScope",
      sourcemeta::core::JSON{mcp_cache_scope_string(cache_policy.scope)},
      MCP_HASH_CACHE_SCOPE);

  return sourcemeta::core::jsonrpc_make_success(identifier, std::move(result));
}

auto mcp_make_input_required_result(
    const MCPProtocolVersion version, const JSON::StringView method,
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
  const auto *params{sourcemeta::core::jsonrpc_params(envelope)};
  if (params == nullptr || !params->is_object()) {
    return nullptr;
  }
  const auto *input_responses_field{
      params->try_at("inputResponses", MCP_HASH_INPUT_RESPONSES)};
  if (input_responses_field == nullptr || !input_responses_field->is_object()) {
    return nullptr;
  }
  return input_responses_field;
}

auto mcp_request_state(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView> {
  const auto *params{sourcemeta::core::jsonrpc_params(envelope)};
  if (params == nullptr || !params->is_object()) {
    return std::nullopt;
  }
  const auto *request_state_field{
      params->try_at("requestState", MCP_HASH_REQUEST_STATE)};
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

  auto params{sourcemeta::core::JSON::make_object()};
  params.assign_assume_new("_meta", std::move(meta), MCP_HASH_META);
  params.assign_assume_new("notifications", std::move(notifications),
                           MCP_HASH_NOTIFICATIONS);

  auto notification{sourcemeta::core::JSON::make_object()};
  notification.assign_assume_new("jsonrpc", sourcemeta::core::JSON{"2.0"},
                                 MCP_HASH_JSONRPC);
  notification.assign_assume_new(
      "method",
      sourcemeta::core::JSON{
          MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED},
      MCP_HASH_METHOD);
  notification.assign_assume_new("params", std::move(params), MCP_HASH_PARAMS);
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
  if (const auto *params{envelope.try_at("params", MCP_HASH_PARAMS)};
      params != nullptr && params->is_object()) {
    meta = params->try_at("_meta", MCP_HASH_META);
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
