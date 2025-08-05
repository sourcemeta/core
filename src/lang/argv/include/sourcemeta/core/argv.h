#ifndef SOURCEMETA_CORE_ARGV_H_
#define SOURCEMETA_CORE_ARGV_H_

#ifndef SOURCEMETA_CORE_ARGV_EXPORT
#include <sourcemeta/core/argv_export.h>
#endif

#include <filesystem> // std::filesystem

/// @defgroup argv ARGV
/// @brief Interact with program arguments
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/argv.h>
/// ```

namespace sourcemeta::core {

/// @ingroup argv
///
/// Get a reference to the current program executable, in its absolute form if
/// possible. For example:
///
/// ```cpp
/// #include <sourcemeta/core/argv.h>
/// #include <iostream>
///
/// int main(int argc, char *argv[]) {
///   const auto &program{sourcemeta::core::program_path(argc, argv)};
///   std::cerr << program.string() << "\n";
///   return 0;
/// }
/// ```
SOURCEMETA_CORE_ARGV_EXPORT auto
program_path(const int argc,
             // Because we want to be able to take `argc` and `argv` directly
             // NOLINTNEXTLINE(modernize-avoid-c-arrays)
             const char *const argv[]) -> const std::filesystem::path &;

} // namespace sourcemeta::core

#endif
