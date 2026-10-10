#ifndef SOURCEMETA_CORE_MCP_VALIDATION_H_
#define SOURCEMETA_CORE_MCP_VALIDATION_H_

#include <sourcemeta/core/mcp.h>

#include "helpers.h"

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/text.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <set>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace sourcemeta::core::internal {

inline void require(const bool condition, const char *message) {
  if (!condition) {
    throw std::invalid_argument{message};
  }
}

inline auto valid_id(const JSON &identifier) noexcept -> bool {
  return identifier.is_string() || identifier.is_integer();
}

inline auto valid_implementation(MCPProtocolVersion version, const JSON &value)
    -> bool;
inline auto make_implementation(MCPProtocolVersion version,
                                const MCPImplementation &value) -> JSON;
inline auto valid_capabilities(MCPProtocolVersion version, const JSON &value,
                               bool client) -> bool;
inline auto valid_extensions(const JSON &value) -> bool;
inline auto valid_tool(MCPProtocolVersion version, const JSON &value) -> bool;
inline auto valid_content(MCPProtocolVersion version, const JSON &value)
    -> bool;
inline auto valid_resource(MCPProtocolVersion version, const JSON &value,
                           bool contents, bool is_template) -> bool;
inline auto valid_prompt(MCPProtocolVersion version, const JSON &value) -> bool;
inline auto valid_input_requests(const JSON &value,
                                 const MCPClientCapabilities &capabilities)
    -> bool;
inline auto valid_input_response(JSON::StringView method, const JSON &request,
                                 const JSON &value) -> bool;
inline auto valid_meta(const JSON &value) -> bool;
inline auto valid_meta(MCPProtocolVersion version, const JSON &value) -> bool;
inline auto valid_metadata_object(const JSON &meta) -> bool;
inline auto valid_log_level(JSON::StringView value) noexcept -> bool;

} // namespace sourcemeta::core::internal

