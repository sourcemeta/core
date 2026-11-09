#include <sourcemeta/core/json_packed.h>

#include "construct.h"

#include <bit>       // std::bit_cast
#include <cstddef>   // std::size_t, std::max_align_t
#include <cstdint>   // std::uint32_t, std::uint64_t, std::uint8_t
#include <limits>    // std::numeric_limits
#include <new>       // std::bad_alloc, std::bad_array_new_length
#include <stdexcept> // std::logic_error
#include <utility>   // std::move
#include <vector>    // std::vector

#if defined(__linux__)
#include <sys/mman.h>
#endif

namespace sourcemeta::core {

namespace {

#if defined(__linux__)
// Map one contiguous region per tape allocation. Vector capacity survives
// resets, so steady-state parses can reuse the mapping without a syscall.
template <typename T> class TapeAllocator {
public:
  using value_type = T;
  TapeAllocator() noexcept = default;
  template <typename U> TapeAllocator(const TapeAllocator<U> &) noexcept {}

  [[nodiscard]] T *allocate(std::size_t count) {
    static_assert(alignof(T) <= alignof(std::max_align_t));
    if (count == 0)
      return nullptr;
    if (count > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      throw std::bad_array_new_length{};
    }
    void *address = mmap(nullptr, count * sizeof(T), PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (address == MAP_FAILED)
      throw std::bad_alloc{};
    return static_cast<T *>(address);
  }

  void deallocate(T *address, std::size_t count) noexcept {
    if (address)
      munmap(address, count * sizeof(T));
  }

  template <typename U>
  bool operator==(const TapeAllocator<U> &) const noexcept {
    return true;
  }
};
#else
template <typename T> using TapeAllocator = std::allocator<T>;
#endif

} // namespace

struct JSONPacked::Storage {
  using Token = TapeEntry;
  using Kind = TapeType;
  std::string input_;
  std::vector<Token, TapeAllocator<Token>> tape_;
  std::string decoded_;
  std::vector<JSON> decimals_;

  auto string_at(const Token &token) const -> std::string_view {
    const auto &storage = token.flags ? decoded_ : input_;
    return std::string_view{storage}.substr(token.offset, token.length);
  }

  auto value_at(const Token &token) const -> JSON {
    switch (token.type) {
      case Kind::ObjectStart:
        return JSON::make_object();
      case Kind::ArrayStart:
        return JSON::make_array();
      case Kind::String:
        return JSON{string_at(token)};
      case Kind::True:
        return JSON{true};
      case Kind::False:
        return JSON{false};
      case Kind::Null:
        return JSON{nullptr};
      case Kind::Number:
        switch (static_cast<JSON::Type>(token.flags)) {
          case JSON::Type::Integer:
            return JSON{std::bit_cast<JSON::Integer>(token.column)};
          case JSON::Type::Real:
            return JSON{std::bit_cast<JSON::Real>(token.column)};
          case JSON::Type::Decimal:
            return decimals_.at(token.column);
          default:
            break;
        }
        break;
      default:
        break;
    }
    throw std::logic_error("invalid packed value");
  }

  auto reset() noexcept -> void {
    decimals_.clear();
    decoded_.clear();
    tape_.clear();
    input_.clear();
  }

