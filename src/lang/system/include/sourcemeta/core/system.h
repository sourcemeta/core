#ifndef SOURCEMETA_CORE_SYSTEM_H_
#define SOURCEMETA_CORE_SYSTEM_H_

#ifndef SOURCEMETA_CORE_SYSTEM_EXPORT
#include <sourcemeta/core/system_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/system_atomic.h>
#include <sourcemeta/core/system_error.h>
#include <sourcemeta/core/system_file.h>
#include <sourcemeta/core/system_fileview.h>
#include <sourcemeta/core/system_parallel.h>
#include <sourcemeta/core/system_process.h>
#include <sourcemeta/core/system_stacktrace.h>
#include <sourcemeta/core/system_temporary.h>
#include <sourcemeta/core/system_usage.h>
// NOLINTEND(misc-include-cleaner)

/// @defgroup system System
/// @brief Operating system primitives, wrapping the system calls and
/// lower-level platform APIs that the rest of this project builds on
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/system.h>
/// ```

#endif
