#include <sourcemeta/core/urlpattern_component.h>

#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/urlpattern_error.h>

#include <pcre2.h> // pcre2_compile, pcre2_match, etc.

#include <algorithm>     // std::min
#include <cassert>       // assert
#include <cctype>        // std::isalpha, std::isdigit, std::isalnum
#include <cstdint>       // std::uint8_t
#include <string>        // std::string::size_type
#include <unordered_map> // std::unordered_map
#include <variant>       // std::visit, std::holds_alternative, std::get
#include <vector>        // std::vector

namespace {

auto is_valid_name_start(const char character) -> bool {
  const auto byte = static_cast<unsigned char>(character);
  return std::isalpha(byte) || character == '_' || character == '$' ||
         byte >= 0x80;
}

auto is_valid_name_char(const char character) -> bool {
  const auto byte = static_cast<unsigned char>(character);
  return std::isalnum(byte) || character == '_' || character == '$' ||
         byte >= 0x80;
}

auto is_ascii_string(const std::string &input) -> bool {
  for (const auto character : input) {
    if (static_cast<unsigned char>(character) > 127) {
      return false;
    }
  }
  return true;
}

auto escape_regex_metachar(const char character) -> std::string {
  switch (character) {
    case '\\':
    case '^':
    case '$':
    case '.':
    case '|':
    case '?':
    case '*':
    case '+':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
      return std::string{"\\"} + character;
    default:
      // NOLINTNEXTLINE(modernize-return-braced-init-list)
      return std::string(1, character);
  }
}

auto escape_regex_string(const std::string &input) -> std::string {
  std::string result;
  for (const auto character : input) {
    result += escape_regex_metachar(character);
  }
  return result;
}

// Forward declarations for group token regex building
auto build_token_regex(const sourcemeta::core::URLPatternPart &token,
                       std::vector<std::string> &group_names) -> std::string;

auto build_group_inner_regex(
    const sourcemeta::core::URLPatternPartNonGroup &tokens,
    std::vector<std::string> &group_names) -> std::string {
  std::string result;
  for (const auto &inner_token : tokens) {
    if (std::holds_alternative<sourcemeta::core::URLPatternPartChar>(
            inner_token)) {
      const auto &char_token =
          std::get<sourcemeta::core::URLPatternPartChar>(inner_token);
      result += escape_regex_string(char_token.value);
    } else if (std::holds_alternative<sourcemeta::core::URLPatternPartRegExp>(
                   inner_token)) {
      const auto &regex_token =
          std::get<sourcemeta::core::URLPatternPartRegExp>(inner_token);
      result += "(" + regex_token.original_pattern + ")";
      group_names.emplace_back("");
    } else if (std::holds_alternative<sourcemeta::core::URLPatternPartName>(
                   inner_token)) {
      const auto &name_token =
          std::get<sourcemeta::core::URLPatternPartName>(inner_token);
      result += "(.+?)";
      group_names.emplace_back(name_token.value);
    } else if (std::holds_alternative<
                   sourcemeta::core::URLPatternPartNameRegExp>(inner_token)) {
      const auto &name_regex_token =
          std::get<sourcemeta::core::URLPatternPartNameRegExp>(inner_token);
      result += "(" + name_regex_token.original_pattern + ")";
      group_names.emplace_back(name_regex_token.value);
    } else if (std::holds_alternative<sourcemeta::core::URLPatternPartAsterisk>(
                   inner_token)) {
      result += "(.*?)";
      group_names.emplace_back("");
    }
  }
  return result;
}

auto build_token_regex(const sourcemeta::core::URLPatternPart &token,
                       std::vector<std::string> &group_names) -> std::string {
  if (std::holds_alternative<sourcemeta::core::URLPatternPartChar>(token)) {
    const auto &char_token =
        std::get<sourcemeta::core::URLPatternPartChar>(token);
    return escape_regex_string(char_token.value);
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartRegExp>(token)) {
    const auto &regex_token =
        std::get<sourcemeta::core::URLPatternPartRegExp>(token);
    group_names.emplace_back("");
    return "(" + regex_token.original_pattern + ")";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartRegExpOptional>(
          token)) {
    const auto &regex_token =
        std::get<sourcemeta::core::URLPatternPartRegExpOptional>(token);
    group_names.emplace_back("");
    return "(" + regex_token.original_pattern + ")?";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartRegExpMultiple>(
          token)) {
    const auto &regex_token =
        std::get<sourcemeta::core::URLPatternPartRegExpMultiple>(token);
    group_names.emplace_back("");
    return "(" + regex_token.original_pattern + ")+";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartRegExpAsterisk>(
          token)) {
    const auto &regex_token =
        std::get<sourcemeta::core::URLPatternPartRegExpAsterisk>(token);
    group_names.emplace_back("");
    return "(" + regex_token.original_pattern + ")*";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartAsterisk>(token)) {
    group_names.emplace_back("");
    return "(.*)";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartAsteriskOptional>(
          token)) {
    group_names.emplace_back("");
    return "(.*)?";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartAsteriskMultiple>(
          token)) {
    group_names.emplace_back("");
    return "(.*)+";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartAsteriskAsterisk>(
          token)) {
    group_names.emplace_back("");
    return "(.*)*";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartName>(token)) {
    const auto &name_token =
        std::get<sourcemeta::core::URLPatternPartName>(token);
    group_names.emplace_back(name_token.value);
    // Non-greedy to allow subsequent patterns to match
    return "(.+?)";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartNameRegExp>(
          token)) {
    const auto &name_regex_token =
        std::get<sourcemeta::core::URLPatternPartNameRegExp>(token);
    group_names.emplace_back(name_regex_token.value);
    return "(" + name_regex_token.original_pattern + ")";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartNameOptional>(
          token)) {
    const auto &name_token =
        std::get<sourcemeta::core::URLPatternPartNameOptional>(token);
    group_names.emplace_back(name_token.value);
    // The ? modifier means "optional" (zero or more chars)
    return "(.*)";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartNameMultiple>(
          token)) {
    const auto &name_token =
        std::get<sourcemeta::core::URLPatternPartNameMultiple>(token);
    group_names.emplace_back(name_token.value);
    // The + modifier means "one or more characters", not "repeat the capture"
    return "(.+)";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartNameAsterisk>(
          token)) {
    const auto &name_token =
        std::get<sourcemeta::core::URLPatternPartNameAsterisk>(token);
    group_names.emplace_back(name_token.value);
    // The * modifier means "zero or more characters", not "repeat the capture"
    return "(.*)";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartGroup>(token)) {
    const auto &group_token =
        std::get<sourcemeta::core::URLPatternPartGroup>(token);
    return build_group_inner_regex(group_token.value, group_names);
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartGroupOptional>(
          token)) {
    const auto &group_token =
        std::get<sourcemeta::core::URLPatternPartGroupOptional>(token);
    return "(?:" + build_group_inner_regex(group_token.value, group_names) +
           ")?";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartGroupMultiple>(
          token)) {
    const auto &group_token =
        std::get<sourcemeta::core::URLPatternPartGroupMultiple>(token);
    return "(?:" + build_group_inner_regex(group_token.value, group_names) +
           ")+";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartGroupAsterisk>(
          token)) {
    const auto &group_token =
        std::get<sourcemeta::core::URLPatternPartGroupAsterisk>(token);
    return "(?:" + build_group_inner_regex(group_token.value, group_names) +
           ")*";
  }

  if (std::holds_alternative<sourcemeta::core::URLPatternPartComplexSegment>(
          token)) {
    const auto &segment_token =
        std::get<sourcemeta::core::URLPatternPartComplexSegment>(token);
    std::string result;
    for (const auto &inner_token : segment_token.value) {
      if (std::holds_alternative<sourcemeta::core::URLPatternPartChar>(
              inner_token)) {
        const auto &char_token =
            std::get<sourcemeta::core::URLPatternPartChar>(inner_token);
        result += escape_regex_string(char_token.value);
      } else if (std::holds_alternative<sourcemeta::core::URLPatternPartName>(
                     inner_token)) {
        const auto &name_token =
            std::get<sourcemeta::core::URLPatternPartName>(inner_token);
        group_names.emplace_back(name_token.value);
        // Non-greedy, at least one char, to allow subsequent patterns to match
        result += "(.+?)";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartNameRegExp>(inner_token)) {
        const auto &name_regex_token =
            std::get<sourcemeta::core::URLPatternPartNameRegExp>(inner_token);
        group_names.emplace_back(name_regex_token.value);
        result += "(" + name_regex_token.original_pattern + ")";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartAsterisk>(inner_token)) {
        group_names.emplace_back("");
        // Greedy match for asterisk in complex segment
        result += "([^/]*)";
      } else if (std::holds_alternative<sourcemeta::core::URLPatternPartRegExp>(
                     inner_token)) {
        const auto &regex_token =
            std::get<sourcemeta::core::URLPatternPartRegExp>(inner_token);
        group_names.emplace_back("");
        result += "(" + regex_token.original_pattern + ")";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartRegExpOptional>(
                     inner_token)) {
        const auto &regex_token =
            std::get<sourcemeta::core::URLPatternPartRegExpOptional>(
                inner_token);
        group_names.emplace_back("");
        result += "(" + regex_token.original_pattern + ")?";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartRegExpMultiple>(
                     inner_token)) {
        const auto &regex_token =
            std::get<sourcemeta::core::URLPatternPartRegExpMultiple>(
                inner_token);
        group_names.emplace_back("");
        result += "(" + regex_token.original_pattern + ")+";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartRegExpAsterisk>(
                     inner_token)) {
        const auto &regex_token =
            std::get<sourcemeta::core::URLPatternPartRegExpAsterisk>(
                inner_token);
        group_names.emplace_back("");
        result += "(" + regex_token.original_pattern + ")*";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartNameOptional>(
                     inner_token)) {
        const auto &name_token =
            std::get<sourcemeta::core::URLPatternPartNameOptional>(inner_token);
        group_names.emplace_back(name_token.value);
        result += "([^/]*)?";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartNameMultiple>(
                     inner_token)) {
        const auto &name_token =
            std::get<sourcemeta::core::URLPatternPartNameMultiple>(inner_token);
        group_names.emplace_back(name_token.value);
        result += "([^/]*)+";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartNameAsterisk>(
                     inner_token)) {
        const auto &name_token =
            std::get<sourcemeta::core::URLPatternPartNameAsterisk>(inner_token);
        group_names.emplace_back(name_token.value);
        result += "([^/]*)*";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartAsteriskOptional>(
                     inner_token)) {
        group_names.emplace_back("");
        // Use (?:([^/]+))? so group is truly unset when nothing matches
        result += "(?:([^/]+))?";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartAsteriskMultiple>(
                     inner_token)) {
        group_names.emplace_back("");
        result += "([^/]*)+";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartAsteriskAsterisk>(
                     inner_token)) {
        group_names.emplace_back("");
        result += "([^/]*)*";
      } else if (std::holds_alternative<sourcemeta::core::URLPatternPartGroup>(
                     inner_token)) {
        const auto &group_token =
            std::get<sourcemeta::core::URLPatternPartGroup>(inner_token);
        result += build_group_inner_regex(group_token.value, group_names);
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartGroupOptional>(
                     inner_token)) {
        const auto &group_token =
            std::get<sourcemeta::core::URLPatternPartGroupOptional>(
                inner_token);
        result +=
            "(?:" + build_group_inner_regex(group_token.value, group_names) +
            ")?";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartGroupMultiple>(
                     inner_token)) {
        const auto &group_token =
            std::get<sourcemeta::core::URLPatternPartGroupMultiple>(
                inner_token);
        result +=
            "(?:" + build_group_inner_regex(group_token.value, group_names) +
            ")+";
      } else if (std::holds_alternative<
                     sourcemeta::core::URLPatternPartGroupAsterisk>(
                     inner_token)) {
        const auto &group_token =
            std::get<sourcemeta::core::URLPatternPartGroupAsterisk>(
                inner_token);
        result +=
            "(?:" + build_group_inner_regex(group_token.value, group_names) +
            ")*";
      }
    }
    return result;
  }

  return "";
}

auto match_bare_pattern_with_captures(
    const std::vector<sourcemeta::core::URLPatternPart> &tokens,
    const std::string_view input)
    -> std::optional<sourcemeta::core::URLPatternComponentResult> {
  std::vector<std::string> group_names;
  std::string regex_pattern = "^";
  for (std::size_t index = 0; index < tokens.size(); index += 1) {
    if (index > 0) {
      // Add segment delimiter between tokens
      regex_pattern += "/";
    }
    regex_pattern += build_token_regex(tokens[index], group_names);
  }
  regex_pattern += "$";

  int error_code{0};
  PCRE2_SIZE error_offset{0};
  pcre2_code *regex_raw{pcre2_compile(
      reinterpret_cast<PCRE2_SPTR>(regex_pattern.c_str()), regex_pattern.size(),
      PCRE2_UTF, &error_code, &error_offset, nullptr)};

  if (regex_raw == nullptr) {
    return std::nullopt;
  }

  pcre2_match_data *match_data{
      pcre2_match_data_create_from_pattern(regex_raw, nullptr)};

  const auto match_result{
      pcre2_match(regex_raw, reinterpret_cast<PCRE2_SPTR>(input.data()),
                  input.size(), 0, 0, match_data, nullptr)};

  if (match_result < 0) {
    pcre2_match_data_free(match_data);
    pcre2_code_free(regex_raw);
    return std::nullopt;
  }

  sourcemeta::core::URLPatternComponentResult result;
  const PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);

  const auto match_count = static_cast<std::size_t>(match_result);
  std::size_t unnamed_index{0};
  for (std::size_t index = 1;
       index < match_count && index <= group_names.size(); index += 1) {
    const PCRE2_SIZE start = ovector[2 * index];
    const PCRE2_SIZE end = ovector[2 * index + 1];

    if (start == PCRE2_UNSET || end == PCRE2_UNSET) {
      continue;
    }

    const std::string_view captured{input.data() + start, end - start};
    const auto &name = group_names[index - 1];
    if (name.empty()) {
      result.insert(captured);
      result.insert(std::to_string(unnamed_index), result.size() - 1);
      unnamed_index += 1;
    } else {
      result.insert(name, captured);
    }
  }

