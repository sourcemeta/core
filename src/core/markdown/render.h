#ifndef SOURCEMETA_CORE_MARKDOWN_RENDER_H_
#define SOURCEMETA_CORE_MARKDOWN_RENDER_H_

#include <sourcemeta/core/text.h>

#include "characters.h"
#include "document.h"
#include "postprocess.h"
#include "scanners.h"

#include <algorithm> // std::max
#include <array>     // std::array
#include <cstddef>   // std::size_t
#include <cstdint>   // std::int64_t, std::uint8_t, std::uint32_t, std::uint64_t
#include <cstring>   // std::memcpy
#include <string>    // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core::markdown {

// Whether a byte has to be replaced in HTML text or attribute values, which
// are the quotation mark, the ampersand, and the angle brackets
constexpr std::array<std::uint8_t, 256> HTML_ESCAPED{{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x00
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10
    0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x20
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, // 0x30
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x60
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x70
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xA0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xB0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xC0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xD0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xE0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0xF0
}};

// Whether a byte can appear as it is in a link destination, which are the
// ASCII letters and digits plus the reserved and unreserved characters that
// need no HTML escaping
constexpr std::array<std::uint8_t, 256> HREF_SAFE{{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x00
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10
    0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, // 0x20
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, // 0x30
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0x40
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, // 0x50
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0x60
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, // 0x70
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xA0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xB0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xC0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xD0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xE0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0xF0
}};

constexpr std::string_view HEXADECIMAL_DIGITS{"0123456789ABCDEF"};
constexpr std::uint64_t BYTE_ONES{0x0101010101010101ULL};
constexpr std::uint64_t BYTE_HIGH_BITS{0x8080808080808080ULL};

// Whether any of the eight bytes of a word is the given byte
inline auto has_byte(const std::uint64_t word, const std::uint8_t byte) noexcept
    -> bool {
  const auto difference{word ^ (BYTE_ONES * byte)};
  return ((difference - BYTE_ONES) & ~difference & BYTE_HIGH_BITS) != 0;
}

// An append only buffer that writes straight into the memory of a string and
// grows it geometrically, so that appending does not call into the standard
// library every time
class OutputBuffer {
public:
  auto start(std::string &target, const std::size_t capacity) -> void {
    this->target_ = &target;
    this->resize(std::max(capacity, MINIMUM_CAPACITY), 0);
  }

  auto finish() -> void { this->target_->resize(this->size()); }

  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return static_cast<std::size_t>(this->cursor_ - this->begin_);
  }

  [[nodiscard]] auto empty() const noexcept -> bool {
    return this->cursor_ == this->begin_;
  }

  [[nodiscard]] auto back() const noexcept -> char {
    return *(this->cursor_ - 1);
  }

  // Make room for the given number of bytes, so that the next writes of up
  // to that many bytes need no capacity checks
  auto reserve(const std::size_t count) -> void {
    if (this->capacity() - this->size() < count) [[unlikely]] {
      const auto used{this->size()};
      this->resize(std::max(this->capacity() * 2, used + count), used);
    }
  }

  auto append(const std::string_view value) -> void {
    this->reserve(value.size());
    this->write(value);
  }

  auto push_back(const char character) -> void {
    this->reserve(1);
    this->write(character);
  }

  auto append_number(const std::int64_t number) -> void {
    sourcemeta::core::DigitsBuffer digits;
    this->append(sourcemeta::core::digits_view(number, digits));
  }

  auto write(const std::string_view value) noexcept -> void {
    if (!value.empty()) {
      std::memcpy(this->cursor_, value.data(), value.size());
      this->cursor_ += value.size();
    }
  }

  auto write(const char character) noexcept -> void {
    *this->cursor_ = character;
    ++this->cursor_;
  }

private:
  static constexpr std::size_t MINIMUM_CAPACITY{256};

  [[nodiscard]] auto capacity() const noexcept -> std::size_t {
    return static_cast<std::size_t>(this->end_ - this->begin_);
  }

  auto resize(const std::size_t capacity, const std::size_t used) -> void {
    this->target_->resize_and_overwrite(
        capacity, [](const char *, const std::size_t size) { return size; });
    this->begin_ = this->target_->data();
    this->cursor_ = this->begin_ + used;
    this->end_ = this->begin_ + capacity;
  }

  std::string *target_{nullptr};
  char *begin_{nullptr};
  char *cursor_{nullptr};
  char *end_{nullptr};
};

