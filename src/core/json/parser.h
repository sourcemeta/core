#ifndef SOURCEMETA_CORE_JSON_PARSER_H_
#define SOURCEMETA_CORE_JSON_PARSER_H_

#include <sourcemeta/core/json_error.h>
#include <sourcemeta/core/json_value.h>

#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/unicode.h>

#include "grammar.h"

#include <cassert>    // assert
#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint64_t
#include <functional> // std::reference_wrapper
#include <optional>   // std::optional
#include <stdexcept>  // std::invalid_argument
#include <string>     // std::basic_string
#include <utility>    // std::move
#include <vector>     // std::vector

namespace sourcemeta::core::internal {

inline auto skip_whitespace(const char *&cursor, const char *end,
                            std::uint64_t &line, std::uint64_t &column)
    -> void {
  while (cursor < end) {
    switch (*cursor) {
      case internal::token_whitespace_space<typename JSON::Char>:
      case internal::token_whitespace_tabulation<typename JSON::Char>:
      case internal::token_whitespace_carriage_return<typename JSON::Char>:
        column += 1;
        cursor++;
        continue;
      case internal::token_whitespace_line_feed<typename JSON::Char>:
        line += 1;
        column = 0;
        cursor++;
        continue;
      default:
        return;
    }
  }
}

inline auto parse_null(const std::uint64_t line, std::uint64_t &column,
                       const char *&cursor, const char *end) -> JSON {
  for (
      const auto character :
      internal::constant_null<typename JSON::Char, typename JSON::CharTraits>.substr(
          1)) {
    column += 1;
    if (cursor >= end) {
      throw JSONParseError(line, column);
    }
    if (*cursor != character) {
      throw JSONParseError(line, column);
    }
    cursor++;
  }

  return JSON{nullptr};
}

inline auto parse_boolean_true(const std::uint64_t line, std::uint64_t &column,
                               const char *&cursor, const char *end) -> JSON {
  for (
      const auto character :
      internal::constant_true<typename JSON::Char, typename JSON::CharTraits>.substr(
          1)) {
    column += 1;
    if (cursor >= end) {
      throw JSONParseError(line, column);
    }
    if (*cursor != character) {
      throw JSONParseError(line, column);
    }
    cursor++;
  }

  return JSON{true};
}

inline auto parse_boolean_false(const std::uint64_t line, std::uint64_t &column,
                                const char *&cursor, const char *end) -> JSON {
  for (
      const auto character :
      internal::constant_false<typename JSON::Char, typename JSON::CharTraits>.substr(
          1)) {
    column += 1;
    if (cursor >= end) {
      throw JSONParseError(line, column);
    }
    if (*cursor != character) {
      throw JSONParseError(line, column);
    }
    cursor++;
  }

  return JSON{false};
}

inline auto parse_string_unicode_code_point(const std::uint64_t line,
                                            std::uint64_t &column,
                                            const char *&cursor,
                                            const char *end) -> unsigned long {
  unsigned long result{0};
  for (std::size_t index = 0; index < 4; index++) {
    column += 1;
    if (cursor >= end) {
      throw JSONParseError(line, column);
    }
    const char hex_char{*cursor++};
    unsigned long digit;
    if (hex_char >= '0' && hex_char <= '9') {
      digit = static_cast<unsigned long>(hex_char - '0');
    } else if (hex_char >= 'a' && hex_char <= 'f') {
      digit = static_cast<unsigned long>(hex_char - 'a') + 10;
    } else if (hex_char >= 'A' && hex_char <= 'F') {
      digit = static_cast<unsigned long>(hex_char - 'A') + 10;
    } else {
      throw JSONParseError(line, column);
    }
    result = (result << 4) | digit;
  }

  assert(result <= 0xFFFF);
  return result;
}

inline auto parse_string_unicode(const std::uint64_t line,
                                 std::uint64_t &column, const char *&cursor,
                                 const char *end, typename JSON::String &result)
    -> void {
  auto code_point{parse_string_unicode_code_point(line, column, cursor, end)};
  using CharT = typename JSON::Char;

  // Lone low surrogate without a preceding high surrogate
  if (code_point >= 0xDC00 && code_point <= 0xDFFF) {
    throw JSONParseError(line, column);
  }

  if (code_point >= 0xD800 && code_point <= 0xDBFF) {
    column += 1;
    if (cursor >= end) {
      throw JSONParseError(line, column);
    }
    if (*cursor != internal::token_string_escape<CharT>) {
      throw JSONParseError(line, column);
    }
    cursor++;

    column += 1;
    if (cursor >= end) {
      throw JSONParseError(line, column);
    }
    if (*cursor != internal::token_string_escape_unicode<CharT>) {
      throw JSONParseError(line, column);
    }
    cursor++;

    const auto low_code_point{
        parse_string_unicode_code_point(line, column, cursor, end)};

    // See
    // https://en.wikipedia.org/wiki/UTF-16#Code_points_from_U+010000_to_U+10FFFF
    if (low_code_point >= 0xDC00 && low_code_point <= 0xDFFF) {
      code_point =
          0x10000 + ((code_point - 0xD800) << 10) + (low_code_point - 0xDC00);
    } else {
      throw JSONParseError(line, column);
    }
  }

  sourcemeta::core::codepoint_to_utf8(static_cast<char32_t>(code_point),
                                      result);
}

inline auto parse_string_escape(const std::uint64_t line, std::uint64_t &column,
                                const char *&cursor, const char *end,
                                typename JSON::String &result) -> void {
  column += 1;
  if (cursor >= end) {
    throw JSONParseError(line, column);
  }
  switch (*cursor++) {
    case internal::token_string_quote<typename JSON::Char>:
      result.push_back(internal::token_string_quote<typename JSON::Char>);
      return;
    case internal::token_string_escape<typename JSON::Char>:
      result.push_back(internal::token_string_escape<typename JSON::Char>);
      return;
    case internal::token_string_solidus<typename JSON::Char>:
      result.push_back(internal::token_string_solidus<typename JSON::Char>);
      return;
    case internal::token_string_escape_backspace<typename JSON::Char>:
      result.push_back('\b');
      return;
    case internal::token_string_escape_form_feed<typename JSON::Char>:
      result.push_back('\f');
      return;
    case internal::token_string_escape_line_feed<typename JSON::Char>:
      result.push_back('\n');
      return;
    case internal::token_string_escape_carriage_return<typename JSON::Char>:
      result.push_back('\r');
      return;
    case internal::token_string_escape_tabulation<typename JSON::Char>:
      result.push_back('\t');
      return;

    // Any code point may be represented as a hexadecimal escape sequence.
    // The meaning of such a hexadecimal number is determined by ISO/IEC
    // 10646. If the code point is in the Basic Multilingual Plane (U+0000
    // through U+FFFF), then it may be represented as a six-character
    // sequence: a reverse solidus, followed by the lowercase letter u,
    // followed by four hexadecimal digits that encode the code point.
    // Hexadecimal digits can be digits (U+0030 through U+0039) or the
    // hexadecimal letters A through F in uppercase (U+0041 through U+0046)
    // or lowercase (U+0061 through U+0066).
    // See
    // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
    case internal::token_string_escape_unicode<typename JSON::Char>:
      parse_string_unicode(line, column, cursor, end, result);
      return;

    default:
      throw JSONParseError(line, column);
  }
}

inline auto parse_string(const std::uint64_t line, std::uint64_t &column,
                         const char *&cursor, const char *end) ->
    typename JSON::String {
  typename JSON::String result;
  while (cursor < end) {
    const char *scan{cursor};
    while (scan < end && *scan != '"' && *scan != '\\' &&
           static_cast<unsigned char>(*scan) >= 0x20) {
      scan++;
    }

    if (scan > cursor) {
      column += static_cast<std::uint64_t>(scan - cursor);
      result.append(cursor, static_cast<std::size_t>(scan - cursor));
      cursor = scan;
    }

    if (cursor >= end) {
      column += 1;
      throw JSONParseError(line, column);
    }

    column += 1;
    const char character{*cursor++};

    switch (character) {
      // A string is a sequence of Unicode code points wrapped with quotation
      // marks (U+0022). See
      // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
      case internal::token_string_quote<typename JSON::Char>:
        return result;
      case internal::token_string_escape<typename JSON::Char>:
        parse_string_escape(line, column, cursor, end, result);
        break;
      default:
        throw JSONParseError(line, column);
    }
  }

  column += 1;
  throw JSONParseError(line, column);
}

template <typename CharT, typename Traits>
auto parse_number_decimal(const std::uint64_t line, const std::uint64_t column,
                          const std::basic_string<CharT, Traits> &string)
    -> JSON {
  try {
    return JSON{Decimal{string}};
  } catch (const DecimalParseError &) {
    throw JSONParseError(line, column);
  } catch (const std::invalid_argument &) {
    throw JSONParseError(line, column);
  }
}

template <typename CharT, typename Traits>
auto parse_number_integer_maybe_decimal(
    const std::uint64_t line, const std::uint64_t column,
    const std::basic_string<CharT, Traits> &string) -> JSON {
  const auto result{sourcemeta::core::to_int64_t(string)};
  return result.has_value() ? JSON{result.value()}
                            : parse_number_decimal(line, column, string);
}

template <typename CharT, typename Traits>
auto parse_number_real_maybe_decimal(
    const std::uint64_t line, const std::uint64_t column,
    const std::basic_string<CharT, Traits> &string,
    const std::size_t first_nonzero_position,
    const std::size_t decimal_position) -> JSON {
  // We are guaranteed to not be dealing with exponential numbers here
  assert((string.find('e') == std::basic_string<CharT, Traits>::npos));
  assert((string.find('E') == std::basic_string<CharT, Traits>::npos));

  // If the number has enough significant digits, then we risk completely losing
  // precision of the fractional component, and thus incorrectly interpreting a
  // fractional number as an integral value
  const auto decimal_after_first_nonzero{
      decimal_position != std::basic_string<CharT, Traits>::npos &&
      decimal_position > first_nonzero_position};
  const auto significant_digits{string.length() - first_nonzero_position -
                                (decimal_after_first_nonzero ? 1 : 0)};
  constexpr std::size_t MAX_SAFE_SIGNIFICANT_DIGITS{15};
  if (significant_digits > MAX_SAFE_SIGNIFICANT_DIGITS) {
    return parse_number_decimal(line, column, string);
  }

  const auto result{sourcemeta::core::to_double(string)};
  return result.has_value() ? JSON{result.value()}
                            : parse_number_decimal(line, column, string);
}

inline auto parse_number_exponent_rest(const std::uint64_t line,
                                       std::uint64_t &column,
                                       const std::uint64_t original_column,
                                       const char *&cursor, const char *end,
                                       typename JSON::String &result) -> JSON {
  while (cursor < end) {
    const char character{*cursor};
    switch (character) {
      case internal::token_number_zero<typename JSON::Char>:
      case internal::token_number_one<typename JSON::Char>:
      case internal::token_number_two<typename JSON::Char>:
      case internal::token_number_three<typename JSON::Char>:
      case internal::token_number_four<typename JSON::Char>:
      case internal::token_number_five<typename JSON::Char>:
      case internal::token_number_six<typename JSON::Char>:
      case internal::token_number_seven<typename JSON::Char>:
      case internal::token_number_eight<typename JSON::Char>:
      case internal::token_number_nine<typename JSON::Char>:
        result.push_back(character);
        cursor++;
        column += 1;
        break;
      default:
        // As a heuristic, if a number has exponential notation, it is almost
        // always a big number for which `double` is typically a poor
        // representation. If an exponent is encountered, we just always parse
        // as a high-precision decimal
        return parse_number_decimal(line, original_column, result);
    }
  }

  return parse_number_decimal(line, original_column, result);
}

inline auto parse_number_exponent(const std::uint64_t line,
                                  std::uint64_t &column,
                                  const std::uint64_t original_column,
                                  const char *&cursor, const char *end,
                                  typename JSON::String &result) -> JSON {
  if (cursor >= end) {
    column += 1;
    throw JSONParseError(line, column);
  }
  const char character{*cursor++};
  column += 1;
  switch (character) {
    case internal::token_number_zero<typename JSON::Char>:
    case internal::token_number_one<typename JSON::Char>:
    case internal::token_number_two<typename JSON::Char>:
    case internal::token_number_three<typename JSON::Char>:
    case internal::token_number_four<typename JSON::Char>:
    case internal::token_number_five<typename JSON::Char>:
    case internal::token_number_six<typename JSON::Char>:
    case internal::token_number_seven<typename JSON::Char>:
    case internal::token_number_eight<typename JSON::Char>:
    case internal::token_number_nine<typename JSON::Char>:
      result.push_back(character);
      return parse_number_exponent_rest(line, column, original_column, cursor,
                                        end, result);
    default:
      throw JSONParseError(line, column);
  }
}

inline auto parse_number_exponent_first(const std::uint64_t line,
                                        std::uint64_t &column,
                                        const std::uint64_t original_column,
                                        const char *&cursor, const char *end,
                                        typename JSON::String &result) -> JSON {
  if (cursor >= end) {
    column += 1;
    throw JSONParseError(line, column);
  }
  const char character{*cursor++};
  column += 1;
  switch (character) {
    case internal::token_number_plus<typename JSON::Char>:
      return parse_number_exponent(line, column, original_column, cursor, end,
                                   result);
    case internal::token_number_minus<typename JSON::Char>:
      result.push_back(character);
      return parse_number_exponent(line, column, original_column, cursor, end,
                                   result);

    case internal::token_number_zero<typename JSON::Char>:
    case internal::token_number_one<typename JSON::Char>:
    case internal::token_number_two<typename JSON::Char>:
    case internal::token_number_three<typename JSON::Char>:
    case internal::token_number_four<typename JSON::Char>:
    case internal::token_number_five<typename JSON::Char>:
    case internal::token_number_six<typename JSON::Char>:
    case internal::token_number_seven<typename JSON::Char>:
    case internal::token_number_eight<typename JSON::Char>:
    case internal::token_number_nine<typename JSON::Char>:
      result.push_back(character);
      return parse_number_exponent_rest(line, column, original_column, cursor,
                                        end, result);
    default:
      throw JSONParseError(line, column);
  }
}

inline auto parse_number_fractional(
    const std::uint64_t line, std::uint64_t &column,
    const std::uint64_t original_column, const char *&cursor, const char *end,
    typename JSON::String &result, std::size_t &first_nonzero_position,
    const std::size_t decimal_position) -> JSON {
  while (cursor < end) {
    const char character{*cursor};
    switch (character) {
      // [A number] may have an exponent, prefixed by e (U+0065) or E (U+0045)
      // See
      // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
      case internal::token_number_exponent_uppercase<typename JSON::Char>:
      case internal::token_number_exponent_lowercase<typename JSON::Char>:
        result.push_back(character);
        cursor++;
        column += 1;
        return parse_number_exponent_first(line, column, original_column,
                                           cursor, end, result);

      case internal::token_number_zero<typename JSON::Char>:
        result.push_back(character);
        cursor++;
        column += 1;
        break;
      case internal::token_number_one<typename JSON::Char>:
      case internal::token_number_two<typename JSON::Char>:
      case internal::token_number_three<typename JSON::Char>:
      case internal::token_number_four<typename JSON::Char>:
      case internal::token_number_five<typename JSON::Char>:
      case internal::token_number_six<typename JSON::Char>:
      case internal::token_number_seven<typename JSON::Char>:
      case internal::token_number_eight<typename JSON::Char>:
      case internal::token_number_nine<typename JSON::Char>:
        if (first_nonzero_position ==
            std::basic_string<typename JSON::Char,
                              typename JSON::CharTraits>::npos) {
          first_nonzero_position = result.size();
        }
        result.push_back(character);
        cursor++;
        column += 1;
        break;
      default:
        return parse_number_real_maybe_decimal(line, original_column, result,
                                               first_nonzero_position,
                                               decimal_position);
    }
  }

  return parse_number_real_maybe_decimal(
      line, original_column, result, first_nonzero_position, decimal_position);
}

inline auto parse_number_fractional_first(
    const std::uint64_t line, std::uint64_t &column,
    const std::uint64_t original_column, const char *&cursor, const char *end,
    typename JSON::String &result, std::size_t &first_nonzero_position,
    const std::size_t decimal_position) -> JSON {
  if (cursor >= end) {
    column += 1;
    throw JSONParseError(line, column);
  }
  const char character{*cursor};
  switch (character) {
    // [A number] may have a fractional part prefixed by a decimal point
    // (U+002E). See
    // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
    case internal::token_number_decimal_point<typename JSON::Char>:
      column += 1;
      throw JSONParseError(line, column);
    case internal::token_number_zero<typename JSON::Char>:
      result.push_back(character);
      cursor++;
      column += 1;
      return parse_number_fractional(line, column, original_column, cursor, end,
                                     result, first_nonzero_position,
                                     decimal_position);
    case internal::token_number_one<typename JSON::Char>:
    case internal::token_number_two<typename JSON::Char>:
    case internal::token_number_three<typename JSON::Char>:
    case internal::token_number_four<typename JSON::Char>:
    case internal::token_number_five<typename JSON::Char>:
    case internal::token_number_six<typename JSON::Char>:
    case internal::token_number_seven<typename JSON::Char>:
    case internal::token_number_eight<typename JSON::Char>:
    case internal::token_number_nine<typename JSON::Char>:
      if (first_nonzero_position ==
          std::basic_string<typename JSON::Char,
                            typename JSON::CharTraits>::npos) {
        first_nonzero_position = result.size();
      }
      result.push_back(character);
      cursor++;
      column += 1;
      return parse_number_fractional(line, column, original_column, cursor, end,
                                     result, first_nonzero_position,
                                     decimal_position);
    default:
      return parse_number_real_maybe_decimal(line, original_column, result,
                                             first_nonzero_position,
                                             decimal_position);
  }
}

inline auto parse_number_maybe_fractional(const std::uint64_t line,
                                          std::uint64_t &column,
                                          const std::uint64_t original_column,
                                          const char *&cursor, const char *end,
                                          typename JSON::String &result,
                                          std::size_t &first_nonzero_position)
    -> JSON {
  if (cursor >= end) {
    return JSON{
        parse_number_integer_maybe_decimal(line, original_column, result)};
  }
  const char character{*cursor};
  switch (character) {
    // [A number] may have a fractional part prefixed by a decimal point
    // (U+002E). See
    // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
    case internal::token_number_decimal_point<typename JSON::Char>: {
      const std::size_t decimal_position{result.size()};
      result.push_back(character);
      cursor++;
      column += 1;
      return JSON{parse_number_fractional_first(
          line, column, original_column, cursor, end, result,
          first_nonzero_position, decimal_position)};
    }
    case internal::token_number_exponent_uppercase<typename JSON::Char>:
    case internal::token_number_exponent_lowercase<typename JSON::Char>:
      result.push_back(character);
      cursor++;
      column += 1;
      return JSON{parse_number_exponent_first(line, column, original_column,
                                              cursor, end, result)};
    case internal::token_number_one<typename JSON::Char>:
    case internal::token_number_two<typename JSON::Char>:
    case internal::token_number_three<typename JSON::Char>:
    case internal::token_number_four<typename JSON::Char>:
    case internal::token_number_five<typename JSON::Char>:
    case internal::token_number_six<typename JSON::Char>:
    case internal::token_number_seven<typename JSON::Char>:
    case internal::token_number_eight<typename JSON::Char>:
    case internal::token_number_nine<typename JSON::Char>:
      column += 1;
      throw JSONParseError(line, column);
    default:
      return JSON{
          parse_number_integer_maybe_decimal(line, original_column, result)};
  }
}

inline auto parse_number_any_rest(const std::uint64_t line,
                                  std::uint64_t &column,
                                  const std::uint64_t original_column,
                                  const char *&cursor, const char *end,
                                  typename JSON::String &result,
                                  std::size_t &first_nonzero_position) -> JSON {
  while (cursor < end) {
    const char character{*cursor};
    switch (character) {
      // [A number] may have a fractional part prefixed by a decimal point
      // (U+002E). See
      // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
      case internal::token_number_decimal_point<typename JSON::Char>: {
        const std::size_t decimal_position{result.size()};
        result.push_back(character);
        cursor++;
        column += 1;
        return JSON{parse_number_fractional_first(
            line, column, original_column, cursor, end, result,
            first_nonzero_position, decimal_position)};
      }
      case internal::token_number_exponent_uppercase<typename JSON::Char>:
      case internal::token_number_exponent_lowercase<typename JSON::Char>:
        result.push_back(character);
        cursor++;
        column += 1;
        return JSON{parse_number_exponent_first(line, column, original_column,
                                                cursor, end, result)};
      case internal::token_number_zero<typename JSON::Char>:
      case internal::token_number_one<typename JSON::Char>:
      case internal::token_number_two<typename JSON::Char>:
      case internal::token_number_three<typename JSON::Char>:
      case internal::token_number_four<typename JSON::Char>:
      case internal::token_number_five<typename JSON::Char>:
      case internal::token_number_six<typename JSON::Char>:
      case internal::token_number_seven<typename JSON::Char>:
      case internal::token_number_eight<typename JSON::Char>:
      case internal::token_number_nine<typename JSON::Char>:
        result.push_back(character);
        cursor++;
        column += 1;
        break;
      default:
        return JSON{
            parse_number_integer_maybe_decimal(line, original_column, result)};
    }
  }

  return JSON{
      parse_number_integer_maybe_decimal(line, original_column, result)};
}

inline auto
parse_number_any_negative_first(const std::uint64_t line, std::uint64_t &column,
                                const std::uint64_t original_column,
                                const char *&cursor, const char *end,
                                typename JSON::String &result,
                                std::size_t &first_nonzero_position) -> JSON {
  if (cursor >= end) {
    column += 1;
    throw JSONParseError(line, column);
  }
  const char character{*cursor++};
  column += 1;
  switch (character) {
    // A number is a sequence of decimal digits with no superfluous leading
    // zero. See
    // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
    case internal::token_number_zero<typename JSON::Char>:
      result.push_back(character);
      return parse_number_maybe_fractional(line, column, original_column,
                                           cursor, end, result,
                                           first_nonzero_position);
    case internal::token_number_one<typename JSON::Char>:
    case internal::token_number_two<typename JSON::Char>:
    case internal::token_number_three<typename JSON::Char>:
    case internal::token_number_four<typename JSON::Char>:
    case internal::token_number_five<typename JSON::Char>:
    case internal::token_number_six<typename JSON::Char>:
    case internal::token_number_seven<typename JSON::Char>:
    case internal::token_number_eight<typename JSON::Char>:
    case internal::token_number_nine<typename JSON::Char>:
      first_nonzero_position = result.size();
      result.push_back(character);
      return parse_number_any_rest(line, column, original_column, cursor, end,
                                   result, first_nonzero_position);
    default:
      throw JSONParseError(line, column);
  }
}

inline auto parse_number(const std::uint64_t line, std::uint64_t &column,
                         const char *&cursor, const char *end, const char first)
    -> JSON {
  typename JSON::String result;
  result.push_back(first);

  std::size_t first_nonzero_position{
      std::basic_string<typename JSON::Char, typename JSON::CharTraits>::npos};

  // A number is a sequence of decimal digits with no superfluous leading zero.
  // It may have a preceding minus sign (U+002D). See
  // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
  switch (first) {
    case internal::token_number_minus<typename JSON::Char>:
      return parse_number_any_negative_first(line, column, column, cursor, end,
                                             result, first_nonzero_position);
    case internal::token_number_zero<typename JSON::Char>:
      return parse_number_maybe_fractional(line, column, column, cursor, end,
                                           result, first_nonzero_position);
    default:
      first_nonzero_position = 0;
      return parse_number_any_rest(line, column, column, cursor, end, result,
                                   first_nonzero_position);
  }
}

} // namespace sourcemeta::core::internal

