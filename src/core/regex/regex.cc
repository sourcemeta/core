#include <sourcemeta/core/regex.h>

#include <pcre2.h>

#include <cassert> // assert
#include <cstddef> // std::size_t
#include <cstdint> // std::uint64_t
#include <regex>   // std::regex, std::smatch, std::regex_match
#include <string>  // std::string, std::stoull

namespace {

auto preprocess_ecma_to_pcre2(const std::string &pattern) -> std::string {
  std::string result;
  result.reserve(pattern.size() * 2);
  bool in_char_class{false};

  for (std::size_t index{0}; index < pattern.size(); ++index) {
    if (pattern[index] == '[' && (index == 0 || pattern[index - 1] != '\\')) {
      in_char_class = true;
      result += pattern[index];
      continue;
    }

    if (pattern[index] == ']' && (index == 0 || pattern[index - 1] != '\\')) {
      in_char_class = false;
      result += pattern[index];
      continue;
    }

    if (pattern[index] == '\\' && index + 1 < pattern.size()) {
      const char next_char{pattern[index + 1]};

      if (next_char == '\\') {
        result += "\\\\";
        ++index;
        continue;
      }

      if (next_char == 'u' && index + 2 < pattern.size() &&
          pattern[index + 2] == '{') {
        result += "\\x{";
        index += 2;
        for (++index; index < pattern.size() && pattern[index] != '}';
             ++index) {
          result += pattern[index];
        }
        if (index < pattern.size()) {
          result += '}';
        }
        continue;
      }

      if (next_char == 'u' && index + 5 < pattern.size()) {
        bool is_hex{true};
        for (std::size_t offset{0}; offset < 4; ++offset) {
          const char hex_char{pattern[index + 2 + offset]};
          if (!((hex_char >= '0' && hex_char <= '9') ||
                (hex_char >= 'a' && hex_char <= 'f') ||
                (hex_char >= 'A' && hex_char <= 'F'))) {
            is_hex = false;
            break;
          }
        }

        if (is_hex) {
          result += "\\x{";
          result += pattern[index + 2];
          result += pattern[index + 3];
          result += pattern[index + 4];
          result += pattern[index + 5];
          result += '}';
          index += 5;
          continue;
        }
      }

      if (next_char == 'p' && index + 2 < pattern.size() &&
          pattern[index + 2] == '{') {
        const std::size_t start_index{index};
        index += 3;
        std::string property_name;
        while (index < pattern.size() && pattern[index] != '}') {
          property_name += pattern[index];
          ++index;
        }

        if (index < pattern.size()) {
          if (property_name == "Letter") {
            result += "\\p{L}";
          } else if (property_name == "Uppercase_Letter") {
            result += "\\p{Lu}";
          } else if (property_name == "Lowercase_Letter") {
            result += "\\p{Ll}";
          } else if (property_name == "Titlecase_Letter") {
            result += "\\p{Lt}";
          } else if (property_name == "Modifier_Letter") {
            result += "\\p{Lm}";
          } else if (property_name == "Other_Letter") {
            result += "\\p{Lo}";
          } else if (property_name == "Mark") {
            result += "\\p{M}";
          } else if (property_name == "Nonspacing_Mark") {
            result += "\\p{Mn}";
          } else if (property_name == "Spacing_Mark") {
            result += "\\p{Mc}";
          } else if (property_name == "Enclosing_Mark") {
            result += "\\p{Me}";
          } else if (property_name == "Number") {
            result += "\\p{N}";
          } else if (property_name == "Decimal_Number") {
            result += "\\p{Nd}";
          } else if (property_name == "Letter_Number") {
            result += "\\p{Nl}";
          } else if (property_name == "Other_Number") {
            result += "\\p{No}";
          } else if (property_name == "Punctuation") {
            result += "\\p{P}";
          } else if (property_name == "Connector_Punctuation") {
            result += "\\p{Pc}";
          } else if (property_name == "Dash_Punctuation") {
            result += "\\p{Pd}";
          } else if (property_name == "Open_Punctuation") {
            result += "\\p{Ps}";
          } else if (property_name == "Close_Punctuation") {
            result += "\\p{Pe}";
          } else if (property_name == "Initial_Punctuation") {
            result += "\\p{Pi}";
          } else if (property_name == "Final_Punctuation") {
            result += "\\p{Pf}";
          } else if (property_name == "Other_Punctuation") {
            result += "\\p{Po}";
          } else if (property_name == "Symbol") {
            result += "\\p{S}";
          } else if (property_name == "Math_Symbol") {
            result += "\\p{Sm}";
          } else if (property_name == "Currency_Symbol") {
            result += "\\p{Sc}";
          } else if (property_name == "Modifier_Symbol") {
            result += "\\p{Sk}";
          } else if (property_name == "Other_Symbol") {
            result += "\\p{So}";
          } else if (property_name == "Separator") {
            result += "\\p{Z}";
          } else if (property_name == "Space_Separator") {
            result += "\\p{Zs}";
          } else if (property_name == "Line_Separator") {
            result += "\\p{Zl}";
          } else if (property_name == "Paragraph_Separator") {
            result += "\\p{Zp}";
          } else if (property_name == "Other") {
            result += "\\p{C}";
          } else if (property_name == "Control") {
            result += "\\p{Cc}";
          } else if (property_name == "Format") {
            result += "\\p{Cf}";
          } else if (property_name == "Unassigned") {
            result += "\\p{Cn}";
          } else if (property_name == "Private_Use") {
            result += "\\p{Co}";
          } else {
            result += pattern.substr(start_index, index - start_index + 1);
          }
        } else {
          index = start_index;
          result += pattern[index];
        }
        continue;
      }

      if (next_char == 'P' && index + 2 < pattern.size() &&
          pattern[index + 2] == '{') {
        const std::size_t start_index{index};
        index += 3;
        std::string property_name;
        while (index < pattern.size() && pattern[index] != '}') {
          property_name += pattern[index];
          ++index;
        }

        if (index < pattern.size()) {
          if (property_name == "Letter") {
            result += "\\P{L}";
          } else if (property_name == "Uppercase_Letter") {
            result += "\\P{Lu}";
          } else if (property_name == "Lowercase_Letter") {
            result += "\\P{Ll}";
          } else if (property_name == "Titlecase_Letter") {
            result += "\\P{Lt}";
          } else if (property_name == "Modifier_Letter") {
            result += "\\P{Lm}";
          } else if (property_name == "Other_Letter") {
            result += "\\P{Lo}";
          } else if (property_name == "Mark") {
            result += "\\P{M}";
          } else if (property_name == "Nonspacing_Mark") {
            result += "\\P{Mn}";
          } else if (property_name == "Spacing_Mark") {
            result += "\\P{Mc}";
          } else if (property_name == "Enclosing_Mark") {
            result += "\\P{Me}";
          } else if (property_name == "Number") {
            result += "\\P{N}";
          } else if (property_name == "Decimal_Number") {
            result += "\\P{Nd}";
          } else if (property_name == "Letter_Number") {
            result += "\\P{Nl}";
          } else if (property_name == "Other_Number") {
            result += "\\P{No}";
          } else if (property_name == "Punctuation") {
            result += "\\P{P}";
          } else if (property_name == "Connector_Punctuation") {
            result += "\\P{Pc}";
          } else if (property_name == "Dash_Punctuation") {
            result += "\\P{Pd}";
          } else if (property_name == "Open_Punctuation") {
            result += "\\P{Ps}";
          } else if (property_name == "Close_Punctuation") {
            result += "\\P{Pe}";
          } else if (property_name == "Initial_Punctuation") {
            result += "\\P{Pi}";
          } else if (property_name == "Final_Punctuation") {
            result += "\\P{Pf}";
          } else if (property_name == "Other_Punctuation") {
            result += "\\P{Po}";
          } else if (property_name == "Symbol") {
            result += "\\P{S}";
          } else if (property_name == "Math_Symbol") {
            result += "\\P{Sm}";
          } else if (property_name == "Currency_Symbol") {
            result += "\\P{Sc}";
          } else if (property_name == "Modifier_Symbol") {
            result += "\\P{Sk}";
          } else if (property_name == "Other_Symbol") {
            result += "\\P{So}";
          } else if (property_name == "Separator") {
            result += "\\P{Z}";
          } else if (property_name == "Space_Separator") {
            result += "\\P{Zs}";
          } else if (property_name == "Line_Separator") {
            result += "\\P{Zl}";
          } else if (property_name == "Paragraph_Separator") {
            result += "\\P{Zp}";
          } else if (property_name == "Other") {
            result += "\\P{C}";
          } else if (property_name == "Control") {
            result += "\\P{Cc}";
          } else if (property_name == "Format") {
            result += "\\P{Cf}";
          } else if (property_name == "Unassigned") {
            result += "\\P{Cn}";
          } else if (property_name == "Private_Use") {
            result += "\\P{Co}";
          } else {
            result += pattern.substr(start_index, index - start_index + 1);
          }
        } else {
          index = start_index;
          result += pattern[index];
        }
        continue;
      }

      if (next_char == 'd') {
        result += in_char_class ? "0-9" : "[0-9]";
        ++index;
        continue;
      }

      if (next_char == 'D' && !in_char_class) {
        result += "[^0-9]";
        ++index;
        continue;
      }

      if (next_char == 'w') {
        result += in_char_class ? "a-zA-Z0-9_" : "[a-zA-Z0-9_]";
        ++index;
        continue;
      }

      if (next_char == 'W' && !in_char_class) {
        result += "[^a-zA-Z0-9_]";
        ++index;
        continue;
      }

      if (next_char == 's') {
        result +=
            in_char_class
                ? R"(\t\v\f \x{00A0}\x{FEFF}\p{Zs}\n\r\x{2028}\x{2029})"
                : R"([\t\v\f \x{00A0}\x{FEFF}\p{Zs}\n\r\x{2028}\x{2029}])";
        ++index;
        continue;
      }

      if (next_char == 'S' && !in_char_class) {
        result += R"([^\t\v\f \x{00A0}\x{FEFF}\p{Zs}\n\r\x{2028}\x{2029}])";
        ++index;
        continue;
      }

      if (next_char == 'b' && !in_char_class) {
        result +=
            R"((?:(?<![a-zA-Z0-9_])(?=[a-zA-Z0-9_])|(?<=[a-zA-Z0-9_])(?![a-zA-Z0-9_])))";
        ++index;
        continue;
      }

      if (next_char == 'B' && !in_char_class) {
        result +=
            R"((?:(?<=[a-zA-Z0-9_])(?=[a-zA-Z0-9_])|(?<![a-zA-Z0-9_])(?![a-zA-Z0-9_])))";
        ++index;
        continue;
      }
    }

    if (pattern[index] == '$' && index + 1 < pattern.size()) {
      result += "\\$";
      continue;
    }

    result += pattern[index];
  }

  return result;
}

} // namespace

