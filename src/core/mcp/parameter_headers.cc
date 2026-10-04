#include <sourcemeta/core/mcp.h>

#include "validation.h"

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/unicode.h>

#include <algorithm>
#include <cmath>
#include <set>
#include <string>
#include <vector>

namespace {
using sourcemeta::core::JSON;
using sourcemeta::core::MCPHeaderParameter;

auto lower(const JSON::StringView value) -> std::string {
  std::string result{value};
  for (auto &character : result) {
    character = sourcemeta::core::to_lowercase(character);
  }
  return result;
}

auto token(const JSON::StringView value) -> bool {
  return sourcemeta::core::http_is_token(value);
}

auto visit(const JSON &schema, const bool reachable,
           std::vector<JSON::StringView> &path, std::set<std::string> &names,
           std::vector<MCPHeaderParameter> &result) -> bool {
  if (!schema.is_object()) {
    return true;
  }
  if (const auto *annotation{schema.try_at("x-mcp-header")}; annotation) {
    const auto *type{schema.try_at("type")};
    if (!reachable || path.empty() || !annotation->is_string() ||
        !token(annotation->to_string()) || (type == nullptr) ||
        !type->is_string() ||
        !names.insert(lower(annotation->to_string())).second) {
      return false;
    }
    const auto name{type->to_string()};
    if (name != "string" && name != "integer" && name != "boolean") {
      return false;
    }
    result.push_back(
        MCPHeaderParameter{.name = annotation->to_string(),
                           .path = path,
                           .type = name == "string"    ? JSON::Type::String
                                   : name == "integer" ? JSON::Type::Integer
                                                       : JSON::Type::Boolean});
  }
  // Walk schema locations, never instance-valued enum/default/examples fields.
  for (const auto *const name : {"properties", "patternProperties", "$defs",
                                 "definitions", "dependentSchemas"}) {
    const auto *children{schema.try_at(name)};
    if ((children == nullptr) || !children->is_object()) {
      continue;
    }
    for (const auto &child : children->as_object()) {
      path.push_back(child.first);
      if (!visit(child.second,
                 reachable && JSON::StringView{name} == "properties", path,
                 names, result)) {
        return false;
      }
      path.pop_back();
    }
  }
  for (const auto *const name :
       {"items", "additionalItems", "additionalProperties", "unevaluatedItems",
        "unevaluatedProperties", "contains", "propertyNames", "not", "if",
        "then", "else", "allOf", "anyOf", "oneOf", "prefixItems"}) {
    const auto *child{schema.try_at(name)};
    if (child == nullptr) {
      continue;
    }
    if (child->is_array()) {
      for (const auto &item : child->as_array()) {
        if (!visit(item, false, path, names, result)) {
          return false;
        }
      }
    } else if (!visit(*child, false, path, names, result)) {
      return false;
    }
  }
  return true;
}

auto value_at(const JSON &arguments, const MCPHeaderParameter &parameter)
    -> const JSON * {
  const JSON *value{&arguments};
  for (const auto name : parameter.path) {
    if (!value->is_object()) {
      return nullptr;
    }
    value = value->try_at(name);
    if (value == nullptr) {
      return nullptr;
    }
  }
  return value->is_null() ? nullptr : value;
}

void require_parameters(const std::span<const MCPHeaderParameter> parameters) {
  std::set<std::string> names;
  for (const auto &parameter : parameters) {
    sourcemeta::core::internal::require(
        token(parameter.name) && !parameter.path.empty() &&
            (parameter.type == JSON::Type::String ||
             parameter.type == JSON::Type::Integer ||
             parameter.type == JSON::Type::Boolean) &&
            names.insert(lower(parameter.name)).second,
        "Invalid or duplicate parameter header descriptor");
  }
}

auto safe_parameter(const JSON &value, const MCPHeaderParameter &parameter)
    -> bool {
  if (parameter.type == JSON::Type::Integer) {
    return value.is_integral() && value >= JSON{INT64_C(-9007199254740991)} &&
           value <= JSON{INT64_C(9007199254740991)};
  }
  return value.type() == parameter.type;
}

auto parameter_text(const JSON &value) -> std::string {
  if (value.is_string()) {
    return std::string{value.to_string()};
  }
  if (value.is_boolean()) {
    return value.to_boolean() ? "true" : "false";
  }
  return std::to_string(value.as_integer());
}

auto decode(const JSON::StringView value, std::string &storage)
    -> std::optional<JSON::StringView> {
  if ((!value.empty() && (value.front() == ' ' || value.back() == ' ')) ||
      !std::all_of(value.begin(), value.end(), [](const char character) {
        const auto byte{static_cast<unsigned char>(character)};
        return byte >= 0x20 && byte < 0x7f;
      })) {
    return std::nullopt;
  }
  if (value.starts_with("=?base64?") && value.ends_with("?=") &&
      value.size() >= 11) {
    auto decoded{
        sourcemeta::core::base64_decode(value.substr(9, value.size() - 11))};
    if (!decoded || !sourcemeta::core::is_valid_utf8(*decoded)) {
      return std::nullopt;
    }
    storage = std::move(*decoded);
    return JSON::StringView{storage};
  }
  return value;
}
} // namespace

