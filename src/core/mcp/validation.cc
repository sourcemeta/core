#include "validation.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <initializer_list>
#include <set>

namespace {
using sourcemeta::core::JSON;
using sourcemeta::core::MCPProtocolVersion;

auto field(const JSON &value, const JSON::StringView name) -> const JSON * {
  return value.is_object() ? value.try_at(name) : nullptr;
}

auto string_field(const JSON &value, const JSON::StringView name,
                  const bool required = false) -> bool {
  const auto *item{field(value, name)};
  return item == nullptr ? !required : item->is_string();
}

auto optional_fields(const JSON &value,
                     const std::initializer_list<JSON::StringView> names,
                     const JSON::Type type) -> bool {
  return std::all_of(names.begin(), names.end(), [&](const auto name) {
    const auto *item{field(value, name)};
    return item == nullptr || item->type() == type;
  });
}

auto strings(const JSON &value) -> bool {
  return value.is_array() &&
         std::all_of(value.as_array().begin(), value.as_array().end(),
                     [](const auto &item) { return item.is_string(); });
}

auto annotations(const JSON &value) -> bool {
  const auto *item{field(value, "annotations")};
  if (item == nullptr) {
    return true;
  }
  if (!item->is_object() || !string_field(*item, "lastModified")) {
    return false;
  }
  if (const auto *priority{field(*item, "priority")};
      priority != nullptr &&
      (!priority->is_number() ||
       (priority->is_real() && !std::isfinite(priority->to_real())) ||
       *priority < JSON{0} || *priority > JSON{1})) {
    return false;
  }
  if (const auto *audience{field(*item, "audience")}; audience != nullptr) {
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

auto icons(const JSON &value) -> bool {
  const auto *items{field(value, "icons")};
  if (items == nullptr) {
    return true;
  }
  if (!items->is_array()) {
    return false;
  }
  for (const auto &item : items->as_array()) {
    if (!item.is_object() || !string_field(item, "src", true) ||
        !string_field(item, "mimeType")) {
      return false;
    }
    if (const auto *sizes{field(item, "sizes")};
        (sizes != nullptr) && !strings(*sizes)) {
      return false;
    }
    if (const auto *theme{field(item, "theme")};
        (theme != nullptr) &&
        (!theme->is_string() ||
         (theme->to_string() != "light" && theme->to_string() != "dark"))) {
      return false;
    }
  }
  return true;
}

auto object_schema(const JSON &value) -> bool {
  const auto *type{field(value, "type")};
  return (type != nullptr) && type->is_string() &&
         type->to_string() == "object" &&
         optional_fields(value, {"properties"}, JSON::Type::Object) &&
         (field(value, "required") == nullptr ||
          strings(*field(value, "required")));
}

auto base(const JSON &value) -> bool {
  return value.is_object() && string_field(value, "name", true) &&
         string_field(value, "title") && string_field(value, "description") &&
         sourcemeta::core::internal::valid_meta(value) && icons(value);
}

auto alphanumeric(const char character) noexcept -> bool {
  return (character >= 'a' && character <= 'z') ||
         (character >= 'A' && character <= 'Z') ||
         (character >= '0' && character <= '9');
}

auto metadata_key(const JSON::StringView key, const bool prefix_required)
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
        if (start || !alphanumeric(previous)) {
          return false;
        }
        start = true;
      } else {
        const bool letter{(character >= 'a' && character <= 'z') ||
                          (character >= 'A' && character <= 'Z')};
        if ((start && !letter) ||
            (!alphanumeric(character) && character != '-')) {
          return false;
        }
        start = false;
      }
      previous = character;
    }
    if (start || !alphanumeric(previous)) {
      return false;
    }
    name = key.substr(slash + 1);
  }
  return name.empty() ||
         (alphanumeric(name.front()) && alphanumeric(name.back()) &&
          std::all_of(name.begin(), name.end(), [](const char character) {
            return alphanumeric(character) || character == '_' ||
                   character == '-' || character == '.';
          }));
}

auto trim(JSON::StringView value) -> JSON::StringView {
  while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) {
    value.remove_prefix(1);
  }
  while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) {
    value.remove_suffix(1);
  }
  return value;
}

auto hex(const char character) noexcept -> bool {
  return (character >= '0' && character <= '9') ||
         (character >= 'a' && character <= 'f');
}

