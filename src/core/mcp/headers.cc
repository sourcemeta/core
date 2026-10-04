#include <sourcemeta/core/mcp.h>

#include "helpers.h"
#include "validation.h"

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/unicode.h>

#include <algorithm>
#include <array>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace {

auto plain_header_value(const sourcemeta::core::JSON::StringView value) noexcept
    -> bool {
  if (!value.empty() && (value.front() == ' ' || value.back() == ' ')) {
    return false;
  }
  return std::all_of(value.begin(), value.end(), [](const char character) {
    const auto byte{static_cast<unsigned char>(character)};
    return byte >= 0x20 && byte < 0x7f;
  });
}

auto encoded_header_value(
    const sourcemeta::core::JSON::StringView value) noexcept -> bool {
  return value.starts_with("=?base64?") && value.ends_with("?=") &&
         value.size() >= 11;
}

auto header_matches(const sourcemeta::core::JSON::StringView header,
                    const sourcemeta::core::JSON::StringView body) -> bool {
  if (!plain_header_value(header)) {
    return false;
  }
  if (encoded_header_value(header)) {
    const auto decoded{
        sourcemeta::core::base64_decode(header.substr(9, header.size() - 11))};
    return decoded && sourcemeta::core::is_valid_utf8(*decoded) &&
           *decoded == body;
  }
  return plain_header_value(body) && header == body;
}

auto transport_error(const sourcemeta::core::MCPProtocolVersion version,
                     const sourcemeta::core::JSON *identifier,
                     const std::int64_t code,
                     const sourcemeta::core::JSON::StringView message,
                     std::optional<sourcemeta::core::JSON> data = std::nullopt)
    -> sourcemeta::core::JSON {
  return sourcemeta::core::mcp_make_error(
      version, identifier, code, message, std::move(data),
      sourcemeta::core::MCPErrorContext::Transport);
}

} // namespace