  auto parse(std::string input) -> void {
    reset();
    if (input.empty() ||
        input.size() > std::numeric_limits<std::uint32_t>::max()) {
      throw JSONParseError(1, 0);
    }
    input_ = std::move(input);
    try {
      const char *const begin = input_.data();
      const char *const end = begin + input_.size();
      const char *cursor = begin;
      std::uint64_t line = 1, column = 0;
      tape_.reserve(input_.size() / 8);
      scan_json<false>(cursor, end, begin, line, column, tape_);
      internal::skip_whitespace<false>(cursor, end, line, column);
      if (cursor != end)
        throw JSONParseError(1, 0);

      // Positions are unnecessary after scanning. Use the space already
      // present in a token for a subtree boundary and numeric payload.
      auto parent = tape_.size();
      for (std::size_t index = 0; index < tape_.size(); ++index) {
        auto &token = tape_[index];
        switch (token.type) {
          case Kind::ArrayStart:
          case Kind::ObjectStart:
            token.column = parent;
            parent = index;
            break;
          case Kind::ArrayEnd:
          case Kind::ObjectEnd: {
            auto &open = tape_[parent];
            open.line = index + 1;
            parent = open.column;
            break;
          }
          case Kind::String:
          case Kind::Key:
            if (token.flags & TAPE_FLAG_STRING_ESCAPE) {
              const auto value = internal::unescape_string(begin + token.offset,
                                                           token.length, true);
              token.offset = static_cast<std::uint32_t>(decoded_.size());
              token.length = static_cast<std::uint32_t>(value.size());
              decoded_.append(value);
            }
            break;
          case Kind::Number: {
            auto value = internal::construct_leaf(begin, token);
            token.flags = static_cast<std::uint8_t>(value.type());
            if (value.is_integer()) {
              token.column = std::bit_cast<std::uint64_t>(value.as_integer());
            } else if (value.is_real()) {
              token.column = std::bit_cast<std::uint64_t>(value.as_real());
            } else {
              token.column = decimals_.size();
              decimals_.push_back(std::move(value));
            }
            break;
          }
          default:
            break;
        }
      }
    } catch (...) {
      reset();
      throw;
    }
  }

  auto contains_root_key(std::string_view key) const -> bool {
    if (tape_.empty() || tape_.front().type != Kind::ObjectStart)
      return false;
    for (std::size_t index = 1; index + 1 < tape_.size();) {
      if (string_at(tape_[index]) == key)
        return true;
      const auto &value = tape_[++index];
      index = value.type == Kind::ObjectStart || value.type == Kind::ArrayStart
                  ? value.line
                  : index + 1;
    }
    return false;
  }

  // Convert only when a caller needs Core's independently mutable JSON value.
  auto materialize() const -> JSON {
    if (tape_.empty())
      throw std::logic_error("no packed document");
    auto result = value_at(tape_.front());
    if (!result.is_array() && !result.is_object())
      return result;

    if (result.is_array())
      result.as_array().reserve(tape_.front().count);
    else
      result.as_object().reserve(tape_.front().count);

    std::vector<JSON *> parents{&result};
    for (std::size_t index = 1; index < tape_.size(); ++index) {
      auto &parent = *parents.back();
      const auto *token = &tape_[index];
      if (token->type == Kind::ArrayEnd || token->type == Kind::ObjectEnd) {
        parents.pop_back();
        continue;
      }

      JSON *child;
      if (parent.is_object()) {
        JSON::String key{string_at(*token)};
        token = &tape_[++index];
        const auto hash = JSON::Object::hash(key);
        child =
            &parent.assign_assume_new(std::move(key), value_at(*token), hash);
      } else {
        parent.push_back(value_at(*token));
        child = &parent.back();
      }

      if (child->is_array()) {
        child->as_array().reserve(token->count);
        parents.push_back(child);
      } else if (child->is_object()) {
        child->as_object().reserve(token->count);
        parents.push_back(child);
      }
    }
    return result;
  }
};

JSONPacked::JSONPacked() : storage_{std::make_unique<Storage>()} {}
JSONPacked::~JSONPacked() = default;

auto JSONPacked::parse(std::string input) -> void {
  this->storage_->parse(std::move(input));
}

auto JSONPacked::reset() noexcept -> void { this->storage_->reset(); }

auto JSONPacked::contains_root_key(std::string_view key) const -> bool {
  return this->storage_->contains_root_key(key);
}

auto JSONPacked::materialize() const -> JSON {
  return this->storage_->materialize();
}

} // namespace sourcemeta::core
