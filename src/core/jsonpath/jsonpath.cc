#include <sourcemeta/core/jsonpath.h>

#include "evaluator.h"
#include "grammar.h"
#include "parser.h"

#include <string> // std::to_string

namespace sourcemeta::core {

JSONPath::JSONPath(const JSON::StringView expression)
    : query_{parse_jsonpath(expression)} {}

auto JSONPath::evaluate(const JSON &document, const Callback &callback) const
    -> void {
  EvaluationState state{
      .root = &document, .callback = &callback, .location = WeakPointer{}};
  evaluate_segments(this->query_.segments, 0, document, state);
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
