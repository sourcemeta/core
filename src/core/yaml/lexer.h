#ifndef SOURCEMETA_CORE_YAML_LEXER_H_
#define SOURCEMETA_CORE_YAML_LEXER_H_

#include <sourcemeta/core/yaml_error.h>

#include <cstdint>     // std::uint8_t, std::uint64_t
#include <deque>       // std::deque
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core::yaml {

enum class TokenType : std::uint8_t {
  StreamStart,
  StreamEnd,
  DocumentStart,
  DocumentEnd,
  MappingStart,
  MappingEnd,
  SequenceStart,
  SequenceEnd,
  BlockMappingKey,
  BlockMappingValue,
  BlockSequenceEntry,
  FlowEntry,
  Scalar,
  Anchor,
  Alias,
  Tag,
  DirectiveYAML,
  DirectiveTag,
  DirectiveReserved
};

enum class ScalarStyle : std::uint8_t {
  Plain,
  SingleQuoted,
  DoubleQuoted,
  Literal,
  Folded
};

struct Token {
  TokenType type;
  std::string_view value;
  std::uint64_t line;
  std::uint64_t column;
  std::size_t position{0};
  ScalarStyle scalar_style{ScalarStyle::Plain};
  bool multiline{false};
};

class Lexer {
public:
  Lexer(const std::string_view input) : input_{input} {}

  auto next() -> std::optional<Token> {
    this->skip_whitespace_and_comments();

    if (this->position_ >= this->input_.size()) {
      if (!this->stream_started_) {
        this->stream_started_ = true;
        return Token{.type = TokenType::StreamStart,
                     .value = {},
                     .line = this->line_,
                     .column = this->column_};
      }
      if (!this->stream_ended_) {
        this->stream_ended_ = true;
        const auto end_line{this->column_ > 0 ? this->line_ + 1 : this->line_};
        const std::uint64_t end_column{0};
        return Token{.type = TokenType::StreamEnd,
                     .value = {},
                     .line = end_line,
                     .column = end_column};
      }
      return std::nullopt;
    }

    if (!this->stream_started_) {
      this->stream_started_ = true;
      return Token{.type = TokenType::StreamStart,
                   .value = {},
                   .line = this->line_,
                   .column = this->column_};
    }

    const auto current_line{this->line_};
    const auto current_column{this->column_};
    const auto current_position{this->position_};

    // YAML 1.2.2 forbids tab characters for indentation in block context
    // Tabs before flow indicators ({, [) are allowed as separation whitespace
    if (this->tab_at_line_start_) {
      this->tab_at_line_start_ = false;
      const char next_char{this->peek()};
      if (next_char != '{' && next_char != '[') {
        throw YAMLParseError{current_line, current_column,
                             "Tab characters cannot be used for indentation"};
      }
    }

    if (this->check_document_start()) {
      this->advance(3);
      return Token{.type = TokenType::DocumentStart,
                   .value = "---",
                   .line = current_line,
                   .column = current_column,
                   .position = current_position};
    }

    if (this->check_document_end()) {
      this->advance(3);
      this->validate_trailing_content();
      return Token{.type = TokenType::DocumentEnd,
                   .value = "...",
                   .line = current_line,
                   .column = current_column,
                   .position = current_position};
    }

    const char current{this->peek()};

    if (this->flow_level_ > 0) {
      switch (current) {
        case '{':
          this->advance(1);
          this->flow_level_++;
          return Token{.type = TokenType::MappingStart,
                       .value = "{",
                       .line = current_line,
                       .column = current_column};
        case '}':
          this->advance(1);
          this->flow_level_--;
          if (this->flow_level_ == 0) {
            this->validate_trailing_content();
          }
          return Token{.type = TokenType::MappingEnd,
                       .value = "}",
                       .line = current_line,
                       .column = current_column};
        case '[':
          this->advance(1);
          this->flow_level_++;
          return Token{.type = TokenType::SequenceStart,
                       .value = "[",
                       .line = current_line,
                       .column = current_column};
        case ']':
          this->advance(1);
          this->flow_level_--;
          if (this->flow_level_ == 0) {
            this->validate_trailing_content();
          }
          return Token{.type = TokenType::SequenceEnd,
                       .value = "]",
                       .line = current_line,
                       .column = current_column};
        case ',':
          this->advance(1);
          return Token{.type = TokenType::FlowEntry,
                       .value = ",",
                       .line = current_line,
                       .column = current_column};
        case ':':
          if (this->is_value_indicator()) {
            this->advance(1);
            this->last_was_quoted_scalar_ = false;
            return Token{.type = TokenType::BlockMappingValue,
                         .value = ":",
                         .line = current_line,
                         .column = current_column};
          }
          break;
        case '?':
          if (this->is_explicit_key()) {
            this->advance(1);
            return Token{.type = TokenType::BlockMappingKey,
                         .value = "?",
                         .line = current_line,
                         .column = current_column};
          }
          break;
        default:
          break;
      }
    } else {
      switch (current) {
        case '{':
          this->advance(1);
          this->flow_level_++;
          return Token{.type = TokenType::MappingStart,
                       .value = "{",
                       .line = current_line,
                       .column = current_column};
        case '[':
          this->advance(1);
          this->flow_level_++;
          return Token{.type = TokenType::SequenceStart,
                       .value = "[",
                       .line = current_line,
                       .column = current_column};
        case '-':
          if (this->is_block_sequence_entry()) {
            this->advance(1);
            return Token{.type = TokenType::BlockSequenceEntry,
                         .value = "-",
                         .line = current_line,
                         .column = current_column};
          }
          break;
        case '?':
          if (this->is_explicit_key()) {
            this->advance(1);
            return Token{.type = TokenType::BlockMappingKey,
                         .value = "?",
                         .line = current_line,
                         .column = current_column};
          }
          break;
        case ':':
          if (this->is_value_indicator()) {
            this->advance(1);
            this->last_was_quoted_scalar_ = false;
            return Token{.type = TokenType::BlockMappingValue,
                         .value = ":",
                         .line = current_line,
                         .column = current_column};
          }
          break;
        default:
          break;
      }
    }

    if (current == '&') {
      return this->scan_anchor();
    }

    if (current == '*') {
      return this->scan_alias();
    }

    if (current == '!') {
      return this->scan_tag();
    }

    if (current == '%') {
      return this->scan_directive();
    }

    if (current == '\'') {
      this->last_was_quoted_scalar_ = true;
      return this->scan_single_quoted_scalar();
    }

    if (current == '"') {
      this->last_was_quoted_scalar_ = true;
      return this->scan_double_quoted_scalar();
    }

    if (current == '|') {
      this->last_was_quoted_scalar_ = false;
      return this->scan_literal_block_scalar();
    }

    if (current == '>') {
      this->last_was_quoted_scalar_ = false;
      return this->scan_folded_block_scalar();
    }

    // '#' cannot start a plain scalar (it's a c-indicator)
    if (current == '#') {
      throw YAMLParseError{current_line, current_column,
                           "Unexpected '#' character"};
    }

    this->last_was_quoted_scalar_ = false;
    return this->scan_plain_scalar();
  }

public:
  [[nodiscard]] auto line() const noexcept -> std::uint64_t {
    if (this->position_ >= this->input_.size() && this->column_ > 1) {
      return this->line_ + 1;
    }
    return this->line_;
  }

  [[nodiscard]] auto column() const noexcept -> std::uint64_t {
    if (this->position_ >= this->input_.size()) {
      return 0;
    }
    return this->column_;
  }

  [[nodiscard]] auto flow_level() const noexcept -> std::size_t {
    return this->flow_level_;
  }

  auto set_block_indent(const std::size_t indent) noexcept -> void {
    this->block_indent_ = indent;
  }

  [[nodiscard]] auto block_indent() const noexcept -> std::size_t {
    return this->block_indent_;
  }

  [[nodiscard]] auto position() const noexcept -> std::size_t {
    return this->position_;
  }

private:
  [[nodiscard]] auto peek(const std::size_t offset = 0) const noexcept -> char {
    const auto index{this->position_ + offset};
    if (index >= this->input_.size()) {
      return '\0';
    }
    return this->input_[index];
  }

  auto advance(const std::size_t count) noexcept -> void {
    for (std::size_t index = 0; index < count; ++index) {
      if (this->position_ >= this->input_.size()) {
        break;
      }
      if (this->input_[this->position_] == '\n') {
        this->line_++;
        this->column_ = 1;
      } else {
        this->column_++;
      }
      this->position_++;
    }
  }

  auto skip_whitespace_and_comments() -> void {
    // '#' starts a comment when preceded by whitespace or at start of line
    // Check if position is at start of line or previous character is whitespace
    bool preceded_by_whitespace{
        this->column_ == 1 ||
        (this->position_ > 0 && (this->input_[this->position_ - 1] == ' ' ||
                                 this->input_[this->position_ - 1] == '\t' ||
                                 this->input_[this->position_ - 1] == '\n' ||
                                 this->input_[this->position_ - 1] == '\r'))};
    bool at_line_start{this->column_ == 1};
    this->tab_at_line_start_ = false;
    while (this->position_ < this->input_.size()) {
      const char current{this->peek()};

      if (current == ' ') {
        preceded_by_whitespace = true;
        this->advance(1);
        continue;
      }

      if (current == '\t') {
        if (this->flow_level_ == 0 && at_line_start) {
          this->tab_at_line_start_ = true;
        }
        preceded_by_whitespace = true;
        this->advance(1);
        continue;
      }

      if (current == '\n' || current == '\r') {
        this->advance(1);
        if (current == '\r' && this->peek() == '\n') {
          this->advance(1);
        }
        preceded_by_whitespace = true;
        at_line_start = true;
        this->tab_at_line_start_ = false;
        continue;
      }

      // Comments require preceding whitespace or start of line
      if (current == '#' && preceded_by_whitespace) {
        while (this->position_ < this->input_.size() && this->peek() != '\n') {
          this->advance(1);
        }
        continue;
      }

      break;
    }
  }

  [[nodiscard]] auto check_document_start() const noexcept -> bool {
    if (this->position_ + 2 >= this->input_.size()) {
      return false;
    }
    if (this->input_[this->position_] != '-' ||
        this->input_[this->position_ + 1] != '-' ||
        this->input_[this->position_ + 2] != '-') {
      return false;
    }
    if (this->position_ + 3 < this->input_.size()) {
      const char after{this->input_[this->position_ + 3]};
      return after == ' ' || after == '\t' || after == '\n' || after == '\r' ||
             after == '\0';
    }
    return true;
  }

  [[nodiscard]] auto check_document_end() const noexcept -> bool {
    if (this->position_ + 2 >= this->input_.size()) {
      return false;
    }
    if (this->input_[this->position_] != '.' ||
        this->input_[this->position_ + 1] != '.' ||
        this->input_[this->position_ + 2] != '.') {
      return false;
    }
    if (this->position_ + 3 < this->input_.size()) {
      const char after{this->input_[this->position_ + 3]};
      return after == ' ' || after == '\t' || after == '\n' || after == '\r' ||
             after == '\0';
    }
    return true;
  }

  [[nodiscard]] auto is_block_sequence_entry() const noexcept -> bool {
    if (this->position_ + 1 >= this->input_.size()) {
      return true;
    }
    const char after{this->input_[this->position_ + 1]};
    return after == ' ' || after == '\t' || after == '\n' || after == '\r';
  }

  [[nodiscard]] auto is_explicit_key() const noexcept -> bool {
    if (this->position_ + 1 >= this->input_.size()) {
      return true;
    }
    const char after{this->input_[this->position_ + 1]};
    return after == ' ' || after == '\t' || after == '\n' || after == '\r';
  }

  [[nodiscard]] auto is_value_indicator() const noexcept -> bool {
    if (this->flow_level_ > 0) {
      if (this->last_was_quoted_scalar_) {
        return true;
      }
      if (this->position_ + 1 >= this->input_.size()) {
        return true;
      }
      const char after{this->input_[this->position_ + 1]};
      return after == ' ' || after == '\t' || after == '\n' || after == '\r' ||
             after == ',' || after == '}' || after == ']';
    }
    if (this->position_ + 1 >= this->input_.size()) {
      return true;
    }
    const char after{this->input_[this->position_ + 1]};
    return after == ' ' || after == '\t' || after == '\n' || after == '\r';
  }

  [[nodiscard]] auto line_contains_mapping_key() const noexcept -> bool {
    auto scan_position{this->position_};
    while (scan_position < this->input_.size()) {
      const char character{this->input_[scan_position]};
      if (character == '\n' || character == '\r') {
        return false;
      }
      if (character == ':') {
        if (scan_position + 1 >= this->input_.size()) {
          return true;
        }
        const char after{this->input_[scan_position + 1]};
        if (after == ' ' || after == '\t' || after == '\n' || after == '\r') {
          return true;
        }
      }
      scan_position++;
    }
    return false;
  }

  auto scan_anchor() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};
    const auto start_position{this->position_};

    this->advance(1);

    while (this->position_ < this->input_.size()) {
      const char current{this->peek()};
      // Anchor names contain ns-char minus c-flow-indicator per YAML 1.2.2
      // Only whitespace and flow indicators terminate anchor names
      if (current == ' ' || current == '\t' || current == '\n' ||
          current == '\r' || current == ',' || current == '[' ||
          current == ']' || current == '{' || current == '}') {
        break;
      }
      this->advance(1);
    }

    const auto length{this->position_ - start_position};
    return Token{.type = TokenType::Anchor,
                 .value = this->input_.substr(start_position + 1, length - 1),
                 .line = start_line,
                 .column = start_column};
  }

  auto scan_alias() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};
    const auto start_position{this->position_};

    this->advance(1);

    while (this->position_ < this->input_.size()) {
      const char current{this->peek()};
      // Alias names contain ns-char minus c-flow-indicator per YAML 1.2.2
      // Only whitespace and flow indicators terminate alias names
      if (current == ' ' || current == '\t' || current == '\n' ||
          current == '\r' || current == ',' || current == '[' ||
          current == ']' || current == '{' || current == '}') {
        break;
      }
      this->advance(1);
    }

    const auto length{this->position_ - start_position};
    return Token{.type = TokenType::Alias,
                 .value = this->input_.substr(start_position + 1, length - 1),
                 .line = start_line,
                 .column = start_column};
  }

  auto scan_tag() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};
    const auto start_position{this->position_};

    this->advance(1);

    if (this->peek() == '<') {
      this->advance(1);
      while (this->position_ < this->input_.size() && this->peek() != '>') {
        this->advance(1);
      }
      if (this->peek() == '>') {
        this->advance(1);
      }
    } else {
      while (this->position_ < this->input_.size()) {
        const char current{this->peek()};
        if (current == ' ' || current == '\t' || current == '\n' ||
            current == '\r' || current == ',' || current == '[' ||
            current == ']' || current == '{' || current == '}') {
          break;
        }
        this->advance(1);
      }
    }

    const auto length{this->position_ - start_position};

    // YAML 1.2.2: tags must be separated from content by whitespace
    // In block context, flow indicators after a tag are invalid
    if (this->position_ < this->input_.size() && this->flow_level_ == 0) {
      const char after_tag{this->peek()};
      if (after_tag == ',') {
        throw YAMLParseError{this->line_, this->column_,
                             "Invalid character after tag in block context"};
      }
    }

    return Token{.type = TokenType::Tag,
                 .value = this->input_.substr(start_position, length),
                 .line = start_line,
                 .column = start_column};
  }

  auto scan_directive() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};
    const auto start_position{this->position_};

    this->advance(1);

    while (this->position_ < this->input_.size() && this->peek() != '\n' &&
           this->peek() != '\r') {
      this->advance(1);
    }

    const auto length{this->position_ - start_position};
    const auto directive_content{this->input_.substr(start_position, length)};

    TokenType token_type{TokenType::DirectiveReserved};
    if (directive_content.starts_with("%YAML")) {
      token_type = TokenType::DirectiveYAML;
    } else if (directive_content.starts_with("%TAG")) {
      token_type = TokenType::DirectiveTag;
    }

    return Token{.type = token_type,
                 .value = directive_content,
                 .line = start_line,
                 .column = start_column};
  }

  auto scan_single_quoted_scalar() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};

    this->advance(1);

    auto &buffer{this->get_buffer()};
    std::string line_content;
    bool first_line{true};
    std::size_t pending_newlines{0};
    bool found_closing_quote{false};

    while (this->position_ < this->input_.size()) {
      const char current{this->peek()};

      if (current == '\'') {
        if (this->peek(1) == '\'') {
          line_content += '\'';
          this->advance(2);
        } else {
          this->flush_flow_line(buffer, line_content, pending_newlines,
                                first_line, true);
          this->advance(1);
          found_closing_quote = true;
          break;
        }
      } else if (current == '\n' || current == '\r') {
        this->flush_flow_line(buffer, line_content, pending_newlines,
                              first_line);
        first_line = false;
        this->advance(1);
        if (current == '\r' && this->peek() == '\n') {
          this->advance(1);
        }
        pending_newlines++;
        while (this->position_ < this->input_.size()) {
          const char character{this->peek()};
          if (character == ' ' || character == '\t') {
            this->advance(1);
          } else if (character == '\n') {
            pending_newlines++;
            this->advance(1);
          } else if (character == '\r') {
            pending_newlines++;
            this->advance(1);
            if (this->peek() == '\n') {
              this->advance(1);
            }
          } else {
            break;
          }
        }
        this->validate_flow_scalar_continuation();
      } else {
        line_content += current;
        this->advance(1);
      }
    }

    if (!found_closing_quote) {
      throw YAMLParseError{start_line, start_column,
                           "Missing closing quote in single-quoted scalar"};
    }

    this->validate_trailing_content();

    return Token{.type = TokenType::Scalar,
                 .value = buffer,
                 .line = start_line,
                 .column = start_column,
                 .scalar_style = ScalarStyle::SingleQuoted,
                 .multiline = !first_line};
  }

  auto flush_flow_line(std::string &buffer, std::string &line_content,
                       std::size_t &pending_newlines, const bool first_line,
                       const bool is_final = false) -> void {
    if (!is_final) {
      while (!line_content.empty() &&
             (line_content.back() == ' ' || line_content.back() == '\t')) {
        line_content.pop_back();
      }
    }

    if (pending_newlines > 0 && !first_line) {
      if (pending_newlines == 1) {
        buffer += ' ';
      } else {
        for (std::size_t count = 1; count < pending_newlines; ++count) {
          buffer += '\n';
        }
      }
      pending_newlines = 0;
    }

    if (!line_content.empty()) {
      buffer += line_content;
    }
    line_content.clear();
  }

  // YAML 1.2.2 c-forbidden: document markers at column 1 are forbidden
  // inside flow scalars. Also checks that continuation lines in block
  // context are properly indented
  auto validate_flow_scalar_continuation() -> void {
    if (this->position_ >= this->input_.size()) {
      return;
    }
    // c-forbidden: --- or ... at column 1 followed by whitespace/newline/EOF
    if (this->column_ == 1 && this->position_ + 2 < this->input_.size()) {
      const char first{this->peek()};
      const char second{this->peek(1)};
      const char third{this->peek(2)};
      if ((first == '-' && second == '-' && third == '-') ||
          (first == '.' && second == '.' && third == '.')) {
        if (this->position_ + 3 >= this->input_.size()) {
          throw YAMLParseError{this->line_, this->column_,
                               "Document marker inside flow scalar"};
        }
        const char after{this->input_[this->position_ + 3]};
        if (after == ' ' || after == '\t' || after == '\n' || after == '\r') {
          throw YAMLParseError{this->line_, this->column_,
                               "Document marker inside flow scalar"};
        }
      }
    }
    // In block context, continuation lines must be indented past the
    // current block indentation level
    if (this->flow_level_ == 0 && this->block_indent_ != SIZE_MAX) {
      const auto current_indent{static_cast<std::size_t>(this->column_ - 1)};
      if (current_indent <= this->block_indent_) {
        throw YAMLParseError{this->line_, this->column_,
                             "Insufficient indentation in flow scalar"};
      }
    }
  }

  // After a completed value (quoted scalar, flow collection end), only
  // whitespace, comments (preceded by whitespace), valid indicators, or
  // end-of-line are allowed on the same line
  auto validate_trailing_content() -> void {
    auto lookahead{this->position_};
    bool seen_whitespace{false};
    while (lookahead < this->input_.size()) {
      const char character{this->input_[lookahead]};
      if (character == ' ' || character == '\t') {
        seen_whitespace = true;
        lookahead++;
        continue;
      }
      if (character == '\n' || character == '\r') {
        return;
      }
      // Comment start requires preceding whitespace
      if (character == '#') {
        if (seen_whitespace) {
          return;
        }
        throw YAMLParseError{this->line_, this->column_,
                             "Invalid trailing content"};
      }
      // Value indicator is always valid
      if (character == ':') {
        return;
      }
      // Flow indicators only valid in flow context
      if (this->flow_level_ > 0 &&
          (character == ',' || character == ']' || character == '}' ||
           character == '{' || character == '[')) {
        return;
      }
      throw YAMLParseError{this->line_, this->column_,
                           "Invalid trailing content"};
    }
  }

  auto scan_double_quoted_scalar() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};

    this->advance(1);

    auto &buffer{this->get_buffer()};
    std::string line_content;
    bool first_line{true};
    std::size_t pending_newlines{0};
    bool found_closing_quote{false};

    while (this->position_ < this->input_.size()) {
      const char current{this->peek()};

      if (current == '"') {
        this->flush_flow_line(buffer, line_content, pending_newlines,
                              first_line, true);
        this->advance(1);
        found_closing_quote = true;
        break;
      }

      if (current == '\\') {
        this->advance(1);
        if (this->position_ < this->input_.size()) {
          const char escaped{this->peek()};
          switch (escaped) {
            case '0':
              line_content += '\0';
              break;
            case 'a':
              line_content += '\a';
              break;
            case 'b':
              line_content += '\b';
              break;
            case 't':
            case '\t':
              line_content += '\t';
              break;
            case 'n':
              line_content += '\n';
              break;
            case 'v':
              line_content += '\v';
              break;
            case 'f':
              line_content += '\f';
              break;
            case 'r':
              line_content += '\r';
              break;
            case 'e':
              line_content += '\x1b';
              break;
            case ' ':
              line_content += ' ';
              break;
            case '"':
              line_content += '"';
              break;
            case '/':
              line_content += '/';
              break;
            case '\\':
              line_content += '\\';
              break;
            case 'N':
              line_content += "\xc2\x85";
              break;
            case '_':
              line_content += "\xc2\xa0";
              break;
            case 'L':
              line_content += "\xe2\x80\xa8";
              break;
            case 'P':
              line_content += "\xe2\x80\xa9";
              break;
            case 'x':
              this->advance(1);
              line_content += this->parse_hex_escape(2);
              continue;
            case 'u':
              this->advance(1);
              line_content += this->parse_unicode_escape(4);
              continue;
            case 'U':
              this->advance(1);
              line_content += this->parse_unicode_escape(8);
              continue;
            case '\n':
            case '\r':
              if (escaped == '\r' && this->peek(1) == '\n') {
                this->advance(1);
              }
              this->advance(1);
              while (this->position_ < this->input_.size() &&
                     (this->peek() == ' ' || this->peek() == '\t')) {
                this->advance(1);
              }
              continue;
            default:
              throw YAMLParseError{this->line_, this->column_,
                                   "Invalid escape sequence in "
                                   "double-quoted scalar"};
          }
          this->advance(1);
        }
      } else if (current == '\n' || current == '\r') {
        this->flush_flow_line(buffer, line_content, pending_newlines,
                              first_line);
        first_line = false;
        this->advance(1);
        if (current == '\r' && this->peek() == '\n') {
          this->advance(1);
        }
        pending_newlines++;
        while (this->position_ < this->input_.size()) {
          const char character{this->peek()};
          if (character == ' ' || character == '\t') {
            this->advance(1);
          } else if (character == '\n') {
            pending_newlines++;
            this->advance(1);
          } else if (character == '\r') {
            pending_newlines++;
            this->advance(1);
            if (this->peek() == '\n') {
              this->advance(1);
            }
          } else {
            break;
          }
        }
        this->validate_flow_scalar_continuation();
      } else {
        line_content += current;
        this->advance(1);
      }
    }

    if (!found_closing_quote) {
      throw YAMLParseError{start_line, start_column,
                           "Missing closing quote in double-quoted scalar"};
    }

    this->validate_trailing_content();

    return Token{.type = TokenType::Scalar,
                 .value = buffer,
                 .line = start_line,
                 .column = start_column,
                 .scalar_style = ScalarStyle::DoubleQuoted,
                 .multiline = !first_line};
  }

  auto parse_hex_escape(const std::size_t digits) -> std::string {
    std::string hex;
    for (std::size_t index = 0;
         index < digits && this->position_ < this->input_.size(); ++index) {
      hex += this->peek();
      this->advance(1);
    }

    const auto codepoint{std::stoul(hex, nullptr, 16)};
    return this->codepoint_to_utf8(static_cast<std::uint32_t>(codepoint));
  }

  auto parse_unicode_escape(const std::size_t digits) -> std::string {
    return this->parse_hex_escape(digits);
  }

  [[nodiscard]] auto calculate_parent_indentation(
      const std::size_t indicator_position) const noexcept -> std::size_t {
    // Find the start of the current line by scanning backward
    std::size_t line_start{indicator_position};
    while (line_start > 0 && this->input_[line_start - 1] != '\n' &&
           this->input_[line_start - 1] != '\r') {
      line_start--;
    }

    // Extract the line for logging
    std::size_t line_end{indicator_position};
    while (line_end < this->input_.size() && this->input_[line_end] != '\n' &&
           this->input_[line_end] != '\r') {
      line_end++;
    }
    // Count leading spaces on this line
    std::size_t leading_spaces{0};
    std::size_t scan_position{line_start};
    while (scan_position < this->input_.size() &&
           this->input_[scan_position] == ' ') {
      leading_spaces++;
      scan_position++;
    }

    // Check if line starts with sequence entry "- "
    bool in_sequence_entry{false};
    if (scan_position < this->input_.size() - 1 &&
        this->input_[scan_position] == '-' &&
        this->input_[scan_position + 1] == ' ') {
      in_sequence_entry = true;
    }

    // Check if there's a ':' between line start and indicator (mapping value on
    // same line)
    bool is_mapping_value_same_line{false};
    for (std::size_t index = line_start; index < indicator_position; ++index) {
      if (this->input_[index] == ':') {
        is_mapping_value_same_line = true;
        break;
      }
    }

    // If we're in a sequence entry AND this is a mapping value on the same
    // line, the parent indentation is leading_spaces + 2 (after "- ")
    if (in_sequence_entry && is_mapping_value_same_line) {
      return leading_spaces + 2;
    }

    // If this is a mapping value on the same line (no sequence), use
    // leading_spaces
    if (is_mapping_value_same_line) {
      return leading_spaces;
    }

    // If the indicator is on a separate line (no ':' before it on this line),
    // the block scalar is part of a multi-line structure. The parent
    // indentation is 0 since the explicit indicator specifies the absolute
    // content indentation
    return 0;
  }

  [[nodiscard]] auto codepoint_to_utf8(const std::uint32_t codepoint) const
      -> std::string {
    std::string result;
    if (codepoint < 0x80) {
      result += static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {
      result += static_cast<char>(0xC0 | (codepoint >> 6));
      result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000) {
      result += static_cast<char>(0xE0 | (codepoint >> 12));
      result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
      result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
      result += static_cast<char>(0xF0 | (codepoint >> 18));
      result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
      result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
      result += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    return result;
  }

  auto scan_literal_block_scalar() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};
    const auto indicator_position{this->position_};

    this->advance(1);

    char chomping{'c'};
    std::size_t explicit_indent{0};

    bool seen_header_whitespace{false};
    while (this->position_ < this->input_.size()) {
      const char current{this->peek()};
      if (current == '-') {
        chomping = '-';
        this->advance(1);
      } else if (current == '+') {
        chomping = '+';
        this->advance(1);
      } else if (current >= '1' && current <= '9') {
        explicit_indent = static_cast<std::size_t>(current - '0');
        this->advance(1);
      } else if (current == ' ' || current == '\t') {
        seen_header_whitespace = true;
        this->advance(1);
      } else if (current == '#' && seen_header_whitespace) {
        while (this->position_ < this->input_.size() && this->peek() != '\n') {
          this->advance(1);
        }
      } else if (current == '\n' || current == '\r') {
        break;
      } else {
        throw YAMLParseError{this->line_, this->column_,
                             "Invalid content in block scalar header"};
      }
    }

    if (this->peek() == '\n' || this->peek() == '\r') {
      this->advance(1);
      if (this->input_[this->position_ - 1] == '\r' && this->peek() == '\n') {
        this->advance(1);
      }
    }

    auto &buffer{this->get_buffer()};
    std::size_t content_indent{0};

    if (explicit_indent > 0) {
      const auto parent_indent{
          this->calculate_parent_indentation(indicator_position)};
      content_indent = parent_indent + explicit_indent;
    } else {
      const auto saved_position{this->position_};
      const auto saved_line{this->line_};
      const auto saved_column{this->column_};

      std::size_t max_leading_empty_indent{0};
      std::size_t current_empty_indent{0};
      while (this->position_ < this->input_.size()) {
        if (this->peek() == ' ') {
          content_indent++;
          current_empty_indent++;
          this->advance(1);
        } else if (this->peek() == '\n' || this->peek() == '\r') {
          if (current_empty_indent > max_leading_empty_indent) {
            max_leading_empty_indent = current_empty_indent;
          }
          content_indent = 0;
          current_empty_indent = 0;
          this->advance(1);
        } else {
          break;
        }
      }

      this->position_ = saved_position;
      this->line_ = saved_line;
      this->column_ = saved_column;

      // YAML 1.2.2: leading empty lines in a block scalar must not
      // contain more spaces than the first non-empty content line
      if (max_leading_empty_indent > content_indent && content_indent > 0) {
        throw YAMLParseError{
            start_line, start_column,
            "Leading empty line has more spaces than content indentation"};
      }
    }

    // If content_indent is 0 but indicator is not right after "--- ",
    // we're inside a structure and need at least 1 space of indentation
    // Document-level block scalars (after ---) have indicator at column 5
    if (content_indent == 0 && start_column > 5) {
      content_indent = 1;
    }

    std::string trailing_newlines;

    while (this->position_ < this->input_.size()) {
      std::size_t line_indent{0};
      while (this->position_ < this->input_.size() && this->peek() == ' ') {
        line_indent++;
        this->advance(1);
      }

      if (this->peek() == '\n' || this->peek() == '\r') {
        // If this whitespace-only line has more indentation than
        // content_indent, preserve the extra spaces as content before the
        // newline
        if (line_indent > content_indent) {
          buffer += trailing_newlines;
          trailing_newlines.clear();
          for (std::size_t index = content_indent; index < line_indent;
               ++index) {
            buffer += ' ';
          }
        }
        trailing_newlines += '\n';
        this->advance(1);
        if (this->input_[this->position_ - 1] == '\r' && this->peek() == '\n') {
          this->advance(1);
        }
        continue;
      }

      if (line_indent < content_indent) {
        for (std::size_t index = 0; index < line_indent; ++index) {
          this->position_--;
          this->column_--;
        }
        break;
      }

      // Check for document markers at column 1 - these terminate block scalars
      if (line_indent == 0 && this->position_ + 2 < this->input_.size()) {
        if ((this->peek() == '-' && this->peek(1) == '-' &&
             this->peek(2) == '-') ||
            (this->peek() == '.' && this->peek(1) == '.' &&
             this->peek(2) == '.')) {
          break;
        }
      }

      buffer += trailing_newlines;
      trailing_newlines.clear();

      for (std::size_t index = content_indent; index < line_indent; ++index) {
        buffer += ' ';
      }

      while (this->position_ < this->input_.size() && this->peek() != '\n' &&
             this->peek() != '\r') {
        buffer += this->peek();
        this->advance(1);
      }

      if (this->peek() == '\n' || this->peek() == '\r') {
        trailing_newlines += '\n';
        this->advance(1);
        if (this->input_[this->position_ - 1] == '\r' && this->peek() == '\n') {
          this->advance(1);
        }
      }
    }

    if (chomping == '+') {
      buffer += trailing_newlines;
    } else if (chomping == 'c' && !buffer.empty() &&
               !trailing_newlines.empty()) {
      buffer += '\n';
    }

    return Token{.type = TokenType::Scalar,
                 .value = buffer,
                 .line = start_line,
                 .column = start_column,
                 .scalar_style = ScalarStyle::Literal};
  }

  auto scan_folded_block_scalar() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};
    const auto indicator_position{this->position_};

    this->advance(1);

    char chomping{'c'};
    std::size_t explicit_indent{0};

    bool seen_header_whitespace_folded{false};
    while (this->position_ < this->input_.size()) {
      const char current{this->peek()};
      if (current == '-') {
        chomping = '-';
        this->advance(1);
      } else if (current == '+') {
        chomping = '+';
        this->advance(1);
      } else if (current >= '1' && current <= '9') {
        explicit_indent = static_cast<std::size_t>(current - '0');
        this->advance(1);
      } else if (current == ' ' || current == '\t') {
        seen_header_whitespace_folded = true;
        this->advance(1);
      } else if (current == '#' && seen_header_whitespace_folded) {
        while (this->position_ < this->input_.size() && this->peek() != '\n') {
          this->advance(1);
        }
      } else if (current == '\n' || current == '\r') {
        break;
      } else {
        throw YAMLParseError{this->line_, this->column_,
                             "Invalid content in block scalar header"};
      }
    }

    if (this->peek() == '\n' || this->peek() == '\r') {
      this->advance(1);
      if (this->input_[this->position_ - 1] == '\r' && this->peek() == '\n') {
        this->advance(1);
      }
    }

    auto &buffer{this->get_buffer()};
    std::size_t content_indent{0};

    if (explicit_indent > 0) {
      const auto parent_indent{
          this->calculate_parent_indentation(indicator_position)};
      content_indent = parent_indent + explicit_indent;
    } else {
      const auto saved_position{this->position_};
      const auto saved_line{this->line_};
      const auto saved_column{this->column_};

      std::size_t max_leading_empty_indent{0};
      std::size_t current_empty_indent{0};
      while (this->position_ < this->input_.size()) {
        if (this->peek() == ' ') {
          content_indent++;
          current_empty_indent++;
          this->advance(1);
        } else if (this->peek() == '\n' || this->peek() == '\r') {
          if (current_empty_indent > max_leading_empty_indent) {
            max_leading_empty_indent = current_empty_indent;
          }
          content_indent = 0;
          current_empty_indent = 0;
          this->advance(1);
        } else {
          break;
        }
      }

      this->position_ = saved_position;
      this->line_ = saved_line;
      this->column_ = saved_column;

      // YAML 1.2.2: leading empty lines in a block scalar must not
      // contain more spaces than the first non-empty content line
      if (max_leading_empty_indent > content_indent && content_indent > 0) {
        throw YAMLParseError{
            start_line, start_column,
            "Leading empty line has more spaces than content indentation"};
      }
    }

    // If content_indent is 0 but indicator is not right after "--- ",
    // we're inside a structure and need at least 1 space of indentation
    // Document-level block scalars (after ---) have indicator at column 5
    if (content_indent == 0 && start_column > 5) {
      content_indent = 1;
    }

    std::size_t blank_line_count{0};
    bool previous_was_more_indented{false};
    bool previous_started_with_whitespace{false};
    bool had_line_break{false};

    while (this->position_ < this->input_.size()) {
      std::size_t line_indent{0};
      while (this->position_ < this->input_.size() && this->peek() == ' ') {
        line_indent++;
        this->advance(1);
      }

      if (this->peek() == '\n' || this->peek() == '\r') {
        blank_line_count++;
        this->advance(1);
        if (this->input_[this->position_ - 1] == '\r' && this->peek() == '\n') {
          this->advance(1);
        }
        continue;
      }

      if (line_indent < content_indent) {
        for (std::size_t index = 0; index < line_indent; ++index) {
          this->position_--;
          this->column_--;
        }
        break;
      }

      // Check for document markers at column 1 - these terminate block scalars
      if (line_indent == 0 && this->position_ + 2 < this->input_.size()) {
        if ((this->peek() == '-' && this->peek(1) == '-' &&
             this->peek(2) == '-') ||
            (this->peek() == '.' && this->peek(1) == '.' &&
             this->peek(2) == '.')) {
          break;
        }
      }

      // Check if this line starts with whitespace (tab) after indentation
      const bool starts_with_whitespace{this->peek() == '\t'};

      if (had_line_break) {
        // YAML 1.2.2 spec: newline is preserved if previous or current line
        // is empty, starts with whitespace, or has more indentation
        const bool preserve_line_break{
            previous_was_more_indented || previous_started_with_whitespace ||
            line_indent > content_indent || starts_with_whitespace};

        if (blank_line_count == 0 && !preserve_line_break) {
          buffer += ' ';
        } else {
          if (preserve_line_break) {
            buffer += '\n';
          }
          for (std::size_t count = 0; count < blank_line_count; ++count) {
            buffer += '\n';
          }
        }
      } else if (blank_line_count > 0) {
        for (std::size_t count = 0; count < blank_line_count; ++count) {
          buffer += '\n';
        }
      }
      blank_line_count = 0;
      had_line_break = false;

      for (std::size_t index = content_indent; index < line_indent; ++index) {
        buffer += ' ';
      }

      while (this->position_ < this->input_.size() && this->peek() != '\n' &&
             this->peek() != '\r') {
        buffer += this->peek();
        this->advance(1);
      }

      previous_was_more_indented = (line_indent > content_indent);
      previous_started_with_whitespace = starts_with_whitespace;

      if (this->peek() == '\n' || this->peek() == '\r') {
        had_line_break = true;
        this->advance(1);
        if (this->input_[this->position_ - 1] == '\r' && this->peek() == '\n') {
          this->advance(1);
        }
      }
    }

    if (chomping == '+') {
      if (had_line_break) {
        buffer += '\n';
      }
      for (std::size_t count = 0; count < blank_line_count; ++count) {
        buffer += '\n';
      }
    } else if (chomping == 'c' && !buffer.empty() &&
               (had_line_break || blank_line_count > 0)) {
      buffer += '\n';
    }

    return Token{.type = TokenType::Scalar,
                 .value = buffer,
                 .line = start_line,
                 .column = start_column,
                 .scalar_style = ScalarStyle::Folded};
  }

  auto scan_plain_scalar() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};
    const auto start_position{this->position_};
    const bool in_flow{this->flow_level_ > 0};

    // YAML 1.2.2 ns-plain-first: `-`, `?`, `:` can start a plain scalar
    // only if followed by an ns-plain-safe character. In flow context,
    // flow indicators are not safe
    if (in_flow) {
      const char first{this->peek()};
      if (first == '-' || first == '?' || first == ':') {
        const char after{this->peek(1)};
        if (after == '\0' || after == ' ' || after == '\t' || after == '\n' ||
            after == '\r' || after == ',' || after == '[' || after == ']' ||
            after == '{' || after == '}') {
          throw YAMLParseError{start_line, start_column,
                               "Invalid plain scalar start in flow context"};
        }
      }
    }

    const std::size_t min_indent{
        in_flow
            ? 0
            : (this->block_indent_ == SIZE_MAX ? 0 : this->block_indent_ + 1)};
    bool used_multiline{false};
    std::string pending_whitespace;
    std::string *buffer{nullptr};

    while (this->position_ < this->input_.size()) {
      const auto line_start{this->position_};

      while (this->position_ < this->input_.size()) {
        const char current{this->peek()};

        if (current == ':') {
          const char after{this->peek(1)};
          if (after == ' ' || after == '\t' || after == '\n' || after == '\r' ||
              after == '\0') {
            break;
          }
          if (in_flow && (after == ',' || after == '[' || after == ']' ||
                          after == '{' || after == '}')) {
            break;
          }
        }

        if (current == '#') {
          if (this->position_ > line_start) {
            const char before{this->input_[this->position_ - 1]};
            if (before == ' ' || before == '\t') {
              break;
            }
          }
        }

        if (in_flow && (current == ',' || current == '[' || current == ']' ||
                        current == '{' || current == '}')) {
          break;
        }

        if (current == '\n' || current == '\r') {
          break;
        }

        this->advance(1);
      }

      auto segment_end{this->position_};
      while (segment_end > line_start &&
             (this->input_[segment_end - 1] == ' ' ||
              this->input_[segment_end - 1] == '\t')) {
        segment_end--;
      }

      const auto segment{
          this->input_.substr(line_start, segment_end - line_start)};

      if (!segment.empty()) {
        if (used_multiline) {
          *buffer += pending_whitespace;
        }
        if (buffer != nullptr) {
          *buffer += segment;
        }
        pending_whitespace.clear();
      }

      if (this->position_ >= this->input_.size()) {
        break;
      }

      const char current{this->peek()};
      if (current != '\n' && current != '\r') {
        break;
      }

      const auto saved_position{this->position_};
      const auto saved_line{this->line_};
      const auto saved_column{this->column_};

      std::size_t newline_count{0};
      std::size_t next_line_indent{0};

      while (this->position_ < this->input_.size()) {
        const char character{this->peek()};
        if (character == '\n') {
          newline_count++;
          this->advance(1);
          next_line_indent = 0;
        } else if (character == '\r') {
          newline_count++;
          this->advance(1);
          if (this->peek() == '\n') {
            this->advance(1);
          }
          next_line_indent = 0;
        } else if (character == ' ' || character == '\t') {
          next_line_indent++;
          this->advance(1);
        } else {
          break;
        }
      }

      if (this->position_ >= this->input_.size()) {
        this->position_ = saved_position;
        this->line_ = saved_line;
        this->column_ = saved_column;
        break;
      }

      if (next_line_indent < min_indent) {
        this->position_ = saved_position;
        this->line_ = saved_line;
        this->column_ = saved_column;
        break;
      }

      const char next_char{this->peek()};

      // In flow context, stop if we hit a flow indicator on continuation
      if (in_flow &&
          (next_char == ',' || next_char == '[' || next_char == ']' ||
           next_char == '{' || next_char == '}')) {
        this->position_ = saved_position;
        this->line_ = saved_line;
        this->column_ = saved_column;
        break;
      }

      if (next_char == '-' || next_char == '?' || next_char == ':') {
        const char after{this->peek(1)};
        if (after == ' ' || after == '\t' || after == '\n' || after == '\r' ||
            after == '\0') {
          // Break if indicator is at column 0 (absolute base) or at a valid
          // sibling position. For content starting at column C, a sibling
          // indicator would be at column C-2 (before "- " that introduces
          // content). This prevents treating indicators at invalid intermediate
          // columns as block entries
          if (next_line_indent == 0 || next_line_indent <= start_column - 3) {
            this->position_ = saved_position;
            this->line_ = saved_line;
            this->column_ = saved_column;
            break;
          }
        }
        // In flow context, colon followed by flow indicator ends the scalar
        if (in_flow && next_char == ':') {
          if (after == ',' || after == '[' || after == ']' || after == '{' ||
              after == '}') {
            this->position_ = saved_position;
            this->line_ = saved_line;
            this->column_ = saved_column;
            break;
          }
        }
      }

      // In block context, a mapping key on continuation line ends the scalar
      // In flow context, this check is skipped because : is a value indicator
      if (!in_flow && this->line_contains_mapping_key()) {
        this->position_ = saved_position;
        this->line_ = saved_line;
        this->column_ = saved_column;
        break;
      }

      if (next_line_indent == 0) {
        if ((next_char == '-' && this->peek(1) == '-' &&
             this->peek(2) == '-') ||
            (next_char == '.' && this->peek(1) == '.' &&
             this->peek(2) == '.')) {
          this->position_ = saved_position;
          this->line_ = saved_line;
          this->column_ = saved_column;
          break;
        }
      }

      if (next_char == '#') {
        this->position_ = saved_position;
        this->line_ = saved_line;
        this->column_ = saved_column;
        break;
      }

      if (!used_multiline) {
        buffer = &this->get_buffer();
        *buffer =
            this->input_.substr(start_position, segment_end - start_position);
      }
      used_multiline = true;
      if (newline_count == 1) {
        pending_whitespace = " ";
      } else {
        pending_whitespace = std::string(newline_count - 1, '\n');
      }
    }

    if (used_multiline && buffer != nullptr) {
      return Token{.type = TokenType::Scalar,
                   .value = *buffer,
                   .line = start_line,
                   .column = start_column,
                   .scalar_style = ScalarStyle::Plain,
                   .multiline = true};
    }

    auto length{this->position_ - start_position};
    while (length > 0 && (this->input_[start_position + length - 1] == ' ' ||
                          this->input_[start_position + length - 1] == '\t')) {
      length--;
    }

    return Token{.type = TokenType::Scalar,
                 .value = this->input_.substr(start_position, length),
                 .line = start_line,
                 .column = start_column,
                 .scalar_style = ScalarStyle::Plain};
  }

  auto scan_plain_scalar_single_line() -> Token {
    const auto start_line{this->line_};
    const auto start_column{this->column_};
    const auto start_position{this->position_};

    while (this->position_ < this->input_.size()) {
      const char current{this->peek()};

      if (current == ':') {
        const char after{this->peek(1)};
        if (after == ' ' || after == '\t' || after == '\n' || after == '\r' ||
            after == '\0') {
          break;
        }
        if (this->flow_level_ > 0 &&
            (after == ',' || after == '[' || after == ']' || after == '{' ||
             after == '}')) {
          break;
        }
      }

      if (current == '#') {
        if (this->position_ > start_position) {
          const char before{this->input_[this->position_ - 1]};
          if (before == ' ' || before == '\t') {
            break;
          }
        }
      }

      if (this->flow_level_ > 0) {
        if (current == ',' || current == '[' || current == ']' ||
            current == '{' || current == '}') {
          break;
        }
      }

      if (current == '\n' || current == '\r') {
        break;
      }

      this->advance(1);
    }

    auto length{this->position_ - start_position};

    while (length > 0 && (this->input_[start_position + length - 1] == ' ' ||
                          this->input_[start_position + length - 1] == '\t')) {
      length--;
    }

    return Token{.type = TokenType::Scalar,
                 .value = this->input_.substr(start_position, length),
                 .line = start_line,
                 .column = start_column,
                 .scalar_style = ScalarStyle::Plain};
  }

  auto get_buffer() -> std::string & {
    this->scalar_buffers_.emplace_back();
    return this->scalar_buffers_.back();
  }

  std::string_view input_;
  std::size_t position_{0};
  std::uint64_t line_{1};
  std::uint64_t column_{1};
  std::size_t flow_level_{0};
  bool stream_started_{false};
  bool stream_ended_{false};
  bool last_was_quoted_scalar_{false};
  bool tab_at_line_start_{false};
  // SIZE_MAX means "not set" (top-level), 0 means parent at indent 0
  std::size_t block_indent_{SIZE_MAX};
  std::deque<std::string> scalar_buffers_;
};

} // namespace sourcemeta::core::yaml

#endif