inline auto append_escaped_html(OutputBuffer &output,
                                const std::string_view input) -> void {
  if (input.empty()) {
    return;
  }

  // Every byte takes at most six bytes once escaped
  output.reserve(input.size() * 6);
  const auto *const data{input.data()};
  const auto size{input.size()};
  std::size_t run_start{0};
  std::size_t index{0};
  while (index < size) {
    if (index + 8 <= size) {
      std::uint64_t word{0};
      std::memcpy(&word, data + index, 8);
      if (!has_byte(word, '&') && !has_byte(word, '<') &&
          !has_byte(word, '>') && !has_byte(word, '"')) {
        index += 8;
        continue;
      }
    }

    const auto byte{static_cast<unsigned char>(data[index])};
    if (HTML_ESCAPED[byte] == 0) {
      ++index;
      continue;
    }

    output.write(input.substr(run_start, index - run_start));
    switch (byte) {
      case '&':
        output.write("&amp;");
        break;
      case '<':
        output.write("&lt;");
        break;
      case '>':
        output.write("&gt;");
        break;
      default:
        output.write("&quot;");
        break;
    }

    ++index;
    run_start = index;
  }

  output.write(input.substr(run_start));
}

inline auto append_escaped_href(OutputBuffer &output,
                                const std::string_view input) -> void {
  if (input.empty()) {
    return;
  }

  // Every byte takes at most six bytes once escaped
  output.reserve(input.size() * 6);
  const auto size{input.size()};
  std::size_t run_start{0};
  for (std::size_t index{0}; index < size; ++index) {
    const auto byte{static_cast<unsigned char>(input[index])};
    if (HREF_SAFE[byte] != 0) [[likely]] {
      continue;
    }

    output.write(input.substr(run_start, index - run_start));
    if (byte == '&') {
      output.write("&amp;");
    } else if (byte == '\'') {
      output.write("&#x27;");
    } else {
      output.write('%');
      output.write(HEXADECIMAL_DIGITS[byte >> 4U]);
      output.write(HEXADECIMAL_DIGITS[byte & 0x0FU]);
    }

    run_start = index + 1;
  }

  output.write(input.substr(run_start));
}

// The tags that the GFM tag filter of section 6.11 disallows
constexpr std::array<std::string_view, 9> FILTERED_TAG_NAMES{
    {"title", "textarea", "style", "xmp", "iframe", "noembed", "noframes",
     "script", "plaintext"}};

// Whether raw HTML starts with a tag that the tag filter disallows
inline auto is_filtered_tag(const std::string_view tag) noexcept -> bool {
  if (tag.size() < 3 || tag[0] != '<') {
    return false;
  }

  for (const auto name : FILTERED_TAG_NAMES) {
    std::size_t index{tag[1] == '/' ? 2U : 1U};
    std::size_t name_index{0};
    while (index < tag.size() && name_index < name.size() &&
           sourcemeta::core::to_lowercase(tag[index]) == name[name_index]) {
      ++index;
      ++name_index;
    }

    if (name_index < name.size() || index == tag.size()) {
      continue;
    }

    const auto character{tag[index]};
    if (is_space(character) || character == '>' ||
        (character == '/' && tag.size() >= index + 2 &&
         tag[index + 1] == '>')) {
      return true;
    }
  }

  return false;
}

class HTMLRenderer {
public:
  explicit HTMLRenderer(const Document &document) : document_{document} {}