  pcre2_match_data_free(match_data);
  pcre2_code_free(regex_raw);
  return result;
}

auto parse_single_token(const char *input) -> sourcemeta::core::URLPatternPart {
  const std::string_view input_view{input};

  enum class State : std::uint8_t {
    ReadingChar,
    ReadingName,
    ReadingRegExp,
    AfterBackslash,
    ReadingGroup
  };

  auto position = std::string::size_type{0};
  auto current_state = State::ReadingChar;
  std::string token_buffer;
  std::string name_for_regex;
  sourcemeta::core::URLPatternPartGroup current_group;
  auto inside_group = false;

  while (position < input_view.size()) {
    const auto current = input_view[position];

    if (current_state == State::ReadingChar) {
      if (current == ':') {
        if (!token_buffer.empty()) {
          return sourcemeta::core::URLPatternPartChar{token_buffer};
        }
        current_state = State::ReadingName;
        position += 1;
      } else if (current == '*') {
        if (!token_buffer.empty()) {
          return sourcemeta::core::URLPatternPartChar{token_buffer};
        }
        const auto is_asterisk_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        if (is_asterisk_asterisk) {
          if (position + 2 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 2};
          }
          return sourcemeta::core::URLPatternPartAsteriskAsterisk{};
        } else if (is_optional) {
          if (position + 2 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 2};
          }
          return sourcemeta::core::URLPatternPartAsteriskOptional{};
        } else if (is_multiple) {
          if (position + 2 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 2};
          }
          return sourcemeta::core::URLPatternPartAsteriskMultiple{};
        } else {
          if (position + 1 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 1};
          }
          return sourcemeta::core::URLPatternPartAsterisk{};
        }
      } else if (current == '(') {
        if (!token_buffer.empty()) {
          return sourcemeta::core::URLPatternPartChar{token_buffer};
        }
        current_state = State::ReadingRegExp;
        position += 1;
      } else if (current == '{') {
        if (!token_buffer.empty()) {
          return sourcemeta::core::URLPatternPartChar{token_buffer};
        }
        current_group = sourcemeta::core::URLPatternPartGroup{};
        current_state = State::ReadingGroup;
        inside_group = true;
        position += 1;
      } else if (current == '\\') {
        current_state = State::AfterBackslash;
        position += 1;
      } else {
        token_buffer += current;
        position += 1;
      }
    } else if (current_state == State::ReadingName) {
      if (token_buffer.empty()) {
        if (!is_valid_name_start(current)) {
          throw sourcemeta::core::URLPatternParseError{position};
        }
        token_buffer += current;
        position += 1;
      } else {
        if (current == '}' && inside_group) {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartName{token_buffer});
          token_buffer.clear();
          const auto is_optional = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '?';
          const auto is_multiple = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '+';
          const auto is_asterisk = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '*';
          if (is_optional) {
            if (position + 2 != input_view.size()) {
              throw sourcemeta::core::URLPatternParseError{position + 2};
            }
            return sourcemeta::core::URLPatternPartGroupOptional{
                .value = current_group.value};
          } else if (is_multiple) {
            if (position + 2 != input_view.size()) {
              throw sourcemeta::core::URLPatternParseError{position + 2};
            }
            return sourcemeta::core::URLPatternPartGroupMultiple{
                .value = current_group.value};
          } else if (is_asterisk) {
            if (position + 2 != input_view.size()) {
              throw sourcemeta::core::URLPatternParseError{position + 2};
            }
            return sourcemeta::core::URLPatternPartGroupAsterisk{
                .value = current_group.value};
          } else {
            if (position + 1 != input_view.size()) {
              throw sourcemeta::core::URLPatternParseError{position + 1};
            }
            return current_group;
          }
        } else if (current == '(') {
          name_for_regex = token_buffer;
          token_buffer.clear();
          current_state = State::ReadingRegExp;
          position += 1;
        } else if (current == '?') {
          if (position + 1 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 1};
          }
          return sourcemeta::core::URLPatternPartNameOptional{token_buffer};
        } else if (current == '+') {
          if (position + 1 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 1};
          }
          return sourcemeta::core::URLPatternPartNameMultiple{token_buffer};
        } else if (current == '*') {
          if (position + 1 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 1};
          }
          return sourcemeta::core::URLPatternPartNameAsterisk{token_buffer};
        } else if (is_valid_name_char(current)) {
          token_buffer += current;
          position += 1;
        } else {
          throw sourcemeta::core::URLPatternParseError{position};
        }
      }
    } else if (current_state == State::ReadingRegExp) {
      if (current == '\\') {
        token_buffer += current;
        current_state = State::AfterBackslash;
        position += 1;
      } else if (current == ')') {
        if (!is_ascii_string(token_buffer)) {
          throw sourcemeta::core::URLPatternParseError{position};
        }
        const auto regex_opt{
            sourcemeta::core::to_regex("^" + token_buffer + "$")};
        if (!regex_opt.has_value()) {
          throw sourcemeta::core::URLPatternParseError{position};
        }
        const auto &regex{regex_opt.value()};
        const std::string original_pattern{token_buffer};
        token_buffer.clear();
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        const auto is_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        if (!name_for_regex.empty()) {
          if (is_optional || is_multiple || is_asterisk) {
            throw sourcemeta::core::URLPatternParseError{position + 1};
          }
          if (position + 1 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 1};
          }
          return sourcemeta::core::URLPatternPartNameRegExp{
              .value = name_for_regex,
              .modifier = regex,
              .original_pattern = original_pattern};
        } else if (inside_group) {
          if (is_optional) {
            current_group.value.emplace_back(
                sourcemeta::core::URLPatternPartRegExpOptional{
                    .value = regex, .original_pattern = original_pattern});
          } else if (is_multiple) {
            current_group.value.emplace_back(
                sourcemeta::core::URLPatternPartRegExpMultiple{
                    .value = regex, .original_pattern = original_pattern});
          } else if (is_asterisk) {
            current_group.value.emplace_back(
                sourcemeta::core::URLPatternPartRegExpAsterisk{
                    .value = regex, .original_pattern = original_pattern});
          } else {
            current_group.value.emplace_back(
                sourcemeta::core::URLPatternPartRegExp{
                    .value = regex, .original_pattern = original_pattern});
          }
          if (is_optional || is_multiple || is_asterisk) {
            position += 2;
          } else {
            position += 1;
          }
          current_state = State::ReadingGroup;
        } else {
          if (is_optional) {
            if (position + 2 != input_view.size()) {
              throw sourcemeta::core::URLPatternParseError{position + 2};
            }
            return sourcemeta::core::URLPatternPartRegExpOptional{
                .value = regex, .original_pattern = original_pattern};
          } else if (is_multiple) {
            if (position + 2 != input_view.size()) {
              throw sourcemeta::core::URLPatternParseError{position + 2};
            }
            return sourcemeta::core::URLPatternPartRegExpMultiple{
                .value = regex, .original_pattern = original_pattern};
          } else if (is_asterisk) {
            if (position + 2 != input_view.size()) {
              throw sourcemeta::core::URLPatternParseError{position + 2};
            }
            return sourcemeta::core::URLPatternPartRegExpAsterisk{
                .value = regex, .original_pattern = original_pattern};
          } else {
            if (position + 1 != input_view.size()) {
              throw sourcemeta::core::URLPatternParseError{position + 1};
            }
            return sourcemeta::core::URLPatternPartRegExp{
                .value = regex, .original_pattern = original_pattern};
          }
        }
      } else {
        token_buffer += current;
        position += 1;
      }
    } else if (current_state == State::AfterBackslash) {
      token_buffer += current;
      current_state = inside_group ? State::ReadingGroup : State::ReadingRegExp;
      position += 1;
    } else if (current_state == State::ReadingGroup) {
      if (current == ':') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingName;
        position += 1;
      } else if (current == '*') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        const auto is_asterisk_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        if (is_asterisk_asterisk) {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartAsteriskAsterisk{});
          position += 2;
        } else if (is_optional) {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartAsteriskOptional{});
          position += 2;
        } else if (is_multiple) {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartAsteriskMultiple{});
          position += 2;
        } else {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartAsterisk{});
          position += 1;
        }
      } else if (current == '(') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingRegExp;
        position += 1;
      } else if (current == '}') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(
              sourcemeta::core::URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        const auto is_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        if (is_optional) {
          if (position + 2 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 2};
          }
          return sourcemeta::core::URLPatternPartGroupOptional{
              .value = current_group.value};
        } else if (is_multiple) {
          if (position + 2 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 2};
          }
          return sourcemeta::core::URLPatternPartGroupMultiple{
              .value = current_group.value};
        } else if (is_asterisk) {
          if (position + 2 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 2};
          }
          return sourcemeta::core::URLPatternPartGroupAsterisk{
              .value = current_group.value};
        } else {
          if (position + 1 != input_view.size()) {
            throw sourcemeta::core::URLPatternParseError{position + 1};
          }
          return current_group;
        }
      } else if (current == '\\') {
        current_state = State::AfterBackslash;
        position += 1;
      } else {
        token_buffer += current;
        position += 1;
      }
    }
  }

  if (current_state == State::ReadingChar) {
    return sourcemeta::core::URLPatternPartChar{token_buffer};
  } else if (current_state == State::ReadingName) {
    if (!token_buffer.empty()) {
      return sourcemeta::core::URLPatternPartName{token_buffer};
    }
    throw sourcemeta::core::URLPatternParseError{position};
  } else if (current_state == State::ReadingRegExp ||
             current_state == State::ReadingGroup) {
    throw sourcemeta::core::URLPatternParseError{position};
  }

  return sourcemeta::core::URLPatternPartChar{""};
}

} // namespace

