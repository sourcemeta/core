#include <sourcemeta/core/jsonpath.h>

#include "evaluator.h"
#include "grammar.h"
#include "parser.h"

#include <memory>  // std::make_unique
#include <string>  // std::to_string
#include <utility> // std::move
#include <vector>  // std::vector

namespace sourcemeta::core {

JSONPath::JSONPath(const JSON::StringView expression)
    : internal_{std::make_unique<JSONPathQuery>(parse_jsonpath(expression))} {}

JSONPath::~JSONPath() = default;
JSONPath::JSONPath(JSONPath &&other) noexcept = default;
auto JSONPath::operator=(JSONPath &&other) noexcept -> JSONPath & = default;

auto JSONPath::evaluate(const JSON &document, const Callback &callback) const
    -> void {
  EvaluationState state{
      .root = &document, .callback = &callback, .location = WeakPointer{}};
  evaluate_segments(this->internal_->segments, 0, document, state);
}

auto JSONPath::evaluate(const JSON &document) const -> std::vector<Node> {
  std::vector<Node> result;
  this->evaluate(
      document,
      [&result](const JSON &value, const WeakPointer &location) -> void {
        result.push_back({.value = &value, .location = location});
      });
  return result;
}

auto JSONPath::normalize(const WeakPointer &location) -> JSON::String {
  JSON::String result{"$"};
  for (const auto &token : location) {
    if (token.is_property()) {
      result += "['";
      for (const char character : token.to_property()) {
        // RFC 9535 Section 2.7: "normal-escapable" covers the apostrophe,
        // the backslash, and the control characters with a two character
        // JSON escape, while "normal-hexchar" covers the remaining control
        // characters in lowercase hexadecimal form
        switch (character) {
          case '\x08':
            result += "\\b";
            break;
          case '\x0C':
            result += "\\f";
            break;
          case '\n':
            result += "\\n";
            break;
          case '\r':
            result += "\\r";
            break;
          case '\t':
            result += "\\t";
            break;
          case '\'':
            result += "\\'";
            break;
          case '\\':
            result += "\\\\";
            break;
          default:
            if (static_cast<unsigned char>(character) < 0x20) {
              constexpr JSON::StringView hexadecimal{"0123456789abcdef"};
              result += "\\u00";
              result +=
                  hexadecimal[(static_cast<unsigned char>(character) >> 4) &
                              0xF];
              result +=
                  hexadecimal[static_cast<unsigned char>(character) & 0xF];
            } else {
              result += character;
            }
        }
      }

      result += "']";
    } else {
      result += '[';
      result += std::to_string(token.to_index());
      result += ']';
    }
  }

  return result;
}

} // namespace sourcemeta::core