  auto render(std::string &output, const bool unsafe,
              const std::size_t capacity) -> void {
    this->output_.start(output, capacity);
    this->unsafe_ = unsafe;
    this->plain_ = NO_NODE;
    this->footnote_index_ = 0;
    this->written_footnote_index_ = 0;
    this->needs_closing_table_body_ = false;
    this->in_table_header_ = false;
    const auto &nodes{this->document_.nodes};
    auto current{ROOT_NODE};
    bool entering{true};
    while (true) {
      this->render_node(current, entering);
      const auto &node{nodes[current]};
      if (entering && !is_leaf_type(node.type)) {
        if (node.first_child != NO_NODE) {
          current = node.first_child;
        } else {
          entering = false;
        }

        continue;
      }

      if (current == ROOT_NODE) {
        break;
      }

      entering = node.next != NO_NODE;
      current = entering ? node.next : node.parent;
    }

    if (this->footnote_index_ > 0) {
      this->output_.append("</ol>\n</section>\n");
    }

    this->output_.finish();
  }

private:
  auto line_break() -> void {
    if (!this->output_.empty() && this->output_.back() != '\n') {
      this->output_.push_back('\n');
    }
  }

  auto append_destination(const std::string_view url) -> void {
    if (this->unsafe_ || !is_dangerous_url(url)) {
      append_escaped_href(this->output_, url);
    }
  }

  auto append_footnote_backreference(const Node &definition) -> bool {
    if (this->written_footnote_index_ >= this->footnote_index_) {
      return false;
    }

    this->written_footnote_index_ = this->footnote_index_;
    auto &output{this->output_};
    output.append("<a href=\"#fnref-");
    append_escaped_href(output, definition.literal);
    output.append("\" class=\"footnote-backref\" data-footnote-backref "
                  "data-footnote-backref-idx=\"");
    output.append_number(this->written_footnote_index_);
    output.append("\" aria-label=\"Back to reference ");
    output.append_number(this->written_footnote_index_);
    output.append("\">\xE2\x86\xA9</a>");
    for (std::uint32_t index{2}; index <= definition.data; ++index) {
      output.append(" <a href=\"#fnref-");
      append_escaped_href(output, definition.literal);
      output.push_back('-');
      output.append_number(index);
      output.append("\" class=\"footnote-backref\" data-footnote-backref "
                    "data-footnote-backref-idx=\"");
      output.append_number(this->written_footnote_index_);
      output.push_back('-');
      output.append_number(index);
      output.append("\" aria-label=\"Back to reference ");
      output.append_number(this->written_footnote_index_);
      output.push_back('-');
      output.append_number(index);
      output.append("\">\xE2\x86\xA9<sup class=\"footnote-ref\">");
      output.append_number(index);
      output.append("</sup></a>");
    }

    return true;
  }

  [[nodiscard]] auto is_tight_paragraph(const Node &paragraph) const noexcept
      -> bool {
    const auto &nodes{this->document_.nodes};
    const auto grandparent{nodes[paragraph.parent].parent};
    return grandparent != NO_NODE &&
           nodes[grandparent].type == NodeType::List &&
           has_flag(nodes[grandparent], FLAG_TIGHT);
  }

  auto render_plain(const Node &node) -> void {
    switch (node.type) {
      case NodeType::Text:
      case NodeType::Code:
      case NodeType::HTMLInline:
        append_escaped_html(this->output_, node.literal);
        break;
      case NodeType::LineBreak:
      case NodeType::SoftBreak:
        this->output_.push_back(' ');
        break;
      case NodeType::Document:
      case NodeType::BlockQuote:
      case NodeType::List:
      case NodeType::Item:
      case NodeType::CodeBlock:
      case NodeType::HTMLBlock:
      case NodeType::Paragraph:
      case NodeType::Heading:
      case NodeType::ThematicBreak:
      case NodeType::FootnoteDefinition:
      case NodeType::Table:
      case NodeType::TableRow:
      case NodeType::TableCell:
      case NodeType::Emphasis:
      case NodeType::Strong:
      case NodeType::Link:
      case NodeType::Image:
      case NodeType::FootnoteReference:
      case NodeType::Strikethrough:
        break;
    }
  }

  auto render_code_block(const Node &node) -> void {
    auto &output{this->output_};
    this->line_break();
    const auto info{node.title};
    if (info.empty()) {
      output.append("<pre><code>");
    } else {
      std::size_t first_tag{0};
      while (first_tag < info.size() && !is_space(info[first_tag])) {
        ++first_tag;
      }

      output.append("<pre lang=\"");
      append_escaped_html(output, info.substr(0, first_tag));
      output.append("\"><code>");
    }

    append_escaped_html(output, node.literal.data() != nullptr
                                    ? node.literal
                                    : this->document_.content_of(node));
    output.append("</code></pre>\n");
  }

