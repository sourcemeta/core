#ifndef SOURCEMETA_CORE_YAML_STRINGIFY_H_
#define SOURCEMETA_CORE_YAML_STRINGIFY_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/yaml_roundtrip.h>

#include <array>   // std::array
#include <cassert> // assert
#include <cmath>   // std::modf
#include <cstddef> // std::size_t
#include <iomanip> // std::setprecision
#include <ios>     // std::noshowpoint, std::fixed
#include <ostream> // std::basic_ostream
#include <string>  // std::to_string

namespace sourcemeta::core::yaml {

using OutputStream = std::basic_ostream<JSON::Char, JSON::CharTraits>;

static constexpr std::size_t INDENT_WIDTH{2};
static constexpr std::array<char, 16> HEX_DIGITS{{'0', '1', '2', '3', '4', '5',
                                                  '6', '7', '8', '9', 'a', 'b',
                                                  'c', 'd', 'e', 'f'}};

inline auto write_indent(OutputStream &stream, const std::size_t indent,
                         const std::size_t width = INDENT_WIDTH) -> void {
  for (std::size_t index{0}; index < indent * width; ++index) {
    stream.put(' ');
  }
}

inline auto looks_like_number(const std::string &value) -> bool {
  std::size_t start{0};
  if (value[0] == '-' || value[0] == '+') {
    start = 1;
  }

  if (start >= value.size()) {
    return false;
  }

  if (value.size() > start + 1 && value[start] == '0') {
    const char second{value[start + 1]};
    if (second == 'x' || second == 'X' || second == 'o' || second == 'O') {
      return true;
    }
  }

  bool has_digit{false};
  bool has_dot{false};
  bool has_exponent{false};

  for (std::size_t index{start}; index < value.size(); ++index) {
    const char character{value[index]};
    if (character >= '0' && character <= '9') {
      has_digit = true;
    } else if (character == '.' && !has_dot && !has_exponent) {
      has_dot = true;
    } else if ((character == 'e' || character == 'E') && !has_exponent &&
               has_digit) {
      has_exponent = true;
      if (index + 1 < value.size() &&
          (value[index + 1] == '+' || value[index + 1] == '-')) {
        ++index;
      }
    } else {
      return false;
    }
  }

  return has_digit;
}

inline auto needs_quoting(const std::string &value) -> bool {
  if (value.empty()) {
    return true;
  }

  if (value == "null" || value == "Null" || value == "NULL" || value == "~" ||
      value == "true" || value == "True" || value == "TRUE" ||
      value == "false" || value == "False" || value == "FALSE") {
    return true;
  }

  if (value == ".inf" || value == ".Inf" || value == ".INF" ||
      value == "+.inf" || value == "+.Inf" || value == "+.INF" ||
      value == "-.inf" || value == "-.Inf" || value == "-.INF" ||
      value == ".nan" || value == ".NaN" || value == ".NAN") {
    return true;
  }

  if (value.size() >= 3 &&
      ((value[0] == '-' && value[1] == '-' && value[2] == '-') ||
       (value[0] == '.' && value[1] == '.' && value[2] == '.')) &&
      (value.size() == 3 || value[3] == ' ' || value[3] == '\t')) {
    return true;
  }

  if (looks_like_number(value)) {
    return true;
  }

  const char first{value[0]};

  if (first == ',' || first == '[' || first == ']' || first == '{' ||
      first == '}' || first == '#' || first == '&' || first == '*' ||
      first == '!' || first == '|' || first == '>' || first == '\'' ||
      first == '"' || first == '%' || first == '@' || first == '`') {
    return true;
  }

  if (first == '-' || first == '?' || first == ':') {
    if (value.size() == 1 || value[1] == ' ') {
      return true;
    }
  }

  if (value.front() == ' ' || value.back() == ' ') {
    return true;
  }

  for (std::size_t index{0}; index < value.size(); ++index) {
    const char character{value[index]};
    if (character < ' ') {
      return true;
    }

    if (character == ':' &&
        (index + 1 >= value.size() || value[index + 1] == ' ')) {
      return true;
    }

    if (character == ' ' && index + 1 < value.size() &&
        value[index + 1] == '#') {
      return true;
    }
  }

  return false;
}

inline auto can_single_quote(const std::string &value) -> bool {
  for (const char character : value) {
    if (character < ' ' && character != '\t') {
      return false;
    }
  }

  return true;
}

inline auto write_double_quoted(OutputStream &stream, const std::string &value)
    -> void {
  stream.put('"');
  for (const char character : value) {
    switch (character) {
      case '"':
        stream.write("\\\"", 2);
        break;
      case '\\':
        stream.write("\\\\", 2);
        break;
      case '\n':
        stream.write("\\n", 2);
        break;
      case '\r':
        stream.write("\\r", 2);
        break;
      case '\t':
        stream.write("\\t", 2);
        break;
      case '\0':
        stream.write("\\0", 2);
        break;
      default:
        if (character >= '\x01' && character < '\x20') {
          const auto byte{static_cast<unsigned char>(character)};
          stream.write("\\x", 2);
          stream.put(HEX_DIGITS[byte >> 4u]);
          stream.put(HEX_DIGITS[byte & 0x0Fu]);
        } else {
          stream.put(character);
        }
        break;
    }
  }
  stream.put('"');
}

inline auto write_single_quoted(OutputStream &stream, const std::string &value)
    -> void {
  stream.put('\'');
  for (const char character : value) {
    if (character == '\'') {
      stream.write("''", 2);
    } else {
      stream.put(character);
    }
  }
  stream.put('\'');
}

inline auto write_string(OutputStream &stream, const std::string &value)
    -> void {
  if (needs_quoting(value)) {
    write_double_quoted(stream, value);
  } else {
    stream.write(value.data(), static_cast<std::streamsize>(value.size()));
  }
}

inline auto write_block_scalar(
    OutputStream &stream, const std::string &value, const std::size_t indent,
    const YAMLScalarStyle style, const YAMLChomping chomping,
    const std::optional<std::string> &header_comment = std::nullopt,
    const std::size_t indent_width = INDENT_WIDTH) -> void {
  stream.put(style == YAMLScalarStyle::Literal ? '|' : '>');
  if (chomping == YAMLChomping::Strip) {
    stream.put('-');
  } else if (chomping == YAMLChomping::Keep) {
    stream.put('+');
  }
  if (header_comment.has_value()) {
    stream.put(' ');
    const auto &comment{header_comment.value()};
    stream.write(comment.data(), static_cast<std::streamsize>(comment.size()));
  }
  stream.put('\n');

  std::size_t position{0};
  while (position < value.size()) {
    auto line_end{value.find('\n', position)};
    if (line_end == std::string::npos) {
      write_indent(stream, indent, indent_width);
      stream.write(value.data() + position,
                   static_cast<std::streamsize>(value.size() - position));
      stream.put('\n');
      break;
    }

    if (line_end > position) {
      write_indent(stream, indent, indent_width);
    }
    stream.write(value.data() + position,
                 static_cast<std::streamsize>(line_end - position));
    stream.put('\n');
    position = line_end + 1;
  }
}

inline auto write_string_with_style(OutputStream &stream,
                                    const std::string &value,
                                    const YAMLRoundTrip *roundtrip,
                                    const Pointer &pointer) -> void {
  if (roundtrip) {
    const auto match{roundtrip->styles.find(pointer)};
    if (match != roundtrip->styles.end() && match->second.scalar.has_value()) {
      switch (match->second.scalar.value()) {
        case YAMLScalarStyle::SingleQuoted:
          if (can_single_quote(value)) {
            write_single_quoted(stream, value);
            return;
          }
          break;
        case YAMLScalarStyle::DoubleQuoted:
          write_double_quoted(stream, value);
          return;
        default:
          break;
      }
    }
  }

  write_string(stream, value);
}

inline auto write_key_string(OutputStream &stream, const std::string &key,
                             const YAMLRoundTrip *roundtrip,
                             const Pointer &pointer) -> void {
  if (roundtrip) {
    const auto match{roundtrip->key_styles.find(pointer)};
    if (match != roundtrip->key_styles.end()) {
      switch (match->second) {
        case YAMLScalarStyle::SingleQuoted:
          if (can_single_quote(key)) {
            write_single_quoted(stream, key);
            return;
          }
          break;
        case YAMLScalarStyle::DoubleQuoted:
          write_double_quoted(stream, key);
          return;
        default:
          break;
      }
    }
  }
  write_string(stream, key);
}

// Forward declarations for recursive flow collection writing
inline auto write_flow_mapping(OutputStream &stream, const JSON &value,
                               const YAMLRoundTrip *roundtrip, Pointer &pointer)
    -> void;
inline auto write_flow_sequence(OutputStream &stream, const JSON &value,
                                const YAMLRoundTrip *roundtrip,
                                Pointer &pointer) -> void;

inline auto write_inline_value(OutputStream &stream, const JSON &value,
                               const YAMLRoundTrip *roundtrip, Pointer &pointer)
    -> void {
  if (roundtrip) {
    const auto alias_match{roundtrip->aliases.find(pointer)};
    if (alias_match != roundtrip->aliases.end()) {
      stream.put('*');
      const auto &name{alias_match->second};
      stream.write(name.data(), static_cast<std::streamsize>(name.size()));
      return;
    }
  }
  switch (value.type()) {
    case JSON::Type::Null:
      stream.write("null", 4);
      break;
    case JSON::Type::Boolean:
      if (value.to_boolean()) {
        stream.write("true", 4);
      } else {
        stream.write("false", 5);
      }
      break;
    case JSON::Type::Integer: {
      const auto string{std::to_string(value.to_integer())};
      stream.write(string.c_str(), static_cast<std::streamsize>(string.size()));
    } break;
    case JSON::Type::Real: {
      const auto real{value.to_real()};
      if (real == 0.0) {
        stream.write("0.0", 3);
      } else {
        const auto flags{stream.flags()};
        const auto precision{stream.precision()};
        double integer_part;
        if (std::modf(real, &integer_part) == 0.0) {
          stream << std::fixed << std::setprecision(1) << real;
        } else {
          stream << std::noshowpoint << real;
        }
        stream.flags(flags);
        stream.precision(precision);
      }
    } break;
    case JSON::Type::Decimal:
      stream << value.to_decimal().to_scientific_string();
      break;
    case JSON::Type::String:
      write_string_with_style(stream, value.to_string(), roundtrip, pointer);
      break;
    case JSON::Type::Object:
      if (value.empty()) {
        stream.write("{}", 2);
      } else {
        write_flow_mapping(stream, value, roundtrip, pointer);
      }
      break;
    case JSON::Type::Array:
      if (value.empty()) {
        stream.write("[]", 2);
      } else {
        write_flow_sequence(stream, value, roundtrip, pointer);
      }
      break;
  }
}

inline auto is_implicit_null(const JSON &value, const YAMLRoundTrip *roundtrip,
                             const Pointer &pointer) -> bool {
  if (!roundtrip || !value.is_null()) {
    return false;
  }
  if (roundtrip->aliases.contains(pointer)) {
    return false;
  }
  const auto match{roundtrip->styles.find(pointer)};
  if (match == roundtrip->styles.end()) {
    return true;
  }
  return !match->second.scalar.has_value();
}

inline auto is_flow_style(const YAMLRoundTrip *roundtrip,
                          const Pointer &pointer) -> bool {
  if (!roundtrip) {
    return false;
  }

  const auto match{roundtrip->styles.find(pointer)};
  return match != roundtrip->styles.end() &&
         match->second.collection.has_value() &&
         match->second.collection.value() == YAMLCollectionStyle::Flow;
}

inline auto has_node_prefix(const YAMLRoundTrip *roundtrip,
                            const Pointer &pointer) -> bool {
  if (!roundtrip) {
    return false;
  }

  if (roundtrip->aliases.contains(pointer)) {
    return true;
  }

  const auto match{roundtrip->styles.find(pointer)};
  return match != roundtrip->styles.end() && match->second.anchor.has_value();
}

inline auto write_flow_mapping(OutputStream &stream, const JSON &value,
                               const YAMLRoundTrip *roundtrip, Pointer &pointer)
    -> void {
  stream.put('{');
  bool first{true};
  for (const auto &entry : value.as_object()) {
    if (!first) {
      stream.write(", ", 2);
    }
    first = false;
    pointer.push_back(entry.first);
    write_key_string(stream, entry.first, roundtrip, pointer);
    stream.write(": ", 2);
    write_inline_value(stream, entry.second, roundtrip, pointer);
    pointer.pop_back();
  }
  stream.put('}');
}

inline auto write_flow_sequence(OutputStream &stream, const JSON &value,
                                const YAMLRoundTrip *roundtrip,
                                Pointer &pointer) -> void {
  stream.put('[');
  bool first{true};
  std::size_t item_index{0};
  for (const auto &item : value.as_array()) {
    if (!first) {
      stream.write(", ", 2);
    }
    first = false;
    pointer.push_back(item_index);
    write_inline_value(stream, item, roundtrip, pointer);
    pointer.pop_back();
    item_index++;
  }
  stream.put(']');
}

inline auto
write_comments_before(OutputStream &stream, const YAMLRoundTrip *roundtrip,
                      const Pointer &pointer, const std::size_t indent,
                      const std::size_t width = INDENT_WIDTH) -> void {
  if (!roundtrip) {
    return;
  }
  const auto match{roundtrip->styles.find(pointer)};
  if (match == roundtrip->styles.end()) {
    return;
  }
  for (const auto &comment : match->second.comments_before) {
    if (comment.empty()) {
      stream.put('\n');
    } else {
      write_indent(stream, indent, width);
      stream.write(comment.data(),
                   static_cast<std::streamsize>(comment.size()));
      stream.put('\n');
    }
  }
}

inline auto write_comment_inline(OutputStream &stream,
                                 const YAMLRoundTrip *roundtrip,
                                 const Pointer &pointer) -> bool {
  if (!roundtrip) {
    return false;
  }
  const auto match{roundtrip->styles.find(pointer)};
  if (match == roundtrip->styles.end() ||
      !match->second.comment_inline.has_value()) {
    return false;
  }
  stream.put(' ');
  const auto &comment{match->second.comment_inline.value()};
  stream.write(comment.data(), static_cast<std::streamsize>(comment.size()));
  return true;
}

inline auto write_block_mapping(OutputStream &stream, const JSON &value,
                                std::size_t indent, bool skip_first_indent,
                                const YAMLRoundTrip *roundtrip,
                                Pointer &pointer) -> void;
inline auto write_block_sequence(OutputStream &stream, const JSON &value,
                                 std::size_t indent, bool skip_first_indent,
                                 const YAMLRoundTrip *roundtrip,
                                 Pointer &pointer) -> void;

inline auto write_node(OutputStream &stream, const JSON &value,
                       const std::size_t indent, const bool skip_first_indent,
                       const YAMLRoundTrip *roundtrip, Pointer &pointer)
    -> void {
  // Handle alias references
  if (roundtrip) {
    const auto alias_match{roundtrip->aliases.find(pointer)};
    if (alias_match != roundtrip->aliases.end()) {
      stream.put('*');
      const auto &name{alias_match->second};
      stream.write(name.data(), static_cast<std::streamsize>(name.size()));
      write_comment_inline(stream, roundtrip, pointer);
      stream.put('\n');
      return;
    }
  }

  // Handle anchor definitions
  bool has_anchor{false};
  if (roundtrip) {
    const auto style_match{roundtrip->styles.find(pointer)};
    if (style_match != roundtrip->styles.end() &&
        style_match->second.anchor.has_value()) {
      stream.put('&');
      const auto &name{style_match->second.anchor.value()};
      stream.write(name.data(), static_cast<std::streamsize>(name.size()));
      has_anchor = true;
    }
  }

  if (value.is_object() && !value.empty()) {
    if (is_flow_style(roundtrip, pointer)) {
      if (has_anchor) {
        stream.put(' ');
      }
      write_flow_mapping(stream, value, roundtrip, pointer);
      write_comment_inline(stream, roundtrip, pointer);
      stream.put('\n');
    } else {
      if (has_anchor) {
        stream.put('\n');
      }
      write_block_mapping(stream, value, indent,
                          has_anchor ? false : skip_first_indent, roundtrip,
                          pointer);
    }
  } else if (value.is_array() && !value.empty()) {
    if (is_flow_style(roundtrip, pointer)) {
      if (has_anchor) {
        stream.put(' ');
      }
      write_flow_sequence(stream, value, roundtrip, pointer);
      write_comment_inline(stream, roundtrip, pointer);
      stream.put('\n');
    } else {
      if (has_anchor) {
        stream.put('\n');
      }
      write_block_sequence(stream, value, indent,
                           has_anchor ? false : skip_first_indent, roundtrip,
                           pointer);
    }
  } else if (roundtrip && value.is_string()) {
    const auto match{roundtrip->styles.find(pointer)};
    if (match != roundtrip->styles.end() && match->second.scalar.has_value() &&
        (match->second.scalar.value() == YAMLScalarStyle::Literal ||
         match->second.scalar.value() == YAMLScalarStyle::Folded)) {
      if (has_anchor) {
        stream.put(' ');
      }
      const auto chomping{match->second.chomping.value_or(YAMLChomping::Clip)};
      const auto &content{match->second.block_content.has_value()
                              ? match->second.block_content.value()
                              : value.to_string()};
      write_block_scalar(stream, content, indent, match->second.scalar.value(),
                         chomping, match->second.comment_inline,
                         roundtrip->indent_width);
    } else {
      if (has_anchor) {
        stream.put(' ');
      }
      write_inline_value(stream, value, roundtrip, pointer);
      write_comment_inline(stream, roundtrip, pointer);
      stream.put('\n');
    }
  } else {
    if (has_anchor) {
      stream.put(' ');
    }
    write_inline_value(stream, value, roundtrip, pointer);
    write_comment_inline(stream, roundtrip, pointer);
    stream.put('\n');
  }
}

inline auto write_block_mapping(OutputStream &stream, const JSON &value,
                                const std::size_t indent,
                                const bool skip_first_indent,
                                const YAMLRoundTrip *roundtrip,
                                Pointer &pointer) -> void {
  assert(value.is_object() && !value.empty());
  const auto width{roundtrip ? roundtrip->indent_width : INDENT_WIDTH};
  bool first{true};
  for (const auto &entry : value.as_object()) {
    pointer.push_back(entry.first);
    if (!first || !skip_first_indent) {
      write_comments_before(stream, roundtrip, pointer, indent, width);
      write_indent(stream, indent, width);
    }
    first = false;

    write_key_string(stream, entry.first, roundtrip, pointer);
    stream.put(':');

    if (is_implicit_null(entry.second, roundtrip, pointer)) {
      if (roundtrip) {
        const auto style_match{roundtrip->styles.find(pointer)};
        if (style_match != roundtrip->styles.end() &&
            style_match->second.anchor.has_value()) {
          stream.put(' ');
          stream.put('&');
          const auto &name{style_match->second.anchor.value()};
          stream.write(name.data(), static_cast<std::streamsize>(name.size()));
        }
      }
      write_comment_inline(stream, roundtrip, pointer);
      stream.put('\n');
    } else {
      bool has_indicator_comment{false};
      if (roundtrip) {
        const auto style_match{roundtrip->styles.find(pointer)};
        if (style_match != roundtrip->styles.end() &&
            style_match->second.comment_on_indicator.has_value()) {
          has_indicator_comment = true;
          stream.put(' ');
          const auto &comment{style_match->second.comment_on_indicator.value()};
          stream.write(comment.data(),
                       static_cast<std::streamsize>(comment.size()));
          stream.put('\n');
          write_indent(stream, indent + 1, width);
        }
      }
      if (!has_indicator_comment) {
        const bool nested{
            (entry.second.is_object() || entry.second.is_array()) &&
            !entry.second.empty() && !is_flow_style(roundtrip, pointer) &&
            !has_node_prefix(roundtrip, pointer)};
        if (nested) {
          write_comment_inline(stream, roundtrip, pointer);
          stream.put('\n');
        } else {
          stream.put(' ');
        }
      }
      write_node(stream, entry.second, indent + 1,
                 has_indicator_comment ? true : false, roundtrip, pointer);
    }

    pointer.pop_back();
  }
}

inline auto write_block_sequence(OutputStream &stream, const JSON &value,
                                 const std::size_t indent,
                                 const bool skip_first_indent,
                                 const YAMLRoundTrip *roundtrip,
                                 Pointer &pointer) -> void {
  assert(value.is_array() && !value.empty());
  const auto width{roundtrip ? roundtrip->indent_width : INDENT_WIDTH};
  bool first{true};
  std::size_t item_index{0};
  for (const auto &item : value.as_array()) {
    pointer.push_back(item_index);
    if (!first || !skip_first_indent) {
      write_comments_before(stream, roundtrip, pointer, indent, width);
      write_indent(stream, indent, width);
    }
    first = false;

    if (is_implicit_null(item, roundtrip, pointer)) {
      stream.put('-');
      if (roundtrip) {
        const auto style_match{roundtrip->styles.find(pointer)};
        if (style_match != roundtrip->styles.end()) {
          if (style_match->second.anchor.has_value()) {
            stream.put(' ');
            stream.put('&');
            const auto &name{style_match->second.anchor.value()};
            stream.write(name.data(),
                         static_cast<std::streamsize>(name.size()));
          }
          if (style_match->second.comment_on_indicator.has_value() &&
              !style_match->second.comment_on_indicator.value().empty()) {
            stream.put(' ');
            const auto &comment{
                style_match->second.comment_on_indicator.value()};
            stream.write(comment.data(),
                         static_cast<std::streamsize>(comment.size()));
          }
        }
      }
      write_comment_inline(stream, roundtrip, pointer);
      stream.put('\n');
    } else {
      bool has_indicator{false};
      if (roundtrip) {
        const auto style_match{roundtrip->styles.find(pointer)};
        if (style_match != roundtrip->styles.end() &&
            style_match->second.comment_on_indicator.has_value()) {
          has_indicator = true;
          const auto &comment{style_match->second.comment_on_indicator.value()};
          if (comment.empty()) {
            stream.put('-');
          } else {
            stream.write("- ", 2);
            stream.write(comment.data(),
                         static_cast<std::streamsize>(comment.size()));
          }
          stream.put('\n');
          write_indent(stream, indent + 1, width);
        }
      }
      if (!has_indicator) {
        stream.write("- ", 2);
      }
      write_node(stream, item, indent + 1, true, roundtrip, pointer);
    }

    pointer.pop_back();
    item_index++;
  }
}

template <template <typename T> typename Allocator>
auto stringify_yaml(const JSON &document, OutputStream &stream,
                    const YAMLRoundTrip *roundtrip = nullptr) -> void {
  if (roundtrip) {
    for (const auto &comment : roundtrip->leading_comments) {
      stream.write(comment.data(),
                   static_cast<std::streamsize>(comment.size()));
      stream.put('\n');
    }
  }

  if (roundtrip && roundtrip->explicit_document_start) {
    stream.write("---", 3);
    if (roundtrip->document_start_comment.has_value()) {
      stream.put(' ');
      const auto &comment{roundtrip->document_start_comment.value()};
      stream.write(comment.data(),
                   static_cast<std::streamsize>(comment.size()));
    }
    stream.put('\n');
    for (const auto &comment : roundtrip->post_start_comments) {
      stream.write(comment.data(),
                   static_cast<std::streamsize>(comment.size()));
      stream.put('\n');
    }
  }

  Pointer pointer;
  write_node(stream, document, 0, false, roundtrip, pointer);

  if (roundtrip) {
    for (const auto &comment : roundtrip->pre_end_comments) {
      stream.write(comment.data(),
                   static_cast<std::streamsize>(comment.size()));
      stream.put('\n');
    }
  }

  if (roundtrip && roundtrip->explicit_document_end) {
    stream.write("...", 3);
    if (roundtrip->document_end_comment.has_value()) {
      stream.put(' ');
      const auto &comment{roundtrip->document_end_comment.value()};
      stream.write(comment.data(),
                   static_cast<std::streamsize>(comment.size()));
    }
    stream.put('\n');
  }

  if (roundtrip) {
    for (const auto &comment : roundtrip->trailing_comments) {
      stream.write(comment.data(),
                   static_cast<std::streamsize>(comment.size()));
      stream.put('\n');
    }
  }
}

} // namespace sourcemeta::core::yaml

#endif
