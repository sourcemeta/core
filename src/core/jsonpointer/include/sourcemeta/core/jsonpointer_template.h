#ifndef SOURCEMETA_CORE_JSONPOINTER_TEMPLATE_H_
#define SOURCEMETA_CORE_JSONPOINTER_TEMPLATE_H_

#include <sourcemeta/core/jsonpointer_pointer.h>
#include <sourcemeta/core/jsonpointer_token.h>

#include <algorithm> // std::copy
#include <cassert>   // assert
#include <iterator>  // std::back_inserter
#include <variant>   // std::variant
#include <vector>    // std::vector

namespace sourcemeta::core {

/// @ingroup jsonpointer
template <typename PointerT> class GenericPointerTemplate {
public:
  /// The type of wildcard
  enum class Wildcard { Property, Item, Key };

  using Regex = typename PointerT::Value::String;
  using Token = typename PointerT::Token;
  using Container = std::vector<std::variant<Token, Wildcard, Regex>>;

  /// This constructor creates an empty JSON Pointer template. For example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpointer.h>
  ///
  /// const sourcemeta::core::PointerTemplate pointer;
  /// ```
  GenericPointerTemplate() : data{} {}

  /// This constructor creates a JSON Pointer template from an existing JSON
  /// Pointer. For example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpointer.h>
  ///
  /// const sourcemeta::core::Pointer base{"foo", "bar"};
  /// const sourcemeta::core::PointerTemplate pointer{base};
  /// ```
  GenericPointerTemplate(const PointerT &other) { this->push_back(other); }

  // Member types
  using value_type = typename Container::value_type;
  using allocator_type = typename Container::allocator_type;
  using size_type = typename Container::size_type;
  using difference_type = typename Container::difference_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using pointer = typename Container::pointer;
  using const_pointer = typename Container::const_pointer;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  using reverse_iterator = typename Container::reverse_iterator;
  using const_reverse_iterator = typename Container::const_reverse_iterator;

  /// Get a mutable begin iterator on the pointer
  auto begin() noexcept -> iterator { return this->data.begin(); }
  /// Get a mutable end iterator on the pointer
  auto end() noexcept -> iterator { return this->data.end(); }
  /// Get a constant begin iterator on the pointer
  auto begin() const noexcept -> const_iterator { return this->data.begin(); }
  /// Get a constant end iterator on the pointer
  auto end() const noexcept -> const_iterator { return this->data.end(); }
  /// Get a constant begin iterator on the pointer
  auto cbegin() const noexcept -> const_iterator { return this->data.cbegin(); }
  /// Get a constant end iterator on the pointer
  auto cend() const noexcept -> const_iterator { return this->data.cend(); }
  /// Get a mutable reverse begin iterator on the pointer
  auto rbegin() noexcept -> reverse_iterator { return this->data.rbegin(); }
  /// Get a mutable reverse end iterator on the pointer
  auto rend() noexcept -> reverse_iterator { return this->data.rend(); }
  /// Get a constant reverse begin iterator on the pointer
  auto rbegin() const noexcept -> const_reverse_iterator {
    return this->data.rbegin();
  }
  /// Get a constant reverse end iterator on the pointer
  auto rend() const noexcept -> const_reverse_iterator {
    return this->data.rend();
  }
  /// Get a constant reverse begin iterator on the pointer
  auto crbegin() const noexcept -> const_reverse_iterator {
    return this->data.crbegin();
  }
  /// Get a constant reverse end iterator on the pointer
  auto crend() const noexcept -> const_reverse_iterator {
    return this->data.crend();
  }

  /// Emplace a token or wildcard into the back of a JSON Pointer template. For
  /// example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpointer.h>
  ///
  /// sourcemeta::core::PointerTemplate pointer;
  /// pointer.emplace_back(sourcemeta::core::PointerTemplate::Wildcard::Property);
  /// ```
  template <class... Args> auto emplace_back(Args &&...args) -> reference {
    // It is a logical error to push a token after a key wildcard
    assert(this->data.empty() ||
           !std::holds_alternative<Wildcard>(this->data.back()) ||
           std::get<Wildcard>(this->data.back()) != Wildcard::Key);
    return this->data.emplace_back(args...);
  }

  /// Push a copy of a JSON Pointer into the back of a JSON Pointer template.
  /// For example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpointer.h>
  ///
  /// sourcemeta::core::PointerTemplate result;
  /// const sourcemeta::core::Pointer pointer{"bar", "baz"};
  /// result.push_back(pointer);
  /// ```
  auto push_back(const PointerT &other) -> void {
    // It is a logical error to push a token after a key wildcard
    assert(this->data.empty() ||
           !std::holds_alternative<Wildcard>(this->data.back()) ||
           std::get<Wildcard>(this->data.back()) != Wildcard::Key);
    this->data.reserve(this->data.size() + other.size());
    std::copy(other.cbegin(), other.cend(), std::back_inserter(this->data));
  }

  /// Remove the last token of a JSON Pointer template. For example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpointer.h>
  ///
  /// const sourcemeta::core::Pointer base{"bar", "baz"};
  /// sourcemeta::core::PointerTemplate pointer{base};
  /// pointer.pop_back();
  /// ```
  auto pop_back() -> void {
    assert(!this->data.empty());
    this->data.pop_back();
  }

  /// Compare JSON Pointer template instances
  auto operator==(const GenericPointerTemplate<PointerT> &other) const noexcept
      -> bool {
    return this->data == other.data;
  }

private:
  Container data;
};

} // namespace sourcemeta::core

#endif
