#include <sourcemeta/core/uritemplate.h>

#include "helpers.h"

#include <cassert> // assert
#include <utility> // std::pair
#include <vector>  // std::vector

namespace sourcemeta::core {

template <typename T>
static auto try_parse(std::string_view &remaining, std::size_t &offset,
                      std::vector<URITemplateToken> &tokens) -> bool {
  if (auto result = parse_expression<T>(remaining)) {
    tokens.emplace_back(std::move(result->first));
    remaining.remove_prefix(result->second);
    offset += result->second;
    return true;
  }

  return false;
}

template <typename... Ts>
static auto try_parse_any(std::string_view &remaining, std::size_t &offset,
                          std::vector<URITemplateToken> &tokens) -> bool {
  return (try_parse<Ts>(remaining, offset, tokens) || ...);
}

URITemplate::URITemplate(const std::string_view source) {
  std::string_view remaining{source};
  std::size_t offset = 0;

  while (!remaining.empty()) {
    try {
      if (!try_parse_any<URITemplateTokenReservedExpansion,
                         URITemplateTokenFragmentExpansion,
                         URITemplateTokenLabelExpansion,
                         URITemplateTokenPathExpansion,
                         URITemplateTokenPathParameterExpansion,
                         URITemplateTokenQueryExpansion,
                         URITemplateTokenQueryContinuationExpansion,
                         URITemplateTokenVariable, URITemplateTokenLiteral>(
              remaining, offset, this->tokens_)) {
        break;
      }
    } catch (URITemplateParseError &error) {
      throw URITemplateParseError(offset + error.column());
    }
  }
}

auto URITemplate::size() const noexcept -> std::uint64_t {
  return static_cast<std::uint64_t>(this->tokens_.size());
}

auto URITemplate::empty() const noexcept -> bool {
  return this->tokens_.empty();
}

auto URITemplate::at(const std::size_t index) const & -> const
    URITemplateToken & {
  assert(index < this->tokens_.size());
  return this->tokens_[index];
}

auto URITemplate::at(const std::size_t index) && -> URITemplateToken {
  assert(index < this->tokens_.size());
  return std::move(this->tokens_[index]);
}

auto URITemplate::begin() const noexcept
    -> std::vector<URITemplateToken>::const_iterator {
  return this->tokens_.cbegin();
}

auto URITemplate::end() const noexcept
    -> std::vector<URITemplateToken>::const_iterator {
  return this->tokens_.cend();
}

auto URITemplate::expand(
    const std::function<URITemplateValue(std::string_view name)> &callback)
    const -> std::string {
  std::string result;

  for (const auto &token : this->tokens_) {
    std::visit(
        [&result, &callback](const auto &expansion) {
          using T = std::decay_t<decltype(expansion)>;
          if constexpr (std::is_same_v<T, URITemplateTokenLiteral>) {
            result += expansion.value;
          } else {
            expand_expression<T>(result, expansion.variables, callback);
          }
        },
        token);
  }

  return result;
}

template <typename T> auto is_matchable_token(const T &token) noexcept -> bool {
  return token.variables.size() == 1 && token.variables[0].length == 0 &&
         !token.variables[0].explode;
}

auto URITemplate::is_matchable(const char delimiter) const noexcept -> bool {
  bool previous_was_variable{false};

  for (std::size_t index = 0; index < this->tokens_.size(); ++index) {
    const auto &token{this->tokens_[index]};

    if (const auto *literal = std::get_if<URITemplateTokenLiteral>(&token)) {
      if (previous_was_variable && !literal->value.empty() &&
          literal->value.front() != delimiter) {
        return false;
      }

      previous_was_variable = false;
    } else if (const auto *variable =
                   std::get_if<URITemplateTokenVariable>(&token)) {
      if (previous_was_variable || !is_matchable_token(*variable)) {
        return false;
      }

      previous_was_variable = true;
    } else if (const auto *reserved =
                   std::get_if<URITemplateTokenReservedExpansion>(&token)) {
      if (index != this->tokens_.size() - 1 || previous_was_variable ||
          !is_matchable_token(*reserved)) {
        return false;
      }
    } else {
      return false;
    }
  }

  return true;
}

auto URITemplateTokenLiteral::match(const std::string_view uri,
                                    const std::size_t position,
                                    const char) const noexcept -> std::size_t {
  if (uri.substr(position).starts_with(this->value)) {
    return position + this->value.size();
  }

  return 0;
}

auto URITemplateTokenVariable::match(const std::string_view uri,
                                     const std::size_t position,
                                     const char delimiter) const noexcept
    -> std::size_t {
  const auto result{uri.find(delimiter, position)};
  const auto end{(result == std::string_view::npos) ? uri.size() : result};
  return (end == position) ? 0 : end;
}

auto URITemplateTokenReservedExpansion::match(const std::string_view uri,
                                              const std::size_t position,
                                              const char) const noexcept
    -> std::size_t {
  return (position == uri.size()) ? 0 : uri.size();
}

auto URITemplateTokenFragmentExpansion::match(const std::string_view,
                                              const std::size_t,
                                              const char) const noexcept
    -> std::size_t {
  return 0;
}

auto URITemplateTokenLabelExpansion::match(const std::string_view,
                                           const std::size_t,
                                           const char) const noexcept
    -> std::size_t {
  return 0;
}

auto URITemplateTokenPathExpansion::match(const std::string_view,
                                          const std::size_t,
                                          const char) const noexcept
    -> std::size_t {
  return 0;
}

auto URITemplateTokenPathParameterExpansion::match(const std::string_view,
                                                   const std::size_t,
                                                   const char) const noexcept
    -> std::size_t {
  return 0;
}

auto URITemplateTokenQueryExpansion::match(const std::string_view,
                                           const std::size_t,
                                           const char) const noexcept
    -> std::size_t {
  return 0;
}

auto URITemplateTokenQueryContinuationExpansion::match(
    const std::string_view, const std::size_t, const char) const noexcept
    -> std::size_t {
  return 0;
}

auto URITemplate::match(
    const std::string_view uri, const char delimiter,
    const std::function<void(std::string_view, std::string_view)> &callback)
    const -> bool {
  assert(this->is_matchable(delimiter));
  std::size_t position{0};
  for (const auto &token : this->tokens_) {
    if (!std::visit(
            [&](const auto &value) {
              const auto end{value.match(uri, position, delimiter)};
              if (end > position) {
                if constexpr (requires { value.variables; }) {
                  callback(value.variables[0].name,
                           uri.substr(position, end - position));
                }

                position = end;
                return true;
              } else {
                return false;
              }
            },
            token)) {
      return false;
    }
  }

  return position == uri.size();
}

} // namespace sourcemeta::core
