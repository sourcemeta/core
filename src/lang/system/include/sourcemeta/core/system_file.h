#ifndef SOURCEMETA_CORE_SYSTEM_FILE_H_
#define SOURCEMETA_CORE_SYSTEM_FILE_H_

#ifndef SOURCEMETA_CORE_SYSTEM_EXPORT
#include <sourcemeta/core/system_export.h>
#endif

#include <filesystem> // std::filesystem::path

namespace sourcemeta::core {

/// @ingroup system
///
/// Flush an existing file to disk, beyond just to the operating system. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/system.h>
///
/// sourcemeta::core::flush("/foo/bar.txt");
/// ```
SOURCEMETA_CORE_SYSTEM_EXPORT
auto flush(const std::filesystem::path &path) -> void;

} // namespace sourcemeta::core

#endif
