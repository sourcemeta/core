#ifndef SOURCEMETA_CORE_MARKDOWN_CHARACTERS_H_
#define SOURCEMETA_CORE_MARKDOWN_CHARACTERS_H_

#include <sourcemeta/core/html_entity.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/unicode.h>

#include <algorithm>   // std::min
#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core::markdown {

// The ASCII whitespace that the parser trims is only the tab, the line feed,
// the carriage return, and the space
inline auto is_space(const char character) noexcept -> bool {
  return character == ' ' || character == '\t' || character == '\n' ||
         character == '\r';
}

// The ASCII punctuation characters of GFM section 2.1
inline auto is_punctuation(const char character) noexcept -> bool {
  return (character >= '!' && character <= '/') ||
         (character >= ':' && character <= '@') ||
         (character >= '[' && character <= '`') ||
         (character >= '{' && character <= '~');
}

inline auto is_space_or_tab(const char character) noexcept -> bool {
  return character == ' ' || character == '\t';
}

inline auto is_line_end(const char character) noexcept -> bool {
  return character == '\n' || character == '\r';
}

// The Unicode whitespace of GFM section 2.1
inline auto is_unicode_whitespace(const char32_t codepoint) noexcept -> bool {
  if (codepoint < 0x80) {
    return codepoint == U'\t' || codepoint == U'\n' || codepoint == U'\f' ||
           codepoint == U'\r' || codepoint == U' ';
  }

  return sourcemeta::core::general_category(codepoint) ==
         sourcemeta::core::GeneralCategory::SpaceSeparator;
}

// The punctuation of GFM section 2.1
inline auto is_unicode_punctuation(const char32_t codepoint) noexcept -> bool {
  if (codepoint < 0x80) {
    return is_punctuation(static_cast<char>(codepoint));
  }

  const auto category{sourcemeta::core::general_category(codepoint)};
  return category == sourcemeta::core::GeneralCategory::ConnectorPunctuation ||
         category == sourcemeta::core::GeneralCategory::DashPunctuation ||
         category == sourcemeta::core::GeneralCategory::OpenPunctuation ||
         category == sourcemeta::core::GeneralCategory::ClosePunctuation ||
         category == sourcemeta::core::GeneralCategory::InitialPunctuation ||
         category == sourcemeta::core::GeneralCategory::FinalPunctuation ||
         category == sourcemeta::core::GeneralCategory::OtherPunctuation;
}

inline auto trim_left(std::string_view input) noexcept -> std::string_view {
  std::size_t index{0};
  while (index < input.size() && is_space(input[index])) {
    ++index;
  }

  input.remove_prefix(index);
  return input;
}

inline auto trim_right(std::string_view input) noexcept -> std::string_view {
  std::size_t size{input.size()};
  while (size > 0 && is_space(input[size - 1])) {
    --size;
  }

  return input.substr(0, size);
}

inline auto trim(const std::string_view input) noexcept -> std::string_view {
  return trim_right(trim_left(input));
}

// Decode the character reference that follows an ampersand as GFM section 2.5
// describes, appending its characters and returning the number of bytes it
// takes after the ampersand, or zero if the input does not start with one
inline auto decode_character_reference(std::string &output,
                                       const std::string_view input)
    -> std::size_t {
  const auto size{input.size()};
  if (size >= 3 && input[0] == '#') {
    char32_t codepoint{0};
    std::size_t index{0};
    std::size_t digits{0};
    if (sourcemeta::core::is_digit(input[1])) {
      for (index = 1; index < size && sourcemeta::core::is_digit(input[index]);
           ++index) {
        codepoint =
            (codepoint * 10) + static_cast<char32_t>(input[index] - '0');
        codepoint = std::min(codepoint, char32_t{0x110000});
      }

      digits = index - 1;
    } else if (input[1] == 'x' || input[1] == 'X') {
      for (index = 2; index < size; ++index) {
        const auto value{sourcemeta::core::hex_digit_value(input[index])};
        if (value < 0) {
          break;
        }

        codepoint = (codepoint * 16) + static_cast<char32_t>(value);
        codepoint = std::min(codepoint, char32_t{0x110000});
      }

      digits = index - 2;
    }

    if (digits >= 1 && digits <= 8 && index < size && input[index] == ';') {
      if (codepoint == 0 || !sourcemeta::core::is_valid_codepoint(codepoint)) {
        codepoint = 0xFFFD;
      }

      sourcemeta::core::codepoint_to_utf8(codepoint, output);
      return index + 1;
    }

    return 0;
  }

  // The longest name is 31 characters long, so there is no point in looking
  // for a semicolon any further
  const auto limit{size > 32 ? std::size_t{32} : size};
  for (std::size_t index{2}; index < limit; ++index) {
    if (input[index] == ' ') {
      break;
    }

    if (input[index] == ';') {
      const auto characters{
          sourcemeta::core::html_entity(input.substr(0, index + 1))};
      if (!characters.empty()) {
        output.append(characters);
        return index + 1;
      }

      break;
    }
  }

  return 0;
}

// Replace every character reference of the input, returning whether the input
// had any ampersand at all, in which case the output holds the result
inline auto decode_character_references(std::string &output,
                                        const std::string_view input) -> bool {
  auto ampersand{input.find('&')};
  if (ampersand == std::string_view::npos) {
    return false;
  }

  std::size_t index{0};
  while (ampersand != std::string_view::npos) {
    output.append(input.substr(index, ampersand - index));
    index = ampersand + 1;
    const auto consumed{
        decode_character_reference(output, input.substr(index))};
    if (consumed == 0) {
      output.push_back('&');
    } else {
      index += consumed;
    }

    ampersand = input.find('&', index);
  }

  output.append(input.substr(index));
  return true;
}

// Remove the backslash of every backslash escape as GFM section 2.4 describes
inline auto remove_backslash_escapes(std::string &buffer) -> void {
  const auto size{buffer.size()};
  std::size_t write{0};
  for (std::size_t read{0}; read < size; ++read) {
    if (buffer[read] == '\\' && read + 1 < size &&
        is_punctuation(buffer[read + 1])) {
      ++read;
    }

    buffer[write] = buffer[read];
    ++write;
  }

  buffer.resize(write);
}

// Normalise a link label as GFM section 4.7 describes, folding its case,
// trimming it, and collapsing every run of whitespace into a single space
inline auto normalize_label(std::string &output, const std::string_view label)
    -> void {
  output.clear();
  bool pending_space{false};
  std::size_t index{0};
  while (index < label.size()) {
    const auto byte{static_cast<unsigned char>(label[index])};
    if (byte < 0x80) {
      if (is_space(label[index])) {
        pending_space = !output.empty();
        ++index;
        continue;
      }

      if (pending_space) {
        output.push_back(' ');
        pending_space = false;
      }

      output.push_back(sourcemeta::core::to_lowercase(label[index]));
      ++index;
      continue;
    }

    if (pending_space) {
      output.push_back(' ');
      pending_space = false;
    }

    const auto decoded{sourcemeta::core::utf8_decode(label, index)};
    if (!decoded.has_value()) {
      sourcemeta::core::codepoint_to_utf8(0xFFFD, output);
      ++index;
      continue;
    }

    const auto [codepoint, length]{decoded.value()};
    const auto folded{sourcemeta::core::case_fold(codepoint)};
    if (folded.empty()) {
      output.append(label.substr(index, length));
    } else {
      for (const auto character : folded) {
        sourcemeta::core::codepoint_to_utf8(character, output);
      }
    }

    index += length;
  }
}

} // namespace sourcemeta::core::markdown

#endif
