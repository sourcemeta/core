#ifndef SOURCEMETA_CORE_URITEMPLATE_ERROR_H_
#define SOURCEMETA_CORE_URITEMPLATE_ERROR_H_

#ifndef SOURCEMETA_CORE_URITEMPLATE_EXPORT
#include <sourcemeta/core/uritemplate_export.h>
#endif

#include <cstdint>   // std::uint64_t
#include <exception> // std::exception
#include <stdexcept> // std::runtime_error
#include <string>    // std::string

namespace sourcemeta::core {

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251 4275)
#endif

/// @ingroup uritemplate
/// An error that represents a URI Template parsing failure
class SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateParseError
    : public std::exception {
public:
  URITemplateParseError(const std::uint64_t column) : column_{column} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "The input is not a valid URI Template";
  }

  /// Get the column number of the error
  [[nodiscard]] auto column() const noexcept -> std::uint64_t {
    return this->column_;
  }

private:
  std::uint64_t column_;
};

/// @ingroup uritemplate
/// An error that represents a URI Template expansion failure
class SOURCEMETA_CORE_URITEMPLATE_EXPORT URITemplateExpansionError
    : public std::runtime_error {
public:
  URITemplateExpansionError(const std::string &message)
      : std::runtime_error{message} {}
};

#if defined(_MSC_VER)
#pragma warning(default : 4251 4275)
#endif

} // namespace sourcemeta::core

#endif
