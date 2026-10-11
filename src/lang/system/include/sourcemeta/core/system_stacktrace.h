#ifndef SOURCEMETA_CORE_SYSTEM_STACKTRACE_H_
#define SOURCEMETA_CORE_SYSTEM_STACKTRACE_H_

#ifndef SOURCEMETA_CORE_SYSTEM_EXPORT
#include <sourcemeta/core/system_export.h>
#endif

namespace sourcemeta::core {

/// @ingroup system
///
/// Install a process-wide handler that prints a stack trace on fatal signals.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/system.h>
///
/// auto main() -> int {
///   sourcemeta::core::stacktrace_on_crash();
///   // ... rest of the program
/// }
/// ```
SOURCEMETA_CORE_SYSTEM_EXPORT
auto stacktrace_on_crash() -> void;

/// @ingroup system
///
/// Print the current backtrace. Safe to call from any thread, but not from a
/// signal handler. For example:
///
/// ```cpp
/// #include <sourcemeta/core/system.h>
///
/// sourcemeta::core::stacktrace();
/// ```
SOURCEMETA_CORE_SYSTEM_EXPORT
auto stacktrace() -> void;

} // namespace sourcemeta::core

#endif