namespace sourcemeta::core {

auto to_regex(const std::string &pattern) -> std::optional<Regex> {
  if (pattern == ".*" || pattern == "^.*$" || pattern == "^(.*)$" ||
      pattern == "(.*)" || pattern == "[\\s\\S]*" || pattern == "^[\\s\\S]*$") {
    return RegexTypeNoop{};

    // Note that the JSON Schema specification does not impose the use of any
    // regular expression flag. Given popular adoption, we assume `.` matches
    // new line characters (as in the `DOTALL`) option
  } else if (pattern == ".+" || pattern == "^.+$" || pattern == "^(.+)$" ||
             pattern == ".") {
    return RegexTypeNonEmpty{};
  }

  const std::regex PREFIX_REGEX{R"(^\^([a-zA-Z0-9-_/@]+)(\.\*)?)"};
  std::smatch matches_prefix;
  if (std::regex_match(pattern, matches_prefix, PREFIX_REGEX)) {
    return RegexTypePrefix{matches_prefix[1].str()};
  }

  const std::regex RANGE_REGEX{R"(^\^\.\{(\d+),(\d+)\}\$$)"};
  std::smatch matches_range;
  if (std::regex_match(pattern, matches_range, RANGE_REGEX)) {
    const std::uint64_t minimum{std::stoull(matches_range[1].str())};
    const std::uint64_t maximum{std::stoull(matches_range[2].str())};
    assert(minimum <= maximum);
    return RegexTypeRange{minimum, maximum};
  }

  const std::string pcre2_pattern{preprocess_ecma_to_pcre2(pattern)};
  int pcre2_error_code{0};
  PCRE2_SIZE pcre2_error_offset{0};
  pcre2_code *pcre2_regex_raw{pcre2_compile(
      reinterpret_cast<PCRE2_SPTR>(pcre2_pattern.c_str()), pcre2_pattern.size(),
      PCRE2_UTF | PCRE2_UCP | PCRE2_NO_AUTO_CAPTURE | PCRE2_DOTALL |
          PCRE2_DOLLAR_ENDONLY | PCRE2_NEVER_BACKSLASH_C | PCRE2_NO_UTF_CHECK,
      &pcre2_error_code, &pcre2_error_offset, nullptr)};

  if (pcre2_regex_raw != nullptr) {
    std::shared_ptr<pcre2_code> pcre2_regex{pcre2_regex_raw, pcre2_code_free};
    pcre2_jit_compile(pcre2_regex.get(), PCRE2_JIT_COMPLETE);
    return RegexTypePCRE2{std::shared_ptr<void>(pcre2_regex)};
  }

  return std::nullopt;
}

auto matches(const Regex &regex, const std::string &value) -> bool {
  switch (static_cast<RegexIndex>(regex.index())) {
    case RegexIndex::Prefix:
      return value.starts_with(*std::get_if<RegexTypePrefix>(&regex));
    case RegexIndex::NonEmpty:
      return !value.empty();
    case RegexIndex::Range:
      return value.size() >= std::get_if<RegexTypeRange>(&regex)->first &&
             value.size() <= std::get_if<RegexTypeRange>(&regex)->second;
    case RegexIndex::PCRE2: {
      const RegexTypePCRE2 *pcre2_regex{std::get_if<RegexTypePCRE2>(&regex)};
      auto *pcre2_code_ptr{static_cast<pcre2_code *>(pcre2_regex->code.get())};
      // Re-use this to avoid creating and destroying the `struct`on every call
      thread_local pcre2_match_data *match_data{
          pcre2_match_data_create(1, nullptr)};
      const int match_result{pcre2_match(
          pcre2_code_ptr, reinterpret_cast<PCRE2_SPTR>(value.c_str()),
          value.size(), 0, PCRE2_NO_UTF_CHECK, match_data, nullptr)};
      return match_result >= 0;
    }
    case RegexIndex::Noop:
      return true;
  }

#if defined(_MSC_VER) && !defined(__clang__)
  __assume(false);
#else
  __builtin_unreachable();
#endif
}

auto matches_if_valid(const std::string &pattern, const std::string &value)
    -> bool {
  const auto regex{to_regex(pattern)};
  return regex.has_value() && matches(regex.value(), value);
}

} // namespace sourcemeta::core
