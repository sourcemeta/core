#ifndef SOURCEMETA_CORE_ERROR_STATIC_STRING_H_
#define SOURCEMETA_CORE_ERROR_STATIC_STRING_H_

#include <cstddef> // std::size_t

namespace sourcemeta::core {

// Declared and never defined. A consteval constructor is only ever evaluated
// at compile time, so reaching this call makes the invocation ill-formed and
// names the reason, while no call to it can ever be emitted
[[noreturn]] auto static_string_is_not_null_terminated() -> void;

/// @ingroup error
/// A null-terminated string that is known at compile time to outlive every
/// error that carries it.
///
/// ```cpp
/// #include <sourcemeta/core/error.h>
/// #include <cstring>
/// #include <cassert>
///
/// const sourcemeta::core::StaticString message{"Something went wrong"};
/// assert(std::strcmp(message.data(), "Something went wrong") == 0);
/// ```
class StaticString {
public:
  /// Construct from a string literal.
  ///
  /// ```cpp
  /// #include <sourcemeta/core/error.h>
  ///
  /// const sourcemeta::core::StaticString message{"Something went wrong"};
  /// ```
  template <std::size_t Length>
  // Binding to the array rather than to a pointer is what rejects a message
  // that does not outlive the error, and the implicit conversion is what lets
  // a throw site read as a bare literal
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions,modernize-avoid-c-arrays)
  consteval StaticString(const char (&value)[Length]) noexcept : value_{value} {
    if (value[Length - 1] != '\0') {
      static_string_is_not_null_terminated();
    }
  }

  /// Get the underlying null-terminated string.
  ///
  /// ```cpp
  /// #include <sourcemeta/core/error.h>
  /// #include <cstring>
  /// #include <cassert>
  ///
  /// const sourcemeta::core::StaticString message{"Something went wrong"};
  /// assert(std::strlen(message.data()) == 20);
  /// ```
  [[nodiscard]] constexpr auto data() const noexcept -> const char * {
    return this->value_;
  }

private:
  const char *value_;
};

} // namespace sourcemeta::core

#endif