  auto render_table_cell(const Node &node, const bool entering) -> void {
    auto &output{this->output_};
    if (!entering) {
      output.append(this->in_table_header_ ? "</th>" : "</td>");
      return;
    }

    this->line_break();
    output.append(this->in_table_header_ ? "<th" : "<td");
    const auto &nodes{this->document_.nodes};
    const auto &table{nodes[nodes[node.parent].parent]};
    const auto &data{this->document_.tables[table.data]};
    switch (this->document_.alignments[data.alignments_offset + node.data]) {
      case 'l':
        output.append(" align=\"left\"");
        break;
      case 'c':
        output.append(" align=\"center\"");
        break;
      case 'r':
        output.append(" align=\"right\"");
        break;
      default:
        break;
    }

    output.push_back('>');
  }

  auto render_node(const std::uint32_t index, const bool entering) -> void {
    const auto &nodes{this->document_.nodes};
    const auto &node{nodes[index]};
    if (this->plain_ == index) {
      this->plain_ = NO_NODE;
    }

    if (this->plain_ != NO_NODE) {
      this->render_plain(node);
      return;
    }

    auto &output{this->output_};
    switch (node.type) {
      case NodeType::Document:
        break;
      case NodeType::BlockQuote:
        this->line_break();
        output.append(entering ? "<blockquote>\n" : "</blockquote>\n");
        break;
      case NodeType::List: {
        const auto &data{this->document_.lists[node.data]};
        if (!entering) {
          output.append(data.ordered ? "</ol>\n" : "</ul>\n");
        } else if (!data.ordered) {
          this->line_break();
          output.append("<ul>\n");
        } else if (data.start == 1) {
          this->line_break();
          output.append("<ol>\n");
        } else {
          this->line_break();
          output.append("<ol start=\"");
          output.append_number(data.start);
          output.append("\">\n");
        }

        break;
      }

      case NodeType::Item:
        if (!entering) {
          output.append("</li>\n");
        } else if (!has_flag(node, FLAG_TASK)) {
          this->line_break();
          output.append("<li>");
        } else {
          this->line_break();
          output.append(
              has_flag(node, FLAG_CHECKED)
                  ? R"(<li><input type="checkbox" checked="" disabled="" /> )"
                  : R"(<li><input type="checkbox" disabled="" /> )");
        }

        break;
      case NodeType::Heading:
        if (entering) {
          this->line_break();
          output.append("<h");
          output.push_back(static_cast<char>('0' + node.level));
          output.push_back('>');
        } else {
          output.append("</h");
          output.push_back(static_cast<char>('0' + node.level));
          output.append(">\n");
        }

        break;
      case NodeType::CodeBlock:
        this->render_code_block(node);
        break;
      case NodeType::HTMLBlock:
        this->line_break();
        if (!this->unsafe_) {
          output.append("<!-- raw HTML omitted -->");
        } else {
          this->append_filtered_html(this->document_.content_of(node));
        }

        this->line_break();
        break;
      case NodeType::ThematicBreak:
        this->line_break();
        output.append("<hr />\n");
        break;
      case NodeType::Paragraph:
        if (this->is_tight_paragraph(node)) {
          break;
        }

        if (entering) {
          this->line_break();
          output.append("<p>");
          break;
        }

        if (nodes[node.parent].type == NodeType::FootnoteDefinition &&
            node.next == NO_NODE) {
          output.push_back(' ');
          this->append_footnote_backreference(nodes[node.parent]);
        }

        output.append("</p>\n");
        break;
      case NodeType::Text:
        append_escaped_html(output, node.literal);
        break;
      case NodeType::SoftBreak:
        output.push_back('\n');
        break;
      case NodeType::LineBreak:
        output.append("<br />\n");
        break;
      case NodeType::Code:
        output.append("<code>");
        append_escaped_html(output, node.literal);
        output.append("</code>");
        break;
      case NodeType::HTMLInline:
        if (!this->unsafe_) {
          output.append("<!-- raw HTML omitted -->");
        } else if (is_filtered_tag(node.literal)) {
          output.append("&lt;");
          output.append(node.literal.substr(1));
        } else {
          output.append(node.literal);
        }

        break;
      case NodeType::Strong:
        if (nodes[node.parent].type != NodeType::Strong) {
          output.append(entering ? "<strong>" : "</strong>");
        }

        break;
      case NodeType::Emphasis:
        output.append(entering ? "<em>" : "</em>");
        break;
      case NodeType::Strikethrough:
        output.append(entering ? "<del>" : "</del>");
        break;
      case NodeType::Link:
        if (!entering) {
          output.append("</a>");
          break;
        }

        output.append("<a href=\"");
        this->append_destination(node.literal);
        if (!node.title.empty()) {
          output.append("\" title=\"");
          append_escaped_html(output, node.title);
        }

        output.append("\">");
        break;
      case NodeType::Image:
        if (entering) {
          output.append("<img src=\"");
          this->append_destination(node.literal);
          output.append("\" alt=\"");
          this->plain_ = index;
          break;
        }

        if (!node.title.empty()) {
          output.append("\" title=\"");
          append_escaped_html(output, node.title);
        }

        output.append("\" />");
        break;
      case NodeType::FootnoteDefinition:
        if (entering) {
          if (this->footnote_index_ == 0) {
            output.append(
                "<section class=\"footnotes\" data-footnotes>\n<ol>\n");
          }

          ++this->footnote_index_;
          output.append("<li id=\"fn-");
          append_escaped_href(output, node.literal);
          output.append("\">\n");
          break;
        }

        if (this->append_footnote_backreference(node)) {
          output.push_back('\n');
        }

        output.append("</li>\n");
        break;
      case NodeType::FootnoteReference: {
        if (!entering) {
          break;
        }

        const auto &definition{nodes[node.data]};
        output.append(R"(<sup class="footnote-ref"><a href="#fn-)");
        append_escaped_href(output, definition.literal);
        output.append("\" id=\"fnref-");
        append_escaped_href(output, definition.literal);
        if (node.extra > 1) {
          output.push_back('-');
          output.append_number(node.extra);
        }

        output.append(R"(" data-footnote-ref>)");
        append_escaped_href(output, node.literal);
        output.append("</a></sup>");
        break;
      }

      case NodeType::Table:
        if (entering) {
          this->line_break();
          output.append("<table>");
          this->needs_closing_table_body_ = false;
          break;
        }

        if (this->needs_closing_table_body_) {
          this->line_break();
          output.append("</tbody>");
          this->line_break();
        }

        this->needs_closing_table_body_ = false;
        this->line_break();
        output.append("</table>");
        this->line_break();
        break;
      case NodeType::TableRow:
        this->line_break();
        if (!entering) {
          output.append("</tr>");
          if (has_flag(node, FLAG_HEADER)) {
            this->line_break();
            output.append("</thead>");
            this->in_table_header_ = false;
          }

          break;
        }

        if (has_flag(node, FLAG_HEADER)) {
          this->in_table_header_ = true;
          output.append("<thead>");
          this->line_break();
        } else if (!this->needs_closing_table_body_) {
          output.append("<tbody>");
          this->line_break();
          this->needs_closing_table_body_ = true;
        }

        output.append("<tr>");
        break;
      case NodeType::TableCell:
        this->render_table_cell(node, entering);
        break;
    }
  }

  auto append_filtered_html(std::string_view html) -> void {
    auto &output{this->output_};
    while (!html.empty()) {
      const auto bracket{html.find('<')};
      if (bracket == std::string_view::npos) {
        break;
      }

      output.append(html.substr(0, bracket));
      html.remove_prefix(bracket);
      output.append(is_filtered_tag(html) ? "&lt;" : "<");
      html.remove_prefix(1);
    }

    output.append(html);
  }

  const Document &document_;
  OutputBuffer output_;
  bool unsafe_{false};
  std::uint32_t plain_{NO_NODE};
  std::uint32_t footnote_index_{0};
  std::uint32_t written_footnote_index_{0};
  bool needs_closing_table_body_{false};
  bool in_table_header_{false};
};

} // namespace sourcemeta::core::markdown

#endif