namespace sourcemeta::core::internal::validation {
using sourcemeta::core::JSON;
using sourcemeta::core::MCPProtocolVersion;

inline auto field(const JSON &value, const JSON::StringView name,
                  const JSON::Object::hash_type hash) -> const JSON * {
  return value.is_object() ? value.try_at(name, hash) : nullptr;
}

inline auto string_field(const JSON &value, const JSON::StringView name,
                         const JSON::Object::hash_type hash,
                         const bool required = false) -> bool {
  const auto *item{field(value, name, hash)};
  return item == nullptr ? !required : item->is_string();
}

inline auto
optional_fields(const JSON &value,
                const std::initializer_list<
                    std::pair<JSON::StringView, JSON::Object::hash_type>>
                    names,
                const JSON::Type type) -> bool {
  return std::all_of(names.begin(), names.end(), [&](const auto name) {
    const auto *item{field(value, name.first, name.second)};
    return item == nullptr || item->type() == type;
  });
}

inline auto strings(const JSON &value) -> bool {
  return value.is_array() &&
         std::all_of(value.as_array().begin(), value.as_array().end(),
                     [](const auto &item) { return item.is_string(); });
}

inline auto annotations(const JSON &value) -> bool {
  const auto *item{
      field(value, "annotations", sourcemeta::core::MCP_HASH_ANNOTATIONS)};
  if (item == nullptr) {
    return true;
  }
  if (!item->is_object() ||
      !string_field(*item, "lastModified",
                    sourcemeta::core::MCP_HASH_LAST_MODIFIED)) {
    return false;
  }
  if (const auto *priority{
          field(*item, "priority", sourcemeta::core::MCP_HASH_PRIORITY)};
      priority != nullptr &&
      (!priority->is_number() ||
       (priority->is_real() && !std::isfinite(priority->to_real())) ||
       *priority < JSON{0} || *priority > JSON{1})) {
    return false;
  }
  if (const auto *audience{
          field(*item, "audience", sourcemeta::core::MCP_HASH_AUDIENCE)};
      audience != nullptr) {
    if (!strings(*audience)) {
      return false;
    }
    for (const auto &role : audience->as_array()) {
      if (role.to_string() != "user" && role.to_string() != "assistant") {
        return false;
      }
    }
  }
  return true;
}

inline auto icons(const JSON &value) -> bool {
  const auto *items{field(value, "icons", sourcemeta::core::MCP_HASH_ICONS)};
  if (items == nullptr) {
    return true;
  }
  if (!items->is_array()) {
    return false;
  }
  for (const auto &item : items->as_array()) {
    if (!item.is_object() ||
        !string_field(item, "src", sourcemeta::core::MCP_HASH_SRC, true) ||
        !string_field(item, "mimeType", sourcemeta::core::MCP_HASH_MIME_TYPE)) {
      return false;
    }
    if (const auto *sizes{
            field(item, "sizes", sourcemeta::core::MCP_HASH_SIZES)};
        (sizes != nullptr) && !strings(*sizes)) {
      return false;
    }
    if (const auto *theme{
            field(item, "theme", sourcemeta::core::MCP_HASH_THEME)};
        (theme != nullptr) &&
        (!theme->is_string() ||
         (theme->to_string() != "light" && theme->to_string() != "dark"))) {
      return false;
    }
  }
  return true;
}

inline auto object_schema(const JSON &value) -> bool {
  const auto *type{field(value, "type", sourcemeta::core::MCP_HASH_TYPE)};
  return (type != nullptr) && type->is_string() &&
         type->to_string() == "object";
}

inline auto base(const JSON &value) -> bool {
  return value.is_object() &&
         string_field(value, "name", sourcemeta::core::MCP_HASH_NAME, true) &&
         string_field(value, "title", sourcemeta::core::MCP_HASH_TITLE) &&
         string_field(value, "description",
                      sourcemeta::core::MCP_HASH_DESCRIPTION) &&
         sourcemeta::core::internal::valid_meta(value) && icons(value);
}

inline auto metadata_key(const JSON::StringView key, const bool prefix_required)
    -> bool {
  const auto slash{key.find('/')};
  JSON::StringView name{key};
  if (slash == JSON::StringView::npos) {
    if (prefix_required) {
      return false;
    }
  } else {
    if (slash == 0 || key.find('/', slash + 1) != JSON::StringView::npos) {
      return false;
    }
    const auto prefix{key.substr(0, slash)};
    bool start = true;
    char previous = '.';
    for (const char character : prefix) {
      if (character == '.') {
        if (start || !sourcemeta::core::is_alphanum(previous)) {
          return false;
        }
        start = true;
      } else {
        if ((start && !sourcemeta::core::is_alpha(character)) ||
            (!sourcemeta::core::is_alphanum(character) && character != '-')) {
          return false;
        }
        start = false;
      }
      previous = character;
    }
    if (start || !sourcemeta::core::is_alphanum(previous)) {
      return false;
    }
    name = key.substr(slash + 1);
  }
  return name.empty() ||
         (sourcemeta::core::is_alphanum(name.front()) &&
          sourcemeta::core::is_alphanum(name.back()) &&
          std::all_of(name.begin(), name.end(), [](const char character) {
            return sourcemeta::core::is_alphanum(character) ||
                   character == '_' || character == '-' || character == '.';
          }));
}

inline auto traceparent(const JSON::StringView value) -> bool {
  if (value.size() < 55 || value[2] != '-' || value[35] != '-' ||
      value[52] != '-' || value.starts_with("ff") ||
      (value.starts_with("00") && value.size() != 55) ||
      (value.size() > 55 && value[55] != '-')) {
    return false;
  }
  for (std::size_t index = 0; index < 55; ++index) {
    if (index != 2 && index != 35 && index != 52 &&
        (!sourcemeta::core::is_hex_digit(value[index]) ||
         sourcemeta::core::to_lowercase(value[index]) != value[index])) {
      return false;
    }
  }
  return value.substr(3, 32).find_first_not_of('0') != JSON::StringView::npos &&
         value.substr(36, 16).find_first_not_of('0') != JSON::StringView::npos;
}

inline auto trace_key_part(const JSON::StringView value, const bool tenant,
                           const std::size_t maximum) -> bool {
  if (value.empty() || value.size() > maximum ||
      !((value.front() >= 'a' && value.front() <= 'z') ||
        (tenant && sourcemeta::core::is_digit(value.front())))) {
    return false;
  }
  return std::all_of(value.begin(), value.end(), [](const char character) {
    return (character >= 'a' && character <= 'z') ||
           sourcemeta::core::is_digit(character) || character == '_' ||
           character == '-' || character == '*' || character == '/';
  });
}

inline auto tracestate(JSON::StringView value) -> bool {
  std::set<JSON::StringView> keys;
  std::size_t members = 0;
  while (true) {
    if (++members > 32) {
      return false;
    }
    const auto comma{value.find(',')};
    const auto entry{sourcemeta::core::trim(value.substr(0, comma),
                                            sourcemeta::core::http_is_ows)};
    if (!entry.empty()) {
      const auto equal{entry.find('=')};
      if (equal == JSON::StringView::npos) {
        return false;
      }
      const auto key{entry.substr(0, equal)};
      const auto content{entry.substr(equal + 1)};
      const auto at_position{key.find('@')};
      const bool valid_key{
          at_position == JSON::StringView::npos
              ? trace_key_part(key, false, 256)
              : trace_key_part(key.substr(0, at_position), true, 241) &&
                    trace_key_part(key.substr(at_position + 1), false, 14)};
      if (!valid_key || !keys.insert(key).second || content.empty() ||
          content.size() > 256 || content.back() == ' ' ||
          !std::all_of(content.begin(), content.end(),
                       [](const char character) {
                         return character >= 0x20 && character <= 0x7e &&
                                character != ',' && character != '=';
                       })) {
        return false;
      }
    }
    if (comma == JSON::StringView::npos) {
      return true;
    }
    value.remove_prefix(comma + 1);
  }
}

inline auto baggage_value(const JSON::StringView value) -> bool {
  for (std::size_t index = 0; index < value.size(); ++index) {
    const auto character{value[index]};
    if (character <= 0x20 || character > 0x7e || character == '"' ||
        character == ',' || character == ';' || character == '\\') {
      return false;
    }
    if (character == '%') {
      if (!sourcemeta::core::is_percent_triplet(value, index)) {
        return false;
      }
      index += 2;
    }
  }
  return true;
}

inline auto baggage(JSON::StringView value) -> bool {
  std::size_t count = 0;
  while (true) {
    if (++count > 180) {
      return false;
    }
    const auto comma{value.find(',')};
    auto member{sourcemeta::core::trim(value.substr(0, comma),
                                       sourcemeta::core::http_is_ows)};
    bool first = true;
    while (true) {
      const auto semicolon{member.find(';')};
      const auto property{sourcemeta::core::trim(
          member.substr(0, semicolon), sourcemeta::core::http_is_ows)};
      const auto equal{property.find('=')};
      if ((first && equal == JSON::StringView::npos) ||
          !sourcemeta::core::http_is_token(sourcemeta::core::trim(
              property.substr(0, equal), sourcemeta::core::http_is_ows)) ||
          (equal != JSON::StringView::npos &&
           !baggage_value(sourcemeta::core::trim(
               property.substr(equal + 1), sourcemeta::core::http_is_ows)))) {
        return false;
      }
      first = false;
      if (semicolon == JSON::StringView::npos) {
        break;
      }
      member.remove_prefix(semicolon + 1);
    }
    if (comma == JSON::StringView::npos) {
      return true;
    }
    value.remove_prefix(comma + 1);
  }
}

inline auto sampling_block(const JSON &value) -> bool {
  const auto *type{field(value, "type", sourcemeta::core::MCP_HASH_TYPE)};
  if ((type == nullptr) || !type->is_string() ||
      !sourcemeta::core::internal::valid_meta(MCPProtocolVersion::V_2026_07_28,
                                              value)) {
    return false;
  }
  const auto name{type->to_string()};
  if (name == "text" || name == "image" || name == "audio") {
    return sourcemeta::core::internal::valid_content(
        MCPProtocolVersion::V_2026_07_28, value);
  }
  if (name == "tool_use") {
    const auto *input{field(value, "input", sourcemeta::core::MCP_HASH_INPUT)};
    return string_field(value, "id", sourcemeta::core::MCP_HASH_ID, true) &&
           string_field(value, "name", sourcemeta::core::MCP_HASH_NAME, true) &&
           (input != nullptr) && input->is_object();
  }
  if (name == "tool_result") {
    const auto *content{
        field(value, "content", sourcemeta::core::MCP_HASH_CONTENT)};
    if (!string_field(value, "toolUseId",
                      sourcemeta::core::MCP_HASH_TOOL_USE_ID, true) ||
        (content == nullptr) || !content->is_array() ||
        !optional_fields(value,
                         {{"isError", sourcemeta::core::MCP_HASH_IS_ERROR}},
                         JSON::Type::Boolean)) {
      return false;
    }
    for (const auto &block : content->as_array()) {
      if (!sourcemeta::core::internal::valid_content(
              MCPProtocolVersion::V_2026_07_28, block)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

inline auto sampling_message_block(const JSON &block, const bool tools,
                                   const JSON::StringView role,
                                   const bool answering,
                                   std::set<JSON::StringView> &pending,
                                   std::set<JSON::StringView> &next) -> bool {
  if (!sampling_block(block)) {
    return false;
  }
  const auto type{
      block.at("type", sourcemeta::core::MCP_HASH_TYPE).to_string()};
  if (type == "tool_result") {
    return tools && answering && role == "user" &&
           pending.erase(
               block.at("toolUseId", sourcemeta::core::MCP_HASH_TOOL_USE_ID)
                   .to_string()) == 1;
  }
  if (answering) {
    return false;
  }
  if (type == "tool_use") {
    return tools && role == "assistant" &&
           next.insert(
                   block.at("id", sourcemeta::core::MCP_HASH_ID).to_string())
               .second;
  }
  return true;
}

inline auto
sampling_messages(const JSON &messages,
                  const sourcemeta::core::MCPClientCapabilities &capabilities)
    -> bool {
  if (!messages.is_array()) {
    return false;
  }
  std::set<JSON::StringView> pending;
  for (const auto &message : messages.as_array()) {
    const auto *role{field(message, "role", sourcemeta::core::MCP_HASH_ROLE)};
    const auto *content{
        field(message, "content", sourcemeta::core::MCP_HASH_CONTENT)};
    if ((role == nullptr) || !role->is_string() ||
        (role->to_string() != "user" && role->to_string() != "assistant") ||
        (content == nullptr) ||
        !sourcemeta::core::internal::valid_meta(
            MCPProtocolVersion::V_2026_07_28, message)) {
      return false;
    }
    // Tool calls must be resolved by the immediately following user message,
    // which may contain only the corresponding tool results.
    const bool answering{!pending.empty()};
    std::set<JSON::StringView> next;
    if (content->is_array()) {
      for (const auto &block : content->as_array()) {
        if (!sampling_message_block(block, capabilities.sampling_tools,
                                    role->to_string(), answering, pending,
                                    next)) {
          return false;
        }
      }
    } else if (!sampling_message_block(*content, capabilities.sampling_tools,
                                       role->to_string(), answering, pending,
                                       next)) {
      return false;
    }
    if (!pending.empty()) {
      return false;
    }
    pending = std::move(next);
  }
  return pending.empty();
}

inline auto sampling_options(const JSON &params) -> bool {
  if (!string_field(params, "systemPrompt",
                    sourcemeta::core::MCP_HASH_SYSTEM_PROMPT) ||
      !optional_fields(
          params,
          {{"metadata", sourcemeta::core::MCP_HASH_METADATA},
           {"modelPreferences", sourcemeta::core::MCP_HASH_MODEL_PREFERENCES},
           {"toolChoice", sourcemeta::core::MCP_HASH_TOOL_CHOICE}},
          JSON::Type::Object)) {
    return false;
  }
  if (const auto *temperature{
          field(params, "temperature", sourcemeta::core::MCP_HASH_TEMPERATURE)};
      (temperature != nullptr) && !temperature->is_number()) {
    return false;
  }
  if (const auto *stop{field(params, "stopSequences",
                             sourcemeta::core::MCP_HASH_STOP_SEQUENCES)};
      (stop != nullptr) && !strings(*stop)) {
    return false;
  }
  if (const auto *choice{
          field(params, "toolChoice", sourcemeta::core::MCP_HASH_TOOL_CHOICE)};
      choice) {
    const auto *mode{field(*choice, "mode", sourcemeta::core::MCP_HASH_MODE)};
    if ((mode != nullptr) &&
        (!mode->is_string() ||
         (mode->to_string() != "auto" && mode->to_string() != "none" &&
          mode->to_string() != "required"))) {
      return false;
    }
  }
  if (const auto *preferences{
          field(params, "modelPreferences",
                sourcemeta::core::MCP_HASH_MODEL_PREFERENCES)};
      preferences) {
    for (const auto &[name, hash] :
         {std::pair{JSON::StringView{"costPriority"},
                    sourcemeta::core::MCP_HASH_COST_PRIORITY},
          std::pair{JSON::StringView{"speedPriority"},
                    sourcemeta::core::MCP_HASH_SPEED_PRIORITY},
          std::pair{JSON::StringView{"intelligencePriority"},
                    sourcemeta::core::MCP_HASH_INTELLIGENCE_PRIORITY}}) {
      const auto *value{field(*preferences, name, hash)};
      if ((value != nullptr) && (!value->is_number() || value->as_real() < 0 ||
                                 value->as_real() > 1)) {
        return false;
      }
    }
    if (const auto *hints{
            field(*preferences, "hints", sourcemeta::core::MCP_HASH_HINTS)};
        hints) {
      if (!hints->is_array()) {
        return false;
      }
      for (const auto &hint : hints->as_array()) {
        if (!hint.is_object() ||
            !string_field(hint, "name", sourcemeta::core::MCP_HASH_NAME)) {
          return false;
        }
      }
    }
  }
  if (const auto *tools{
          field(params, "tools", sourcemeta::core::MCP_HASH_TOOLS)};
      tools) {
    if (!tools->is_array()) {
      return false;
    }
    for (const auto &tool : tools->as_array()) {
      if (!sourcemeta::core::internal::valid_tool(
              MCPProtocolVersion::V_2026_07_28, tool)) {
        return false;
      }
    }
  }
  return true;
}

} // namespace sourcemeta::core::internal::validation

namespace sourcemeta::core::internal {

inline auto valid_log_level(const JSON::StringView value) noexcept -> bool {
  return mcp_resolve_log_level(value).has_value();
}

inline auto valid_meta(const JSON &value) -> bool {
  const auto *meta{
      validation::field(value, "_meta", sourcemeta::core::MCP_HASH_META)};
  return meta == nullptr || meta->is_object();
}

inline auto valid_meta(const MCPProtocolVersion version, const JSON &value)
    -> bool {
  const auto *meta{
      validation::field(value, "_meta", sourcemeta::core::MCP_HASH_META)};
  return meta == nullptr || (version == MCPProtocolVersion::V_2026_07_28
                                 ? valid_metadata_object(*meta)
                                 : meta->is_object());
}

inline auto valid_metadata_object(const JSON &meta) -> bool {
  if (!meta.is_object()) {
    return false;
  }
  for (const auto &entry : meta.as_object()) {
    if (!validation::metadata_key(entry.first, false)) {
      return false;
    }
  }
  for (const auto &[name, hash] :
       {std::pair{JSON::StringView{"traceparent"},
                  sourcemeta::core::MCP_HASH_TRACEPARENT},
        std::pair{JSON::StringView{"tracestate"},
                  sourcemeta::core::MCP_HASH_TRACESTATE},
        std::pair{JSON::StringView{"baggage"},
                  sourcemeta::core::MCP_HASH_BAGGAGE}}) {
    if (const auto *value{meta.try_at(name, hash)}; value) {
      if (!value->is_string()) {
        return false;
      }
      const auto text{value->to_string()};
      const bool valid{JSON::StringView{name} == "traceparent"
                           ? validation::traceparent(text)
                       : JSON::StringView{name} == "tracestate"
                           ? validation::tracestate(text)
                           : validation::baggage(text)};
      if (!valid) {
        return false;
      }
    }
  }
  return true;
}

inline auto valid_implementation(const MCPProtocolVersion version,
                                 const JSON &value) -> bool {
  if (!value.is_object() ||
      !validation::string_field(value, "name", sourcemeta::core::MCP_HASH_NAME,
                                true) ||
      !validation::string_field(value, "version",
                                sourcemeta::core::MCP_HASH_VERSION, true)) {
    return false;
  }
  if (mcp_supports_implementation_title(version) &&
      !validation::string_field(value, "title",
                                sourcemeta::core::MCP_HASH_TITLE)) {
    return false;
  }
  return !mcp_supports_implementation_description(version) ||
         (validation::string_field(value, "description",
                                   sourcemeta::core::MCP_HASH_DESCRIPTION) &&
          validation::string_field(value, "websiteUrl",
                                   sourcemeta::core::MCP_HASH_WEBSITE_URL) &&
          validation::icons(value));
}

inline auto make_implementation(const MCPProtocolVersion version,
                                const MCPImplementation &value) -> JSON {
  auto result{JSON::make_object()};
  result.assign_assume_new("name", JSON{value.name},
                           sourcemeta::core::MCP_HASH_NAME);
  result.assign_assume_new("version", JSON{value.version},
                           sourcemeta::core::MCP_HASH_VERSION);
  if (mcp_supports_implementation_title(version) && !value.title.empty()) {
    result.assign_assume_new("title", JSON{value.title},
                             sourcemeta::core::MCP_HASH_TITLE);
  }
  if (mcp_supports_implementation_description(version)) {
    if (!value.description.empty()) {
      result.assign_assume_new("description", JSON{value.description},
                               sourcemeta::core::MCP_HASH_DESCRIPTION);
    }
    if (!value.website_url.empty()) {
      result.assign_assume_new("websiteUrl", JSON{value.website_url},
                               sourcemeta::core::MCP_HASH_WEBSITE_URL);
    }
    if (value.icons != nullptr) {
      result.assign_assume_new("icons", JSON{*value.icons},
                               sourcemeta::core::MCP_HASH_ICONS);
    }
  }
  require(valid_implementation(version, result),
          "Invalid MCP implementation information");
  return result;
}

inline auto valid_extensions(const JSON &value) -> bool {
  if (!value.is_object()) {
    return false;
  }
  for (const auto &entry : value.as_object()) {
    if (!validation::metadata_key(entry.first, true) ||
        !entry.second.is_object()) {
      return false;
    }
  }
  return true;
}

inline auto valid_capabilities(const MCPProtocolVersion version,
                               const JSON &value, const bool client) -> bool {
  if (!value.is_object()) {
    return false;
  }
  if (const auto *experimental{validation::field(
          value, "experimental", sourcemeta::core::MCP_HASH_EXPERIMENTAL)};
      experimental) {
    if (!experimental->is_object()) {
      return false;
    }
    for (const auto &entry : experimental->as_object()) {
      if (!entry.second.is_object()) {
        return false;
      }
    }
  }
  if (version == MCPProtocolVersion::V_2026_07_28) {
    if (const auto *extensions{validation::field(
            value, "extensions", sourcemeta::core::MCP_HASH_EXTENSIONS)};
        (extensions != nullptr) && !valid_extensions(*extensions)) {
      return false;
    }
  }
  if (version == MCPProtocolVersion::V_2025_11_25) {
    if (const auto *tasks{validation::field(value, "tasks",
                                            sourcemeta::core::MCP_HASH_TASKS)};
        tasks) {
      if (!tasks->is_object() ||
          !validation::optional_fields(
              *tasks,
              {{"list", sourcemeta::core::MCP_HASH_LIST},
               {"cancel", sourcemeta::core::MCP_HASH_CANCEL},
               {"requests", sourcemeta::core::MCP_HASH_REQUESTS}},
              JSON::Type::Object)) {
        return false;
      }
      if (const auto *requests{validation::field(
              *tasks, "requests", sourcemeta::core::MCP_HASH_REQUESTS)};
          requests) {
        if (!validation::optional_fields(
                *requests,
                {{"sampling", sourcemeta::core::MCP_HASH_SAMPLING},
                 {"elicitation", sourcemeta::core::MCP_HASH_ELICITATION},
                 {"tools", sourcemeta::core::MCP_HASH_TOOLS}},
                JSON::Type::Object)) {
          return false;
        }
        for (const auto &[name, hash] :
             {std::pair{JSON::StringView{"sampling"},
                        sourcemeta::core::MCP_HASH_SAMPLING},
              std::pair{JSON::StringView{"elicitation"},
                        sourcemeta::core::MCP_HASH_ELICITATION},
              std::pair{JSON::StringView{"tools"},
                        sourcemeta::core::MCP_HASH_TOOLS}}) {
          if (const auto *group{validation::field(*requests, name, hash)};
              (group != nullptr) &&
              !validation::optional_fields(
                  *group,
                  {{"createMessage", sourcemeta::core::MCP_HASH_CREATE_MESSAGE},
                   {"create", sourcemeta::core::MCP_HASH_CREATE},
                   {"call", sourcemeta::core::MCP_HASH_CALL}},
                  JSON::Type::Object)) {
            return false;
          }
        }
      }
    }
  }
  if (client) {
    if (!validation::optional_fields(
            value,
            {{"roots", sourcemeta::core::MCP_HASH_ROOTS},
             {"sampling", sourcemeta::core::MCP_HASH_SAMPLING}},
            JSON::Type::Object)) {
      return false;
    }
    if (const auto *roots{validation::field(value, "roots",
                                            sourcemeta::core::MCP_HASH_ROOTS)};
        (roots != nullptr) && version != MCPProtocolVersion::V_2026_07_28 &&
        !validation::optional_fields(
            *roots, {{"listChanged", sourcemeta::core::MCP_HASH_LIST_CHANGED}},
            JSON::Type::Boolean)) {
      return false;
    }
    if (const auto *sampling{validation::field(
            value, "sampling", sourcemeta::core::MCP_HASH_SAMPLING)};
        (sampling != nullptr) &&
        mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25) &&
        !validation::optional_fields(
            *sampling,
            {{"tools", sourcemeta::core::MCP_HASH_TOOLS},
             {"context", sourcemeta::core::MCP_HASH_CONTEXT}},
            JSON::Type::Object)) {
      return false;
    }
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_06_18)) {
      if (!validation::optional_fields(
              value, {{"elicitation", sourcemeta::core::MCP_HASH_ELICITATION}},
              JSON::Type::Object)) {
        return false;
      }
      if (const auto *elicitation{validation::field(
              value, "elicitation", sourcemeta::core::MCP_HASH_ELICITATION)};
          (elicitation != nullptr) &&
          mcp_protocol_version_at_least(version,
                                        MCPProtocolVersion::V_2025_11_25) &&
          !validation::optional_fields(
              *elicitation,
              {{"form", sourcemeta::core::MCP_HASH_FORM},
               {"url", sourcemeta::core::MCP_HASH_URL}},
              JSON::Type::Object)) {
        return false;
      }
    }
  } else {
    if (!validation::optional_fields(
            value,
            {{"logging", sourcemeta::core::MCP_HASH_LOGGING},
             {"completions", sourcemeta::core::MCP_HASH_COMPLETIONS},
             {"prompts", sourcemeta::core::MCP_HASH_PROMPTS},
             {"resources", sourcemeta::core::MCP_HASH_RESOURCES},
             {"tools", sourcemeta::core::MCP_HASH_TOOLS}},
            JSON::Type::Object)) {
      return false;
    }
    for (const auto &[name, hash] :
         {std::pair{JSON::StringView{"prompts"},
                    sourcemeta::core::MCP_HASH_PROMPTS},
          std::pair{JSON::StringView{"resources"},
                    sourcemeta::core::MCP_HASH_RESOURCES},
          std::pair{JSON::StringView{"tools"},
                    sourcemeta::core::MCP_HASH_TOOLS}}) {
      if (const auto *item{validation::field(value, name, hash)};
          (item != nullptr) &&
          !validation::optional_fields(
              *item, {{"listChanged", sourcemeta::core::MCP_HASH_LIST_CHANGED}},
              JSON::Type::Boolean)) {
        return false;
      }
    }
    if (const auto *resources{validation::field(
            value, "resources", sourcemeta::core::MCP_HASH_RESOURCES)};
        (resources != nullptr) &&
        !validation::optional_fields(
            *resources, {{"subscribe", sourcemeta::core::MCP_HASH_SUBSCRIBE}},
            JSON::Type::Boolean)) {
      return false;
    }
  }
  return true;
}

inline auto valid_tool(const MCPProtocolVersion version, const JSON &value)
    -> bool {
  if (!validation::base(value) || !valid_meta(version, value)) {
    return false;
  }
  const auto *input{validation::field(value, "inputSchema",
                                      sourcemeta::core::MCP_HASH_INPUT_SCHEMA)};
  if ((input == nullptr) || !validation::object_schema(*input)) {
    return false;
  }
  if (mcp_supports_output_schema(version)) {
    if (const auto *output{validation::field(
            value, "outputSchema", sourcemeta::core::MCP_HASH_OUTPUT_SCHEMA)};
        (output != nullptr) &&
        (!output->is_object() || (version != MCPProtocolVersion::V_2026_07_28 &&
                                  !validation::object_schema(*output)))) {
      return false;
    }
  }
  if (const auto *item{validation::field(
          value, "annotations", sourcemeta::core::MCP_HASH_ANNOTATIONS)};
      (item != nullptr) &&
      (!item->is_object() ||
       !validation::string_field(*item, "title",
                                 sourcemeta::core::MCP_HASH_TITLE) ||
       !validation::optional_fields(
           *item,
           {{"readOnlyHint", sourcemeta::core::MCP_HASH_READ_ONLY_HINT},
            {"destructiveHint", sourcemeta::core::MCP_HASH_DESTRUCTIVE_HINT},
            {"idempotentHint", sourcemeta::core::MCP_HASH_IDEMPOTENT_HINT},
            {"openWorldHint", sourcemeta::core::MCP_HASH_OPEN_WORLD_HINT}},
           JSON::Type::Boolean))) {
    return false;
  }
  return true;
}

inline auto valid_resource(const MCPProtocolVersion version, const JSON &value,
                           const bool contents, const bool is_template)
    -> bool {
  if (!value.is_object() || !valid_meta(version, value) ||
      !validation::string_field(value, is_template ? "uriTemplate" : "uri",
                                is_template
                                    ? sourcemeta::core::MCP_HASH_URI_TEMPLATE
                                    : sourcemeta::core::MCP_HASH_URI,
                                true) ||
      !validation::string_field(value, "mimeType",
                                sourcemeta::core::MCP_HASH_MIME_TYPE)) {
    return false;
  }
  if (contents) {
    const auto *text{
        validation::field(value, "text", sourcemeta::core::MCP_HASH_TEXT)};
    const auto *blob{
        validation::field(value, "blob", sourcemeta::core::MCP_HASH_BLOB)};
    return ((text != nullptr) && text->is_string()) ||
           ((blob != nullptr) && blob->is_string());
  }
  const auto *size{
      validation::field(value, "size", sourcemeta::core::MCP_HASH_SIZE)};
  return validation::base(value) && validation::annotations(value) &&
         ((size == nullptr) || (size->is_integer() && size->to_integer() >= 0));
}

inline auto valid_content(const MCPProtocolVersion version, const JSON &value)
    -> bool {
  if (!value.is_object() || !valid_meta(version, value) ||
      !validation::annotations(value)) {
    return false;
  }
  const auto *type{
      validation::field(value, "type", sourcemeta::core::MCP_HASH_TYPE)};
  if ((type == nullptr) || !type->is_string()) {
    return false;
  }
  const auto name{type->to_string()};
  if (name == "text") {
    return validation::string_field(value, "text",
                                    sourcemeta::core::MCP_HASH_TEXT, true);
  }
  if (name == "image" || name == "audio") {
    return validation::string_field(value, "data",
                                    sourcemeta::core::MCP_HASH_DATA, true) &&
           validation::string_field(value, "mimeType",
                                    sourcemeta::core::MCP_HASH_MIME_TYPE, true);
  }
  if (name == "resource") {
    const auto *resource{validation::field(
        value, "resource", sourcemeta::core::MCP_HASH_RESOURCE)};
    return (resource != nullptr) &&
           valid_resource(version, *resource, true, false);
  }
  return name == "resource_link" &&
         mcp_supports_resource_link_content(version) &&
         valid_resource(version, value, false, false);
}

inline auto valid_prompt(const MCPProtocolVersion version, const JSON &value)
    -> bool {
  if (!validation::base(value) || !valid_meta(version, value)) {
    return false;
  }
  const auto *arguments{validation::field(
      value, "arguments", sourcemeta::core::MCP_HASH_ARGUMENTS)};
  if (arguments == nullptr) {
    return true;
  }
  if (!arguments->is_array()) {
    return false;
  }
  for (const auto &argument : arguments->as_array()) {
    if (!validation::base(argument) || !valid_meta(version, argument) ||
        !validation::optional_fields(
            argument, {{"required", sourcemeta::core::MCP_HASH_REQUIRED}},
            JSON::Type::Boolean)) {
      return false;
    }
  }
  return true;
}

inline auto valid_input_requests(const JSON &value,
                                 const MCPClientCapabilities &capabilities)
    -> bool {
  if (!value.is_object()) {
    return false;
  }
  for (const auto &entry : value.as_object()) {
    const auto &request{entry.second};
    const auto *method{validation::field(request, "method",
                                         sourcemeta::core::MCP_HASH_METHOD)};
    const auto *params{validation::field(request, "params",
                                         sourcemeta::core::MCP_HASH_PARAMS)};
    if ((method == nullptr) || !method->is_string() ||
        request.defines("id", sourcemeta::core::MCP_HASH_ID) ||
        request.defines("jsonrpc", sourcemeta::core::MCP_HASH_JSONRPC)) {
      return false;
    }
    // Nested server requests do not carry the outer stateless client metadata,
    // but any optional metadata they do provide must still be well formed.
    if (params != nullptr &&
        (!params->is_object() ||
         !valid_meta(MCPProtocolVersion::V_2026_07_28, *params))) {
      return false;
    }
    const auto name{method->to_string()};
    if (name == MCP_METHOD_ROOTS_LIST) {
      if (!capabilities.roots) {
        return false;
      }
    } else if (name == MCP_METHOD_ELICITATION_CREATE) {
      if ((params == nullptr) || !params->is_object() ||
          !validation::string_field(*params, "message",
                                    sourcemeta::core::MCP_HASH_MESSAGE, true)) {
        return false;
      }
      const auto *mode{
          validation::field(*params, "mode", sourcemeta::core::MCP_HASH_MODE)};
      if ((mode != nullptr) &&
          (!mode->is_string() ||
           (mode->to_string() != "form" && mode->to_string() != "url"))) {
        return false;
      }
      if ((mode != nullptr) && mode->to_string() == "url") {
        if (!capabilities.elicitation_url ||
            !validation::string_field(*params, "url",
                                      sourcemeta::core::MCP_HASH_URL, true)) {
          return false;
        }
      } else {
        const auto *schema{
            validation::field(*params, "requestedSchema",
                              sourcemeta::core::MCP_HASH_REQUESTED_SCHEMA)};
        const bool implicit_form{capabilities.elicitation &&
                                 !capabilities.elicitation_url};
        if ((!capabilities.elicitation_form && !implicit_form) ||
            (schema == nullptr) || !validation::object_schema(*schema)) {
          return false;
        }
      }
    } else if (name == MCP_METHOD_SAMPLING_CREATE_MESSAGE) {
      if (!capabilities.sampling || (params == nullptr) ||
          !params->is_object() || !validation::sampling_options(*params)) {
        return false;
      }
      if (const auto *context{
              validation::field(*params, "includeContext",
                                sourcemeta::core::MCP_HASH_INCLUDE_CONTEXT)};
          context) {
        if (!context->is_string() ||
            (context->to_string() != "none" &&
             context->to_string() != "thisServer" &&
             context->to_string() != "allServers") ||
            (context->to_string() != "none" &&
             !capabilities.sampling_context)) {
          return false;
        }
      }
      const auto *messages{validation::field(
          *params, "messages", sourcemeta::core::MCP_HASH_MESSAGES)};
      const auto *tokens{validation::field(
          *params, "maxTokens", sourcemeta::core::MCP_HASH_MAX_TOKENS)};
      if ((messages == nullptr) ||
          !validation::sampling_messages(*messages, capabilities) ||
          (tokens == nullptr) || !tokens->is_integral()) {
        return false;
      }
      if (((validation::field(*params, "tools",
                              sourcemeta::core::MCP_HASH_TOOLS) != nullptr) ||
           (validation::field(*params, "toolChoice",
                              sourcemeta::core::MCP_HASH_TOOL_CHOICE) !=
            nullptr)) &&
          !capabilities.sampling_tools) {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

inline auto valid_input_response(const JSON::StringView method,
                                 const JSON &request, const JSON &value)
    -> bool {
  if (!value.is_object() ||
      !valid_meta(MCPProtocolVersion::V_2026_07_28, value)) {
    return false;
  }
  if (method == MCP_METHOD_ROOTS_LIST) {
    const auto *roots{
        validation::field(value, "roots", sourcemeta::core::MCP_HASH_ROOTS)};
    if ((roots == nullptr) || !roots->is_array()) {
      return false;
    }
    for (const auto &root : roots->as_array()) {
      const auto *uri{
          validation::field(root, "uri", sourcemeta::core::MCP_HASH_URI)};
      if ((uri == nullptr) || !uri->is_string() ||
          !uri->to_string().starts_with("file://") ||
          !validation::string_field(root, "name",
                                    sourcemeta::core::MCP_HASH_NAME) ||
          !valid_meta(MCPProtocolVersion::V_2026_07_28, root)) {
        return false;
      }
    }
    return true;
  }
  if (method == MCP_METHOD_ELICITATION_CREATE) {
    const auto *action{
        validation::field(value, "action", sourcemeta::core::MCP_HASH_ACTION)};
    if ((action == nullptr) || !action->is_string() ||
        (action->to_string() != "accept" && action->to_string() != "decline" &&
         action->to_string() != "cancel")) {
      return false;
    }
    if (const auto *content{validation::field(
            value, "content", sourcemeta::core::MCP_HASH_CONTENT)};
        content) {
      const auto *params{validation::field(request, "params",
                                           sourcemeta::core::MCP_HASH_PARAMS)};
      const auto *mode{params != nullptr
                           ? validation::field(*params, "mode",
                                               sourcemeta::core::MCP_HASH_MODE)
                           : nullptr};
      if (action->to_string() != "accept" ||
          (mode != nullptr &&
           (!mode->is_string() || mode->to_string() != "form")) ||
          !content->is_object()) {
        return false;
      }
      for (const auto &entry : content->as_object()) {
        if (entry.second.is_real() && !std::isfinite(entry.second.to_real())) {
          return false;
        }
        if (!entry.second.is_string() && !entry.second.is_number() &&
            !entry.second.is_boolean() && !validation::strings(entry.second)) {
          return false;
        }
      }
    }
    return true;
  }
  if (method == MCP_METHOD_SAMPLING_CREATE_MESSAGE) {
    const auto *role{
        validation::field(value, "role", sourcemeta::core::MCP_HASH_ROLE)};
    const auto *content{validation::field(value, "content",
                                          sourcemeta::core::MCP_HASH_CONTENT)};
    if ((role == nullptr) || !role->is_string() ||
        (role->to_string() != "user" && role->to_string() != "assistant") ||
        (content == nullptr) ||
        !validation::string_field(value, "model",
                                  sourcemeta::core::MCP_HASH_MODEL, true) ||
        !validation::string_field(value, "stopReason",
                                  sourcemeta::core::MCP_HASH_STOP_REASON)) {
      return false;
    }
    if (!content->is_array()) {
      return validation::sampling_block(*content);
    }
    return std::all_of(
        content->as_array().begin(), content->as_array().end(),
        [](const auto &block) { return validation::sampling_block(block); });
  }
  return false;
}

} // namespace sourcemeta::core::internal

#endif