auto traceparent(const JSON::StringView value) -> bool {
  if (value.size() < 55 || value[2] != '-' || value[35] != '-' ||
      value[52] != '-' || value.starts_with("ff") ||
      (value.starts_with("00") && value.size() != 55) ||
      (value.size() > 55 && value[55] != '-')) {
    return false;
  }
  for (std::size_t index = 0; index < 55; ++index) {
    if (index != 2 && index != 35 && index != 52 && !hex(value[index])) {
      return false;
    }
  }
  return value.substr(3, 32).find_first_not_of('0') != JSON::StringView::npos &&
         value.substr(36, 16).find_first_not_of('0') != JSON::StringView::npos;
}

auto trace_key_part(const JSON::StringView value, const bool tenant,
                    const std::size_t maximum) -> bool {
  if (value.empty() || value.size() > maximum ||
      !((value.front() >= 'a' && value.front() <= 'z') ||
        (tenant && value.front() >= '0' && value.front() <= '9'))) {
    return false;
  }
  return std::all_of(value.begin(), value.end(), [](const char character) {
    return (character >= 'a' && character <= 'z') ||
           (character >= '0' && character <= '9') || character == '_' ||
           character == '-' || character == '*' || character == '/';
  });
}

auto tracestate(JSON::StringView value) -> bool {
  std::set<JSON::StringView> keys;
  std::size_t members = 0;
  while (true) {
    if (++members > 32) {
      return false;
    }
    const auto comma{value.find(',')};
    const auto entry{trim(value.substr(0, comma))};
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

auto baggage_token(const JSON::StringView value) -> bool {
  return !value.empty() &&
         std::all_of(value.begin(), value.end(), [](const char character) {
           return alphanumeric(character) ||
                  JSON::StringView{"!#$%&'*+-.^_`|~"}.find(character) !=
                      JSON::StringView::npos;
         });
}

auto baggage_value(const JSON::StringView value) -> bool {
  for (std::size_t index = 0; index < value.size(); ++index) {
    const auto character{value[index]};
    if (character <= 0x20 || character > 0x7e || character == '"' ||
        character == ',' || character == ';' || character == '\\') {
      return false;
    }
    if (character == '%') {
      if (index + 2 >= value.size()) {
        return false;
      }
      for (std::size_t offset = 1; offset <= 2; ++offset) {
        const auto digit{value[index + offset]};
        if (!hex(digit) && !(digit >= 'A' && digit <= 'F')) {
          return false;
        }
      }
      index += 2;
    }
  }
  return true;
}

auto baggage(JSON::StringView value) -> bool {
  std::size_t count = 0;
  while (true) {
    if (++count > 180) {
      return false;
    }
    const auto comma{value.find(',')};
    auto member{trim(value.substr(0, comma))};
    bool first = true;
    while (true) {
      const auto semicolon{member.find(';')};
      const auto property{trim(member.substr(0, semicolon))};
      const auto equal{property.find('=')};
      if ((first && equal == JSON::StringView::npos) ||
          !baggage_token(trim(property.substr(0, equal))) ||
          (equal != JSON::StringView::npos &&
           !baggage_value(trim(property.substr(equal + 1))))) {
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

auto enum_options(const JSON &value, const JSON::StringView keyword) -> bool {
  const auto *items{field(value, keyword)};
  if ((items == nullptr) || !items->is_array()) {
    return false;
  }
  for (const auto &item : items->as_array()) {
    if (!item.is_object() || !string_field(item, "const", true) ||
        !string_field(item, "title", true)) {
      return false;
    }
  }
  return true;
}

auto form_schema(const JSON &schema) -> bool {
  const auto *properties{field(schema, "properties")};
  if (!object_schema(schema) || (properties == nullptr) ||
      !properties->is_object() || !string_field(schema, "$schema")) {
    return false;
  }
  for (const auto &entry : properties->as_object()) {
    const auto &property{entry.second};
    const auto *type{field(property, "type")};
    if ((type == nullptr) || !type->is_string() ||
        !string_field(property, "title") ||
        !string_field(property, "description")) {
      return false;
    }
    const auto name{type->to_string()};
    const auto *default_value{field(property, "default")};
    if (name == "string") {
      if ((default_value != nullptr) && !default_value->is_string()) {
        return false;
      }
      if ((field(property, "enum") != nullptr) &&
          !strings(*field(property, "enum"))) {
        return false;
      }
      if ((field(property, "enumNames") != nullptr) &&
          !strings(*field(property, "enumNames"))) {
        return false;
      }
      if ((field(property, "oneOf") != nullptr) &&
          !enum_options(property, "oneOf")) {
        return false;
      }
      if (!optional_fields(property, {"minLength", "maxLength"},
                           JSON::Type::Integer)) {
        return false;
      }
      if (const auto *format{field(property, "format")};
          (format != nullptr) &&
          (!format->is_string() ||
           (format->to_string() != "date" &&
            format->to_string() != "date-time" &&
            format->to_string() != "email" && format->to_string() != "uri"))) {
        return false;
      }
    } else if (name == "boolean") {
      if ((default_value != nullptr) && !default_value->is_boolean()) {
        return false;
      }
    } else if (name == "number" || name == "integer") {
      for (const auto *const key : {"default", "minimum", "maximum"}) {
        if (const auto *value{field(property, key)};
            (value != nullptr) && !value->is_number()) {
          return false;
        }
      }
    } else if (name == "array") {
      const auto *items{field(property, "items")};
      if ((items == nullptr) || !items->is_object() ||
          ((default_value != nullptr) && !strings(*default_value)) ||
          !optional_fields(property, {"minItems", "maxItems"},
                           JSON::Type::Integer)) {
        return false;
      }
      const auto *item_type{field(*items, "type")};
      const auto *values{field(*items, "enum")};
      if (!((item_type != nullptr) && item_type->is_string() &&
            item_type->to_string() == "string" && (values != nullptr) &&
            strings(*values)) &&
          !enum_options(*items, "anyOf")) {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

auto sampling_block(const JSON &value) -> bool {
  const auto *type{field(value, "type")};
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
    const auto *input{field(value, "input")};
    return string_field(value, "id", true) &&
           string_field(value, "name", true) && (input != nullptr) &&
           input->is_object();
  }
  if (name == "tool_result") {
    const auto *content{field(value, "content")};
    if (!string_field(value, "toolUseId", true) || (content == nullptr) ||
        !content->is_array() ||
        !optional_fields(value, {"isError"}, JSON::Type::Boolean)) {
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

auto sampling_messages(
    const JSON &messages,
    const sourcemeta::core::MCPClientCapabilities &capabilities) -> bool {
  if (!messages.is_array()) {
    return false;
  }
  std::set<JSON::StringView> pending;
  for (const auto &message : messages.as_array()) {
    const auto *role{field(message, "role")};
    const auto *content{field(message, "content")};
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
    const auto block_valid = [&](const JSON &block) {
      if (!sampling_block(block)) {
        return false;
      }
      const auto type{block.at("type").to_string()};
      if (type == "tool_result") {
        return capabilities.sampling_tools && answering &&
               role->to_string() == "user" &&
               pending.erase(block.at("toolUseId").to_string()) == 1;
      }
      if (answering) {
        return false;
      }
      if (type == "tool_use") {
        return capabilities.sampling_tools &&
               role->to_string() == "assistant" &&
               next.insert(block.at("id").to_string()).second;
      }
      return true;
    };
    if (content->is_array()) {
      if (!std::all_of(content->as_array().begin(), content->as_array().end(),
                       block_valid)) {
        return false;
      }
    } else if (!block_valid(*content)) {
      return false;
    }
    if (!pending.empty()) {
      return false;
    }
    pending = std::move(next);
  }
  return pending.empty();
}

auto sampling_options(const JSON &params) -> bool {
  if (!string_field(params, "systemPrompt") ||
      !optional_fields(params, {"metadata", "modelPreferences", "toolChoice"},
                       JSON::Type::Object)) {
    return false;
  }
  if (const auto *temperature{field(params, "temperature")};
      (temperature != nullptr) && !temperature->is_number()) {
    return false;
  }
  if (const auto *stop{field(params, "stopSequences")};
      (stop != nullptr) && !strings(*stop)) {
    return false;
  }
  if (const auto *choice{field(params, "toolChoice")}; choice) {
    const auto *mode{field(*choice, "mode")};
    if ((mode != nullptr) &&
        (!mode->is_string() ||
         (mode->to_string() != "auto" && mode->to_string() != "none" &&
          mode->to_string() != "required"))) {
      return false;
    }
  }
  if (const auto *preferences{field(params, "modelPreferences")}; preferences) {
    for (const auto *const name :
         {"costPriority", "speedPriority", "intelligencePriority"}) {
      const auto *value{field(*preferences, name)};
      if ((value != nullptr) && (!value->is_number() || value->as_real() < 0 ||
                                 value->as_real() > 1)) {
        return false;
      }
    }
    if (const auto *hints{field(*preferences, "hints")}; hints) {
      if (!hints->is_array()) {
        return false;
      }
      for (const auto &hint : hints->as_array()) {
        if (!hint.is_object() || !string_field(hint, "name")) {
          return false;
        }
      }
    }
  }
  if (const auto *tools{field(params, "tools")}; tools) {
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

} // namespace

namespace sourcemeta::core::internal {

auto valid_log_level(const JSON::StringView value) noexcept -> bool {
  return mcp_resolve_log_level(value).has_value();
}

auto valid_meta(const JSON &value) -> bool {
  const auto *meta{field(value, "_meta")};
  return meta == nullptr || meta->is_object();
}

auto valid_meta(const MCPProtocolVersion version, const JSON &value) -> bool {
  const auto *meta{field(value, "_meta")};
  return meta == nullptr || (version == MCPProtocolVersion::V_2026_07_28
                                 ? valid_metadata_object(*meta)
                                 : meta->is_object());
}

auto valid_metadata_object(const JSON &meta) -> bool {
  if (!meta.is_object()) {
    return false;
  }
  for (const auto &entry : meta.as_object()) {
    if (!metadata_key(entry.first, false)) {
      return false;
    }
  }
  for (const auto *const name : {"traceparent", "tracestate", "baggage"}) {
    if (const auto *value{meta.try_at(name)}; value) {
      if (!value->is_string()) {
        return false;
      }
      const auto text{value->to_string()};
      const bool valid{
          JSON::StringView{name} == "traceparent"  ? traceparent(text)
          : JSON::StringView{name} == "tracestate" ? tracestate(text)
                                                   : baggage(text)};
      if (!valid) {
        return false;
      }
    }
  }
  return true;
}

auto valid_implementation(const MCPProtocolVersion version, const JSON &value)
    -> bool {
  if (!value.is_object() || !string_field(value, "name", true) ||
      !string_field(value, "version", true)) {
    return false;
  }
  if (mcp_supports_implementation_title(version) &&
      !string_field(value, "title")) {
    return false;
  }
  return !mcp_supports_implementation_description(version) ||
         (string_field(value, "description") &&
          string_field(value, "websiteUrl") && icons(value));
}

auto make_implementation(const MCPProtocolVersion version,
                         const MCPImplementation &value) -> JSON {
  auto result{JSON::make_object()};
  result.assign_assume_new("name", JSON{value.name});
  result.assign_assume_new("version", JSON{value.version});
  if (mcp_supports_implementation_title(version) && !value.title.empty()) {
    result.assign_assume_new("title", JSON{value.title});
  }
  if (mcp_supports_implementation_description(version)) {
    if (!value.description.empty()) {
      result.assign_assume_new("description", JSON{value.description});
    }
    if (!value.website_url.empty()) {
      result.assign_assume_new("websiteUrl", JSON{value.website_url});
    }
    if (value.icons != nullptr) {
      result.assign_assume_new("icons", JSON{*value.icons});
    }
  }
  require(valid_implementation(version, result),
          "Invalid MCP implementation information");
  return result;
}

auto valid_extensions(const JSON &value) -> bool {
  if (!value.is_object()) {
    return false;
  }
  for (const auto &entry : value.as_object()) {
    if (!metadata_key(entry.first, true) || !entry.second.is_object()) {
      return false;
    }
  }
  return true;
}

auto valid_capabilities(const MCPProtocolVersion version, const JSON &value,
                        const bool client) -> bool {
  if (!value.is_object()) {
    return false;
  }
  if (const auto *experimental{field(value, "experimental")}; experimental) {
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
    if (const auto *extensions{field(value, "extensions")};
        (extensions != nullptr) && !valid_extensions(*extensions)) {
      return false;
    }
  }
  if (version == MCPProtocolVersion::V_2025_11_25) {
    if (const auto *tasks{field(value, "tasks")}; tasks) {
      if (!tasks->is_object() ||
          !optional_fields(*tasks, {"list", "cancel", "requests"},
                           JSON::Type::Object)) {
        return false;
      }
      if (const auto *requests{field(*tasks, "requests")}; requests) {
        if (!optional_fields(*requests, {"sampling", "elicitation", "tools"},
                             JSON::Type::Object)) {
          return false;
        }
        for (const auto *const name : {"sampling", "elicitation", "tools"}) {
          if (const auto *group{field(*requests, name)};
              (group != nullptr) &&
              !optional_fields(*group, {"createMessage", "create", "call"},
                               JSON::Type::Object)) {
            return false;
          }
        }
      }
    }
  }
  if (client) {
    if (!optional_fields(value, {"roots", "sampling"}, JSON::Type::Object)) {
      return false;
    }
    if (const auto *roots{field(value, "roots")};
        (roots != nullptr) && version != MCPProtocolVersion::V_2026_07_28 &&
        !optional_fields(*roots, {"listChanged"}, JSON::Type::Boolean)) {
      return false;
    }
    if (const auto *sampling{field(value, "sampling")};
        (sampling != nullptr) &&
        mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25) &&
        !optional_fields(*sampling, {"tools", "context"}, JSON::Type::Object)) {
      return false;
    }
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_06_18)) {
      if (!optional_fields(value, {"elicitation"}, JSON::Type::Object)) {
        return false;
      }
      if (const auto *elicitation{field(value, "elicitation")};
          (elicitation != nullptr) &&
          mcp_protocol_version_at_least(version,
                                        MCPProtocolVersion::V_2025_11_25) &&
          !optional_fields(*elicitation, {"form", "url"}, JSON::Type::Object)) {
        return false;
      }
    }
  } else {
    if (!optional_fields(
            value, {"logging", "completions", "prompts", "resources", "tools"},
            JSON::Type::Object)) {
      return false;
    }
    for (const auto *const name : {"prompts", "resources", "tools"}) {
      if (const auto *item{field(value, name)};
          (item != nullptr) &&
          !optional_fields(*item, {"listChanged"}, JSON::Type::Boolean)) {
        return false;
      }
    }
    if (const auto *resources{field(value, "resources")};
        (resources != nullptr) &&
        !optional_fields(*resources, {"subscribe"}, JSON::Type::Boolean)) {
      return false;
    }
  }
  return true;
}

auto valid_tool(const MCPProtocolVersion version, const JSON &value) -> bool {
  if (!base(value) || !valid_meta(version, value)) {
    return false;
  }
  const auto *input{field(value, "inputSchema")};
  if ((input == nullptr) || !object_schema(*input)) {
    return false;
  }
  if (mcp_supports_output_schema(version)) {
    if (const auto *output{field(value, "outputSchema")};
        (output != nullptr) &&
        (!output->is_object() || (version != MCPProtocolVersion::V_2026_07_28 &&
                                  !object_schema(*output)))) {
      return false;
    }
  }
  if (const auto *item{field(value, "annotations")};
      (item != nullptr) &&
      (!item->is_object() || !string_field(*item, "title") ||
       !optional_fields(*item,
                        {"readOnlyHint", "destructiveHint", "idempotentHint",
                         "openWorldHint"},
                        JSON::Type::Boolean))) {
    return false;
  }
  return true;
}

auto valid_resource(const MCPProtocolVersion version, const JSON &value,
                    const bool contents, const bool is_template) -> bool {
  if (!value.is_object() || !valid_meta(version, value) ||
      !string_field(value, is_template ? "uriTemplate" : "uri", true) ||
      !string_field(value, "mimeType")) {
    return false;
  }
  if (contents) {
    const auto *text{field(value, "text")};
    const auto *blob{field(value, "blob")};
    return ((text != nullptr) && text->is_string()) ||
           ((blob != nullptr) && blob->is_string());
  }
  const auto *size{field(value, "size")};
  return base(value) && annotations(value) &&
         ((size == nullptr) || (size->is_integer() && size->to_integer() >= 0));
}

auto valid_content(const MCPProtocolVersion version, const JSON &value)
    -> bool {
  if (!value.is_object() || !valid_meta(version, value) ||
      !annotations(value)) {
    return false;
  }
  const auto *type{field(value, "type")};
  if ((type == nullptr) || !type->is_string()) {
    return false;
  }
  const auto name{type->to_string()};
  if (name == "text") {
    return string_field(value, "text", true);
  }
  if (name == "image" || name == "audio") {
    return string_field(value, "data", true) &&
           string_field(value, "mimeType", true);
  }
  if (name == "resource") {
    const auto *resource{field(value, "resource")};
    return (resource != nullptr) &&
           valid_resource(version, *resource, true, false);
  }
  return name == "resource_link" &&
         mcp_supports_resource_link_content(version) &&
         valid_resource(version, value, false, false);
}

auto valid_prompt(const MCPProtocolVersion version, const JSON &value) -> bool {
  if (!base(value) || !valid_meta(version, value)) {
    return false;
  }
  const auto *arguments{field(value, "arguments")};
  if (arguments == nullptr) {
    return true;
  }
  if (!arguments->is_array()) {
    return false;
  }
  for (const auto &argument : arguments->as_array()) {
    if (!base(argument) || !valid_meta(version, argument) ||
        !optional_fields(argument, {"required"}, JSON::Type::Boolean)) {
      return false;
    }
  }
  return true;
}

auto valid_input_requests(const JSON &value,
                          const MCPClientCapabilities &capabilities) -> bool {
  if (!value.is_object()) {
    return false;
  }
  for (const auto &entry : value.as_object()) {
    const auto &request{entry.second};
    const auto *method{field(request, "method")};
    const auto *params{field(request, "params")};
    if ((method == nullptr) || !method->is_string() || request.defines("id") ||
        request.defines("jsonrpc")) {
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
          !string_field(*params, "message", true)) {
        return false;
      }
      const auto *mode{field(*params, "mode")};
      if ((mode != nullptr) &&
          (!mode->is_string() ||
           (mode->to_string() != "form" && mode->to_string() != "url"))) {
        return false;
      }
      if ((mode != nullptr) && mode->to_string() == "url") {
        if (!capabilities.elicitation_url ||
            !string_field(*params, "url", true)) {
          return false;
        }
      } else {
        const auto *schema{field(*params, "requestedSchema")};
        const bool implicit_form{capabilities.elicitation &&
                                 !capabilities.elicitation_url};
        if ((!capabilities.elicitation_form && !implicit_form) ||
            (schema == nullptr) || !form_schema(*schema)) {
          return false;
        }
      }
    } else if (name == MCP_METHOD_SAMPLING_CREATE_MESSAGE) {
      if (!capabilities.sampling || (params == nullptr) ||
          !params->is_object() || !sampling_options(*params)) {
        return false;
      }
      if (const auto *context{field(*params, "includeContext")}; context) {
        if (!context->is_string() ||
            (context->to_string() != "none" &&
             context->to_string() != "thisServer" &&
             context->to_string() != "allServers") ||
            (context->to_string() != "none" &&
             !capabilities.sampling_context)) {
          return false;
        }
      }
      const auto *messages{field(*params, "messages")};
      const auto *tokens{field(*params, "maxTokens")};
      if ((messages == nullptr) ||
          !sampling_messages(*messages, capabilities) || (tokens == nullptr) ||
          !tokens->is_integral()) {
        return false;
      }
      if (((field(*params, "tools") != nullptr) ||
           (field(*params, "toolChoice") != nullptr)) &&
          !capabilities.sampling_tools) {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

auto valid_input_response(const JSON::StringView method, const JSON &request,
                          const JSON &value) -> bool {
  if (!value.is_object() ||
      !valid_meta(MCPProtocolVersion::V_2026_07_28, value)) {
    return false;
  }
  if (method == MCP_METHOD_ROOTS_LIST) {
    const auto *roots{field(value, "roots")};
    if ((roots == nullptr) || !roots->is_array()) {
      return false;
    }
    for (const auto &root : roots->as_array()) {
      const auto *uri{field(root, "uri")};
      if ((uri == nullptr) || !uri->is_string() ||
          !uri->to_string().starts_with("file://") ||
          !string_field(root, "name") ||
          !valid_meta(MCPProtocolVersion::V_2026_07_28, root)) {
        return false;
      }
    }
    return true;
  }
  if (method == MCP_METHOD_ELICITATION_CREATE) {
    const auto *action{field(value, "action")};
    if ((action == nullptr) || !action->is_string() ||
        (action->to_string() != "accept" && action->to_string() != "decline" &&
         action->to_string() != "cancel")) {
      return false;
    }
    if (const auto *content{field(value, "content")}; content) {
      const auto *params{field(request, "params")};
      const auto *mode{params != nullptr ? field(*params, "mode") : nullptr};
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
            !entry.second.is_boolean() && !strings(entry.second)) {
          return false;
        }
      }
    }
    return true;
  }
  if (method == MCP_METHOD_SAMPLING_CREATE_MESSAGE) {
    const auto *role{field(value, "role")};
    const auto *content{field(value, "content")};
    if ((role == nullptr) || !role->is_string() ||
        (role->to_string() != "user" && role->to_string() != "assistant") ||
        (content == nullptr) || !string_field(value, "model", true) ||
        !string_field(value, "stopReason")) {
      return false;
    }
    if (!content->is_array()) {
      return sampling_block(*content);
    }
    return std::all_of(content->as_array().begin(), content->as_array().end(),
                       [](const auto &block) { return sampling_block(block); });
  }
  return false;
}

} // namespace sourcemeta::core::internal
