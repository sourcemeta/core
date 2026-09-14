#ifndef SOURCEMETA_CORE_MARKDOWN_POSTPROCESS_H_
#define SOURCEMETA_CORE_MARKDOWN_POSTPROCESS_H_

#include "characters.h"
#include "document.h"
#include "inlines.h"
#include "references.h"

#include <algorithm>     // std::sort
#include <array>         // std::array
#include <charconv>      // std::to_chars
#include <cstddef>       // std::size_t
#include <cstdint>       // std::uint32_t
#include <cstring>       // std::memchr
#include <functional>    // std::less
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

namespace sourcemeta::core::markdown {

inline auto is_leaf_type(const NodeType type) noexcept -> bool {
  return type == NodeType::HTMLBlock || type == NodeType::ThematicBreak ||
         type == NodeType::CodeBlock || type == NodeType::Text ||
         type == NodeType::SoftBreak || type == NodeType::LineBreak ||
         type == NodeType::Code || type == NodeType::HTMLInline;
}

// The node after the given one in document order when skipping its
// descendants, without leaving the subtree of the given root, or no node at
// the end of that subtree
inline auto next_skipping_descendants(const Document &document,
                                      std::uint32_t index,
                                      const std::uint32_t root) noexcept
    -> std::uint32_t {
  while (index != root && index != NO_NODE) {
    const auto &current{document.nodes[index]};
    if (current.next != NO_NODE) {
      return current.next;
    }

    index = current.parent;
  }

  return NO_NODE;
}

// Whether a node is still part of the document
inline auto is_attached(const Document &document, std::uint32_t index) noexcept
    -> bool {
  while (index != ROOT_NODE && index != NO_NODE) {
    index = document.nodes[index].parent;
  }

  return index == ROOT_NODE;
}

// The passes that run on the block structure once it is complete
class PostProcessor {
public:
  explicit PostProcessor(Document &document) : document_{document} {}

  // Parse the inlines of every leaf block in document order, which is the
  // order in which references count towards their expansion limit. Blocks
  // without footnote references are finished right away, while their nodes
  // are still in the cache, and the rest wait for footnotes to resolve
  auto parse_inlines(InlineParser &parser) -> void {
    this->deferred_.clear();
    auto current{this->document_.nodes[ROOT_NODE].first_child};
    while (current != NO_NODE) {
      const auto type{this->document_.nodes[current].type};
      if (type == NodeType::Paragraph || type == NodeType::Heading ||
          type == NodeType::TableCell) {
        const auto references{this->document_.footnote_reference_nodes.size()};
        parser.parse(current);
        if (this->document_.footnote_reference_nodes.size() == references) {
          this->finish_inlines(current);
        } else {
          this->deferred_.push_back(current);
        }

        current =
            next_skipping_descendants(this->document_, current, ROOT_NODE);
      } else if (this->document_.nodes[current].first_child != NO_NODE) {
        current = this->document_.nodes[current].first_child;
      } else {
        current =
            next_skipping_descendants(this->document_, current, ROOT_NODE);
      }
    }
  }

  // Number footnote definitions in the order in which they are first
  // referenced and move them to the end of the document, dropping those that
  // are never referenced or that repeat a label
  auto process_footnotes() -> void {
    const auto &definitions{this->document_.footnote_definition_nodes};
    const auto &references{this->document_.footnote_reference_nodes};
    if (definitions.empty() && references.empty()) {
      return;
    }

    auto &nodes{this->document_.nodes};
    this->definitions_.clear();
    this->definition_nodes_.clear();
    // Without nesting, the order in which definitions were opened matches the
    // order in which they end, which is the order in which labels claim them
    if (this->document_.nested_footnote_definitions) {
      this->register_definitions_in_post_order();
    } else {
      for (const auto definition : definitions) {
        this->register_definition(definition);
      }
    }

    std::uint32_t last_index{0};
    for (const auto reference : references) {
      if (is_attached(this->document_, reference)) {
        this->resolve_reference(reference, last_index);
      }
    }

    this->ordered_.clear();
    for (const auto &entry : this->definitions_) {
      this->ordered_.push_back(entry.second);
    }

    std::sort(this->ordered_.begin(), this->ordered_.end(),
              [&nodes](const std::uint32_t left, const std::uint32_t right) {
                return nodes[left].extra < nodes[right].extra;
              });
    for (const auto definition : this->ordered_) {
      this->document_.unlink(definition);
      if (nodes[definition].extra > 0) {
        set_flag(nodes[definition], FLAG_REFERENCED, true);
        this->document_.append_child(ROOT_NODE, definition);
      }
    }

    for (const auto definition : this->definition_nodes_) {
      if (!has_flag(nodes[definition], FLAG_REFERENCED)) {
        this->document_.unlink(definition);
      }
    }

    for (const auto block : this->deferred_) {
      this->finish_inlines(block);
    }
  }

private:
  // Merge the adjacent text nodes of a leaf block and turn the email
  // addresses of GFM section 6.9 that appear outside of links into links
  auto finish_inlines(const std::uint32_t block) -> void {
    auto current{this->document_.nodes[block].first_child};
    while (current != NO_NODE) {
      if (this->document_.nodes[current].type == NodeType::Text) {
        this->merge_following_text(current);
      }

      if (this->document_.nodes[current].first_child != NO_NODE) {
        current = this->document_.nodes[current].first_child;
      } else {
        current = next_skipping_descendants(this->document_, current, block);
      }
    }

    // Only text with an at sign, which a character reference may produce, can
    // have email addresses
    const auto content{
        this->document_.content_of(this->document_.nodes[block])};
    if (content.empty() ||
        (std::memchr(content.data(), '@', content.size()) == nullptr &&
         std::memchr(content.data(), '&', content.size()) == nullptr)) {
      return;
    }

    current = this->document_.nodes[block].first_child;
    while (current != NO_NODE) {
      const auto type{this->document_.nodes[current].type};
      if (type == NodeType::Text) {
        const auto next{
            next_skipping_descendants(this->document_, current, block)};
        this->link_emails_in_text(current);
        current = next;
      } else if (type != NodeType::Link &&
                 this->document_.nodes[current].first_child != NO_NODE) {
        current = this->document_.nodes[current].first_child;
      } else {
        current = next_skipping_descendants(this->document_, current, block);
      }
    }
  }