namespace sourcemeta::core {

auto mcp_validate_request_meta(const JSON &envelope)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>> {
  if (!envelope.is_object()) {
    return {MCPRequestMetaStatus::InvalidEnvelope, std::nullopt};
  }
  const auto *rpc{envelope.try_at("jsonrpc", MCP_HASH_JSONRPC)};
  const auto *method{envelope.try_at("method", MCP_HASH_METHOD)};
  const auto *identifier{envelope.try_at("id", MCP_HASH_ID)};
  if (!rpc || !rpc->is_string() || rpc->to_string() != "2.0" || !method ||
      !method->is_string() || !identifier || !internal::valid_id(*identifier) ||
      envelope.defines("result") || envelope.defines("error")) {
    return {MCPRequestMetaStatus::InvalidEnvelope, std::nullopt};
  }
  const auto *parameters{envelope.try_at("params", MCP_HASH_PARAMS)};
  if (!parameters) {
    return {MCPRequestMetaStatus::MissingParams, std::nullopt};
  }
  return mcp_validate_request_parameters(*parameters);
}

auto mcp_validate_request_parameters(const JSON &parameters_value)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>> {
  const auto *parameters{&parameters_value};
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

  if (!internal::valid_metadata_object(*meta)) {
    return {MCPRequestMetaStatus::InvalidMetaKey, std::nullopt};
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

  const JSON::StringView protocol_str{protocol_version_field->to_string()};
  if (protocol_str.empty()) {
    return {MCPRequestMetaStatus::UnsupportedProtocolVersion, std::nullopt};
  }

  const auto resolved{mcp_resolve_protocol_version(protocol_str)};
  if (!resolved.has_value() || !mcp_requires_request_meta(*resolved)) {
    return {MCPRequestMetaStatus::UnsupportedProtocolVersion, std::nullopt};
  }

  const auto *client_capabilities{
      meta->try_at("io.modelcontextprotocol/clientCapabilities",
                   MCP_HASH_META_CLIENT_CAPABILITIES)};
  if (client_capabilities == nullptr) {
    return {MCPRequestMetaStatus::MissingClientCapabilities, std::nullopt};
  }
  if (!client_capabilities->is_object()) {
    return {MCPRequestMetaStatus::ClientCapabilitiesNotObject, std::nullopt};
  }

  if (!internal::valid_capabilities(resolved.value(), *client_capabilities,
                                    true)) {
    return {MCPRequestMetaStatus::InvalidClientCapabilities, std::nullopt};
  }
  std::optional<MCPImplementation> client_info;
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

    MCPImplementation info;
    info.name = name_field->to_string();
    info.version = version_field->to_string();

    const auto *title_field{client_info_field->try_at("title", MCP_HASH_TITLE)};
    if (title_field != nullptr) {
      if (!title_field->is_string()) {
        return {MCPRequestMetaStatus::ClientInfoTitleNotString, std::nullopt};
      }
      info.title = title_field->to_string();
    }

    const auto *description_field{
        client_info_field->try_at("description", MCP_HASH_DESCRIPTION)};
    if (description_field != nullptr) {
      if (!description_field->is_string()) {
        return {MCPRequestMetaStatus::ClientInfoDescriptionNotString,
                std::nullopt};
      }
      info.description = description_field->to_string();
    }

    const auto *website_url_field{
        client_info_field->try_at("websiteUrl", MCP_HASH_WEBSITE_URL)};
    if (website_url_field != nullptr) {
      if (!website_url_field->is_string()) {
        return {MCPRequestMetaStatus::ClientInfoWebsiteUrlNotString,
                std::nullopt};
      }
      info.website_url = website_url_field->to_string();
    }
    if (!internal::valid_implementation(resolved.value(), *client_info_field)) {
      return {MCPRequestMetaStatus::InvalidClientInfo, std::nullopt};
    }

    info.icons = client_info_field->try_at("icons");
    client_info = info;
  }

  std::optional<JSON::StringView> log_level;
  const auto *log_level_field{meta->try_at("io.modelcontextprotocol/logLevel",
                                           MCP_HASH_META_LOG_LEVEL)};
  if (log_level_field != nullptr) {
    if (!log_level_field->is_string() ||
        !internal::valid_log_level(log_level_field->to_string())) {
      return {MCPRequestMetaStatus::InvalidLogLevel, std::nullopt};
    }
    log_level = log_level_field->to_string();
  }
  const auto *progress_token{meta->try_at("progressToken")};
  if (progress_token && !internal::valid_id(*progress_token)) {
    return {MCPRequestMetaStatus::InvalidProgressToken, std::nullopt};
  }
  return {MCPRequestMetaStatus::Valid,
          MCPRequestMeta{resolved.value(), *client_capabilities, client_info,
                         log_level, *meta, progress_token}};
}

auto mcp_make_error_request_meta(
    const MCPProtocolVersion version,
    const std::optional<sourcemeta::core::JSON> &identifier,
    const MCPRequestMetaStatus status, const JSON::StringView requested,
    const std::span<const JSON::StringView> supported)
    -> sourcemeta::core::JSON {
  internal::require(version == MCPProtocolVersion::V_2026_07_28,
                    "Stateless metadata errors require MCP 2026-07-28");
  const auto *const identifier_pointer{
      identifier.has_value() ? &identifier.value() : nullptr};
  switch (status) {
    case MCPRequestMetaStatus::Valid:
      throw std::invalid_argument{
          "Cannot construct an error for valid metadata"};
    case MCPRequestMetaStatus::InvalidEnvelope:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_REQUEST, "Invalid Request");
    case MCPRequestMetaStatus::InvalidMetaKey:
    case MCPRequestMetaStatus::InvalidClientCapabilities:
    case MCPRequestMetaStatus::ClientInfoWebsiteUrlNotString:
    case MCPRequestMetaStatus::InvalidClientInfo:
    case MCPRequestMetaStatus::InvalidLogLevel:
    case MCPRequestMetaStatus::InvalidProgressToken:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: malformed metadata field");
    case MCPRequestMetaStatus::MissingParams:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: missing params object");
    case MCPRequestMetaStatus::ParamsNotObject:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: params must be an object");
    case MCPRequestMetaStatus::MissingMeta:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: missing _meta object");
    case MCPRequestMetaStatus::MetaNotObject:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: _meta must be an object");
    case MCPRequestMetaStatus::MissingProtocolVersion:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: missing protocolVersion in _meta");
    case MCPRequestMetaStatus::ProtocolVersionNotString:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: protocolVersion must be a string");
    case MCPRequestMetaStatus::UnsupportedProtocolVersion:
      return mcp_make_error_unsupported_protocol_version(version, identifier,
                                                         requested, supported);
    case MCPRequestMetaStatus::MissingClientCapabilities:
      return mcp_make_error(
          version, identifier_pointer, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: missing clientCapabilities in _meta");
    case MCPRequestMetaStatus::ClientCapabilitiesNotObject:
      return mcp_make_error(
          version, identifier_pointer, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientCapabilities must be an object");
    case MCPRequestMetaStatus::ClientInfoNotObject:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: clientInfo must be an object");
    case MCPRequestMetaStatus::MissingClientInfoName:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: missing name in clientInfo");
    case MCPRequestMetaStatus::ClientInfoNameNotString:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: clientInfo name must be a string");
    case MCPRequestMetaStatus::MissingClientInfoVersion:
      return mcp_make_error(version, identifier_pointer,
                            JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid params: missing version in clientInfo");
    case MCPRequestMetaStatus::ClientInfoVersionNotString:
      return mcp_make_error(
          version, identifier_pointer, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientInfo version must be a string");
    case MCPRequestMetaStatus::ClientInfoTitleNotString:
      return mcp_make_error(
          version, identifier_pointer, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientInfo title must be a string");
    case MCPRequestMetaStatus::ClientInfoDescriptionNotString:
      return mcp_make_error(
          version, identifier_pointer, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: clientInfo description must be a string");
  }
  std::unreachable();
}

auto mcp_has_required_request_meta(const sourcemeta::core::JSON &envelope)
    -> bool {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  return status == MCPRequestMetaStatus::Valid;
}

auto mcp_request_method_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView> {
  if (!envelope.is_object()) {
    return std::nullopt;
  }
  const auto *method_field{envelope.try_at("method", MCP_HASH_METHOD)};
  if (method_field != nullptr && method_field->is_string()) {
    return method_field->to_string();
  }
  return std::nullopt;
}

auto mcp_request_name_from_body(const sourcemeta::core::JSON &envelope)
    -> std::optional<std::string> {
  if (!envelope.is_object()) {
    return std::nullopt;
  }

  const auto *parameters{envelope.try_at("params", MCP_HASH_PARAMS)};
  if (parameters == nullptr || !parameters->is_object()) {
    return std::nullopt;
  }

  const auto method{mcp_request_method_from_body(envelope)};
  if (method.has_value() &&
      (method.value() == MCP_METHOD_RESOURCES_READ ||
       method.value() == MCP_METHOD_RESOURCES_SUBSCRIBE ||
       method.value() == MCP_METHOD_RESOURCES_UNSUBSCRIBE)) {
    const auto *uri_field{parameters->try_at("uri", MCP_HASH_URI)};
    if (uri_field != nullptr && uri_field->is_string()) {
      return uri_field->to_string();
    }
    return std::nullopt;
  }

  const auto *name_field{parameters->try_at("name", MCP_HASH_NAME)};
  if (name_field != nullptr && name_field->is_string()) {
    return name_field->to_string();
  }

  const auto *uri_field{parameters->try_at("uri", MCP_HASH_URI)};
  if (uri_field != nullptr && uri_field->is_string()) {
    return uri_field->to_string();
  }

  return std::nullopt;
}

auto mcp_encode_header_value(const JSON::StringView value) -> std::string {
  internal::require(is_valid_utf8(value), "MCP header values must be UTF-8");
  if (plain_header_value(value) && !encoded_header_value(value)) {
    return std::string{value};
  }
  return "=?base64?" + sourcemeta::core::base64_encode(value) + "?=";
}

auto mcp_validate_request_headers(
    const MCPProtocolVersion version,
    const std::optional<JSON::StringView> &protocol_version_header,
    const std::optional<JSON::StringView> &method_header,
    const std::optional<JSON::StringView> &name_header, const JSON &envelope,
    const std::span<const JSON::StringView> supported_versions)
    -> std::optional<JSON> {
  internal::require(!supported_versions.empty(),
                    "Supported protocol versions must be explicit");
  const auto *identifier{
      envelope.is_object() ? envelope.try_at("id", MCP_HASH_ID) : nullptr};
  const bool valid_identifier{identifier && internal::valid_id(*identifier)};
  const auto method{mcp_request_method_from_body(envelope)};
  const auto *rpc{envelope.is_object()
                      ? envelope.try_at("jsonrpc", MCP_HASH_JSONRPC)
                      : nullptr};
  if (!rpc || !rpc->is_string() || rpc->to_string() != "2.0" || !method ||
      envelope.defines("result") || envelope.defines("error") ||
      (envelope.defines("id") && !valid_identifier)) {
    return transport_error(version, identifier, JSONRPC_CODE_INVALID_REQUEST,
                           "Invalid Request");
  }

  if (!mcp_requires_request_meta(version)) {
    // Mcp-Method and Mcp-Name are not routing fields in legacy revisions.
    if (!protocol_version_header) {
      return std::nullopt;
    }
    if (!plain_header_value(*protocol_version_header)) {
      return transport_error(version, identifier, JSONRPC_CODE_INVALID_REQUEST,
                             "Invalid protocol version header");
    }
    if (std::find(supported_versions.begin(), supported_versions.end(),
                  *protocol_version_header) == supported_versions.end()) {
      return transport_error(version, identifier, JSONRPC_CODE_INVALID_REQUEST,
                             "Unsupported protocol version");
    }
    if (*protocol_version_header != mcp_protocol_version_string(version)) {
      return transport_error(version, identifier, JSONRPC_CODE_INVALID_REQUEST,
                             "Protocol version disagrees with session");
    }
    return std::nullopt;
  }

  const auto *parameters{envelope.try_at("params", MCP_HASH_PARAMS)};
  if (!parameters || !parameters->is_object()) {
    return transport_error(version, identifier, JSONRPC_CODE_INVALID_PARAMS,
                           "Invalid params");
  }
  // Stateless metadata is required on requests, not notifications.
  std::optional<JSON::StringView> body_version;
  if (identifier) {
    const auto *meta{parameters->try_at("_meta", MCP_HASH_META)};
    const auto *field{
        meta && meta->is_object()
            ? meta->try_at("io.modelcontextprotocol/protocolVersion",
                           MCP_HASH_META_PROTOCOL_VERSION)
            : nullptr};
    if (!field || !field->is_string()) {
      return transport_error(
          version, identifier, JSONRPC_CODE_INVALID_PARAMS,
          "Invalid params: missing or malformed protocol metadata");
    }
    body_version = field->to_string();
  }
  if (identifier) {
    const auto status{mcp_validate_request_meta(envelope).first};
    // Preserve raw version comparison precedence for an unknown revision.
    if (status != MCPRequestMetaStatus::Valid &&
        status != MCPRequestMetaStatus::UnsupportedProtocolVersion) {
      return mcp_make_error_request_meta(version, *identifier, status,
                                         body_version.value_or(""),
                                         supported_versions);
    }
  }
  if (!protocol_version_header ||
      !plain_header_value(*protocol_version_header)) {
    return mcp_make_error_header_mismatch(
        version, identifier ? std::optional<JSON>{*identifier} : std::nullopt,
        "Missing or malformed MCP-Protocol-Version");
  }
  // Mirrored values must agree before checking whether the revision is
  // supported.
  if (body_version && *body_version != *protocol_version_header) {
    return mcp_make_error_header_mismatch(
        version, identifier ? std::optional<JSON>{*identifier} : std::nullopt,
        MCP_HEADER_PROTOCOL_VERSION, *protocol_version_header, *body_version);
  }
  if (std::find(supported_versions.begin(), supported_versions.end(),
                *protocol_version_header) == supported_versions.end()) {
    return mcp_make_error_unsupported_protocol_version(
        version, identifier ? std::optional<JSON>{*identifier} : std::nullopt,
        *protocol_version_header, supported_versions);
  }
  if (*protocol_version_header != mcp_protocol_version_string(version)) {
    return mcp_make_error_header_mismatch(
        version, identifier ? std::optional<JSON>{*identifier} : std::nullopt,
        MCP_HEADER_PROTOCOL_VERSION, *protocol_version_header,
        mcp_protocol_version_string(version));
  }
  if (identifier) {
    const auto [status, meta]{mcp_validate_request_meta(envelope)};
    if (status != MCPRequestMetaStatus::Valid) {
      return mcp_make_error_request_meta(version, *identifier, status,
                                         *protocol_version_header,
                                         supported_versions);
    }
  }
  if (!method_header || !plain_header_value(*method_header)) {
    return mcp_make_error_header_mismatch(
        version, identifier ? std::optional<JSON>{*identifier} : std::nullopt,
        "Missing or malformed Mcp-Method");
  }
  if (*method_header != *method) {
    return mcp_make_error_header_mismatch(
        version, identifier ? std::optional<JSON>{*identifier} : std::nullopt,
        MCP_HEADER_METHOD, *method_header, *method);
  }
  const bool named{mcp_is_mrtr_method(*method)};
  if (named) {
    const auto *name{parameters->try_at(
        *method == MCP_METHOD_RESOURCES_READ ? "uri" : "name")};
    if (!name || !name->is_string()) {
      return transport_error(version, identifier, JSONRPC_CODE_INVALID_PARAMS,
                             "Invalid params");
    }
    if (!name_header || !header_matches(*name_header, name->to_string())) {
      return mcp_make_error_header_mismatch(
          version, identifier ? std::optional<JSON>{*identifier} : std::nullopt,
          MCP_HEADER_NAME, name_header.value_or(""), name->to_string());
    }
  } else if (name_header) {
    return mcp_make_error_header_mismatch(
        version, identifier ? std::optional<JSON>{*identifier} : std::nullopt,
        MCP_HEADER_NAME, *name_header);
  }
  return std::nullopt;
}

auto mcp_validate_request_headers(
    const MCPProtocolVersion version,
    const std::span<const std::pair<JSON::StringView, JSON::StringView>>
        headers,
    const JSON &envelope,
    const std::span<const JSON::StringView> supported_versions)
    -> std::optional<JSON> {
  std::optional<JSON::StringView> protocol;
  std::optional<JSON::StringView> method;
  std::optional<JSON::StringView> name;
  for (const auto &entry : headers) {
    std::optional<JSON::StringView> *slot = nullptr;
    if (equals_ignore_case(entry.first, MCP_HEADER_PROTOCOL_VERSION)) {
      slot = &protocol;
    } else if (version == MCPProtocolVersion::V_2026_07_28 &&
               equals_ignore_case(entry.first, MCP_HEADER_METHOD)) {
      slot = &method;
    } else if (version == MCPProtocolVersion::V_2026_07_28 &&
               equals_ignore_case(entry.first, MCP_HEADER_NAME)) {
      slot = &name;
    }
    if (!slot) {
      continue;
    }
    if (slot->has_value()) {
      const auto *identifier{envelope.is_object() ? envelope.try_at("id")
                                                  : nullptr};
      return version == MCPProtocolVersion::V_2026_07_28
                 ? mcp_make_error_header_mismatch(
                       version,
                       identifier ? std::optional<JSON>{*identifier}
                                  : std::nullopt,
                       "Duplicate routing header")
                 : transport_error(version, identifier,
                                   JSONRPC_CODE_INVALID_REQUEST,
                                   "Duplicate protocol header");
    }
    *slot = entry.second;
  }
  return mcp_validate_request_headers(version, protocol, method, name, envelope,
                                      supported_versions);
}

} // namespace sourcemeta::core
