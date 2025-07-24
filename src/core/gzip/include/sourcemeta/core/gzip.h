#ifndef SOURCEMETA_CORE_GZIP_H_
#define SOURCEMETA_CORE_GZIP_H_

#ifndef SOURCEMETA_CORE_GZIP_EXPORT
#include <sourcemeta/core/gzip_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/gzip_error.h>
// NOLINTEND(misc-include-cleaner)

#include <istream>     // std::istream
#include <string>      // std::string
#include <string_view> // std::string_view

/// @defgroup gzip GZIP
/// @brief A growing implementation of RFC 1952 GZIP.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/gzip.h>
/// ```

namespace sourcemeta::core {

/// @ingroup gzip
///
/// Compress an input string into a sequence of bytes represented using a
/// string. For example:
///
/// ```cpp
/// #include <sourcemeta/core/gzip.h>
/// #include <cassert>
///
/// const auto result{sourcemeta::core::gzip("Hello World")};
/// assert(!result.empty());
/// ```
SOURCEMETA_CORE_GZIP_EXPORT auto gzip(std::string_view input) -> std::string;

/// @ingroup gzip
///
/// Decompress an input stream into a sequence of bytes represented using a
/// string. For example:
///
/// ```cpp
/// #include <sourcemeta/core/gzip.h>
/// #include <cassert>
/// #include <sstream>
///
/// const auto input{sourcemeta::core::gzip("Hello World")};
/// std::istringstream stream{input};
/// const auto result{sourcemeta::core::gunzip(stream)};
/// assert(result == "Hello World");
/// ```
SOURCEMETA_CORE_GZIP_EXPORT auto gunzip(std::istream &stream) -> std::string;

} // namespace sourcemeta::core

#endif