  auto register_definitions_in_post_order() -> void {
    const auto &nodes{this->document_.nodes};
    auto current{ROOT_NODE};
    bool entering{true};
    while (true) {
      if (entering && !is_leaf_type(nodes[current].type) &&
          nodes[current].first_child != NO_NODE) {
        current = nodes[current].first_child;
        continue;
      }

      if (nodes[current].type == NodeType::FootnoteDefinition) {
        this->register_definition(current);
      }

      if (current == ROOT_NODE) {
        break;
      }

      entering = nodes[current].next != NO_NODE;
      current = entering ? nodes[current].next : nodes[current].parent;
    }
  }

  auto register_definition(const std::uint32_t index) -> void {
    normalize_label(this->label_buffer_, this->document_.nodes[index].literal);
    if (this->label_buffer_.empty()) {
      return;
    }

    this->definition_nodes_.push_back(index);
    if (!this->definitions_.contains(std::string_view{this->label_buffer_})) {
      this->definitions_.emplace(
          this->document_.strings.store(this->label_buffer_), index);
    }
  }

  auto resolve_reference(const std::uint32_t index, std::uint32_t &last_index)
      -> void {
    auto &nodes{this->document_.nodes};
    const auto label{nodes[index].literal};
    auto definition{NO_NODE};
    if (!label.empty() && label.size() <= MAXIMUM_LINK_LABEL_LENGTH) {
      normalize_label(this->label_buffer_, label);
      const auto match{
          this->definitions_.find(std::string_view{this->label_buffer_})};
      if (match != this->definitions_.end()) {
        definition = match->second;
      }
    }

    if (definition == NO_NODE) {
      this->buffer_.assign("[^");
      this->buffer_.append(label);
      this->buffer_.push_back(']');
      const auto text{this->document_.create(NodeType::Text)};
      nodes[text].literal = this->document_.strings.store(this->buffer_);
      this->document_.insert_after(index, text);
      this->document_.unlink(index);
      return;
    }

    if (nodes[definition].extra == 0) {
      ++last_index;
      nodes[definition].extra = last_index;
    }

    ++nodes[definition].data;
    nodes[index].data = definition;
    nodes[index].extra = nodes[definition].data;
    std::array<char, 16> digits{};
    const auto result{std::to_chars(
        digits.data(), digits.data() + digits.size(), nodes[definition].extra)};
    nodes[index].literal = this->document_.strings.store(std::string_view{
        digits.data(), static_cast<std::size_t>(result.ptr - digits.data())});
  }

  [[nodiscard]] auto is_in_content(const std::string_view value) const noexcept
      -> bool {
    const std::less<const char *> before{};
    const auto *const begin{this->document_.content.data()};
    const auto *const end{begin + this->document_.content.size()};
    return !before(value.data(), begin) &&
           !before(end, value.data() + value.size());
  }

  auto merge_following_text(const std::uint32_t index) -> void {
    auto &nodes{this->document_.nodes};
    auto next{nodes[index].next};
    if (next == NO_NODE || nodes[next].type != NodeType::Text) {
      return;
    }

    auto literal{nodes[index].literal};
    bool buffered{false};
    while (next != NO_NODE && nodes[next].type == NodeType::Text) {
      const auto piece{nodes[next].literal};
      if (buffered) {
        this->buffer_.append(piece);
      } else if (literal.empty()) {
        literal = piece;
      } else if (!piece.empty()) {
        if (literal.data() + literal.size() == piece.data() &&
            this->is_in_content(literal) && this->is_in_content(piece)) {
          literal =
              std::string_view{literal.data(), literal.size() + piece.size()};
        } else {
          this->buffer_.assign(literal);
          this->buffer_.append(piece);
          buffered = true;
        }
      }

      const auto following{nodes[next].next};
      this->document_.unlink(next);
      next = following;
    }

    nodes[index].literal =
        buffered ? this->document_.strings.store(this->buffer_) : literal;
  }