// We use "goto" to avoid recursion
// NOLINTBEGIN(cppcoreguidelines-avoid-goto)

#define CALLBACK_PRE(value_type, context, index, property)                     \
  if (callback) {                                                              \
    callback(JSON::ParsePhase::Pre, JSON::Type::value_type, line, column,      \
             context, index, property);                                        \
  }

#define CALLBACK_PRE_WITH_POSITION(value_type, line, column, context, index,   \
                                   property)                                   \
  if (callback) {                                                              \
    callback(JSON::ParsePhase::Pre, JSON::Type::value_type, line, column,      \
             context, index, property);                                        \
  }

#define CALLBACK_POST(value_type)                                              \
  if (callback) {                                                              \
    callback(JSON::ParsePhase::Post, JSON::Type::value_type, line, column,     \
             JSON::ParseContext::Root, 0, JSON::StringView{});                 \
  }

namespace sourcemeta::core {
inline auto internal_parse_json(const char *&cursor, const char *end,
                                std::uint64_t &line, std::uint64_t &column,
                                const JSON::ParseCallback &callback) -> JSON {
  using Result = JSON;
  enum class Container : std::uint8_t { Array, Object };
  std::vector<Container> levels;
  std::vector<std::reference_wrapper<Result>> frames;
  levels.reserve(32);
  frames.reserve(32);
  std::optional<Result> result;
  typename Result::String key{""};
  std::uint64_t key_line{0};
  std::uint64_t key_column{0};
  char character = 0;

  /*
   * Parse any JSON document
   */

  internal::skip_whitespace(cursor, end, line, column);
  if (cursor >= end) {
    column += 1;
    throw JSONParseError(line, column);
  }
  column += 1;
  character = *cursor++;

  // A JSON value can be an object, array, number, string, true, false, or null.
  // See
  // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
  switch (character) {
    case internal::constant_true<typename JSON::Char, typename JSON::CharTraits>.front():
      if (callback) {
        CALLBACK_PRE(Boolean, JSON::ParseContext::Root, 0, JSON::StringView{});
        const auto value{
            internal::parse_boolean_true(line, column, cursor, end)};
        CALLBACK_POST(Boolean);
        return value;
      } else {
        return internal::parse_boolean_true(line, column, cursor, end);
      }
    case internal::constant_false<typename JSON::Char, typename JSON::CharTraits>.front():
      if (callback) {
        CALLBACK_PRE(Boolean, JSON::ParseContext::Root, 0, JSON::StringView{});
        const auto value{
            internal::parse_boolean_false(line, column, cursor, end)};
        CALLBACK_POST(Boolean);
        return value;
      } else {
        return internal::parse_boolean_false(line, column, cursor, end);
      }
    case internal::constant_null<typename JSON::Char, typename JSON::CharTraits>.front():
      if (callback) {
        CALLBACK_PRE(Null, JSON::ParseContext::Root, 0, JSON::StringView{});
        const auto value{internal::parse_null(line, column, cursor, end)};
        CALLBACK_POST(Null);
        return value;
      } else {
        return internal::parse_null(line, column, cursor, end);
      }

    // A string is a sequence of Unicode code points wrapped with quotation
    // marks (U+0022). See
    // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
    case internal::token_string_quote<typename JSON::Char>:
      if (callback) {
        CALLBACK_PRE(String, JSON::ParseContext::Root, 0, JSON::StringView{});
        const Result value{internal::parse_string(line, column, cursor, end)};
        CALLBACK_POST(String);
        return value;
      } else {
        return Result{internal::parse_string(line, column, cursor, end)};
      }
    case internal::token_array_begin<typename JSON::Char>:
      CALLBACK_PRE(Array, JSON::ParseContext::Root, 0, JSON::StringView{});
      goto do_parse_array;
    case internal::token_object_begin<typename JSON::Char>:
      CALLBACK_PRE(Object, JSON::ParseContext::Root, 0, JSON::StringView{});
      goto do_parse_object;

    case internal::token_number_minus<typename JSON::Char>:
    case internal::token_number_zero<typename JSON::Char>:
    case internal::token_number_one<typename JSON::Char>:
    case internal::token_number_two<typename JSON::Char>:
    case internal::token_number_three<typename JSON::Char>:
    case internal::token_number_four<typename JSON::Char>:
    case internal::token_number_five<typename JSON::Char>:
    case internal::token_number_six<typename JSON::Char>:
    case internal::token_number_seven<typename JSON::Char>:
    case internal::token_number_eight<typename JSON::Char>:
    case internal::token_number_nine<typename JSON::Char>:
      if (callback) {
        const auto current_line{line};
        const auto current_column{column};
        const auto value{
            internal::parse_number(line, column, cursor, end, character)};
        if (value.is_integer()) {
          CALLBACK_PRE_WITH_POSITION(Integer, current_line, current_column,
                                     JSON::ParseContext::Root, 0,
                                     JSON::StringView{});
          CALLBACK_POST(Integer);
        } else if (value.is_decimal()) {
          CALLBACK_PRE_WITH_POSITION(Decimal, current_line, current_column,
                                     JSON::ParseContext::Root, 0,
                                     JSON::StringView{});
          CALLBACK_POST(Decimal);
        } else {
          CALLBACK_PRE_WITH_POSITION(Real, current_line, current_column,
                                     JSON::ParseContext::Root, 0,
                                     JSON::StringView{});
          CALLBACK_POST(Real);
        }

        return value;
      }

      return internal::parse_number(line, column, cursor, end, character);

    default:
      throw JSONParseError(line, column);
  }

  /*
   * Parse an array
   */

do_parse_array:
  if (levels.empty()) {
    assert(!result.has_value());
    levels.push_back(Container::Array);
    result = std::make_optional<Result>(Result::make_array());
    frames.emplace_back(result.value());
  } else if (levels.back() == Container::Array) {
    assert(result.has_value());
    levels.push_back(Container::Array);
    assert(!frames.empty());
    assert(frames.back().get().is_array());
    frames.back().get().push_back(Result::make_array());
    frames.emplace_back(frames.back().get().back());
  } else if (levels.back() == Container::Object) {
    assert(result.has_value());
    levels.push_back(Container::Array);
    assert(!frames.empty());
    assert(frames.back().get().is_object());
    frames.back().get().assign(key, Result::make_array());
    frames.emplace_back(frames.back().get().at(key));
  }

  // An array structure is a pair of square bracket tokens surrounding zero or
  // more values. The values are separated by commas.
  // See
  // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf

do_parse_array_item:
  assert(levels.back() == Container::Array);
  internal::skip_whitespace(cursor, end, line, column);
  if (cursor >= end) {
    column += 1;
    goto error;
  }
  column += 1;
  character = *cursor++;
  switch (character) {
    case internal::token_array_end<typename JSON::Char>:
      if (frames.back().get().empty()) {
        CALLBACK_POST(Array);
        goto do_parse_container_end;
      } else {
        throw JSONParseError(line, column);
      }

    case internal::token_array_begin<typename JSON::Char>:
      CALLBACK_PRE(Array, JSON::ParseContext::Index, frames.back().get().size(),
                   JSON::StringView{});
      goto do_parse_array;
    case internal::token_object_begin<typename JSON::Char>:
      CALLBACK_PRE(Object, JSON::ParseContext::Index,
                   frames.back().get().size(), JSON::StringView{});
      goto do_parse_object;
    case internal::constant_true<typename JSON::Char, typename JSON::CharTraits>.front():
      CALLBACK_PRE(Boolean, JSON::ParseContext::Index,
                   frames.back().get().size(), JSON::StringView{});
      frames.back().get().push_back(
          internal::parse_boolean_true(line, column, cursor, end));
      CALLBACK_POST(Boolean);
      goto do_parse_array_item_separator;
    case internal::constant_false<typename JSON::Char, typename JSON::CharTraits>.front():
      CALLBACK_PRE(Boolean, JSON::ParseContext::Index,
                   frames.back().get().size(), JSON::StringView{});
      frames.back().get().push_back(
          internal::parse_boolean_false(line, column, cursor, end));
      CALLBACK_POST(Boolean);
      goto do_parse_array_item_separator;
    case internal::constant_null<typename JSON::Char, typename JSON::CharTraits>.front():
      CALLBACK_PRE(Null, JSON::ParseContext::Index, frames.back().get().size(),
                   JSON::StringView{});
      frames.back().get().push_back(
          internal::parse_null(line, column, cursor, end));
      CALLBACK_POST(Null);
      goto do_parse_array_item_separator;

    // A string is a sequence of Unicode code points wrapped with quotation
    // marks (U+0022). See
    // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
    case internal::token_string_quote<typename JSON::Char>:
      CALLBACK_PRE(String, JSON::ParseContext::Index,
                   frames.back().get().size(), JSON::StringView{});
      frames.back().get().push_back(
          Result{internal::parse_string(line, column, cursor, end)});
      CALLBACK_POST(String);
      goto do_parse_array_item_separator;

    case internal::token_number_minus<typename JSON::Char>:
    case internal::token_number_zero<typename JSON::Char>:
    case internal::token_number_one<typename JSON::Char>:
    case internal::token_number_two<typename JSON::Char>:
    case internal::token_number_three<typename JSON::Char>:
    case internal::token_number_four<typename JSON::Char>:
    case internal::token_number_five<typename JSON::Char>:
    case internal::token_number_six<typename JSON::Char>:
    case internal::token_number_seven<typename JSON::Char>:
    case internal::token_number_eight<typename JSON::Char>:
    case internal::token_number_nine<typename JSON::Char>:
      if (callback) {
        const auto current_line{line};
        const auto current_column{column};
        const auto current_index{frames.back().get().size()};
        const auto value{
            internal::parse_number(line, column, cursor, end, character)};
        if (value.is_integer()) {
          CALLBACK_PRE_WITH_POSITION(Integer, current_line, current_column,
                                     JSON::ParseContext::Index, current_index,
                                     JSON::StringView{});
        } else if (value.is_decimal()) {
          CALLBACK_PRE_WITH_POSITION(Decimal, current_line, current_column,
                                     JSON::ParseContext::Index, current_index,
                                     JSON::StringView{});
        } else {
          CALLBACK_PRE_WITH_POSITION(Real, current_line, current_column,
                                     JSON::ParseContext::Index, current_index,
                                     JSON::StringView{});
        }

        frames.back().get().push_back(value);

        if (value.is_integer()) {
          CALLBACK_POST(Integer);
        } else if (value.is_decimal()) {
          CALLBACK_POST(Decimal);
        } else {
          CALLBACK_POST(Real);
        }
      } else {
        frames.back().get().push_back(
            internal::parse_number(line, column, cursor, end, character));
      }

      goto do_parse_array_item_separator;

    default:
      goto error;
  }

do_parse_array_item_separator:
  assert(levels.back() == Container::Array);
  internal::skip_whitespace(cursor, end, line, column);
  if (cursor >= end) {
    column += 1;
    goto error;
  }
  column += 1;
  character = *cursor++;
  switch (character) {
    case internal::token_array_delimiter<typename JSON::Char>:
      goto do_parse_array_item;
    case internal::token_array_end<typename JSON::Char>:
      CALLBACK_POST(Array);
      goto do_parse_container_end;

    default:
      goto error;
  }

  /*
   * Parse an object
   */

do_parse_object:
  if (levels.empty()) {
    assert(levels.empty());
    assert(!result.has_value());
    levels.push_back(Container::Object);
    result = std::make_optional<Result>(Result::make_object());
    frames.emplace_back(result.value());
  } else if (levels.back() == Container::Array) {
    assert(result.has_value());
    levels.push_back(Container::Object);
    assert(!frames.empty());
    assert(frames.back().get().is_array());
    frames.back().get().push_back(Result::make_object());
    frames.emplace_back(frames.back().get().back());
  } else if (levels.back() == Container::Object) {
    assert(result.has_value());
    levels.push_back(Container::Object);
    assert(!frames.empty());
    assert(frames.back().get().is_object());
    frames.back().get().assign(key, Result::make_object());
    frames.emplace_back(frames.back().get().at(key));
  }

  frames.back().get().as_object().reserve(8);

  // An object structure is represented as a pair of curly bracket tokens
  // surrounding zero or more name/value pairs. A name is a string. A single
  // colon token follows each name, separating the name from the value. A
  // single comma token separates a value from a following name. See
  // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf

do_parse_object_property_key:
  assert(levels.back() == Container::Object);
  internal::skip_whitespace(cursor, end, line, column);
  if (cursor >= end) {
    column += 1;
    goto error;
  }
  column += 1;
  character = *cursor++;
  switch (character) {
    case internal::token_object_end<typename JSON::Char>:
      if (frames.back().get().empty()) {
        CALLBACK_POST(Object);
        goto do_parse_container_end;
      } else {
        goto error;
      }

    // A string is a sequence of Unicode code points wrapped with quotation
    // marks (U+0022). See
    // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
    case internal::token_string_quote<typename JSON::Char>:
      key_line = line;
      key_column = column;
      key = internal::parse_string(line, column, cursor, end);
      goto do_parse_object_property_separator;

    default:
      goto error;
  }

do_parse_object_property_separator:
  assert(levels.back() == Container::Object);
  internal::skip_whitespace(cursor, end, line, column);
  if (cursor >= end) {
    column += 1;
    goto error;
  }
  column += 1;
  character = *cursor++;
  switch (character) {
    case internal::token_object_key_delimiter<typename JSON::Char>:
      goto do_parse_object_property_value;

    default:
      goto error;
  }

do_parse_object_property_value:
  assert(levels.back() == Container::Object);
  internal::skip_whitespace(cursor, end, line, column);
  if (cursor >= end) {
    column += 1;
    goto error;
  }
  column += 1;
  character = *cursor++;
  switch (character) {
    case internal::token_array_begin<typename JSON::Char>:
      CALLBACK_PRE_WITH_POSITION(Array, key_line, key_column,
                                 JSON::ParseContext::Property, 0, key);
      goto do_parse_array;
    case internal::token_object_begin<typename JSON::Char>:
      CALLBACK_PRE_WITH_POSITION(Object, key_line, key_column,
                                 JSON::ParseContext::Property, 0, key);
      goto do_parse_object;
    case internal::constant_true<typename JSON::Char, typename JSON::CharTraits>.front():
      CALLBACK_PRE_WITH_POSITION(Boolean, key_line, key_column,
                                 JSON::ParseContext::Property, 0, key);
      frames.back().get().assign_assume_new(
          key, internal::parse_boolean_true(line, column, cursor, end));
      CALLBACK_POST(Boolean);
      goto do_parse_object_property_end;
    case internal::constant_false<typename JSON::Char, typename JSON::CharTraits>.front():
      CALLBACK_PRE_WITH_POSITION(Boolean, key_line, key_column,
                                 JSON::ParseContext::Property, 0, key);
      frames.back().get().assign_assume_new(
          key, internal::parse_boolean_false(line, column, cursor, end));
      CALLBACK_POST(Boolean);
      goto do_parse_object_property_end;
    case internal::constant_null<typename JSON::Char, typename JSON::CharTraits>.front():
      CALLBACK_PRE_WITH_POSITION(Null, key_line, key_column,
                                 JSON::ParseContext::Property, 0, key);
      frames.back().get().assign_assume_new(
          key, internal::parse_null(line, column, cursor, end));
      CALLBACK_POST(Null);
      goto do_parse_object_property_end;

    // A string is a sequence of Unicode code points wrapped with quotation
    // marks (U+0022). See
    // https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf
    case internal::token_string_quote<typename JSON::Char>:
      CALLBACK_PRE_WITH_POSITION(String, key_line, key_column,
                                 JSON::ParseContext::Property, 0, key);
      frames.back().get().assign_assume_new(
          key, Result{internal::parse_string(line, column, cursor, end)});
      CALLBACK_POST(String);
      goto do_parse_object_property_end;

    case internal::token_number_minus<typename JSON::Char>:
    case internal::token_number_zero<typename JSON::Char>:
    case internal::token_number_one<typename JSON::Char>:
    case internal::token_number_two<typename JSON::Char>:
    case internal::token_number_three<typename JSON::Char>:
    case internal::token_number_four<typename JSON::Char>:
    case internal::token_number_five<typename JSON::Char>:
    case internal::token_number_six<typename JSON::Char>:
    case internal::token_number_seven<typename JSON::Char>:
    case internal::token_number_eight<typename JSON::Char>:
    case internal::token_number_nine<typename JSON::Char>:
      if (callback) {
        auto value{
            internal::parse_number(line, column, cursor, end, character)};
        const auto value_type{value.type()};
        if (value_type == JSON::Type::Integer) {
          CALLBACK_PRE_WITH_POSITION(Integer, key_line, key_column,
                                     JSON::ParseContext::Property, 0, key);
        } else if (value_type == JSON::Type::Decimal) {
          CALLBACK_PRE_WITH_POSITION(Decimal, key_line, key_column,
                                     JSON::ParseContext::Property, 0, key);
        } else {
          CALLBACK_PRE_WITH_POSITION(Real, key_line, key_column,
                                     JSON::ParseContext::Property, 0, key);
        }

        frames.back().get().assign_assume_new(key, std::move(value));

        if (value_type == JSON::Type::Integer) {
          CALLBACK_POST(Integer);
        } else if (value_type == JSON::Type::Decimal) {
          CALLBACK_POST(Decimal);
        } else {
          CALLBACK_POST(Real);
        }
      } else {
        frames.back().get().assign_assume_new(
            key, internal::parse_number(line, column, cursor, end, character));
      }

      goto do_parse_object_property_end;

    default:
      goto error;
  }

do_parse_object_property_end:
  assert(levels.back() == Container::Object);
  internal::skip_whitespace(cursor, end, line, column);
  if (cursor >= end) {
    column += 1;
    goto error;
  }
  column += 1;
  character = *cursor++;
  switch (character) {
    case internal::token_object_delimiter<typename JSON::Char>:
      goto do_parse_object_property_key;
    case internal::token_object_end<typename JSON::Char>:
      CALLBACK_POST(Object);
      goto do_parse_container_end;

    default:
      goto error;
  }

  /*
   * Finish parsing a container
   */

error:
  // For some strange reason, with certain AppleClang versions,
  // the program crashes when de-allocating huge array/objects
  // before throwing an error. The error goes away if we manually
  // reset every frame of the resulting object. Compiler error?
  // Seen on Apple clang version 14.0.3 (clang-1403.0.22.14.1)
  while (!frames.empty()) {
    frames.back().get().into(Result{nullptr});
    frames.pop_back();
  }

  throw JSONParseError(line, column);

do_parse_container_end:
  assert(!levels.empty());
  if (levels.size() == 1) {
    return result.value();
  }

  frames.pop_back();
  levels.pop_back();
  if (levels.back() == Container::Array) {
    goto do_parse_array_item_separator;
  } else {
    goto do_parse_object_property_end;
  }
}

// NOLINTEND(cppcoreguidelines-avoid-goto)

inline auto internal_parse_json(
    const std::basic_string<typename JSON::Char, typename JSON::CharTraits>
        &input,
    std::uint64_t &line, std::uint64_t &column,
    const JSON::ParseCallback &callback) -> JSON {
  const char *cursor{input.data()};
  return internal_parse_json(cursor, input.data() + input.size(), line, column,
                             callback);
}

} // namespace sourcemeta::core

#undef CALLBACK_PRE
#undef CALLBACK_PRE_WITH_POSITION
#undef CALLBACK_POST

#endif
