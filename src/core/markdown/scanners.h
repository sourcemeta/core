#ifndef SOURCEMETA_CORE_MARKDOWN_SCANNERS_H_
#define SOURCEMETA_CORE_MARKDOWN_SCANNERS_H_

#include "characters.h"

#include <algorithm>   // std::binary_search
#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <string_view> // std::string_view

// Every scanner matches at the given offset of the input, never looks past
// its end, and returns the length of the match, where zero means no match

namespace sourcemeta::core::markdown {

inline auto character_at(const std::string_view input,
                         const std::size_t index) noexcept -> char {
  return index < input.size() ? input[index] : '\0';
}

// The whitespace that raw HTML allows between its parts
inline auto is_html_space(const char character) noexcept -> bool {
  return character == ' ' || character == '\t' || character == '\v' ||
         character == '\f' || character == '\r' || character == '\n';
}

// The whitespace that the table and task list extensions allow within a line
inline auto is_line_space(const char character) noexcept -> bool {
  return character == ' ' || character == '\t' || character == '\v' ||
         character == '\f';
}

inline auto to_lower_ascii(const char character) noexcept -> char {
  return character >= 'A' && character <= 'Z'
             ? static_cast<char>(character + ('a' - 'A'))
             : character;
}

inline auto starts_with_ignoring_case(const std::string_view input,
                                      const std::size_t offset,
                                      const std::string_view prefix) noexcept
    -> bool {
  if (offset > input.size() || input.size() - offset < prefix.size()) {
    return false;
  }

  for (std::size_t index{0}; index < prefix.size(); ++index) {
    if (to_lower_ascii(input[offset + index]) != prefix[index]) {
      return false;
    }
  }

  return true;
}

inline auto scan_spacechars(const std::string_view input,
                            const std::size_t offset) noexcept -> std::size_t {
  auto position{offset};
  while (position < input.size() && is_html_space(input[position])) {
    ++position;
  }

  return position - offset;
}

// An absolute URI of GFM section 6.9, right after the opening angle bracket
inline auto scan_autolink_uri(const std::string_view input,
                              const std::size_t offset) noexcept
    -> std::size_t {
  auto position{offset};
  if (!is_letter(character_at(input, position))) {
    return 0;
  }

  ++position;
  std::size_t scheme_length{0};
  while (scheme_length < 31) {
    const auto character{character_at(input, position)};
    if (!is_alphanumeric(character) && character != '.' && character != '+' &&
        character != '-') {
      break;
    }

    ++position;
    ++scheme_length;
  }

  if (scheme_length == 0 || character_at(input, position) != ':') {
    return 0;
  }

  ++position;
  while (position < input.size()) {
    const auto character{static_cast<unsigned char>(input[position])};
    if (character <= 0x20 || character == '<' || character == '>') {
      break;
    }

    ++position;
  }

  if (character_at(input, position) != '>') {
    return 0;
  }

  return position + 1 - offset;
}

inline auto is_email_local_character(const char character) noexcept -> bool {
  if (is_alphanumeric(character)) {
    return true;
  }

  switch (character) {
    case '.':
    case '!':
    case '#':
    case '$':
    case '%':
    case '&':
    case '\'':
    case '*':
    case '+':
    case '/':
    case '=':
    case '?':
    case '^':
    case '_':
    case '`':
    case '{':
    case '|':
    case '}':
    case '~':
    case '-':
      return true;
    default:
      return false;
  }
}

// An email address of GFM section 6.9, right after the opening angle bracket
inline auto scan_autolink_email(const std::string_view input,
                                const std::size_t offset) noexcept
    -> std::size_t {
  auto position{offset};
  while (position < input.size() && is_email_local_character(input[position])) {
    ++position;
  }

  if (position == offset || character_at(input, position) != '@') {
    return 0;
  }

  ++position;
  while (true) {
    if (!is_alphanumeric(character_at(input, position))) {
      return 0;
    }

    const auto label_start{position};
    ++position;
    while (position < input.size() &&
           (is_alphanumeric(input[position]) || input[position] == '-')) {
      ++position;
    }

    if (position - label_start > 63 || input[position - 1] == '-') {
      return 0;
    }

    const auto character{character_at(input, position)};
    if (character == '>') {
      return position + 1 - offset;
    }

    if (character != '.') {
      return 0;
    }

    ++position;
  }
}

inline auto scan_tag_name(const std::string_view input,
                          const std::size_t offset) noexcept -> std::size_t {
  if (!is_letter(character_at(input, offset))) {
    return 0;
  }

  auto position{offset + 1};
  while (position < input.size() &&
         (is_alphanumeric(input[position]) || input[position] == '-')) {
    ++position;
  }

  return position - offset;
}

inline auto is_attribute_name_start(const char character) noexcept -> bool {
  return is_letter(character) || character == '_' || character == ':';
}

inline auto is_attribute_name_character(const char character) noexcept -> bool {
  return is_alphanumeric(character) || character == '_' || character == ':' ||
         character == '.' || character == '-';
}

// The value of an attribute, at the equals sign or the whitespace before it
inline auto
scan_attribute_value_specification(const std::string_view input,
                                   const std::size_t offset) noexcept
    -> std::size_t {
  auto position{offset + scan_spacechars(input, offset)};
  if (character_at(input, position) != '=') {
    return 0;
  }

  ++position;
  position += scan_spacechars(input, position);
  const auto quote{character_at(input, position)};
  if (quote == '"' || quote == '\'') {
    const auto closing{input.find(quote, position + 1)};
    if (closing == std::string_view::npos) {
      return 0;
    }

    return closing + 1 - offset;
  }

  const auto value_start{position};
  while (position < input.size()) {
    const auto character{input[position]};
    if (character == ' ' || character == '\t' || character == '\r' ||
        character == '\n' || character == '\v' || character == '\f' ||
        character == '"' || character == '\'' || character == '=' ||
        character == '<' || character == '>' || character == '`') {
      break;
    }

    ++position;
  }

  if (position == value_start) {
    return 0;
  }

  return position - offset;
}

// An open tag or a closing tag of GFM section 6.6, right after the opening
// angle bracket
inline auto scan_html_tag(const std::string_view input,
                          const std::size_t offset) noexcept -> std::size_t {
  auto position{offset};
  if (character_at(input, position) == '/') {
    ++position;
    const auto name_length{scan_tag_name(input, position)};
    if (name_length == 0) {
      return 0;
    }

    position += name_length;
    position += scan_spacechars(input, position);
    if (character_at(input, position) != '>') {
      return 0;
    }

    return position + 1 - offset;
  }

  const auto name_length{scan_tag_name(input, position)};
  if (name_length == 0) {
    return 0;
  }

  position += name_length;
  while (true) {
    const auto spaces{scan_spacechars(input, position)};
    if (spaces == 0 ||
        !is_attribute_name_start(character_at(input, position + spaces))) {
      break;
    }

    auto name_end{position + spaces + 1};
    while (name_end < input.size() &&
           is_attribute_name_character(input[name_end])) {
      ++name_end;
    }

    position = name_end + scan_attribute_value_specification(input, name_end);
  }

  position += scan_spacechars(input, position);
  if (character_at(input, position) == '/') {
    ++position;
  }

  if (character_at(input, position) != '>') {
    return 0;
  }

  return position + 1 - offset;
}

// An HTML comment, at the two dashes that follow the exclamation mark
inline auto scan_html_comment(const std::string_view input,
                              const std::size_t offset) noexcept
    -> std::size_t {
  if (character_at(input, offset) != '-' ||
      character_at(input, offset + 1) != '-') {
    return 0;
  }

  std::size_t dashes{0};
  for (auto position{offset + 2}; position < input.size(); ++position) {
    const auto character{input[position]};
    if (dashes == 2) {
      if (character == '>') {
        return position + 1 - offset;
      }

      dashes = 0;
    } else if (character == '-') {
      ++dashes;
    } else {
      dashes = 0;
    }
  }

  return 0;
}

// The content of a processing instruction, right after its question mark,
// without the closing question mark and angle bracket
inline auto scan_html_processing_instruction(const std::string_view input,
                                             const std::size_t offset) noexcept
    -> std::size_t {
  auto position{offset};
  while (position < input.size()) {
    if (input[position] == '?') {
      if (position + 1 < input.size() && input[position + 1] != '>') {
        position += 2;
        continue;
      }

      break;
    }

    ++position;
  }

  return position - offset;
}

// A declaration, right after its exclamation mark, without the closing angle
// bracket
inline auto scan_html_declaration(const std::string_view input,
                                  const std::size_t offset) noexcept
    -> std::size_t {
  auto position{offset};
  while (position < input.size() && input[position] >= 'A' &&
         input[position] <= 'Z') {
    ++position;
  }

  if (position == offset) {
    return 0;
  }

  const auto spaces{scan_spacechars(input, position)};
  if (spaces == 0) {
    return 0;
  }

  const auto closing{input.find('>', position + spaces)};
  return (closing == std::string_view::npos ? input.size() : closing) - offset;
}

// A CDATA section, right after its opening bracket, without the closing
// brackets and angle bracket
inline auto scan_html_cdata(const std::string_view input,
                            const std::size_t offset) noexcept -> std::size_t {
  if (offset > input.size() ||
      input.substr(offset, 6) != std::string_view{"CDATA["}) {
    return 0;
  }

  auto boundary{offset + 6};
  std::size_t brackets{0};
  for (auto position{offset + 6}; position < input.size(); ++position) {
    const auto character{input[position]};
    if (brackets == 2) {
      if (character == '>') {
        break;
      }

      brackets = 0;
      boundary = position + 1;
    } else if (character == ']') {
      ++brackets;
    } else {
      brackets = 0;
      boundary = position + 1;
    }
  }

  return boundary - offset;
}

// The lowercase names of the HTML block condition 6 of GFM section 4.6
constexpr std::array<std::string_view, 62> BLOCK_TAG_NAMES{
    {"address",  "article",    "aside",  "base",     "basefont", "blockquote",
     "body",     "caption",    "center", "col",      "colgroup", "dd",
     "details",  "dialog",     "dir",    "div",      "dl",       "dt",
     "fieldset", "figcaption", "figure", "footer",   "form",     "frame",
     "frameset", "h1",         "h2",     "h3",       "h4",       "h5",
     "h6",       "head",       "header", "hr",       "html",     "iframe",
     "legend",   "li",         "link",   "main",     "menu",     "menuitem",
     "nav",      "noframes",   "ol",     "optgroup", "option",   "p",
     "param",    "section",    "source", "summary",  "table",    "tbody",
     "td",       "tfoot",      "th",     "thead",    "title",    "tr",
     "track",    "ul"}};

// Returns the lowercase alphanumeric run at the given offset, if it is short
// enough to be a tag name that the HTML block conditions care about
inline auto lowercase_tag_name(const std::string_view input,
                               const std::size_t offset,
                               std::array<char, 16> &buffer) noexcept
    -> std::string_view {
  std::size_t length{0};
  while (offset + length < input.size() &&
         is_alphanumeric(input[offset + length])) {
    if (length == buffer.size()) {
      return {};
    }

    buffer[length] = to_lower_ascii(input[offset + length]);
    ++length;
  }

  return {buffer.data(), length};
}

// The start condition of an HTML block of GFM section 4.6, returning the
// number of the condition from one to six or zero
inline auto scan_html_block_start(const std::string_view input,
                                  const std::size_t offset) noexcept
    -> std::size_t {
  if (character_at(input, offset) != '<') {
    return 0;
  }

  const auto next{character_at(input, offset + 1)};
  if (next == '!') {
    const auto after{character_at(input, offset + 2)};
    if (after == '-' && character_at(input, offset + 3) == '-') {
      return 2;
    }

    if (after == '[' &&
        starts_with_ignoring_case(input, offset + 3, "cdata[")) {
      return 5;
    }

    return after >= 'A' && after <= 'Z' ? 4 : 0;
  }

  if (next == '?') {
    return 3;
  }

  std::array<char, 16> buffer{};
  const auto name{lowercase_tag_name(input, offset + 1, buffer)};
  if (name == "script" || name == "pre" || name == "textarea" ||
      name == "style") {
    const auto after{character_at(input, offset + 1 + name.size())};
    return is_html_space(after) || after == '>' ? 1 : 0;
  }

  auto position{offset + 1};
  if (next == '/') {
    ++position;
  }

  const auto block_name{lowercase_tag_name(input, position, buffer)};
  if (block_name.empty() ||
      !std::binary_search(BLOCK_TAG_NAMES.cbegin(), BLOCK_TAG_NAMES.cend(),
                          block_name)) {
    return 0;
  }

  position += block_name.size();
  const auto after{character_at(input, position)};
  if (is_html_space(after) || after == '>') {
    return 6;
  }

  return after == '/' && character_at(input, position + 1) == '>' ? 6 : 0;
}

// The start condition 7 of an HTML block of GFM section 4.6
inline auto scan_html_block_start_7(const std::string_view input,
                                    const std::size_t offset) noexcept -> bool {
  if (character_at(input, offset) != '<') {
    return false;
  }

  const auto length{scan_html_tag(input, offset + 1)};
  if (length == 0) {
    return false;
  }

  auto position{offset + 1 + length};
  while (position < input.size() &&
         (input[position] == '\t' || input[position] == '\f' ||
          input[position] == ' ')) {
    ++position;
  }

  const auto character{character_at(input, position)};
  return character == '\n' || character == '\r';
}

// The end conditions of the HTML blocks of GFM section 4.6
inline auto scan_html_block_end(const std::string_view input,
                                const std::size_t offset,
                                const std::size_t condition) noexcept -> bool {
  if (offset > input.size()) {
    return false;
  }

  const auto line{input.substr(offset)};
  switch (condition) {
    case 1: {
      auto position{line.find("</")};
      while (position != std::string_view::npos) {
        std::array<char, 16> buffer{};
        const auto name{lowercase_tag_name(line, position + 2, buffer)};
        if ((name == "script" || name == "pre" || name == "textarea" ||
             name == "style") &&
            character_at(line, position + 2 + name.size()) == '>') {
          return true;
        }

        position = line.find("</", position + 1);
      }

      return false;
    }

    case 2:
      return line.find("-->") != std::string_view::npos;
    case 3:
      return line.find("?>") != std::string_view::npos;
    case 4:
      return line.find('>') != std::string_view::npos;
    case 5:
      return line.find("]]>") != std::string_view::npos;
    default:
      return false;
  }
}

// A link title of GFM section 6.3, where every closing character preceded by
// a backslash may either close the title or belong to it, and the longest
// alternative wins
inline auto scan_link_title(const std::string_view input,
                            const std::size_t offset) noexcept -> std::size_t {
  const auto opening{character_at(input, offset)};
  if (opening != '"' && opening != '\'' && opening != '(') {
    return 0;
  }

  const auto closing{opening == '(' ? ')' : opening};
  std::size_t accepted{0};
  for (auto position{offset + 1}; position < input.size(); ++position) {
    const auto character{input[position]};
    const auto escaped{position > offset + 1 && input[position - 1] == '\\'};
    if (character == closing) {
      if (!escaped) {
        return position + 1 - offset;
      }

      accepted = position + 1 - offset;
    } else if (opening == '(' && character == '(' && !escaped) {
      return accepted;
    }
  }

  return accepted;
}

// The start of an ATX heading of GFM section 4.2, including the whitespace
// after its opening sequence
inline auto scan_atx_heading_start(const std::string_view input,
                                   const std::size_t offset) noexcept
    -> std::size_t {
  auto position{offset};
  while (position < input.size() && input[position] == '#') {
    ++position;
  }

  const auto hashes{position - offset};
  if (hashes == 0 || hashes > 6) {
    return 0;
  }

  const auto character{character_at(input, position)};
  if (character == ' ' || character == '\t') {
    while (position < input.size() &&
           (input[position] == ' ' || input[position] == '\t')) {
      ++position;
    }

    return position - offset;
  }

  return character == '\n' || character == '\r' ? position + 1 - offset : 0;
}

// The underline of a setext heading of GFM section 4.3, returning the level
// of the heading or zero
inline auto scan_setext_heading_line(const std::string_view input,
                                     const std::size_t offset) noexcept
    -> std::uint8_t {
  const auto marker{character_at(input, offset)};
  if (marker != '=' && marker != '-') {
    return 0;
  }

  auto position{offset};
  while (position < input.size() && input[position] == marker) {
    ++position;
  }

  while (position < input.size() &&
         (input[position] == ' ' || input[position] == '\t')) {
    ++position;
  }

  const auto character{character_at(input, position)};
  if (character != '\n' && character != '\r') {
    return 0;
  }

  return marker == '=' ? 1 : 2;
}

// The opening fence of a fenced code block of GFM section 4.5, returning the
// length of the fence
inline auto scan_open_code_fence(const std::string_view input,
                                 const std::size_t offset) noexcept
    -> std::size_t {
  const auto marker{character_at(input, offset)};
  if (marker != '`' && marker != '~') {
    return 0;
  }

  auto position{offset};
  while (position < input.size() && input[position] == marker) {
    ++position;
  }

  const auto length{position - offset};
  if (length < 3) {
    return 0;
  }

  while (position < input.size()) {
    const auto character{input[position]};
    if (character == '\n' || character == '\r') {
      return length;
    }

    if (marker == '`' && character == '`') {
      return 0;
    }

    ++position;
  }

  return 0;
}

// The closing fence of a fenced code block of GFM section 4.5, returning the
// length of the fence
inline auto scan_close_code_fence(const std::string_view input,
                                  const std::size_t offset) noexcept
    -> std::size_t {
  const auto marker{character_at(input, offset)};
  if (marker != '`' && marker != '~') {
    return 0;
  }

  auto position{offset};
  while (position < input.size() && input[position] == marker) {
    ++position;
  }

  const auto length{position - offset};
  if (length < 3) {
    return 0;
  }

  while (position < input.size() &&
         (input[position] == ' ' || input[position] == '\t')) {
    ++position;
  }

  const auto character{character_at(input, position)};
  return character == '\n' || character == '\r' ? length : 0;
}

// Whether a link destination starts with a scheme that can run code or read
// local files, where only a few image data URIs are safe
inline auto is_dangerous_url(const std::string_view url) noexcept -> bool {
  if (starts_with_ignoring_case(url, 0, "data:image/png") ||
      starts_with_ignoring_case(url, 0, "data:image/gif") ||
      starts_with_ignoring_case(url, 0, "data:image/jpeg") ||
      starts_with_ignoring_case(url, 0, "data:image/webp")) {
    return false;
  }

  return starts_with_ignoring_case(url, 0, "javascript:") ||
         starts_with_ignoring_case(url, 0, "vbscript:") ||
         starts_with_ignoring_case(url, 0, "file:") ||
         starts_with_ignoring_case(url, 0, "data:");
}

// The start of a footnote definition, including the whitespace after its
// colon
inline auto scan_footnote_definition(const std::string_view input,
                                     const std::size_t offset) noexcept
    -> std::size_t {
  if (character_at(input, offset) != '[' ||
      character_at(input, offset + 1) != '^') {
    return 0;
  }

  auto position{offset + 2};
  while (position < input.size()) {
    const auto character{input[position]};
    if (character == ']' || character == ' ' || character == '\r' ||
        character == '\n' || character == '\t') {
      break;
    }

    ++position;
  }

  if (position == offset + 2 || character_at(input, position) != ']' ||
      character_at(input, position + 1) != ':') {
    return 0;
  }

  position += 2;
  while (position < input.size() &&
         (input[position] == ' ' || input[position] == '\t')) {
    ++position;
  }

  return position - offset;
}

inline auto scan_table_marker(const std::string_view input,
                              std::size_t position) noexcept -> std::size_t {
  const auto start{position};
  while (position < input.size() && is_line_space(input[position])) {
    ++position;
  }

  if (character_at(input, position) == ':') {
    ++position;
  }

  const auto dashes_start{position};
  while (position < input.size() && input[position] == '-') {
    ++position;
  }

  if (position == dashes_start) {
    return 0;
  }

  if (character_at(input, position) == ':') {
    ++position;
  }

  while (position < input.size() && is_line_space(input[position])) {
    ++position;
  }

  return position - start;
}

// The end of a table row, which is any whitespace and a line ending
inline auto scan_table_row_end(const std::string_view input,
                               const std::size_t offset) noexcept
    -> std::size_t {
  if (offset >= input.size()) {
    return 0;
  }

  auto position{offset};
  while (position < input.size() && is_line_space(input[position])) {
    ++position;
  }

  if (character_at(input, position) == '\r') {
    ++position;
  }

  return character_at(input, position) == '\n' ? position + 1 - offset : 0;
}

// The delimiter row of a table of GFM section 4.10
inline auto scan_table_start(const std::string_view input,
                             const std::size_t offset) noexcept -> std::size_t {
  if (offset >= input.size()) {
    return 0;
  }

  auto position{offset};
  if (input[position] == '|') {
    ++position;
  }

  const auto first{scan_table_marker(input, position)};
  if (first == 0) {
    return 0;
  }

  position += first;
  while (character_at(input, position) == '|') {
    const auto marker{scan_table_marker(input, position + 1)};
    if (marker == 0) {
      ++position;
      break;
    }

    position += 1 + marker;
  }

  const auto end{scan_table_row_end(input, position)};
  return end == 0 ? 0 : position + end - offset;
}

// The content of a table cell, which ends at a line ending or at a pipe that
// is not preceded by a backslash
inline auto scan_table_cell(const std::string_view input,
                            const std::size_t offset) noexcept -> std::size_t {
  auto position{offset};
  while (position < input.size()) {
    const auto character{input[position]};
    if (character == '\r' || character == '\n') {
      break;
    }

    if (character == '|' &&
        (position == offset || input[position - 1] != '\\')) {
      break;
    }

    ++position;
  }

  return position - offset;
}

// A pipe that ends a table cell, including the whitespace after it
inline auto scan_table_cell_end(const std::string_view input,
                                const std::size_t offset) noexcept
    -> std::size_t {
  if (character_at(input, offset) != '|') {
    return 0;
  }

  auto position{offset + 1};
  while (position < input.size() && is_line_space(input[position])) {
    ++position;
  }

  return position - offset;
}

inline auto scan_task_list_box(const std::string_view input,
                               std::size_t position) noexcept -> bool {
  const auto spaces_start{position};
  while (position < input.size() && is_line_space(input[position])) {
    ++position;
  }

  if (position == spaces_start || character_at(input, position) != '[') {
    return false;
  }

  const auto state{character_at(input, position + 1)};
  if ((state != ' ' && state != 'x' && state != 'X') ||
      character_at(input, position + 2) != ']') {
    return false;
  }

  return is_line_space(character_at(input, position + 3));
}

// A list item whose content starts with a task list item marker of GFM
// section 5.3, as a whole line
inline auto scan_task_list_item(const std::string_view input) noexcept -> bool {
  std::size_t position{0};
  while (position < input.size() && is_line_space(input[position])) {
    ++position;
  }

  const auto marker{character_at(input, position)};
  if (marker == '-' || marker == '+' || marker == '*') {
    return scan_task_list_box(input, position + 1);
  }

  if (!is_digit(marker)) {
    return false;
  }

  auto digits_end{position};
  while (digits_end < input.size() && is_digit(input[digits_end])) {
    ++digits_end;
  }

  // The marker is a run of digits followed by any single character, which may
  // also be the last digit of the run
  if (digits_end - position >= 2 && scan_task_list_box(input, digits_end)) {
    return true;
  }

  const auto character{character_at(input, digits_end)};
  if (character == '\n' || digits_end >= input.size()) {
    return false;
  }

  char32_t codepoint{0};
  const auto length{decode_utf8(input.substr(digits_end), codepoint)};
  return length > 0 && scan_task_list_box(input, digits_end + length);
}

} // namespace sourcemeta::core::markdown

#endif
