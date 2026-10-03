#include <sourcemeta/core/mcp_capabilities.h>
#include <sourcemeta/core/mcp_error.h>
#include <sourcemeta/core/mcp_headers.h>
#include <sourcemeta/core/mcp_protocol.h>

#include "helpers.h"

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <cassert>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

auto mcp_is_valid_header_characters(
    const sourcemeta::core::JSON::StringView raw_header) noexcept -> bool {
  for (const char character : raw_header) {
    const auto unsigned_character{static_cast<unsigned char>(character)};
    if (unsigned_character != 0x09 &&
        (unsigned_character < 0x20 || unsigned_character > 0x7E)) {
      return false;
    }
  }
  return true;
}

auto mcp_decode_header_value(
    const sourcemeta::core::JSON::StringView raw_header)
    -> std::optional<std::string> {
  if (!mcp_is_valid_header_characters(raw_header)) {
    return std::nullopt;
  }
  if (raw_header.starts_with("=?base64?") && raw_header.ends_with("?=") &&
      raw_header.size() >= 11) {
    const auto payload{raw_header.substr(9, raw_header.size() - 11)};
    return sourcemeta::core::base64_decode(payload);
  }
  return std::string{raw_header};
}

} // namespace

namespace sourcemeta::core {

auto mcp_validate_request_meta(
    const sourcemeta::core::JSON &envelope_or_parameters)
    -> std::pair<MCPRequestMetaStatus, std::optional<MCPRequestMeta>> {
  const sourcemeta::core::JSON *parameters = nullptr;
  const auto *jsonrpc_field{
      envelope_or_parameters.try_at("jsonrpc", MCP_HASH_JSONRPC)};
  const auto *method_field{
      envelope_or_parameters.try_at("method", MCP_HASH_METHOD)};
  const bool is_envelope{
      jsonrpc_field != nullptr && jsonrpc_field->is_string() &&
      jsonrpc_field->to_string() == "2.0" && method_field != nullptr &&
      method_field->is_string() && !envelope_or_parameters.defines("_meta")};

  if (is_envelope) {
    parameters = envelope_or_parameters.try_at("params", MCP_HASH_PARAMS);
    if (parameters == nullptr) {
      return {MCPRequestMetaStatus::MissingParams, std::nullopt};
    }
  } else {
    parameters = &envelope_or_parameters;
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

  const auto *client_capabilities{
      meta->try_at("io.modelcontextprotocol/clientCapabilities",
                   MCP_HASH_META_CLIENT_CAPABILITIES)};
  if (client_capabilities == nullptr) {
    return {MCPRequestMetaStatus::MissingClientCapabilities, std::nullopt};
  }
  if (!client_capabilities->is_object()) {
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

    MCPClientInfo info;
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
    if (website_url_field != nullptr && website_url_field->is_string()) {
      info.website_url = website_url_field->to_string();
    }

    client_info = info;
  }

  std::optional<JSON::StringView> log_level;
  const auto *log_level_field{meta->try_at("io.modelcontextprotocol/logLevel",
                                           MCP_HASH_META_LOG_LEVEL)};
  if (log_level_field != nullptr && log_level_field->is_string()) {
    log_level = log_level_field->to_string();
  }

  MCPRequestMeta result;
  result.protocol_version = resolved.value();
  result.client_capabilities = client_capabilities;
  result.parsed_client_capabilities =
      mcp_parse_client_capabilities(*client_capabilities);
  result.client_info = client_info;
  result.log_level = log_level;
  result.meta_object = meta;

  return {MCPRequestMetaStatus::Valid, result};
}

auto mcp_make_error_request_meta(const sourcemeta::core::JSON &identifier,
                                 const MCPRequestMetaStatus status,
                                 const JSON::StringView requested,
                                 const std::vector<JSON::StringView> &supported)
    -> sourcemeta::core::JSON {
  auto envelope = [&]() -> sourcemeta::core::JSON {
    switch (status) {
      case MCPRequestMetaStatus::Valid:
        assert(status != MCPRequestMetaStatus::Valid);
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INTERNAL, "Internal error");
      case MCPRequestMetaStatus::MissingParams:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: missing params object");
      case MCPRequestMetaStatus::ParamsNotObject:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: params must be an object");
      case MCPRequestMetaStatus::MissingMeta:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: missing _meta object");
      case MCPRequestMetaStatus::MetaNotObject:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: _meta must be an object");
      case MCPRequestMetaStatus::MissingProtocolVersion:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: missing protocolVersion in _meta");
      case MCPRequestMetaStatus::ProtocolVersionNotString:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: protocolVersion must be a string");
      case MCPRequestMetaStatus::UnsupportedProtocolVersion:
        return mcp_make_error_unsupported_protocol_version(
            identifier, requested,
            supported.empty()
                ? std::vector<JSON::StringView>{MCP_PROTOCOL_VERSION_2026_07_28}
                : supported);
      case MCPRequestMetaStatus::MissingClientCapabilities:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: missing clientCapabilities in _meta");
      case MCPRequestMetaStatus::ClientCapabilitiesNotObject:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: clientCapabilities must be an object");
      case MCPRequestMetaStatus::ClientInfoNotObject:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: clientInfo must be an object");
      case MCPRequestMetaStatus::MissingClientInfoName:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: missing name in clientInfo");
      case MCPRequestMetaStatus::ClientInfoNameNotString:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: clientInfo name must be a string");
      case MCPRequestMetaStatus::MissingClientInfoVersion:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: missing version in clientInfo");
      case MCPRequestMetaStatus::ClientInfoVersionNotString:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: clientInfo version must be a string");
      case MCPRequestMetaStatus::ClientInfoTitleNotString:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: clientInfo title must be a string");
      case MCPRequestMetaStatus::ClientInfoDescriptionNotString:
        return sourcemeta::core::jsonrpc_make_error(
            &identifier, JSONRPC_CODE_INVALID_PARAMS,
            "Invalid params: clientInfo description must be a string");
    }
    std::unreachable();
  }();
  if (identifier.is_null()) {
    envelope.erase("id", MCP_HASH_ID);
  }
  return envelope;
}

auto mcp_request_protocol_version(const sourcemeta::core::JSON &envelope)
    -> std::optional<MCPProtocolVersion> {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  if (status == MCPRequestMetaStatus::Valid && meta.has_value()) {
    return meta->protocol_version;
  }
  return std::nullopt;
}

auto mcp_request_client_info(const sourcemeta::core::JSON &envelope)
    -> std::optional<MCPClientInfo> {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  if (status == MCPRequestMetaStatus::Valid && meta.has_value()) {
    return meta->client_info;
  }
  return std::nullopt;
}

auto mcp_request_client_capabilities(const sourcemeta::core::JSON &envelope)
    -> const sourcemeta::core::JSON * {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  if (status == MCPRequestMetaStatus::Valid && meta.has_value()) {
    return meta->client_capabilities;
  }
  return nullptr;
}

auto mcp_request_log_level(const sourcemeta::core::JSON &envelope)
    -> std::optional<JSON::StringView> {
  const auto [status, meta]{mcp_validate_request_meta(envelope)};
  if (status == MCPRequestMetaStatus::Valid && meta.has_value()) {
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
  if (method.has_value() && method.value() == MCP_METHOD_RESOURCES_READ) {
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

auto mcp_validate_request_headers(
    const MCPProtocolVersion version,
    const std::optional<JSON::StringView> &protocol_version_header,
    const std::optional<JSON::StringView> &method_header,
    const std::optional<JSON::StringView> &name_header,
    const sourcemeta::core::JSON &envelope)
    -> std::optional<sourcemeta::core::JSON> {
  const auto *raw_identifier{sourcemeta::core::jsonrpc_request_id(envelope)};
  auto make_error = [&](const std::int64_t code, const JSON::StringView message,
                        std::optional<sourcemeta::core::JSON> data =
                            std::nullopt) -> sourcemeta::core::JSON {
    auto error_envelope{sourcemeta::core::jsonrpc_make_error(
        raw_identifier, code, message, std::move(data))};
    if (raw_identifier == nullptr || raw_identifier->is_null()) {
      error_envelope.erase("id", MCP_HASH_ID);
    }
    return error_envelope;
  };

  if (!envelope.is_object()) {
    return make_error(JSONRPC_CODE_INVALID_REQUEST, "Invalid Request");
  }

  const auto body_method{mcp_request_method_from_body(envelope)};
  if (!body_method.has_value()) {
    return make_error(JSONRPC_CODE_INVALID_REQUEST, "Invalid Request");
  }

  const auto *parameters{sourcemeta::core::jsonrpc_params(envelope)};
  const auto is_named{mcp_is_named_request_method(body_method.value())};
  const sourcemeta::core::JSON *name_field{nullptr};
  if (is_named) {
    if (parameters == nullptr || !parameters->is_object()) {
      return make_error(JSONRPC_CODE_INVALID_PARAMS, "Invalid params");
    }

    if (body_method.value() == MCP_METHOD_RESOURCES_READ) {
      name_field = parameters->try_at("uri", MCP_HASH_URI);
    } else {
      name_field = parameters->try_at("name", MCP_HASH_NAME);
    }

    if (name_field == nullptr || !name_field->is_string()) {
      return make_error(JSONRPC_CODE_INVALID_PARAMS, "Invalid params");
    }
  }

  if (version == MCPProtocolVersion::V_2026_07_28) {
    if (!protocol_version_header.has_value()) {
      return make_error(MCP_CODE_HEADER_MISMATCH,
                        "Missing required header: MCP-Protocol-Version");
    }

    if (!mcp_is_valid_header_characters(protocol_version_header.value())) {
      return make_error(MCP_CODE_HEADER_MISMATCH,
                        "Invalid characters in header: MCP-Protocol-Version");
    }

    const auto resolved_protocol{
        mcp_resolve_protocol_version(protocol_version_header.value())};
    if (!resolved_protocol.has_value() ||
        resolved_protocol.value() != version) {
      auto data{sourcemeta::core::JSON::make_object()};
      auto supported_array{sourcemeta::core::JSON::make_array()};
      supported_array.push_back(
          sourcemeta::core::JSON{mcp_protocol_version_string(version)});
      data.assign_assume_new("supported", std::move(supported_array),
                             MCP_HASH_SUPPORTED);
      data.assign_assume_new(
          "requested", sourcemeta::core::JSON{protocol_version_header.value()},
          MCP_HASH_REQUESTED);
      return make_error(MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
                        "Unsupported protocol version", std::move(data));
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
      auto data{sourcemeta::core::JSON::make_object()};
      data.assign_assume_new(
          "header", sourcemeta::core::JSON{MCP_HEADER_PROTOCOL_VERSION},
          MCP_HASH_HEADER);
      data.assign_assume_new(
          "headerValue",
          sourcemeta::core::JSON{protocol_version_header.value()},
          MCP_HASH_HEADER_VALUE);
      data.assign_assume_new(
          "bodyValue",
          sourcemeta::core::JSON{body_metadata_protocol.value_or("")},
          MCP_HASH_BODY_VALUE);
      return make_error(MCP_CODE_HEADER_MISMATCH, "Header mismatch",
                        std::move(data));
    }

    if (!method_header.has_value()) {
      return make_error(MCP_CODE_HEADER_MISMATCH,
                        "Missing required header: Mcp-Method");
    }

    if (!mcp_is_valid_header_characters(method_header.value())) {
      return make_error(MCP_CODE_HEADER_MISMATCH,
                        "Invalid characters in header: Mcp-Method");
    }

    if (method_header.value() != body_method.value()) {
      auto data{sourcemeta::core::JSON::make_object()};
      data.assign_assume_new(
          "header", sourcemeta::core::JSON{MCP_HEADER_METHOD}, MCP_HASH_HEADER);
      data.assign_assume_new("headerValue",
                             sourcemeta::core::JSON{method_header.value()},
                             MCP_HASH_HEADER_VALUE);
      data.assign_assume_new("bodyValue",
                             sourcemeta::core::JSON{body_method.value()},
                             MCP_HASH_BODY_VALUE);
      return make_error(MCP_CODE_HEADER_MISMATCH, "Header mismatch",
                        std::move(data));
    }

    if (is_named) {
      if (!name_header.has_value()) {
        return make_error(MCP_CODE_HEADER_MISMATCH,
                          "Missing required header: Mcp-Name");
      }

      if (!mcp_is_valid_header_characters(name_header.value())) {
        return make_error(MCP_CODE_HEADER_MISMATCH,
                          "Invalid characters in header: Mcp-Name");
      }

      const auto decoded_name{mcp_decode_header_value(name_header.value())};
      if (!decoded_name.has_value() ||
          decoded_name.value() != name_field->to_string()) {
        auto data{sourcemeta::core::JSON::make_object()};
        data.assign_assume_new(
            "header", sourcemeta::core::JSON{MCP_HEADER_NAME}, MCP_HASH_HEADER);
        data.assign_assume_new("headerValue",
                               sourcemeta::core::JSON{name_header.value()},
                               MCP_HASH_HEADER_VALUE);
        data.assign_assume_new("bodyValue",
                               sourcemeta::core::JSON{name_field->to_string()},
                               MCP_HASH_BODY_VALUE);
        return make_error(MCP_CODE_HEADER_MISMATCH, "Header mismatch",
                          std::move(data));
      }
    } else {
      if (name_header.has_value()) {
        auto data{sourcemeta::core::JSON::make_object()};
        data.assign_assume_new(
            "header", sourcemeta::core::JSON{MCP_HEADER_NAME}, MCP_HASH_HEADER);
        data.assign_assume_new("headerValue",
                               sourcemeta::core::JSON{name_header.value()},
                               MCP_HASH_HEADER_VALUE);
        return make_error(MCP_CODE_HEADER_MISMATCH,
                          "Header mismatch: unexpected header provided",
                          std::move(data));
      }
    }

    return std::nullopt;
  }

  // Legacy protocol versions
  if (protocol_version_header.has_value()) {
    if (!mcp_is_valid_header_characters(protocol_version_header.value())) {
      return make_error(MCP_CODE_HEADER_MISMATCH,
                        "Invalid characters in header: MCP-Protocol-Version");
    }

    const auto resolved_protocol{
        mcp_resolve_protocol_version(protocol_version_header.value())};
    if (!resolved_protocol.has_value()) {
      auto data{sourcemeta::core::JSON::make_object()};
      auto supported_array{sourcemeta::core::JSON::make_array()};
      supported_array.push_back(
          sourcemeta::core::JSON{mcp_protocol_version_string(version)});
      data.assign_assume_new("supported", std::move(supported_array),
                             MCP_HASH_SUPPORTED);
      data.assign_assume_new(
          "requested", sourcemeta::core::JSON{protocol_version_header.value()},
          MCP_HASH_REQUESTED);
      return make_error(MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
                        "Unsupported protocol version", std::move(data));
    }
    if (resolved_protocol.value() != version) {
      auto data{sourcemeta::core::JSON::make_object()};
      data.assign_assume_new(
          "header", sourcemeta::core::JSON{MCP_HEADER_PROTOCOL_VERSION},
          MCP_HASH_HEADER);
      data.assign_assume_new(
          "headerValue",
          sourcemeta::core::JSON{protocol_version_header.value()},
          MCP_HASH_HEADER_VALUE);
      data.assign_assume_new(
          "bodyValue",
          sourcemeta::core::JSON{mcp_protocol_version_string(version)},
          MCP_HASH_BODY_VALUE);
      return make_error(MCP_CODE_HEADER_MISMATCH, "Header mismatch",
                        std::move(data));
    }
  }

  if (method_header.has_value()) {
    if (!mcp_is_valid_header_characters(method_header.value())) {
      return make_error(MCP_CODE_HEADER_MISMATCH,
                        "Invalid characters in header: Mcp-Method");
    }

    if (method_header.value() != body_method.value()) {
      auto data{sourcemeta::core::JSON::make_object()};
      data.assign_assume_new(
          "header", sourcemeta::core::JSON{MCP_HEADER_METHOD}, MCP_HASH_HEADER);
      data.assign_assume_new("headerValue",
                             sourcemeta::core::JSON{method_header.value()},
                             MCP_HASH_HEADER_VALUE);
      data.assign_assume_new("bodyValue",
                             sourcemeta::core::JSON{body_method.value()},
                             MCP_HASH_BODY_VALUE);
      return make_error(MCP_CODE_HEADER_MISMATCH, "Header mismatch",
                        std::move(data));
    }
  }

  if (is_named) {
    if (name_header.has_value()) {
      if (!mcp_is_valid_header_characters(name_header.value())) {
        return make_error(MCP_CODE_HEADER_MISMATCH,
                          "Invalid characters in header: Mcp-Name");
      }

      const auto decoded_name{mcp_decode_header_value(name_header.value())};
      if (!decoded_name.has_value() ||
          decoded_name.value() != name_field->to_string()) {
        auto data{sourcemeta::core::JSON::make_object()};
        data.assign_assume_new(
            "header", sourcemeta::core::JSON{MCP_HEADER_NAME}, MCP_HASH_HEADER);
        data.assign_assume_new("headerValue",
                               sourcemeta::core::JSON{name_header.value()},
                               MCP_HASH_HEADER_VALUE);
        data.assign_assume_new("bodyValue",
                               sourcemeta::core::JSON{name_field->to_string()},
                               MCP_HASH_BODY_VALUE);
        return make_error(MCP_CODE_HEADER_MISMATCH, "Header mismatch",
                          std::move(data));
      }
    }
  } else {
    if (name_header.has_value()) {
      auto data{sourcemeta::core::JSON::make_object()};
      data.assign_assume_new("header", sourcemeta::core::JSON{MCP_HEADER_NAME},
                             MCP_HASH_HEADER);
      data.assign_assume_new("headerValue",
                             sourcemeta::core::JSON{name_header.value()},
                             MCP_HASH_HEADER_VALUE);
      return make_error(MCP_CODE_HEADER_MISMATCH,
                        "Header mismatch: unexpected header provided",
                        std::move(data));
    }
  }

  return std::nullopt;
}

} // namespace sourcemeta::core