namespace sourcemeta::core {
auto mcp_header_parameters(const MCPProtocolVersion version, const JSON &schema)
    -> std::optional<std::vector<MCPHeaderParameter>> {
  internal::require(version == MCPProtocolVersion::V_2026_07_28,
                    "Parameter headers require MCP 2026-07-28");
  if (!schema.is_object()) {
    return std::nullopt;
  }
  std::vector<MCPHeaderParameter> result;
  std::vector<JSON::StringView> path;
  std::set<std::string> names;
  if (!visit(schema, true, path, names, result)) {
    return std::nullopt;
  }
  return result;
}

auto mcp_make_parameter_headers(
    const std::span<const MCPHeaderParameter> parameters, const JSON &arguments)
    -> std::vector<std::pair<std::string, std::string>> {
  require_parameters(parameters);
  internal::require(arguments.is_object(), "Tool arguments must be an object");
  std::vector<std::pair<std::string, std::string>> result;
  result.reserve(parameters.size());
  for (const auto &parameter : parameters) {
    if (const auto *value{value_at(arguments, parameter)}; value) {
      internal::require(safe_parameter(*value, parameter),
                        "Invalid mirrored argument type or unsafe integer");
      result.emplace_back("Mcp-Param-" + std::string{parameter.name},
                          mcp_encode_header_value(parameter_text(*value)));
    }
  }
  return result;
}

auto mcp_validate_parameter_headers(
    const MCPProtocolVersion version, const JSON &identifier,
    const std::span<const MCPHeaderParameter> parameters, const JSON &arguments,
    const std::span<const std::pair<JSON::StringView, JSON::StringView>>
        headers) -> std::optional<JSON> {
  internal::require(version == MCPProtocolVersion::V_2026_07_28,
                    "Parameter headers require MCP 2026-07-28");
  internal::require(internal::valid_id(identifier),
                    "Invalid MCP request identifier");
  require_parameters(parameters);
  if (!arguments.is_object()) {
    return mcp_make_error(version, &identifier, JSONRPC_CODE_INVALID_PARAMS,
                          "Invalid tool arguments");
  }
  for (const auto &parameter : parameters) {
    const std::string name{"Mcp-Param-" + std::string{parameter.name}};

    std::optional<JSON::StringView> header;
    for (const auto &candidate : headers) {
      if (equals_ignore_case(candidate.first, name)) {
        if (header) {
          return mcp_make_error_header_mismatch(version, identifier,
                                                "Duplicate parameter header");
        }
        header = candidate.second;
      }
    }
    const auto *value{value_at(arguments, parameter)};
    if (value == nullptr) {
      if (header) {
        return mcp_make_error_header_mismatch(version, identifier, name,
                                              *header);
      }
      continue;
    }
    if (!safe_parameter(*value, parameter)) {
      return mcp_make_error(version, &identifier, JSONRPC_CODE_INVALID_PARAMS,
                            "Invalid mirrored argument");
    }
    std::string decoded_storage;
    const auto decoded{header ? decode(*header, decoded_storage)
                              : std::nullopt};
    bool matches = false;
    if (decoded) {
      if (parameter.type == JSON::Type::Integer) {
        try {
          const auto number{parse_json(*decoded)};
          matches = number.is_number() && number == *value;
        } catch (const std::exception &) {
          matches = false;
        }
      } else {
        matches = value->is_string() ? *decoded == value->to_string()
                                     : *decoded == parameter_text(*value);
      }
    }
    if (!matches) {
      return mcp_make_error_header_mismatch(version, identifier, name,
                                            header.value_or(""),
                                            parameter_text(*value));
    }
  }
  return std::nullopt;
}
} // namespace sourcemeta::core
