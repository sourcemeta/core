#ifndef SOURCEMETA_CORE_URITEMPLATE_H_
#define SOURCEMETA_CORE_URITEMPLATE_H_

#ifndef SOURCEMETA_CORE_URITEMPLATE_EXPORT
#include <sourcemeta/core/uritemplate_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/uritemplate_error.h>
// NOLINTEND(misc-include-cleaner)

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t, std::uint16_t
#include <functional>  // std::function
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <tuple>       // std::tuple
#include <type_traits> // std::void_t
#include <variant>     // std::variant
#include <vector>      // std::vector

/// @defgroup uritemplate URI Template
/// @brief A strict RFC 6570 URI Template implementation.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/uritemplate.h>
/// ```

namespace sourcemeta::core {

/// @ingroup uritemplate
/// The return type for URI Template variable callbacks (value, key?, has_more)
using URITemplateValue = std::optional<
    std::tuple<std::string_view, std::optional<std::string_view>, bool>>;

#ifndef DOXYGEN
struct URITemplateTokenLiteral;
struct URITemplateTokenVariable;
struct URITemplateTokenReservedExpansion;
struct URITemplateTokenFragmentExpansion;
struct URITemplateTokenLabelExpansion;
struct URITemplateTokenPathExpansion;
struct URITemplateTokenPathParameterExpansion;
struct URITemplateTokenQueryExpansion;
struct URITemplateTokenQueryContinuationExpansion;
#endif

/// @ingroup uritemplate
/// A token in a parsed URI Template
using URITemplateToken = std::variant<
    URITemplateTokenLiteral, URITemplateTokenVariable,
    URITemplateTokenReservedExpansion, URITemplateTokenFragmentExpansion,
    URITemplateTokenLabelExpansion, URITemplateTokenPathExpansion,
    URITemplateTokenPathParameterExpansion, URITemplateTokenQueryExpansion,
    URITemplateTokenQueryContinuationExpansion>;

/// @ingroup uritemplate
/// The result of parsing a token: the token and how many characters were
/// consumed
using URITemplateParseResult =
    std::optional<std::pair<URITemplateToken, std::size_t>>;

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

/// @ingroup uritemplate
/// A literal string segment in a URI Template
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateTokenLiteral {
  std::string_view value;
};

/// @ingroup uritemplate
/// A variable specification within a URI Template expression
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateVariableSpecification {
  std::string_view name;
  // As per the RFC, the range is 1-9999. 0 means "no prefix length"
  std::uint16_t length{0};
  bool explode{false};
};

/// @ingroup uritemplate
/// A simple string variable expansion {var} in a URI Template (Level 1)
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateTokenVariable {
  std::vector<URITemplateVariableSpecification> variables;
  static constexpr char separator = ',';
  static constexpr bool named = false;
  static constexpr bool allow_reserved = false;
};

/// @ingroup uritemplate
/// A reserved expansion {+var} in a URI Template (Level 2)
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateTokenReservedExpansion {
  std::vector<URITemplateVariableSpecification> variables;
  static constexpr char op = '+';
  static constexpr char separator = ',';
  static constexpr bool named = false;
  static constexpr bool allow_reserved = true;
};

/// @ingroup uritemplate
/// A fragment expansion {#var} in a URI Template (Level 2)
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateTokenFragmentExpansion {
  std::vector<URITemplateVariableSpecification> variables;
  static constexpr char op = '#';
  static constexpr char separator = ',';
  static constexpr char prefix = '#';
  static constexpr bool named = false;
  static constexpr bool allow_reserved = true;
};

/// @ingroup uritemplate
/// A label expansion {.var} in a URI Template (Level 3)
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateTokenLabelExpansion {
  std::vector<URITemplateVariableSpecification> variables;
  static constexpr char op = '.';
  static constexpr char separator = '.';
  static constexpr char prefix = '.';
  static constexpr bool named = false;
  static constexpr bool allow_reserved = false;
};

/// @ingroup uritemplate
/// A path expansion {/var} in a URI Template (Level 3)
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateTokenPathExpansion {
  std::vector<URITemplateVariableSpecification> variables;
  static constexpr char op = '/';
  static constexpr char separator = '/';
  static constexpr char prefix = '/';
  static constexpr bool named = false;
  static constexpr bool allow_reserved = false;
};

/// @ingroup uritemplate
/// A path parameter expansion {;var} in a URI Template (Level 3)
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT
    URITemplateTokenPathParameterExpansion {
  std::vector<URITemplateVariableSpecification> variables;
  static constexpr char op = ';';
  static constexpr char separator = ';';
  static constexpr char prefix = ';';
  static constexpr bool named = true;
  static constexpr bool allow_reserved = false;
};

/// @ingroup uritemplate
/// A query expansion {?var} in a URI Template (Level 3)
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateTokenQueryExpansion {
  std::vector<URITemplateVariableSpecification> variables;
  static constexpr char op = '?';
  static constexpr char separator = '&';
  static constexpr char prefix = '?';
  static constexpr bool named = true;
  static constexpr bool allow_reserved = false;
  static constexpr char empty_suffix = '=';
};

/// @ingroup uritemplate
/// A query continuation expansion {&var} in a URI Template (Level 3)
struct SOURCEMETA_CORE_URITEMPLATE_EXPORT
    URITemplateTokenQueryContinuationExpansion {
  std::vector<URITemplateVariableSpecification> variables;
  static constexpr char op = '&';
  static constexpr char separator = '&';
  static constexpr char prefix = '&';
  static constexpr bool named = true;
  static constexpr bool allow_reserved = false;
  static constexpr char empty_suffix = '=';
};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

/// @ingroup uritemplate
/// A parsed URI Template per RFC 6570. This class behaves like a view. The
/// source string must outlive the template
class SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplate {
public:
  /// Parse a URI Template from a string view. For example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/uritemplate.h>
  ///
  /// const std::string source{"http://example.com/~{username}/"};
  /// const sourcemeta::core::URITemplate uri_template{source};
  /// ```
  URITemplate(std::string_view source);

  /// Get the number of tokens in the template
  [[nodiscard]] auto size() const noexcept -> std::uint64_t;

  /// Check if the template is empty
  [[nodiscard]] auto empty() const noexcept -> bool;

  /// Get the token at the given index
  [[nodiscard]] auto at(std::size_t index) const -> const URITemplateToken &;

  /// Iterator to the beginning of the tokens
  [[nodiscard]] auto begin() const noexcept
      -> std::vector<URITemplateToken>::const_iterator;

  /// Iterator to the end of the tokens
  [[nodiscard]] auto end() const noexcept
      -> std::vector<URITemplateToken>::const_iterator;

  /// Expand the template by looking up variable values via a callback.
  /// The callback is called repeatedly for composite values
  [[nodiscard]] auto expand(
      const std::function<URITemplateValue(std::string_view)> &callback) const
      -> std::string;

  /// Expand the template using an associative container (string values only)
  template <typename Container,
            typename = std::void_t<typename Container::key_type>>
  [[nodiscard]] auto expand(const Container &variables) const -> std::string {
    return this->expand([&variables](
                            const std::string_view name) -> URITemplateValue {
      const auto iterator{variables.find(typename Container::key_type{name})};
      if (iterator == variables.end()) {
        return std::nullopt;
      } else {
        return std::make_tuple(std::string_view{iterator->second}, std::nullopt,
                               false);
      }
    });
  }

private:
// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif
  std::vector<URITemplateToken> tokens_;
#if defined(_MSC_VER)
#pragma warning(default : 4251)
#endif
};

} // namespace sourcemeta::core

#endif