namespace sourcemeta::core {

auto URLPatternComponentResult::size() const noexcept -> std::size_t {
  return this->positions.size();
}

auto URLPatternComponentResult::at(const std::size_t index) const noexcept
    -> std::string_view {
  assert(index < this->positions.size());
  return this->positions.at(index);
}

auto URLPatternComponentResult::at(const std::string_view name) const noexcept
    -> std::optional<std::string_view> {
  const auto iterator{this->names.find(std::string{name})};
  if (iterator == this->names.end()) {
    return std::nullopt;
  }
  return this->at(iterator->second);
}

auto URLPatternComponentResult::insert(const std::string_view value) -> void {
  this->positions.push_back(value);
}

auto URLPatternComponentResult::insert(const std::string_view name,
                                       const std::string_view value) -> void {
  this->insert(value);
  this->names.emplace(std::string{name}, this->positions.size() - 1);
}

auto URLPatternComponentResult::insert(const std::string_view name,
                                       const std::size_t index) -> void {
  this->names.emplace(std::string{name}, index);
}

namespace {

auto match_single_part(const URLPatternPart &value,
                       const std::string_view input)
    -> std::optional<URLPatternComponentResult> {
  URLPatternComponentResult result;

  const auto token_matches = std::visit(
      [&input](const auto &token_value) -> bool {
        return token_value.matches(input);
      },
      value);

  if (!token_matches) {
    return std::nullopt;
  }

  if (std::holds_alternative<URLPatternPartChar>(value) ||
      std::holds_alternative<URLPatternPartGroup>(value) ||
      std::holds_alternative<URLPatternPartGroupOptional>(value) ||
      std::holds_alternative<URLPatternPartGroupMultiple>(value) ||
      std::holds_alternative<URLPatternPartGroupAsterisk>(value)) {
    return result;
  }

  if (std::holds_alternative<URLPatternPartName>(value)) {
    const auto &name_token = std::get<URLPatternPartName>(value);
    result.insert(name_token.value, input);
  } else if (std::holds_alternative<URLPatternPartNameRegExp>(value)) {
    const auto &name_regex_token = std::get<URLPatternPartNameRegExp>(value);
    result.insert(name_regex_token.value, input);
  } else if (std::holds_alternative<URLPatternPartNameOptional>(value)) {
    const auto &name_optional_token =
        std::get<URLPatternPartNameOptional>(value);
    result.insert(name_optional_token.value, input);
  } else if (std::holds_alternative<URLPatternPartNameMultiple>(value)) {
    const auto &name_multiple_token =
        std::get<URLPatternPartNameMultiple>(value);
    result.insert(name_multiple_token.value, input);
  } else if (std::holds_alternative<URLPatternPartNameAsterisk>(value)) {
    const auto &name_asterisk_token =
        std::get<URLPatternPartNameAsterisk>(value);
    result.insert(name_asterisk_token.value, input);
  } else {
    result.insert(input);
  }

  return result;
}

} // namespace

namespace {

// WHATWG URLPattern comparison: part type indices
// See https://urlpattern.spec.whatwg.org/#compare-component-part-lists
// Lower index = more specific = sorts first
enum class PartType : std::uint8_t {
  FixedText = 0,       // Literal characters
  Regexp = 1,          // Custom regex pattern
  SegmentWildcard = 2, // Named parameter like :id
  FullWildcard = 3     // Asterisk wildcard *
};

// WHATWG URLPattern comparison: modifier indices
// Lower index = more specific = sorts first
enum class ModifierType : std::uint8_t {
  None = 0,      // Required, single match
  Optional = 1,  // ? modifier
  OneOrMore = 2, // + modifier
  ZeroOrMore = 3 // * modifier
};

auto get_part_type(const URLPatternPart &part) -> PartType;
auto get_modifier_type(const URLPatternPart &part) -> ModifierType;
auto get_fixed_text_value(const URLPatternPart &part) -> std::string_view;
auto get_regexp_pattern(const URLPatternPart &part) -> std::string_view;

template <typename T> constexpr auto get_nongroup_part_type_impl() -> PartType {
  if constexpr (std::is_same_v<T, URLPatternPartRegExp> ||
                std::is_same_v<T, URLPatternPartRegExpOptional> ||
                std::is_same_v<T, URLPatternPartRegExpMultiple> ||
                std::is_same_v<T, URLPatternPartRegExpAsterisk> ||
                std::is_same_v<T, URLPatternPartNameRegExp>) {
    return PartType::Regexp;
  } else if constexpr (std::is_same_v<T, URLPatternPartName> ||
                       std::is_same_v<T, URLPatternPartNameOptional> ||
                       std::is_same_v<T, URLPatternPartNameMultiple> ||
                       std::is_same_v<T, URLPatternPartNameAsterisk>) {
    return PartType::SegmentWildcard;
  } else if constexpr (std::is_same_v<T, URLPatternPartAsterisk> ||
                       std::is_same_v<T, URLPatternPartAsteriskOptional> ||
                       std::is_same_v<T, URLPatternPartAsteriskMultiple> ||
                       std::is_same_v<T, URLPatternPartAsteriskAsterisk>) {
    return PartType::FullWildcard;
  } else {
    return PartType::FixedText;
  }
}

auto get_nongroup_part_type(const auto &part) -> PartType {
  using T = std::decay_t<decltype(part)>;
  return get_nongroup_part_type_impl<T>();
}

auto get_nongroup_modifier_type(const auto &part) -> ModifierType {
  using T = std::decay_t<decltype(part)>;
  if constexpr (std::is_same_v<T, URLPatternPartRegExpOptional> ||
                std::is_same_v<T, URLPatternPartNameOptional> ||
                std::is_same_v<T, URLPatternPartAsteriskOptional>) {
    return ModifierType::Optional;
  } else if constexpr (std::is_same_v<T, URLPatternPartRegExpMultiple> ||
                       std::is_same_v<T, URLPatternPartNameMultiple> ||
                       std::is_same_v<T, URLPatternPartAsteriskMultiple>) {
    return ModifierType::OneOrMore;
  } else if constexpr (std::is_same_v<T, URLPatternPartRegExpAsterisk> ||
                       std::is_same_v<T, URLPatternPartNameAsterisk> ||
                       std::is_same_v<T, URLPatternPartAsteriskAsterisk>) {
    return ModifierType::ZeroOrMore;
  } else {
    return ModifierType::None;
  }
}

auto get_group_part_type(const URLPatternPartNonGroup &contents) -> PartType {
  auto result = PartType::FixedText;
  for (const auto &item : contents) {
    const auto item_type = std::visit(
        [](const auto &p) { return get_nongroup_part_type(p); }, item);
    if (static_cast<int>(item_type) > static_cast<int>(result)) {
      result = item_type;
    }
  }
  return result;
}

auto get_part_type(const URLPatternPart &part) -> PartType {
  return std::visit(
      [](const auto &p) -> PartType {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, URLPatternPartGroup> ||
                      std::is_same_v<T, URLPatternPartGroupOptional> ||
                      std::is_same_v<T, URLPatternPartGroupMultiple> ||
                      std::is_same_v<T, URLPatternPartGroupAsterisk>) {
          return get_group_part_type(p.value);
        } else if constexpr (std::is_same_v<T, URLPatternPartComplexSegment>) {
          auto result = PartType::FixedText;
          for (const auto &item : p.value) {
            const auto item_type = std::visit(
                [](const auto &inner) -> PartType {
                  using InnerT = std::decay_t<decltype(inner)>;
                  if constexpr (std::is_same_v<InnerT, URLPatternPartGroup> ||
                                std::is_same_v<InnerT,
                                               URLPatternPartGroupOptional> ||
                                std::is_same_v<InnerT,
                                               URLPatternPartGroupMultiple> ||
                                std::is_same_v<InnerT,
                                               URLPatternPartGroupAsterisk>) {
                    return get_group_part_type(inner.value);
                  } else {
                    return get_nongroup_part_type(inner);
                  }
                },
                item);
            if (static_cast<int>(item_type) > static_cast<int>(result)) {
              result = item_type;
            }
          }
          return result;
        } else {
          return get_nongroup_part_type(p);
        }
      },
      part);
}

auto get_modifier_type(const URLPatternPart &part) -> ModifierType {
  return std::visit(
      [](const auto &p) -> ModifierType {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, URLPatternPartGroupOptional>) {
          return ModifierType::Optional;
        } else if constexpr (std::is_same_v<T, URLPatternPartGroupMultiple>) {
          return ModifierType::OneOrMore;
        } else if constexpr (std::is_same_v<T, URLPatternPartGroupAsterisk>) {
          return ModifierType::ZeroOrMore;
        } else if constexpr (std::is_same_v<T, URLPatternPartGroup> ||
                             std::is_same_v<T, URLPatternPartComplexSegment>) {
          return ModifierType::None;
        } else {
          return get_nongroup_modifier_type(p);
        }
      },
      part);
}

auto get_fixed_text_value(const URLPatternPart &part) -> std::string_view {
  return std::visit(
      [](const auto &p) -> std::string_view {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, URLPatternPartChar>) {
          return p.value;
        } else if constexpr (std::is_same_v<T, URLPatternPartGroup> ||
                             std::is_same_v<T, URLPatternPartGroupOptional> ||
                             std::is_same_v<T, URLPatternPartGroupMultiple> ||
                             std::is_same_v<T, URLPatternPartGroupAsterisk>) {
          if (p.value.size() == 1) {
            if (const auto *char_part =
                    std::get_if<URLPatternPartChar>(&p.value[0])) {
              return char_part->value;
            }
          }
          return {};
        } else {
          return {};
        }
      },
      part);
}

auto get_regexp_pattern(const URLPatternPart &part) -> std::string_view {
  return std::visit(
      [](const auto &p) -> std::string_view {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, URLPatternPartRegExp> ||
                      std::is_same_v<T, URLPatternPartRegExpOptional> ||
                      std::is_same_v<T, URLPatternPartRegExpMultiple> ||
                      std::is_same_v<T, URLPatternPartRegExpAsterisk> ||
                      std::is_same_v<T, URLPatternPartNameRegExp>) {
          return p.original_pattern;
        } else {
          return {};
        }
      },
      part);
}

auto compare_urlpattern_parts(const URLPatternPart &left,
                              const URLPatternPart &right)
    -> std::strong_ordering;
auto is_group_type(const URLPatternPart &part) -> bool;
auto get_inner_segment_prefix(const URLPatternPart &part) -> bool;
auto get_inner_segment_suffix(const URLPatternPart &part) -> bool;

auto compare_urlpattern_parts_vector(const std::vector<URLPatternPart> &left,
                                     const std::vector<URLPatternPart> &right)
    -> std::strong_ordering {
  const auto min_size = std::min(left.size(), right.size());

  for (std::size_t index{0}; index < min_size; index++) {
    const auto comparison{compare_urlpattern_parts(left[index], right[index])};
    if (comparison != std::strong_ordering::equal) {
      return comparison;
    }
  }

  if (left.size() != right.size()) {
    return left.size() <=> right.size();
  }

  return std::strong_ordering::equal;
}

auto is_group_type(const URLPatternPart &part) -> bool {
  return std::holds_alternative<URLPatternPartGroup>(part) ||
         std::holds_alternative<URLPatternPartGroupOptional>(part) ||
         std::holds_alternative<URLPatternPartGroupMultiple>(part) ||
         std::holds_alternative<URLPatternPartGroupAsterisk>(part);
}

auto get_inner_segment_prefix(const URLPatternPart &part) -> bool {
  return std::visit(
      [](const auto &p) -> bool {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, URLPatternPartGroup> ||
                      std::is_same_v<T, URLPatternPartGroupOptional> ||
                      std::is_same_v<T, URLPatternPartGroupMultiple> ||
                      std::is_same_v<T, URLPatternPartGroupAsterisk>) {
          return p.has_inner_segment_prefix;
        } else {
          return false;
        }
      },
      part);
}

auto get_inner_segment_suffix(const URLPatternPart &part) -> bool {
  return std::visit(
      [](const auto &p) -> bool {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, URLPatternPartGroup> ||
                      std::is_same_v<T, URLPatternPartGroupOptional> ||
                      std::is_same_v<T, URLPatternPartGroupMultiple> ||
                      std::is_same_v<T, URLPatternPartGroupAsterisk>) {
          return p.has_inner_segment_suffix;
        } else {
          return false;
        }
      },
      part);
}

auto compare_urlpattern_parts(const URLPatternPart &left,
                              const URLPatternPart &right)
    -> std::strong_ordering {
  const auto left_type = get_part_type(left);
  const auto right_type = get_part_type(right);

  if (left_type != right_type) {
    return static_cast<int>(right_type) <=> static_cast<int>(left_type);
  }

  if (left_type == PartType::FixedText) {
    const auto left_value = get_fixed_text_value(left);
    const auto right_value = get_fixed_text_value(right);
    if (left_value != right_value) {
      return left_value <=> right_value;
    }
  }

  if (left_type == PartType::Regexp) {
    const auto left_pattern = get_regexp_pattern(left);
    const auto right_pattern = get_regexp_pattern(right);
    if (left_pattern.size() != right_pattern.size()) {
      return left_pattern.size() <=> right_pattern.size();
    }
  }

  if (left_type == PartType::SegmentWildcard) {
    const auto left_is_group = is_group_type(left);
    const auto right_is_group = is_group_type(right);

    if (left_is_group != right_is_group) {
      auto group_is_more_specific = [](const URLPatternPart &part) -> bool {
        const auto has_prefix = get_inner_segment_prefix(part);
        const auto has_suffix = get_inner_segment_suffix(part);
        return !has_prefix || has_suffix;
      };

      if (left_is_group && group_is_more_specific(left)) {
        return std::strong_ordering::greater;
      }
      if (right_is_group && group_is_more_specific(right)) {
        return std::strong_ordering::less;
      }
    }
  }

  const auto left_modifier = get_modifier_type(left);
  const auto right_modifier = get_modifier_type(right);

  auto modifier_sort_value = [](ModifierType mod) -> int {
    switch (mod) {
      case ModifierType::None:
        return 0;
      case ModifierType::OneOrMore:
        return 1;
      case ModifierType::Optional:
        return 2;
      case ModifierType::ZeroOrMore:
        return 3;
    }
    return 0;
  };

  return modifier_sort_value(right_modifier) <=>
         modifier_sort_value(left_modifier);
}

} // namespace

auto URLPatternProtocol::match(const std::string_view input) const
    -> std::optional<URLPatternComponentResult> {
  return match_single_part(this->value, input);
}

auto URLPatternUsername::match(const std::string_view input) const
    -> std::optional<URLPatternComponentResult> {
  return match_single_part(this->value, input);
}

auto URLPatternPassword::match(const std::string_view input) const
    -> std::optional<URLPatternComponentResult> {
  return match_single_part(this->value, input);
}

auto URLPatternPort::match(const std::string_view input) const
    -> std::optional<URLPatternComponentResult> {
  return match_single_part(this->value, input);
}

auto URLPatternSearch::match(const std::string_view input) const
    -> std::optional<URLPatternComponentResult> {
  return match_single_part(this->value, input);
}

auto URLPatternHash::match(const std::string_view input) const
    -> std::optional<URLPatternComponentResult> {
  return match_single_part(this->value, input);
}

