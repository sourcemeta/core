#ifndef SOURCEMETA_CORE_URLPATTERN_ERROR_H_
#define SOURCEMETA_CORE_URLPATTERN_ERROR_H_

#ifndef SOURCEMETA_CORE_URLPATTERN_EXPORT
#include <sourcemeta/core/urlpattern_export.h>
#endif

#include <cstdint>   // std::uint64_t
#include <exception> // std::exception

namespace sourcemeta::core {

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251 4275)
#endif

/// @ingroup urlpattern
/// An error that represents a URL Pattern failure
class SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternParseError
    : public std::exception {
public:
  URLPatternParseError(const std::uint64_t column) : column_{column} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "The input is not a valid URL Pattern";
  }

  /// Get the column number of the error
  [[nodiscard]] auto column() const noexcept -> std::uint64_t {
    return column_;
  }

private:
  std::uint64_t column_;
};

#if defined(_MSC_VER)
#pragma warning(default : 4251 4275)
#endif

} // namespace sourcemeta::core

#endif