  static auto
  matches_protocol(const std::string_view data, const std::size_t separator,
                   const std::size_t rewind, const std::size_t max_rewind,
                   const std::string_view protocol) noexcept -> bool {
    const auto length{protocol.size()};
    if (length > max_rewind - rewind ||
        data.substr(separator - rewind - length, length) != protocol) {
      return false;
    }

    return length == max_rewind - rewind ||
           !is_alphanumeric(data[separator - rewind - length - 1]);
  }

  auto link_emails_in_text(std::uint32_t text) -> void {
    auto &nodes{this->document_.nodes};
    const auto data{nodes[text].literal};
    std::size_t start{0};
    std::size_t offset{0};
    std::size_t remaining{data.size()};
    while (offset < remaining) {
      const auto *const found{static_cast<const char *>(
          std::memchr(data.data() + start + offset, '@', remaining - offset))};
      if (found == nullptr) {
        break;
      }

      auto max_rewind{
          static_cast<std::size_t>(found - (data.data() + start + offset))};
      bool auto_mailto{true};
      bool is_xmpp{false};
      std::size_t periods{0};
      std::size_t rewind{0};
      std::size_t link_end{0};
      bool retry{true};
      bool skip{false};
      while (retry) {
        retry = false;
        const auto separator{start + offset + max_rewind};
        for (rewind = 0; rewind < max_rewind; ++rewind) {
          const auto character{data[separator - rewind - 1]};
          if (is_alphanumeric(character) || character == '.' ||
              character == '+' || character == '-' || character == '_') {
            continue;
          }

          if (character == ':' && matches_protocol(data, separator, rewind,
                                                   max_rewind, "mailto:")) {
            auto_mailto = false;
            continue;
          }

          if (character == ':' &&
              matches_protocol(data, separator, rewind, max_rewind, "xmpp:")) {
            auto_mailto = false;
            is_xmpp = true;
            continue;
          }

          break;
        }

        if (rewind == 0) {
          offset += max_rewind + 1;
          skip = true;
          break;
        }

        const auto limit{remaining - offset - max_rewind};
        for (link_end = 1; link_end < limit; ++link_end) {
          const auto character{data[separator + link_end]};
          if (is_alphanumeric(character)) {
            continue;
          }

          if (character == '@') {
            offset += max_rewind + 1;
            max_rewind = link_end - 1;
            retry = true;
            break;
          }

          if (character == '.' && link_end < limit - 1 &&
              is_alphanumeric(data[separator + link_end + 1])) {
            ++periods;
          } else if (character != '-' && character != '_' &&
                     !(character == '/' && is_xmpp)) {
            break;
          }
        }
      }

      if (skip) {
        continue;
      }

      const auto separator{start + offset + max_rewind};
      if (link_end < 2 || periods == 0 ||
          (!is_letter(data[separator + link_end - 1]) &&
           data[separator + link_end - 1] != '.')) {
        offset += max_rewind + link_end;
        continue;
      }

      link_end = trim_autolink_end(data.substr(separator), link_end);
      if (link_end == 0) {
        offset += max_rewind + 1;
        continue;
      }

      const auto email{data.substr(separator - rewind, link_end + rewind)};
      this->buffer_.clear();
      if (auto_mailto) {
        this->buffer_.append("mailto:");
      }

      this->buffer_.append(email);
      const auto link{this->document_.create(NodeType::Link)};
      nodes[link].literal = this->document_.strings.store(this->buffer_);
      const auto link_text{this->document_.create(NodeType::Text)};
      nodes[link_text].literal = email;
      this->document_.append_child(link, link_text);
      this->document_.insert_after(text, link);
      const auto after{this->document_.create(NodeType::Text)};
      nodes[after].literal = data.substr(
          separator + link_end, remaining - offset - max_rewind - link_end);
      this->document_.insert_after(link, after);
      nodes[text].literal = data.substr(start, offset + max_rewind - rewind);
      text = after;
      start += offset + max_rewind + link_end;
      remaining -= offset + max_rewind + link_end;
      offset = 0;
    }
  }

  Document &document_;
  std::unordered_map<std::string_view, std::uint32_t> definitions_;
  std::vector<std::uint32_t> definition_nodes_;
  std::vector<std::uint32_t> ordered_;
  std::vector<std::uint32_t> deferred_;
  std::string label_buffer_;
  std::string buffer_;
};

} // namespace sourcemeta::core::markdown

#endif
