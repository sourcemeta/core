#ifndef SOURCEMETA_CORE_JSONLD_ERROR_H_
#define SOURCEMETA_CORE_JSONLD_ERROR_H_

#ifndef SOURCEMETA_CORE_JSONLD_EXPORT
#include <sourcemeta/core/jsonld_export.h>
#endif

#include <exception>   // std::exception
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251 4275)
#endif

/// @ingroup jsonld
/// An error that represents a JSON-LD processing failure. The message is one of
/// the error codes defined by the JSON-LD 1.1 API specification, such as
/// `invalid @id value` or `cyclic IRI mapping`
class SOURCEMETA_CORE_JSONLD_EXPORT JSONLDError : public std::exception {
public:
  JSONLDError(const char *code) : code_{code} {}
  JSONLDError(std::string code) = delete;
  JSONLDError(std::string &&code) = delete;
  JSONLDError(std::string_view code) = delete;

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->code_;
  }

  /// Get the JSON-LD error code
  [[nodiscard]] auto code() const noexcept -> const char * {
    return this->code_;
  }

private:
  const char *code_;
};

#if defined(_MSC_VER)
#pragma warning(default : 4251 4275)
#endif

} // namespace sourcemeta::core

#endif
