#ifndef SOURCEMETA_CORE_ALLOCATOR_H_
#define SOURCEMETA_CORE_ALLOCATOR_H_

#ifndef SOURCEMETA_CORE_ALLOCATOR_EXPORT
#include <sourcemeta/core/allocator_export.h>
#endif

#include <string_view> // std::string_view

/// @defgroup allocator Allocator
/// @brief The allocator that every program in this project runs on.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/allocator.h>
/// ```
///
/// Linking this library into a program is what puts its allocator in place, as
/// the standard allocation entry points it defines are the ones the program
/// already calls. No other library in this project links it, so a program that
/// does not ask for it keeps the allocator its platform ships with.

namespace sourcemeta::core {

/// @ingroup allocator
///
/// Report the allocator this program was linked against. For example:
///
/// ```cpp
/// #include <sourcemeta/core/allocator.h>
/// #include <cassert>
///
/// auto main() -> int {
///   assert(!sourcemeta::core::allocator_name().empty());
/// }
/// ```
SOURCEMETA_CORE_ALLOCATOR_EXPORT
auto allocator_name() -> std::string_view;

} // namespace sourcemeta::core

#endif
