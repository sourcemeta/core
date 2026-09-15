#ifndef SOURCEMETA_CORE_MARKDOWN_RENDER_H_
#define SOURCEMETA_CORE_MARKDOWN_RENDER_H_

#include <sourcemeta/core/html.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include "characters.h"
#include "document.h"
#include "postprocess.h"
#include "scanners.h"

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core::markdown {

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

  auto render(const bool unsafe, const std::size_t capacity) -> std::string {
    this->writer_.reserve(capacity);
    this->unsafe_ = unsafe;
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
      // An image renders the text of its descendants as its alternative text
      // when entering it, so they are not visited on their own
      if (entering && !is_leaf_type(node.type) &&
          node.type != NodeType::Image) {
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
      this->writer_.close().raw("\n").close().raw("\n");
    }

    return this->writer_.take();
  }

private:
  // A link destination, which safe mode drops when its scheme can run code or
  // read local files
  auto destination(const std::string_view url) -> std::string_view {
    this->value_.clear();
    if (this->unsafe_ || !is_dangerous_url(url)) {
      sourcemeta::core::URI::escape_reference(url, this->value_);
    }

    return this->value_.view();
  }

  auto footnote_fragment(const std::string_view prefix,
                         const std::string_view label) -> std::string_view {
    this->value_.clear();
    this->value_.append(prefix);
    sourcemeta::core::URI::escape_reference(label, this->value_);
    return this->value_.view();
  }

  // The alternative text of an image, which is the text of its descendants
  // without their markup
  auto alternative_text(const std::uint32_t image) -> std::string_view {
    this->value_.clear();
    const auto &nodes{this->document_.nodes};
    auto current{nodes[image].first_child};
    while (current != NO_NODE) {
      const auto &node{nodes[current]};
      if (node.type == NodeType::Text || node.type == NodeType::Code ||
          node.type == NodeType::HTMLInline) {
        this->value_.append(node.literal);
      } else if (node.type == NodeType::SoftBreak ||
                 node.type == NodeType::LineBreak) {
        this->value_.append(' ');
      }

      current =
          node.first_child != NO_NODE
              ? node.first_child
              : next_skipping_descendants(this->document_, current, image);
    }

    return this->value_.view();
  }

  auto append_footnote_backreference(const Node &definition) -> bool {
    if (this->written_footnote_index_ >= this->footnote_index_) {
      return false;
    }

    this->written_footnote_index_ = this->footnote_index_;
    auto &writer{this->writer_};
    writer.a().attribute(
        "href", this->footnote_fragment("#fnref-", definition.literal));
    writer.attribute("class", "footnote-backref")
        .attribute("data-footnote-backref");
    sourcemeta::core::DigitsBuffer written_digits;
    const auto written_index{sourcemeta::core::digits_view(
        this->written_footnote_index_, written_digits)};
    writer.attribute("data-footnote-backref-idx", written_index);
    this->value_.clear();
    this->value_.append("Back to reference ");
    this->value_.append(written_index);
    writer.attribute("aria-label", this->value_.view());
    writer.raw("\xE2\x86\xA9").close();
    for (std::uint32_t index{2}; index <= definition.data; ++index) {
      sourcemeta::core::DigitsBuffer repeat_digits;
      const auto repeat{sourcemeta::core::digits_view(index, repeat_digits)};
      writer.raw(" ");
      this->footnote_fragment("#fnref-", definition.literal);
      this->value_.append('-');
      this->value_.append(repeat);
      writer.a().attribute("href", this->value_.view());
      writer.attribute("class", "footnote-backref")
          .attribute("data-footnote-backref");
      this->value_.clear();
      this->value_.append(written_index);
      this->value_.append('-');
      this->value_.append(repeat);
      writer.attribute("data-footnote-backref-idx", this->value_.view());
      this->value_.clear();
      this->value_.append("Back to reference ");
      this->value_.append(written_index);
      this->value_.append('-');
      this->value_.append(repeat);
      writer.attribute("aria-label", this->value_.view());
      writer.raw("\xE2\x86\xA9");
      writer.sup().attribute("class", "footnote-ref").text(repeat);
      writer.close().close();
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

  auto open_heading(const std::uint8_t level) -> void {
    switch (level) {
      case 1:
        this->writer_.h1();
        break;
      case 2:
        this->writer_.h2();
        break;
      case 3:
        this->writer_.h3();
        break;
      case 4:
        this->writer_.h4();
        break;
      case 5:
        this->writer_.h5();
        break;
      default:
        this->writer_.h6();
        break;
    }
  }

  auto render_code_block(const Node &node) -> void {
    auto &writer{this->writer_};
    writer.ensure_line_feed();
    writer.pre();
    const auto info{node.title};
    if (!info.empty()) {
      std::size_t first_tag{0};
      while (first_tag < info.size() && !is_space(info[first_tag])) {
        ++first_tag;
      }

      writer.attribute("lang", info.substr(0, first_tag));
    }

    writer.code(node.literal.data() != nullptr
                    ? node.literal
                    : this->document_.content_of(node));
    writer.close().raw("\n");
  }

  auto render_table_cell(const Node &node, const bool entering) -> void {
    auto &writer{this->writer_};
    if (!entering) {
      writer.close();
      return;
    }

    writer.ensure_line_feed();
    if (this->in_table_header_) {
      writer.th();
    } else {
      writer.td();
    }

    const auto &nodes{this->document_.nodes};
    const auto &table{nodes[nodes[node.parent].parent]};
    const auto &data{this->document_.tables[table.data]};
    switch (this->document_.alignments[data.alignments_offset + node.data]) {
      case 'l':
        writer.attribute("align", "left");
        break;
      case 'c':
        writer.attribute("align", "center");
        break;
      case 'r':
        writer.attribute("align", "right");
        break;
      default:
        break;
    }
  }

  auto render_node(const std::uint32_t index, const bool entering) -> void {
    const auto &nodes{this->document_.nodes};
    const auto &node{nodes[index]};
    auto &writer{this->writer_};
    switch (node.type) {
      case NodeType::Document:
        break;
      case NodeType::BlockQuote:
        writer.ensure_line_feed();
        if (entering) {
          writer.blockquote();
        } else {
          writer.close();
        }

        writer.raw("\n");
        break;
      case NodeType::List: {
        if (!entering) {
          writer.close().raw("\n");
          break;
        }

        const auto &data{this->document_.lists[node.data]};
        writer.ensure_line_feed();
        if (!data.ordered) {
          writer.ul();
        } else {
          writer.ol();
          if (data.start != 1) {
            sourcemeta::core::DigitsBuffer digits;
            writer.attribute("start",
                             sourcemeta::core::digits_view(data.start, digits));
          }
        }

        writer.raw("\n");
        break;
      }

      case NodeType::Item:
        if (!entering) {
          writer.close().raw("\n");
          break;
        }

        writer.ensure_line_feed();
        writer.li();
        if (has_flag(node, FLAG_TASK)) {
          writer.input().attribute("type", "checkbox");
          if (has_flag(node, FLAG_CHECKED)) {
            writer.attribute("checked", "");
          }

          writer.attribute("disabled", "").raw(" ");
        }

        break;
      case NodeType::Heading:
        if (entering) {
          writer.ensure_line_feed();
          this->open_heading(node.level);
        } else {
          writer.close().raw("\n");
        }

        break;
      case NodeType::CodeBlock:
        this->render_code_block(node);
        break;
      case NodeType::HTMLBlock:
        writer.ensure_line_feed();
        if (!this->unsafe_) {
          writer.raw("<!-- raw HTML omitted -->");
        } else {
          this->append_filtered_html(this->document_.content_of(node));
        }

        writer.ensure_line_feed();
        break;
      case NodeType::ThematicBreak:
        writer.ensure_line_feed();
        writer.hr().raw("\n");
        break;
      case NodeType::Paragraph:
        if (this->is_tight_paragraph(node)) {
          break;
        }

        if (entering) {
          writer.ensure_line_feed();
          writer.p();
          break;
        }

        if (nodes[node.parent].type == NodeType::FootnoteDefinition &&
            node.next == NO_NODE) {
          writer.raw(" ");
          this->append_footnote_backreference(nodes[node.parent]);
        }

        writer.close().raw("\n");
        break;
      case NodeType::Text:
        writer.text(node.literal);
        break;
      case NodeType::SoftBreak:
        writer.raw("\n");
        break;
      case NodeType::LineBreak:
        writer.br().raw("\n");
        break;
      case NodeType::Code:
        writer.code(node.literal);
        break;
      case NodeType::HTMLInline:
        if (!this->unsafe_) {
          writer.raw("<!-- raw HTML omitted -->");
        } else if (is_filtered_tag(node.literal)) {
          writer.raw("&lt;").raw(node.literal.substr(1));
        } else {
          writer.raw(node.literal);
        }

        break;
      case NodeType::Strong:
        if (nodes[node.parent].type != NodeType::Strong) {
          if (entering) {
            writer.strong();
          } else {
            writer.close();
          }
        }

        break;
      case NodeType::Emphasis:
        if (entering) {
          writer.em();
        } else {
          writer.close();
        }

        break;
      case NodeType::Strikethrough:
        if (entering) {
          writer.del();
        } else {
          writer.close();
        }

        break;
      case NodeType::Link:
        if (!entering) {
          writer.close();
          break;
        }

        writer.a().attribute("href", this->destination(node.literal));
        if (!node.title.empty()) {
          writer.attribute("title", node.title);
        }

        break;
      case NodeType::Image:
        writer.img().attribute("src", this->destination(node.literal));
        writer.attribute("alt", this->alternative_text(index));
        if (!node.title.empty()) {
          writer.attribute("title", node.title);
        }

        break;
      case NodeType::FootnoteDefinition:
        if (entering) {
          if (this->footnote_index_ == 0) {
            writer.section()
                .attribute("class", "footnotes")
                .attribute("data-footnotes")
                .raw("\n");
            writer.ol().raw("\n");
          }

          ++this->footnote_index_;
          writer.li()
              .attribute("id", this->footnote_fragment("fn-", node.literal))
              .raw("\n");
          break;
        }

        if (this->append_footnote_backreference(node)) {
          writer.raw("\n");
        }

        writer.close().raw("\n");
        break;
      case NodeType::FootnoteReference: {
        if (!entering) {
          break;
        }

        const auto &definition{nodes[node.data]};
        writer.sup().attribute("class", "footnote-ref");
        writer.a().attribute(
            "href", this->footnote_fragment("#fn-", definition.literal));
        this->footnote_fragment("fnref-", definition.literal);
        if (node.extra > 1) {
          sourcemeta::core::DigitsBuffer extra_digits;
          this->value_.append('-');
          this->value_.append(
              sourcemeta::core::digits_view(node.extra, extra_digits));
        }

        writer.attribute("id", this->value_.view())
            .attribute("data-footnote-ref");
        writer.text(node.literal).close().close();
        break;
      }

      case NodeType::Table:
        if (entering) {
          writer.ensure_line_feed();
          writer.table();
          this->needs_closing_table_body_ = false;
          break;
        }

        if (this->needs_closing_table_body_) {
          writer.ensure_line_feed();
          writer.close();
          writer.ensure_line_feed();
        }

        this->needs_closing_table_body_ = false;
        writer.ensure_line_feed();
        writer.close();
        writer.ensure_line_feed();
        break;
      case NodeType::TableRow:
        writer.ensure_line_feed();
        if (!entering) {
          writer.close();
          if (has_flag(node, FLAG_HEADER)) {
            writer.ensure_line_feed();
            writer.close();
            this->in_table_header_ = false;
          }

          break;
        }

        if (has_flag(node, FLAG_HEADER)) {
          this->in_table_header_ = true;
          writer.thead();
          writer.ensure_line_feed();
        } else if (!this->needs_closing_table_body_) {
          writer.tbody();
          writer.ensure_line_feed();
          this->needs_closing_table_body_ = true;
        }

        writer.tr();
        break;
      case NodeType::TableCell:
        this->render_table_cell(node, entering);
        break;
    }
  }

  auto append_filtered_html(std::string_view html) -> void {
    auto &writer{this->writer_};
    while (!html.empty()) {
      const auto bracket{html.find('<')};
      if (bracket == std::string_view::npos) {
        break;
      }

      writer.raw(html.substr(0, bracket));
      html.remove_prefix(bracket);
      writer.raw(is_filtered_tag(html) ? "&lt;" : "<");
      html.remove_prefix(1);
    }

    writer.raw(html);
  }

  const Document &document_;
  sourcemeta::core::HTMLWriter writer_;
  sourcemeta::core::HTMLBuffer value_;
  bool unsafe_{false};
  std::uint32_t footnote_index_{0};
  std::uint32_t written_footnote_index_{0};
  bool needs_closing_table_body_{false};
  bool in_table_header_{false};
};

} // namespace sourcemeta::core::markdown

#endif
