#ifndef SOURCEMETA_CORE_UNICODE_H_
#define SOURCEMETA_CORE_UNICODE_H_

#ifndef SOURCEMETA_CORE_UNICODE_EXPORT
#include <sourcemeta/core/unicode_export.h>
#endif

#include <ostream> // std::ostream
#include <string>  // std::string

/// @defgroup unicode Unicode
/// @brief Unicode encoding utilities.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// ```

namespace sourcemeta::core {

/// @ingroup unicode
/// Encode a single Unicode codepoint as a UTF-8 string. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::codepoint_to_utf8(0x41) == "A");
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto codepoint_to_utf8(const char32_t codepoint) -> std::string;

/// @ingroup unicode
/// Encode a single Unicode codepoint as UTF-8 into an output stream.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <sstream>
/// #include <cassert>
///
/// std::ostringstream output;
/// sourcemeta::core::codepoint_to_utf8(0x41, output);
/// assert(output.str() == "A");
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto codepoint_to_utf8(const char32_t codepoint, std::ostream &output) -> void;

} // namespace sourcemeta::core

#endif