auto URLPatternHostname::match(const std::string_view input) const
    -> std::optional<URLPatternComponentResult> {
  URLPatternComponentResult result;
  auto input_position = std::string::size_type{0};
  auto pattern_position = std::size_t{0};
  auto just_consumed_delimiter = true;

  while (pattern_position < this->value.size()) {
    const auto &token = this->value[pattern_position];
    const auto is_optional =
        std::holds_alternative<URLPatternPartNameOptional>(token) ||
        std::holds_alternative<URLPatternPartRegExpOptional>(token) ||
        std::holds_alternative<URLPatternPartAsteriskOptional>(token) ||
        std::holds_alternative<URLPatternPartGroupOptional>(token);
    const auto is_multiple =
        std::holds_alternative<URLPatternPartNameMultiple>(token);
    const auto is_regex_multiple =
        std::holds_alternative<URLPatternPartRegExpMultiple>(token);
    const auto is_regex_asterisk =
        std::holds_alternative<URLPatternPartRegExpAsterisk>(token);
    const auto is_asterisk_multiple =
        std::holds_alternative<URLPatternPartAsteriskMultiple>(token);
    const auto is_asterisk_asterisk =
        std::holds_alternative<URLPatternPartAsteriskAsterisk>(token);
    const auto is_name_asterisk =
        std::holds_alternative<URLPatternPartNameAsterisk>(token);
    const auto is_group = std::holds_alternative<URLPatternPartGroup>(token);
    const auto is_group_optional =
        std::holds_alternative<URLPatternPartGroupOptional>(token);
    const auto is_group_multiple =
        std::holds_alternative<URLPatternPartGroupMultiple>(token);
    const auto is_group_asterisk =
        std::holds_alternative<URLPatternPartGroupAsterisk>(token);
    const auto is_complex_segment =
        std::holds_alternative<URLPatternPartComplexSegment>(token);
    const auto is_asterisk =
        std::holds_alternative<URLPatternPartAsterisk>(token);

    // When the pattern is just a single asterisk (default wildcard),
    // it should consume all remaining input
    if (is_asterisk && this->value.size() == 1) {
      if (input_position >= input.size()) {
        pattern_position += 1;
        break;
      }
      const auto remaining_host = input.substr(input_position);
      if (!remaining_host.empty()) {
        result.insert(remaining_host);
      }
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_asterisk_asterisk) {
      if (input_position >= input.size()) {
        pattern_position += 1;
        break;
      }
      const auto remaining_host = input.substr(input_position);
      if (!remaining_host.empty()) {
        result.insert(remaining_host);
      }
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_regex_asterisk) {
      if (input_position >= input.size()) {
        pattern_position += 1;
        break;
      }
      const auto remaining_host = input.substr(input_position);
      if (!remaining_host.empty()) {
        const auto &regex_asterisk_token =
            std::get<URLPatternPartRegExpAsterisk>(token);
        if (!regex_asterisk_token.matches(remaining_host)) {
          return std::nullopt;
        }
        result.insert(remaining_host);
      }
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_asterisk_multiple) {
      if (input_position >= input.size()) {
        return std::nullopt;
      }
      const auto remaining_host = input.substr(input_position);
      if (remaining_host.empty()) {
        return std::nullopt;
      }
      result.insert(remaining_host);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_regex_multiple) {
      if (input_position >= input.size()) {
        return std::nullopt;
      }
      const auto remaining_host = input.substr(input_position);
      if (remaining_host.empty()) {
        return std::nullopt;
      }
      const auto &regex_multiple_token =
          std::get<URLPatternPartRegExpMultiple>(token);
      if (!regex_multiple_token.matches(remaining_host)) {
        return std::nullopt;
      }
      result.insert(remaining_host);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_multiple) {
      if (input_position >= input.size()) {
        return std::nullopt;
      }
      const auto remaining_host = input.substr(input_position);
      if (remaining_host.empty()) {
        return std::nullopt;
      }
      const auto &multiple_token = std::get<URLPatternPartNameMultiple>(token);
      result.insert(multiple_token.value, remaining_host);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_name_asterisk) {
      if (input_position >= input.size()) {
        pattern_position += 1;
        break;
      }
      const auto remaining_host = input.substr(input_position);
      if (!remaining_host.empty()) {
        const auto &asterisk_token =
            std::get<URLPatternPartNameAsterisk>(token);
        result.insert(asterisk_token.value, remaining_host);
      }
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_group || is_group_optional || is_group_multiple ||
        is_group_asterisk) {
      const auto &group_tokens =
          [&token, is_group, is_group_optional,
           is_group_multiple]() -> const URLPatternPartNonGroup & {
        if (is_group) {
          return std::get<URLPatternPartGroup>(token).value;
        } else if (is_group_optional) {
          return std::get<URLPatternPartGroupOptional>(token).value;
        } else if (is_group_multiple) {
          return std::get<URLPatternPartGroupMultiple>(token).value;
        } else {
          return std::get<URLPatternPartGroupAsterisk>(token).value;
        }
      }();

      auto group_match_count = std::size_t{0};
      auto current_input_position = input_position;

      while (true) {

        auto group_input_position = current_input_position;
        auto group_token_index = std::size_t{0};
        auto group_result_positions = std::vector<std::string_view>{};
        auto group_result_names =
            std::unordered_map<std::string_view, std::size_t>{};
        auto group_matched = true;

        while (group_token_index < group_tokens.size()) {
          if (group_input_position >= input.size()) {
            group_matched = false;
            break;
          }

          auto group_segment_start = group_input_position;
          auto temp_group_position = group_input_position;
          while (temp_group_position < input.size() &&
                 input[temp_group_position] != '.') {
            temp_group_position += 1;
          }
          const auto group_segment = input.substr(
              group_segment_start, temp_group_position - group_segment_start);

          const auto group_token_matches = std::visit(
              [&group_segment](const auto &group_token_value) -> bool {
                return group_token_value.matches(group_segment);
              },
              group_tokens[group_token_index]);

          if (!group_token_matches) {
            group_matched = false;
            break;
          }

          const auto needs_capture = std::visit(
              [](const auto &group_token_value) -> bool {
                using T = std::decay_t<decltype(group_token_value)>;
                return std::is_same_v<T, URLPatternPartName> ||
                       std::is_same_v<T, URLPatternPartNameRegExp> ||
                       std::is_same_v<T, URLPatternPartNameOptional>;
              },
              group_tokens[group_token_index]);

          if (needs_capture && !group_segment.empty()) {
            const auto &group_tok = group_tokens[group_token_index];
            const auto &token_name = [&group_tok]() -> const std::string & {
              if (std::holds_alternative<URLPatternPartName>(group_tok)) {
                return std::get<URLPatternPartName>(group_tok).value;
              } else if (std::holds_alternative<URLPatternPartNameRegExp>(
                             group_tok)) {
                return std::get<URLPatternPartNameRegExp>(group_tok).value;
              } else {
                return std::get<URLPatternPartNameOptional>(group_tok).value;
              }
            }();
            const auto position_index =
                result.size() + group_result_positions.size();
            group_result_positions.emplace_back(group_segment);
            group_result_names.emplace(token_name, position_index);
          }

          group_input_position = temp_group_position;
          if (group_input_position < input.size() &&
              input[group_input_position] == '.') {
            group_input_position += 1;
          }
          group_token_index += 1;
        }

        if (group_matched && group_token_index == group_tokens.size()) {
          for (const auto &pos : group_result_positions) {
            result.insert(pos);
          }
          for (const auto &[name, index] : group_result_names) {
            result.insert(name, index);
          }
          current_input_position = group_input_position;
          group_match_count += 1;

          if (is_group || is_group_optional) {
            break;
          }
        } else {
          break;
        }
      }

      if (is_group_multiple && group_match_count == 0) {
        return std::nullopt;
      }
      if (is_group && group_match_count == 0) {
        return std::nullopt;
      }
      input_position = current_input_position;
      pattern_position += 1;
      just_consumed_delimiter =
          (input_position > 0 && input_position <= input.size() &&
           input[input_position - 1] == '.');
      continue;
    }

    if (input_position >= input.size() && !just_consumed_delimiter) {
      if (is_optional) {
        pattern_position += 1;
        continue;
      }
      return std::nullopt;
    }

    just_consumed_delimiter = false;

    auto segment_start = input_position;
    auto temp_input_position = input_position;
    while (temp_input_position < input.size() &&
           input[temp_input_position] != '.') {
      temp_input_position += 1;
    }
    const auto segment =
        input.substr(segment_start, temp_input_position - segment_start);

    if (is_optional && segment.empty()) {
      pattern_position += 1;
      continue;
    }

    if (is_optional && pattern_position + 1 < this->value.size()) {
      const auto &next_token = this->value[pattern_position + 1];
      if (std::holds_alternative<URLPatternPartChar>(next_token)) {
        const auto &char_token = std::get<URLPatternPartChar>(next_token);
        if (char_token.value == segment) {
          pattern_position += 1;
          continue;
        }
      }
    }

    input_position = temp_input_position;

    if (is_complex_segment) {
      const auto &complex_token = std::get<URLPatternPartComplexSegment>(token);

      auto segment_position = std::string_view::size_type{0};
      auto complex_token_index = std::size_t{0};

      while (complex_token_index < complex_token.value.size()) {
        const auto &sub_token = complex_token.value[complex_token_index];

        const auto is_sub_char =
            std::holds_alternative<URLPatternPartChar>(sub_token);
        const auto is_sub_name =
            std::holds_alternative<URLPatternPartName>(sub_token);
        const auto is_sub_asterisk =
            std::holds_alternative<URLPatternPartAsterisk>(sub_token);
        const auto is_sub_group =
            std::holds_alternative<URLPatternPartGroup>(sub_token) ||
            std::holds_alternative<URLPatternPartGroupOptional>(sub_token) ||
            std::holds_alternative<URLPatternPartGroupMultiple>(sub_token) ||
            std::holds_alternative<URLPatternPartGroupAsterisk>(sub_token);

        if (is_sub_char) {
          const auto &char_token = std::get<URLPatternPartChar>(sub_token);
          if (segment_position + char_token.value.size() > segment.size() ||
              segment.substr(segment_position, char_token.value.size()) !=
                  char_token.value) {
            return std::nullopt;
          }
          segment_position += char_token.value.size();
        } else if (is_sub_asterisk) {
          auto asterisk_end = segment_position;
          if (complex_token_index + 1 < complex_token.value.size()) {
            const auto &next_token =
                complex_token.value[complex_token_index + 1];
            if (std::holds_alternative<URLPatternPartChar>(next_token)) {
              const auto &next_char = std::get<URLPatternPartChar>(next_token);
              const auto found_pos =
                  segment.find(next_char.value, segment_position);
              if (found_pos == std::string_view::npos) {
                return std::nullopt;
              }
              asterisk_end = found_pos;
            } else {
              asterisk_end = segment.size();
            }
          } else {
            asterisk_end = segment.size();
          }

          const auto captured_value =
              segment.substr(segment_position, asterisk_end - segment_position);
          result.insert(captured_value);
          segment_position = asterisk_end;
        } else if (is_sub_name) {
          const auto &name_token = std::get<URLPatternPartName>(sub_token);

          auto name_end = segment_position;
          if (complex_token_index + 1 < complex_token.value.size()) {
            const auto &next_token =
                complex_token.value[complex_token_index + 1];
            if (std::holds_alternative<URLPatternPartChar>(next_token)) {
              const auto &next_char = std::get<URLPatternPartChar>(next_token);
              const auto found_pos =
                  segment.find(next_char.value, segment_position);
              if (found_pos == std::string_view::npos) {
                return std::nullopt;
              }
              name_end = found_pos;
            } else if (std::holds_alternative<URLPatternPartGroup>(
                           next_token) ||
                       std::holds_alternative<URLPatternPartGroupOptional>(
                           next_token) ||
                       std::holds_alternative<URLPatternPartGroupMultiple>(
                           next_token) ||
                       std::holds_alternative<URLPatternPartGroupAsterisk>(
                           next_token)) {
              const auto is_next_group_optional =
                  std::holds_alternative<URLPatternPartGroupOptional>(
                      next_token);
              const auto is_next_group_multiple =
                  std::holds_alternative<URLPatternPartGroupMultiple>(
                      next_token);
              const auto is_next_group_asterisk =
                  std::holds_alternative<URLPatternPartGroupAsterisk>(
                      next_token);

              const auto &next_group_tokens =
                  [&next_token, is_next_group_optional, is_next_group_multiple,
                   is_next_group_asterisk]() -> const URLPatternPartNonGroup & {
                if (is_next_group_optional) {
                  return std::get<URLPatternPartGroupOptional>(next_token)
                      .value;
                } else if (is_next_group_multiple) {
                  return std::get<URLPatternPartGroupMultiple>(next_token)
                      .value;
                } else if (is_next_group_asterisk) {
                  return std::get<URLPatternPartGroupAsterisk>(next_token)
                      .value;
                } else {
                  return std::get<URLPatternPartGroup>(next_token).value;
                }
              }();

              if (!next_group_tokens.empty() &&
                  std::holds_alternative<URLPatternPartChar>(
                      next_group_tokens[0])) {
                const auto &group_first_char =
                    std::get<URLPatternPartChar>(next_group_tokens[0]);
                const auto found_pos =
                    segment.find(group_first_char.value, segment_position);
                if (found_pos == std::string_view::npos) {
                  return std::nullopt;
                }
                name_end = found_pos;
              } else {
                name_end = segment.size();
              }
            } else {
              name_end = segment.size();
            }
          } else {
            name_end = segment.size();
          }

          const auto captured_value =
              segment.substr(segment_position, name_end - segment_position);

          if (captured_value.empty()) {
            return std::nullopt;
          }

          result.insert(name_token.value, captured_value);
          segment_position = name_end;
        } else if (is_sub_group) {
          const auto is_sub_group_optional =
              std::holds_alternative<URLPatternPartGroupOptional>(sub_token);
          const auto is_sub_group_multiple =
              std::holds_alternative<URLPatternPartGroupMultiple>(sub_token);
          const auto is_sub_group_asterisk =
              std::holds_alternative<URLPatternPartGroupAsterisk>(sub_token);

          const auto &group_tokens =
              [&sub_token, is_sub_group_optional, is_sub_group_multiple,
               is_sub_group_asterisk]() -> const URLPatternPartNonGroup & {
            if (is_sub_group_optional) {
              return std::get<URLPatternPartGroupOptional>(sub_token).value;
            } else if (is_sub_group_multiple) {
              return std::get<URLPatternPartGroupMultiple>(sub_token).value;
            } else if (is_sub_group_asterisk) {
              return std::get<URLPatternPartGroupAsterisk>(sub_token).value;
            } else {
              return std::get<URLPatternPartGroup>(sub_token).value;
            }
          }();

          auto group_match_count = std::size_t{0};

          while (true) {
            auto group_segment_position = segment_position;
            auto group_matched = true;

            for (const auto &group_token : group_tokens) {
              const auto is_group_char =
                  std::holds_alternative<URLPatternPartChar>(group_token);
              const auto is_group_name =
                  std::holds_alternative<URLPatternPartName>(group_token);

              if (is_group_char) {
                const auto &char_token =
                    std::get<URLPatternPartChar>(group_token);
                if (group_segment_position + char_token.value.size() >
                        segment.size() ||
                    segment.substr(group_segment_position,
                                   char_token.value.size()) !=
                        char_token.value) {
                  group_matched = false;
                  break;
                }
                group_segment_position += char_token.value.size();
              } else if (is_group_name) {
                const auto &name_token =
                    std::get<URLPatternPartName>(group_token);
                auto name_end = group_segment_position;

                if (complex_token_index + 1 < complex_token.value.size()) {
                  const auto &next_token =
                      complex_token.value[complex_token_index + 1];
                  if (std::holds_alternative<URLPatternPartChar>(next_token)) {
                    const auto &next_char =
                        std::get<URLPatternPartChar>(next_token);
                    const auto found_pos =
                        segment.find(next_char.value, group_segment_position);
                    if (found_pos == std::string_view::npos) {
                      group_matched = false;
                      break;
                    }
                    name_end = found_pos;
                  } else {
                    name_end = segment.size();
                  }
                } else {
                  name_end = segment.size();
                }

                const auto captured_value = segment.substr(
                    group_segment_position, name_end - group_segment_position);
                if (captured_value.empty()) {
                  group_matched = false;
                  break;
                }

                result.insert(name_token.value, captured_value);
                group_segment_position = name_end;
              }
            }

            if (group_matched) {
              segment_position = group_segment_position;
              group_match_count += 1;
              if (!is_sub_group_multiple && !is_sub_group_asterisk) {
                break;
              }
            } else {
              break;
            }
          }

          if (is_sub_group_multiple && group_match_count == 0) {
            return std::nullopt;
          }
          if (!is_sub_group_optional && !is_sub_group_multiple &&
              !is_sub_group_asterisk && group_match_count == 0) {
            return std::nullopt;
          }
        }

        complex_token_index += 1;
      }

      if (segment_position != segment.size()) {
        return std::nullopt;
      }
    } else {
      const auto token_matches = std::visit(
          [&segment](const auto &token_value) -> bool {
            return token_value.matches(segment);
          },
          token);

      if (!token_matches) {
        return std::nullopt;
      }

      if (!std::holds_alternative<URLPatternPartChar>(token)) {
        if (std::holds_alternative<URLPatternPartName>(token)) {
          const auto &name_token = std::get<URLPatternPartName>(token);
          result.insert(name_token.value, segment);
        } else if (std::holds_alternative<URLPatternPartNameRegExp>(token)) {
          const auto &name_regex_token =
              std::get<URLPatternPartNameRegExp>(token);
          result.insert(name_regex_token.value, segment);
        } else if (std::holds_alternative<URLPatternPartNameOptional>(token)) {
          const auto &name_optional_token =
              std::get<URLPatternPartNameOptional>(token);
          result.insert(name_optional_token.value, segment);
        } else {
          result.insert(segment);
        }
      }
    }

    if (input_position < input.size()) {
      input_position += 1;
      just_consumed_delimiter = true;
    }
    pattern_position += 1;
  }

  if (input_position < input.size()) {
    return std::nullopt;
  }

  return result;
}

auto URLPatternPathname::match(const std::string_view input) const
    -> std::optional<URLPatternComponentResult> {
  // An empty input matches only if the pattern is a single asterisk (wildcard)
  if (input.empty()) {
    if (this->value.size() == 1 &&
        std::holds_alternative<URLPatternPartAsterisk>(this->value[0])) {
      return URLPatternComponentResult{};
    }
    return std::nullopt;
  }

  if (input[0] != '/') {
    if (!this->is_bare_pattern) {
      return std::nullopt;
    }

    return match_bare_pattern_with_captures(this->value, input);
  }

  if (this->is_bare_pattern) {
    return std::nullopt;
  }

  URLPatternComponentResult result;
  auto input_position = std::string::size_type{1};
  auto pattern_position = std::size_t{0};
  auto just_consumed_delimiter = true;

  while (pattern_position < this->value.size()) {
    const auto &token = this->value[pattern_position];
    const auto is_optional =
        std::holds_alternative<URLPatternPartNameOptional>(token) ||
        std::holds_alternative<URLPatternPartRegExpOptional>(token) ||
        std::holds_alternative<URLPatternPartAsteriskOptional>(token) ||
        std::holds_alternative<URLPatternPartGroupOptional>(token);
    const auto is_multiple =
        std::holds_alternative<URLPatternPartNameMultiple>(token);
    const auto is_regex_multiple =
        std::holds_alternative<URLPatternPartRegExpMultiple>(token);
    const auto is_regex_asterisk =
        std::holds_alternative<URLPatternPartRegExpAsterisk>(token);
    const auto is_asterisk_multiple =
        std::holds_alternative<URLPatternPartAsteriskMultiple>(token);
    const auto is_asterisk_asterisk =
        std::holds_alternative<URLPatternPartAsteriskAsterisk>(token);
    const auto is_name_asterisk =
        std::holds_alternative<URLPatternPartNameAsterisk>(token);
    const auto is_group = std::holds_alternative<URLPatternPartGroup>(token);
    const auto is_group_optional =
        std::holds_alternative<URLPatternPartGroupOptional>(token);
    const auto is_group_multiple =
        std::holds_alternative<URLPatternPartGroupMultiple>(token);
    const auto is_group_asterisk =
        std::holds_alternative<URLPatternPartGroupAsterisk>(token);
    const auto is_complex_segment =
        std::holds_alternative<URLPatternPartComplexSegment>(token);

    if (is_asterisk_asterisk && pattern_position + 1 == this->value.size()) {
      const auto remaining_path = input.substr(input_position);
      const bool should_capture =
          !remaining_path.empty() ||
          (just_consumed_delimiter && pattern_position > 0);
      if (should_capture) {
        result.insert(remaining_path);
        input_position = input.size();
      }
      pattern_position += 1;
      break;
    }

    if (is_regex_asterisk && pattern_position + 1 == this->value.size()) {
      const auto remaining_path = input.substr(input_position);
      const bool should_capture =
          !remaining_path.empty() ||
          (just_consumed_delimiter && pattern_position > 0);
      if (should_capture) {
        const auto &regex_asterisk_token =
            std::get<URLPatternPartRegExpAsterisk>(token);
        if (!regex_asterisk_token.matches(remaining_path)) {
          return std::nullopt;
        }
        result.insert(remaining_path);
        input_position = input.size();
      }
      pattern_position += 1;
      break;
    }

    if (is_asterisk_multiple && pattern_position + 1 == this->value.size()) {
      const auto remaining_path = input.substr(input_position);
      const bool should_capture =
          !remaining_path.empty() ||
          (just_consumed_delimiter && pattern_position > 0);
      if (!should_capture) {
        return std::nullopt;
      }
      result.insert(remaining_path);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_regex_multiple && pattern_position + 1 == this->value.size()) {
      const auto remaining_path = input.substr(input_position);
      const bool should_capture =
          !remaining_path.empty() ||
          (just_consumed_delimiter && pattern_position > 0);
      if (!should_capture) {
        return std::nullopt;
      }
      const auto &regex_multiple_token =
          std::get<URLPatternPartRegExpMultiple>(token);
      if (!regex_multiple_token.matches(remaining_path)) {
        return std::nullopt;
      }
      result.insert(remaining_path);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_multiple) {
      if (input_position >= input.size()) {
        return std::nullopt;
      }
      const auto remaining_path = input.substr(input_position);
      if (remaining_path.empty()) {
        return std::nullopt;
      }
      const auto &multiple_token = std::get<URLPatternPartNameMultiple>(token);
      result.insert(multiple_token.value, remaining_path);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_name_asterisk) {
      if (input_position >= input.size()) {
        if (just_consumed_delimiter && pattern_position > 0) {
          return std::nullopt;
        }
        pattern_position += 1;
        break;
      }
      const auto remaining_path = input.substr(input_position);
      if (!remaining_path.empty()) {
        const auto &asterisk_token =
            std::get<URLPatternPartNameAsterisk>(token);
        result.insert(asterisk_token.value, remaining_path);
      }
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    const auto is_plain_regex =
        std::holds_alternative<URLPatternPartRegExp>(token);
    const auto is_plain_asterisk =
        std::holds_alternative<URLPatternPartAsterisk>(token);
    const auto is_optional_regex =
        std::holds_alternative<URLPatternPartRegExpOptional>(token);
    const auto is_optional_asterisk =
        std::holds_alternative<URLPatternPartAsteriskOptional>(token);
    const auto is_named_regex =
        std::holds_alternative<URLPatternPartNameRegExp>(token);

    if (is_plain_regex && pattern_position + 1 == this->value.size()) {
      if (input_position >= input.size() && !just_consumed_delimiter) {
        return std::nullopt;
      }
      const auto remaining_path = input.substr(input_position);
      const auto &regex_token = std::get<URLPatternPartRegExp>(token);
      if (!regex_token.matches(remaining_path)) {
        return std::nullopt;
      }
      result.insert(remaining_path);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_named_regex && pattern_position + 1 == this->value.size()) {
      if (input_position >= input.size() && !just_consumed_delimiter) {
        return std::nullopt;
      }
      const auto remaining_path = input.substr(input_position);
      const auto &regex_token = std::get<URLPatternPartNameRegExp>(token);
      if (!regex_token.matches(remaining_path)) {
        return std::nullopt;
      }
      result.insert(regex_token.value, remaining_path);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_plain_asterisk && pattern_position + 1 == this->value.size()) {
      if (input_position >= input.size() && !just_consumed_delimiter) {
        return std::nullopt;
      }
      const auto remaining_path = input.substr(input_position);
      result.insert(remaining_path);
      input_position = input.size();
      pattern_position += 1;
      break;
    }

    if (is_optional_regex && pattern_position + 1 == this->value.size()) {
      const auto remaining_path = input.substr(input_position);
      const bool should_capture =
          !remaining_path.empty() ||
          (just_consumed_delimiter && pattern_position > 0);
      if (should_capture) {
        const auto &regex_token = std::get<URLPatternPartRegExpOptional>(token);
        if (!regex_token.matches(remaining_path)) {
          return std::nullopt;
        }
        result.insert(remaining_path);
        input_position = input.size();
      }
      pattern_position += 1;
      break;
    }

    if (is_optional_asterisk && pattern_position + 1 == this->value.size()) {
      const auto remaining_path = input.substr(input_position);
      const bool should_capture =
          !remaining_path.empty() ||
          (just_consumed_delimiter && pattern_position > 0);
      if (should_capture) {
        result.insert(remaining_path);
        input_position = input.size();
      }
      pattern_position += 1;
      break;
    }

    if (is_group || is_group_optional || is_group_multiple ||
        is_group_asterisk) {
      const auto &group_tokens =
          [&token, is_group, is_group_optional,
           is_group_multiple]() -> const URLPatternPartNonGroup & {
        if (is_group) {
          return std::get<URLPatternPartGroup>(token).value;
        } else if (is_group_optional) {
          return std::get<URLPatternPartGroupOptional>(token).value;
        } else if (is_group_multiple) {
          return std::get<URLPatternPartGroupMultiple>(token).value;
        } else {
          return std::get<URLPatternPartGroupAsterisk>(token).value;
        }
      }();

      const auto group_has_prefix = get_inner_segment_prefix(token);

      auto group_match_count = std::size_t{0};
      auto current_input_position = input_position;

      while (true) {
        auto group_input_position = current_input_position;

        if (group_has_prefix) {
          if (group_input_position >= input.size() ||
              input[group_input_position] != '/') {
            break;
          }
          group_input_position += 1;
        }

        auto group_token_index = std::size_t{0};
        auto group_result_positions = std::vector<std::string_view>{};
        auto group_result_names =
            std::unordered_map<std::string_view, std::size_t>{};
        auto group_matched = true;

        while (group_token_index < group_tokens.size()) {
          if (group_input_position >= input.size()) {
            group_matched = false;
            break;
          }

          auto group_segment_start = group_input_position;
          auto temp_group_position = group_input_position;
          while (temp_group_position < input.size() &&
                 input[temp_group_position] != '/') {
            temp_group_position += 1;
          }
          const auto group_segment = input.substr(
              group_segment_start, temp_group_position - group_segment_start);

          const auto group_token_matches = std::visit(
              [&group_segment](const auto &group_token_value) -> bool {
                return group_token_value.matches(group_segment);
              },
              group_tokens[group_token_index]);

          if (!group_token_matches) {
            group_matched = false;
            break;
          }

          const auto needs_capture = std::visit(
              [](const auto &group_token_value) -> bool {
                using T = std::decay_t<decltype(group_token_value)>;
                return std::is_same_v<T, URLPatternPartName> ||
                       std::is_same_v<T, URLPatternPartNameRegExp> ||
                       std::is_same_v<T, URLPatternPartNameOptional>;
              },
              group_tokens[group_token_index]);

          if (needs_capture && !group_segment.empty()) {
            const auto &group_tok = group_tokens[group_token_index];
            const auto &token_name = [&group_tok]() -> const std::string & {
              if (std::holds_alternative<URLPatternPartName>(group_tok)) {
                return std::get<URLPatternPartName>(group_tok).value;
              } else if (std::holds_alternative<URLPatternPartNameRegExp>(
                             group_tok)) {
                return std::get<URLPatternPartNameRegExp>(group_tok).value;
              } else {
                return std::get<URLPatternPartNameOptional>(group_tok).value;
              }
            }();
            const auto position_index =
                result.size() + group_result_positions.size();
            group_result_positions.emplace_back(group_segment);
            group_result_names.emplace(token_name, position_index);
          }

          group_input_position = temp_group_position;
          if (group_input_position < input.size() &&
              input[group_input_position] == '/') {
            group_input_position += 1;
          }
          group_token_index += 1;
        }

        if (group_matched && group_token_index == group_tokens.size()) {
          for (const auto &pos : group_result_positions) {
            result.insert(pos);
          }
          for (const auto &[name, index] : group_result_names) {
            result.insert(name, index);
          }
          if (group_has_prefix && group_input_position > 0 &&
              group_input_position <= input.size() &&
              input[group_input_position - 1] == '/') {
            current_input_position = group_input_position - 1;
          } else {
            current_input_position = group_input_position;
          }
          group_match_count += 1;

          if (is_group || is_group_optional) {
            break;
          }
        } else {
          break;
        }
      }

      if (is_group_multiple && group_match_count == 0) {
        return std::nullopt;
      }
      if (is_group && group_match_count == 0) {
        return std::nullopt;
      }
      input_position = current_input_position;
      pattern_position += 1;
      just_consumed_delimiter =
          (input_position > 0 && input_position <= input.size() &&
           input[input_position - 1] == '/');
      continue;
    }

    if (input_position >= input.size() && !just_consumed_delimiter) {
      if (is_optional) {
        pattern_position += 1;
        continue;
      }
      return std::nullopt;
    }

    if (input_position >= input.size() && just_consumed_delimiter &&
        pattern_position > 0) {
      const auto is_name_based =
          std::holds_alternative<URLPatternPartName>(token) ||
          std::holds_alternative<URLPatternPartNameOptional>(token) ||
          std::holds_alternative<URLPatternPartNameMultiple>(token) ||
          std::holds_alternative<URLPatternPartNameAsterisk>(token) ||
          std::holds_alternative<URLPatternPartNameRegExp>(token);
      if (is_name_based) {
        return std::nullopt;
      }
    }

    just_consumed_delimiter = false;

    auto segment_start = input_position;
    auto temp_input_position = input_position;
    while (temp_input_position < input.size() &&
           input[temp_input_position] != '/') {
      temp_input_position += 1;
    }
    const auto segment =
        input.substr(segment_start, temp_input_position - segment_start);

    if (is_optional && segment.empty()) {
      pattern_position += 1;
      continue;
    }

    if (is_optional && pattern_position + 1 < this->value.size()) {
      const auto &next_token = this->value[pattern_position + 1];
      if (std::holds_alternative<URLPatternPartChar>(next_token)) {
        const auto &char_token = std::get<URLPatternPartChar>(next_token);
        if (char_token.value == segment) {
          pattern_position += 1;
          continue;
        }
      }
    }

    input_position = temp_input_position;

    if (is_complex_segment) {
      const auto &complex_token = std::get<URLPatternPartComplexSegment>(token);

      auto segment_position = std::string_view::size_type{0};
      auto complex_token_index = std::size_t{0};

      while (complex_token_index < complex_token.value.size()) {
        const auto &sub_token = complex_token.value[complex_token_index];

        const auto is_sub_char =
            std::holds_alternative<URLPatternPartChar>(sub_token);
        const auto is_sub_name =
            std::holds_alternative<URLPatternPartName>(sub_token);
        const auto is_sub_group =
            std::holds_alternative<URLPatternPartGroup>(sub_token) ||
            std::holds_alternative<URLPatternPartGroupOptional>(sub_token) ||
            std::holds_alternative<URLPatternPartGroupMultiple>(sub_token) ||
            std::holds_alternative<URLPatternPartGroupAsterisk>(sub_token);

        if (is_sub_char) {
          const auto &char_token = std::get<URLPatternPartChar>(sub_token);
          if (segment_position + char_token.value.size() > segment.size() ||
              segment.substr(segment_position, char_token.value.size()) !=
                  char_token.value) {
            return std::nullopt;
          }
          segment_position += char_token.value.size();
        } else if (is_sub_name) {
          const auto &name_token = std::get<URLPatternPartName>(sub_token);

          auto name_end = segment_position;
          if (complex_token_index + 1 < complex_token.value.size()) {
            const auto &next_token =
                complex_token.value[complex_token_index + 1];
            if (std::holds_alternative<URLPatternPartChar>(next_token)) {
              const auto &next_char = std::get<URLPatternPartChar>(next_token);
              const auto found_pos =
                  segment.find(next_char.value, segment_position);
              if (found_pos == std::string_view::npos) {
                return std::nullopt;
              }
              name_end = found_pos;
            } else if (std::holds_alternative<URLPatternPartGroup>(
                           next_token) ||
                       std::holds_alternative<URLPatternPartGroupOptional>(
                           next_token) ||
                       std::holds_alternative<URLPatternPartGroupMultiple>(
                           next_token) ||
                       std::holds_alternative<URLPatternPartGroupAsterisk>(
                           next_token)) {
              const auto is_next_group_optional =
                  std::holds_alternative<URLPatternPartGroupOptional>(
                      next_token);
              const auto is_next_group_multiple =
                  std::holds_alternative<URLPatternPartGroupMultiple>(
                      next_token);
              const auto is_next_group_asterisk =
                  std::holds_alternative<URLPatternPartGroupAsterisk>(
                      next_token);

              const auto &next_group_tokens =
                  [&next_token, is_next_group_optional, is_next_group_multiple,
                   is_next_group_asterisk]() -> const URLPatternPartNonGroup & {
                if (is_next_group_optional) {
                  return std::get<URLPatternPartGroupOptional>(next_token)
                      .value;
                } else if (is_next_group_multiple) {
                  return std::get<URLPatternPartGroupMultiple>(next_token)
                      .value;
                } else if (is_next_group_asterisk) {
                  return std::get<URLPatternPartGroupAsterisk>(next_token)
                      .value;
                } else {
                  return std::get<URLPatternPartGroup>(next_token).value;
                }
              }();

              if (!next_group_tokens.empty() &&
                  std::holds_alternative<URLPatternPartChar>(
                      next_group_tokens[0])) {
                const auto &group_first_char =
                    std::get<URLPatternPartChar>(next_group_tokens[0]);
                const auto found_pos =
                    segment.find(group_first_char.value, segment_position);
                if (found_pos == std::string_view::npos) {
                  return std::nullopt;
                }
                name_end = found_pos;
              } else {
                name_end = segment.size();
              }
            } else {
              name_end = segment.size();
            }
          } else {
            name_end = segment.size();
          }

          const auto captured_value =
              segment.substr(segment_position, name_end - segment_position);

          if (captured_value.empty()) {
            return std::nullopt;
          }

          result.insert(name_token.value, captured_value);
          segment_position = name_end;
        } else if (is_sub_group) {
          const auto is_sub_group_optional =
              std::holds_alternative<URLPatternPartGroupOptional>(sub_token);
          const auto is_sub_group_multiple =
              std::holds_alternative<URLPatternPartGroupMultiple>(sub_token);
          const auto is_sub_group_asterisk =
              std::holds_alternative<URLPatternPartGroupAsterisk>(sub_token);

          const auto &group_tokens =
              [&sub_token, is_sub_group_optional, is_sub_group_multiple,
               is_sub_group_asterisk]() -> const URLPatternPartNonGroup & {
            if (is_sub_group_optional) {
              return std::get<URLPatternPartGroupOptional>(sub_token).value;
            } else if (is_sub_group_multiple) {
              return std::get<URLPatternPartGroupMultiple>(sub_token).value;
            } else if (is_sub_group_asterisk) {
              return std::get<URLPatternPartGroupAsterisk>(sub_token).value;
            } else {
              return std::get<URLPatternPartGroup>(sub_token).value;
            }
          }();

          auto group_match_count = std::size_t{0};

          while (true) {
            auto group_segment_position = segment_position;
            auto group_matched = true;

            for (const auto &group_token : group_tokens) {
              const auto is_group_char =
                  std::holds_alternative<URLPatternPartChar>(group_token);
              const auto is_group_name =
                  std::holds_alternative<URLPatternPartName>(group_token);

              if (is_group_char) {
                const auto &char_token =
                    std::get<URLPatternPartChar>(group_token);
                if (group_segment_position + char_token.value.size() >
                        segment.size() ||
                    segment.substr(group_segment_position,
                                   char_token.value.size()) !=
                        char_token.value) {
                  group_matched = false;
                  break;
                }
                group_segment_position += char_token.value.size();
              } else if (is_group_name) {
                const auto &name_token =
                    std::get<URLPatternPartName>(group_token);
                auto name_end = group_segment_position;

                if (complex_token_index + 1 < complex_token.value.size()) {
                  const auto &next_token =
                      complex_token.value[complex_token_index + 1];
                  if (std::holds_alternative<URLPatternPartChar>(next_token)) {
                    const auto &next_char =
                        std::get<URLPatternPartChar>(next_token);
                    const auto found_pos =
                        segment.find(next_char.value, group_segment_position);
                    if (found_pos == std::string_view::npos) {
                      group_matched = false;
                      break;
                    }
                    name_end = found_pos;
                  } else {
                    name_end = segment.size();
                  }
                } else {
                  name_end = segment.size();
                }

                const auto captured_value = segment.substr(
                    group_segment_position, name_end - group_segment_position);
                if (captured_value.empty()) {
                  group_matched = false;
                  break;
                }

                result.insert(name_token.value, captured_value);
                group_segment_position = name_end;
              }
            }

            if (group_matched) {
              segment_position = group_segment_position;
              group_match_count += 1;
              if (!is_sub_group_multiple && !is_sub_group_asterisk) {
                break;
              }
            } else {
              break;
            }
          }

          if (is_sub_group_multiple && group_match_count == 0) {
            return std::nullopt;
          }
          if (!is_sub_group_optional && !is_sub_group_multiple &&
              !is_sub_group_asterisk && group_match_count == 0) {
            return std::nullopt;
          }
        }

        complex_token_index += 1;
      }

      if (segment_position != segment.size()) {
        return std::nullopt;
      }
    } else {
      const auto token_matches = std::visit(
          [&segment](const auto &token_value) -> bool {
            return token_value.matches(segment);
          },
          token);

      if (!token_matches) {
        return std::nullopt;
      }

      if (!std::holds_alternative<URLPatternPartChar>(token)) {
        if (std::holds_alternative<URLPatternPartName>(token)) {
          const auto &name_token = std::get<URLPatternPartName>(token);
          result.insert(name_token.value, segment);
        } else if (std::holds_alternative<URLPatternPartNameRegExp>(token)) {
          const auto &name_regex_token =
              std::get<URLPatternPartNameRegExp>(token);
          result.insert(name_regex_token.value, segment);
        } else if (std::holds_alternative<URLPatternPartNameOptional>(token)) {
          const auto &name_optional_token =
              std::get<URLPatternPartNameOptional>(token);
          result.insert(name_optional_token.value, segment);
        } else {
          result.insert(segment);
        }
      }
    }

    if (input_position < input.size() &&
        pattern_position + 1 < this->value.size()) {
      const auto &next_token = this->value[pattern_position + 1];
      const auto next_has_prefix = get_inner_segment_prefix(next_token);
      if (!next_has_prefix) {
        input_position += 1;
        just_consumed_delimiter = true;
      }
    }
    pattern_position += 1;
  }

  if (input_position < input.size()) {
    return std::nullopt;
  }

  return result;
}

URLPatternProtocol::URLPatternProtocol(const char *input)
    : value{parse_single_token(input)} {}

URLPatternUsername::URLPatternUsername(const char *input)
    : value{parse_single_token(input)} {}

URLPatternPassword::URLPatternPassword(const char *input)
    : value{parse_single_token(input)} {}

URLPatternPort::URLPatternPort(const char *input)
    : value{parse_single_token(input)} {}

URLPatternSearch::URLPatternSearch(const char *input)
    : value{parse_single_token(input)} {}

URLPatternHash::URLPatternHash(const char *input)
    : value{parse_single_token(input)} {}

URLPatternPathname::URLPatternPathname(const char *input) {
  const std::string_view input_view{input};
  if (input_view.empty()) {
    throw URLPatternParseError{0};
  }

  const auto starts_with_slash = input_view[0] == '/';
  if (!starts_with_slash) {
    this->is_bare_pattern = true;
  }

  enum class State : std::uint8_t {
    ReadingChar,
    ReadingName,
    ReadingRegExp,
    AfterBackslash,
    ReadingGroup
  };

  auto position =
      starts_with_slash ? std::string::size_type{1} : std::string::size_type{0};
  auto current_state = State::ReadingChar;
  std::string token_buffer;
  std::string name_for_regex;
  auto after_segment_delimiter = false;
  URLPatternPartGroup current_group;
  auto group_after_segment_delimiter = false;
  auto group_has_delimiter = false;
  auto group_inner_segment_prefix = false;
  auto group_inner_segment_suffix = false;
  auto inside_group = false;
  std::vector<std::variant<
      URLPatternPartRegExp, URLPatternPartRegExpOptional,
      URLPatternPartRegExpMultiple, URLPatternPartRegExpAsterisk,
      URLPatternPartName, URLPatternPartNameRegExp, URLPatternPartNameOptional,
      URLPatternPartNameMultiple, URLPatternPartNameAsterisk,
      URLPatternPartChar, URLPatternPartAsterisk,
      URLPatternPartAsteriskOptional, URLPatternPartAsteriskMultiple,
      URLPatternPartAsteriskAsterisk, URLPatternPartGroup,
      URLPatternPartGroupOptional, URLPatternPartGroupMultiple,
      URLPatternPartGroupAsterisk>>
      segment_tokens;

  const auto flush_segment_tokens = [&]() -> void {
    if (!token_buffer.empty()) {
      segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
      token_buffer.clear();
    }
    if (segment_tokens.size() == 0 && after_segment_delimiter) {
      this->value.emplace_back(URLPatternPartChar{""});
    } else if (segment_tokens.size() == 1) {
      std::visit([&](const auto &token) { this->value.emplace_back(token); },
                 segment_tokens[0]);
    } else if (segment_tokens.size() > 1) {
      this->value.emplace_back(URLPatternPartComplexSegment{segment_tokens});
    }
    segment_tokens.clear();
  };

  while (position < input_view.size()) {
    const auto current = input_view[position];

    if (current_state == State::ReadingChar) {
      if (current == '/') {
        flush_segment_tokens();
        after_segment_delimiter = true;
        position += 1;
      } else if (current == ':') {
        if (!token_buffer.empty()) {
          segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingName;
        after_segment_delimiter = false;
        position += 1;
      } else if (current == '*') {
        if (!token_buffer.empty()) {
          segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        const auto is_asterisk_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        if (is_asterisk_asterisk) {
          segment_tokens.emplace_back(URLPatternPartAsteriskAsterisk{});
          position += 2;
        } else if (is_optional) {
          segment_tokens.emplace_back(URLPatternPartAsteriskOptional{});
          position += 2;
        } else if (is_multiple) {
          segment_tokens.emplace_back(URLPatternPartAsteriskMultiple{});
          position += 2;
        } else {
          segment_tokens.emplace_back(URLPatternPartAsterisk{});
          position += 1;
        }
        after_segment_delimiter = false;
      } else if (current == '(') {
        if (!token_buffer.empty()) {
          segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingRegExp;
        after_segment_delimiter = false;
        position += 1;
      } else if (current == '{') {
        if (!token_buffer.empty()) {
          segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_group = URLPatternPartGroup{};
        current_state = State::ReadingGroup;
        group_after_segment_delimiter = false;
        group_has_delimiter = false;
        group_inner_segment_prefix = false;
        group_inner_segment_suffix = false;
        after_segment_delimiter = false;
        inside_group = true;
        position += 1;
      } else if (current == '\\') {
        current_state = State::AfterBackslash;
        after_segment_delimiter = false;
        position += 1;
      } else {
        token_buffer += current;
        after_segment_delimiter = false;
        position += 1;
      }
    } else if (current_state == State::ReadingName) {
      if (token_buffer.empty()) {
        if (!is_valid_name_start(current)) {
          throw URLPatternParseError{position};
        }
        token_buffer += current;
        position += 1;
      } else {
        if (current == '/') {
          if (inside_group) {
            current_group.value.emplace_back(URLPatternPartName{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingGroup;
            group_after_segment_delimiter = true;
            position += 1;
          } else {
            segment_tokens.emplace_back(URLPatternPartName{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingChar;
            after_segment_delimiter = false;
          }
        } else if (current == '}' && inside_group) {
          current_group.value.emplace_back(URLPatternPartName{token_buffer});
          token_buffer.clear();
          const auto is_optional = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '?';
          const auto is_multiple = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '+';
          const auto is_asterisk = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '*';
          const auto has_delimiter = group_has_delimiter;
          group_inner_segment_suffix = group_after_segment_delimiter;
          if (has_delimiter) {
            flush_segment_tokens();
          }
          if (is_optional) {
            segment_tokens.emplace_back(URLPatternPartGroupOptional{
                .value = current_group.value,
                .has_inner_segment_prefix = group_inner_segment_prefix,
                .has_inner_segment_suffix = group_inner_segment_suffix});
            position += 2;
          } else if (is_multiple) {
            segment_tokens.emplace_back(URLPatternPartGroupMultiple{
                .value = current_group.value,
                .has_inner_segment_prefix = group_inner_segment_prefix,
                .has_inner_segment_suffix = group_inner_segment_suffix});
            position += 2;
          } else if (is_asterisk) {
            segment_tokens.emplace_back(URLPatternPartGroupAsterisk{
                .value = current_group.value,
                .has_inner_segment_prefix = group_inner_segment_prefix,
                .has_inner_segment_suffix = group_inner_segment_suffix});
            position += 2;
          } else {
            current_group.has_inner_segment_prefix = group_inner_segment_prefix;
            current_group.has_inner_segment_suffix = group_inner_segment_suffix;
            segment_tokens.emplace_back(current_group);
            position += 1;
          }
          if (has_delimiter) {
            flush_segment_tokens();
          }
          current_state = State::ReadingChar;
          after_segment_delimiter = false;
          inside_group = false;
          group_after_segment_delimiter = false;
          group_has_delimiter = false;
        } else if (current == '(') {
          name_for_regex = token_buffer;
          token_buffer.clear();
          current_state = State::ReadingRegExp;
          after_segment_delimiter = false;
          position += 1;
        } else if (current == '*') {
          if (inside_group) {
            current_group.value.emplace_back(
                URLPatternPartNameAsterisk{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingGroup;
            group_after_segment_delimiter = false;
          } else {
            segment_tokens.emplace_back(
                URLPatternPartNameAsterisk{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingChar;
            after_segment_delimiter = false;
          }
          position += 1;
        } else if (current == '?') {
          if (inside_group) {
            current_group.value.emplace_back(
                URLPatternPartNameOptional{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingGroup;
            group_after_segment_delimiter = false;
          } else {
            segment_tokens.emplace_back(
                URLPatternPartNameOptional{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingChar;
            after_segment_delimiter = false;
          }
          position += 1;
        } else if (current == '+') {
          if (inside_group) {
            current_group.value.emplace_back(
                URLPatternPartNameMultiple{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingGroup;
            group_after_segment_delimiter = false;
          } else {
            segment_tokens.emplace_back(
                URLPatternPartNameMultiple{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingChar;
            after_segment_delimiter = false;
          }
          position += 1;
        } else if (is_valid_name_char(current)) {
          token_buffer += current;
          position += 1;
        } else if (current == '\\') {
          // End name and process escape sequence
          if (inside_group) {
            current_group.value.emplace_back(URLPatternPartName{token_buffer});
            token_buffer.clear();
            current_state = State::AfterBackslash;
            group_after_segment_delimiter = false;
          } else {
            segment_tokens.emplace_back(URLPatternPartName{token_buffer});
            token_buffer.clear();
            current_state = State::AfterBackslash;
            after_segment_delimiter = false;
          }
          position += 1;
        } else {
          if (inside_group) {
            throw URLPatternParseError{position};
          } else {
            const auto can_start_new_token = current == '.' || current == ':' ||
                                             current == '{' || current == '(' ||
                                             current == '*';
            if (can_start_new_token) {
              segment_tokens.emplace_back(URLPatternPartName{token_buffer});
              token_buffer.clear();
              current_state = State::ReadingChar;
              after_segment_delimiter = false;
            } else {
              throw URLPatternParseError{position};
            }
          }
        }
      }
    } else if (current_state == State::ReadingRegExp) {
      if (current == ')') {
        if (token_buffer.empty()) {
          throw URLPatternParseError{position};
        }
        if (!is_ascii_string(token_buffer)) {
          throw URLPatternParseError{position};
        }
        const auto regex = to_regex(token_buffer);
        if (!regex.has_value()) {
          throw URLPatternParseError{position};
        }
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        const auto is_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        if (name_for_regex.empty()) {
          if (inside_group) {
            if (is_optional) {
              current_group.value.emplace_back(URLPatternPartRegExpOptional{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else if (is_multiple) {
              current_group.value.emplace_back(URLPatternPartRegExpMultiple{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else if (is_asterisk) {
              current_group.value.emplace_back(URLPatternPartRegExpAsterisk{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else {
              current_group.value.emplace_back(URLPatternPartRegExp{
                  .value = regex.value(), .original_pattern = token_buffer});
            }
          } else {
            if (is_optional) {
              segment_tokens.emplace_back(URLPatternPartRegExpOptional{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else if (is_multiple) {
              segment_tokens.emplace_back(URLPatternPartRegExpMultiple{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else if (is_asterisk) {
              segment_tokens.emplace_back(URLPatternPartRegExpAsterisk{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else {
              segment_tokens.emplace_back(URLPatternPartRegExp{
                  .value = regex.value(), .original_pattern = token_buffer});
            }
          }
        } else {
          if (inside_group) {
            current_group.value.emplace_back(
                URLPatternPartNameRegExp{.value = name_for_regex,
                                         .modifier = regex.value(),
                                         .original_pattern = token_buffer});
          } else {
            segment_tokens.emplace_back(
                URLPatternPartNameRegExp{.value = name_for_regex,
                                         .modifier = regex.value(),
                                         .original_pattern = token_buffer});
          }
          name_for_regex.clear();
        }
        token_buffer.clear();
        current_state = inside_group ? State::ReadingGroup : State::ReadingChar;
        if (inside_group) {
          group_after_segment_delimiter = false;
        } else {
          after_segment_delimiter = false;
        }
        position += (is_optional || is_multiple || is_asterisk) ? 2 : 1;
      } else {
        token_buffer += current;
        position += 1;
      }
    } else if (current_state == State::AfterBackslash) {
      if (position >= input_view.size()) {
        throw URLPatternParseError{position - 1};
      }
      token_buffer += current;
      current_state = inside_group ? State::ReadingGroup : State::ReadingChar;
      position += 1;
    } else if (current_state == State::ReadingGroup) {
      if (current == '}') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        } else if (group_after_segment_delimiter) {
          current_group.value.emplace_back(URLPatternPartChar{""});
        }
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        const auto is_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        const auto has_delimiter = group_has_delimiter;
        group_inner_segment_suffix = group_after_segment_delimiter;
        if (has_delimiter) {
          flush_segment_tokens();
        }
        if (is_optional) {
          segment_tokens.emplace_back(URLPatternPartGroupOptional{
              .value = current_group.value,
              .has_inner_segment_prefix = group_inner_segment_prefix,
              .has_inner_segment_suffix = group_inner_segment_suffix});
          position += 2;
        } else if (is_multiple) {
          segment_tokens.emplace_back(URLPatternPartGroupMultiple{
              .value = current_group.value,
              .has_inner_segment_prefix = group_inner_segment_prefix,
              .has_inner_segment_suffix = group_inner_segment_suffix});
          position += 2;
        } else if (is_asterisk) {
          segment_tokens.emplace_back(URLPatternPartGroupAsterisk{
              .value = current_group.value,
              .has_inner_segment_prefix = group_inner_segment_prefix,
              .has_inner_segment_suffix = group_inner_segment_suffix});
          position += 2;
        } else {
          current_group.has_inner_segment_prefix = group_inner_segment_prefix;
          current_group.has_inner_segment_suffix = group_inner_segment_suffix;
          segment_tokens.emplace_back(current_group);
          position += 1;
        }
        if (has_delimiter) {
          flush_segment_tokens();
        }
        current_state = State::ReadingChar;
        after_segment_delimiter = false;
        inside_group = false;
        group_after_segment_delimiter = false;
        group_has_delimiter = false;
      } else if (current == '/') {
        if (current_group.value.empty() && token_buffer.empty()) {
          group_inner_segment_prefix = true;
        }
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        } else if (group_after_segment_delimiter) {
          current_group.value.emplace_back(URLPatternPartChar{""});
        }
        group_after_segment_delimiter = true;
        group_has_delimiter = true;
        position += 1;
      } else if (current == ':') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingName;
        group_after_segment_delimiter = false;
        position += 1;
      } else if (current == '*') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        const auto is_asterisk_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        if (is_asterisk_asterisk) {
          current_group.value.emplace_back(URLPatternPartAsteriskAsterisk{});
          position += 2;
        } else if (is_optional) {
          current_group.value.emplace_back(URLPatternPartAsteriskOptional{});
          position += 2;
        } else if (is_multiple) {
          current_group.value.emplace_back(URLPatternPartAsteriskMultiple{});
          position += 2;
        } else {
          current_group.value.emplace_back(URLPatternPartAsterisk{});
          position += 1;
        }
        group_after_segment_delimiter = false;
      } else if (current == '(') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingRegExp;
        group_after_segment_delimiter = false;
        position += 1;
      } else if (current == '\\') {
        current_state = State::AfterBackslash;
        group_after_segment_delimiter = false;
        position += 1;
      } else {
        token_buffer += current;
        group_after_segment_delimiter = false;
        position += 1;
      }
    }
  }

  if (current_state == State::AfterBackslash) {
    throw URLPatternParseError{position - 1};
  }

  if (current_state == State::ReadingRegExp) {
    throw URLPatternParseError{position};
  }

  if (current_state == State::ReadingName && token_buffer.empty()) {
    throw URLPatternParseError{position};
  }

  if (!token_buffer.empty()) {
    if (current_state == State::ReadingName) {
      segment_tokens.emplace_back(URLPatternPartName{token_buffer});
      token_buffer.clear();
    }
  }

  flush_segment_tokens();
}

URLPatternHostname::URLPatternHostname(const char *input) {
  const std::string_view input_view{input};

  enum class State : std::uint8_t {
    ReadingChar,
    ReadingName,
    ReadingRegExp,
    AfterBackslash,
    ReadingGroup
  };

  auto position = std::string::size_type{0};
  auto current_state = State::ReadingChar;
  std::string token_buffer;
  std::string name_for_regex;
  auto after_segment_delimiter = true;
  URLPatternPartGroup current_group;
  auto group_after_segment_delimiter = false;
  auto group_has_delimiter = false;
  auto group_inner_segment_prefix = false;
  auto group_inner_segment_suffix = false;
  auto inside_group = false;
  std::vector<std::variant<
      URLPatternPartRegExp, URLPatternPartRegExpOptional,
      URLPatternPartRegExpMultiple, URLPatternPartRegExpAsterisk,
      URLPatternPartName, URLPatternPartNameRegExp, URLPatternPartNameOptional,
      URLPatternPartNameMultiple, URLPatternPartNameAsterisk,
      URLPatternPartChar, URLPatternPartAsterisk,
      URLPatternPartAsteriskOptional, URLPatternPartAsteriskMultiple,
      URLPatternPartAsteriskAsterisk, URLPatternPartGroup,
      URLPatternPartGroupOptional, URLPatternPartGroupMultiple,
      URLPatternPartGroupAsterisk>>
      segment_tokens;

  const auto flush_segment_tokens = [&]() -> void {
    if (!token_buffer.empty()) {
      segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
      token_buffer.clear();
    }
    if (segment_tokens.size() == 0 && after_segment_delimiter) {
      this->value.emplace_back(URLPatternPartChar{""});
    } else if (segment_tokens.size() == 1) {
      std::visit([&](const auto &token) { this->value.emplace_back(token); },
                 segment_tokens[0]);
    } else if (segment_tokens.size() > 1) {
      this->value.emplace_back(URLPatternPartComplexSegment{segment_tokens});
    }
    segment_tokens.clear();
  };

  while (position < input_view.size()) {
    const auto current = input_view[position];

    if (current_state == State::ReadingChar) {
      if (current == '.') {
        flush_segment_tokens();
        after_segment_delimiter = true;
        position += 1;
      } else if (current == ':') {
        if (!token_buffer.empty()) {
          segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingName;
        after_segment_delimiter = false;
        position += 1;
      } else if (current == '*') {
        if (!token_buffer.empty()) {
          segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        const auto is_asterisk_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        if (is_asterisk_asterisk) {
          segment_tokens.emplace_back(URLPatternPartAsteriskAsterisk{});
          position += 2;
        } else if (is_optional) {
          segment_tokens.emplace_back(URLPatternPartAsteriskOptional{});
          position += 2;
        } else if (is_multiple) {
          segment_tokens.emplace_back(URLPatternPartAsteriskMultiple{});
          position += 2;
        } else {
          segment_tokens.emplace_back(URLPatternPartAsterisk{});
          position += 1;
        }
        after_segment_delimiter = false;
      } else if (current == '(') {
        if (!token_buffer.empty()) {
          segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingRegExp;
        after_segment_delimiter = false;
        position += 1;
      } else if (current == '{') {
        if (!token_buffer.empty()) {
          segment_tokens.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_group = URLPatternPartGroup{};
        current_state = State::ReadingGroup;
        group_after_segment_delimiter = false;
        group_has_delimiter = false;
        group_inner_segment_prefix = false;
        group_inner_segment_suffix = false;
        after_segment_delimiter = false;
        inside_group = true;
        position += 1;
      } else if (current == '\\') {
        current_state = State::AfterBackslash;
        after_segment_delimiter = false;
        position += 1;
      } else {
        token_buffer += current;
        after_segment_delimiter = false;
        position += 1;
      }
    } else if (current_state == State::ReadingName) {
      if (token_buffer.empty()) {
        if (!is_valid_name_start(current)) {
          throw URLPatternParseError{position};
        }
        token_buffer += current;
        position += 1;
      } else {
        if (current == '.') {
          if (inside_group) {
            current_group.value.emplace_back(URLPatternPartName{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingGroup;
            group_after_segment_delimiter = true;
            position += 1;
          } else {
            segment_tokens.emplace_back(URLPatternPartName{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingChar;
            after_segment_delimiter = false;
          }
        } else if (current == '}' && inside_group) {
          current_group.value.emplace_back(URLPatternPartName{token_buffer});
          token_buffer.clear();
          const auto is_optional = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '?';
          const auto is_multiple = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '+';
          const auto is_asterisk = position + 1 < input_view.size() &&
                                   input_view[position + 1] == '*';
          const auto has_delimiter = group_has_delimiter;
          group_inner_segment_suffix = group_after_segment_delimiter;
          if (has_delimiter) {
            flush_segment_tokens();
          }
          if (is_optional) {
            segment_tokens.emplace_back(URLPatternPartGroupOptional{
                .value = current_group.value,
                .has_inner_segment_prefix = group_inner_segment_prefix,
                .has_inner_segment_suffix = group_inner_segment_suffix});
            position += 2;
          } else if (is_multiple) {
            segment_tokens.emplace_back(URLPatternPartGroupMultiple{
                .value = current_group.value,
                .has_inner_segment_prefix = group_inner_segment_prefix,
                .has_inner_segment_suffix = group_inner_segment_suffix});
            position += 2;
          } else if (is_asterisk) {
            segment_tokens.emplace_back(URLPatternPartGroupAsterisk{
                .value = current_group.value,
                .has_inner_segment_prefix = group_inner_segment_prefix,
                .has_inner_segment_suffix = group_inner_segment_suffix});
            position += 2;
          } else {
            current_group.has_inner_segment_prefix = group_inner_segment_prefix;
            current_group.has_inner_segment_suffix = group_inner_segment_suffix;
            segment_tokens.emplace_back(current_group);
            position += 1;
          }
          if (has_delimiter) {
            flush_segment_tokens();
          }
          current_state = State::ReadingChar;
          after_segment_delimiter = false;
          inside_group = false;
          group_after_segment_delimiter = false;
          group_has_delimiter = false;
        } else if (current == '(') {
          name_for_regex = token_buffer;
          token_buffer.clear();
          current_state = State::ReadingRegExp;
          after_segment_delimiter = false;
          position += 1;
        } else if (current == '*') {
          if (inside_group) {
            current_group.value.emplace_back(
                URLPatternPartNameAsterisk{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingGroup;
            group_after_segment_delimiter = false;
          } else {
            segment_tokens.emplace_back(
                URLPatternPartNameAsterisk{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingChar;
            after_segment_delimiter = false;
          }
          position += 1;
        } else if (current == '?') {
          if (inside_group) {
            current_group.value.emplace_back(
                URLPatternPartNameOptional{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingGroup;
            group_after_segment_delimiter = false;
          } else {
            segment_tokens.emplace_back(
                URLPatternPartNameOptional{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingChar;
            after_segment_delimiter = false;
          }
          position += 1;
        } else if (current == '+') {
          if (inside_group) {
            current_group.value.emplace_back(
                URLPatternPartNameMultiple{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingGroup;
            group_after_segment_delimiter = false;
          } else {
            segment_tokens.emplace_back(
                URLPatternPartNameMultiple{token_buffer});
            token_buffer.clear();
            current_state = State::ReadingChar;
            after_segment_delimiter = false;
          }
          position += 1;
        } else if (is_valid_name_char(current)) {
          token_buffer += current;
          position += 1;
        } else {
          const auto can_end_name = current == ':' || current == '{' ||
                                    current == '(' || current == '*' ||
                                    current == '[' || current == ']' ||
                                    current == '}' || current == '\\';
          if (inside_group) {
            if (can_end_name) {
              current_group.value.emplace_back(
                  URLPatternPartName{token_buffer});
              token_buffer.clear();
              current_state = State::ReadingGroup;
              group_after_segment_delimiter = false;
            } else {
              throw URLPatternParseError{position};
            }
          } else {
            if (can_end_name) {
              segment_tokens.emplace_back(URLPatternPartName{token_buffer});
              token_buffer.clear();
              current_state = State::ReadingChar;
              after_segment_delimiter = false;
            } else {
              throw URLPatternParseError{position};
            }
          }
        }
      }
    } else if (current_state == State::ReadingRegExp) {
      if (current == ')') {
        if (token_buffer.empty()) {
          throw URLPatternParseError{position};
        }
        if (!is_ascii_string(token_buffer)) {
          throw URLPatternParseError{position};
        }
        const auto regex = to_regex(token_buffer);
        if (!regex.has_value()) {
          throw URLPatternParseError{position};
        }
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        const auto is_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        if (name_for_regex.empty()) {
          if (inside_group) {
            if (is_optional) {
              current_group.value.emplace_back(URLPatternPartRegExpOptional{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else if (is_multiple) {
              current_group.value.emplace_back(URLPatternPartRegExpMultiple{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else if (is_asterisk) {
              current_group.value.emplace_back(URLPatternPartRegExpAsterisk{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else {
              current_group.value.emplace_back(URLPatternPartRegExp{
                  .value = regex.value(), .original_pattern = token_buffer});
            }
          } else {
            if (is_optional) {
              segment_tokens.emplace_back(URLPatternPartRegExpOptional{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else if (is_multiple) {
              segment_tokens.emplace_back(URLPatternPartRegExpMultiple{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else if (is_asterisk) {
              segment_tokens.emplace_back(URLPatternPartRegExpAsterisk{
                  .value = regex.value(), .original_pattern = token_buffer});
            } else {
              segment_tokens.emplace_back(URLPatternPartRegExp{
                  .value = regex.value(), .original_pattern = token_buffer});
            }
          }
        } else {
          if (inside_group) {
            current_group.value.emplace_back(
                URLPatternPartNameRegExp{.value = name_for_regex,
                                         .modifier = regex.value(),
                                         .original_pattern = token_buffer});
          } else {
            segment_tokens.emplace_back(
                URLPatternPartNameRegExp{.value = name_for_regex,
                                         .modifier = regex.value(),
                                         .original_pattern = token_buffer});
          }
          name_for_regex.clear();
        }
        token_buffer.clear();
        current_state = inside_group ? State::ReadingGroup : State::ReadingChar;
        if (inside_group) {
          group_after_segment_delimiter = false;
        } else {
          after_segment_delimiter = false;
        }
        position += (is_optional || is_multiple || is_asterisk) ? 2 : 1;
      } else {
        token_buffer += current;
        position += 1;
      }
    } else if (current_state == State::AfterBackslash) {
      if (position >= input_view.size()) {
        throw URLPatternParseError{position - 1};
      }
      token_buffer += current;
      current_state = inside_group ? State::ReadingGroup : State::ReadingChar;
      position += 1;
    } else if (current_state == State::ReadingGroup) {
      if (current == '}') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        } else if (group_after_segment_delimiter) {
          current_group.value.emplace_back(URLPatternPartChar{""});
        }
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        const auto is_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        const auto has_delimiter = group_has_delimiter;
        group_inner_segment_suffix = group_after_segment_delimiter;
        if (has_delimiter) {
          flush_segment_tokens();
        }
        if (is_optional) {
          segment_tokens.emplace_back(URLPatternPartGroupOptional{
              .value = current_group.value,
              .has_inner_segment_prefix = group_inner_segment_prefix,
              .has_inner_segment_suffix = group_inner_segment_suffix});
          position += 2;
        } else if (is_multiple) {
          segment_tokens.emplace_back(URLPatternPartGroupMultiple{
              .value = current_group.value,
              .has_inner_segment_prefix = group_inner_segment_prefix,
              .has_inner_segment_suffix = group_inner_segment_suffix});
          position += 2;
        } else if (is_asterisk) {
          segment_tokens.emplace_back(URLPatternPartGroupAsterisk{
              .value = current_group.value,
              .has_inner_segment_prefix = group_inner_segment_prefix,
              .has_inner_segment_suffix = group_inner_segment_suffix});
          position += 2;
        } else {
          current_group.has_inner_segment_prefix = group_inner_segment_prefix;
          current_group.has_inner_segment_suffix = group_inner_segment_suffix;
          segment_tokens.emplace_back(current_group);
          position += 1;
        }
        if (has_delimiter) {
          flush_segment_tokens();
        }
        current_state = State::ReadingChar;
        after_segment_delimiter = false;
        inside_group = false;
        group_after_segment_delimiter = false;
        group_has_delimiter = false;
      } else if (current == '.') {
        if (current_group.value.empty() && token_buffer.empty()) {
          group_inner_segment_prefix = true;
        }
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        } else if (group_after_segment_delimiter) {
          current_group.value.emplace_back(URLPatternPartChar{""});
        }
        group_after_segment_delimiter = true;
        group_has_delimiter = true;
        position += 1;
      } else if (current == ':') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingName;
        group_after_segment_delimiter = false;
        position += 1;
      } else if (current == '*') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        const auto is_asterisk_asterisk =
            position + 1 < input_view.size() && input_view[position + 1] == '*';
        const auto is_optional =
            position + 1 < input_view.size() && input_view[position + 1] == '?';
        const auto is_multiple =
            position + 1 < input_view.size() && input_view[position + 1] == '+';
        if (is_asterisk_asterisk) {
          current_group.value.emplace_back(URLPatternPartAsteriskAsterisk{});
          position += 2;
        } else if (is_optional) {
          current_group.value.emplace_back(URLPatternPartAsteriskOptional{});
          position += 2;
        } else if (is_multiple) {
          current_group.value.emplace_back(URLPatternPartAsteriskMultiple{});
          position += 2;
        } else {
          current_group.value.emplace_back(URLPatternPartAsterisk{});
          position += 1;
        }
        group_after_segment_delimiter = false;
      } else if (current == '(') {
        if (!token_buffer.empty()) {
          current_group.value.emplace_back(URLPatternPartChar{token_buffer});
          token_buffer.clear();
        }
        current_state = State::ReadingRegExp;
        group_after_segment_delimiter = false;
        position += 1;
      } else if (current == '\\') {
        current_state = State::AfterBackslash;
        group_after_segment_delimiter = false;
        position += 1;
      } else {
        token_buffer += current;
        group_after_segment_delimiter = false;
        position += 1;
      }
    }
  }

  if (current_state == State::AfterBackslash) {
    throw URLPatternParseError{position - 1};
  }

  if (current_state == State::ReadingRegExp) {
    throw URLPatternParseError{position};
  }

  if (current_state == State::ReadingName && token_buffer.empty()) {
    throw URLPatternParseError{position};
  }

  if (!token_buffer.empty()) {
    if (current_state == State::ReadingName) {
      segment_tokens.emplace_back(URLPatternPartName{token_buffer});
      token_buffer.clear();
    }
  }

  flush_segment_tokens();
}

auto URLPatternProtocol::operator<=>(const URLPatternProtocol &other) const
    -> std::strong_ordering {
  return compare_urlpattern_parts(this->value, other.value);
}

auto URLPatternUsername::operator<=>(const URLPatternUsername &other) const
    -> std::strong_ordering {
  return compare_urlpattern_parts(this->value, other.value);
}

auto URLPatternPassword::operator<=>(const URLPatternPassword &other) const
    -> std::strong_ordering {
  return compare_urlpattern_parts(this->value, other.value);
}

auto URLPatternHostname::operator<=>(const URLPatternHostname &other) const
    -> std::strong_ordering {
  std::vector<URLPatternPart> left_reversed(this->value.rbegin(),
                                            this->value.rend());
  std::vector<URLPatternPart> right_reversed(other.value.rbegin(),
                                             other.value.rend());
  return compare_urlpattern_parts_vector(left_reversed, right_reversed);
}

auto URLPatternPort::operator<=>(const URLPatternPort &other) const
    -> std::strong_ordering {
  return compare_urlpattern_parts(this->value, other.value);
}

auto URLPatternPathname::operator<=>(const URLPatternPathname &other) const
    -> std::strong_ordering {
  const auto comparison{
      compare_urlpattern_parts_vector(this->value, other.value)};
  if (comparison != std::strong_ordering::equal) {
    return comparison;
  }
  return this->is_bare_pattern <=> other.is_bare_pattern;
}

auto URLPatternSearch::operator<=>(const URLPatternSearch &other) const
    -> std::strong_ordering {
  return compare_urlpattern_parts(this->value, other.value);
}

auto URLPatternHash::operator<=>(const URLPatternHash &other) const
    -> std::strong_ordering {
  return compare_urlpattern_parts(this->value, other.value);
}

} // namespace sourcemeta::core
